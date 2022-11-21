#pragma once
#include <iostream>
#include <string>
#include <set>
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "OperatorEnums.h"
#include "util/EnumUtil.h"
#include <memory>
using namespace std;

class Node {

private:
	string name;
	DistributionEnums distribution;
	OperatorEnums operation;
	NodeTypeEnums type;

	shared_ptr<set<string>> dependences;  //structural dependency
	shared_ptr<set<string>> dominant;     //Dom set

	shared_ptr<set<string>> supportV;
	shared_ptr<set<string>> uniqueM;
	shared_ptr<set<string>> perfectM;

	shared_ptr<set<string>> parentNodeNames;
	shared_ptr<set<string>> semd;


	int pow;
	int lut;
	int rotl;
	int tablelut;

	double qms;

	Node *parent;
	Node *leftChild;
	Node *rightChild;
	Node *simpliedNode;

public:
	Node() {
		this->name = "default";
		dependences = make_shared<set<string>>();
		dominant = make_shared<set<string>>();

		supportV = make_shared<set<string>>();
		uniqueM = make_shared<set<string>>();
		perfectM = make_shared<set<string>>();

        parentNodeNames = make_shared<set<string>>();

	}
	~Node() {

	}
	Node(string nodeName, OperatorEnums nodeOp, NodeTypeEnums nodeType) {
		name = nodeName;
		distribution=DistributionEnums ::NULLDISTRIBUTION;
		operation = nodeOp;
		type = nodeType;


		dependences = make_shared<set<string>>();
		dominant = make_shared<set<string>>();

		supportV = make_shared<set<string>>();
		uniqueM = make_shared<set<string>>();
		perfectM = make_shared<set<string>>();

		parentNodeNames = make_shared<set<string>>();


        semd = NULL;

		parent = NULL;
		leftChild = NULL;
		rightChild = NULL;
		simpliedNode = NULL;
	}

	int getPow(){
		return pow;
	}

	void setPow(int p){
		pow=p;
	}

	int getTablelut(){
		return tablelut;
	}

	void setTablelut(int p){
		tablelut=p;
	}

	int getRotl(){
		return rotl;
	}

	void setRotl(int p){
		rotl=p;
	}

	int getLut(){
		return lut;
	}

	void setLut(int l){
		lut=l;
	}

	string getName() {
		return name;
	}

	double getQMS() {
		return qms;
	}

	void setQMS(double qms) {
		this->qms = qms;
	}


	void setName(string nodeName){
		name = nodeName;
	}

	DistributionEnums getDistributionEnums() {
		return distribution;
	}

	void setDistributionEnums(DistributionEnums dis) {
		distribution = dis;
	}

	OperatorEnums getOperatorEnums() {
		return operation;
	}

	void setOperatorEnums(OperatorEnums op) {
		operation = op;
	}

	NodeTypeEnums getNodeTypeEnums() {
		return  type;
	}

	void setNodeTypeEnums(NodeTypeEnums t) {
		type = t;
	}

	shared_ptr<set<string>> getDependences() {
		return dependences;
	}

	void setDependences(shared_ptr<set<string>> dep) {
	    dependences = dep;
	}

	shared_ptr<set<string>> getDominant() {
		return dominant;
	}

	void setDominant(shared_ptr<set<string>> dom) {
		dominant = dom;
	}


	shared_ptr<set<string>> getSupportV() {
		return supportV;
	}


	void setSupportV(shared_ptr<set<string>> SupportV) {
		supportV = SupportV;
	}

	shared_ptr<set<string>> getUniqueM() {
		return uniqueM;
	}

	void setUniqueM(shared_ptr<set<string>> UniqueM) {
		uniqueM = UniqueM;
	}

	shared_ptr<set<string>> getPerfectM() {
		return perfectM;
	}

	void setPerfectM(shared_ptr<set<string>> PerfectM) {
		perfectM = PerfectM;
	}





	shared_ptr<set<string>> getParentNodeNames() {
		return parentNodeNames;
	}


	void setParentNodeNames(shared_ptr<set<string>> parentNames) {
		parentNodeNames = parentNames;
	}

	shared_ptr<set<string>> getSemd() {
		return semd;
	}


	void setSemd(shared_ptr<set<string>> semd) {
		this->semd = semd;
	}



	Node* getParent() {
		return parent;
	}

	void setParent(Node* node) {
		parent = node;
	}

	Node* getLeftChild() {
		return leftChild;
	}

	void setLeftChild(Node* node) {
		leftChild = node;
	}

	Node *getRightChild() {
		return rightChild;
	}

	void setRightChild(Node* node) {
		rightChild = node;
	}

	Node *getSimpliedNode() {
		return simpliedNode;
	}

	void setSimpliedNode(Node* node) {
		simpliedNode = node;
	}



	static string setToString(set<string> s) {
		string str = "";
		set<string>::iterator iter = s.begin();

		str.append("[");
		while (iter != s.end())
		{
			str.append(*iter + ", ");
			++iter;
		}
		str.append("]");
		return str;
	}
;

	string toString() {
		string s = "";
		s.append("Node [name=" + name);
		s.append(", distribution=" + EnumUtil::distributionToString(distribution));
		s.append(", operator=" + EnumUtil::operationToString(operation));
		s.append(", type=" + EnumUtil::typeToString(type));
		s.append(", dependences=" + setToString(*dependences));
		s.append(", dominant=" + setToString(*dominant));
		s.append(", supportV =" + setToString(*supportV));
		s.append(", uniqueM=" + setToString(*uniqueM));
		s.append(", perfectM=" + setToString(*perfectM));
		if (parent != NULL) {
			s.append(", parent=" + (*parent).toString());
		}
		else {
			s.append(", parent=null" );
		}
		if (leftChild != NULL) {
			s.append(", leftChild=" + (*leftChild).toString());
		}
		else {
			s.append(", leftChild=null");
		}
		if (rightChild != NULL) {
			s.append(", rightChild=" + (*rightChild).toString());
		}
		else {
			s.append(", rightChild=null");
		}

	
		s.append("]");
		return s;
	}

	static void printTreeAtNode(Node n) {
		cout << n.getName() << endl;
		cout << n.toString() << endl;
		if (n.leftChild != NULL) {	
			printTreeAtNode(*n.leftChild);
		}

		if (n.rightChild != NULL) {
			printTreeAtNode(*n.rightChild);
		}
	}


	static void getRandSet(Node& n, set<string> &rand) {
		if (n.getNodeTypeEnums() == NodeTypeEnums::MASK) {
			rand.insert(" " + n.getName() + " ");
		}
		if (n.leftChild != NULL) {
			getRandSet(*n.leftChild, rand);
		}

		if (n.rightChild != NULL) {
			getRandSet(*n.rightChild, rand);
		}
	}

	static bool hasKey(Node n) {
		if (n.getNodeTypeEnums() == NodeTypeEnums::SECRECT) {
		    return true;
		}

		if (hasKey(*(n.getLeftChild()))) {
		    return true;
		}

		if (hasKey(*(n.getRightChild()))) {
		    return true;
		}

		return false;
	}

	static void getRandSetWithNoSpace(Node n, set<string> &rand) {
		if (n.getNodeTypeEnums() == NodeTypeEnums::MASK) {
			rand.insert(n.getName());
		}
		if (n.leftChild != NULL) {
			getRandSetWithNoSpace(*n.leftChild, rand);
		}

		if (n.rightChild != NULL) {
			getRandSetWithNoSpace(*n.rightChild, rand);
		}
	}
	
	static void getSecretSet(Node& n, set<string> &rand) {
		if (n.getNodeTypeEnums() == NodeTypeEnums::SECRECT) {
			rand.insert(" "+n.getName()+" ");
		}
		if (n.leftChild != NULL) {
			getSecretSet(*n.leftChild, rand);
		}

		if (n.rightChild != NULL) {
			getSecretSet(*n.rightChild, rand);
		}
	}

	static void getPlainSet(Node& n, set<string> &plain) {
		if (n.getNodeTypeEnums() == NodeTypeEnums::PLAIN) {
			plain.insert(" " + n.getName() + " ");
		}
		if (n.leftChild != NULL) {
			getPlainSet(*n.leftChild, plain);
		}

		if (n.rightChild != NULL) {
			getPlainSet(*n.rightChild, plain);
		}
	}

};