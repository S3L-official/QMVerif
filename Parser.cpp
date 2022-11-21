//
// Created by Pengfei Gao on 2018/8/17.
//
#include <vector>
#include <fstream>
#include "Parser.h"
#include "util/FileUtil.h"
#include "util/NodeUtil.h"
#include "Global.h"
map<string, Node*>& Parser::getNodeMap() {
    return nodeMap;
}

vector<string> Parser::getInterV() {
    return InterV;
}
vector<string> Parser::getRandV(){
    return RandV;
}
vector<string> Parser::getSecV(){
    return SecV;
}

void Parser::parseForECFile() {
    char* benchmarkPath = (char *)absoluteBenchmarkPath.data();
    vector<string> PublicV;
    vector<string> CodeV;

    vector<string> MainV;


    map<string, string> nameMap;


    char* str = readBenchmark(benchmarkPath);
    vector<string> lines = getLines(str);
    PublicV=getPlainVector(lines);


    vector<string> beforeModule=getDefineFunction(lines);
    vector<vector<string>> before_part_tokens = getTokenize(beforeModule);
    vector<vector<string>> main_part_tokens = getTokenize(getMain(lines));


    RandV=getRandVector(main_part_tokens);
    CodeV=getInterCodeVector(main_part_tokens);
    ConstantV=getConstantVector(before_part_tokens);

    MainV=getMainInputVector(main_part_tokens);
    set<string> temp1;
    set<string> temp2;
    set<string> temp3;
    SetUtil::VectorToSet(temp1,MainV);
    SetUtil::VectorToSet(temp2,PublicV);
    SetUtil::Difference(temp1,temp2,temp3);
    SetUtil::SetToVector(temp3,SecV);

    Node* n_0 = new Node("0", OperatorEnums::NULLOPERATOR, NodeTypeEnums::CONSTANT);
    n_0->setDistributionEnums(DistributionEnums::CST);
    nodeMap["0"] = n_0;
    nameMap["0"]="0";

    Node* n_1 = new Node("1", OperatorEnums::NULLOPERATOR, NodeTypeEnums::CONSTANT);
    n_1->setDistributionEnums(DistributionEnums::CST);
    nodeMap["1"] = n_1;
    nameMap["1"]="1";

    Node* rnew = new Node("rnew", OperatorEnums::NULLOPERATOR, NodeTypeEnums::MASK);
    rnew->setDistributionEnums(DistributionEnums::RUD);
    nodeMap["rnew"] = rnew;



    for (unsigned int i = 0; i < RandV.size(); i++) {
        Node* n = new Node(RandV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::MASK);
        n->setDistributionEnums(DistributionEnums::RUD);
        nameMap[RandV[i]]=RandV[i];
        nodeMap[RandV[i]] = n;
    }
    for (unsigned int i = 0; i < SecV.size(); i++) {
        Node* n = new Node(SecV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::SECRECT);
        n->setDistributionEnums(DistributionEnums::UKD);
        nameMap[SecV[i]]=SecV[i];
        nodeMap[SecV[i]] = n;
    }
    for (unsigned int i = 0; i < PublicV.size(); i++) {
        Node* n = new Node(PublicV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::PLAIN);
        n->setDistributionEnums(DistributionEnums::UKD);
        nameMap[PublicV[i]]=PublicV[i];
        nodeMap[PublicV[i]] = n;
    }

    for (unsigned int i = 0; i < ConstantV.size(); i++) {
        Node *n = new Node(ConstantV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::CONSTANT);
        n->setDistributionEnums(DistributionEnums::ISD);
        nameMap[ConstantV[i]]=ConstantV[i];
        nodeMap[ConstantV[i]] = n;
    }


    vector<string> v;
    vector<string> hdName;
    for (unsigned int i = 0; i < CodeV.size(); i++) {
        string line = CodeV[i];
//        cout << line << endl;

        v.clear();
        char* token=(char *)" ";
        char * pch;
        pch = strtok ((char*)line.c_str(),token);
        while (pch != NULL)
        {
            string a = pch;
            v.push_back(a);
            pch = strtok (NULL, token);
        }

        InterV.push_back(v[0]+"_"+std::to_string(i));
        Node* n = new Node(InterV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::INTERMEDIATE);
        nodeMap[InterV[i]] = n;



        if (line.find("*") != -1) {
            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[2]]]);
            nodeMap[InterV[i]]->setRightChild(nodeMap[nameMap[v[4]]]);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::GFMUL);

            nodeMap[nameMap[v[2]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
            nodeMap[nameMap[v[4]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());

        }
        else if (line.find("@") != -1) {
            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[2]]]);
            nodeMap[InterV[i]]->setRightChild(nodeMap[nameMap[v[4]]]);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::MUL);

            nodeMap[nameMap[v[2]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
            nodeMap[nameMap[v[4]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
        }
        else if (line.find("^") != -1) {
            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[2]]]);
            nodeMap[InterV[i]]->setRightChild(nodeMap[nameMap[v[4]]]);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::XOR);

            nodeMap[nameMap[v[2]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
            nodeMap[nameMap[v[4]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
        }
        else if (line.find("&") != -1) {
            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[2]]]);
            nodeMap[InterV[i]]->setRightChild(nodeMap[nameMap[v[4]]]);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::AND);

            nodeMap[nameMap[v[2]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
            nodeMap[nameMap[v[4]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
        }else if (line.find("-") != -1) {
            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[2]]]);
            nodeMap[InterV[i]]->setRightChild(nodeMap[nameMap[v[4]]]);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::MINU);

            nodeMap[nameMap[v[2]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
            nodeMap[nameMap[v[4]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
        }else if (line.find("+") != -1) {
            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[2]]]);
            nodeMap[InterV[i]]->setRightChild(nodeMap[nameMap[v[4]]]);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::ADD);

            nodeMap[nameMap[v[2]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
            nodeMap[nameMap[v[4]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
        }else if (line.find("pow") != -1) {

            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[3]]]);
            nodeMap[InterV[i]]->setRightChild(NULL);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::POW);
            int pow=atoi(v[2].substr(3,2).c_str());
            nodeMap[InterV[i]]->setPow(pow);

            nodeMap[nameMap[v[3]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());

        }else if (line.find("lut") != -1) {

            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[3]]]);
            nodeMap[InterV[i]]->setRightChild(NULL);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::LUT);
            int lut=atoi(v[2].substr(3,2).c_str());
            nodeMap[InterV[i]]->setLut(lut);

            nodeMap[nameMap[v[3]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());

        }else if (line.find("rotl") != -1) {

            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[3]]]);
            nodeMap[InterV[i]]->setRightChild(NULL);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::ROTL);
            int rotl=atoi(v[2].substr(5,2).c_str());
            nodeMap[InterV[i]]->setRotl(rotl);

            nodeMap[nameMap[v[3]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());

        }else if (line.find("tableLUT") != -1) {

            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[3]]]);
            nodeMap[InterV[i]]->setRightChild(NULL);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::TABLELUT);
            int tableLut=atoi(v[2].substr(8,2).c_str());
            nodeMap[InterV[i]]->setTablelut(tableLut);
            cout<<tableLut<<endl;

            nodeMap[nameMap[v[3]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());

        }else if (line.find("affineF") != -1) {

            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[3]]]);
            nodeMap[InterV[i]]->setRightChild(NULL);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::AFFINEF);

            nodeMap[nameMap[v[3]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());

        }else if (line.find("rcon") != -1) {

            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[3]]]);
            nodeMap[InterV[i]]->setRightChild(NULL);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::RCON);

            nodeMap[nameMap[v[3]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());

        }else if (line.find("bnot") != -1) {

            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[3]]]);
            nodeMap[InterV[i]]->setRightChild(NULL);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::NOT);

            nodeMap[nameMap[v[3]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());

        }else if (line.find("sbox") != -1) {

            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[3]]]);
            nodeMap[InterV[i]]->setRightChild(NULL);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::SBOX);

            nodeMap[nameMap[v[3]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());

        }else if (v.size() == 3 && v[1]=="=" ) {

            nodeMap[InterV[i]]->setLeftChild(nodeMap[nameMap[v[2]]]);
            nodeMap[InterV[i]]->setRightChild(NULL);
            nodeMap[InterV[i]]->setOperatorEnums(OperatorEnums::EQUAL);
            nodeMap[InterV[i]]->setDistributionEnums(nodeMap[nameMap[v[2]]]->getDistributionEnums());

            nodeMap[nameMap[v[2]]]->getParentNodeNames()->insert(nodeMap[InterV[i]]->getName());
        }

        if(needHD) {
            if(nameMap[v[0]] != "") {
                cout << "HD(" << nameMap[v[0]] << ", " << InterV[i] << ")" << endl;
                string name = "hd" + to_string(i);
                Node* hdn = new Node(name, OperatorEnums::XOR, NodeTypeEnums::INTERMEDIATE);
                hdn->setLeftChild(nodeMap[nameMap[v[0]]]);
                hdn->setRightChild(n);
                NodeUtil::getAuxiliaryTable(hdn);
                hdName.push_back(name);
                nodeMap[name] = hdn;
            }
        }
        nameMap[v[0]]=InterV[i];
        NodeUtil::getAuxiliaryTable(nodeMap[InterV[i]]);

    }

    InterV.insert(InterV.end(), hdName.begin(), hdName.end());
}

void Parser::parseOld() {

    vector<string> PublicV;
    vector<string> CodeV;

    vector<Node> RandNodeVector;
    vector<Node> SecNodeVector;
    vector<Node> PublicNodeVector;
    vector<Node> InterNodeVector;
    vector<Node> ConstantNodeVector;

    string RandFile = absoluteBenchmarkPath + "/inRandList.txt";
    string SecFile = absoluteBenchmarkPath + "/inSecList.txt";
    string PublicFile = absoluteBenchmarkPath + "/inPubList.txt";
    string CodeFile = absoluteBenchmarkPath + "/code.cpp";

    ifstream infile;
    FileUtil::readFileToNodeVector(RandFile, RandV);
    FileUtil::readFileToNodeVector(SecFile, SecV);
    FileUtil::readFileToNodeVector(PublicFile, PublicV);
    FileUtil::readFileToCodeVector(CodeFile, CodeV);


    for (unsigned int i = 0; i < RandV.size(); i++) {
        Node* n = new Node(RandV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::MASK);
        n->setDistributionEnums(DistributionEnums::RUD);
        nodeMap[RandV[i]] = n;
    }

    for (unsigned int i = 0; i < SecV.size(); i++) {
        Node* n = new Node(SecV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::SECRECT);
        n->setDistributionEnums(DistributionEnums::UKD);
        nodeMap[SecV[i]] = n;
    }

    for (unsigned int i = 0; i < PublicV.size(); i++) {
        Node* n = new Node(PublicV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::PLAIN);
        n->setDistributionEnums(DistributionEnums::UKD);
        nodeMap[PublicV[i]] = n;
    }


    Node* t = new Node("1", OperatorEnums::NULLOPERATOR, NodeTypeEnums::CONSTANT);
    Node* f = new Node("0", OperatorEnums::NULLOPERATOR, NodeTypeEnums::CONSTANT);
    t->setDistributionEnums(DistributionEnums::CST);
    f->setDistributionEnums(DistributionEnums::CST);
    nodeMap["1"] = t;
    nodeMap["0"] = f;


    vector<string> v;

    cout << "start, hold on..." << endl;
    for (unsigned int i = 0; i < CodeV.size(); i++) {

        string str = CodeV[i];
        str.erase(std::remove(str.begin(), str.end(), ';'), str.end());
        StringUtil::trim(str);

        string temp[3];
        vector<string> v1;
        vector<string> v2;

        //add inter node one by one to nodeMap
        v.clear();
        StringUtil::SplitString(str, v, "=");
        InterV.push_back(v[0]);
        Node* n = new Node(v[0], OperatorEnums::NULLOPERATOR, NodeTypeEnums::INTERMEDIATE);
        nodeMap[InterV[i]] = n;


        if (str.find("^") != -1) {
            v1.clear();
            v2.clear();
            StringUtil::SplitString(str, v1, "=");
            temp[0] = v1[0];
            StringUtil::SplitString(v1[1], v2, "^");
            temp[1] = v2[0];
            temp[2] = v2[1];


            nodeMap[temp[0]]->setLeftChild(nodeMap[temp[1]]);
            nodeMap[temp[0]]->setRightChild(nodeMap[temp[2]]);
            nodeMap[temp[0]]->setOperatorEnums(OperatorEnums::XOR);

            nodeMap[temp[1]]->getParentNodeNames()->insert(nodeMap[temp[0]]->getName());
            nodeMap[temp[2]]->getParentNodeNames()->insert(nodeMap[temp[0]]->getName());

        } else if (str.find("&") != -1) {
            v1.clear();
            v2.clear();
            StringUtil::SplitString(str, v1, "=");

            temp[0] = v1[0];
            StringUtil::SplitString(v1[1], v2, "&");
            temp[1] = v2[0];
            temp[2] = v2[1];

            nodeMap[temp[0]]->setLeftChild(nodeMap[temp[1]]);
            nodeMap[temp[0]]->setRightChild(nodeMap[temp[2]]);
            nodeMap[temp[0]]->setOperatorEnums(OperatorEnums::AND);

            nodeMap[temp[1]]->getParentNodeNames()->insert(nodeMap[temp[0]]->getName());
            nodeMap[temp[2]]->getParentNodeNames()->insert(nodeMap[temp[0]]->getName());

        } else if (str.find("|") != -1) {
            v1.clear();
            v2.clear();
            StringUtil::SplitString(str, v1, "=");
            temp[0] = v1[0];
            StringUtil::SplitString(v1[1], v2, "|");
            temp[1] = v2[0];
            temp[2] = v2[1];

            nodeMap[temp[0]]->setLeftChild(nodeMap[temp[1]]);
            nodeMap[temp[0]]->setRightChild(nodeMap[temp[2]]);
            nodeMap[temp[0]]->setOperatorEnums(OperatorEnums::OR);

            nodeMap[temp[1]]->getParentNodeNames()->insert(nodeMap[temp[0]]->getName());
            nodeMap[temp[2]]->getParentNodeNames()->insert(nodeMap[temp[0]]->getName());

        } else if (str.find("~") != -1) {
            v1.clear();
            v2.clear();
            StringUtil::SplitString(str, v1, "=");
            temp[0] = v1[0];
            str = v1[1];
            str.erase(std::remove(str.begin(), str.end(), '~'), str.end());
            temp[1] = str;

            nodeMap[temp[0]]->setLeftChild(nodeMap[temp[1]]);
            nodeMap[temp[0]]->setRightChild(NULL);
            nodeMap[temp[0]]->setOperatorEnums(OperatorEnums::NOT);
            nodeMap[temp[0]]->setDistributionEnums(nodeMap[temp[1]]->getDistributionEnums());

            nodeMap[temp[1]]->getParentNodeNames()->insert(nodeMap[temp[0]]->getName());
        } else {
            v1.clear();
            v2.clear();
            StringUtil::SplitString(str, v1, "=");
            temp[0] = v1[0];
            temp[1] = v1[1];

            nodeMap[temp[0]]->setLeftChild(nodeMap[temp[1]]);
            nodeMap[temp[0]]->setRightChild(NULL);
            nodeMap[temp[0]]->setOperatorEnums(OperatorEnums::EQUAL);
            nodeMap[temp[0]]->setDistributionEnums(nodeMap[temp[1]]->getDistributionEnums());

            nodeMap[temp[1]]->getParentNodeNames()->insert(nodeMap[temp[0]]->getName());
        }
        NodeUtil::getAuxiliaryTable(nodeMap[InterV[i]]);
    }
}

void Parser::parse(){

    if(absoluteBenchmarkPath.find(".ec") != absoluteBenchmarkPath.npos) {
        parseForECFile();
    } else {
        parseOld();
    }
}
