#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <map>
#include <list>
#include <set>
#include "z3++.h"
#include <deque>
#include "../Node.h"
#include "SetUtil.h"
#include <stack>


using namespace std;
class NodeUtil {
public:

	static void countInterNodes(Node* node, int & result) {
		if(node == NULL)
			return;
		if(node->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE)
			result++;

		countInterNodes(node->getLeftChild(), result);
		countInterNodes(node->getRightChild(), result);
	}



	static void nodeCopy(Node* n, Node* copyNode) {


		Node* newLeftNode = NULL;
	    Node* newRightNode = NULL;

	    if(copyNode->getLeftChild() != NULL) {
	        newLeftNode = new Node();
	        nodeCopy(newLeftNode, copyNode->getLeftChild());
	    }
	    if(copyNode->getRightChild() != NULL) {
            newRightNode = new Node();
	        nodeCopy(newRightNode, copyNode->getRightChild());
	    }
		n->setName(copyNode->getName());
		n->setDistributionEnums(copyNode->getDistributionEnums());
		n->setNodeTypeEnums(copyNode->getNodeTypeEnums());
		n->setOperatorEnums(copyNode->getOperatorEnums());
		n->setDependences(SetUtil::copySet(copyNode->getDependences()));
		n->setDominant(SetUtil::copySet(copyNode->getDominant()));
		n->setSupportV(SetUtil::copySet(copyNode->getSupportV()));
		n->setPerfectM(SetUtil::copySet(copyNode->getPerfectM()));
		n->setUniqueM(SetUtil::copySet(copyNode->getUniqueM()));
		n->setParent(copyNode->getParent());
		n->setParentNodeNames(copyNode->getParentNodeNames());
		n->setSemd(copyNode->getSemd());
		n->setSimpliedNode(copyNode->getSimpliedNode());
		n->setLeftChild(newLeftNode);
        n->setRightChild(newRightNode);
        n->setPow(copyNode->getPow());
        n->setRotl(copyNode->getRotl());

	}


	static void copyNode(Node* des, Node* sou) {
		des->setName(sou->getName());
		des->setDistributionEnums(sou->getDistributionEnums());
		des->setOperatorEnums(sou->getOperatorEnums());
		des->setNodeTypeEnums(sou->getNodeTypeEnums());

		NodeUtil::clear(des->getLeftChild());
		NodeUtil::clear(des->getRightChild());

		des->setLeftChild(NodeUtil::getNewNode(sou->getLeftChild()));
		des->setRightChild(NodeUtil::getNewNode(sou->getRightChild()));
		des->setPow(sou->getPow());
		des->setRotl(sou->getRotl());


	}


	static void statisticKeyWithOutSomeExpression(set<string> & alreadyElimateExpressName, Node* node, int & result) {
		if(node == NULL)
			return;
		if(alreadyElimateExpressName.find(node->getName()) != alreadyElimateExpressName.end()) {
			cout << node->getName() << endl;
			return;
		}
		if(node->getNodeTypeEnums() == NodeTypeEnums::SECRECT) {
			result++;
			return;
		}
		statisticKeyWithOutSomeExpression(alreadyElimateExpressName, node->getLeftChild(), result);
		statisticKeyWithOutSomeExpression(alreadyElimateExpressName, node->getRightChild(), result);
	}

	static void removeEqual(Node* node) {
	    if(node == NULL || node->getNodeTypeEnums() != NodeTypeEnums::INTERMEDIATE) {
	        return;
	    }

	    removeEqual(node->getLeftChild());
	    removeEqual(node->getRightChild());

	    if(node->getOperatorEnums() == OperatorEnums::EQUAL) {
            Node *newLeft = getNewNode(node->getLeftChild());
            NodeUtil::copyNode(node, newLeft);
            delete newLeft;
	    }
	}

	static void pushROTL(Node* node) {
	    if(node == NULL)
	        return;

	    if(node->getOperatorEnums() == OperatorEnums::ROTL ){

	        if(node->getLeftChild()->getOperatorEnums() == OperatorEnums::ROTL) {
	            int rotl = node->getRotl();
                Node *newLeft1 = getNewNode(node->getLeftChild());
                NodeUtil::copyNode(node, newLeft1);
                delete newLeft1;
                node->setRotl(node->getRotl() + rotl);

	        }

	        if(node->getLeftChild()->getOperatorEnums() == OperatorEnums::XOR) {

                int rotl = node->getRotl();

                Node *newLeft1 = getNewNode(node->getLeftChild());
                NodeUtil::copyNode(node, newLeft1);
                delete newLeft1;

                Node *newLeft = node->getLeftChild();
                Node *newRight = node->getRightChild();

                Node *nodeL = new Node("gpf", OperatorEnums::ROTL, NodeTypeEnums::NULLNODETYPE);
                nodeL->setRotl(rotl);
                node->setLeftChild(nodeL);
                nodeL->setLeftChild(newLeft);

                Node *nodeR = new Node("gpf", OperatorEnums::ROTL, NodeTypeEnums::NULLNODETYPE);
                nodeR->setRotl(rotl);
                node->setRightChild(nodeR);
                nodeR->setLeftChild(newRight);
            }

	    }


	    if(node->getOperatorEnums() == OperatorEnums::ROTL && node->getNodeTypeEnums() != NodeTypeEnums::INTERMEDIATE) {
	        int rotl = node->getRotl();
            Node *newLeft1 = getNewNode(node->getLeftChild());
            NodeUtil::copyNode(node, newLeft1);
            delete newLeft1;

            node->setName(node->getName() + "_rotl_" + to_string(rotl));
	    }

	    pushROTL(node->getLeftChild());
	    pushROTL(node->getRightChild());
	}




    static void remove2(Node* node, set<string>& tempR) {
        if(node == NULL)
            return;
        if(node->getNodeTypeEnums() != NodeTypeEnums::INTERMEDIATE)
            return;
        string leftChildName = node->getLeftChild()->getName();
        if(tempR.find(leftChildName) != tempR.end()) {
            Node *newRight = getNewNode(node->getRightChild());
            NodeUtil::copyNode(node, newRight);
            delete newRight;
            return;
        }
        
        if(node->getRightChild() != NULL) {
            string rightChildName = node->getRightChild()->getName();
            if(tempR.find(rightChildName) != tempR.end()) {
                Node *newLeft = getNewNode(node->getLeftChild());
                NodeUtil::copyNode(node, newLeft);
                delete newLeft;
                return;
            }
        }
        
        remove2(node->getLeftChild(), tempR);
        remove2(node->getRightChild(), tempR);
    }
    

    static void getSubNodeByName(Node* node, string subNodeName, Node* & result, bool & flag) {
        if(flag)
            return;
        if(node == NULL)
            return;
        if(node->getName() == subNodeName) {
            result = node;
            flag = true;
            return;
        }
        
        getSubNodeByName(node->getLeftChild(), subNodeName, result, flag);
        getSubNodeByName(node->getRightChild(), subNodeName, result, flag);

    }


	static void findParent(Node* node, string r, bool &flag, Node* result) {
	    if(flag)
	        return;
	    if(node == NULL)
	        return;
	    if(node->getLeftChild()->getName() == r) {
	        result = node->getLeftChild();
	        flag = true;
	    }
	    if(node->getRightChild()->getName() == r) {
	        result = node->getRightChild();
	        flag = true;
	    }

	    findParent(node->getLeftChild(), r, flag, result);
        findParent(node->getRightChild(), r, flag, result);

	}

	static void exploreNode(Node* node, vector<Node*>& nodes, map<string, set<string>> & variables) {
	    if(node == NULL)
	        return;
	    if(node->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE && node->getOperatorEnums() != OperatorEnums::XOR)
	        return;
	    if(node->getNodeTypeEnums() != NodeTypeEnums::INTERMEDIATE) {
	        if(node->getNodeTypeEnums() == NodeTypeEnums::MASK) {
	            if(variables["r"].count(node->getName()) == 0)
	                variables["r"].insert(node->getName());
	            else {
	                assert(false);
	            }
	        } else if(node->getNodeTypeEnums() == NodeTypeEnums::SECRECT) {
                if(variables["k"].count(node->getName()) == 0)
                    variables["k"].insert(node->getName());
                else
                    assert(false);
	        }
            return;
	    }

	    if(node->getOperatorEnums() == OperatorEnums::XOR) {
	        nodes.push_back(node);
            exploreNode(node->getLeftChild(), nodes, variables);
            exploreNode(node->getRightChild(), nodes, variables);
        }
	}

    /**
	static void removeZeroAndOne(Node *node) {
		if(node == NULL)
			return;

		removeZeroAndOne(node->getLeftChild());
		removeZeroAndOne(node->getRightChild());

		if(node->getOperatorEnums() == OperatorEnums::XOR && node->getLeftChild()->getName() == "1") {
			node->setOperatorEnums(OperatorEnums::NOT);
			node->setLeftChild(node->getRightChild());
			node->setRightChild(NULL);

		}else if (node->getOperatorEnums() == OperatorEnums::XOR && node->getRightChild()->getName() == "1") {
			node->setOperatorEnums(OperatorEnums::NOT);
			node->setRightChild(NULL);

		}else if (node->getOperatorEnums() == OperatorEnums::XOR && node->getLeftChild()->getName() == "0") {
			Node *right = node->getRightChild();
			NodeUtil::copyNode(node, right);
			delete right;

		}else if (node->getOperatorEnums() == OperatorEnums::XOR && node->getRightChild()->getName() == "0") {
			Node *left = node->getLeftChild();
			NodeUtil::copyNode(node, left);
			delete left;

		}else if (node->getOperatorEnums() == OperatorEnums::AND && node->getLeftChild()->getName() == "1") {
			Node *right = node->getRightChild();
			NodeUtil::copyNode(node, right);
			delete right;

		}else if (node->getOperatorEnums() == OperatorEnums::AND && node->getRightChild()->getName() == "1") {
		    Node *left = node->getLeftChild();
			NodeUtil::copyNode(node, left);
			delete left;

		}else if (node->getOperatorEnums() == OperatorEnums::AND && node->getLeftChild()->getName() == "0") {
		    node->setName("0");
			node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
			node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
			node->setLeftChild(NULL);
			node->setRightChild(NULL);

			mutate(*node, "0", 0);

		}else if (node->getOperatorEnums() == OperatorEnums::AND && node->getRightChild()->getName() == "0") {
		    node->setName("0");
			node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
			node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
			node->setLeftChild(NULL);
			node->setRightChild(NULL);

			mutate(*node, "0", 0);

		}else if (node->getOperatorEnums() == OperatorEnums::OR && node->getLeftChild()->getName() == "1") {
		    node->setName("1");
			node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
			node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
			node->setLeftChild(NULL);
			node->setRightChild(NULL);

			mutate(*node, "1", 1);

		}else if (node->getOperatorEnums() == OperatorEnums::OR && node->getRightChild()->getName() == "1") {
		    node->setName("1");
			node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
			node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
			node->setLeftChild(NULL);
			node->setRightChild(NULL);

			mutate(*node, "1", 1);

		}else if (node->getOperatorEnums() == OperatorEnums::OR && node->getLeftChild()->getName() == "0") {
			Node *right = node->getRightChild();
			NodeUtil::copyNode(node, right);
			delete right;

		}else if (node->getOperatorEnums() == OperatorEnums::OR && node->getRightChild()->getName() == "0") {
		    Node *left = node->getLeftChild();
			NodeUtil::copyNode(node, left);
			delete left;

		}else if (node->getOperatorEnums() == OperatorEnums::NOT && node->getLeftChild()->getName() == "0") {
		    node->setName("1");
			node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
			node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
			node->setLeftChild(NULL);
			node->setRightChild(NULL);

			mutate(*node, "1", 1);

		}else if (node->getOperatorEnums() == OperatorEnums::NOT && node->getLeftChild()->getName() == "1") {
		    node->setName("0");
			node->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
			node->setOperatorEnums(OperatorEnums::NULLOPERATOR);
			node->setLeftChild(NULL);
			node->setRightChild(NULL);

			mutate(*node, "0", 0);

		}


	}
    **/




	static bool hasChildrenUKD(Node* node, deque<Node* > UKDNds, int &count) {
		count++;
	    if(node == NULL)
	    	return false;
	    if(findUKD(node, UKDNds))
	    	return true;

	    if(hasChildrenUKD(node->getLeftChild(), UKDNds, count) ||
				hasChildrenUKD(node->getRightChild(), UKDNds, count))
	    	return true;

		return false;

	}

	static bool findUKD(Node* node, deque<Node* > UKDNds) {

		deque<Node* >::iterator pos;
		pos = find(UKDNds.begin(), UKDNds.end(), node);
		if(pos != UKDNds.end()) {
		    return true;
		}
		else {
		    return false;
		}

	}



	static void getAuxiliaryTable(Node *node) {
		if ((*node).getLeftChild() == NULL && (*node).getRightChild() == NULL) {
			if ((*node).getNodeTypeEnums() == NodeTypeEnums::MASK) {
				node->getSupportV()->insert(" " + node->getName() + " ");
				node->getUniqueM()->insert(" " + node->getName() + " ");
				node->getPerfectM()->insert(" " + node->getName() + " ");
				node->getDominant()->insert(" " + node->getName() + " ");
				return;
			}
			else {
				node->getSupportV()->insert(" " + node->getName() + " ");
				return;
			}
		}
		else {

			if ((*node).getLeftChild() != NULL) {
				if (node->getLeftChild()->getSupportV()->size() == 0) {
					getAuxiliaryTable((*node).getLeftChild());
				}
			}
			if ((*node).getRightChild() != NULL) {
				if (node->getRightChild()->getSupportV()->size() == 0) {
					getAuxiliaryTable((*node).getRightChild());
				}
			}

			if (node->getOperatorEnums() == OperatorEnums::NOT || node->getOperatorEnums() == OperatorEnums::EQUAL
					|| node->getOperatorEnums() == OperatorEnums::AFFINEF || node->getOperatorEnums() == OperatorEnums::RCON ||
					node->getOperatorEnums() == OperatorEnums::SBOX || node->getOperatorEnums() == OperatorEnums::ROTL
					|| node->getOperatorEnums() == OperatorEnums::TABLELUT) {
				node->setSupportV(node->getLeftChild()->getSupportV());
				node->setUniqueM(node->getLeftChild()->getUniqueM());
				node->setPerfectM(node->getLeftChild()->getPerfectM());
				node->setDominant(node->getLeftChild()->getDominant());
			} else if(node->getOperatorEnums() == OperatorEnums::POW) {
				node->setSupportV(node->getLeftChild()->getSupportV());
				node->setUniqueM(node->getLeftChild()->getUniqueM());
				node->setPerfectM(node->getLeftChild()->getPerfectM());
			}
			else {
				SetUtil::Union(*node->getLeftChild()->getSupportV(), *node->getRightChild()->getSupportV(), *node->getSupportV());
				shared_ptr<set<string>> temp1 = make_shared<set<string>>();
				shared_ptr<set<string>> temp2 = make_shared<set<string>>();
				SetUtil::Union(*node->getLeftChild()->getUniqueM(), *node->getRightChild()->getUniqueM(), *temp1);
				SetUtil::Intersection(*node->getLeftChild()->getSupportV(), *node->getRightChild()->getSupportV(), *temp2);
				SetUtil::Difference(*temp1, *temp2, *node->getUniqueM());
				if (node->getOperatorEnums() == OperatorEnums::XOR || node->getOperatorEnums() == OperatorEnums::ADD
				|| node->getOperatorEnums() == OperatorEnums::MINU ||
				    (node->getOperatorEnums() == OperatorEnums::MUL && (node->getLeftChild()->getNodeTypeEnums() == NodeTypeEnums ::CONSTANT||
				    node->getRightChild()->getNodeTypeEnums() == NodeTypeEnums::CONSTANT))) {
					shared_ptr<set<string>> temp3 = make_shared<set<string>>();
					SetUtil::Union(*node->getLeftChild()->getPerfectM(), *node->getRightChild()->getPerfectM(), *temp3);
					SetUtil::Intersection(*node->getUniqueM(), *temp3, *node->getPerfectM());

					shared_ptr<set<string>> temp5 = make_shared<set<string>>();
					SetUtil::Union(*node->getLeftChild()->getDominant(), *node->getRightChild()->getDominant(), *temp5);
					SetUtil::Intersection(*temp5, *node->getUniqueM(), *node->getDominant());
				} else {
				    node->getDominant()->clear();
				}
			}

		}
	}


	static void emptySupport(Node* node) {
	    if(node == NULL)
	        return;
	    node->getUniqueM()->clear();
	    node->getSupportV()->clear();
	    node->getDominant()->clear();
	    node->getPerfectM()->clear();
	    emptySupport(node->getLeftChild());
        emptySupport(node->getRightChild());

	}

	static void getNodeMap(Node* node, map<string, Node*>& nodeMap) {
	    if(node == NULL) {
	        return;
	    }
		nodeMap[node->getName()] = node;
	    getNodeMap(node->getLeftChild(), nodeMap);
	    getNodeMap(node->getRightChild(), nodeMap);

	}


	static int getHeight(Node* node) {
		if(node == NULL) {
			return 0;
		}
		return max(getHeight(node->getLeftChild()), getHeight(node->getRightChild())) + 1;
	}

	static void printTree(Node* node, int indent) {
		if(node == NULL)
			return;
		std::cout << regularStringInTree(node, indent) << "\n";
		printTree(node->getLeftChild(), indent + 2);
		printTree(node->getRightChild(), indent + 2);
	}

	static void printExpression(Node* node, string &res) {
		if(node == NULL)
			return;
	    printExpression(node->getLeftChild(), res);
	    if(node->getNodeTypeEnums() != NodeTypeEnums::INTERMEDIATE) {
	    	res += node->getName() + " ";
	    }
	    if(node->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
	        if(node->getOperatorEnums() == OperatorEnums::NOT || node->getOperatorEnums() == OperatorEnums::ROTL)
	            res = EnumUtil::operationToString(node->getOperatorEnums()) + " " + res;
	        else if(node->getOperatorEnums() != OperatorEnums::EQUAL)
	            res += EnumUtil::operationToString(node->getOperatorEnums()) + " ";
	    }
	    if(node->getRightChild() != NULL) {
            printExpression(node->getRightChild(), res);
        }

	}


    static void printExpression2(Node* node, string & re) {
        if(node == NULL)
            return;
        if(node->getNodeTypeEnums() != NodeTypeEnums::INTERMEDIATE) {
            re += node->getName() + " ";
            cout << node->getName() + " ";
            return;
        }
        if(node->getOperatorEnums() == OperatorEnums::NOT ||
           node->getOperatorEnums() == OperatorEnums::ROTL) {

            cout << "( ";
            cout << EnumUtil::operationToString(node->getOperatorEnums()) + " ";
            re += "( ";
            re += EnumUtil::operationToString(node->getOperatorEnums()) + " ";
            re += "( ";
            printExpression2(node->getLeftChild(), re);
            re += " )";
            cout << " )";
            re += " )";
        } else if(node->getOperatorEnums() == OperatorEnums::EQUAL) {
            cout << " ";
            re += " ";
            printExpression2(node->getLeftChild(),re);
            re += " ";
            cout << " ";

        } else {
            cout << " ";
            re += " ";
            printExpression2(node->getLeftChild(), re);
            cout << EnumUtil::operationToString(node->getOperatorEnums()) + " ";
            re += EnumUtil::operationToString(node->getOperatorEnums()) + " ";
            printExpression2(node->getRightChild(), re);
            cout << " ";
            re += " ";
        }

    }

	static string regularStringInTree(Node* node, int indent) {
		string s = "";
		for(int i = 0; i < indent; i++)
			s += "-";
		s += node->getName();
		if(node->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
			switch(node->getOperatorEnums()) {
				case OperatorEnums::XOR:
					s += "(XOR)";
					break;
				case OperatorEnums::OR:
					s += "(OR)";
					break;
				case OperatorEnums::AND:
					s += "(AND)";
					break;
				case OperatorEnums::NOT:
					s += "(NOT)";
					break;
				case OperatorEnums ::GFMUL:
					s += "(gfmul)";
					break;
				case OperatorEnums ::MINU:
					s += "(MINU)";
					break;
				case OperatorEnums ::EQUAL:
					s += "(=)";
					break;
				case OperatorEnums ::ROTL:
					s += "(ROTL)";
					break;
			    case OperatorEnums ::POW:
			        s += "(Pow" + to_string(node->getPow()) + ")";
			        break;
				default:
					s += "(D)";
			}
		} else {
			switch(node->getNodeTypeEnums()) {
				case NodeTypeEnums ::MASK:
					s += "(Random)";
					break;
				case NodeTypeEnums ::SECRECT:
					s += "(Key)";
					break;
				case NodeTypeEnums ::CONSTANT:
					s += "Constant";
                    break;
                case NodeTypeEnums::PLAIN:
                    s += "PLAIN";
				default:
					s += "(what node type?)";
					break;
			}

		}
		return s;
	}

	static void traverse(string name, Node* node, set<string>& xorAndOr, set<string>& And, set<string>& Not) {
		if(node == NULL)
			return;
		traverse(name, node->getLeftChild(), xorAndOr, And, Not);

		if(node->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
            if((node->getOperatorEnums() == OperatorEnums::XOR || node->getOperatorEnums() == OperatorEnums::OR) && (node->getLeftChild()->getName() == name || node->getRightChild()->getName() == name) ) {
                xorAndOr.insert(node->getName());
            } else if((node->getOperatorEnums() == OperatorEnums::AND) && (node->getLeftChild()->getName() == name
               || node->getRightChild()->getName() == name)
                  ){
                And.insert(node->getName());
            } else if(node->getLeftChild()->getName() == name && node->getOperatorEnums() == OperatorEnums::NOT) {
                Not.insert(node->getName());
            }
		}
		traverse(name, node->getRightChild(), xorAndOr, And, Not);
	}

	static void traverse2(string name, Node* node, set<string>& Xor, set<string>& Or, set<string>& And, set<string>& Not) {
		if(node == NULL)
			return;
		traverse2(name, node->getLeftChild(),Xor, Or, And, Not);
		if(node->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
		    if(node->getOperatorEnums() == OperatorEnums::XOR &&
			   (node->getLeftChild()->getName() == name || node->getRightChild()->getName() == name)) {
		    	Xor.insert(node->getName());
		    } else if(node->getOperatorEnums() == OperatorEnums::OR &&
			   (node->getLeftChild()->getName() == name || node->getRightChild()->getName() == name)) {
		    	Or.insert(node->getName());
		    } else if((node->getOperatorEnums() == OperatorEnums::AND) &&
					  (node->getLeftChild()->getName() == name ||
					   node->getRightChild()->getName() == name)
					){
				And.insert(node->getName());
			} else if(node->getLeftChild()->getName() == name &&
					  node->getOperatorEnums() == OperatorEnums::NOT) {
				Not.insert(node->getName());
			}
		}
		traverse2(name, node->getRightChild(), Xor, Or, And, Not);
	}

	static void traverseBv(string name, Node* node, vector<Node*>& XorAdd, vector<Node*>& AndMulGfmulRshLsh, vector<Node*>& Or, vector<Node*>& Not) {
		if(node == NULL)
			return;
		traverseBv(name, node->getLeftChild(),XorAdd, AndMulGfmulRshLsh, Or, Not);
		if(node->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
		    if((node->getOperatorEnums() == OperatorEnums::XOR ||
                node->getOperatorEnums() == OperatorEnums::ADD) &&
			   (node->getLeftChild()->getName() == name || node->getRightChild()->getName() == name)) {
		    	XorAdd.push_back(node);
		    } else if((node->getOperatorEnums() == OperatorEnums::AND ||
                       node->getOperatorEnums() == OperatorEnums::MUL ||
                       node->getOperatorEnums() == OperatorEnums::GFMUL ||
                       node->getOperatorEnums() == OperatorEnums::RSH ||
                       node->getOperatorEnums() == OperatorEnums::LSH ) &&
			            (node->getLeftChild()->getName() == name || node->getRightChild()->getName() == name)) {
		    	AndMulGfmulRshLsh.push_back(node);
		    } else if((node->getOperatorEnums() == OperatorEnums::OR) &&
					  (node->getLeftChild()->getName() == name ||
					   node->getRightChild()->getName() == name)
					){


				Or.push_back(node);
			} else if(node->getLeftChild()->getName() == name &&
					  node->getOperatorEnums() == OperatorEnums::NOT) {


				Not.push_back(node);
			}
		}
        traverseBv(name, node->getRightChild(),XorAdd, AndMulGfmulRshLsh, Or, Not);
	}

	static void traverse3(string name, Node* node, set<string>& all) {
		if(node == NULL)
			return;
		traverse3(name, node->getLeftChild(), all);
		if(node->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
			if(node->getOperatorEnums() == OperatorEnums::NOT) {
				if(node->getLeftChild()->getName() == name)
				    all.insert(node->getName());
			} else {
			    if(node->getLeftChild()->getName() == name ||(node->getRightChild() != NULL && node->getRightChild()->getName() == name) )
			        all.insert(node->getName());
			}
		}
		traverse3(name, node->getRightChild(), all);
	}





	static void mutate(Node& node, string n, bool b) {
		map<string, Node*> nodeMap;
		getNodeMap(&node, nodeMap);

		set<string> parentAll;
		NodeUtil::traverse3(n, &node, parentAll);
		vector<string> parentAllVec;
		SetUtil::SetToVector(parentAll, parentAllVec);

		while(parentAllVec.size() != 0) {

			for (int i = 0; i < parentAllVec.size(); i++) {
				while (nodeMap[parentAllVec[i]] != NULL) {
//					NodeUtil::printTree(&node, 0);
					nodeMap.clear();
					getNodeMap(&node, nodeMap);
					Node *l = nodeMap[parentAllVec[i]];

					// Xor
					if (l->getOperatorEnums() == OperatorEnums::XOR &&
						l->getLeftChild()->getName() == n) {
						if (b) {
							l->setOperatorEnums(OperatorEnums::NOT);
							l->setLeftChild(l->getRightChild());
							l->setRightChild(NULL);
						} else {
							Node *rightChild = l->getRightChild();
							NodeUtil::copyNode(l, rightChild);
							delete rightChild;
						}
					} else if (l->getOperatorEnums() == OperatorEnums::XOR &&
							   l->getRightChild()->getName() == n
							) {

						if (b) {
							l->setOperatorEnums(OperatorEnums::NOT);
							l->setRightChild(NULL);
						} else {
							Node *leftChild = l->getLeftChild();
							NodeUtil::copyNode(l, leftChild);
							delete leftChild;

						}
						// OR
					} else if (l->getOperatorEnums() == OperatorEnums::OR) {
						if (b) {
							l->setName("1");
							l->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
							l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
							l->setLeftChild(NULL);
							l->setRightChild(NULL);

							mutate(node, "1", 1);

						} else {
							if (l->getLeftChild()->getName() == n) {
								Node *rightChild = l->getRightChild();
								NodeUtil::copyNode(l, rightChild);
								delete rightChild;

							} else {
								Node *leftChild = l->getLeftChild();
								NodeUtil::copyNode(l, leftChild);
								delete leftChild;

							}
						}
						//And
					} else if (l->getOperatorEnums() == OperatorEnums::AND) {
						if (!b) {
							l->setName("0");
							l->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
							l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
							l->setLeftChild(NULL);
							l->setRightChild(NULL);

							mutate(node, "0", 0);
						} else {
							if (l->getLeftChild()->getName() == n) {
								Node *rightChild = l->getRightChild();
								NodeUtil::copyNode(l, rightChild);
								delete rightChild;

							} else {
								Node *leftChild = l->getLeftChild();
								NodeUtil::copyNode(l, leftChild);
								delete leftChild;

							}
						}
					} else {
						if (b) {
							l->setName("0");
						} else {
							l->setName("1");
						}
						l->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
						l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
						l->setLeftChild(NULL);
						l->setRightChild(NULL);

						if (b) {
							mutate(node, "0", 0);
						} else {
							mutate(node, "1", 1);
						}

					}
					nodeMap.clear();
					getNodeMap(&node, nodeMap);

				}


			}

			parentAllVec.clear();
			parentAll.clear();
			NodeUtil::traverse3(n, &node, parentAll);
			SetUtil::SetToVector(parentAll, parentAllVec);

		}
	}



	static void mutateBv(Node* node, string n, bool b) {
		map<string, Node*> nodeMap;
		getNodeMap(node, nodeMap);

		set<string> parentAll;
		NodeUtil::traverse3(n, node, parentAll);
		vector<string> parentAllVec;
		SetUtil::SetToVector(parentAll, parentAllVec);

		while(parentAllVec.size() != 0) {

			for (int i = 0; i < parentAllVec.size(); i++) {
				while (nodeMap[parentAllVec[i]] != NULL) {
					nodeMap.clear();
					getNodeMap(node, nodeMap);
					Node *l = nodeMap[parentAllVec[i]];

					if ((l->getOperatorEnums() == OperatorEnums::XOR || l->getOperatorEnums() == OperatorEnums::ADD) &&
						l->getLeftChild()->getName() == n) {
						if (b) {
						    if(l->getOperatorEnums() == OperatorEnums::XOR) {
                                l->setOperatorEnums(OperatorEnums::NOT);
                                l->setLeftChild(l->getRightChild());
                                l->setRightChild(NULL);
                            } else if(l->getOperatorEnums() == OperatorEnums::ADD) {
						        l->getLeftChild()->setName("1");
                                l->getLeftChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                                l->getLeftChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                                l->getLeftChild()->setLeftChild(NULL);
                                l->getLeftChild()->setRightChild(NULL);
						    }
						} else {
							Node *rightChild = l->getRightChild();
							NodeUtil::copyNode(l, rightChild);
							delete rightChild;
						}
					} else if ((l->getOperatorEnums() == OperatorEnums::XOR || l->getOperatorEnums() == OperatorEnums::ADD) &&
							   l->getRightChild()->getName() == n
							) {

						if (b) {

                            if(l->getOperatorEnums() == OperatorEnums::XOR) {
                                l->setOperatorEnums(OperatorEnums::NOT);
                                l->setRightChild(NULL);
                            } else if(l->getOperatorEnums() == OperatorEnums::ADD) {
                                l->getRightChild()->setName("1");
                                l->getRightChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                                l->getRightChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                                l->getRightChild()->setLeftChild(NULL);
                                l->getRightChild()->setRightChild(NULL);
                            }
						} else {
							Node *leftChild = l->getLeftChild();
							NodeUtil::copyNode(l, leftChild);
							delete leftChild;

						}
					} else if (l->getOperatorEnums() == OperatorEnums::OR) {
						if (b) {
							l->setName("1");
							l->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
							l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
							l->setLeftChild(NULL);
							l->setRightChild(NULL);
						} else {
							if (l->getLeftChild()->getName() == n) {
								Node *rightChild = l->getRightChild();
								NodeUtil::copyNode(l, rightChild);
								delete rightChild;

							} else {
								Node *leftChild = l->getLeftChild();
								NodeUtil::copyNode(l, leftChild);
								delete leftChild;
							}
						}
					} else if (l->getOperatorEnums() == OperatorEnums::AND ||
                            l->getOperatorEnums() == OperatorEnums::MUL ||
                            l->getOperatorEnums() == OperatorEnums::GFMUL ||
                            l->getOperatorEnums() == OperatorEnums::RSH ||
                            l->getOperatorEnums() == OperatorEnums::LSH ) {
						if (!b) {
							l->setName("0");
							l->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
							l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
							l->setLeftChild(NULL);
							l->setRightChild(NULL);
						} else {
							if (l->getLeftChild()->getName() == n) {
							    l->getLeftChild()->setName("1");
                                l->getLeftChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                                l->getLeftChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                                l->getLeftChild()->setLeftChild(NULL);
                                l->getLeftChild()->setRightChild(NULL);

							} else {
                                l->getRightChild()->setName("1");
                                l->getRightChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                                l->getRightChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                                l->getRightChild()->setLeftChild(NULL);
                                l->getRightChild()->setRightChild(NULL);

							}
						}
					} else if (l->getOperatorEnums() == OperatorEnums::NOT){
						if (b) {
							l->setName("0");
						} else {
							l->setName("1");
						}
						l->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
						l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
						l->setLeftChild(NULL);
						l->setRightChild(NULL);

						if (b) {
							mutateBv(node, "0", 0);
						} else {
							mutateBv(node, "1", 1);
						}

					} else {
					    assert(false);
					}
					nodeMap.clear();
					getNodeMap(node, nodeMap);

				}


			}

			parentAllVec.clear();
			parentAll.clear();
			NodeUtil::traverse3(n, node, parentAll);
			SetUtil::SetToVector(parentAll, parentAllVec);

		}
	}

    static void mutateBv1(Node* node, string n, bool b) {
        map<string, Node*> nodeMap;
        NodeUtil::getNodeMap(node, nodeMap);

        vector<Node*> parentWithXorAddV;
        vector<Node*> parentWithOrV;
        vector<Node*> parentWithAndMulV;
        vector<Node*> parentWithNotV;

        NodeUtil::traverseBv(n, node, parentWithXorAddV, parentWithAndMulV, parentWithOrV,
                             parentWithNotV);

        for(int i = 0; i < parentWithXorAddV.size(); i++) {
            Node *l = parentWithXorAddV[i];
            if(l == NULL) continue;
            if (l->getLeftChild()->getName() == n) {
                if (b) {
                    if(l->getOperatorEnums() == OperatorEnums::XOR) {
                        l->setOperatorEnums(OperatorEnums::NOT);
                        l->setLeftChild(l->getRightChild());
                        l->setRightChild(NULL);
                    } else if(l->getOperatorEnums() == OperatorEnums::ADD) {
                        l->getLeftChild()->setName("1");
                        l->getLeftChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                        l->getLeftChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                        l->getLeftChild()->setLeftChild(NULL);
                        l->getLeftChild()->setRightChild(NULL);
                    }
                } else {
                    Node *rightChild = l->getRightChild();
                    NodeUtil::copyNode(l, rightChild);
                    delete rightChild;
                }
            } else if (l->getRightChild()->getName() == n) {
                if (b) {

                    if(l->getOperatorEnums() == OperatorEnums::XOR) {
                        l->setOperatorEnums(OperatorEnums::NOT);
                        l->setRightChild(NULL);
                    } else if(l->getOperatorEnums() == OperatorEnums::ADD) {
                        l->getRightChild()->setName("1");
                        l->getRightChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                        l->getRightChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                        l->getRightChild()->setLeftChild(NULL);
                        l->getRightChild()->setRightChild(NULL);
                    }
                } else {
                    Node *leftChild = l->getLeftChild();
                    NodeUtil::copyNode(l, leftChild);
                    delete leftChild;

                }
            }

        }

        for(int i = 0; i < parentWithAndMulV.size(); i++) {
            Node *l = parentWithAndMulV[i];
            if(l == NULL) continue;
            if (!b) {
                l->setName(n);
                l->setNodeTypeEnums(NodeTypeEnums::MASK);
                l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                l->setLeftChild(NULL);
                l->setRightChild(NULL);
                mutateBv1(node, n, 0);
            } else {
                if (l->getLeftChild()->getName() == n) {
                    l->getLeftChild()->setName("1");
                    l->getLeftChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                    l->getLeftChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                    l->getLeftChild()->setLeftChild(NULL);
                    l->getLeftChild()->setRightChild(NULL);

                } else {
                    l->getRightChild()->setName("1");
                    l->getRightChild()->setNodeTypeEnums(NodeTypeEnums::CONSTANT);
                    l->getRightChild()->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                    l->getRightChild()->setLeftChild(NULL);
                    l->getRightChild()->setRightChild(NULL);

                }
            }


        }
        for(int i = 0; i < parentWithOrV.size(); i++) {
            Node *l = parentWithOrV[i];
            if(l == NULL) continue;
            if (b) {
                l->setName(n);
                l->setNodeTypeEnums(NodeTypeEnums::MASK);
                l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
                l->setLeftChild(NULL);
                l->setRightChild(NULL);
                mutateBv1(node, n, 1);
            } else {
                if (l->getLeftChild()->getName() == n) {
                    Node *rightChild = l->getRightChild();
                    NodeUtil::copyNode(l, rightChild);
                    delete rightChild;

                } else {
                    Node *leftChild = l->getLeftChild();
                    NodeUtil::copyNode(l, leftChild);
                    delete leftChild;
                }
            }


        }
        for(int i = 0; i < parentWithNotV.size(); i++) {
            Node *l = parentWithNotV[i];
            l->setName(n);
            l->setNodeTypeEnums(NodeTypeEnums::MASK);
            l->setOperatorEnums(OperatorEnums::NULLOPERATOR);
            l->setLeftChild(NULL);
            l->setRightChild(NULL);
            mutateBv1(node, n, !b);
        }


    }

	static Node* getNewNode(Node* tempNode) {
		if(tempNode == NULL)
			return NULL;

	    Node* temp = new Node();
		NodeUtil::nodeCopy(temp, tempNode);
		return temp;
	}

	static void printNodeMap(map<string, Node> nodemap) {
		map<string, Node>::iterator it;
		it = nodemap.begin();
		while(it != nodemap.end()) {
		    cout << "Name: " << it->first << endl;
		    if(it->second.getSemd() != NULL) {
				cout << "random variable: ";
				SetUtil::put_out(*(it->second).getSemd());
			}
			if(it->second.getSimpliedNode() != NULL) {
				cout << "simplfied tree " << endl;
		        NodeUtil::printTree((it->second).getSimpliedNode(), 0);
			}
			it++;
		}
	}



	static void getRidOf(Node* node, map<string, Node*>& nodeMap) {
		if(node == NULL)
			return;
		getRidOf(node->getLeftChild(), nodeMap);
		if(node->getNodeTypeEnums() == NodeTypeEnums::MASK) {
			node = nodeMap[node->getName()];
		}
		getRidOf(node->getRightChild(), nodeMap);

	}

	static Node* findR2(Node* node, string randomName) {
		if(node != NULL && node->getName() == randomName)
			return node;
		Node* left = NULL;
		Node* right = NULL;
		if(node != NULL) {
			left = findR2(node->getLeftChild(), randomName);
			right = findR2(node->getRightChild(), randomName);
		}

		if(left != NULL)
			return left;
		else if(right!= NULL)
			return right;
		else
			return NULL;
	}

	static void getDominatedExpressions(Node* current, Node* &name, set<string>& dominatedExpresstionNames, bool &flag) {
		if(flag)
			return;
		if(current->getNodeTypeEnums() != NodeTypeEnums::INTERMEDIATE)
			return;
		if(current->getDominant()->size() > 0 && !SetUtil::containValue(dominatedExpresstionNames, current->getName())) {
		    name = current;
		    dominatedExpresstionNames.insert(current->getName());
		    flag = true;
		    return;
		}
		if(current->getLeftChild() != NULL)
			getDominatedExpressions(current->getLeftChild(), name, dominatedExpresstionNames, flag);
		if(current->getRightChild() != NULL)
			getDominatedExpressions(current->getRightChild(), name, dominatedExpresstionNames, flag);
	}

	static void getANodeNum(Node* node, string name, int & result) {
		if(node == NULL)
			return;
		if(node->getName() == name) {
			result++;
			return;
		}
		if(node->getRightChild() != NULL) {
			getANodeNum(node->getRightChild(), name, result);
		}
		if(node->getLeftChild() != NULL) {
			getANodeNum(node->getLeftChild(), name, result);
		}
	}

	static int numberOfANodeInSet(string name, set<Node*>& subset) {
		set<Node*>::iterator it;
		int result = 0;
		for(it = subset.begin(); it != subset.end(); it++) {
			int re = 0;
			getANodeNum(*it, name, re);
			result += re;
		}
		return result;
	}

	static void clear(Node *n) {
	    if(n != NULL) {
            clear(n->getLeftChild());
            clear(n->getRightChild());
        }
        delete n;
	}

	static Node* parser(list<string> *s, string K, string R)
    {
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

};

