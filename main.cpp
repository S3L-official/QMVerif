#include <iostream>
#include <string>
#include <algorithm>
#include "Parser.h"
#include "Reasoner.h"
#include "Global.h"

using namespace std;

class InputParser{
public:
    InputParser (int &argc, char **argv){
        for (int i=1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }
    /// @author iain
    const std::string& getCmdOption(const std::string &option) const{
        std::vector<std::string>::const_iterator itr;
        itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()){
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }
    /// @author iain
    bool cmdOptionExists(const std::string &option) const{
        return std::find(this->tokens.begin(), this->tokens.end(), option)
               != this->tokens.end();
    }
private:
    std::vector <std::string> tokens;
};

/**
 * @param name benchmarkname
 * @param order  order number
 * @param bitLength bit
 */
void driver(string path, int bitLength, bool disableSimplify, int bf, int oracle) {

    Parser p(path);
    p.parse();
    Reasoner reasoner(bitLength, p.getNodeMap(), p.getInterV(), p.getRandV(), p.getSecV());
    reasoner.setDisableSimplify(disableSimplify);

    if(bf == 1 && !oracle)
        reasoner.reasonForOneOrderCounting();
    else if(bf == 0 && !oracle)
        reasoner.reasonForOneOrder();
    else if(bf == 1 && oracle)
        reasoner.reasonForOneOrderOracle();
    else if(bf == 0 && oracle)
        reasoner.reasonForOneOrderOracle();

    cout << reasoner.getResult() << endl;

    ofstream fout;
    if(bf == 1)
        fout.open(path + "_resultBF.txt");
    else
        fout.open(path + "_resultSMT.txt");

    fout << reasoner.getResult();
    fout << flush;
    fout.close();
}

void driverQMS(string path, int bitLength, bool disableSimplify, int bf) {

    Parser p(path);
    p.parse();
    Reasoner reasoner(bitLength, p.getNodeMap(), p.getInterV(), p.getRandV(), p.getSecV());
    reasoner.setDisableSimplify(disableSimplify);

    if(bf == 1){
        reasoner.reasonForOneOrderQMS();
    } else {
        reasoner.reasonForOneOrderQMSZ3();
    }


    cout << reasoner.getResult() << endl;
//    reasoner.printResult();

    ofstream fout;
    if(bf == 1)
        fout.open(path + "_QMSresultSBF.txt");
    else
        fout.open(path + "_QMSresultZ3.txt");

    fout << reasoner.getResult();
    fout << flush;
    fout.close();
}


int main(int argc, char* argv[]) {

    InputParser inputParser(argc, argv);

    string path = inputParser.getCmdOption("-p");
    int qms = 0;
    if(inputParser.cmdOptionExists("-qms"))
        qms = 1;

    int needSimplify = 0;
    if(inputParser.cmdOptionExists("-sim"))
        needSimplify = 1;

    int bf = 0;
    if(inputParser.cmdOptionExists("-bf"))
        bf = 1;

    int oracle = 0;
    if(inputParser.cmdOptionExists("-oracle"))
        oracle = 1;
    if(inputParser.cmdOptionExists("-hd"))
        needHD = 1;

    int bitlength = atoi(inputParser.getCmdOption("-b").c_str());

    if(qms == 0) {
        if(needSimplify)
            driver(path, bitlength, false, bf, oracle);
        else
            driver(path, bitlength, true, bf, oracle);
    }
    else {
        if(needSimplify)
            driverQMS(path, bitlength, false, bf);
        else
            driverQMS(path, bitlength, true, bf);
    }


   return 0;
}




