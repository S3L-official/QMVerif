//
// Created by Pengfei Gao on 2018/8/17.
//

#ifndef HOSCINFER_PARSER_H
#define HOSCINFER_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cstring>
#include <fstream>
#include "Node.h"
using namespace std;


class Parser {
private:
    string absoluteBenchmarkPath;
    map<string, Node*> nodeMap;
    vector<string> InterV;
    vector<string> RandV;
    vector<string> SecV;
    vector<string> ConstantV;


    char* readBenchmark(char* benchmarkPath) {
        filebuf *pbuf;
        ifstream filestr;
        long size;
        char * buffer;
        filestr.open (benchmarkPath, ios::binary);
        pbuf=filestr.rdbuf();

        size=pbuf->pubseekoff (0,ios::end,ios::in);
        pbuf->pubseekpos (0,ios::in);

        buffer=new char[size];

        pbuf->sgetn (buffer,size);

        filestr.close();

        return buffer;
    }

    vector<string> getLines(char* str) {
        vector<string> ret;
        char * pch;
        pch = strtok (str,"\n");
        while (pch != NULL)
        {
            string a = pch;
            if (a.size() > 1)
                ret.push_back(a);

            pch = strtok (NULL, "\n");
        }
        return ret;
    }

    vector<string> getDefineFunction(vector<string> vec) {
        vector<string> ret;
        for (unsigned i = 0; i < vec.size(); i++) {
            if (vec[i].find("module") != string::npos) {
                break;
            }
            ret.push_back(vec[i]);

        }
        return ret;
    }

    vector<string> getPlainVector(vector<string>  lines) {
        vector<string> plain;
        for (unsigned i = 0; i < lines.size(); i++) {
            string line=lines[i];
            vector<string> v;
            char* token;
            if (line.find("Sharing") != string::npos) {
                token = (char*)" ()*.,;:\t\r\n";
                char * pch;
                pch = strtok ((char*)line.c_str(),token);
                while (pch != NULL)
                {
                    string a = pch;
                    if(a.find("'") != string::npos)
                        a = a.replace(a.find("'"), 1, "_");
                    v.push_back(a);
                    pch = strtok (NULL, token);
                }
                plain.push_back(v[1]);
            }

        }
        return plain;
    }

    vector<string> getMain(vector<string> vec) {
        bool main_flag = false;
        vector<string> ret;
        for (unsigned i = 0; i < vec.size(); i++) {
            if (vec[i].find("main") != string::npos) {
                main_flag = true;
            }

            if (main_flag) ret.push_back(vec[i]);

            if (vec[i].find("}") != string::npos && main_flag) break;
        }

        return ret;
    }

    vector<vector<string>> getTokenize(vector<string> vec) {
        vector<vector<string>> ret;
        for (unsigned i = 0; i < vec.size(); i++) {
            string line = vec[i];
            vector<string> v;
            char* token;

            // comments or non-comments
            if (line.find("(*") != string::npos && line.find("*)") != string::npos) {
                token = (char*)" .,;:\t\r\n";
            } else {
                token = (char*)" .,;:()\t\r\n";
            }

            char * pch;
            pch = strtok ((char*)line.c_str(),token);
            while (pch != NULL)
            {
                string a = pch;
                if(a.find("'") != string::npos)
                    a = a.replace(a.find("'"), 1, "_");
                v.push_back(a);
                pch = strtok (NULL, token);
            }
            ret.push_back(v);
        }

        return ret;
    }

    vector<string> getRandVector(vector<vector<string>>  tokens) {
        vector<string> rand;
        // get variables with %distr value
        for (unsigned i = 0; i < tokens.size(); i++) {
            vector<string> &line = tokens[i];
            if (line.size() == 3 && line[2] == "$distr") {
                rand.push_back(line[0]);
                line = {""};
            }
        }
        return rand;
    }

    vector<string> getConstantVector(vector<vector<string>>  tokens) {
        vector<string> constant;
        for (unsigned i = 0; i < tokens.size(); i++) {
            vector<string> &line = tokens[i];
            if (line.size() == 3 && line[0] == "op" && line[2]=="byte" && line[1].find("Ox")!= string::npos) {
                constant.push_back(line[1]);
                line = {""};
            }
        }
        return constant;
    }

    vector<string> getMainInputVector(vector<vector<string>>  tokens) {
        vector<string> input;
        vector<string> &line = tokens[0];
        for (unsigned i = 2; i < line.size(); i++) {
            if (line[i] == "byte") break;
            input.push_back(line[i]);
        }
        return input;
    }

    string getCodeLine(vector<string> line) {
        string s="";
        for (unsigned i = 0; i < line.size(); i++) {
            s=s+line[i]+ " ";
        }
        return s;
    }

    int tempNum = 0;
    string getTemp() {
        stringstream ss;
        ss << "temp_" << tempNum;
        tempNum++;
        string temp = ss.str();
        return temp;
    }

    vector<string> getInterCodeVector(vector<vector<string>> tokens) {

        vector<string> codeVector;
        for (unsigned i = 0; i < tokens.size(); i++) {
            vector<string> line = tokens[i];
            // empty line
            if (line.size() <= 1) continue;

                // xor
            else if (line.size() >= 5 && line[3] == "^") {

                if(line.size() > 5) {
                    int num = (line.size() - 3) / 2;
                    string oldTemp="";
                    for (int i = 1; i <= num; i++) {
                        string temp = getTemp();
                        string newLine="";
                        if (i==1){
                            newLine=temp+" = "+line[2]+" ^ "+line[4];
                            oldTemp=temp;
                        }else if(i==num){
                            newLine=line[0]+" = "+oldTemp+" ^ "+line[line.size()-1];
                        }else{
                            newLine=temp+" = "+oldTemp+" ^ "+line[2*i+2];
                            oldTemp=temp;
                        }
                        codeVector.push_back(newLine);
                        //cout<<newLine<<endl;
                    }
                } else {
                    codeVector.push_back(getCodeLine(line));
                }

            }

            else if (line.size() >= 5 && line[3] == "*" && line[4].find("pow")!= string::npos) {
                string temp=getTemp();
                string s=temp+" = "+line[4]+" "+line[5];
                //cout<<s<<endl;
                codeVector.push_back(s);
                s=line[0]+" = "+line[2]+" * "+temp;
                codeVector.push_back(s);
                // cout<<s<<endl;
            }


            else if (line.size() >= 5 && line[3] == "*") {

                if(line.size() > 5) {
                    int num = (line.size() - 3) / 2;
                    cout<<num<<endl;
                    string oldTemp="";
                    for (int i = 1; i <= num; i++) {
                        string temp = getTemp();
                        string newLine="";
                        if (i==1){
                            newLine=temp+" = "+line[2]+" * "+line[4];
                            oldTemp=temp;
                        }else if(i==num){
                            newLine=line[0]+" = "+oldTemp+" * "+line[line.size()-1];
                        }else{
                            newLine=temp+" = "+oldTemp+" * "+line[2*i+2];
                            oldTemp=temp;
                        }
                        codeVector.push_back(newLine);
                        cout<<newLine<<endl;
                    }
                } else {
                    codeVector.push_back(getCodeLine(line));
                }

            }

            else if (line.size() >= 5 && line[3] == "@") {

                if(line.size() > 5) {
                    int num = (line.size() - 3) / 2;
                    cout<<num<<endl;
                    string oldTemp="";
                    for (int i = 1; i <= num; i++) {
                        string temp = getTemp();
                        string newLine="";
                        if (i==1){
                            newLine=temp+" = "+line[2]+" @ "+line[4];
                            oldTemp=temp;
                        }else if(i==num){
                            newLine=line[0]+" = "+oldTemp+" @ "+line[line.size()-1];
                        }else{
                            newLine=temp+" = "+oldTemp+" @ "+line[2*i+2];
                            oldTemp=temp;
                        }
                        codeVector.push_back(newLine);
                        cout<<newLine<<endl;
                    }
                } else {
                    codeVector.push_back(getCodeLine(line));
                }

            }

            else if (line.size() >= 5 && line[3] == "&") {

                if(line.size() > 5) {
                    int num = (line.size() - 3) / 2;
                    cout<<num<<endl;
                    string oldTemp="";
                    for (int i = 1; i <= num; i++) {
                        string temp = getTemp();
                        string newLine="";
                        if (i==1){
                            newLine=temp+" = "+line[2]+" & "+line[4];
                            oldTemp=temp;
                        }else if(i==num){
                            newLine=line[0]+" = "+oldTemp+" & "+line[line.size()-1];
                        }else{
                            newLine=temp+" = "+oldTemp+" & "+line[2*i+2];
                            oldTemp=temp;
                        }
                        codeVector.push_back(newLine);
                        cout<<newLine<<endl;
                    }
                } else {
                    codeVector.push_back(getCodeLine(line));
                }

            }
            else if (line.size() == 5 && line[3].find("-")!= string::npos ){
                codeVector.push_back(getCodeLine(line));
            }
            else if (line.size() == 5 && line[3].find("+")!= string::npos ){
                codeVector.push_back(getCodeLine(line));
            }


            else if (line.size() == 4 && line[2].find("pow")!= string::npos ){
                codeVector.push_back(getCodeLine(line));
            }

            else if (line.size() == 4 && line[2] == "affineF") {
                codeVector.push_back(getCodeLine(line));
            }

            else if (line.size() == 4 && line[2] == "rcon") {
                codeVector.push_back(getCodeLine(line));
            }

            else if (line.size() == 4 && line[2] == "bnot") {
                codeVector.push_back(getCodeLine(line));
            }

            else if (line.size() == 4 && line[2] == "sbox") {
                codeVector.push_back(getCodeLine(line));
            }

            else if (line.size() == 4 && line[2].find("lut")!= string::npos) {
                codeVector.push_back(getCodeLine(line));
            }

            else if (line.size() == 4 && line[2].find("rotl")!= string::npos) {
                codeVector.push_back(getCodeLine(line));
            }

            else if (line.size() == 4 && line[2].find("tableLUT")!= string::npos) {
                codeVector.push_back(getCodeLine(line));
            }

            else if (line.size() == 3 && line[1]=="=" && line[2].find("distr")== string::npos) {
                codeVector.push_back(getCodeLine(line));
            }


        }
        return codeVector;

    }
public:
    Parser(string path) {
        absoluteBenchmarkPath = path;
    }
    void parseOld();
    void parse();
    map<string, Node*>& getNodeMap();
    vector<string> getInterV();
    vector<string> getRandV();
    vector<string> getSecV();
    void parseForECFile();




};


#endif //HOSCINFER_PARSER_H
