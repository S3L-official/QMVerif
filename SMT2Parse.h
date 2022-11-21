#pragma once
#include <iostream>
#include <stack>
#include <string>
#include <set>
#include <sstream>
#include <math.h>
#include <fstream>
#include <map>
#include "Node.h"
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "OperatorEnums.h"
#include "util/EnumUtil.h"
#include "util/SetUtil.h"
#include "util/StringUtil.h"
#include "util/NodeUtil.h"
#include "z3++.h"
using namespace std;

class SMT2Parse {
public:

	static string getFirstString(Node n) {
		string s="";
		string leftName, rightName;
		if (n.getLeftChild() != NULL) {
			leftName = n.getLeftChild()->getName();
		}
		if (n.getRightChild() != NULL) {
			rightName = n.getRightChild()->getName();
		}


		switch(n.getOperatorEnums()) {
			case OperatorEnums ::AND:
				s.append("( bvand  " + leftName + "  " + rightName + "  )  ");
				break;
			case OperatorEnums ::XOR:
				s.append("( bvxor  " + leftName + "  " + rightName + "  )  ");
				break;
			case OperatorEnums ::OR:
				s.append("( bvor  " + leftName + "  " + rightName + "  )  ");
				break;
			case OperatorEnums ::NOT:
				s.append("( bvnot  " + leftName + "  " + rightName + "  )  ");
				break;
			case OperatorEnums ::ADD:
				s.append("( bvadd  " + leftName + "  " + rightName + "  )  ");
				break;
			case OperatorEnums ::MINU:
				s.append("( bvsub  " + leftName + "  " + rightName + "  )  ");
				break;
			case OperatorEnums::MUL:
				s.append("( bvmul  " + leftName + "  " + rightName + "  )  ");
				break;
			case OperatorEnums::RSH:
				s.append("( bvlshr  " + leftName + "  " + rightName + "  )  ");
				break;
			case OperatorEnums::LSH:
				s.append("( bvlshl  " + leftName + "  " + rightName + "  )  ");
				break;
			case OperatorEnums::GFMUL:
				s.append("( bvgfmul " + leftName + "  " + rightName + "  )  ");
				break;
			case OperatorEnums ::POW:
			    s.append("( pow" + to_string(n.getPow()) + " " + leftName + " ) ");
				break;
		    case OperatorEnums ::ROTL:
		        s.append("((_ rotate_left " + to_string(n.getRotl()) + ")" + " " + leftName + " ) ");
                break;
			default:
				s.append("  " + leftName + "  " + rightName + "  ");
		}

		return s;
	}

    static string getFirstStringBool(Node n) {
        string s="";
        string leftName, rightName;
        if (n.getLeftChild() != NULL) {
            leftName = n.getLeftChild()->getName();
        }
        if (n.getRightChild() != NULL) {
            rightName = n.getRightChild()->getName();
        }


        if (n.getOperatorEnums() == OperatorEnums::AND) {
            s.append("( and  " + leftName + "  " + rightName + "  )");
        }else if (n.getOperatorEnums() == OperatorEnums::OR) {
            s.append("( or  " + leftName + "  " + rightName + "  )");
        }else if (n.getOperatorEnums() == OperatorEnums::XOR) {
            s.append("( xor  " + leftName + "  " + rightName + "  )");
        }else if (n.getOperatorEnums() == OperatorEnums::NOT) {
            s.append("( not  " + leftName + "  " + rightName + "  )");
        }
        else {
            s.append("  " + leftName + "  " + rightName + "  ");
        }
        return s;
    }
    static string parseNodeBool(Node n) {
        string s = getFirstStringBool(n);
        if (n.getNodeTypeEnums() == NodeTypeEnums::MASK ||
            n.getNodeTypeEnums() == NodeTypeEnums::SECRECT ||
            n.getNodeTypeEnums()==NodeTypeEnums::PLAIN) {
            return " " + n.getName() + " ";
        }

        if(n.getNodeTypeEnums() == NodeTypeEnums::CONSTANT) {
            string name = n.getName();
            name.at(0) = '#';
            return " " + name + " ";
        }

        if (n.getLeftChild() != NULL) {
            string ss = parseNodeBool(*n.getLeftChild()) ;
            s = s.replace(s.find(" " + n.getLeftChild()->getName() + " "), n.getLeftChild()->getName().size() + 2, ss);

        }
        if (n.getRightChild() != NULL) {
            string ss = parseNodeBool(*n.getRightChild()) ;
            s = s.replace(s.find(" " + n.getRightChild()->getName() + " "), n.getRightChild()->getName().size() + 2, ss);

        }
        return s;
    }

	static string parseNodeBv(Node n) {
		string s = getFirstString(n);
		if (n.getNodeTypeEnums() == NodeTypeEnums::MASK ||
			n.getNodeTypeEnums() == NodeTypeEnums::SECRECT ||
			n.getNodeTypeEnums()==NodeTypeEnums::PLAIN) {
			return " " + n.getName() + " ";
		}

		if(n.getNodeTypeEnums() == NodeTypeEnums::CONSTANT) {
			string name = n.getName();
			name.at(0) = '#';
			return " " + name + " ";
		}

		if (n.getLeftChild() != NULL) {
			string ss = parseNodeBv(*n.getLeftChild()) ;
            s = s.replace(s.find(" " + n.getLeftChild()->getName() + " "), n.getLeftChild()->getName().size() + 2, ss);

		}
		if (n.getRightChild() != NULL) {
			string ss = parseNodeBv(*n.getRightChild()) ;
			s = s.replace(s.find(" " + n.getRightChild()->getName() + " "), n.getRightChild()->getName().size() + 2, ss);

		}
		return s;
	}

	static string getFirstStringForCounting(Node n) {
		string s="";
		string leftName, rightName;
		if (n.getLeftChild() != NULL) {
			leftName = n.getLeftChild()->getName();
		}
		if (n.getRightChild() != NULL) {
			rightName = n.getRightChild()->getName();
		}


		if (n.getOperatorEnums() == OperatorEnums::AND) {
			s.append("( &  " + leftName + "  " + rightName + "  ) ");
		}else if (n.getOperatorEnums() == OperatorEnums::OR) {
			s.append("( |  " + leftName + "  " + rightName + "  ) ");
		}else if (n.getOperatorEnums() == OperatorEnums::XOR) {
			s.append("( ^  " + leftName + "  " + rightName + "  ) ");
		}else if (n.getOperatorEnums() == OperatorEnums::NOT) {
			s.append("( ~  " + leftName + "  " + rightName + "  ) ");
		} else if(n.getOperatorEnums() == OperatorEnums::MINU) {
			s.append("( -  " + leftName + "  " + rightName + "  ) ");
        } else if(n.getOperatorEnums() == OperatorEnums::ADD) {
            s.append("( +  " + leftName + "  " + rightName + "  ) ");
		} else if(n.getOperatorEnums() == OperatorEnums::GFMUL) {
			s.append("( !  " + leftName + "  " + rightName + "  ) ");
		} else if(n.getOperatorEnums() == OperatorEnums::MUL) {
			s.append("( *  " + leftName + "  " + rightName + "  ) ");
		} else if(n.getOperatorEnums() == OperatorEnums::POW) {
			int powNum = n.getPow();
			s.append("( pow" + to_string(powNum) + "  " + leftName + " ) ");
		}
		else {
			s.append("  " + leftName + "  " + rightName + "  ");
		}
		return s;
	}

	static string parseNodeForCounting(Node n) {
		string s = getFirstStringForCounting(n);
		if (n.getNodeTypeEnums() == NodeTypeEnums::MASK ||
			n.getNodeTypeEnums() == NodeTypeEnums::SECRECT ||
			n.getNodeTypeEnums()==NodeTypeEnums::PLAIN) {
			return " " + n.getName() + " ";
		}

		if(n.getNodeTypeEnums() == NodeTypeEnums::CONSTANT) {
			string name = n.getName();
			name.at(0) = '0';
			return " " + name + " ";
		}

		if (n.getLeftChild() != NULL) {
			string ss = parseNodeForCounting(*n.getLeftChild()) ;
			s = s.replace(s.find(" " + n.getLeftChild()->getName() + " "), n.getLeftChild()->getName().size() + 2, ss);

		}
		if (n.getRightChild() != NULL) {
			string ss = parseNodeForCounting(*n.getRightChild()) ;
			s = s.replace(s.find(" " + n.getRightChild()->getName() + " "), n.getRightChild()->getName().size() + 2, ss);

		}
		return s;
	}








	static DistributionEnums getDistributionByZ3Test(Node node, int bitLength) {

		string bitLengthS = StringUtil::getString(bitLength);

		set<string> rand;
		set<string> secret;
		set<string> plain;
		vector<string> randVector;
		vector<string> secretVector;
		vector<string> plainVector;
		vector<string> notCareRandVector;
		vector<string> num1;
		vector<string> num2;
		map<string, string> randMap;
		map<string, string> secretMap1;
		map<string, string> secretMap2;
		map<string, string> plainMap;


		if(node.getSemd() != NULL) {
			rand = *node.getSemd();
		} else {
			Node::getRandSet(node, rand);
		}
		Node::getSecretSet(node, secret);
		Node::getPlainSet(node, plain);


		SetUtil::SetToVector(rand, randVector);
		SetUtil::SetToVector(secret, secretVector);
		SetUtil::SetToVector(plain, plainVector);



		int randCount = rand.size();
		int secretCount = secret.size();
		int plainCount = plain.size();
		int num = 1;//record for all rand power set size
		for (int i = 0; i < randCount; i++) {
			num = num * 2;
		}
		num = int(pow(num, bitLength));



		string finalString="";
		z3::context contextUse;
		z3::solver solverUse(contextUse);

		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun r");
			s.append(StringUtil::getString(i) + "() (_ BitVec " + bitLengthS + "))");

			finalString.append(s+"\n");
		}

		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun rr");
			s.append(StringUtil::getString(i) + "() (_ BitVec " + bitLengthS + "))");

			finalString.append(s+"\n");
		}
		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun n");
			s.append(StringUtil::getString(i) + "() (_ BitVec 32))");
			num1.push_back("n" + StringUtil::getString(i));

			finalString.append(s+"\n");
		}
		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun nn");
			s.append(StringUtil::getString(i) + "() (_ BitVec 32) )");
			num2.push_back("nn" + StringUtil::getString(i));

			finalString.append(s+"\n");
		}
		for (int i = 0; i < secretCount; i++) {
			string s = "";
			s.append("(declare-fun k");
			s.append(StringUtil::getString(i) + "() (_ BitVec " + bitLengthS + "))");
			secretMap1[secretVector[i]] = " k" + StringUtil::getString(i) + " ";

			finalString.append(s+"\n");
		}
		for (int i = 0; i < secretCount; i++) {
			string s = "";
			s.append("(declare-fun kk");
			s.append(StringUtil::getString(i) + "() (_ BitVec " + bitLengthS + "))");
			secretMap2[secretVector[i]] = " kk" + StringUtil::getString(i) + " ";

			finalString.append(s+"\n");
		}

		for(int i = 0; i < secretCount; i++) {
			string s = "(assert ( not ( = k" + to_string(i) + " kk" + to_string(i) + " )))\n";
			finalString.append(s);
		}

		for (int i = 0; i < plainCount; i++) {
			string s = "";
			s.append("(declare-fun ");
			s.append(plainVector[i] + "() (_ BitVec " + bitLengthS + "))");

			finalString.append(s+"\n");
		}

		finalString.append("( declare-fun c () (_ BitVec " + bitLengthS + "))\n");


		vector<vector<int>> tempVec = StringUtil::getDistribution(bitLength, randCount);
		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(assert(= r" + StringUtil::getString(i)+" ");
			string ss = parseNodeBv(node);
			StringUtil::replaceThroughMap(ss, secretVector, secretMap1);
            StringUtil::BitToBoolVectorBv(randVector, tempVec[i], randMap, bitLengthS);
			StringUtil::replaceThroughMap(ss, randVector, randMap);

			s.append(ss + " ))");
			StringUtil::replace_all(s, " 0 ", "  (_ bv0 " + bitLengthS + ") ");
			StringUtil::replace_all(s, " 1 ", "  (_ bv" + StringUtil::getString(int(pow(2, bitLength) - 1)) + " " + bitLengthS + ") ");


			finalString.append(s+"\n");
		}

		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(assert(= rr" + StringUtil::getString(i)+" ");
			string ss = parseNodeBv(node);
			StringUtil::replaceThroughMap(ss, secretVector, secretMap2);
			StringUtil::BitToBoolVectorBv(randVector, tempVec[i], randMap, bitLengthS);
			StringUtil::replaceThroughMap(ss, randVector, randMap);

			s.append(ss + "))");
			StringUtil::replace_all(s, " 0 ", "  (_ bv0 " + bitLengthS + ") ");
			StringUtil::replace_all(s, " 1 ", "  (_ bv" + StringUtil::getString(int(pow(2, bitLength) - 1)) + " " + bitLengthS + ") ");


			finalString.append(s+"\n");
		}

		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(assert (= n" + StringUtil::getString(i) + " (fun " + "r" + StringUtil::getString(i) + "  c )))");


			finalString.append(s+"\n");
		}

		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(assert (= nn" + StringUtil::getString(i) + " (fun " + "rr" + StringUtil::getString(i) + " c )))");


			finalString.append(s+"\n");
		}

		string s = "(assert (not (= ";
		string s1, s2;
		StringUtil::VectorLogicAdd(num1, s1);
		StringUtil::VectorLogicAdd(num2, s2);

		s.append(s1 + " ");
		s.append(s2);
		s.append(")))");

		finalString.append(s+"\n");

		size_t foundGFMUL = finalString.find("bvgfmul");
		if(foundGFMUL != string::npos) {
			finalString = SMT2Parse::powdefine() + finalString;
			finalString = SMT2Parse::bvgfmulDefinationITE() + finalString;
		}


		finalString = SMT2Parse::fundefine() + "\n" + finalString;


		outputToFile(finalString, 6);
		solverUse.from_string(finalString.data());
		
		cout << "z3 finished parse, no error" << endl;

		DistributionEnums dis;
		cout << "begin check using z3" << endl;
		switch (solverUse.check()) {
			case z3::unsat:  // std::cout << "unsat\n";
				dis=DistributionEnums ::ISD; break;
			case z3::sat:    // std::cout << "sat\n";
				dis=DistributionEnums ::NPM;
				break;
			case z3::unknown: 
				std::cout << "unknown\n";
				dis=DistributionEnums ::UKD; break;
		}

		solverUse.reset();

		return dis;


	}

    static double getQMSbyZ3(Node node, int bitLength) {

	string bitLengthS = StringUtil::getString(bitLength);

		set<string> rand;
		set<string> secret;
		set<string> plain;
		vector<string> randVector;
		vector<string> secretVector;
		vector<string> plainVector;
		vector<string> notCareRandVector;
		vector<string> num1;
		vector<string> num2;
		map<string, string> randMap;
		map<string, string> secretMap1;
		map<string, string> secretMap2;
		map<string, string> plainMap;


		if(node.getSemd() != NULL) {
			rand = *node.getSemd();
		} else {
			Node::getRandSet(node, rand);
		}
		Node::getSecretSet(node, secret);
		Node::getPlainSet(node, plain);


		SetUtil::SetToVector(rand, randVector);
		SetUtil::SetToVector(secret, secretVector);
		SetUtil::SetToVector(plain, plainVector);



		int randCount = rand.size();
		int secretCount = secret.size();
		int plainCount = plain.size();
		int num = 1;//record for all rand power set size
		for (int i = 0; i < randCount; i++) {
			num = num * 2;
		}
		num = int(pow(num, bitLength));



		string finalString="";
		z3::context contextUse;
		z3::solver solverUse(contextUse);

		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun r");
			s.append(StringUtil::getString(i) + "() (_ BitVec " + bitLengthS + "))");

			finalString.append(s+"\n");
		}

		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun rr");
			s.append(StringUtil::getString(i) + "() (_ BitVec " + bitLengthS + "))");

			finalString.append(s+"\n");
		}
		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun n");
			s.append(StringUtil::getString(i) + "() (_ BitVec 32))");
			num1.push_back("n" + StringUtil::getString(i));

			finalString.append(s+"\n");
		}
		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun nn");
			s.append(StringUtil::getString(i) + "() (_ BitVec 32) )");
			num2.push_back("nn" + StringUtil::getString(i));

			finalString.append(s+"\n");
		}
		for (int i = 0; i < secretCount; i++) {
			string s = "";
			s.append("(declare-fun k");
			s.append(StringUtil::getString(i) + "() (_ BitVec " + bitLengthS + "))");
			secretMap1[secretVector[i]] = " k" + StringUtil::getString(i) + " ";

			finalString.append(s+"\n");
		}
		for (int i = 0; i < secretCount; i++) {
			string s = "";
			s.append("(declare-fun kk");
			s.append(StringUtil::getString(i) + "() (_ BitVec " + bitLengthS + "))");
			secretMap2[secretVector[i]] = " kk" + StringUtil::getString(i) + " ";

			finalString.append(s+"\n");
		}

		for(int i = 0; i < secretCount; i++) {
			string s = "(assert ( not ( = k" + to_string(i) + " kk" + to_string(i) + " )))\n";
			finalString.append(s);
		}

		for (int i = 0; i < plainCount; i++) {
			string s = "";
			s.append("(declare-fun ");
			s.append(plainVector[i] + "() (_ BitVec " + bitLengthS + "))");

			finalString.append(s+"\n");
		}

		finalString.append("( declare-fun c () (_ BitVec " + bitLengthS + "))\n");


		vector<vector<int>> tempVec = StringUtil::getDistribution(bitLength, randCount);
		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(assert(= r" + StringUtil::getString(i)+" ");
			string ss = parseNodeBv(node);
			StringUtil::replaceThroughMap(ss, secretVector, secretMap1);
            StringUtil::BitToBoolVectorBv(randVector, tempVec[i], randMap, bitLengthS);
			StringUtil::replaceThroughMap(ss, randVector, randMap);

			s.append(ss + " ))");
			StringUtil::replace_all(s, " 0 ", "  (_ bv0 " + bitLengthS + ") ");
			StringUtil::replace_all(s, " 1 ", "  (_ bv" + StringUtil::getString(int(pow(2, bitLength) - 1)) + " " + bitLengthS + ") ");


			finalString.append(s+"\n");
		}

		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(assert(= rr" + StringUtil::getString(i)+" ");
			string ss = parseNodeBv(node);
			StringUtil::replaceThroughMap(ss, secretVector, secretMap2);
			StringUtil::BitToBoolVectorBv(randVector, tempVec[i], randMap, bitLengthS);
			StringUtil::replaceThroughMap(ss, randVector, randMap);

			s.append(ss + "))");
			StringUtil::replace_all(s, " 0 ", "  (_ bv0 " + bitLengthS + ") ");
			StringUtil::replace_all(s, " 1 ", "  (_ bv" + StringUtil::getString(int(pow(2, bitLength) - 1)) + " " + bitLengthS + ") ");


			finalString.append(s+"\n");
		}

		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(assert (= n" + StringUtil::getString(i) + " (fun " + "r" + StringUtil::getString(i) + "  c )))");

			finalString.append(s+"\n");
		}

		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(assert (= nn" + StringUtil::getString(i) + " (fun " + "rr" + StringUtil::getString(i) + " c )))");

			finalString.append(s+"\n");
		}

	double step=1.0;
	for(unsigned int i=0;i<rand.size();i++)
	{
		step=step/2.0;
	}
	
        string ss = "(define-fun pow2 ((x (_ BitVec 8))  )(_ BitVec 8)\n(ite  (= x #x00)  #x00\n (table (bvadd (_ bv256 10) ( bvadd   ((_ zero_extend 2)(table ((_ zero_extend 2)x) )) ((_ zero_extend 2) (table  ((_ zero_extend 2)x) ))  ) ))\n)\n)\n";
        ss.append("(define-fun pow4 ((x (_ BitVec 8))  )(_ BitVec 8)\n(ite  (= x #x00)  #x00 \n(pow2 (pow2 x))\n )\n)\n");
        ss.append("(define-fun pow8 ((x (_ BitVec 8))  )(_ BitVec 8)\n(ite  (= x #x00)  #x00 \n(pow4 (pow4 x))\n )\n)\n");
        ss.append("(define-fun pow16 ((x (_ BitVec 8))  )(_ BitVec 8)\n(ite  (= x #x00)  #x00 \n(pow8 (pow8 x))\n )\n)\n\n");
        finalString=ss+finalString;
        finalString = SMT2Parse::bvgfmulDefinationITE() + finalString;
        finalString = SMT2Parse::fundefine() + "\n" + finalString;
        
        
        
        finalString.append("(declare-const num1 (_ BitVec 32))\n(declare-const num2 (_ BitVec 32))\n");
        string s1, s2,tem;
        StringUtil::VectorLogicAdd(num1, s1);
        StringUtil::VectorLogicAdd(num2, s2);
	
        string temp1,temp2;
        temp1.append("(assert (= num1 ");
        //cout<<"s1: "<<s1<<endl;
        temp1.append(s1+" ))");
        finalString.append(temp1+"\n");
        temp2.append("(assert (= num2 ");
        temp2.append(s2+" ))");
        finalString.append(temp2+"\n");
        
        
        bool flag=1;
        int bz=0;
        double low=0.0;
        double high;
        high=pow(2,8*rand.size());
        double con=pow(2,8*rand.size());
        stack<double> st;
        int mid=ceil((low+high)/2.0);
        double qms=mid/con;
        
        if(rand.size()==0) {return 0.0;}else{
        while(low<high && flag){
            string qmss;
            stringstream strstream;
       	    string c="";
       	    c=finalString+c;
            int qmsTemp=(1-qms)*num;
            strstream<<qmsTemp;
            strstream>>qmss;
            

            c.append("(assert (or (bvsgt (bvsub num1 num2) (_ bv"+qmss+" 32)) (bvsgt (bvsub num2 num1) (_ bv"+qmss+" 32))))\n");
            solverUse.from_string(c.data());
            switch (solverUse.check()) {
		        case z3::unsat:
		            //if(mid+0.01>1)
		            //{flag=0;break;}
		            low=mid;
		            //flag=0;
		            bz=1;
		            cout << "unsat" << endl;
		            break;
		        case z3::sat:
		            high=mid-1;
		            bz=2;
		            cout << "sat" << endl;
		            break;
		        case z3::unknown:
		             low=mid;
		            bz=3;
		            break;
		    }
            
            mid=ceil((low+high)/2.0);
            qms=mid/con;
        }
        }
        
        return low/con;


    }





	static void outputToFile(string & content, int name) {
        string cnfname = std::to_string(name) + "";
        cnfname.append(".smt2");
        streambuf *cout_buf = cout.rdbuf();
        ofstream of("./" + cnfname);
        streambuf *file_buf = of.rdbuf();
        cout.rdbuf(file_buf);

        cout << content << std::endl;
        of.flush();
        of.close();
        cout.rdbuf(cout_buf);
	}



	static string bvgfmulDefination() {
	    return "(declare-const array (Array (_ BitVec 10) (_ BitVec 8)))\n"
			   "(assert (= (select array (_ bv0 10)) #x00))\n"
			   "(assert (= (select array (_ bv1 10)) #x00))\n"
			   "(assert (= (select array (_ bv2 10)) #x19))\n"
			   "(assert (= (select array (_ bv3 10)) #x01))\n"
			   "(assert (= (select array (_ bv4 10)) #x32))\n"
			   "(assert (= (select array (_ bv5 10)) #x02))\n"
			   "(assert (= (select array (_ bv6 10)) #x1a))\n"
			   "(assert (= (select array (_ bv7 10)) #xc6))\n"
			   "(assert (= (select array (_ bv8 10)) #x4b))\n"
			   "(assert (= (select array (_ bv9 10)) #xc7))\n"
			   "(assert (= (select array (_ bv10 10)) #x1b))\n"
			   "(assert (= (select array (_ bv11 10)) #x68))\n"
			   "(assert (= (select array (_ bv12 10)) #x33))\n"
			   "(assert (= (select array (_ bv13 10)) #xee))\n"
			   "(assert (= (select array (_ bv14 10)) #xdf))\n"
			   "(assert (= (select array (_ bv15 10)) #x03))\n"
			   "(assert (= (select array (_ bv16 10)) #x64))\n"
			   "(assert (= (select array (_ bv17 10)) #x04))\n"
			   "(assert (= (select array (_ bv18 10)) #xe0))\n"
			   "(assert (= (select array (_ bv19 10)) #x0e))\n"
			   "(assert (= (select array (_ bv20 10)) #x34))\n"
			   "(assert (= (select array (_ bv21 10)) #x8d))\n"
			   "(assert (= (select array (_ bv22 10)) #x81))\n"
			   "(assert (= (select array (_ bv23 10)) #xef))\n"
			   "(assert (= (select array (_ bv24 10)) #x4c))\n"
			   "(assert (= (select array (_ bv25 10)) #x71))\n"
			   "(assert (= (select array (_ bv26 10)) #x08))\n"
			   "(assert (= (select array (_ bv27 10)) #xc8))\n"
			   "(assert (= (select array (_ bv28 10)) #xf8))\n"
			   "(assert (= (select array (_ bv29 10)) #x69))\n"
			   "(assert (= (select array (_ bv30 10)) #x1c))\n"
			   "(assert (= (select array (_ bv31 10)) #xc1))\n"
			   "(assert (= (select array (_ bv32 10)) #x7d))\n"
			   "(assert (= (select array (_ bv33 10)) #xc2))\n"
			   "(assert (= (select array (_ bv34 10)) #x1d))\n"
			   "(assert (= (select array (_ bv35 10)) #xb5))\n"
			   "(assert (= (select array (_ bv36 10)) #xf9))\n"
			   "(assert (= (select array (_ bv37 10)) #xb9))\n"
			   "(assert (= (select array (_ bv38 10)) #x27))\n"
			   "(assert (= (select array (_ bv39 10)) #x6a))\n"
			   "(assert (= (select array (_ bv40 10)) #x4d))\n"
			   "(assert (= (select array (_ bv41 10)) #xe4))\n"
			   "(assert (= (select array (_ bv42 10)) #xa6))\n"
			   "(assert (= (select array (_ bv43 10)) #x72))\n"
			   "(assert (= (select array (_ bv44 10)) #x9a))\n"
			   "(assert (= (select array (_ bv45 10)) #xc9))\n"
			   "(assert (= (select array (_ bv46 10)) #x09))\n"
			   "(assert (= (select array (_ bv47 10)) #x78))\n"
			   "(assert (= (select array (_ bv48 10)) #x65))\n"
			   "(assert (= (select array (_ bv49 10)) #x2f))\n"
			   "(assert (= (select array (_ bv50 10)) #x8a))\n"
			   "(assert (= (select array (_ bv51 10)) #x05))\n"
			   "(assert (= (select array (_ bv52 10)) #x21))\n"
			   "(assert (= (select array (_ bv53 10)) #x0f))\n"
			   "(assert (= (select array (_ bv54 10)) #xe1))\n"
			   "(assert (= (select array (_ bv55 10)) #x24))\n"
			   "(assert (= (select array (_ bv56 10)) #x12))\n"
			   "(assert (= (select array (_ bv57 10)) #xf0))\n"
			   "(assert (= (select array (_ bv58 10)) #x82))\n"
			   "(assert (= (select array (_ bv59 10)) #x45))\n"
			   "(assert (= (select array (_ bv60 10)) #x35))\n"
			   "(assert (= (select array (_ bv61 10)) #x93))\n"
			   "(assert (= (select array (_ bv62 10)) #xda))\n"
			   "(assert (= (select array (_ bv63 10)) #x8e))\n"
			   "(assert (= (select array (_ bv64 10)) #x96))\n"
			   "(assert (= (select array (_ bv65 10)) #x8f))\n"
			   "(assert (= (select array (_ bv66 10)) #xdb))\n"
			   "(assert (= (select array (_ bv67 10)) #xbd))\n"
			   "(assert (= (select array (_ bv68 10)) #x36))\n"
			   "(assert (= (select array (_ bv69 10)) #xd0))\n"
			   "(assert (= (select array (_ bv70 10)) #xce))\n"
			   "(assert (= (select array (_ bv71 10)) #x94))\n"
			   "(assert (= (select array (_ bv72 10)) #x13))\n"
			   "(assert (= (select array (_ bv73 10)) #x5c))\n"
			   "(assert (= (select array (_ bv74 10)) #xd2))\n"
			   "(assert (= (select array (_ bv75 10)) #xf1))\n"
			   "(assert (= (select array (_ bv76 10)) #x40))\n"
			   "(assert (= (select array (_ bv77 10)) #x46))\n"
			   "(assert (= (select array (_ bv78 10)) #x83))\n"
			   "(assert (= (select array (_ bv79 10)) #x38))\n"
			   "(assert (= (select array (_ bv80 10)) #x66))\n"
			   "(assert (= (select array (_ bv81 10)) #xdd))\n"
			   "(assert (= (select array (_ bv82 10)) #xfd))\n"
			   "(assert (= (select array (_ bv83 10)) #x30))\n"
			   "(assert (= (select array (_ bv84 10)) #xbf))\n"
			   "(assert (= (select array (_ bv85 10)) #x06))\n"
			   "(assert (= (select array (_ bv86 10)) #x8b))\n"
			   "(assert (= (select array (_ bv87 10)) #x62))\n"
			   "(assert (= (select array (_ bv88 10)) #xb3))\n"
			   "(assert (= (select array (_ bv89 10)) #x25))\n"
			   "(assert (= (select array (_ bv90 10)) #xe2))\n"
			   "(assert (= (select array (_ bv91 10)) #x98))\n"
			   "(assert (= (select array (_ bv92 10)) #x22))\n"
			   "(assert (= (select array (_ bv93 10)) #x88))\n"
			   "(assert (= (select array (_ bv94 10)) #x91))\n"
			   "(assert (= (select array (_ bv95 10)) #x10))\n"
			   "(assert (= (select array (_ bv96 10)) #x7e))\n"
			   "(assert (= (select array (_ bv97 10)) #x6e))\n"
			   "(assert (= (select array (_ bv98 10)) #x48))\n"
			   "(assert (= (select array (_ bv99 10)) #xc3))\n"
			   "(assert (= (select array (_ bv100 10)) #xa3))\n"
			   "(assert (= (select array (_ bv101 10)) #xb6))\n"
			   "(assert (= (select array (_ bv102 10)) #x1e))\n"
			   "(assert (= (select array (_ bv103 10)) #x42))\n"
			   "(assert (= (select array (_ bv104 10)) #x3a))\n"
			   "(assert (= (select array (_ bv105 10)) #x6b))\n"
			   "(assert (= (select array (_ bv106 10)) #x28))\n"
			   "(assert (= (select array (_ bv107 10)) #x54))\n"
			   "(assert (= (select array (_ bv108 10)) #xfa))\n"
			   "(assert (= (select array (_ bv109 10)) #x85))\n"
			   "(assert (= (select array (_ bv110 10)) #x3d))\n"
			   "(assert (= (select array (_ bv111 10)) #xba))\n"
			   "(assert (= (select array (_ bv112 10)) #x2b))\n"
			   "(assert (= (select array (_ bv113 10)) #x79))\n"
			   "(assert (= (select array (_ bv114 10)) #x0a))\n"
			   "(assert (= (select array (_ bv115 10)) #x15))\n"
			   "(assert (= (select array (_ bv116 10)) #x9b))\n"
			   "(assert (= (select array (_ bv117 10)) #x9f))\n"
			   "(assert (= (select array (_ bv118 10)) #x5e))\n"
			   "(assert (= (select array (_ bv119 10)) #xca))\n"
			   "(assert (= (select array (_ bv120 10)) #x4e))\n"
			   "(assert (= (select array (_ bv121 10)) #xd4))\n"
			   "(assert (= (select array (_ bv122 10)) #xac))\n"
			   "(assert (= (select array (_ bv123 10)) #xe5))\n"
			   "(assert (= (select array (_ bv124 10)) #xf3))\n"
			   "(assert (= (select array (_ bv125 10)) #x73))\n"
			   "(assert (= (select array (_ bv126 10)) #xa7))\n"
			   "(assert (= (select array (_ bv127 10)) #x57))\n"
			   "(assert (= (select array (_ bv128 10)) #xaf))\n"
			   "(assert (= (select array (_ bv129 10)) #x58))\n"
			   "(assert (= (select array (_ bv130 10)) #xa8))\n"
			   "(assert (= (select array (_ bv131 10)) #x50))\n"
			   "(assert (= (select array (_ bv132 10)) #xf4))\n"
			   "(assert (= (select array (_ bv133 10)) #xea))\n"
			   "(assert (= (select array (_ bv134 10)) #xd6))\n"
			   "(assert (= (select array (_ bv135 10)) #x74))\n"
			   "(assert (= (select array (_ bv136 10)) #x4f))\n"
			   "(assert (= (select array (_ bv137 10)) #xae))\n"
			   "(assert (= (select array (_ bv138 10)) #xe9))\n"
			   "(assert (= (select array (_ bv139 10)) #xd5))\n"
			   "(assert (= (select array (_ bv140 10)) #xe7))\n"
			   "(assert (= (select array (_ bv141 10)) #xe6))\n"
			   "(assert (= (select array (_ bv142 10)) #xad))\n"
			   "(assert (= (select array (_ bv143 10)) #xe8))\n"
			   "(assert (= (select array (_ bv144 10)) #x2c))\n"
			   "(assert (= (select array (_ bv145 10)) #xd7))\n"
			   "(assert (= (select array (_ bv146 10)) #x75))\n"
			   "(assert (= (select array (_ bv147 10)) #x7a))\n"
			   "(assert (= (select array (_ bv148 10)) #xeb))\n"
			   "(assert (= (select array (_ bv149 10)) #x16))\n"
			   "(assert (= (select array (_ bv150 10)) #x0b))\n"
			   "(assert (= (select array (_ bv151 10)) #xf5))\n"
			   "(assert (= (select array (_ bv152 10)) #x59))\n"
			   "(assert (= (select array (_ bv153 10)) #xcb))\n"
			   "(assert (= (select array (_ bv154 10)) #x5f))\n"
			   "(assert (= (select array (_ bv155 10)) #xb0))\n"
			   "(assert (= (select array (_ bv156 10)) #x9c))\n"
			   "(assert (= (select array (_ bv157 10)) #xa9))\n"
			   "(assert (= (select array (_ bv158 10)) #x51))\n"
			   "(assert (= (select array (_ bv159 10)) #xa0))\n"
			   "(assert (= (select array (_ bv160 10)) #x7f))\n"
			   "(assert (= (select array (_ bv161 10)) #x0c))\n"
			   "(assert (= (select array (_ bv162 10)) #xf6))\n"
			   "(assert (= (select array (_ bv163 10)) #x6f))\n"
			   "(assert (= (select array (_ bv164 10)) #x17))\n"
			   "(assert (= (select array (_ bv165 10)) #xc4))\n"
			   "(assert (= (select array (_ bv166 10)) #x49))\n"
			   "(assert (= (select array (_ bv167 10)) #xec))\n"
			   "(assert (= (select array (_ bv168 10)) #xd8))\n"
			   "(assert (= (select array (_ bv169 10)) #x43))\n"
			   "(assert (= (select array (_ bv170 10)) #x1f))\n"
			   "(assert (= (select array (_ bv171 10)) #x2d))\n"
			   "(assert (= (select array (_ bv172 10)) #xa4))\n"
			   "(assert (= (select array (_ bv173 10)) #x76))\n"
			   "(assert (= (select array (_ bv174 10)) #x7b))\n"
			   "(assert (= (select array (_ bv175 10)) #xb7))\n"
			   "(assert (= (select array (_ bv176 10)) #xcc))\n"
			   "(assert (= (select array (_ bv177 10)) #xbb))\n"
			   "(assert (= (select array (_ bv178 10)) #x3e))\n"
			   "(assert (= (select array (_ bv179 10)) #x5a))\n"
			   "(assert (= (select array (_ bv180 10)) #xfb))\n"
			   "(assert (= (select array (_ bv181 10)) #x60))\n"
			   "(assert (= (select array (_ bv182 10)) #xb1))\n"
			   "(assert (= (select array (_ bv183 10)) #x86))\n"
			   "(assert (= (select array (_ bv184 10)) #x3b))\n"
			   "(assert (= (select array (_ bv185 10)) #x52))\n"
			   "(assert (= (select array (_ bv186 10)) #xa1))\n"
			   "(assert (= (select array (_ bv187 10)) #x6c))\n"
			   "(assert (= (select array (_ bv188 10)) #xaa))\n"
			   "(assert (= (select array (_ bv189 10)) #x55))\n"
			   "(assert (= (select array (_ bv190 10)) #x29))\n"
			   "(assert (= (select array (_ bv191 10)) #x9d))\n"
			   "(assert (= (select array (_ bv192 10)) #x97))\n"
			   "(assert (= (select array (_ bv193 10)) #xb2))\n"
			   "(assert (= (select array (_ bv194 10)) #x87))\n"
			   "(assert (= (select array (_ bv195 10)) #x90))\n"
			   "(assert (= (select array (_ bv196 10)) #x61))\n"
			   "(assert (= (select array (_ bv197 10)) #xbe))\n"
			   "(assert (= (select array (_ bv198 10)) #xdc))\n"
			   "(assert (= (select array (_ bv199 10)) #xfc))\n"
			   "(assert (= (select array (_ bv200 10)) #xbc))\n"
			   "(assert (= (select array (_ bv201 10)) #x95))\n"
			   "(assert (= (select array (_ bv202 10)) #xcf))\n"
			   "(assert (= (select array (_ bv203 10)) #xcd))\n"
			   "(assert (= (select array (_ bv204 10)) #x37))\n"
			   "(assert (= (select array (_ bv205 10)) #x3f))\n"
			   "(assert (= (select array (_ bv206 10)) #x5b))\n"
			   "(assert (= (select array (_ bv207 10)) #xd1))\n"
			   "(assert (= (select array (_ bv208 10)) #x53))\n"
			   "(assert (= (select array (_ bv209 10)) #x39))\n"
			   "(assert (= (select array (_ bv210 10)) #x84))\n"
			   "(assert (= (select array (_ bv211 10)) #x3c))\n"
			   "(assert (= (select array (_ bv212 10)) #x41))\n"
			   "(assert (= (select array (_ bv213 10)) #xa2))\n"
			   "(assert (= (select array (_ bv214 10)) #x6d))\n"
			   "(assert (= (select array (_ bv215 10)) #x47))\n"
			   "(assert (= (select array (_ bv216 10)) #x14))\n"
			   "(assert (= (select array (_ bv217 10)) #x2a))\n"
			   "(assert (= (select array (_ bv218 10)) #x9e))\n"
			   "(assert (= (select array (_ bv219 10)) #x5d))\n"
			   "(assert (= (select array (_ bv220 10)) #x56))\n"
			   "(assert (= (select array (_ bv221 10)) #xf2))\n"
			   "(assert (= (select array (_ bv222 10)) #xd3))\n"
			   "(assert (= (select array (_ bv223 10)) #xab))\n"
			   "(assert (= (select array (_ bv224 10)) #x44))\n"
			   "(assert (= (select array (_ bv225 10)) #x11))\n"
			   "(assert (= (select array (_ bv226 10)) #x92))\n"
			   "(assert (= (select array (_ bv227 10)) #xd9))\n"
			   "(assert (= (select array (_ bv228 10)) #x23))\n"
			   "(assert (= (select array (_ bv229 10)) #x20))\n"
			   "(assert (= (select array (_ bv230 10)) #x2e))\n"
			   "(assert (= (select array (_ bv231 10)) #x89))\n"
			   "(assert (= (select array (_ bv232 10)) #xb4))\n"
			   "(assert (= (select array (_ bv233 10)) #x7c))\n"
			   "(assert (= (select array (_ bv234 10)) #xb8))\n"
			   "(assert (= (select array (_ bv235 10)) #x26))\n"
			   "(assert (= (select array (_ bv236 10)) #x77))\n"
			   "(assert (= (select array (_ bv237 10)) #x99))\n"
			   "(assert (= (select array (_ bv238 10)) #xe3))\n"
			   "(assert (= (select array (_ bv239 10)) #xa5))\n"
			   "(assert (= (select array (_ bv240 10)) #x67))\n"
			   "(assert (= (select array (_ bv241 10)) #x4a))\n"
			   "(assert (= (select array (_ bv242 10)) #xed))\n"
			   "(assert (= (select array (_ bv243 10)) #xde))\n"
			   "(assert (= (select array (_ bv244 10)) #xc5))\n"
			   "(assert (= (select array (_ bv245 10)) #x31))\n"
			   "(assert (= (select array (_ bv246 10)) #xfe))\n"
			   "(assert (= (select array (_ bv247 10)) #x18))\n"
			   "(assert (= (select array (_ bv248 10)) #x0d))\n"
			   "(assert (= (select array (_ bv249 10)) #x63))\n"
			   "(assert (= (select array (_ bv250 10)) #x8c))\n"
			   "(assert (= (select array (_ bv251 10)) #x80))\n"
			   "(assert (= (select array (_ bv252 10)) #xc0))\n"
			   "(assert (= (select array (_ bv253 10)) #xf7))\n"
			   "(assert (= (select array (_ bv254 10)) #x70))\n"
			   "(assert (= (select array (_ bv255 10)) #x07))\n"
			   "(assert (= (select array (_ bv256 10)) #x01))\n"
			   "(assert (= (select array (_ bv257 10)) #x03))\n"
			   "(assert (= (select array (_ bv258 10)) #x05))\n"
			   "(assert (= (select array (_ bv259 10)) #x0f))\n"
			   "(assert (= (select array (_ bv260 10)) #x11))\n"
			   "(assert (= (select array (_ bv261 10)) #x33))\n"
			   "(assert (= (select array (_ bv262 10)) #x55))\n"
			   "(assert (= (select array (_ bv263 10)) #xff))\n"
			   "(assert (= (select array (_ bv264 10)) #x1a))\n"
			   "(assert (= (select array (_ bv265 10)) #x2e))\n"
			   "(assert (= (select array (_ bv266 10)) #x72))\n"
			   "(assert (= (select array (_ bv267 10)) #x96))\n"
			   "(assert (= (select array (_ bv268 10)) #xa1))\n"
			   "(assert (= (select array (_ bv269 10)) #xf8))\n"
			   "(assert (= (select array (_ bv270 10)) #x13))\n"
			   "(assert (= (select array (_ bv271 10)) #x35))\n"
			   "(assert (= (select array (_ bv272 10)) #x5f))\n"
			   "(assert (= (select array (_ bv273 10)) #xe1))\n"
			   "(assert (= (select array (_ bv274 10)) #x38))\n"
			   "(assert (= (select array (_ bv275 10)) #x48))\n"
			   "(assert (= (select array (_ bv276 10)) #xd8))\n"
			   "(assert (= (select array (_ bv277 10)) #x73))\n"
			   "(assert (= (select array (_ bv278 10)) #x95))\n"
			   "(assert (= (select array (_ bv279 10)) #xa4))\n"
			   "(assert (= (select array (_ bv280 10)) #xf7))\n"
			   "(assert (= (select array (_ bv281 10)) #x02))\n"
			   "(assert (= (select array (_ bv282 10)) #x06))\n"
			   "(assert (= (select array (_ bv283 10)) #x0a))\n"
			   "(assert (= (select array (_ bv284 10)) #x1e))\n"
			   "(assert (= (select array (_ bv285 10)) #x22))\n"
			   "(assert (= (select array (_ bv286 10)) #x66))\n"
			   "(assert (= (select array (_ bv287 10)) #xaa))\n"
			   "(assert (= (select array (_ bv288 10)) #xe5))\n"
			   "(assert (= (select array (_ bv289 10)) #x34))\n"
			   "(assert (= (select array (_ bv290 10)) #x5c))\n"
			   "(assert (= (select array (_ bv291 10)) #xe4))\n"
			   "(assert (= (select array (_ bv292 10)) #x37))\n"
			   "(assert (= (select array (_ bv293 10)) #x59))\n"
			   "(assert (= (select array (_ bv294 10)) #xeb))\n"
			   "(assert (= (select array (_ bv295 10)) #x26))\n"
			   "(assert (= (select array (_ bv296 10)) #x6a))\n"
			   "(assert (= (select array (_ bv297 10)) #xbe))\n"
			   "(assert (= (select array (_ bv298 10)) #xd9))\n"
			   "(assert (= (select array (_ bv299 10)) #x70))\n"
			   "(assert (= (select array (_ bv300 10)) #x90))\n"
			   "(assert (= (select array (_ bv301 10)) #xab))\n"
			   "(assert (= (select array (_ bv302 10)) #xe6))\n"
			   "(assert (= (select array (_ bv303 10)) #x31))\n"
			   "(assert (= (select array (_ bv304 10)) #x53))\n"
			   "(assert (= (select array (_ bv305 10)) #xf5))\n"
			   "(assert (= (select array (_ bv306 10)) #x04))\n"
			   "(assert (= (select array (_ bv307 10)) #x0c))\n"
			   "(assert (= (select array (_ bv308 10)) #x14))\n"
			   "(assert (= (select array (_ bv309 10)) #x3c))\n"
			   "(assert (= (select array (_ bv310 10)) #x44))\n"
			   "(assert (= (select array (_ bv311 10)) #xcc))\n"
			   "(assert (= (select array (_ bv312 10)) #x4f))\n"
			   "(assert (= (select array (_ bv313 10)) #xd1))\n"
			   "(assert (= (select array (_ bv314 10)) #x68))\n"
			   "(assert (= (select array (_ bv315 10)) #xb8))\n"
			   "(assert (= (select array (_ bv316 10)) #xd3))\n"
			   "(assert (= (select array (_ bv317 10)) #x6e))\n"
			   "(assert (= (select array (_ bv318 10)) #xb2))\n"
			   "(assert (= (select array (_ bv319 10)) #xcd))\n"
			   "(assert (= (select array (_ bv320 10)) #x4c))\n"
			   "(assert (= (select array (_ bv321 10)) #xd4))\n"
			   "(assert (= (select array (_ bv322 10)) #x67))\n"
			   "(assert (= (select array (_ bv323 10)) #xa9))\n"
			   "(assert (= (select array (_ bv324 10)) #xe0))\n"
			   "(assert (= (select array (_ bv325 10)) #x3b))\n"
			   "(assert (= (select array (_ bv326 10)) #x4d))\n"
			   "(assert (= (select array (_ bv327 10)) #xd7))\n"
			   "(assert (= (select array (_ bv328 10)) #x62))\n"
			   "(assert (= (select array (_ bv329 10)) #xa6))\n"
			   "(assert (= (select array (_ bv330 10)) #xf1))\n"
			   "(assert (= (select array (_ bv331 10)) #x08))\n"
			   "(assert (= (select array (_ bv332 10)) #x18))\n"
			   "(assert (= (select array (_ bv333 10)) #x28))\n"
			   "(assert (= (select array (_ bv334 10)) #x78))\n"
			   "(assert (= (select array (_ bv335 10)) #x88))\n"
			   "(assert (= (select array (_ bv336 10)) #x83))\n"
			   "(assert (= (select array (_ bv337 10)) #x9e))\n"
			   "(assert (= (select array (_ bv338 10)) #xb9))\n"
			   "(assert (= (select array (_ bv339 10)) #xd0))\n"
			   "(assert (= (select array (_ bv340 10)) #x6b))\n"
			   "(assert (= (select array (_ bv341 10)) #xbd))\n"
			   "(assert (= (select array (_ bv342 10)) #xdc))\n"
			   "(assert (= (select array (_ bv343 10)) #x7f))\n"
			   "(assert (= (select array (_ bv344 10)) #x81))\n"
			   "(assert (= (select array (_ bv345 10)) #x98))\n"
			   "(assert (= (select array (_ bv346 10)) #xb3))\n"
			   "(assert (= (select array (_ bv347 10)) #xce))\n"
			   "(assert (= (select array (_ bv348 10)) #x49))\n"
			   "(assert (= (select array (_ bv349 10)) #xdb))\n"
			   "(assert (= (select array (_ bv350 10)) #x76))\n"
			   "(assert (= (select array (_ bv351 10)) #x9a))\n"
			   "(assert (= (select array (_ bv352 10)) #xb5))\n"
			   "(assert (= (select array (_ bv353 10)) #xc4))\n"
			   "(assert (= (select array (_ bv354 10)) #x57))\n"
			   "(assert (= (select array (_ bv355 10)) #xf9))\n"
			   "(assert (= (select array (_ bv356 10)) #x10))\n"
			   "(assert (= (select array (_ bv357 10)) #x30))\n"
			   "(assert (= (select array (_ bv358 10)) #x50))\n"
			   "(assert (= (select array (_ bv359 10)) #xf0))\n"
			   "(assert (= (select array (_ bv360 10)) #x0b))\n"
			   "(assert (= (select array (_ bv361 10)) #x1d))\n"
			   "(assert (= (select array (_ bv362 10)) #x27))\n"
			   "(assert (= (select array (_ bv363 10)) #x69))\n"
			   "(assert (= (select array (_ bv364 10)) #xbb))\n"
			   "(assert (= (select array (_ bv365 10)) #xd6))\n"
			   "(assert (= (select array (_ bv366 10)) #x61))\n"
			   "(assert (= (select array (_ bv367 10)) #xa3))\n"
			   "(assert (= (select array (_ bv368 10)) #xfe))\n"
			   "(assert (= (select array (_ bv369 10)) #x19))\n"
			   "(assert (= (select array (_ bv370 10)) #x2b))\n"
			   "(assert (= (select array (_ bv371 10)) #x7d))\n"
			   "(assert (= (select array (_ bv372 10)) #x87))\n"
			   "(assert (= (select array (_ bv373 10)) #x92))\n"
			   "(assert (= (select array (_ bv374 10)) #xad))\n"
			   "(assert (= (select array (_ bv375 10)) #xec))\n"
			   "(assert (= (select array (_ bv376 10)) #x2f))\n"
			   "(assert (= (select array (_ bv377 10)) #x71))\n"
			   "(assert (= (select array (_ bv378 10)) #x93))\n"
			   "(assert (= (select array (_ bv379 10)) #xae))\n"
			   "(assert (= (select array (_ bv380 10)) #xe9))\n"
			   "(assert (= (select array (_ bv381 10)) #x20))\n"
			   "(assert (= (select array (_ bv382 10)) #x60))\n"
			   "(assert (= (select array (_ bv383 10)) #xa0))\n"
			   "(assert (= (select array (_ bv384 10)) #xfb))\n"
			   "(assert (= (select array (_ bv385 10)) #x16))\n"
			   "(assert (= (select array (_ bv386 10)) #x3a))\n"
			   "(assert (= (select array (_ bv387 10)) #x4e))\n"
			   "(assert (= (select array (_ bv388 10)) #xd2))\n"
			   "(assert (= (select array (_ bv389 10)) #x6d))\n"
			   "(assert (= (select array (_ bv390 10)) #xb7))\n"
			   "(assert (= (select array (_ bv391 10)) #xc2))\n"
			   "(assert (= (select array (_ bv392 10)) #x5d))\n"
			   "(assert (= (select array (_ bv393 10)) #xe7))\n"
			   "(assert (= (select array (_ bv394 10)) #x32))\n"
			   "(assert (= (select array (_ bv395 10)) #x56))\n"
			   "(assert (= (select array (_ bv396 10)) #xfa))\n"
			   "(assert (= (select array (_ bv397 10)) #x15))\n"
			   "(assert (= (select array (_ bv398 10)) #x3f))\n"
			   "(assert (= (select array (_ bv399 10)) #x41))\n"
			   "(assert (= (select array (_ bv400 10)) #xc3))\n"
			   "(assert (= (select array (_ bv401 10)) #x5e))\n"
			   "(assert (= (select array (_ bv402 10)) #xe2))\n"
			   "(assert (= (select array (_ bv403 10)) #x3d))\n"
			   "(assert (= (select array (_ bv404 10)) #x47))\n"
			   "(assert (= (select array (_ bv405 10)) #xc9))\n"
			   "(assert (= (select array (_ bv406 10)) #x40))\n"
			   "(assert (= (select array (_ bv407 10)) #xc0))\n"
			   "(assert (= (select array (_ bv408 10)) #x5b))\n"
			   "(assert (= (select array (_ bv409 10)) #xed))\n"
			   "(assert (= (select array (_ bv410 10)) #x2c))\n"
			   "(assert (= (select array (_ bv411 10)) #x74))\n"
			   "(assert (= (select array (_ bv412 10)) #x9c))\n"
			   "(assert (= (select array (_ bv413 10)) #xbf))\n"
			   "(assert (= (select array (_ bv414 10)) #xda))\n"
			   "(assert (= (select array (_ bv415 10)) #x75))\n"
			   "(assert (= (select array (_ bv416 10)) #x9f))\n"
			   "(assert (= (select array (_ bv417 10)) #xba))\n"
			   "(assert (= (select array (_ bv418 10)) #xd5))\n"
			   "(assert (= (select array (_ bv419 10)) #x64))\n"
			   "(assert (= (select array (_ bv420 10)) #xac))\n"
			   "(assert (= (select array (_ bv421 10)) #xef))\n"
			   "(assert (= (select array (_ bv422 10)) #x2a))\n"
			   "(assert (= (select array (_ bv423 10)) #x7e))\n"
			   "(assert (= (select array (_ bv424 10)) #x82))\n"
			   "(assert (= (select array (_ bv425 10)) #x9d))\n"
			   "(assert (= (select array (_ bv426 10)) #xbc))\n"
			   "(assert (= (select array (_ bv427 10)) #xdf))\n"
			   "(assert (= (select array (_ bv428 10)) #x7a))\n"
			   "(assert (= (select array (_ bv429 10)) #x8e))\n"
			   "(assert (= (select array (_ bv430 10)) #x89))\n"
			   "(assert (= (select array (_ bv431 10)) #x80))\n"
			   "(assert (= (select array (_ bv432 10)) #x9b))\n"
			   "(assert (= (select array (_ bv433 10)) #xb6))\n"
			   "(assert (= (select array (_ bv434 10)) #xc1))\n"
			   "(assert (= (select array (_ bv435 10)) #x58))\n"
			   "(assert (= (select array (_ bv436 10)) #xe8))\n"
			   "(assert (= (select array (_ bv437 10)) #x23))\n"
			   "(assert (= (select array (_ bv438 10)) #x65))\n"
			   "(assert (= (select array (_ bv439 10)) #xaf))\n"
			   "(assert (= (select array (_ bv440 10)) #xea))\n"
			   "(assert (= (select array (_ bv441 10)) #x25))\n"
			   "(assert (= (select array (_ bv442 10)) #x6f))\n"
			   "(assert (= (select array (_ bv443 10)) #xb1))\n"
			   "(assert (= (select array (_ bv444 10)) #xc8))\n"
			   "(assert (= (select array (_ bv445 10)) #x43))\n"
			   "(assert (= (select array (_ bv446 10)) #xc5))\n"
			   "(assert (= (select array (_ bv447 10)) #x54))\n"
			   "(assert (= (select array (_ bv448 10)) #xfc))\n"
			   "(assert (= (select array (_ bv449 10)) #x1f))\n"
			   "(assert (= (select array (_ bv450 10)) #x21))\n"
			   "(assert (= (select array (_ bv451 10)) #x63))\n"
			   "(assert (= (select array (_ bv452 10)) #xa5))\n"
			   "(assert (= (select array (_ bv453 10)) #xf4))\n"
			   "(assert (= (select array (_ bv454 10)) #x07))\n"
			   "(assert (= (select array (_ bv455 10)) #x09))\n"
			   "(assert (= (select array (_ bv456 10)) #x1b))\n"
			   "(assert (= (select array (_ bv457 10)) #x2d))\n"
			   "(assert (= (select array (_ bv458 10)) #x77))\n"
			   "(assert (= (select array (_ bv459 10)) #x99))\n"
			   "(assert (= (select array (_ bv460 10)) #xb0))\n"
			   "(assert (= (select array (_ bv461 10)) #xcb))\n"
			   "(assert (= (select array (_ bv462 10)) #x46))\n"
			   "(assert (= (select array (_ bv463 10)) #xca))\n"
			   "(assert (= (select array (_ bv464 10)) #x45))\n"
			   "(assert (= (select array (_ bv465 10)) #xcf))\n"
			   "(assert (= (select array (_ bv466 10)) #x4a))\n"
			   "(assert (= (select array (_ bv467 10)) #xde))\n"
			   "(assert (= (select array (_ bv468 10)) #x79))\n"
			   "(assert (= (select array (_ bv469 10)) #x8b))\n"
			   "(assert (= (select array (_ bv470 10)) #x86))\n"
			   "(assert (= (select array (_ bv471 10)) #x91))\n"
			   "(assert (= (select array (_ bv472 10)) #xa8))\n"
			   "(assert (= (select array (_ bv473 10)) #xe3))\n"
			   "(assert (= (select array (_ bv474 10)) #x3e))\n"
			   "(assert (= (select array (_ bv475 10)) #x42))\n"
			   "(assert (= (select array (_ bv476 10)) #xc6))\n"
			   "(assert (= (select array (_ bv477 10)) #x51))\n"
			   "(assert (= (select array (_ bv478 10)) #xf3))\n"
			   "(assert (= (select array (_ bv479 10)) #x0e))\n"
			   "(assert (= (select array (_ bv480 10)) #x12))\n"
			   "(assert (= (select array (_ bv481 10)) #x36))\n"
			   "(assert (= (select array (_ bv482 10)) #x5a))\n"
			   "(assert (= (select array (_ bv483 10)) #xee))\n"
			   "(assert (= (select array (_ bv484 10)) #x29))\n"
			   "(assert (= (select array (_ bv485 10)) #x7b))\n"
			   "(assert (= (select array (_ bv486 10)) #x8d))\n"
			   "(assert (= (select array (_ bv487 10)) #x8c))\n"
			   "(assert (= (select array (_ bv488 10)) #x8f))\n"
			   "(assert (= (select array (_ bv489 10)) #x8a))\n"
			   "(assert (= (select array (_ bv490 10)) #x85))\n"
			   "(assert (= (select array (_ bv491 10)) #x94))\n"
			   "(assert (= (select array (_ bv492 10)) #xa7))\n"
			   "(assert (= (select array (_ bv493 10)) #xf2))\n"
			   "(assert (= (select array (_ bv494 10)) #x0d))\n"
			   "(assert (= (select array (_ bv495 10)) #x17))\n"
			   "(assert (= (select array (_ bv496 10)) #x39))\n"
			   "(assert (= (select array (_ bv497 10)) #x4b))\n"
			   "(assert (= (select array (_ bv498 10)) #xdd))\n"
			   "(assert (= (select array (_ bv499 10)) #x7c))\n"
			   "(assert (= (select array (_ bv500 10)) #x84))\n"
			   "(assert (= (select array (_ bv501 10)) #x97))\n"
			   "(assert (= (select array (_ bv502 10)) #xa2))\n"
			   "(assert (= (select array (_ bv503 10)) #xfd))\n"
			   "(assert (= (select array (_ bv504 10)) #x1c))\n"
			   "(assert (= (select array (_ bv505 10)) #x24))\n"
			   "(assert (= (select array (_ bv506 10)) #x6c))\n"
			   "(assert (= (select array (_ bv507 10)) #xb4))\n"
			   "(assert (= (select array (_ bv508 10)) #xc7))\n"
			   "(assert (= (select array (_ bv509 10)) #x52))\n"
			   "(assert (= (select array (_ bv510 10)) #xf6))\n"
			   "(assert (= (select array (_ bv511 10)) #x01))\n"
			   "(assert (= (select array (_ bv512 10)) #x03))\n"
			   "(assert (= (select array (_ bv513 10)) #x05))\n"
			   "(assert (= (select array (_ bv514 10)) #x0f))\n"
			   "(assert (= (select array (_ bv515 10)) #x11))\n"
			   "(assert (= (select array (_ bv516 10)) #x33))\n"
			   "(assert (= (select array (_ bv517 10)) #x55))\n"
			   "(assert (= (select array (_ bv518 10)) #xff))\n"
			   "(assert (= (select array (_ bv519 10)) #x1a))\n"
			   "(assert (= (select array (_ bv520 10)) #x2e))\n"
			   "(assert (= (select array (_ bv521 10)) #x72))\n"
			   "(assert (= (select array (_ bv522 10)) #x96))\n"
			   "(assert (= (select array (_ bv523 10)) #xa1))\n"
			   "(assert (= (select array (_ bv524 10)) #xf8))\n"
			   "(assert (= (select array (_ bv525 10)) #x13))\n"
			   "(assert (= (select array (_ bv526 10)) #x35))\n"
			   "(assert (= (select array (_ bv527 10)) #x5f))\n"
			   "(assert (= (select array (_ bv528 10)) #xe1))\n"
			   "(assert (= (select array (_ bv529 10)) #x38))\n"
			   "(assert (= (select array (_ bv530 10)) #x48))\n"
			   "(assert (= (select array (_ bv531 10)) #xd8))\n"
			   "(assert (= (select array (_ bv532 10)) #x73))\n"
			   "(assert (= (select array (_ bv533 10)) #x95))\n"
			   "(assert (= (select array (_ bv534 10)) #xa4))\n"
			   "(assert (= (select array (_ bv535 10)) #xf7))\n"
			   "(assert (= (select array (_ bv536 10)) #x02))\n"
			   "(assert (= (select array (_ bv537 10)) #x06))\n"
			   "(assert (= (select array (_ bv538 10)) #x0a))\n"
			   "(assert (= (select array (_ bv539 10)) #x1e))\n"
			   "(assert (= (select array (_ bv540 10)) #x22))\n"
			   "(assert (= (select array (_ bv541 10)) #x66))\n"
			   "(assert (= (select array (_ bv542 10)) #xaa))\n"
			   "(assert (= (select array (_ bv543 10)) #xe5))\n"
			   "(assert (= (select array (_ bv544 10)) #x34))\n"
			   "(assert (= (select array (_ bv545 10)) #x5c))\n"
			   "(assert (= (select array (_ bv546 10)) #xe4))\n"
			   "(assert (= (select array (_ bv547 10)) #x37))\n"
			   "(assert (= (select array (_ bv548 10)) #x59))\n"
			   "(assert (= (select array (_ bv549 10)) #xeb))\n"
			   "(assert (= (select array (_ bv550 10)) #x26))\n"
			   "(assert (= (select array (_ bv551 10)) #x6a))\n"
			   "(assert (= (select array (_ bv552 10)) #xbe))\n"
			   "(assert (= (select array (_ bv553 10)) #xd9))\n"
			   "(assert (= (select array (_ bv554 10)) #x70))\n"
			   "(assert (= (select array (_ bv555 10)) #x90))\n"
			   "(assert (= (select array (_ bv556 10)) #xab))\n"
			   "(assert (= (select array (_ bv557 10)) #xe6))\n"
			   "(assert (= (select array (_ bv558 10)) #x31))\n"
			   "(assert (= (select array (_ bv559 10)) #x53))\n"
			   "(assert (= (select array (_ bv560 10)) #xf5))\n"
			   "(assert (= (select array (_ bv561 10)) #x04))\n"
			   "(assert (= (select array (_ bv562 10)) #x0c))\n"
			   "(assert (= (select array (_ bv563 10)) #x14))\n"
			   "(assert (= (select array (_ bv564 10)) #x3c))\n"
			   "(assert (= (select array (_ bv565 10)) #x44))\n"
			   "(assert (= (select array (_ bv566 10)) #xcc))\n"
			   "(assert (= (select array (_ bv567 10)) #x4f))\n"
			   "(assert (= (select array (_ bv568 10)) #xd1))\n"
			   "(assert (= (select array (_ bv569 10)) #x68))\n"
			   "(assert (= (select array (_ bv570 10)) #xb8))\n"
			   "(assert (= (select array (_ bv571 10)) #xd3))\n"
			   "(assert (= (select array (_ bv572 10)) #x6e))\n"
			   "(assert (= (select array (_ bv573 10)) #xb2))\n"
			   "(assert (= (select array (_ bv574 10)) #xcd))\n"
			   "(assert (= (select array (_ bv575 10)) #x4c))\n"
			   "(assert (= (select array (_ bv576 10)) #xd4))\n"
			   "(assert (= (select array (_ bv577 10)) #x67))\n"
			   "(assert (= (select array (_ bv578 10)) #xa9))\n"
			   "(assert (= (select array (_ bv579 10)) #xe0))\n"
			   "(assert (= (select array (_ bv580 10)) #x3b))\n"
			   "(assert (= (select array (_ bv581 10)) #x4d))\n"
			   "(assert (= (select array (_ bv582 10)) #xd7))\n"
			   "(assert (= (select array (_ bv583 10)) #x62))\n"
			   "(assert (= (select array (_ bv584 10)) #xa6))\n"
			   "(assert (= (select array (_ bv585 10)) #xf1))\n"
			   "(assert (= (select array (_ bv586 10)) #x08))\n"
			   "(assert (= (select array (_ bv587 10)) #x18))\n"
			   "(assert (= (select array (_ bv588 10)) #x28))\n"
			   "(assert (= (select array (_ bv589 10)) #x78))\n"
			   "(assert (= (select array (_ bv590 10)) #x88))\n"
			   "(assert (= (select array (_ bv591 10)) #x83))\n"
			   "(assert (= (select array (_ bv592 10)) #x9e))\n"
			   "(assert (= (select array (_ bv593 10)) #xb9))\n"
			   "(assert (= (select array (_ bv594 10)) #xd0))\n"
			   "(assert (= (select array (_ bv595 10)) #x6b))\n"
			   "(assert (= (select array (_ bv596 10)) #xbd))\n"
			   "(assert (= (select array (_ bv597 10)) #xdc))\n"
			   "(assert (= (select array (_ bv598 10)) #x7f))\n"
			   "(assert (= (select array (_ bv599 10)) #x81))\n"
			   "(assert (= (select array (_ bv600 10)) #x98))\n"
			   "(assert (= (select array (_ bv601 10)) #xb3))\n"
			   "(assert (= (select array (_ bv602 10)) #xce))\n"
			   "(assert (= (select array (_ bv603 10)) #x49))\n"
			   "(assert (= (select array (_ bv604 10)) #xdb))\n"
			   "(assert (= (select array (_ bv605 10)) #x76))\n"
			   "(assert (= (select array (_ bv606 10)) #x9a))\n"
			   "(assert (= (select array (_ bv607 10)) #xb5))\n"
			   "(assert (= (select array (_ bv608 10)) #xc4))\n"
			   "(assert (= (select array (_ bv609 10)) #x57))\n"
			   "(assert (= (select array (_ bv610 10)) #xf9))\n"
			   "(assert (= (select array (_ bv611 10)) #x10))\n"
			   "(assert (= (select array (_ bv612 10)) #x30))\n"
			   "(assert (= (select array (_ bv613 10)) #x50))\n"
			   "(assert (= (select array (_ bv614 10)) #xf0))\n"
			   "(assert (= (select array (_ bv615 10)) #x0b))\n"
			   "(assert (= (select array (_ bv616 10)) #x1d))\n"
			   "(assert (= (select array (_ bv617 10)) #x27))\n"
			   "(assert (= (select array (_ bv618 10)) #x69))\n"
			   "(assert (= (select array (_ bv619 10)) #xbb))\n"
			   "(assert (= (select array (_ bv620 10)) #xd6))\n"
			   "(assert (= (select array (_ bv621 10)) #x61))\n"
			   "(assert (= (select array (_ bv622 10)) #xa3))\n"
			   "(assert (= (select array (_ bv623 10)) #xfe))\n"
			   "(assert (= (select array (_ bv624 10)) #x19))\n"
			   "(assert (= (select array (_ bv625 10)) #x2b))\n"
			   "(assert (= (select array (_ bv626 10)) #x7d))\n"
			   "(assert (= (select array (_ bv627 10)) #x87))\n"
			   "(assert (= (select array (_ bv628 10)) #x92))\n"
			   "(assert (= (select array (_ bv629 10)) #xad))\n"
			   "(assert (= (select array (_ bv630 10)) #xec))\n"
			   "(assert (= (select array (_ bv631 10)) #x2f))\n"
			   "(assert (= (select array (_ bv632 10)) #x71))\n"
			   "(assert (= (select array (_ bv633 10)) #x93))\n"
			   "(assert (= (select array (_ bv634 10)) #xae))\n"
			   "(assert (= (select array (_ bv635 10)) #xe9))\n"
			   "(assert (= (select array (_ bv636 10)) #x20))\n"
			   "(assert (= (select array (_ bv637 10)) #x60))\n"
			   "(assert (= (select array (_ bv638 10)) #xa0))\n"
			   "(assert (= (select array (_ bv639 10)) #xfb))\n"
			   "(assert (= (select array (_ bv640 10)) #x16))\n"
			   "(assert (= (select array (_ bv641 10)) #x3a))\n"
			   "(assert (= (select array (_ bv642 10)) #x4e))\n"
			   "(assert (= (select array (_ bv643 10)) #xd2))\n"
			   "(assert (= (select array (_ bv644 10)) #x6d))\n"
			   "(assert (= (select array (_ bv645 10)) #xb7))\n"
			   "(assert (= (select array (_ bv646 10)) #xc2))\n"
			   "(assert (= (select array (_ bv647 10)) #x5d))\n"
			   "(assert (= (select array (_ bv648 10)) #xe7))\n"
			   "(assert (= (select array (_ bv649 10)) #x32))\n"
			   "(assert (= (select array (_ bv650 10)) #x56))\n"
			   "(assert (= (select array (_ bv651 10)) #xfa))\n"
			   "(assert (= (select array (_ bv652 10)) #x15))\n"
			   "(assert (= (select array (_ bv653 10)) #x3f))\n"
			   "(assert (= (select array (_ bv654 10)) #x41))\n"
			   "(assert (= (select array (_ bv655 10)) #xc3))\n"
			   "(assert (= (select array (_ bv656 10)) #x5e))\n"
			   "(assert (= (select array (_ bv657 10)) #xe2))\n"
			   "(assert (= (select array (_ bv658 10)) #x3d))\n"
			   "(assert (= (select array (_ bv659 10)) #x47))\n"
			   "(assert (= (select array (_ bv660 10)) #xc9))\n"
			   "(assert (= (select array (_ bv661 10)) #x40))\n"
			   "(assert (= (select array (_ bv662 10)) #xc0))\n"
			   "(assert (= (select array (_ bv663 10)) #x5b))\n"
			   "(assert (= (select array (_ bv664 10)) #xed))\n"
			   "(assert (= (select array (_ bv665 10)) #x2c))\n"
			   "(assert (= (select array (_ bv666 10)) #x74))\n"
			   "(assert (= (select array (_ bv667 10)) #x9c))\n"
			   "(assert (= (select array (_ bv668 10)) #xbf))\n"
			   "(assert (= (select array (_ bv669 10)) #xda))\n"
			   "(assert (= (select array (_ bv670 10)) #x75))\n"
			   "(assert (= (select array (_ bv671 10)) #x9f))\n"
			   "(assert (= (select array (_ bv672 10)) #xba))\n"
			   "(assert (= (select array (_ bv673 10)) #xd5))\n"
			   "(assert (= (select array (_ bv674 10)) #x64))\n"
			   "(assert (= (select array (_ bv675 10)) #xac))\n"
			   "(assert (= (select array (_ bv676 10)) #xef))\n"
			   "(assert (= (select array (_ bv677 10)) #x2a))\n"
			   "(assert (= (select array (_ bv678 10)) #x7e))\n"
			   "(assert (= (select array (_ bv679 10)) #x82))\n"
			   "(assert (= (select array (_ bv680 10)) #x9d))\n"
			   "(assert (= (select array (_ bv681 10)) #xbc))\n"
			   "(assert (= (select array (_ bv682 10)) #xdf))\n"
			   "(assert (= (select array (_ bv683 10)) #x7a))\n"
			   "(assert (= (select array (_ bv684 10)) #x8e))\n"
			   "(assert (= (select array (_ bv685 10)) #x89))\n"
			   "(assert (= (select array (_ bv686 10)) #x80))\n"
			   "(assert (= (select array (_ bv687 10)) #x9b))\n"
			   "(assert (= (select array (_ bv688 10)) #xb6))\n"
			   "(assert (= (select array (_ bv689 10)) #xc1))\n"
			   "(assert (= (select array (_ bv690 10)) #x58))\n"
			   "(assert (= (select array (_ bv691 10)) #xe8))\n"
			   "(assert (= (select array (_ bv692 10)) #x23))\n"
			   "(assert (= (select array (_ bv693 10)) #x65))\n"
			   "(assert (= (select array (_ bv694 10)) #xaf))\n"
			   "(assert (= (select array (_ bv695 10)) #xea))\n"
			   "(assert (= (select array (_ bv696 10)) #x25))\n"
			   "(assert (= (select array (_ bv697 10)) #x6f))\n"
			   "(assert (= (select array (_ bv698 10)) #xb1))\n"
			   "(assert (= (select array (_ bv699 10)) #xc8))\n"
			   "(assert (= (select array (_ bv700 10)) #x43))\n"
			   "(assert (= (select array (_ bv701 10)) #xc5))\n"
			   "(assert (= (select array (_ bv702 10)) #x54))\n"
			   "(assert (= (select array (_ bv703 10)) #xfc))\n"
			   "(assert (= (select array (_ bv704 10)) #x1f))\n"
			   "(assert (= (select array (_ bv705 10)) #x21))\n"
			   "(assert (= (select array (_ bv706 10)) #x63))\n"
			   "(assert (= (select array (_ bv707 10)) #xa5))\n"
			   "(assert (= (select array (_ bv708 10)) #xf4))\n"
			   "(assert (= (select array (_ bv709 10)) #x07))\n"
			   "(assert (= (select array (_ bv710 10)) #x09))\n"
			   "(assert (= (select array (_ bv711 10)) #x1b))\n"
			   "(assert (= (select array (_ bv712 10)) #x2d))\n"
			   "(assert (= (select array (_ bv713 10)) #x77))\n"
			   "(assert (= (select array (_ bv714 10)) #x99))\n"
			   "(assert (= (select array (_ bv715 10)) #xb0))\n"
			   "(assert (= (select array (_ bv716 10)) #xcb))\n"
			   "(assert (= (select array (_ bv717 10)) #x46))\n"
			   "(assert (= (select array (_ bv718 10)) #xca))\n"
			   "(assert (= (select array (_ bv719 10)) #x45))\n"
			   "(assert (= (select array (_ bv720 10)) #xcf))\n"
			   "(assert (= (select array (_ bv721 10)) #x4a))\n"
			   "(assert (= (select array (_ bv722 10)) #xde))\n"
			   "(assert (= (select array (_ bv723 10)) #x79))\n"
			   "(assert (= (select array (_ bv724 10)) #x8b))\n"
			   "(assert (= (select array (_ bv725 10)) #x86))\n"
			   "(assert (= (select array (_ bv726 10)) #x91))\n"
			   "(assert (= (select array (_ bv727 10)) #xa8))\n"
			   "(assert (= (select array (_ bv728 10)) #xe3))\n"
			   "(assert (= (select array (_ bv729 10)) #x3e))\n"
			   "(assert (= (select array (_ bv730 10)) #x42))\n"
			   "(assert (= (select array (_ bv731 10)) #xc6))\n"
			   "(assert (= (select array (_ bv732 10)) #x51))\n"
			   "(assert (= (select array (_ bv733 10)) #xf3))\n"
			   "(assert (= (select array (_ bv734 10)) #x0e))\n"
			   "(assert (= (select array (_ bv735 10)) #x12))\n"
			   "(assert (= (select array (_ bv736 10)) #x36))\n"
			   "(assert (= (select array (_ bv737 10)) #x5a))\n"
			   "(assert (= (select array (_ bv738 10)) #xee))\n"
			   "(assert (= (select array (_ bv739 10)) #x29))\n"
			   "(assert (= (select array (_ bv740 10)) #x7b))\n"
			   "(assert (= (select array (_ bv741 10)) #x8d))\n"
			   "(assert (= (select array (_ bv742 10)) #x8c))\n"
			   "(assert (= (select array (_ bv743 10)) #x8f))\n"
			   "(assert (= (select array (_ bv744 10)) #x8a))\n"
			   "(assert (= (select array (_ bv745 10)) #x85))\n"
			   "(assert (= (select array (_ bv746 10)) #x94))\n"
			   "(assert (= (select array (_ bv747 10)) #xa7))\n"
			   "(assert (= (select array (_ bv748 10)) #xf2))\n"
			   "(assert (= (select array (_ bv749 10)) #x0d))\n"
			   "(assert (= (select array (_ bv750 10)) #x17))\n"
			   "(assert (= (select array (_ bv751 10)) #x39))\n"
			   "(assert (= (select array (_ bv752 10)) #x4b))\n"
			   "(assert (= (select array (_ bv753 10)) #xdd))\n"
			   "(assert (= (select array (_ bv754 10)) #x7c))\n"
			   "(assert (= (select array (_ bv755 10)) #x84))\n"
			   "(assert (= (select array (_ bv756 10)) #x97))\n"
			   "(assert (= (select array (_ bv757 10)) #xa2))\n"
			   "(assert (= (select array (_ bv758 10)) #xfd))\n"
			   "(assert (= (select array (_ bv759 10)) #x1c))\n"
			   "(assert (= (select array (_ bv760 10)) #x24))\n"
			   "(assert (= (select array (_ bv761 10)) #x6c))\n"
			   "(assert (= (select array (_ bv762 10)) #xb4))\n"
			   "(assert (= (select array (_ bv763 10)) #xc7))\n"
			   "(assert (= (select array (_ bv764 10)) #x52))\n"
			   "(assert (= (select array (_ bv765 10)) #xf6))\n"
			   "(assert (= (select array (_ bv766 10)) #x01))\n"
			   "(define-fun bvgfmul ((x (_ BitVec 8)) (y (_ BitVec 8)) )(_ BitVec 8)\n"
			   "  (ite (or (= x #x00) (= y #x00)) #x00 \n"
			   "    (select array (bvadd (_ bv256 10) ( bvadd   ((_ zero_extend 2)(select array ((_ zero_extend 2)x) )) ((_ zero_extend 2) (select array  ((_ zero_extend 2)y) ))  ) ))\n"
			   "  )\n"
			   ")\n";
	}

	static string bvgfmulDefinationITE() {
		return "(define-fun table ( (x (_ BitVec 10) ) ) (_ BitVec 8)\n"
						  "(ite  (= x (_ bv0 10)) #x00\n"
						  "(ite  (= x (_ bv1 10)) #x00\n"
						  "(ite  (= x (_ bv2 10)) #x19\n"
						  "(ite  (= x (_ bv3 10)) #x01\n"
						  "(ite  (= x (_ bv4 10)) #x32\n"
						  "(ite  (= x (_ bv5 10)) #x02\n"
						  "(ite  (= x (_ bv6 10)) #x1a\n"
						  "(ite  (= x (_ bv7 10)) #xc6\n"
						  "(ite  (= x (_ bv8 10)) #x4b\n"
						  "(ite  (= x (_ bv9 10)) #xc7\n"
						  "(ite  (= x (_ bv10 10)) #x1b\n"
						  "(ite  (= x (_ bv11 10)) #x68\n"
						  "(ite  (= x (_ bv12 10)) #x33\n"
						  "(ite  (= x (_ bv13 10)) #xee\n"
						  "(ite  (= x (_ bv14 10)) #xdf\n"
						  "(ite  (= x (_ bv15 10)) #x03\n"
						  "(ite  (= x (_ bv16 10)) #x64\n"
						  "(ite  (= x (_ bv17 10)) #x04\n"
						  "(ite  (= x (_ bv18 10)) #xe0\n"
						  "(ite  (= x (_ bv19 10)) #x0e\n"
						  "(ite  (= x (_ bv20 10)) #x34\n"
						  "(ite  (= x (_ bv21 10)) #x8d\n"
						  "(ite  (= x (_ bv22 10)) #x81\n"
						  "(ite  (= x (_ bv23 10)) #xef\n"
						  "(ite  (= x (_ bv24 10)) #x4c\n"
						  "(ite  (= x (_ bv25 10)) #x71\n"
						  "(ite  (= x (_ bv26 10)) #x08\n"
						  "(ite  (= x (_ bv27 10)) #xc8\n"
						  "(ite  (= x (_ bv28 10)) #xf8\n"
						  "(ite  (= x (_ bv29 10)) #x69\n"
						  "(ite  (= x (_ bv30 10)) #x1c\n"
						  "(ite  (= x (_ bv31 10)) #xc1\n"
						  "(ite  (= x (_ bv32 10)) #x7d\n"
						  "(ite  (= x (_ bv33 10)) #xc2\n"
						  "(ite  (= x (_ bv34 10)) #x1d\n"
						  "(ite  (= x (_ bv35 10)) #xb5\n"
						  "(ite  (= x (_ bv36 10)) #xf9\n"
						  "(ite  (= x (_ bv37 10)) #xb9\n"
						  "(ite  (= x (_ bv38 10)) #x27\n"
						  "(ite  (= x (_ bv39 10)) #x6a\n"
						  "(ite  (= x (_ bv40 10)) #x4d\n"
						  "(ite  (= x (_ bv41 10)) #xe4\n"
						  "(ite  (= x (_ bv42 10)) #xa6\n"
						  "(ite  (= x (_ bv43 10)) #x72\n"
						  "(ite  (= x (_ bv44 10)) #x9a\n"
						  "(ite  (= x (_ bv45 10)) #xc9\n"
						  "(ite  (= x (_ bv46 10)) #x09\n"
						  "(ite  (= x (_ bv47 10)) #x78\n"
						  "(ite  (= x (_ bv48 10)) #x65\n"
						  "(ite  (= x (_ bv49 10)) #x2f\n"
						  "(ite  (= x (_ bv50 10)) #x8a\n"
						  "(ite  (= x (_ bv51 10)) #x05\n"
						  "(ite  (= x (_ bv52 10)) #x21\n"
						  "(ite  (= x (_ bv53 10)) #x0f\n"
						  "(ite  (= x (_ bv54 10)) #xe1\n"
						  "(ite  (= x (_ bv55 10)) #x24\n"
						  "(ite  (= x (_ bv56 10)) #x12\n"
						  "(ite  (= x (_ bv57 10)) #xf0\n"
						  "(ite  (= x (_ bv58 10)) #x82\n"
						  "(ite  (= x (_ bv59 10)) #x45\n"
						  "(ite  (= x (_ bv60 10)) #x35\n"
						  "(ite  (= x (_ bv61 10)) #x93\n"
						  "(ite  (= x (_ bv62 10)) #xda\n"
						  "(ite  (= x (_ bv63 10)) #x8e\n"
						  "(ite  (= x (_ bv64 10)) #x96\n"
						  "(ite  (= x (_ bv65 10)) #x8f\n"
						  "(ite  (= x (_ bv66 10)) #xdb\n"
						  "(ite  (= x (_ bv67 10)) #xbd\n"
						  "(ite  (= x (_ bv68 10)) #x36\n"
						  "(ite  (= x (_ bv69 10)) #xd0\n"
						  "(ite  (= x (_ bv70 10)) #xce\n"
						  "(ite  (= x (_ bv71 10)) #x94\n"
						  "(ite  (= x (_ bv72 10)) #x13\n"
						  "(ite  (= x (_ bv73 10)) #x5c\n"
						  "(ite  (= x (_ bv74 10)) #xd2\n"
						  "(ite  (= x (_ bv75 10)) #xf1\n"
						  "(ite  (= x (_ bv76 10)) #x40\n"
						  "(ite  (= x (_ bv77 10)) #x46\n"
						  "(ite  (= x (_ bv78 10)) #x83\n"
						  "(ite  (= x (_ bv79 10)) #x38\n"
						  "(ite  (= x (_ bv80 10)) #x66\n"
						  "(ite  (= x (_ bv81 10)) #xdd\n"
						  "(ite  (= x (_ bv82 10)) #xfd\n"
						  "(ite  (= x (_ bv83 10)) #x30\n"
						  "(ite  (= x (_ bv84 10)) #xbf\n"
						  "(ite  (= x (_ bv85 10)) #x06\n"
						  "(ite  (= x (_ bv86 10)) #x8b\n"
						  "(ite  (= x (_ bv87 10)) #x62\n"
						  "(ite  (= x (_ bv88 10)) #xb3\n"
						  "(ite  (= x (_ bv89 10)) #x25\n"
						  "(ite  (= x (_ bv90 10)) #xe2\n"
						  "(ite  (= x (_ bv91 10)) #x98\n"
						  "(ite  (= x (_ bv92 10)) #x22\n"
						  "(ite  (= x (_ bv93 10)) #x88\n"
						  "(ite  (= x (_ bv94 10)) #x91\n"
						  "(ite  (= x (_ bv95 10)) #x10\n"
						  "(ite  (= x (_ bv96 10)) #x7e\n"
						  "(ite  (= x (_ bv97 10)) #x6e\n"
						  "(ite  (= x (_ bv98 10)) #x48\n"
						  "(ite  (= x (_ bv99 10)) #xc3\n"
						  "(ite  (= x (_ bv100 10)) #xa3\n"
						  "(ite  (= x (_ bv101 10)) #xb6\n"
						  "(ite  (= x (_ bv102 10)) #x1e\n"
						  "(ite  (= x (_ bv103 10)) #x42\n"
						  "(ite  (= x (_ bv104 10)) #x3a\n"
						  "(ite  (= x (_ bv105 10)) #x6b\n"
						  "(ite  (= x (_ bv106 10)) #x28\n"
						  "(ite  (= x (_ bv107 10)) #x54\n"
						  "(ite  (= x (_ bv108 10)) #xfa\n"
						  "(ite  (= x (_ bv109 10)) #x85\n"
						  "(ite  (= x (_ bv110 10)) #x3d\n"
						  "(ite  (= x (_ bv111 10)) #xba\n"
						  "(ite  (= x (_ bv112 10)) #x2b\n"
						  "(ite  (= x (_ bv113 10)) #x79\n"
						  "(ite  (= x (_ bv114 10)) #x0a\n"
						  "(ite  (= x (_ bv115 10)) #x15\n"
						  "(ite  (= x (_ bv116 10)) #x9b\n"
						  "(ite  (= x (_ bv117 10)) #x9f\n"
						  "(ite  (= x (_ bv118 10)) #x5e\n"
						  "(ite  (= x (_ bv119 10)) #xca\n"
						  "(ite  (= x (_ bv120 10)) #x4e\n"
						  "(ite  (= x (_ bv121 10)) #xd4\n"
						  "(ite  (= x (_ bv122 10)) #xac\n"
						  "(ite  (= x (_ bv123 10)) #xe5\n"
						  "(ite  (= x (_ bv124 10)) #xf3\n"
						  "(ite  (= x (_ bv125 10)) #x73\n"
						  "(ite  (= x (_ bv126 10)) #xa7\n"
						  "(ite  (= x (_ bv127 10)) #x57\n"
						  "(ite  (= x (_ bv128 10)) #xaf\n"
						  "(ite  (= x (_ bv129 10)) #x58\n"
						  "(ite  (= x (_ bv130 10)) #xa8\n"
						  "(ite  (= x (_ bv131 10)) #x50\n"
						  "(ite  (= x (_ bv132 10)) #xf4\n"
						  "(ite  (= x (_ bv133 10)) #xea\n"
						  "(ite  (= x (_ bv134 10)) #xd6\n"
						  "(ite  (= x (_ bv135 10)) #x74\n"
						  "(ite  (= x (_ bv136 10)) #x4f\n"
						  "(ite  (= x (_ bv137 10)) #xae\n"
						  "(ite  (= x (_ bv138 10)) #xe9\n"
						  "(ite  (= x (_ bv139 10)) #xd5\n"
						  "(ite  (= x (_ bv140 10)) #xe7\n"
						  "(ite  (= x (_ bv141 10)) #xe6\n"
						  "(ite  (= x (_ bv142 10)) #xad\n"
						  "(ite  (= x (_ bv143 10)) #xe8\n"
						  "(ite  (= x (_ bv144 10)) #x2c\n"
						  "(ite  (= x (_ bv145 10)) #xd7\n"
						  "(ite  (= x (_ bv146 10)) #x75\n"
						  "(ite  (= x (_ bv147 10)) #x7a\n"
						  "(ite  (= x (_ bv148 10)) #xeb\n"
						  "(ite  (= x (_ bv149 10)) #x16\n"
						  "(ite  (= x (_ bv150 10)) #x0b\n"
						  "(ite  (= x (_ bv151 10)) #xf5\n"
						  "(ite  (= x (_ bv152 10)) #x59\n"
						  "(ite  (= x (_ bv153 10)) #xcb\n"
						  "(ite  (= x (_ bv154 10)) #x5f\n"
						  "(ite  (= x (_ bv155 10)) #xb0\n"
						  "(ite  (= x (_ bv156 10)) #x9c\n"
						  "(ite  (= x (_ bv157 10)) #xa9\n"
						  "(ite  (= x (_ bv158 10)) #x51\n"
						  "(ite  (= x (_ bv159 10)) #xa0\n"
						  "(ite  (= x (_ bv160 10)) #x7f\n"
						  "(ite  (= x (_ bv161 10)) #x0c\n"
						  "(ite  (= x (_ bv162 10)) #xf6\n"
						  "(ite  (= x (_ bv163 10)) #x6f\n"
						  "(ite  (= x (_ bv164 10)) #x17\n"
						  "(ite  (= x (_ bv165 10)) #xc4\n"
						  "(ite  (= x (_ bv166 10)) #x49\n"
						  "(ite  (= x (_ bv167 10)) #xec\n"
						  "(ite  (= x (_ bv168 10)) #xd8\n"
						  "(ite  (= x (_ bv169 10)) #x43\n"
						  "(ite  (= x (_ bv170 10)) #x1f\n"
						  "(ite  (= x (_ bv171 10)) #x2d\n"
						  "(ite  (= x (_ bv172 10)) #xa4\n"
						  "(ite  (= x (_ bv173 10)) #x76\n"
						  "(ite  (= x (_ bv174 10)) #x7b\n"
						  "(ite  (= x (_ bv175 10)) #xb7\n"
						  "(ite  (= x (_ bv176 10)) #xcc\n"
						  "(ite  (= x (_ bv177 10)) #xbb\n"
						  "(ite  (= x (_ bv178 10)) #x3e\n"
						  "(ite  (= x (_ bv179 10)) #x5a\n"
						  "(ite  (= x (_ bv180 10)) #xfb\n"
						  "(ite  (= x (_ bv181 10)) #x60\n"
						  "(ite  (= x (_ bv182 10)) #xb1\n"
						  "(ite  (= x (_ bv183 10)) #x86\n"
						  "(ite  (= x (_ bv184 10)) #x3b\n"
						  "(ite  (= x (_ bv185 10)) #x52\n"
						  "(ite  (= x (_ bv186 10)) #xa1\n"
						  "(ite  (= x (_ bv187 10)) #x6c\n"
						  "(ite  (= x (_ bv188 10)) #xaa\n"
						  "(ite  (= x (_ bv189 10)) #x55\n"
						  "(ite  (= x (_ bv190 10)) #x29\n"
						  "(ite  (= x (_ bv191 10)) #x9d\n"
						  "(ite  (= x (_ bv192 10)) #x97\n"
						  "(ite  (= x (_ bv193 10)) #xb2\n"
						  "(ite  (= x (_ bv194 10)) #x87\n"
						  "(ite  (= x (_ bv195 10)) #x90\n"
						  "(ite  (= x (_ bv196 10)) #x61\n"
						  "(ite  (= x (_ bv197 10)) #xbe\n"
						  "(ite  (= x (_ bv198 10)) #xdc\n"
						  "(ite  (= x (_ bv199 10)) #xfc\n"
						  "(ite  (= x (_ bv200 10)) #xbc\n"
						  "(ite  (= x (_ bv201 10)) #x95\n"
						  "(ite  (= x (_ bv202 10)) #xcf\n"
						  "(ite  (= x (_ bv203 10)) #xcd\n"
						  "(ite  (= x (_ bv204 10)) #x37\n"
						  "(ite  (= x (_ bv205 10)) #x3f\n"
						  "(ite  (= x (_ bv206 10)) #x5b\n"
						  "(ite  (= x (_ bv207 10)) #xd1\n"
						  "(ite  (= x (_ bv208 10)) #x53\n"
						  "(ite  (= x (_ bv209 10)) #x39\n"
						  "(ite  (= x (_ bv210 10)) #x84\n"
						  "(ite  (= x (_ bv211 10)) #x3c\n"
						  "(ite  (= x (_ bv212 10)) #x41\n"
						  "(ite  (= x (_ bv213 10)) #xa2\n"
						  "(ite  (= x (_ bv214 10)) #x6d\n"
						  "(ite  (= x (_ bv215 10)) #x47\n"
						  "(ite  (= x (_ bv216 10)) #x14\n"
						  "(ite  (= x (_ bv217 10)) #x2a\n"
						  "(ite  (= x (_ bv218 10)) #x9e\n"
						  "(ite  (= x (_ bv219 10)) #x5d\n"
						  "(ite  (= x (_ bv220 10)) #x56\n"
						  "(ite  (= x (_ bv221 10)) #xf2\n"
						  "(ite  (= x (_ bv222 10)) #xd3\n"
						  "(ite  (= x (_ bv223 10)) #xab\n"
						  "(ite  (= x (_ bv224 10)) #x44\n"
						  "(ite  (= x (_ bv225 10)) #x11\n"
						  "(ite  (= x (_ bv226 10)) #x92\n"
						  "(ite  (= x (_ bv227 10)) #xd9\n"
						  "(ite  (= x (_ bv228 10)) #x23\n"
						  "(ite  (= x (_ bv229 10)) #x20\n"
						  "(ite  (= x (_ bv230 10)) #x2e\n"
						  "(ite  (= x (_ bv231 10)) #x89\n"
						  "(ite  (= x (_ bv232 10)) #xb4\n"
						  "(ite  (= x (_ bv233 10)) #x7c\n"
						  "(ite  (= x (_ bv234 10)) #xb8\n"
						  "(ite  (= x (_ bv235 10)) #x26\n"
						  "(ite  (= x (_ bv236 10)) #x77\n"
						  "(ite  (= x (_ bv237 10)) #x99\n"
						  "(ite  (= x (_ bv238 10)) #xe3\n"
						  "(ite  (= x (_ bv239 10)) #xa5\n"
						  "(ite  (= x (_ bv240 10)) #x67\n"
						  "(ite  (= x (_ bv241 10)) #x4a\n"
						  "(ite  (= x (_ bv242 10)) #xed\n"
						  "(ite  (= x (_ bv243 10)) #xde\n"
						  "(ite  (= x (_ bv244 10)) #xc5\n"
						  "(ite  (= x (_ bv245 10)) #x31\n"
						  "(ite  (= x (_ bv246 10)) #xfe\n"
						  "(ite  (= x (_ bv247 10)) #x18\n"
						  "(ite  (= x (_ bv248 10)) #x0d\n"
						  "(ite  (= x (_ bv249 10)) #x63\n"
						  "(ite  (= x (_ bv250 10)) #x8c\n"
						  "(ite  (= x (_ bv251 10)) #x80\n"
						  "(ite  (= x (_ bv252 10)) #xc0\n"
						  "(ite  (= x (_ bv253 10)) #xf7\n"
						  "(ite  (= x (_ bv254 10)) #x70\n"
						  "(ite  (= x (_ bv255 10)) #x07\n"
						  "(ite  (= x (_ bv256 10)) #x01\n"
						  "(ite  (= x (_ bv257 10)) #x03\n"
						  "(ite  (= x (_ bv258 10)) #x05\n"
						  "(ite  (= x (_ bv259 10)) #x0f\n"
						  "(ite  (= x (_ bv260 10)) #x11\n"
						  "(ite  (= x (_ bv261 10)) #x33\n"
						  "(ite  (= x (_ bv262 10)) #x55\n"
						  "(ite  (= x (_ bv263 10)) #xff\n"
						  "(ite  (= x (_ bv264 10)) #x1a\n"
						  "(ite  (= x (_ bv265 10)) #x2e\n"
						  "(ite  (= x (_ bv266 10)) #x72\n"
						  "(ite  (= x (_ bv267 10)) #x96\n"
						  "(ite  (= x (_ bv268 10)) #xa1\n"
						  "(ite  (= x (_ bv269 10)) #xf8\n"
						  "(ite  (= x (_ bv270 10)) #x13\n"
						  "(ite  (= x (_ bv271 10)) #x35\n"
						  "(ite  (= x (_ bv272 10)) #x5f\n"
						  "(ite  (= x (_ bv273 10)) #xe1\n"
						  "(ite  (= x (_ bv274 10)) #x38\n"
						  "(ite  (= x (_ bv275 10)) #x48\n"
						  "(ite  (= x (_ bv276 10)) #xd8\n"
						  "(ite  (= x (_ bv277 10)) #x73\n"
						  "(ite  (= x (_ bv278 10)) #x95\n"
						  "(ite  (= x (_ bv279 10)) #xa4\n"
						  "(ite  (= x (_ bv280 10)) #xf7\n"
						  "(ite  (= x (_ bv281 10)) #x02\n"
						  "(ite  (= x (_ bv282 10)) #x06\n"
						  "(ite  (= x (_ bv283 10)) #x0a\n"
						  "(ite  (= x (_ bv284 10)) #x1e\n"
						  "(ite  (= x (_ bv285 10)) #x22\n"
						  "(ite  (= x (_ bv286 10)) #x66\n"
						  "(ite  (= x (_ bv287 10)) #xaa\n"
						  "(ite  (= x (_ bv288 10)) #xe5\n"
						  "(ite  (= x (_ bv289 10)) #x34\n"
						  "(ite  (= x (_ bv290 10)) #x5c\n"
						  "(ite  (= x (_ bv291 10)) #xe4\n"
						  "(ite  (= x (_ bv292 10)) #x37\n"
						  "(ite  (= x (_ bv293 10)) #x59\n"
						  "(ite  (= x (_ bv294 10)) #xeb\n"
						  "(ite  (= x (_ bv295 10)) #x26\n"
						  "(ite  (= x (_ bv296 10)) #x6a\n"
						  "(ite  (= x (_ bv297 10)) #xbe\n"
						  "(ite  (= x (_ bv298 10)) #xd9\n"
						  "(ite  (= x (_ bv299 10)) #x70\n"
						  "(ite  (= x (_ bv300 10)) #x90\n"
						  "(ite  (= x (_ bv301 10)) #xab\n"
						  "(ite  (= x (_ bv302 10)) #xe6\n"
						  "(ite  (= x (_ bv303 10)) #x31\n"
						  "(ite  (= x (_ bv304 10)) #x53\n"
						  "(ite  (= x (_ bv305 10)) #xf5\n"
						  "(ite  (= x (_ bv306 10)) #x04\n"
						  "(ite  (= x (_ bv307 10)) #x0c\n"
						  "(ite  (= x (_ bv308 10)) #x14\n"
						  "(ite  (= x (_ bv309 10)) #x3c\n"
						  "(ite  (= x (_ bv310 10)) #x44\n"
						  "(ite  (= x (_ bv311 10)) #xcc\n"
						  "(ite  (= x (_ bv312 10)) #x4f\n"
						  "(ite  (= x (_ bv313 10)) #xd1\n"
						  "(ite  (= x (_ bv314 10)) #x68\n"
						  "(ite  (= x (_ bv315 10)) #xb8\n"
						  "(ite  (= x (_ bv316 10)) #xd3\n"
						  "(ite  (= x (_ bv317 10)) #x6e\n"
						  "(ite  (= x (_ bv318 10)) #xb2\n"
						  "(ite  (= x (_ bv319 10)) #xcd\n"
						  "(ite  (= x (_ bv320 10)) #x4c\n"
						  "(ite  (= x (_ bv321 10)) #xd4\n"
						  "(ite  (= x (_ bv322 10)) #x67\n"
						  "(ite  (= x (_ bv323 10)) #xa9\n"
						  "(ite  (= x (_ bv324 10)) #xe0\n"
						  "(ite  (= x (_ bv325 10)) #x3b\n"
						  "(ite  (= x (_ bv326 10)) #x4d\n"
						  "(ite  (= x (_ bv327 10)) #xd7\n"
						  "(ite  (= x (_ bv328 10)) #x62\n"
						  "(ite  (= x (_ bv329 10)) #xa6\n"
						  "(ite  (= x (_ bv330 10)) #xf1\n"
						  "(ite  (= x (_ bv331 10)) #x08\n"
						  "(ite  (= x (_ bv332 10)) #x18\n"
						  "(ite  (= x (_ bv333 10)) #x28\n"
						  "(ite  (= x (_ bv334 10)) #x78\n"
						  "(ite  (= x (_ bv335 10)) #x88\n"
						  "(ite  (= x (_ bv336 10)) #x83\n"
						  "(ite  (= x (_ bv337 10)) #x9e\n"
						  "(ite  (= x (_ bv338 10)) #xb9\n"
						  "(ite  (= x (_ bv339 10)) #xd0\n"
						  "(ite  (= x (_ bv340 10)) #x6b\n"
						  "(ite  (= x (_ bv341 10)) #xbd\n"
						  "(ite  (= x (_ bv342 10)) #xdc\n"
						  "(ite  (= x (_ bv343 10)) #x7f\n"
						  "(ite  (= x (_ bv344 10)) #x81\n"
						  "(ite  (= x (_ bv345 10)) #x98\n"
						  "(ite  (= x (_ bv346 10)) #xb3\n"
						  "(ite  (= x (_ bv347 10)) #xce\n"
						  "(ite  (= x (_ bv348 10)) #x49\n"
						  "(ite  (= x (_ bv349 10)) #xdb\n"
						  "(ite  (= x (_ bv350 10)) #x76\n"
						  "(ite  (= x (_ bv351 10)) #x9a\n"
						  "(ite  (= x (_ bv352 10)) #xb5\n"
						  "(ite  (= x (_ bv353 10)) #xc4\n"
						  "(ite  (= x (_ bv354 10)) #x57\n"
						  "(ite  (= x (_ bv355 10)) #xf9\n"
						  "(ite  (= x (_ bv356 10)) #x10\n"
						  "(ite  (= x (_ bv357 10)) #x30\n"
						  "(ite  (= x (_ bv358 10)) #x50\n"
						  "(ite  (= x (_ bv359 10)) #xf0\n"
						  "(ite  (= x (_ bv360 10)) #x0b\n"
						  "(ite  (= x (_ bv361 10)) #x1d\n"
						  "(ite  (= x (_ bv362 10)) #x27\n"
						  "(ite  (= x (_ bv363 10)) #x69\n"
						  "(ite  (= x (_ bv364 10)) #xbb\n"
						  "(ite  (= x (_ bv365 10)) #xd6\n"
						  "(ite  (= x (_ bv366 10)) #x61\n"
						  "(ite  (= x (_ bv367 10)) #xa3\n"
						  "(ite  (= x (_ bv368 10)) #xfe\n"
						  "(ite  (= x (_ bv369 10)) #x19\n"
						  "(ite  (= x (_ bv370 10)) #x2b\n"
						  "(ite  (= x (_ bv371 10)) #x7d\n"
						  "(ite  (= x (_ bv372 10)) #x87\n"
						  "(ite  (= x (_ bv373 10)) #x92\n"
						  "(ite  (= x (_ bv374 10)) #xad\n"
						  "(ite  (= x (_ bv375 10)) #xec\n"
						  "(ite  (= x (_ bv376 10)) #x2f\n"
						  "(ite  (= x (_ bv377 10)) #x71\n"
						  "(ite  (= x (_ bv378 10)) #x93\n"
						  "(ite  (= x (_ bv379 10)) #xae\n"
						  "(ite  (= x (_ bv380 10)) #xe9\n"
						  "(ite  (= x (_ bv381 10)) #x20\n"
						  "(ite  (= x (_ bv382 10)) #x60\n"
						  "(ite  (= x (_ bv383 10)) #xa0\n"
						  "(ite  (= x (_ bv384 10)) #xfb\n"
						  "(ite  (= x (_ bv385 10)) #x16\n"
						  "(ite  (= x (_ bv386 10)) #x3a\n"
						  "(ite  (= x (_ bv387 10)) #x4e\n"
						  "(ite  (= x (_ bv388 10)) #xd2\n"
						  "(ite  (= x (_ bv389 10)) #x6d\n"
						  "(ite  (= x (_ bv390 10)) #xb7\n"
						  "(ite  (= x (_ bv391 10)) #xc2\n"
						  "(ite  (= x (_ bv392 10)) #x5d\n"
						  "(ite  (= x (_ bv393 10)) #xe7\n"
						  "(ite  (= x (_ bv394 10)) #x32\n"
						  "(ite  (= x (_ bv395 10)) #x56\n"
						  "(ite  (= x (_ bv396 10)) #xfa\n"
						  "(ite  (= x (_ bv397 10)) #x15\n"
						  "(ite  (= x (_ bv398 10)) #x3f\n"
						  "(ite  (= x (_ bv399 10)) #x41\n"
						  "(ite  (= x (_ bv400 10)) #xc3\n"
						  "(ite  (= x (_ bv401 10)) #x5e\n"
						  "(ite  (= x (_ bv402 10)) #xe2\n"
						  "(ite  (= x (_ bv403 10)) #x3d\n"
						  "(ite  (= x (_ bv404 10)) #x47\n"
						  "(ite  (= x (_ bv405 10)) #xc9\n"
						  "(ite  (= x (_ bv406 10)) #x40\n"
						  "(ite  (= x (_ bv407 10)) #xc0\n"
						  "(ite  (= x (_ bv408 10)) #x5b\n"
						  "(ite  (= x (_ bv409 10)) #xed\n"
						  "(ite  (= x (_ bv410 10)) #x2c\n"
						  "(ite  (= x (_ bv411 10)) #x74\n"
						  "(ite  (= x (_ bv412 10)) #x9c\n"
						  "(ite  (= x (_ bv413 10)) #xbf\n"
						  "(ite  (= x (_ bv414 10)) #xda\n"
						  "(ite  (= x (_ bv415 10)) #x75\n"
						  "(ite  (= x (_ bv416 10)) #x9f\n"
						  "(ite  (= x (_ bv417 10)) #xba\n"
						  "(ite  (= x (_ bv418 10)) #xd5\n"
						  "(ite  (= x (_ bv419 10)) #x64\n"
						  "(ite  (= x (_ bv420 10)) #xac\n"
						  "(ite  (= x (_ bv421 10)) #xef\n"
						  "(ite  (= x (_ bv422 10)) #x2a\n"
						  "(ite  (= x (_ bv423 10)) #x7e\n"
						  "(ite  (= x (_ bv424 10)) #x82\n"
						  "(ite  (= x (_ bv425 10)) #x9d\n"
						  "(ite  (= x (_ bv426 10)) #xbc\n"
						  "(ite  (= x (_ bv427 10)) #xdf\n"
						  "(ite  (= x (_ bv428 10)) #x7a\n"
						  "(ite  (= x (_ bv429 10)) #x8e\n"
						  "(ite  (= x (_ bv430 10)) #x89\n"
						  "(ite  (= x (_ bv431 10)) #x80\n"
						  "(ite  (= x (_ bv432 10)) #x9b\n"
						  "(ite  (= x (_ bv433 10)) #xb6\n"
						  "(ite  (= x (_ bv434 10)) #xc1\n"
						  "(ite  (= x (_ bv435 10)) #x58\n"
						  "(ite  (= x (_ bv436 10)) #xe8\n"
						  "(ite  (= x (_ bv437 10)) #x23\n"
						  "(ite  (= x (_ bv438 10)) #x65\n"
						  "(ite  (= x (_ bv439 10)) #xaf\n"
						  "(ite  (= x (_ bv440 10)) #xea\n"
						  "(ite  (= x (_ bv441 10)) #x25\n"
						  "(ite  (= x (_ bv442 10)) #x6f\n"
						  "(ite  (= x (_ bv443 10)) #xb1\n"
						  "(ite  (= x (_ bv444 10)) #xc8\n"
						  "(ite  (= x (_ bv445 10)) #x43\n"
						  "(ite  (= x (_ bv446 10)) #xc5\n"
						  "(ite  (= x (_ bv447 10)) #x54\n"
						  "(ite  (= x (_ bv448 10)) #xfc\n"
						  "(ite  (= x (_ bv449 10)) #x1f\n"
						  "(ite  (= x (_ bv450 10)) #x21\n"
						  "(ite  (= x (_ bv451 10)) #x63\n"
						  "(ite  (= x (_ bv452 10)) #xa5\n"
						  "(ite  (= x (_ bv453 10)) #xf4\n"
						  "(ite  (= x (_ bv454 10)) #x07\n"
						  "(ite  (= x (_ bv455 10)) #x09\n"
						  "(ite  (= x (_ bv456 10)) #x1b\n"
						  "(ite  (= x (_ bv457 10)) #x2d\n"
						  "(ite  (= x (_ bv458 10)) #x77\n"
						  "(ite  (= x (_ bv459 10)) #x99\n"
						  "(ite  (= x (_ bv460 10)) #xb0\n"
						  "(ite  (= x (_ bv461 10)) #xcb\n"
						  "(ite  (= x (_ bv462 10)) #x46\n"
						  "(ite  (= x (_ bv463 10)) #xca\n"
						  "(ite  (= x (_ bv464 10)) #x45\n"
						  "(ite  (= x (_ bv465 10)) #xcf\n"
						  "(ite  (= x (_ bv466 10)) #x4a\n"
						  "(ite  (= x (_ bv467 10)) #xde\n"
						  "(ite  (= x (_ bv468 10)) #x79\n"
						  "(ite  (= x (_ bv469 10)) #x8b\n"
						  "(ite  (= x (_ bv470 10)) #x86\n"
						  "(ite  (= x (_ bv471 10)) #x91\n"
						  "(ite  (= x (_ bv472 10)) #xa8\n"
						  "(ite  (= x (_ bv473 10)) #xe3\n"
						  "(ite  (= x (_ bv474 10)) #x3e\n"
						  "(ite  (= x (_ bv475 10)) #x42\n"
						  "(ite  (= x (_ bv476 10)) #xc6\n"
						  "(ite  (= x (_ bv477 10)) #x51\n"
						  "(ite  (= x (_ bv478 10)) #xf3\n"
						  "(ite  (= x (_ bv479 10)) #x0e\n"
						  "(ite  (= x (_ bv480 10)) #x12\n"
						  "(ite  (= x (_ bv481 10)) #x36\n"
						  "(ite  (= x (_ bv482 10)) #x5a\n"
						  "(ite  (= x (_ bv483 10)) #xee\n"
						  "(ite  (= x (_ bv484 10)) #x29\n"
						  "(ite  (= x (_ bv485 10)) #x7b\n"
						  "(ite  (= x (_ bv486 10)) #x8d\n"
						  "(ite  (= x (_ bv487 10)) #x8c\n"
						  "(ite  (= x (_ bv488 10)) #x8f\n"
						  "(ite  (= x (_ bv489 10)) #x8a\n"
						  "(ite  (= x (_ bv490 10)) #x85\n"
						  "(ite  (= x (_ bv491 10)) #x94\n"
						  "(ite  (= x (_ bv492 10)) #xa7\n"
						  "(ite  (= x (_ bv493 10)) #xf2\n"
						  "(ite  (= x (_ bv494 10)) #x0d\n"
						  "(ite  (= x (_ bv495 10)) #x17\n"
						  "(ite  (= x (_ bv496 10)) #x39\n"
						  "(ite  (= x (_ bv497 10)) #x4b\n"
						  "(ite  (= x (_ bv498 10)) #xdd\n"
						  "(ite  (= x (_ bv499 10)) #x7c\n"
						  "(ite  (= x (_ bv500 10)) #x84\n"
						  "(ite  (= x (_ bv501 10)) #x97\n"
						  "(ite  (= x (_ bv502 10)) #xa2\n"
						  "(ite  (= x (_ bv503 10)) #xfd\n"
						  "(ite  (= x (_ bv504 10)) #x1c\n"
						  "(ite  (= x (_ bv505 10)) #x24\n"
						  "(ite  (= x (_ bv506 10)) #x6c\n"
						  "(ite  (= x (_ bv507 10)) #xb4\n"
						  "(ite  (= x (_ bv508 10)) #xc7\n"
						  "(ite  (= x (_ bv509 10)) #x52\n"
						  "(ite  (= x (_ bv510 10)) #xf6\n"
						  "(ite  (= x (_ bv511 10)) #x01\n"
						  "(ite  (= x (_ bv512 10)) #x03\n"
						  "(ite  (= x (_ bv513 10)) #x05\n"
						  "(ite  (= x (_ bv514 10)) #x0f\n"
						  "(ite  (= x (_ bv515 10)) #x11\n"
						  "(ite  (= x (_ bv516 10)) #x33\n"
						  "(ite  (= x (_ bv517 10)) #x55\n"
						  "(ite  (= x (_ bv518 10)) #xff\n"
						  "(ite  (= x (_ bv519 10)) #x1a\n"
						  "(ite  (= x (_ bv520 10)) #x2e\n"
						  "(ite  (= x (_ bv521 10)) #x72\n"
						  "(ite  (= x (_ bv522 10)) #x96\n"
						  "(ite  (= x (_ bv523 10)) #xa1\n"
						  "(ite  (= x (_ bv524 10)) #xf8\n"
						  "(ite  (= x (_ bv525 10)) #x13\n"
						  "(ite  (= x (_ bv526 10)) #x35\n"
						  "(ite  (= x (_ bv527 10)) #x5f\n"
						  "(ite  (= x (_ bv528 10)) #xe1\n"
						  "(ite  (= x (_ bv529 10)) #x38\n"
						  "(ite  (= x (_ bv530 10)) #x48\n"
						  "(ite  (= x (_ bv531 10)) #xd8\n"
						  "(ite  (= x (_ bv532 10)) #x73\n"
						  "(ite  (= x (_ bv533 10)) #x95\n"
						  "(ite  (= x (_ bv534 10)) #xa4\n"
						  "(ite  (= x (_ bv535 10)) #xf7\n"
						  "(ite  (= x (_ bv536 10)) #x02\n"
						  "(ite  (= x (_ bv537 10)) #x06\n"
						  "(ite  (= x (_ bv538 10)) #x0a\n"
						  "(ite  (= x (_ bv539 10)) #x1e\n"
						  "(ite  (= x (_ bv540 10)) #x22\n"
						  "(ite  (= x (_ bv541 10)) #x66\n"
						  "(ite  (= x (_ bv542 10)) #xaa\n"
						  "(ite  (= x (_ bv543 10)) #xe5\n"
						  "(ite  (= x (_ bv544 10)) #x34\n"
						  "(ite  (= x (_ bv545 10)) #x5c\n"
						  "(ite  (= x (_ bv546 10)) #xe4\n"
						  "(ite  (= x (_ bv547 10)) #x37\n"
						  "(ite  (= x (_ bv548 10)) #x59\n"
						  "(ite  (= x (_ bv549 10)) #xeb\n"
						  "(ite  (= x (_ bv550 10)) #x26\n"
						  "(ite  (= x (_ bv551 10)) #x6a\n"
						  "(ite  (= x (_ bv552 10)) #xbe\n"
						  "(ite  (= x (_ bv553 10)) #xd9\n"
						  "(ite  (= x (_ bv554 10)) #x70\n"
						  "(ite  (= x (_ bv555 10)) #x90\n"
						  "(ite  (= x (_ bv556 10)) #xab\n"
						  "(ite  (= x (_ bv557 10)) #xe6\n"
						  "(ite  (= x (_ bv558 10)) #x31\n"
						  "(ite  (= x (_ bv559 10)) #x53\n"
						  "(ite  (= x (_ bv560 10)) #xf5\n"
						  "(ite  (= x (_ bv561 10)) #x04\n"
						  "(ite  (= x (_ bv562 10)) #x0c\n"
						  "(ite  (= x (_ bv563 10)) #x14\n"
						  "(ite  (= x (_ bv564 10)) #x3c\n"
						  "(ite  (= x (_ bv565 10)) #x44\n"
						  "(ite  (= x (_ bv566 10)) #xcc\n"
						  "(ite  (= x (_ bv567 10)) #x4f\n"
						  "(ite  (= x (_ bv568 10)) #xd1\n"
						  "(ite  (= x (_ bv569 10)) #x68\n"
						  "(ite  (= x (_ bv570 10)) #xb8\n"
						  "(ite  (= x (_ bv571 10)) #xd3\n"
						  "(ite  (= x (_ bv572 10)) #x6e\n"
						  "(ite  (= x (_ bv573 10)) #xb2\n"
						  "(ite  (= x (_ bv574 10)) #xcd\n"
						  "(ite  (= x (_ bv575 10)) #x4c\n"
						  "(ite  (= x (_ bv576 10)) #xd4\n"
						  "(ite  (= x (_ bv577 10)) #x67\n"
						  "(ite  (= x (_ bv578 10)) #xa9\n"
						  "(ite  (= x (_ bv579 10)) #xe0\n"
						  "(ite  (= x (_ bv580 10)) #x3b\n"
						  "(ite  (= x (_ bv581 10)) #x4d\n"
						  "(ite  (= x (_ bv582 10)) #xd7\n"
						  "(ite  (= x (_ bv583 10)) #x62\n"
						  "(ite  (= x (_ bv584 10)) #xa6\n"
						  "(ite  (= x (_ bv585 10)) #xf1\n"
						  "(ite  (= x (_ bv586 10)) #x08\n"
						  "(ite  (= x (_ bv587 10)) #x18\n"
						  "(ite  (= x (_ bv588 10)) #x28\n"
						  "(ite  (= x (_ bv589 10)) #x78\n"
						  "(ite  (= x (_ bv590 10)) #x88\n"
						  "(ite  (= x (_ bv591 10)) #x83\n"
						  "(ite  (= x (_ bv592 10)) #x9e\n"
						  "(ite  (= x (_ bv593 10)) #xb9\n"
						  "(ite  (= x (_ bv594 10)) #xd0\n"
						  "(ite  (= x (_ bv595 10)) #x6b\n"
						  "(ite  (= x (_ bv596 10)) #xbd\n"
						  "(ite  (= x (_ bv597 10)) #xdc\n"
						  "(ite  (= x (_ bv598 10)) #x7f\n"
						  "(ite  (= x (_ bv599 10)) #x81\n"
						  "(ite  (= x (_ bv600 10)) #x98\n"
						  "(ite  (= x (_ bv601 10)) #xb3\n"
						  "(ite  (= x (_ bv602 10)) #xce\n"
						  "(ite  (= x (_ bv603 10)) #x49\n"
						  "(ite  (= x (_ bv604 10)) #xdb\n"
						  "(ite  (= x (_ bv605 10)) #x76\n"
						  "(ite  (= x (_ bv606 10)) #x9a\n"
						  "(ite  (= x (_ bv607 10)) #xb5\n"
						  "(ite  (= x (_ bv608 10)) #xc4\n"
						  "(ite  (= x (_ bv609 10)) #x57\n"
						  "(ite  (= x (_ bv610 10)) #xf9\n"
						  "(ite  (= x (_ bv611 10)) #x10\n"
						  "(ite  (= x (_ bv612 10)) #x30\n"
						  "(ite  (= x (_ bv613 10)) #x50\n"
						  "(ite  (= x (_ bv614 10)) #xf0\n"
						  "(ite  (= x (_ bv615 10)) #x0b\n"
						  "(ite  (= x (_ bv616 10)) #x1d\n"
						  "(ite  (= x (_ bv617 10)) #x27\n"
						  "(ite  (= x (_ bv618 10)) #x69\n"
						  "(ite  (= x (_ bv619 10)) #xbb\n"
						  "(ite  (= x (_ bv620 10)) #xd6\n"
						  "(ite  (= x (_ bv621 10)) #x61\n"
						  "(ite  (= x (_ bv622 10)) #xa3\n"
						  "(ite  (= x (_ bv623 10)) #xfe\n"
						  "(ite  (= x (_ bv624 10)) #x19\n"
						  "(ite  (= x (_ bv625 10)) #x2b\n"
						  "(ite  (= x (_ bv626 10)) #x7d\n"
						  "(ite  (= x (_ bv627 10)) #x87\n"
						  "(ite  (= x (_ bv628 10)) #x92\n"
						  "(ite  (= x (_ bv629 10)) #xad\n"
						  "(ite  (= x (_ bv630 10)) #xec\n"
						  "(ite  (= x (_ bv631 10)) #x2f\n"
						  "(ite  (= x (_ bv632 10)) #x71\n"
						  "(ite  (= x (_ bv633 10)) #x93\n"
						  "(ite  (= x (_ bv634 10)) #xae\n"
						  "(ite  (= x (_ bv635 10)) #xe9\n"
						  "(ite  (= x (_ bv636 10)) #x20\n"
						  "(ite  (= x (_ bv637 10)) #x60\n"
						  "(ite  (= x (_ bv638 10)) #xa0\n"
						  "(ite  (= x (_ bv639 10)) #xfb\n"
						  "(ite  (= x (_ bv640 10)) #x16\n"
						  "(ite  (= x (_ bv641 10)) #x3a\n"
						  "(ite  (= x (_ bv642 10)) #x4e\n"
						  "(ite  (= x (_ bv643 10)) #xd2\n"
						  "(ite  (= x (_ bv644 10)) #x6d\n"
						  "(ite  (= x (_ bv645 10)) #xb7\n"
						  "(ite  (= x (_ bv646 10)) #xc2\n"
						  "(ite  (= x (_ bv647 10)) #x5d\n"
						  "(ite  (= x (_ bv648 10)) #xe7\n"
						  "(ite  (= x (_ bv649 10)) #x32\n"
						  "(ite  (= x (_ bv650 10)) #x56\n"
						  "(ite  (= x (_ bv651 10)) #xfa\n"
						  "(ite  (= x (_ bv652 10)) #x15\n"
						  "(ite  (= x (_ bv653 10)) #x3f\n"
						  "(ite  (= x (_ bv654 10)) #x41\n"
						  "(ite  (= x (_ bv655 10)) #xc3\n"
						  "(ite  (= x (_ bv656 10)) #x5e\n"
						  "(ite  (= x (_ bv657 10)) #xe2\n"
						  "(ite  (= x (_ bv658 10)) #x3d\n"
						  "(ite  (= x (_ bv659 10)) #x47\n"
						  "(ite  (= x (_ bv660 10)) #xc9\n"
						  "(ite  (= x (_ bv661 10)) #x40\n"
						  "(ite  (= x (_ bv662 10)) #xc0\n"
						  "(ite  (= x (_ bv663 10)) #x5b\n"
						  "(ite  (= x (_ bv664 10)) #xed\n"
						  "(ite  (= x (_ bv665 10)) #x2c\n"
						  "(ite  (= x (_ bv666 10)) #x74\n"
						  "(ite  (= x (_ bv667 10)) #x9c\n"
						  "(ite  (= x (_ bv668 10)) #xbf\n"
						  "(ite  (= x (_ bv669 10)) #xda\n"
						  "(ite  (= x (_ bv670 10)) #x75\n"
						  "(ite  (= x (_ bv671 10)) #x9f\n"
						  "(ite  (= x (_ bv672 10)) #xba\n"
						  "(ite  (= x (_ bv673 10)) #xd5\n"
						  "(ite  (= x (_ bv674 10)) #x64\n"
						  "(ite  (= x (_ bv675 10)) #xac\n"
						  "(ite  (= x (_ bv676 10)) #xef\n"
						  "(ite  (= x (_ bv677 10)) #x2a\n"
						  "(ite  (= x (_ bv678 10)) #x7e\n"
						  "(ite  (= x (_ bv679 10)) #x82\n"
						  "(ite  (= x (_ bv680 10)) #x9d\n"
						  "(ite  (= x (_ bv681 10)) #xbc\n"
						  "(ite  (= x (_ bv682 10)) #xdf\n"
						  "(ite  (= x (_ bv683 10)) #x7a\n"
						  "(ite  (= x (_ bv684 10)) #x8e\n"
						  "(ite  (= x (_ bv685 10)) #x89\n"
						  "(ite  (= x (_ bv686 10)) #x80\n"
						  "(ite  (= x (_ bv687 10)) #x9b\n"
						  "(ite  (= x (_ bv688 10)) #xb6\n"
						  "(ite  (= x (_ bv689 10)) #xc1\n"
						  "(ite  (= x (_ bv690 10)) #x58\n"
						  "(ite  (= x (_ bv691 10)) #xe8\n"
						  "(ite  (= x (_ bv692 10)) #x23\n"
						  "(ite  (= x (_ bv693 10)) #x65\n"
						  "(ite  (= x (_ bv694 10)) #xaf\n"
						  "(ite  (= x (_ bv695 10)) #xea\n"
						  "(ite  (= x (_ bv696 10)) #x25\n"
						  "(ite  (= x (_ bv697 10)) #x6f\n"
						  "(ite  (= x (_ bv698 10)) #xb1\n"
						  "(ite  (= x (_ bv699 10)) #xc8\n"
						  "(ite  (= x (_ bv700 10)) #x43\n"
						  "(ite  (= x (_ bv701 10)) #xc5\n"
						  "(ite  (= x (_ bv702 10)) #x54\n"
						  "(ite  (= x (_ bv703 10)) #xfc\n"
						  "(ite  (= x (_ bv704 10)) #x1f\n"
						  "(ite  (= x (_ bv705 10)) #x21\n"
						  "(ite  (= x (_ bv706 10)) #x63\n"
						  "(ite  (= x (_ bv707 10)) #xa5\n"
						  "(ite  (= x (_ bv708 10)) #xf4\n"
						  "(ite  (= x (_ bv709 10)) #x07\n"
						  "(ite  (= x (_ bv710 10)) #x09\n"
						  "(ite  (= x (_ bv711 10)) #x1b\n"
						  "(ite  (= x (_ bv712 10)) #x2d\n"
						  "(ite  (= x (_ bv713 10)) #x77\n"
						  "(ite  (= x (_ bv714 10)) #x99\n"
						  "(ite  (= x (_ bv715 10)) #xb0\n"
						  "(ite  (= x (_ bv716 10)) #xcb\n"
						  "(ite  (= x (_ bv717 10)) #x46\n"
						  "(ite  (= x (_ bv718 10)) #xca\n"
						  "(ite  (= x (_ bv719 10)) #x45\n"
						  "(ite  (= x (_ bv720 10)) #xcf\n"
						  "(ite  (= x (_ bv721 10)) #x4a\n"
						  "(ite  (= x (_ bv722 10)) #xde\n"
						  "(ite  (= x (_ bv723 10)) #x79\n"
						  "(ite  (= x (_ bv724 10)) #x8b\n"
						  "(ite  (= x (_ bv725 10)) #x86\n"
						  "(ite  (= x (_ bv726 10)) #x91\n"
						  "(ite  (= x (_ bv727 10)) #xa8\n"
						  "(ite  (= x (_ bv728 10)) #xe3\n"
						  "(ite  (= x (_ bv729 10)) #x3e\n"
						  "(ite  (= x (_ bv730 10)) #x42\n"
						  "(ite  (= x (_ bv731 10)) #xc6\n"
						  "(ite  (= x (_ bv732 10)) #x51\n"
						  "(ite  (= x (_ bv733 10)) #xf3\n"
						  "(ite  (= x (_ bv734 10)) #x0e\n"
						  "(ite  (= x (_ bv735 10)) #x12\n"
						  "(ite  (= x (_ bv736 10)) #x36\n"
						  "(ite  (= x (_ bv737 10)) #x5a\n"
						  "(ite  (= x (_ bv738 10)) #xee\n"
						  "(ite  (= x (_ bv739 10)) #x29\n"
						  "(ite  (= x (_ bv740 10)) #x7b\n"
						  "(ite  (= x (_ bv741 10)) #x8d\n"
						  "(ite  (= x (_ bv742 10)) #x8c\n"
						  "(ite  (= x (_ bv743 10)) #x8f\n"
						  "(ite  (= x (_ bv744 10)) #x8a\n"
						  "(ite  (= x (_ bv745 10)) #x85\n"
						  "(ite  (= x (_ bv746 10)) #x94\n"
						  "(ite  (= x (_ bv747 10)) #xa7\n"
						  "(ite  (= x (_ bv748 10)) #xf2\n"
						  "(ite  (= x (_ bv749 10)) #x0d\n"
						  "(ite  (= x (_ bv750 10)) #x17\n"
						  "(ite  (= x (_ bv751 10)) #x39\n"
						  "(ite  (= x (_ bv752 10)) #x4b\n"
						  "(ite  (= x (_ bv753 10)) #xdd\n"
						  "(ite  (= x (_ bv754 10)) #x7c\n"
						  "(ite  (= x (_ bv755 10)) #x84\n"
						  "(ite  (= x (_ bv756 10)) #x97\n"
						  "(ite  (= x (_ bv757 10)) #xa2\n"
						  "(ite  (= x (_ bv758 10)) #xfd\n"
						  "(ite  (= x (_ bv759 10)) #x1c\n"
						  "(ite  (= x (_ bv760 10)) #x24\n"
						  "(ite  (= x (_ bv761 10)) #x6c\n"
						  "(ite  (= x (_ bv762 10)) #xb4\n"
						  "(ite  (= x (_ bv763 10)) #xc7\n"
						  "(ite  (= x (_ bv764 10)) #x52\n"
						  "(ite  (= x (_ bv765 10)) #xf6\n"
						  "(ite  (= x (_ bv766 10)) #x01\n"
						  " #x00 )))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))\n"
						  ")\n"
						  "(define-fun bvgfmul ((x (_ BitVec 8)) (y (_ BitVec 8)) )(_ BitVec 8)\n"
						  "  (ite (or (= x #x00) (= y #x00)) #x00 \n"
						  "    (table (bvadd (_ bv256 10) ( bvadd   ((_ zero_extend 2)(table ((_ zero_extend 2)x) )) ((_ zero_extend 2) (table  ((_ zero_extend 2)y) ))  ) ))\n"
						  "  )\n"
						  ")\n";
	}


	static string powdefine() {
	    return "(define-fun pow2 ((x (_ BitVec 8))) (_ BitVec 8)\n"
               "\t(bvgfmul x x)\n"
               ")\n"
               "\n"
               "\n"
               "(define-fun pow4 ((x (_ BitVec 8))) (_ BitVec 8)\n"
               "\t(bvgfmul (pow2 x) (pow2 x))\n"
               ")\n"
               "\n"
               "\n"
               "(define-fun pow8 ((x (_ BitVec 8))) (_ BitVec 8)\n"
               "\t(bvgfmul (pow4 x) (pow4 x))\n"
               ")\n"
               "\n"
               "(define-fun pow16 ((x (_ BitVec 8))) (_ BitVec 8)\n"
               "\t(bvgfmul (pow8 x) (pow8 x))\n"
               ")\n";
	}

	static string fundefine() {

		return "(define-fun fun ( (x (_ BitVec 8)) (c (_ BitVec 8))) (_ BitVec 32)\n"
			   "\t(ite (= x c) (_ bv1 32) (_ bv0 32) )\n"
			   ")";
	}
    static DistributionEnums getDistributionByZ3(Node node,set<string> notCareRandSet) {

        set<string> rand;
        set<string> tempRand;
        set<string> secret;
        set<string> plain;
        vector<string> randVector;
        vector<string> secretVector;
        vector<string> plainVector;
        vector<string> notCareRandVector;
        vector<string> num1;
        vector<string> num2;
        map<string, string> randMap;
        map<string, string> secretMap1;
        map<string, string> secretMap2;
        map<string, string> plainMap;


        Node::getRandSet(node, tempRand);
//		cout << tempRand.size() << endl;
//		cout << notCareRandSet.size() << endl;
        SetUtil::Difference(tempRand,notCareRandSet,rand);
        Node::getSecretSet(node, secret);
        Node::getPlainSet(node, plain);


        SetUtil::SetToVector(rand, randVector);
        SetUtil::SetToVector(secret, secretVector);
        SetUtil::SetToVector(plain, plainVector);
        SetUtil::SetToVector(notCareRandSet,notCareRandVector);



        int randCount = rand.size();
        int secretCount = secret.size();
        int plainCount = plain.size();
        int num = 1;//record for all rand power set size
        for (int i = 0; i < randCount; i++) {
            num = num * 2;
        }

//		if(randCount==0){
//			num=0;
//		}


        string finalString="";
        z3::context contextUse;
//		z3::solver solverUse(contextUse);
        z3::solver solverUse = z3::tactic(contextUse, "smt").mk_solver();


        //declare variable for smt2
        for (int i = 0; i < num; i++) {
            string s = "";
            s.append("(declare-fun r");
            s.append(StringUtil::getString(i) + "() Bool )");

            finalString.append(s+"\n");
        }

        for (int i = 0; i < num; i++) {
            string s = "";
            s.append("(declare-fun rr");
            s.append(StringUtil::getString(i) + "() Bool )");

            finalString.append(s+"\n");
        }
        for (int i = 0; i < num; i++) {
            string s = "";
            s.append("(declare-fun n");
            s.append(StringUtil::getString(i) + "() Int )");
            num1.push_back("n" + StringUtil::getString(i));

            finalString.append(s+"\n");
        }
        for (int i = 0; i < num; i++) {
            string s = "";
            s.append("(declare-fun nn");
            s.append(StringUtil::getString(i) + "() Int )");
            num2.push_back("nn" + StringUtil::getString(i));

            finalString.append(s+"\n");
        }
        for (int i = 0; i < secretCount; i++) {
            string s = "";
            s.append("(declare-fun k");
            s.append(StringUtil::getString(i) + "() Bool )");
            secretMap1[secretVector[i]] = " k" + StringUtil::getString(i) + " ";

            finalString.append(s+"\n");
        }
        for (int i = 0; i < secretCount; i++) {
            string s = "";
            s.append("(declare-fun kk");
            s.append(StringUtil::getString(i) + "() Bool )");
            secretMap2[secretVector[i]] = " kk" + StringUtil::getString(i) + " ";

            finalString.append(s+"\n");
        }
        for (int i = 0; i < plainCount; i++) {
            string s = "";
            s.append("(declare-fun ");
            s.append(plainVector[i] + "() Bool )");

            finalString.append(s+"\n");
        }


        //assert for smt2
        //(assert(= n8_1(xor (and f(xor k2 f)) (and f(xor k1 f)))))
        for (int i = 0; i < num; i++) {
            string s = "";
            s.append("(assert(= r" + StringUtil::getString(i)+" ");
            string ss = parseNodeBool(node);
            StringUtil::replaceThroughMap(ss, secretVector, secretMap1);
            vector<int> temp;
            StringUtil::IntToBinaryVector(i, temp, randCount);
            StringUtil::BitToBoolVector(randVector, temp, randMap);
            StringUtil::replaceThroughMap(ss, randVector, randMap);

            for(unsigned int m=0;m<notCareRandSet.size();m++){
                StringUtil::replace_all(ss, notCareRandVector[m], " false ");
            }

            s.append(ss + " ))");
            StringUtil::replace_all(s, " 0 ", " false ");
            StringUtil::replace_all(s, " 1 ", " true ");


            finalString.append(s+"\n");
        }

        //(assert (= n8_11 (xor (and f  (xor k22 f)) (and f  (xor k11 f))  )))
        for (int i = 0; i < num; i++) {
            string s = "";
            s.append("(assert(= rr" + StringUtil::getString(i)+" ");
            string ss = parseNodeBool(node);
            StringUtil::replaceThroughMap(ss, secretVector, secretMap2);
            vector<int> temp;
            StringUtil::IntToBinaryVector(i, temp, randCount);
            StringUtil::BitToBoolVector(randVector, temp, randMap);
            StringUtil::replaceThroughMap(ss, randVector, randMap);
            for(unsigned int m=0;m<notCareRandSet.size();m++){
                StringUtil::replace_all(ss, notCareRandVector[m], " false ");
            }
            s.append(ss + "))");
            StringUtil::replace_all(s, " 0 ", " false ");
            StringUtil::replace_all(s, " 1 ", " true ");

            finalString.append(s+"\n");
        }

        //(assert (or  (and (= num1 1) n8_1)   (and (= num1 0)(not n8_1))  ))
        for (int i = 0; i < num; i++) {
            string s = "";
            s.append("(assert (or  (and (= n" + StringUtil::getString(i) + " 1)");
            s.append(" r" + StringUtil::getString(i) + ")   (and (= n" + StringUtil::getString(i) + " 0)");
            s.append("(not r" + StringUtil::getString(i) + "))  ))");

            finalString.append(s+"\n");
        }

        //(assert(or (and (= num11 1) n8_11)   (and (= num11 0)(not n8_11))))
        for (int i = 0; i < num; i++) {
            string s = "";
            s.append("(assert (or  (and (= nn" + StringUtil::getString(i) + " 1)");
            s.append(" rr" + StringUtil::getString(i) + ")   (and (= nn" + StringUtil::getString(i) + " 0)");
            s.append("(not rr" + StringUtil::getString(i) + "))  ))");

            finalString.append(s+"\n");
        }

        string s = "(assert(not (= ";
        string s1, s2;
        StringUtil::VectorLogicAdd(num1, s1);
        StringUtil::VectorLogicAdd(num2, s2);
//		if(s1 == "" && s2 == "")
//			return DistributionEnums::ISD;
        s.append(s1 + " ");
        s.append(s2);
        s.append(")))");

        finalString.append(s+"\n");

        solverUse.from_string(finalString.data());
        // export for yao
//		string cnfname = std::to_string(exportInt) + "";
//		exportInt++;
//		cnfname.append(".smt2");
//		streambuf *cout_buf = cout.rdbuf();
//		ofstream of("./smt/" + cnfname);
//		streambuf *file_buf = of.rdbuf();
//		cout.rdbuf(file_buf);
//
//		cout << solverUse << std::endl;
//		of.flush();
//		of.close();
//		cout.rdbuf(cout_buf);

        DistributionEnums dis;
        switch (solverUse.check()) {
            case z3::unsat:  // std::cout << "unsat\n";
                dis=DistributionEnums ::ISD; break;
            case z3::sat:    // std::cout << "sat\n";
                dis=DistributionEnums ::NPM;
//				std::cout << solverUse.get_model() << "\n";
                break;
            case z3::unknown: //std::cout << "unknown\n";
                dis=DistributionEnums ::UKD; break;
        }

        solverUse.reset();

        return dis;


    }
};

