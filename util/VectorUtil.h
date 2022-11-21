#pragma once
#include <iostream>  
#include <algorithm>
#include <string>
#include <vector>

using namespace std;
class VectorUtil {
public:
    static bool contain(vector<string>& v, string t) {
        for(int i = 0; i < v.size(); i++) {
            if(v[i] == t)
                return true;
        }
        return false;
    }

    static vector<string> difference(vector<string>& all, set<string> temp) {
        vector<string> result = all;
        set<string>::iterator it;
        for(it = temp.begin(); it != temp.end(); it++) {
            vector<string>::iterator itv = result.begin();
            for(; itv!= result.end();) {
                if(*itv == *it) {
                    itv = result.erase(itv);
                    break;
                } else {
                    itv++;
                }
            }
        }

        return result;

    }


};
