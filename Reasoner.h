//
// Created by Pengfei Gao on 2018/8/17.
//

#ifndef HOSCINFER_REASONER_H
#define HOSCINFER_REASONER_H


#include <map>
#include <vector>
#include "Node.h"
#include <fstream>
#include <list>



class Reasoner {
private:
    int bitLength;
    int numberOfNPM;
    int numberOfSidRule;
    int numberOfSidSMT;
    int totalCheck;
    int numberOfNoKey;

    int numberOfUKD;

    bool disableSimplify;

    string result;

    string zero;
    string one;

    map<string, Node*>& nodeMap;
    vector<string> interV;
    vector<string> randV;
    vector<string> secretV;
    set<string> secret;
    map<string, int> stringToInt;
    void check(const set<Node* > &subset, int d);
public:
    Reasoner(int b, map<string, Node*>& n, vector<string> i, vector<string> r, vector<string> s) : bitLength(b),  nodeMap(n), interV(i), randV(r), secretV(s) {
        for(int p = 0; p < randV.size(); p++) {
            stringToInt[randV[p]] = p;
        }
        numberOfNPM = 0;
        numberOfSidRule = 0;
        numberOfSidSMT = 0;
        totalCheck = 0;
        numberOfNoKey = 0;

        numberOfUKD = 0;

        disableSimplify = true;

        for(int j = 0; j < s.size(); j++) {
            secret.insert(" " + s[j] + " ");
        }

        if(b == 1) {
            zero = "0";
            one = "1";
        } else {
            zero = "Ox00";
            one = "Oxff";
        }

    }

    void reasonForOneOrder();
    bool isDontCareRand(Node n, string rand);
    void reasonForOneOrderQMS();
    void reasonForOneOrderCounting();
    Node* parser(list<string> *s, string K, string R, map<string, DistributionEnums>& alreadyKnown, int & smtcount);
    void reasonForOneOrderQMSZ3();
    DistributionEnums getDistributionbyCounting(Node* tempNode);



    void printResult() {
        cout << "total Check: " << totalCheck << endl;
        cout << "Number of no key: " << numberOfNoKey << endl;
        cout << "Number of NPM: " << numberOfNPM << endl;
        cout << "Number of SidRule: " << numberOfSidRule << endl;
        cout << "Number of SidSMT: " << numberOfSidSMT << endl;
    }

    void removeZeroAndOne(Node *node);
    void mutate(Node* node, string n, bool b);

    void simplifyICRBv(Node* node);

    void removeXor(Node* node, bool& changed);

    void getSimplifiedAndSetSemd(Node* orginNode, Node* tempNode, bool b);

    void nodeElimate(Node* tempNode, string expressName, string candidateDom);
    void doninatedElimitForOneNode(Node* current, bool &f);

    void reasonForOneOrderOracle();
    double getQMSbyCounting(Node* tempNode);

    string getResult() {
        return result;
    }
    void setDisableSimplify(bool flag) {
        disableSimplify = flag;
    }

    static void initWidth(int bw);


};


#endif //HOSCINFER_REASONER_H
