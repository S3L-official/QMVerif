//
// Created by Pengfei Gao on 2018/8/17.
//

#ifndef HOSCINFER_TYPESYSTEM_H
#define HOSCINFER_TYPESYSTEM_H

#include "Node.h"
#include "util/SetUtil.h"
#include "TypeInferenceSort.h"
#include "z3++.h"
#include "util/NodeUtil.h"
#include <map>

class TypeSystem {

    static bool isOperatorInBlackCircle(OperatorEnums e) {
        if(e == OperatorEnums::AND ||
                e == OperatorEnums::OR ||
                e == OperatorEnums::GFMUL ||
                e == OperatorEnums::MUL ||
                e == OperatorEnums::ADD ||
                e == OperatorEnums::MINU ||
                e == OperatorEnums::XOR ||
                e == OperatorEnums::LSH ||
                e == OperatorEnums::RSH ||
                e == OperatorEnums::ROTL) {
            return true;
        }
        return false;

    }


public:
    static void typeInfer(Node* node, TypeInferenceSort sort, set<string>& secretAll) {

        if(node->getOperatorEnums() == OperatorEnums::POW) {
            Node* left = node->getLeftChild();

            if(left->getNodeTypeEnums() == NodeTypeEnums::SECRECT) {
                node->setDistributionEnums(DistributionEnums::NPM);
                return;
            }

            set<string> secret;
            SetUtil::Intersection(*(node->getSupportV().get()),secretAll ,secret);
            if(secret.size() == 0) {
                node->setDistributionEnums(DistributionEnums::ISD);
                return;
            }

            if(left->getDistributionEnums() == DistributionEnums::RUD || left->getDistributionEnums() == DistributionEnums::ISD) {
                node->setDistributionEnums(DistributionEnums::ISD);
                return;
            }


            node->setDistributionEnums(DistributionEnums::UKD);
            return;

        }

        if(node->getOperatorEnums() == OperatorEnums::SBOX) {
            node->setDistributionEnums(node->getLeftChild()->getDistributionEnums());
            return;
        }

        shared_ptr<set<string>> dom = node->getDominant();
        if(dom->size() > 0) {
            node->setDistributionEnums(DistributionEnums::RUD);
            return;
        }
        set<string> secret;
        SetUtil::Intersection(*(node->getSupportV().get()),secretAll ,secret);
        if(secret.size() == 0) {
            node->setDistributionEnums(DistributionEnums::ISD);
            return;
        }
        if (node->getOperatorEnums() == OperatorEnums::NOT || node->getOperatorEnums()==OperatorEnums::EQUAL) {
            node->setDistributionEnums(node->getLeftChild()->getDistributionEnums());
            return;
        }

        if( node->getOperatorEnums() == OperatorEnums::XOR || node->getOperatorEnums() == OperatorEnums::MINU )  {
            if(node->getRightChild() != NULL) {
                if(node->getLeftChild()->getName() == node->getRightChild()->getName()) {
                    node->setDistributionEnums(DistributionEnums::ISD);
                    return;
                }
            }
        }

        if(node->getOperatorEnums() == OperatorEnums::AND && node->getRightChild() != NULL) {
            if(node->getLeftChild()->getName() == node->getRightChild()->getName()) {
                if(node->getDistributionEnums() == DistributionEnums::NPM) {
                    node->setDistributionEnums(DistributionEnums::NPM);
                    return;
                }
            }

        }



        if(node->getNodeTypeEnums() == NodeTypeEnums::SECRECT) {
            node->setDistributionEnums(DistributionEnums::NPM);
            return;
        }

        if(node->getLeftChild()->getNodeTypeEnums() == NodeTypeEnums::SECRECT && node->getRightChild()->getNodeTypeEnums() == NodeTypeEnums::SECRECT) {
            if(node->getLeftChild()->getName() == node->getRightChild()->getName()) {
                if(node->getOperatorEnums() == OperatorEnums::XOR) {
                    node->setDistributionEnums(DistributionEnums::ISD);
                    return;
                }
            }
        }

        set<string> rand;
        Node::getRandSet(*node, rand);
        if(rand.size() == 0 && secret.size() > 0) {
            node->setDistributionEnums(DistributionEnums::NPM);
            return;
        }

        Node* leftChild = node->getLeftChild();
        Node* rightChild = node->getRightChild();
	if(leftChild->getDistributionEnums() == DistributionEnums::NPM && rightChild->getNodeTypeEnums() == NodeTypeEnums::CONSTANT) {
            node->setDistributionEnums(DistributionEnums::NPM);
            return;
	}
	if(rightChild->getDistributionEnums() == DistributionEnums::NPM && leftChild->getNodeTypeEnums() == NodeTypeEnums::CONSTANT) {
            node->setDistributionEnums(DistributionEnums::NPM);
            return;
	}
	
        if(leftChild->getDistributionEnums() == DistributionEnums::RUD &&
           rightChild->getDistributionEnums() == DistributionEnums::RUD &&
                TypeSystem::isOperatorInWhiteCircle(node->getOperatorEnums())) {

            shared_ptr<set<string>> leftDom = leftChild->getDominant();
            shared_ptr<set<string>> rightCareOrNotCare = sort == TypeInferenceSort::SYN ? rightChild->getSupportV() : rightChild->getSemd();
            set<string> temp;
            SetUtil::Difference(*leftDom, *rightCareOrNotCare, temp);

            if (temp.size() > 0) {
                node->setDistributionEnums(DistributionEnums::ISD);
                return;
            }

            shared_ptr<set<string>> rightDom = rightChild->getDominant();
            shared_ptr<set<string>> leftCareOrNotCare = sort == TypeInferenceSort::SYN ? leftChild->getSupportV() : leftChild->getSemd();
            set<string> temp1;
            if(leftCareOrNotCare != nullptr)
                SetUtil::Difference(*rightDom, *leftCareOrNotCare, temp1);
            else {
                temp1 = *rightDom;
            }

            if (temp1.size() > 0) {
                node->setDistributionEnums(DistributionEnums::ISD);
                return;
            }
        }

        if(leftChild->getDistributionEnums() == DistributionEnums::ISD &&
                rightChild->getDistributionEnums() == DistributionEnums::ISD &&
                TypeSystem::isOperatorInBlackCircle(node->getOperatorEnums())) {
            shared_ptr<set<string>>leftCareOrNotCare = sort == TypeInferenceSort::SYN ? leftChild->getSupportV() : leftChild->getSemd();
            shared_ptr<set<string>>rightCareOrNotCare = sort == TypeInferenceSort::SYN ? rightChild->getSupportV() : rightChild->getSemd();
            set<string> temp;
            SetUtil::Intersection(*leftCareOrNotCare, *rightCareOrNotCare, temp);
            if(temp.size() == 0) {
                node->setDistributionEnums(DistributionEnums::ISD);
                return;
            }
        }

        if(rightChild != NULL) {
            if (leftChild->getDistributionEnums() == DistributionEnums::NPM &&
                rightChild->getDistributionEnums() == DistributionEnums::RUD &&
                TypeSystem::isOperatorInWhiteCircle(node->getOperatorEnums())) {

                shared_ptr<set<string>> rightDom = rightChild->getDominant();
                shared_ptr<set<string>> leftCareOrNotCare =
                        sort == TypeInferenceSort::SYN ? leftChild->getSupportV() : leftChild->getSemd();
                set<string> temp;
                SetUtil::Difference(*rightDom, *leftCareOrNotCare, temp);
                if (temp.size() > 0) {
                    node->setDistributionEnums(DistributionEnums::NPM);
                    return;
                }

            }
            if (rightChild->getDistributionEnums() == DistributionEnums::NPM &&
                leftChild->getDistributionEnums() == DistributionEnums::RUD &&
                TypeSystem::isOperatorInWhiteCircle(node->getOperatorEnums())) {

                shared_ptr<set<string>> leftDom = leftChild->getDominant();
                shared_ptr<set<string>> rightCareOrNotCare =
                        sort == TypeInferenceSort::SYN ? rightChild->getSupportV() : rightChild->getSemd();
                set<string> temp;
                SetUtil::Difference(*leftDom, *rightCareOrNotCare, temp);
                if (temp.size() > 0) {
                    node->setDistributionEnums(DistributionEnums::NPM);
                    return;
                }

            }
        }

        if (node->getOperatorEnums() == OperatorEnums::AND || node->getOperatorEnums() == OperatorEnums::OR) {
            if (leftChild->getDistributionEnums() == DistributionEnums::RUD ||
                rightChild->getDistributionEnums() == DistributionEnums::RUD) {

                if (leftChild->getDistributionEnums() != DistributionEnums::UKD &&
                    rightChild->getDistributionEnums() != DistributionEnums::UKD &&
                    leftChild->getDistributionEnums() != DistributionEnums::NPM &&
                    rightChild->getDistributionEnums() != DistributionEnums::NPM) {

                    shared_ptr<set<string>> leftSed = sort == TypeInferenceSort::SYN ? leftChild->getSupportV() : leftChild->getSemd();

                    shared_ptr<set<string>>rightSed = sort == TypeInferenceSort::SYN ? rightChild->getSupportV() : rightChild->getSemd();


                    set<string> *temp = new set<string>();
                    SetUtil::Intersection(*leftSed, *rightSed, *temp);
                    if (temp->size() == 0) {
                        node->setDistributionEnums(DistributionEnums::ISD);
                        return;
                    }
                }


            }
        }
        node->setDistributionEnums(DistributionEnums::UKD);

    }

    static bool isOperatorInWhiteCircle(OperatorEnums e) {
        if(e == OperatorEnums::AND ||
                e == OperatorEnums::OR ||
                e == OperatorEnums::GFMUL ||
                e == OperatorEnums::MUL) {
            return true;
        }
        return false;

    }

};


#endif //HOSCINFER_TYPESYSTEM_H
