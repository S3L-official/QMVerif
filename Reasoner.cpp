//
// Created by Pengfei Gao on 2018/8/17.
//
#include "z3++.h"
#include "Reasoner.h"
#include "util/SetUtil.h"
#include "SMT2Parse.h"
#include "TypeSystem.h"
#include "util/NodeUtil.h"
#include "util/VectorUtil.h"
#include "util/FileUtil.h"
#include "Counting.h"

void Reasoner::reasonForOneOrderQMS() {
    initWidth(bitLength);

    clock_t c1 = clock();
    int rud = 0;
    int sid = 0;
    int npm = 0;
    int smtcheck = 0;
    double qmsTime = 0.0;
    double sum = 0.0;
    double min = 1.0;
    result += "Intermediate: " + to_string(interV.size()) + "\n";
    for(int i = 0; i < interV.size(); i++) {
        cout << interV.size() << endl;
        cout << i << endl;

        Node* node = nodeMap[interV[i]];
        cout << node->getName() << endl;

        clock_t syn = clock();

        TypeSystem::typeInfer(node, TypeInferenceSort::SYN, secret);


        clock_t sem = clock();
        cout << "syn time: " << (sem - syn) / 1000 << endl;
        if(node->getDistributionEnums() == DistributionEnums::UKD) {
            Node* tempNode = new Node();
            NodeUtil::nodeCopy(tempNode, node);
            clock_t copytime = clock();
            cout <<"copy time: " << (copytime - sem) / 1000 << endl;
            int interCount = 0;
            NodeUtil::countInterNodes(node, interCount);
            cout <<"Node size: " << interCount << endl;

            bool hasGotRegion = false;

            doninatedElimitForOneNode(tempNode, hasGotRegion);

            clock_t sem3 = clock();
            cout << "dom ele  time: " << (sem3 - copytime) / 1000 << endl;

            if(tempNode->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
                if (tempNode->getLeftChild()->getSemd() == NULL) {
                    if(!disableSimplify) {
                        getSimplifiedAndSetSemd(node->getLeftChild(), tempNode->getLeftChild(), hasGotRegion);
                    } else {
                        shared_ptr<set<string>> semd = make_shared<set<string>>();
                        Node::getRandSet(*(tempNode->getLeftChild()), *(semd.get()));
                        tempNode->getLeftChild()->setSemd(semd);
                    }

                }

                if (tempNode->getRightChild() != NULL && tempNode->getRightChild()->getSemd() == NULL) {
                    if(!disableSimplify) {
                        getSimplifiedAndSetSemd(node->getRightChild(), tempNode->getRightChild(), hasGotRegion);
                    } else {
                        shared_ptr<set<string>> semd = make_shared<set<string>>();
                        Node::getRandSet(*(tempNode->getRightChild()), *(semd.get()));
                        tempNode->getRightChild()->setSemd(semd);
                    }
                }
            }


            TypeSystem::typeInfer(tempNode, TypeInferenceSort::SEM, secret);

            clock_t sem2 = clock();
            cout << "sem time: " << (sem2 - sem) / 1000 << endl;

            if(tempNode->getDistributionEnums() == DistributionEnums::UKD) {

                if(!disableSimplify ) {
                    getSimplifiedAndSetSemd(node, tempNode, hasGotRegion);
                    if (tempNode->getDistributionEnums() != DistributionEnums::UKD)
                        tempNode->setDistributionEnums(DistributionEnums::UKD);

                    NodeUtil::emptySupport(tempNode);
                    NodeUtil::getAuxiliaryTable(tempNode);
//                    TypeSystem::typeInfer(tempNode, TypeInferenceSort::SEM, secret);
                }
                if(tempNode->getDistributionEnums() == DistributionEnums::UKD) {
                    node->setDistributionEnums(getDistributionbyCounting(tempNode));

                    smtcheck++;
                    assert(node->getDistributionEnums() == DistributionEnums::NPM ||
                           node->getDistributionEnums() == DistributionEnums::ISD ||
                            node->getDistributionEnums() == DistributionEnums::RUD);

                    if(node->getDistributionEnums() == DistributionEnums::NPM) {
                        double step = 1.0;
                        double qms = 1.0;
                        clock_t t1 = clock();

                        set<string> rand;
                        Node::getRandSet(*tempNode, rand);
                        if(rand.size() == 0)
                            qms = 0;
                        else {
                            qms = getQMSbyCounting(tempNode);
                            qms = 1 - qms;

                            clock_t t2 = clock();
                            qmsTime = t2 - t1 + qmsTime;

                        }

                        if(min > qms)
                            min = qms;
                        sum = sum + qms;
                        node->setQMS(qms);

                    } else {
                        sum += 1.0;
                        node->setQMS(1.0);
                    }




                } else {
                    sum += 1.0;
                    node->setDistributionEnums(tempNode->getDistributionEnums());
                    node->setQMS(1.0);
                }

            } else {
                sum += 1.0;
                node->setDistributionEnums(tempNode->getDistributionEnums());
                node->setQMS(1.0);
            }


            clock_t smt = clock();
            cout << "smt time: " << (smt - sem2) / 1000 << endl;
            NodeUtil::clear(tempNode);
        } else {
            if(node->getDistributionEnums() == DistributionEnums::NPM)
                node->setQMS(1 - getQMSbyCounting(node));
            else
                node->setQMS(1.0);
        }

        switch (node->getDistributionEnums()) {
            case DistributionEnums ::RUD:
                rud++;
                break;
            case DistributionEnums ::ISD:
                sid++;
                break;
            case DistributionEnums ::NPM:
                npm++;
                break;
            default:
                assert(false);
        }


        result += "Node: " + node->getName() + ", Type: " + EnumUtil::distributionToString(node->getDistributionEnums()) + ", QMS: " + to_string(node->getQMS()) + "\n";
        cout << "Node: " + node->getName() + ", Type: " + EnumUtil::distributionToString(node->getDistributionEnums()) + "\n";
        if(node->getDistributionEnums() == DistributionEnums::NPM) {
            cout << "haha" << endl;
        }
    }

    clock_t c11 = clock();
    result += "time " + std::to_string((c11 - c1) / 1000) + "ms\n";
    result += "time for QMS: " + std::to_string(qmsTime / 1000) + "ms \n";
    result += "Node SMT check count: " + to_string(smtcheck) + "\n";
    result += "RUD: " + to_string(rud) + "\n";
    result += "SID: " + to_string(sid) + "\n";
    result += "NPM: " + to_string(npm) + "\n";


}

void Reasoner::reasonForOneOrderQMSZ3() {

    initWidth(bitLength);
    clock_t c1 = clock();
    int rud = 0;
    int sid = 0;
    int npm = 0;
    int smtcheck = 0;
    double qmsTime = 0.0;
    double sum = 0.0;
    double min = 1.0;
    result += "Intermediate: " + to_string(interV.size()) + "\n";
    for(int i = 0; i < interV.size(); i++) {
        cout << interV.size() << endl;
        cout << i << endl;

        Node* node = nodeMap[interV[i]];
        cout << node->getName() << endl;

        clock_t syn = clock();

        TypeSystem::typeInfer(node, TypeInferenceSort::SYN, secret);

        clock_t sem = clock();
        cout << "syn time: " << (sem - syn) / 1000 << endl;
        if(node->getDistributionEnums() == DistributionEnums::UKD) {
            Node* tempNode = new Node();
            NodeUtil::nodeCopy(tempNode, node);
            clock_t copytime = clock();
            cout <<"copy time: " << (copytime - sem) / 1000 << endl;
            int interCount = 0;
            NodeUtil::countInterNodes(node, interCount);
            cout <<"Node size: " << interCount << endl;

            bool hasGotRegion = false;

            doninatedElimitForOneNode(tempNode, hasGotRegion);

            clock_t sem3 = clock();
            cout << "dom ele  time: " << (sem3 - copytime) / 1000 << endl;

            if(tempNode->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
                if (tempNode->getLeftChild()->getSemd() == NULL) {
                    if(!disableSimplify) {
                        getSimplifiedAndSetSemd(node->getLeftChild(), tempNode->getLeftChild(), hasGotRegion);
                    } else {
                        shared_ptr<set<string>> semd = make_shared<set<string>>();
                        Node::getRandSet(*(tempNode->getLeftChild()), *(semd.get()));
                        tempNode->getLeftChild()->setSemd(semd);
                    }

                }

                if (tempNode->getRightChild() != NULL && tempNode->getRightChild()->getSemd() == NULL) {
                    if(!disableSimplify) {
                        getSimplifiedAndSetSemd(node->getRightChild(), tempNode->getRightChild(), hasGotRegion);
                    } else {
                        shared_ptr<set<string>> semd = make_shared<set<string>>();
                        Node::getRandSet(*(tempNode->getRightChild()), *(semd.get()));
                        tempNode->getRightChild()->setSemd(semd);
                    }
                }
            }


            TypeSystem::typeInfer(tempNode, TypeInferenceSort::SEM, secret);

            clock_t sem2 = clock();
            cout << "sem time: " << (sem2 - sem) / 1000 << endl;

            if(tempNode->getDistributionEnums() == DistributionEnums::UKD) {

                if(!disableSimplify ) {
                    getSimplifiedAndSetSemd(node, tempNode, hasGotRegion);
                    if (tempNode->getDistributionEnums() != DistributionEnums::UKD)
                        tempNode->setDistributionEnums(DistributionEnums::UKD);

                    NodeUtil::emptySupport(tempNode);
                    NodeUtil::getAuxiliaryTable(tempNode);
//                    TypeSystem::typeInfer(tempNode, TypeInferenceSort::SEM, secret);
                }
                if(tempNode->getDistributionEnums() == DistributionEnums::UKD) {
                    cout << "get Distribution by counting....." << endl;
                    node->setDistributionEnums(getDistributionbyCounting(tempNode));
                    cout << node->getName() << ": " << EnumUtil::distributionToString(node->getDistributionEnums()) << endl;

                    smtcheck++;
                    assert(node->getDistributionEnums() == DistributionEnums::NPM ||
                           node->getDistributionEnums() == DistributionEnums::ISD ||
                            node->getDistributionEnums() == DistributionEnums::RUD);

                    if(node->getDistributionEnums() == DistributionEnums::NPM) {
                        double step = 1.0;
                        double qms = 1.0;
                        clock_t t1 = clock();

                        set<string> rand;
                        Node::getRandSet(*tempNode, rand);
                        if(rand.size() == 0)
                            qms = 0;
                        else {
                            cout << "compute qms:...." << endl;
                            qms = SMT2Parse::getQMSbyZ3(*tempNode, 8);

                            clock_t t2 = clock();
                            qmsTime = t2 - t1 + qmsTime;

                        }

                        if(min > qms)
                            min = qms;
                        sum = sum + qms;
                        node->setQMS(qms);

                    } else {
                        sum += 1.0;
                        node->setQMS(1.0);
                    }




                } else {
                    sum += 1.0;
                    node->setDistributionEnums(tempNode->getDistributionEnums());
                    node->setQMS(1.0);
                }

            } else {
                sum += 1.0;
                node->setDistributionEnums(tempNode->getDistributionEnums());
                node->setQMS(1.0);
            }


            clock_t smt = clock();
            cout << "smt time: " << (smt - sem2) / 1000 << endl;
            NodeUtil::clear(tempNode);
        } else {
            if(node->getDistributionEnums() == DistributionEnums::NPM)
                node->setQMS(1 - getQMSbyCounting(node));
            else
                node->setQMS(1.0);
        }

        switch (node->getDistributionEnums()) {
            case DistributionEnums ::RUD:
                rud++;
                break;
            case DistributionEnums ::ISD:
                sid++;
                break;
            case DistributionEnums ::NPM:
                npm++;
                break;
            default:
                assert(false);
        }


        result += "Node: " + node->getName() + ", Type: " + EnumUtil::distributionToString(node->getDistributionEnums()) + ", QMS: " + to_string(node->getQMS()) + "\n";
        cout << "Node: " + node->getName() + ", Type: " + EnumUtil::distributionToString(node->getDistributionEnums()) + "\n";
        if(node->getDistributionEnums() == DistributionEnums::NPM) {
            cout << "haha" << endl;
        }
    }

    clock_t c11 = clock();
    result += "time " + std::to_string((c11 - c1) / 1000) + "ms\n";
    result += "time for QMS: " + std::to_string(qmsTime / 1000) + "ms \n";
    result += "Node SMT check count: " + to_string(smtcheck) + "\n";
    result += "RUD: " + to_string(rud) + "\n";
    result += "SID: " + to_string(sid) + "\n";
    result += "NPM: " + to_string(npm) + "\n";

}

double Reasoner::getQMSbyCounting(Node* tempNode) {
    string exp = SMT2Parse::parseNodeForCounting(*tempNode);

    set<string> rand;
    set<string> tempRand;
    set<string> secret;
    set<string> plain;
    vector<string> randVector;
    vector<string> secretVector;
    vector<string> plainVector;
    vector<string> notCareRandVector;

    if(tempNode->getSemd() != NULL) {
        rand = *tempNode->getSemd();
    } else {
        Node::getRandSet(*tempNode, rand);
    }
    Node::getSecretSet(*tempNode, secret);
    Node::getPlainSet(*tempNode, plain);


    SetUtil::SetToVector(rand, randVector);
    SetUtil::SetToVector(secret, secretVector);
    SetUtil::SetToVector(plain, plainVector);

    for(int i = 0; i < randVector.size(); i++)
        StringUtil::trim(randVector[i]);
    for(int i = 0; i < secretVector.size(); i++)
        StringUtil::trim(secretVector[i]);
    for(int i = 0; i < plainVector.size(); i++)
        StringUtil::trim(plainVector[i]);

    string randS = "";
    for(int i = 0; i < randVector.size(); i++) {
        if(i != randVector.size() - 1)
            randS += randVector[i] + ",";
        else
            randS += randVector[i];
    }

    string plainS = "";
    for(int i = 0; i < plainVector.size(); i++) {
        if(i != plainVector.size() - 1)
            plainS += plainVector[i] + ",";
        else
            plainS += plainVector[i];
    }


    string secretS = "";
    for(int i = 0; i < secretVector.size(); i++) {
        if(i != secretVector.size() - 1)
            secretS += secretVector[i] + ",";
        else
            secretS += secretVector[i];
    }
    cout << exp << endl;
    cout << plainS << endl;
    cout << secretS << endl;
    cout << randS << endl;
    return Counting::Q(exp, plainS, secretS, randS);
}

DistributionEnums Reasoner::getDistributionbyCounting(Node* tempNode) {
    string exp = SMT2Parse::parseNodeForCounting(*tempNode);

    set<string> rand;
    set<string> tempRand;
    set<string> secret;
    set<string> plain;
    vector<string> randVector;
    vector<string> secretVector;
    vector<string> plainVector;
    vector<string> notCareRandVector;

    if(tempNode->getSemd() != NULL) {
        rand = *tempNode->getSemd();
    } else {
        Node::getRandSet(*tempNode, rand);
    }
    Node::getSecretSet(*tempNode, secret);
    Node::getPlainSet(*tempNode, plain);


    SetUtil::SetToVector(rand, randVector);
    SetUtil::SetToVector(secret, secretVector);
    SetUtil::SetToVector(plain, plainVector);

    for(int i = 0; i < randVector.size(); i++)
        StringUtil::trim(randVector[i]);
    for(int i = 0; i < secretVector.size(); i++)
        StringUtil::trim(secretVector[i]);
    for(int i = 0; i < plainVector.size(); i++)
        StringUtil::trim(plainVector[i]);

    string randS = "";
    for(int i = 0; i < randVector.size(); i++) {
        if(i != randVector.size() - 1)
            randS += randVector[i] + ",";
        else
            randS += randVector[i];
    }

    string plainS = "";
    for(int i = 0; i < plainVector.size(); i++) {
        if(i != plainVector.size() - 1)
            plainS += plainVector[i] + ",";
        else
            plainS += plainVector[i];
    }


    string secretS = "";
    for(int i = 0; i < secretVector.size(); i++) {
        if(i != secretVector.size() - 1)
            secretS += secretVector[i] + ",";
        else
            secretS += secretVector[i];
    }
//    cout << exp << endl;
//    cout << plainS << endl;
//    cout << secretS << endl;
//    cout << randS << endl;

    clock_t startTime = clock_t();
    string result = Counting::D(startTime, exp, plainS, secretS, randS);
    if(result == "SDD")
        return DistributionEnums ::NPM;
    else if(result == "SID")
        return DistributionEnums ::ISD;
    else if(result == "RUD")
        return DistributionEnums ::RUD;
    else if(result == "UKD")
        return DistributionEnums ::UKD;
    else
        return DistributionEnums ::NULLDISTRIBUTION;

}

void Reasoner::reasonForOneOrder() {

    initWidth(bitLength);
    clock_t c1 = clock();
    int rud = 0;
    int sid = 0;
    int npm = 0;
    int smtcheck = 0;
    result += "Intermediate: " + to_string(interV.size()) + "\n";
    for(int i = 0; i < interV.size(); i++) {
        cout << interV.size() << endl;
        cout << i << endl;

        Node* node = nodeMap[interV[i]];
        cout << node->getName() << endl;

        if(node->getOperatorEnums() == OperatorEnums::POW) {
            cout << "POW" << endl;
        }
        if(node->getOperatorEnums() == OperatorEnums::AFFINEF) {
            cout << "AFFINEF" << endl;
        }


        if(node->getName() == "z0_5") {
            NodeUtil::printTree(node, 0);
            getSimplifiedAndSetSemd(node, node, false);

        }

        if(node->getName() == "__c__1_348") {
            int interCount = 0;
            NodeUtil::countInterNodes(node, interCount);
        }

        clock_t syn = clock();

        TypeSystem::typeInfer(node, TypeInferenceSort::SYN, secret);


        clock_t sem = clock();
        cout << "syn time: " << (sem - syn) / 1000 << endl;
        if(node->getDistributionEnums() == DistributionEnums::UKD) {
            Node* tempNode = new Node();
            NodeUtil::nodeCopy(tempNode, node);
            clock_t copytime = clock();
            cout <<"copy time: " << (copytime - sem) / 1000 << endl;
            int interCount = 0;
            NodeUtil::countInterNodes(node, interCount);
            cout <<"Node size: " << interCount << endl;

            bool hasGotRegion = false;

            doninatedElimitForOneNode(tempNode, hasGotRegion);

            clock_t sem3 = clock();
            cout << "dom ele  time: " << (sem3 - copytime) / 1000 << endl;

            if(tempNode->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
                if (tempNode->getLeftChild()->getSemd() == NULL) {
                    if(!disableSimplify) {
                        getSimplifiedAndSetSemd(node->getLeftChild(), tempNode->getLeftChild(), hasGotRegion);
                    } else {
                        shared_ptr<set<string>> semd = make_shared<set<string>>();
                        Node::getRandSet(*(tempNode->getLeftChild()), *(semd.get()));
                        tempNode->getLeftChild()->setSemd(semd);
                    }

                }

                if (tempNode->getRightChild() != NULL && tempNode->getRightChild()->getSemd() == NULL) {
                    if(!disableSimplify) {
                        getSimplifiedAndSetSemd(node->getRightChild(), tempNode->getRightChild(), hasGotRegion);
                    } else {
                        shared_ptr<set<string>> semd = make_shared<set<string>>();
                        Node::getRandSet(*(tempNode->getRightChild()), *(semd.get()));
                        tempNode->getRightChild()->setSemd(semd);
                    }
                }
            }


            TypeSystem::typeInfer(tempNode, TypeInferenceSort::SEM, secret);

            clock_t sem2 = clock();
            cout << "sem time: " << (sem2 - sem) / 1000 << endl;

            if(tempNode->getDistributionEnums() == DistributionEnums::UKD) {

                if(!disableSimplify ) {
                    getSimplifiedAndSetSemd(node, tempNode, hasGotRegion);
                    if (tempNode->getDistributionEnums() != DistributionEnums::UKD)
                        tempNode->setDistributionEnums(DistributionEnums::UKD);

                    NodeUtil::emptySupport(tempNode);
                    NodeUtil::getAuxiliaryTable(tempNode);
//                    TypeSystem::typeInfer(tempNode, TypeInferenceSort::SEM, secret);
                }
                if(tempNode->getDistributionEnums() == DistributionEnums::UKD) {
		    cout << "enter SMT" << endl;
                    node->setDistributionEnums(SMT2Parse::getDistributionByZ3Test(*tempNode, bitLength));
		    cout << "exit SMT" << endl;

                    smtcheck++;
                    assert(node->getDistributionEnums() == DistributionEnums::NPM ||
                           node->getDistributionEnums() == DistributionEnums::ISD ||
                            node->getDistributionEnums() == DistributionEnums::RUD);

                } else {
                    node->setDistributionEnums(tempNode->getDistributionEnums());
                }

            } else {
                node->setDistributionEnums(tempNode->getDistributionEnums());
            }


            clock_t smt = clock();
            cout << "smt time: " << (smt - sem2) / 1000 << endl;
            NodeUtil::clear(tempNode);
        }

        switch (node->getDistributionEnums()) {
            case DistributionEnums ::RUD:
                rud++;
                break;
            case DistributionEnums ::ISD:
                sid++;
                break;
            case DistributionEnums ::NPM:
                npm++;
                break;
            default:
                assert(false);
        }


        result += "Node: " + node->getName() + ", Type: " + EnumUtil::distributionToString(node->getDistributionEnums()) + "\n";
        cout << "Node: " + node->getName() + ", Type: " + EnumUtil::distributionToString(node->getDistributionEnums()) + "\n";
        if(node->getDistributionEnums() == DistributionEnums::NPM) {
            cout << "haha" << endl;
        }
    }

    clock_t c11 = clock();
    result += "time " + std::to_string((c11 - c1) / 1000) + "ms\n";
    result += "Node SMT check count: " + to_string(smtcheck) + "\n";
    result += "RUD: " + to_string(rud) + "\n";
    result += "SID: " + to_string(sid) + "\n";
    result += "NPM: " + to_string(npm) + "\n";

}

void Reasoner::initWidth(int bitLength) {
    ExpTree::bvwidth = bitLength;
    ExpTree::domain=pow(2, ExpTree::bvwidth);
    Counting::bvwidth = bitLength;
    Counting::domain=pow(2, ExpTree::bvwidth);
}

void Reasoner::reasonForOneOrderCounting() {

    initWidth(bitLength);

    clock_t c1 = clock();
    int rud = 0;
    int sid = 0;
    int npm = 0;
    int ukd = 0;
    int smtcheck = 0;
    int non_syn_check = 0;
    result += "Intermediate: " + to_string(interV.size()) + "\n";
    for(int i = 0; i < interV.size(); i++) {
        cout << "current: " << i << " total: " << interV.size() << endl;
        Node* node = nodeMap[interV[i]];
        clock_t syn = clock();
        TypeSystem::typeInfer(node, TypeInferenceSort::SYN, secret);

        clock_t sem = clock();
//        cout << "syn time: " << (sem - syn) / 1000 << endl;
        if(node->getDistributionEnums() == DistributionEnums::UKD) {
            clock_t tt1 = clock();
            Node* tempNode = new Node();
            NodeUtil::nodeCopy(tempNode, node);
            clock_t copytime = clock();
//            cout <<"copy time: " << (copytime - sem) / 1000 << endl;
            int interCount = 0;
            NodeUtil::countInterNodes(node, interCount);
//            cout <<"Node size: " << interCount << endl;

            bool hasGotRegion = false;
            
            doninatedElimitForOneNode(tempNode, hasGotRegion);

            clock_t sem3 = clock();
//            cout << "dom ele  time: " << (sem3 - copytime) / 1000 << endl;
            
            if(tempNode->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
                if (tempNode->getLeftChild()->getSemd() == NULL) {
                    if(!disableSimplify) {
                        getSimplifiedAndSetSemd(node->getLeftChild(), tempNode->getLeftChild(), hasGotRegion);
                    } else {
                        shared_ptr<set<string>> semd = make_shared<set<string>>();
                        Node::getRandSet(*(tempNode->getLeftChild()), *(semd.get()));
                        tempNode->getLeftChild()->setSemd(semd);
                    }
                    
                }
                
                if (tempNode->getRightChild() != NULL && tempNode->getRightChild()->getSemd() == NULL) {
                    if(!disableSimplify) {
                        getSimplifiedAndSetSemd(node->getRightChild(), tempNode->getRightChild(), hasGotRegion);
                    } else {
                        shared_ptr<set<string>> semd = make_shared<set<string>>();
                        Node::getRandSet(*(tempNode->getRightChild()), *(semd.get()));
                        tempNode->getRightChild()->setSemd(semd);
                    }
                }
            }


            clock_t sem1 = clock();
            TypeSystem::typeInfer(tempNode, TypeInferenceSort::SEM, secret);
            
            clock_t sem2 = clock();
//            cout << "sem time: " << (sem1 - sem3) / 1000 << endl;

            if(tempNode->getDistributionEnums() == DistributionEnums::UKD) {
                non_syn_check++;
                
                if(!disableSimplify ) {
                    getSimplifiedAndSetSemd(node, tempNode, hasGotRegion);
                    if (tempNode->getDistributionEnums() != DistributionEnums::UKD)
                        tempNode->setDistributionEnums(DistributionEnums::UKD);
                    
                    NodeUtil::emptySupport(tempNode);
                    NodeUtil::getAuxiliaryTable(tempNode);
//                    NodeUtil::printTree(tempNode, 0);
//                    TypeSystem::typeInfer(tempNode, TypeInferenceSort::SEM, secret);
                }

                if(tempNode->getDistributionEnums() == DistributionEnums::UKD) {

                    string exp = SMT2Parse::parseNodeForCounting(*tempNode);

                    set<string> rand;
                    set<string> tempRand;
                    set<string> secret;
                    set<string> plain;
                    vector<string> randVector;
                    vector<string> secretVector;
                    vector<string> plainVector;
                    vector<string> notCareRandVector;

                    if(tempNode->getSemd() != NULL) {
                        rand = *tempNode->getSemd();
                    } else {
                        Node::getRandSet(*tempNode, rand);
                    }
                    Node::getSecretSet(*tempNode, secret);
                    Node::getPlainSet(*tempNode, plain);


                    SetUtil::SetToVector(rand, randVector);
                    SetUtil::SetToVector(secret, secretVector);
                    SetUtil::SetToVector(plain, plainVector);

                    for(int i = 0; i < randVector.size(); i++)
                        StringUtil::trim(randVector[i]);
                    for(int i = 0; i < secretVector.size(); i++)
                        StringUtil::trim(secretVector[i]);
                    for(int i = 0; i < plainVector.size(); i++)
                        StringUtil::trim(plainVector[i]);

                    string randS = "";
                    for(int i = 0; i < randVector.size(); i++) {
                        if(i != randVector.size() - 1)
                            randS += randVector[i] + ",";
                        else
                            randS += randVector[i];
                    }

                    string plainS = "";
                    for(int i = 0; i < plainVector.size(); i++) {
                        if(i != plainVector.size() - 1)
                            plainS += plainVector[i] + ",";
                        else
                            plainS += plainVector[i];
                    }


                    string secretS = "";
                    for(int i = 0; i < secretVector.size(); i++) {
                        if(i != secretVector.size() - 1)
                            secretS += secretVector[i] + ",";
                        else
                            secretS += secretVector[i];
                    }
//                    cout << exp << endl;
//                    cout << plainS << endl;
//                    cout << secretS << endl;
//                    cout << randS << endl;
                    clock_t startTime = clock_t();
                    string result = Counting::D(startTime, exp, plainS, secretS, randS);


                    if(result  == "RUD") {
                        node->setDistributionEnums(DistributionEnums::RUD);
                    }
                    else if(result == "SID") {
                        node->setDistributionEnums(DistributionEnums::ISD);
                    }
                    else if(result == "SDD"){
                        node->setDistributionEnums(DistributionEnums::NPM);
                    } else if(result == "UKD") {
                        node->setDistributionEnums(DistributionEnums::UKD);
                    }



                    smtcheck++;
                    assert(node->getDistributionEnums() == DistributionEnums::NPM ||
                           node->getDistributionEnums() == DistributionEnums::ISD ||
                            node->getDistributionEnums() == DistributionEnums::RUD ||
                            node->getDistributionEnums() == DistributionEnums::UKD);

                } else {
                    node->setDistributionEnums(tempNode->getDistributionEnums());
                }
                
            } else {
                node->setDistributionEnums(tempNode->getDistributionEnums());
            }
            
            
            clock_t smt = clock();
//            cout << "smt time: " << (smt - sem2) / 1000 << endl;
            NodeUtil::clear(tempNode);

            clock_t tt2 = clock();

//            cout << "test time: " << (tt2 - tt1) / 1000 << endl;
        }
        
        switch (node->getDistributionEnums()) {
            case DistributionEnums ::RUD:
                rud++;
                break;
            case DistributionEnums ::ISD:
                sid++;
                break;
            case DistributionEnums ::NPM:
                npm++;
                break;
            case DistributionEnums ::UKD:
                ukd++;
                break;
            default:
                assert(false);
        }
        
        
        result += "Node: " + node->getName() + ", Type: " + EnumUtil::distributionToString(node->getDistributionEnums()) + "\n";
        cout << "Node: " + node->getName() + ", Type: " + EnumUtil::distributionToString(node->getDistributionEnums()) + "\n";
        //        cout << result << endl;
    }
    
    clock_t c11 = clock();
    result += "time " + std::to_string((c11 - c1) / 1000) + "ms\n";
    result += "Node SMT check count: " + to_string(smtcheck) + "\n";
    result += "RUD: " + to_string(rud) + "\n";
    result += "SID: " + to_string(sid) + "\n";
    result += "NPM: " + to_string(npm) + "\n";
    result += "UKD(out of time): " + to_string(ukd) + "\n";

}

void Reasoner::reasonForOneOrderOracle() {

    initWidth(bitLength);
    map<string, string> oracle;
    FileUtil::readOracle(oracle);


    clock_t c1 = clock();
    int rud = 0;
    int sid = 0;
    int npm = 0;
    int ukd = 0;
    int smtcheck = 0;
    int smtcount = 0;
    map<string, DistributionEnums > alreadyKnown;
    result += "Intermediate: " + to_string(interV.size()) + "\n";
    for(int i = 0; i < interV.size(); i++) {
        cout << interV.size() << endl;
        cout << i << endl;

        Node* node = nodeMap[interV[i]];
        cout << node->getName() << endl;


        clock_t syn = clock();

        TypeSystem::typeInfer(node, TypeInferenceSort::SYN, secret);

        clock_t sem = clock();

        cout << "syn time: " << (sem - syn) / 1000 << endl;

        if(node->getDistributionEnums() == DistributionEnums::UKD) {
            Node* tempNode = new Node();
            NodeUtil::nodeCopy(tempNode, node);
            clock_t copytime = clock();
            cout <<"copy time: " << (copytime - sem) / 1000 << endl;
            int interCount = 0;
            NodeUtil::countInterNodes(node, interCount);
            cout <<"Node size: " << interCount << endl;

            bool hasGotRegion = false;

            doninatedElimitForOneNode(tempNode, hasGotRegion);

            clock_t sem3 = clock();
            cout << "dom ele  time: " << (sem3 - copytime) / 1000 << endl;

            if(tempNode->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
                if (tempNode->getLeftChild()->getSemd() == NULL) {
                    if(!disableSimplify) {
                        getSimplifiedAndSetSemd(node->getLeftChild(), tempNode->getLeftChild(), hasGotRegion);
                    } else {
                        shared_ptr<set<string>> semd = make_shared<set<string>>();
                        Node::getRandSet(*(tempNode->getLeftChild()), *(semd.get()));
                        tempNode->getLeftChild()->setSemd(semd);
                    }

                }

                if (tempNode->getRightChild() != NULL && tempNode->getRightChild()->getSemd() == NULL) {
                    if(!disableSimplify) {
                        getSimplifiedAndSetSemd(node->getRightChild(), tempNode->getRightChild(), hasGotRegion);
                    } else {
                        shared_ptr<set<string>> semd = make_shared<set<string>>();
                        Node::getRandSet(*(tempNode->getRightChild()), *(semd.get()));
                        tempNode->getRightChild()->setSemd(semd);
                    }
                }
            }


            TypeSystem::typeInfer(tempNode, TypeInferenceSort::SEM, secret);

            clock_t sem2 = clock();
            cout << "sem time: " << (sem2 - sem) / 1000 << endl;

            if(tempNode->getDistributionEnums() == DistributionEnums::UKD) {

                if(!disableSimplify ) {
                    getSimplifiedAndSetSemd(node, tempNode, hasGotRegion);
                    if (tempNode->getDistributionEnums() != DistributionEnums::UKD)
                        tempNode->setDistributionEnums(DistributionEnums::UKD);

                    NodeUtil::emptySupport(tempNode);
                    NodeUtil::getAuxiliaryTable(tempNode);
//                    TypeSystem::typeInfer(tempNode, TypeInferenceSort::SEM, secret);
                }


                if(tempNode->getDistributionEnums() == DistributionEnums::UKD) {
                    string exp = SMT2Parse::parseNodeForCounting(*tempNode);


                    set<string> rand;
                    set<string> tempRand;
                    set<string> secret;
                    set<string> plain;
                    vector<string> randVector;
                    vector<string> secretVector;
                    vector<string> plainVector;
                    vector<string> notCareRandVector;

                    if(tempNode->getSemd() != NULL) {
                        rand = *tempNode->getSemd();
                    } else {
                        Node::getRandSet(*tempNode, rand);
                    }
                    Node::getSecretSet(*tempNode, secret);
                    Node::getPlainSet(*tempNode, plain);


                    SetUtil::SetToVector(rand, randVector);
                    SetUtil::SetToVector(secret, secretVector);
                    SetUtil::SetToVector(plain, plainVector);

                    for(int i = 0; i < randVector.size(); i++)
                        StringUtil::trim(randVector[i]);
                    for(int i = 0; i < secretVector.size(); i++)
                        StringUtil::trim(secretVector[i]);
                    for(int i = 0; i < plainVector.size(); i++)
                        StringUtil::trim(plainVector[i]);

                    string randS = "";
                    for(int i = 0; i < randVector.size(); i++) {
                        if(i != randVector.size() - 1)
                            randS += randVector[i] + ",";
                        else
                            randS += randVector[i];
                    }

                    string plainS = "";
                    for(int i = 0; i < plainVector.size(); i++) {
                        if(i != plainVector.size() - 1)
                            plainS += plainVector[i] + ",";
                        else
                            plainS += plainVector[i];
                    }


                    string secretS = "";
                    for(int i = 0; i < secretVector.size(); i++) {
                        if(i != secretVector.size() - 1)
                            secretS += secretVector[i] + ",";
                        else
                            secretS += secretVector[i];
                    }
                    cout << exp << endl;
                    cout << plainS << endl;
                    cout << secretS << endl;
                    cout << randS << endl;

                    string eq_exp = oracle[tempNode->getName()];

                    if(eq_exp == "" && needHD == 1) {
                        string left = oracle[tempNode->getLeftChild()->getName()];
                        string right = oracle[tempNode->getRightChild()->getName()];
                        eq_exp = "( ^ " + left + " " + right + " )";
                    }

                    list<string> s;
	                s = Counting::split(eq_exp, s, ' ');
                    list<string> *ss;
		            ss = &s;
                    Node* eq_node = parser(ss, secretS, randS, alreadyKnown, smtcount);
                    if(eq_node->getDistributionEnums() == DistributionEnums::NPM) {
                        eq_node->setDistributionEnums(getDistributionbyCounting(eq_node));
                        smtcheck++;
                    }
                    node->setDistributionEnums(eq_node->getDistributionEnums());

                    NodeUtil::clear(eq_node);

                    assert(node->getDistributionEnums() == DistributionEnums::NPM ||
                           node->getDistributionEnums() == DistributionEnums::ISD ||
                           node->getDistributionEnums() == DistributionEnums::UKD ||
                            node->getDistributionEnums() == DistributionEnums::RUD);

                } else {
                    node->setDistributionEnums(tempNode->getDistributionEnums());
                }

            } else {
                node->setDistributionEnums(tempNode->getDistributionEnums());
            }


            clock_t smt = clock();
            cout << "smt time: " << (smt - sem2) / 1000 << endl;
            NodeUtil::clear(tempNode);
        }

        switch (node->getDistributionEnums()) {
            case DistributionEnums ::RUD:
                rud++;
                break;
            case DistributionEnums ::ISD:
                sid++;
                break;
            case DistributionEnums ::NPM:
                npm++;
                break;
            case DistributionEnums ::UKD:
                ukd++;
                break;
            default:
                assert(false);

        }


        result += "Node: " + node->getName() + ", Type: " + EnumUtil::distributionToString(node->getDistributionEnums()) + "\n";
        cout << "Node: " + node->getName() + ", Type: " + EnumUtil::distributionToString(node->getDistributionEnums()) + "\n";
        if(node->getDistributionEnums() == DistributionEnums::NPM) {
            cout << "haha" << endl;
        }
    }

    clock_t c11 = clock();
    result += "time " + std::to_string((c11 - c1) / 1000) + "ms\n";
    result += "Node SMT check count: " + to_string(smtcheck) + "\n";
    result += "RUD: " + to_string(rud) + "\n";
    result += "SID: " + to_string(sid) + "\n";
    result += "NPM: " + to_string(npm) + "\n";
    result += "UKD(out of time): " + to_string(ukd) + "\n";
    result += "smtcount: " + to_string(smtcount) + "\n";


}


void Reasoner::getSimplifiedAndSetSemd(Node* orginNode, Node* tempNode, bool hasGotRegion) {
    simplifyICRBv(tempNode);

    shared_ptr<set<string>> semdSet = make_shared<set<string>>();

    Node::getRandSet(*tempNode, *semdSet);
    tempNode->setSemd(semdSet);
    tempNode->setSimpliedNode(tempNode);

    if(!hasGotRegion) {
        orginNode->setSemd(semdSet);
        orginNode->setSimpliedNode(NodeUtil::getNewNode(tempNode));
    }

}

void Reasoner::simplifyICRBv(Node* node) {
    clock_t t1 = clock();
    if(node->getNodeTypeEnums() != NodeTypeEnums::INTERMEDIATE) {
        return;
    }

    if (node->getOperatorEnums() == OperatorEnums::NOT && node->getLeftChild()->getSimpliedNode() != NULL) {
        Node* simplifyed = NodeUtil::getNewNode(node->getLeftChild()->getSimpliedNode());
        delete node->getLeftChild();
        node->setLeftChild(simplifyed);
        return;
    }

    if(node->getLeftChild()->getSimpliedNode() != NULL) {

        Node* simplified = NodeUtil::getNewNode(node->getLeftChild()->getSimpliedNode());
        delete node->getLeftChild();
        node->setLeftChild(simplified);

    }
    if(node->getRightChild() != NULL && node->getRightChild()->getSimpliedNode() != NULL) {
        Node* simplifed = NodeUtil::getNewNode(node->getRightChild()->getSimpliedNode());
        delete node->getRightChild();
        node->setRightChild(simplifed);
    }

    removeZeroAndOne(node);

    set<string> randSet;
    Node::getRandSet(*node, randSet);
    vector<string> randSetVec;
    SetUtil::SetToVector(randSet, randSetVec);

    for (int i = 0; i < randSetVec.size(); i++) {
        if (isDontCareRand(*node, randSetVec[i])) {
            StringUtil::trim(randSetVec[i]);
            vector<Node* > parentWithXorAdd;
            vector<Node* > parentWithOr;
            vector<Node* > parentWithAndMul;
            vector<Node* > parentWithNot;

            NodeUtil::traverseBv(randSetVec[i], node, parentWithXorAdd, parentWithAndMul, parentWithOr,
                                 parentWithNot);
            if (parentWithXorAdd.size() + parentWithAndMul.size() > parentWithOr.size()) {
                mutate(node, randSetVec[i], 0);
            } else {
                mutate(node, randSetVec[i], 1);
            }

        }
    }



    bool changed = false;
    removeXor(node, changed);

    if(changed)
        removeZeroAndOne(node);

     clock_t t2 = clock();
//     cout << "sim time: " << (t2 - t1) / 1000 << endl;

}

void Reasoner::removeXor(Node* node, bool& changed) {
    if(node->getNodeTypeEnums() != NodeTypeEnums::INTERMEDIATE)
        return;
    if(node->getOperatorEnums() == OperatorEnums::XOR && node->getLeftChild()->getName() == node->getRightChild()->getName()) {
        NodeUtil::copyNode(node, nodeMap[zero]);
        changed = true;
        return;
    }
    removeXor(node->getLeftChild(), changed);
    if(node->getRightChild() != NULL)
        removeXor(node->getRightChild(), changed);
}

void Reasoner::mutate(Node* node, string n, bool b) {

//    NodeUtil::printTree(node, 0);
    map<string, Node*> nodeMap;
    NodeUtil::getNodeMap(node, nodeMap);

    set<string> parentAll;
    NodeUtil::traverse3(n, node, parentAll);
    vector<string> parentAllVec;
    SetUtil::SetToVector(parentAll, parentAllVec);

    while(parentAllVec.size() != 0) {

        for (int i = 0; i < parentAllVec.size(); i++) {
            while (nodeMap[parentAllVec[i]] != NULL) {
                Node *l = nodeMap[parentAllVec[i]];

                if (l->getOperatorEnums() == OperatorEnums::XOR &&
                    l->getLeftChild()->getName() == n) {
                    if (b) {
                        l->setOperatorEnums(OperatorEnums::NOT);
                        NodeUtil::clear(l->getLeftChild());
                        l->setLeftChild(l->getRightChild());
                        l->setRightChild(NULL);

                    } else {
                        Node *right = NodeUtil::getNewNode(l->getRightChild());
                        NodeUtil::copyNode(l, right);
                        NodeUtil::clear(right);
                    }
                } else if (l->getOperatorEnums() == OperatorEnums::XOR &&
                           l->getRightChild()->getName() == n
                        ) {

                    if (b) {
                        l->setOperatorEnums(OperatorEnums::NOT);
                        NodeUtil::clear(l->getRightChild());
                    } else {
                        Node *leftChild = NodeUtil::getNewNode(l->getLeftChild());
                        NodeUtil::copyNode(l, leftChild);
                        NodeUtil::clear(leftChild);

                    }
                } else if(l->getOperatorEnums() == OperatorEnums::ADD) {
                    if(b) {
                        if(l->getLeftChild()->getName() == n) {
                            l->getLeftChild()->setName(one);
                            l->getLeftChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                            l->getLeftChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                            l->getLeftChild()->setLeftChild(NULL);
                            l->getLeftChild()->setRightChild(NULL);
                        } else {
                            l->getRightChild()->setName(one);
                            l->getRightChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                            l->getRightChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                            l->getRightChild()->setLeftChild(NULL);
                            l->getRightChild()->setRightChild(NULL);
                        }

                    } else {
                        if(l->getLeftChild()->getName() == n) {
                            Node *right = NodeUtil::getNewNode(l->getRightChild());
                            NodeUtil::copyNode(l, right);
                            NodeUtil::clear(right);
                        } else {
                            Node *leftChild = NodeUtil::getNewNode(l->getLeftChild());
                            NodeUtil::copyNode(l, leftChild);
                            NodeUtil::clear(leftChild);
                        }
                    }

                }else if(l->getOperatorEnums() == OperatorEnums::MINU) {
                    if(b) {
                        if(l->getLeftChild()->getName() == n) {
                            l->getLeftChild()->setName(one);
                            l->getLeftChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                            l->getLeftChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                            l->getLeftChild()->setLeftChild(NULL);
                            l->getLeftChild()->setRightChild(NULL);
                        } else {
                            l->getRightChild()->setName(one);
                            l->getRightChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                            l->getRightChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                            l->getRightChild()->setLeftChild(NULL);
                            l->getRightChild()->setRightChild(NULL);
                        }

                    } else {
                        if(l->getLeftChild()->getName() == n) {
                            l->getLeftChild()->setName(zero);
                            l->getLeftChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                            l->getLeftChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                            l->getLeftChild()->setLeftChild(NULL);
                            l->getLeftChild()->setRightChild(NULL);

                        } else {
                            Node *leftChild = NodeUtil::getNewNode(l->getLeftChild());
                            NodeUtil::copyNode(l, leftChild);
                            NodeUtil::clear(leftChild);
                        }
                    }

                }  else if (l->getOperatorEnums() == OperatorEnums::OR) {
                    if (b) {
                        l->setName(n);
                        l->setNodeTypeEnums(NodeTypeEnums::MASK);
                        l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                        NodeUtil::clear(l->getLeftChild());
                        NodeUtil::clear(l->getRightChild());
                        l->setLeftChild(NULL);
                        l->setRightChild(NULL);
                        mutate(node, n, 1);
                    } else {
                        if (l->getLeftChild()->getName() == n) {
                            Node *right = NodeUtil::getNewNode(l->getRightChild());
                            NodeUtil::copyNode(l, right);
                            NodeUtil::clear(right);

                        } else {
                            Node *leftChild = NodeUtil::getNewNode(l->getLeftChild());
                            NodeUtil::copyNode(l, leftChild);
                            NodeUtil::clear(leftChild);

                        }
                    }
                } else if (l->getOperatorEnums() == OperatorEnums::AND) {
                    if (!b) {
                        l->setName(n);
                        l->setNodeTypeEnums(NodeTypeEnums::MASK);
                        l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                        l->setLeftChild(NULL);
                        l->setRightChild(NULL);
                        mutate(node, n, 0);
                    } else {
                        if (l->getLeftChild()->getName() == n) {
                            Node *right = NodeUtil::getNewNode(l->getRightChild());
                            NodeUtil::copyNode(l, right);
                            NodeUtil::clear(right);
                        } else {
                            Node *leftChild = NodeUtil::getNewNode(l->getLeftChild());
                            NodeUtil::copyNode(l, leftChild);
                            NodeUtil::clear(leftChild);

                        }
                    }
                } else if(l->getOperatorEnums() == OperatorEnums::MUL) {
                    assert(false && "bitvector mul");

                } else if(l->getOperatorEnums() == OperatorEnums::GFMUL) {
                    assert(false && "bitvector GFMUL");
                } else if(l->getOperatorEnums() == OperatorEnums::NOT){
                    l->setName("new");
                    l->setNodeTypeEnums(NodeTypeEnums::MASK);
                    l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                    l->setLeftChild(NULL);
                    l->setRightChild(NULL);
                    mutate(node, "new", !b);

                }
                nodeMap.clear();
                NodeUtil::getNodeMap(node, nodeMap);

            }
        }

        parentAllVec.clear();
        parentAll.clear();
        NodeUtil::traverse3(n, node, parentAll);
        SetUtil::SetToVector(parentAll, parentAllVec);

    }
}



void Reasoner::removeZeroAndOne(Node *node) {
    if(node == NULL)
        return;

    removeZeroAndOne(node->getLeftChild());
    removeZeroAndOne(node->getRightChild());

    if(node->getOperatorEnums() == OperatorEnums::XOR && node->getLeftChild()->getName() == one) {
        node->setOperatorEnums(OperatorEnums::NOT);
        NodeUtil::clear(node->getLeftChild());
        node->setLeftChild(node->getRightChild());
        node->setRightChild(NULL);

    }else if (node->getOperatorEnums() == OperatorEnums::XOR && node->getRightChild()->getName() == one) {
        node->setOperatorEnums(OperatorEnums::NOT);
        NodeUtil::clear(node->getRightChild());
        node->setRightChild(NULL);

    }else if (node->getOperatorEnums() == OperatorEnums::XOR && node->getLeftChild()->getName() == zero) {
        Node *right = NodeUtil::getNewNode(node->getRightChild());
        NodeUtil::copyNode(node, right);
        NodeUtil::clear(right);

    }else if (node->getOperatorEnums() == OperatorEnums::XOR && node->getRightChild()->getName() == zero) {
        Node *left = NodeUtil::getNewNode(node->getLeftChild());
        NodeUtil::copyNode(node, left);
        NodeUtil::clear(left);

    }else if (node->getOperatorEnums() == OperatorEnums::AND && node->getLeftChild()->getName() == one) {
        Node *right = NodeUtil::getNewNode(node->getRightChild());
        NodeUtil::copyNode(node, right);
        NodeUtil::clear(right);

    }else if (node->getOperatorEnums() == OperatorEnums::AND && node->getRightChild()->getName() == one) {
        Node *left = NodeUtil::getNewNode(node->getLeftChild());
        NodeUtil::copyNode(node, left);
        NodeUtil::clear(left);

    }else if (node->getOperatorEnums() == OperatorEnums::AND && node->getLeftChild()->getName() == zero) {
        NodeUtil::clear(node->getRightChild());

        node->setName(zero);
        node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
        node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
        node->setLeftChild(NULL);
        node->setRightChild(NULL);

        mutate(node, zero, 0);

    }else if (node->getOperatorEnums() == OperatorEnums::AND && node->getRightChild()->getName() == zero) {
        NodeUtil::clear(node->getLeftChild());

        node->setName(zero);
        node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
        node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
        node->setLeftChild(NULL);
        node->setRightChild(NULL);

        mutate(node, zero, 0);

    }else if (node->getOperatorEnums() == OperatorEnums::OR && node->getLeftChild()->getName() == one) {
        NodeUtil::clear(node->getRightChild());

        node->setName(one);
        node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
        node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
        node->setLeftChild(NULL);
        node->setRightChild(NULL);

        mutate(node, one, 1);

    }else if (node->getOperatorEnums() == OperatorEnums::OR && node->getRightChild()->getName() == one) {
        NodeUtil::clear(node->getLeftChild());

        node->setName(one);
        node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
        node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
        node->setLeftChild(NULL);
        node->setRightChild(NULL);

        mutate(node, one, 1);

    }else if (node->getOperatorEnums() == OperatorEnums::OR && node->getLeftChild()->getName() == zero) {
        Node *right = NodeUtil::getNewNode(node->getRightChild());
        NodeUtil::copyNode(node, right);
        NodeUtil::clear(right);
    }else if (node->getOperatorEnums() == OperatorEnums::OR && node->getRightChild()->getName() == zero) {
        Node *left = NodeUtil::getNewNode(node->getLeftChild());
        NodeUtil::copyNode(node, left);
        NodeUtil::clear(left);

    }else if (node->getOperatorEnums() == OperatorEnums::NOT && node->getLeftChild()->getName() == zero) {
        NodeUtil::clear(node->getLeftChild());

        node->setName(one);
        node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
        node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
        node->setLeftChild(NULL);
        node->setRightChild(NULL);

        mutate(node, one, 1);

    }else if (node->getOperatorEnums() == OperatorEnums::NOT && node->getLeftChild()->getName() == one) {
        NodeUtil::clear(node->getLeftChild());

        node->setName(zero);
        node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
        node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
        node->setLeftChild(NULL);
        node->setRightChild(NULL);

        mutate(node, zero, 0);

    }


}


void Reasoner::doninatedElimitForOneNode(Node* current, bool &f) {
    set<string> dominatedExpresstionNames;

    Node* name = NULL;
    while(true) {
        if(current->getNodeTypeEnums() != NodeTypeEnums::INTERMEDIATE)
            break;

        bool flag = false;
        NodeUtil::getDominatedExpressions(current, name, dominatedExpresstionNames, flag);

        if(flag == false)
            break;
        name = NodeUtil::getNewNode(name);
        shared_ptr<set<string>> dominateSet = name->getDominant();
        set<string>::iterator iterator1 = dominateSet->begin();

        for (; iterator1 != dominateSet->end(); iterator1++) {
            string candidateDom = *iterator1;
            // find candidateDom number in this set, and express number in this set
            StringUtil::trim(candidateDom);
            int numberOfCandidateDom = 0;
            NodeUtil::getANodeNum(current, candidateDom, numberOfCandidateDom);
            int numberOfExpressions = 0;
            NodeUtil::getANodeNum(current, name->getName(), numberOfExpressions);

            if (numberOfCandidateDom == numberOfExpressions) {
                // print information for debug
//                cout << "use: " << candidateDom << " elimite: " << name->getName() << endl;
                set<string> namesSec;
                Node::getSecretSet(*name, namesSec);
//                cout << "contains k: " << endl;
//                SetUtil::put_out(namesSec);
                // elimite
                nodeElimate(current, name->getName(), candidateDom);
                NodeUtil::emptySupport(current);
                NodeUtil::getAuxiliaryTable(current);
                f = true;
                dominatedExpresstionNames.clear();
                break;
            }

        }
    }
//    SetUtil::put_out(dominatedExpresstionNames);
    delete(name);

}

void Reasoner::nodeElimate(Node* tempNode, string expressName, string candidateDom) {
    if(tempNode->getNodeTypeEnums() != NodeTypeEnums::INTERMEDIATE)
        return;

    if(tempNode->getName() == expressName) {
        if(nodeMap[candidateDom] == NULL) {
            Node* newNode = new Node(candidateDom, OperatorEnums::NULLOPERATOR, NodeTypeEnums::MASK);
            newNode->setDistributionEnums(DistributionEnums::RUD);
            nodeMap[candidateDom] = newNode;
        }
        NodeUtil::copyNode(tempNode, nodeMap[candidateDom]);
        return;
    }

    if(tempNode->getLeftChild() != NULL)
        nodeElimate(tempNode->getLeftChild(), expressName, candidateDom);
    if(tempNode->getRightChild() != NULL)
        nodeElimate(tempNode->getRightChild(), expressName, candidateDom);

}



bool Reasoner::isDontCareRand(Node node, string checkRand) {

//    cout << "check whether random: " + checkRand + " is dontcare varibale.\n";
    string bitLengthS = StringUtil::getString(bitLength);

    set<string> rand;
    set<string> secret;
    set<string> plain;
    vector<string> randVector;
    vector<string> secretVector;
    vector<string> plainVector;

    Node::getRandSet(node, rand);
    Node::getSecretSet(node, secret);
    Node::getPlainSet(node, plain);

    SetUtil::SetToVector(rand, randVector);
    SetUtil::SetToVector(secret, secretVector);
    SetUtil::SetToVector(plain, plainVector);

    int randCount = rand.size();
    int secretCount = secret.size();
    int plainCount = plain.size();

    if(bitLength > 1) {

        string first = "This is node " + node.getName() + " to get not care random smt2 file.";
        string finalString = "";
        finalString.append(SMT2Parse::bvgfmulDefination() + "\n");
        finalString.append(SMT2Parse::powdefine() + "\n");
        z3::context contextUse;
        z3::solver solverUse(contextUse);

        for (int i = 0; i < randCount; i++) {
            string s = "";
            s.append("(declare-fun ");
            s.append(randVector[i] + " () (_ BitVec " + bitLengthS + "))");

            finalString.append(s + "\n");

        }


        for (int i = 0; i < secretCount; i++) {
            string s = "";
            s.append("(declare-fun ");
            s.append(secretVector[i] + " () (_ BitVec " + bitLengthS + "))");
            finalString.append(s + "\n");
        }

        for (int i = 0; i < plainCount; i++) {
            string s = "";
            s.append("(declare-fun ");
            s.append(plainVector[i] + "() (_ BitVec " + bitLengthS + "))");
            finalString.append(s + "\n");
        }

        for (int i = 0; i < 2; i++) {
            string s = "";
            s.append("(declare-fun num");
            s.append(StringUtil::getString(i) + "() (_ BitVec " + bitLengthS + "))");
            finalString.append(s + "\n");

        }

        for (int i = 0; i < 2; i++) {
            string s = "";
            s.append("(declare-fun c");
            s.append(StringUtil::getString(i) + "() (_ BitVec " + bitLengthS + "))");
            finalString.append(s + "\n");

        }


        for (int i = 0; i < 2; i++) {
            string s = "";
            s.append("(assert(= num" + StringUtil::getString(i));
            string ss = SMT2Parse::parseNodeBv(node);
            s.append(" " + ss + " ))");
            if (i == 0) {
                StringUtil::replace_all(s, checkRand, "  c0  ");
            } else {
                StringUtil::replace_all(s, checkRand, "  c1  ");
            }

            StringUtil::replace_all(s, " 0 ", "  (_ bv0 " + bitLengthS + ") ");
            StringUtil::replace_all(s, " 1 ",
                                    "  (_ bv" + StringUtil::getString(int(pow(2, bitLength) - 1)) + " " + bitLengthS +
                                    ") ");

            finalString.append(s + "\n");

        }

        string s = "(assert(not (= num0 num1 ))) ";
        finalString.append(s + "\n");

        solverUse.from_string(finalString.data());


        bool flag = false;

        switch (solverUse.check()) {
            case z3::unsat: {
                flag = true;
                break;
            }
            case z3::sat: {
                break;
            }
            case z3::unknown:
                break;
        }

        return flag;
    } else {
        string first = "This is node " + node.getName() + " to get not care random smt2 file.";
        string finalString="";
        z3::context contextUse;
        z3::solver solverUse = z3::tactic(contextUse, "smt").mk_solver();

        for (int i = 0; i < randCount; i++) {
            string s = "";
            s.append("(declare-fun ");
            s.append(randVector[i] + " () Bool )");

            finalString.append(s+"\n");
        }


        for (int i = 0; i < secretCount; i++) {
            string s = "";
            s.append("(declare-fun ");
            s.append(secretVector[i] + " () Bool )");
            finalString.append(s+"\n");
        }

        for (int i = 0; i < plainCount; i++) {
            string s = "";
            s.append("(declare-fun ");
            s.append(plainVector[i] + "() Bool )");
            finalString.append(s+"\n");
        }

        for (int i = 0; i < 2; i++) {
            string s = "";
            s.append("(declare-fun num");
            s.append(StringUtil::getString(i) + " () Bool )");
            finalString.append(s+"\n");
        }


        for (int i = 0; i < 2; i++) {
            string s = "";
            s.append("(assert(= num" + StringUtil::getString(i));
            string ss = SMT2Parse::parseNodeBool(node);
            s.append(" " + ss + " ))");
            if (i == 0) {
                StringUtil::replace_all(s, checkRand, "  false  ");
            }
            else {
                StringUtil::replace_all(s, checkRand, "  true  ");
            }
            StringUtil::replace_all(s, " 0 ", "  false  ");
            StringUtil::replace_all(s, " 1 ", "  true  ");

            finalString.append(s+"\n");
        }

        string s = "(assert(not (= num0 num1 ))) ";
        finalString.append(s+"\n");
//        cout << finalString  << endl;
        solverUse.from_string(finalString.data());

        bool flag=false;

        switch (solverUse.check()) {
            case z3::unsat:   //std::cout << "unsat\n";
            {
                clock_t c2 = clock();
                flag = true;
                break;
            }
            case z3::sat:     //std::cout << "sat\n";
            {
                clock_t c3 = clock();
                break;
            }
            case z3::unknown: //std::cout << "unknown\n";
                break;
        }

        return flag;

    }
}


Node* Reasoner::parser(list<string> *s, string K, string R, map<string, DistributionEnums>& alreadyKnown, int & smtcount) {
    string operators="+-*&|~^>><<!";
    stack<Node*> ss;
    s->reverse();
    list<string>::iterator it;

    int no = 1;
    for(it=s->begin();it!=s->end();it++)
    {


        if(*it=="("||*it==")"||*it==""||*it==" "||*it=="\n"||*it=="\t")
        {
            continue;
        }else if(*it=="~")
        {
            if(ss.size()<1)
            {
                cout<< "ExpSyntaxError"<<endl;
                exit(0);
            }

            Node* t = ss.top();
            Node* re = new Node("node" + to_string(no), OperatorEnums::NOT, NodeTypeEnums::INTERMEDIATE);
            re->setLeftChild(t);
            ss.pop();
            ss.push(re);
        }else if(*it=="pow2")
        {
            if(ss.size()<1)
            {
                cout<< "ExpSyntaxError"<<endl;
                exit(0);
            }
        }else if(*it=="pow4")
        {
            if(ss.size()<1)
            {
                cout<< "ExpSyntaxError"<<endl;
                exit(0);
            }
        }else if(*it=="pow8")
        {
            if(ss.size()<1)
            {
                cout<< "ExpSyntaxError"<<endl;
                exit(0);
            }
        }else if(*it=="pow16")
        {
            if(ss.size()<1)
            {
                cout<< "ExpSyntaxError"<<endl;
                exit(0);
            }
        }else if(operators.find(*it)!=string::npos)
        {
            if(ss.size()<2)
            {
                cout<< "ExpSyntaxError"<<endl;
                exit(0);
            }
            Node* l = ss.top();
            ss.pop();
            Node* r = ss.top();
            ss.pop();
            Node* expt = NULL;
            if(*it == "+")
                expt = new Node("node" + to_string(no), OperatorEnums::ADD, NodeTypeEnums::INTERMEDIATE);
            else if(*it == "-")
                expt = new Node("node" + to_string(no), OperatorEnums::MINU, NodeTypeEnums::INTERMEDIATE);
            else if(*it == "*")
                expt = new Node("node" + to_string(no), OperatorEnums::MUL, NodeTypeEnums::INTERMEDIATE);
            else if(*it == "&")
                expt = new Node("node" + to_string(no), OperatorEnums::AND, NodeTypeEnums::INTERMEDIATE);
            else if(*it == "|")
                expt = new Node("node" + to_string(no), OperatorEnums::OR, NodeTypeEnums::INTERMEDIATE);
            else if(*it == "^")
                expt = new Node("node" + to_string(no), OperatorEnums::XOR, NodeTypeEnums::INTERMEDIATE);
            else if(*it == "!")
                expt = new Node("node" + to_string(no), OperatorEnums::GFMUL, NodeTypeEnums::INTERMEDIATE);

            expt->setLeftChild(l);
            expt->setRightChild(r);

            NodeUtil::emptySupport(expt);
            NodeUtil::getAuxiliaryTable(expt);

            TypeSystem::typeInfer(expt, TypeInferenceSort::SYN, secret);


            if(expt->getDistributionEnums() == DistributionEnums::UKD) {
                Node *tempNode = new Node();
                NodeUtil::nodeCopy(tempNode, expt);
                bool hasGotRegion = false;

                doninatedElimitForOneNode(tempNode, hasGotRegion);

                if (tempNode->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
                    if (tempNode->getLeftChild()->getSemd() == NULL) {
                        if (!disableSimplify) {
                            getSimplifiedAndSetSemd(expt->getLeftChild(), tempNode->getLeftChild(), hasGotRegion);
                        } else {
                            shared_ptr<set<string>> semd = make_shared<set<string>>();
                            Node::getRandSet(*(tempNode->getLeftChild()), *(semd.get()));
                            tempNode->getLeftChild()->setSemd(semd);
                        }

                    }

                    if (tempNode->getRightChild() != NULL && tempNode->getRightChild()->getSemd() == NULL) {
                        if (!disableSimplify) {
                            getSimplifiedAndSetSemd(expt->getRightChild(), tempNode->getRightChild(), hasGotRegion);
                        } else {
                            shared_ptr<set<string>> semd = make_shared<set<string>>();
                            Node::getRandSet(*(tempNode->getRightChild()), *(semd.get()));
                            tempNode->getRightChild()->setSemd(semd);
                        }
                    }
                }


                TypeSystem::typeInfer(tempNode, TypeInferenceSort::SEM, secret);
                if(tempNode->getDistributionEnums() == DistributionEnums::UKD) {
			        tempNode->setDistributionEnums(DistributionEnums::NPM);
                }
                expt->setDistributionEnums(tempNode->getDistributionEnums());
            }

            ss.push(expt);

            no++;
        }else{
            if(R.find(*it) != string::npos) {
                Node *expt = new Node(*it, OperatorEnums::NULLOPERATOR, NodeTypeEnums::MASK);
                expt->setDistributionEnums(DistributionEnums::RUD);
                ss.push(expt);
            } else if(K.find(*it) != string::npos) {
                Node *expt = new Node(*it, OperatorEnums::NULLOPERATOR, NodeTypeEnums::SECRECT);
                expt->setDistributionEnums(DistributionEnums::UKD);
                ss.push(expt);
            } else if(*it == "C") {
                Node *expt = new Node(*it, OperatorEnums::NULLOPERATOR, NodeTypeEnums::MASK);
                expt->setDistributionEnums(DistributionEnums::RUD);
                ss.push(expt);
            } else {
                Node *expt = new Node(*it, OperatorEnums::NULLOPERATOR, NodeTypeEnums::CONSTANT);
                expt->setDistributionEnums(DistributionEnums::ISD);
                ss.push(expt);
            }
        }
    }
    if(ss.size()!=1)
    {

        cout<< "ExpSyntaxError"<<endl;
        exit(0);
    }

    Node* result = ss.top();
    return result;

}