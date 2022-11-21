#pragma once
#include <iostream>  
#include <set>  
#include <algorithm>  
#include <string>
#include <vector>

using namespace std;
class SetUtil {
public:


	static void put_out(set<string> &p) {
		set<string>::iterator it = p.begin();
		while (it != p.end()) {
			cout << *it << " ";
			it++;
		}
		cout << "\n";
	}

	static string getFirstElement(set<string> &p) {
		set<string>::iterator it = p.begin();
		while (it != p.end()) {
			return *it;
		}
		return "not";
	}

	static Node* getFirstElementAndRemove(set<Node*> &p) {
	    if(p.size() == 0)
	    	return NULL;
		set<Node*>::iterator it = p.begin();
		while (it != p.end()) {
		    Node* temp = *it;
		    p.erase(it);
			return temp;
		}
		return NULL;
	}

	static void VectorToSet(set<string> &s, vector<string> &v) {
		for(unsigned int i=0;i<v.size();i++){
			s.insert(v[i]);
		}
	}
	static void Intersection(set<string> &A, set<string> &B, set<string> &result) {
		set<string>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			if (B.find(*it) != B.end()) result.insert(*it);
			it++;
		}
	}

	static void Intersection(set<int> &A, set<int> &B, set<int> &result) {
		set<int>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			if (B.find(*it) != B.end()) result.insert(*it);
			it++;
		}
	}



	static void Union(set<string> &A, set<string> &B, set<string> &result) {
		set<string>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			result.insert(*it);
			it++;
		}

		it = B.begin();
		while (it != B.end()) {
			result.insert(*it);
			it++;
		}
	}

	static void Union(set<Node*> &A, set<Node*> &B, set<Node*> &result) {
		set<Node*>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			result.insert(*it);
			it++;
		}

		it = B.begin();
		while (it != B.end()) {
			result.insert(*it);
			it++;
		}
	}
	static void Union(set<int> &A, set<int> &B, set<int> &result) {
		set<int>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			result.insert(*it);
			it++;
		}

		it = B.begin();
		while (it != B.end()) {
			result.insert(*it);
			it++;
		}
	}
	static void Difference(set<string> &A, set<string> &B, set<string> &result) {
		set<string>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			if (B.find(*it) == B.end())
                result.insert(*it);
			it++;
		}
	}

	static void Difference(set<Node*> &A, set<Node*> &B, set<Node*> &result) {
		set<Node*>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			if (B.find(*it) == B.end()) result.insert(*it);
			it++;
		}
	}

	static void Difference(set<int> &A, set<int> &B, set<int> &result) {
		set<int>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			if (B.find(*it) == B.end()) result.insert(*it);
			it++;
		}
	}

	static bool Contain(set<string> &A, string temp) {
		bool flag = false;
		set<string>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			if (*it == temp) {
				flag = true;
				break;
			}
			else {
				it++;
			}
		}

		return flag;
	}

	static string printSet(set<string> &A) {
		string test = "{";
		set<string>::iterator it;
		for(it = A.begin(); it != A.end(); it++) {
			test += *it;
			test += ", ";
		}
		test += "}";
		return test;

	}

	static bool containValue(set<string> &A, string temp) {
		bool flag = false;

		if(A.find(temp)!=A.end()){
            flag=true;
		}

		return flag;
	}

	static void SetToVector(set<string> &s, vector<string> &v) {
		set<string>::iterator it = s.begin();
		while (it != s.end()) {
			string ss = *it;
			v.push_back(ss);
			it++;
		}
	}

	static void SetToVectorForNode(set<Node*> &s, vector<Node*> &v) {
		set<Node*>::iterator it = s.begin();
		while (it != s.end()) {
			Node* ss = *it;
			v.push_back(ss);
			it++;
		}
	}

	static bool remove(set<string> &s, string ss) {
		set<string>::iterator it;
		it = s.begin();
		while(it != s.end()){
			if(*it == ss) {
				s.erase(it);
				return true;
			} else {
				it++;
			}
		}
		return false;
	}



	static string SetToString(set<string> &s) {
		string str="";
		set<string>::iterator it = s.begin();
		while (it != s.end()) {
			string ss = *it;
			str=str+" "+ss;
			it++;
		}
		return str;
	}

	static shared_ptr<set<string>> copySet(shared_ptr<set<string>> s) {
		if(s == NULL)
			return NULL;
		shared_ptr<set<string>> result = make_shared<set<string>>();
//		set<string>* result = new set<string>();

		set<string>::iterator it = s->begin();
		while (it != s->end()) {
			result->insert(*it);
		    it++;
		}
		return result;

	}


};
