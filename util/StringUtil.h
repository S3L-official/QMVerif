#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "../Node.h"
#include <math.h>
#include <limits>
using namespace std;

class StringUtil
{
public:
	
	//erase all space in string
	static void trim(string &s)
	{
		/*
		if( !s.empty() )
		{
		s.erase(0,s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
		}
		*/
		int index = 0;
		if (!s.empty())
		{
			while ((index = s.find(' ', index)) != string::npos)
			{
				s.erase(index, 1);
			}
		}

	}

	static void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
	{
		std::string::size_type pos1, pos2;
		pos2 = s.find(c);
		pos1 = 0;
		while (std::string::npos != pos2)
		{
			v.push_back(s.substr(pos1, pos2 - pos1));

			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if (pos1 != s.length())
			v.push_back(s.substr(pos1));
	}
	
	static vector<string> SplitStringBySpace(const std::string& s) {
		vector<string> result;
		int iBegin = 0;
		string::size_type iLatter = 0;
		string::size_type iFormer = string::npos;
		
		while (1)
		{
			iLatter = s.find_first_not_of(' ', iLatter);
			if (string::npos == iLatter)
			{
				break;
			}

			iFormer = s.find_first_of(' ', iLatter + 1);
			if (string::npos == iFormer)
			{
				iFormer = s.length();
			}
			
			// str2, str3, str4
			//string strNew(s, iLatter, iFormer - iLatter);
			result.push_back(s.substr(iLatter, iFormer - iLatter));

			iLatter = iFormer + 1;
			
		}
		return result;
	}

	static string getString(long long int  n)

	{
		return std::to_string(n);

	}
	static string getString(int  n)

	{
		return std::to_string(n);

	}


	// replace all occurance of t in s to w  
	static void replace_all(std::string & s, std::string const & t, std::string const & w)
	{
		string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();
		while (pos != std::string::npos) { // found   
			s.replace(pos, t_size, w);
			pos = s.find(t, pos + r_size);
		}
	}

	//replace string in vector and map
	static void  replaceThroughMap(string &s, vector<string> &v, map<string, string> &m) {
		for (unsigned int i = 0; i < v.size(); i++) {
			replace_all(s, v[i], m[v[i]]);
		}
	}
	

	static void IntToBinaryVector(int n, vector<int> &v,int len)
	{
		int temp;
		temp = n;
		int count = 0;
		while (temp != 0 )
		{
			v.push_back(temp % 2);
			count++;
			temp = temp>>1;
		}
		while (count < len) {
			v.push_back(0);
			count++;
		}
	}

	static vector<vector<int>> getDistribution(int bit, int randSize) {
		vector<vector<int>> result;
//		int total = pow(2, bit * randSize);
		uint64_t total;

		if(randSize <= 4) {
			total = pow(2, bit * randSize);
		} else if(randSize == 8){
			total = std::numeric_limits<uint64_t >::max();
		} else {
			assert(false);
		}
		vector<int> first;
		for(int i =0; i < randSize; i++) {
			first.push_back(0);
		}
		result.push_back(first);
		int bitMax = pow(2, bit) - 1;

		vector<int> next = first;
		for(uint64_t i = 1; i < total; i++) {
			if(i % 100000000000000000 == 0) {
				cout << i << endl;
			}
			for(int j = randSize - 1; j >= 0; j--) {
				if(next[j] < bitMax) {
					next[j]++;
					break;
				} else if(next[j] == bitMax) {
					j--;
					while(next[j] == bitMax)
						j--;
					next[j] += 1;
					for(int p = j + 1; p <= randSize - 1; p++)
						next[p] = 0;
					break;
				}
			}

			result.push_back(next);
		}
		return result;
	}

	static void BitToBoolVector(vector<string> &rand,vector<int> &tag, map<string,string> &m) {
		for (unsigned int i = 0; i < rand.size(); i++) {
			if (tag[i] == 1) {
				m[rand[i]] = " true ";
			}
			else {
				m[rand[i]] = " false ";
			}
		}
	}

	static void BitToBoolVectorBv(vector<string> &rand, vector<int> &tag, map<string,string> &m, string bitLength) {
		for (unsigned int i = 0; i < rand.size(); i++) {
			m[rand[i]] = " (_ bv" + StringUtil::getString(tag[i]) + " " + bitLength + ")";
		}
	}

	static void VectorLogicAdd(vector<string> &v, string &s) {
		string rightAng = "";
		for(long long int i = 0; i < v.size(); i++) {
			if( i < (v.size() - 1)) {
				s += "( bvadd " + v[i] + " ";
				rightAng += ")";
			}
			else
				s += v[i];
		}

		s += " " + rightAng;

	}


	
};


