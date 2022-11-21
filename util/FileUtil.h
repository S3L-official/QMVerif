#pragma once
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
#include "StringUtil.h"

using namespace std;
class FileUtil {
public:
	
	//read a line
	static void readTxtByLine(string file)
	{
		ifstream infile;
		infile.open(file.data());   //link the file object and the file stream object 
		assert(infile.is_open());   //test if read a file failed

		string s;
		while (getline(infile, s))
		{
			cout << s << endl;
		}
		infile.close();           
	}

	//read a char, don't cout space and tab
	static void readTxtBySpace(string file) {
		ifstream infile;
		infile.open(file.data());  
		assert(infile.is_open());  

		char c;
		while (!infile.eof())
		{
			infile >> c;
			cout << c << endl;

		}
		infile.close();             
	}
		
	//read a char
	static void readTxtByChar(string file)
	{
		ifstream infile;
		infile.open(file.data());   
		assert(infile.is_open());   

		char c;
		infile >> noskipws;
		while (!infile.eof())
		{
			infile >> c;
			cout << c << endl;

		}
		infile.close();             
	}

	static void readFileToNodeVector(string filePath, vector<string> &vec) {
		ifstream infile;
		string s;
		cout << filePath << endl;
		infile.open(filePath.data());   //link the file object and the file stream object 
		assert(infile.is_open());   //test if read a file failed
		while (getline(infile, s))
		{
			vector<string> temp;
			temp = StringUtil::SplitStringBySpace(s);
			for (unsigned int i = 0; i < temp.size();i++) {
				vec.push_back(temp[i]);
			}
		}
		infile.close();
	}

	static void readFileToCodeVector(string filePath, vector<string> &vec) {
		ifstream infile;
		string s;
		cout << filePath << endl;
		infile.open(filePath.data());   //link the file object and the file stream object 
		assert(infile.is_open());   //test if read a file failed
		while (getline(infile, s))
		{
			string temp;
			if(s.size()>0){
				temp=s.substr(0, s.size() -1 );
				if (temp.find("=") != -1) {
					vec.push_back(temp);
				}
			}

		}
		infile.close();
	}

	static void readOracle(map<string, string>& oracle) {
		ifstream infile;
		string s;
		string filePath = "../A2B_oracle.txt";
		infile.open(filePath.data());   //link the file object and the file stream object
		assert(infile.is_open());   //test if read a file failed
		while (getline(infile, s))
		{
			string temp1;
			string temp2;
			if(s.size()>0){
				temp1 = s.substr(0, s.find("="));
			    temp2 = s.substr(s.find("=") + 1, s.size() - 1);
			    vector<string> temp1V;
			    StringUtil::SplitString(temp1, temp1V, ":");
			    int test = stoi(temp1V[0]);
			    string& name = temp1V[1];
			    StringUtil::trim(name);
			    string nodeName  = name + "_" + to_string(test);
			    oracle[nodeName] = temp2;
			    cout << nodeName << "-->" << temp2 << endl;
			}

		}
		infile.close();

	}

};
