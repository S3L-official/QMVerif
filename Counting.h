#pragma once

#include <cstdio>
#include<cmath>
#include<set>
#include<sstream>
#include<string>
#include<iostream>
#include<stack>
#include<map>
#include<list>
#include"ExpTree.h"
#include "Global.h"

using namespace std;

int timeLimit = 900;


class Counting{

public:	

static unsigned long long upperbound;
static unsigned long long expectedProb;
static int bvwidth;
static int domain;
static string operators;
static int ComputeQms;
static int isRUD;
static double maxqms;

static string removeSpace(string &l)
{
	stringstream stream;	
	while(l.find("\t",0)!=string::npos)
	{
		unsigned int loc=l.find("\t",0);
		l.erase(loc);
	}
	while(l.find("\n",0)!=string::npos)
	{
		unsigned int loc=l.find("\n",0);
		l.erase(loc);
	}
	while(l.find(" ",0)!=string::npos)
	{
		unsigned int loc=l.find(" ",0);
		l.erase(loc);
	}
	return l;
}


static double QMS(map<string,unsigned long long> *dist1,map<string,unsigned long long> *dist2,map<string,unsigned long long> maxdist)
{
	
	set<string> keys;
	
	map<string,unsigned long long>::iterator it,itt,ittt;
	for(it=dist1->begin();it!=dist1->end();it++)
	{
		keys.insert(it->first);
	
	}
	for(itt=dist2->begin();itt!=dist2->end();itt++)
	{
		keys.insert(itt->first);
	
	}
	for(ittt=maxdist.begin();ittt!=maxdist.end();ittt++)
	{
		keys.insert(ittt->first);
	
	}
	
	maxqms=0.0;
	int v1,v2,v3;
	int dv,mxdv;
	set<string>::iterator ip;
	for(ip=keys.begin();ip!=keys.end();ip++)
	{
		
		if(dist1->find(*ip)!=dist1->end())
		{
			v1=(*dist1)[*ip];
		}else{
			v1=0;
		}
		if(dist2->find(*ip)!=dist2->end())
		{
			v2=(*dist2)[*ip];
		}else{
			v2=0;
		}
		if(maxdist.find(*ip)!=maxdist.end())
		{
			v3=maxdist[*ip];
		}else{
			v3=v1;
		}
		
		if(v1>v2){
			dv=v2;
		}else{
			dv=v1;
		}
		if(v2>v3){
			mxdv=v2;
		}else{
			mxdv=v3;
		}
		(*dist1)[*ip]=dv;
		maxdist[*ip]=mxdv;
		if(maxqms<mxdv-dv)
		{
			maxqms=mxdv-dv;
		}
		if(maxqms==Counting::upperbound)
		{
			return Counting::upperbound;
		}
		
	
	
	}
	return maxqms;
}




static int ExploitRandoms(clock_t startTime, ExpTree *f,int index,list<string> *randoms,map<string,unsigned long long> context,map<string,unsigned long long> *dist)
{
	string rr,v;
	int vv;	
	list<string>::iterator it,itt,it1;
	it=randoms->begin();
	for(int i=0;i<index;i++)
	{it++;}
	rr=*it;
	map<string,unsigned long long>::iterator itt1;

    clock_t current = clock();
	if((current - startTime)/1000000 > timeLimit) {
	    return -1;
	}
	if(index==0)
	{
		for(int i=0;i<domain;i++)
		{
			context[rr]=i;
			
			
			vv=f->evaluate(context);
			
			stringstream stream;
			stream<<vv;
			v=stream.str();
			map<string,unsigned long long>::iterator itt1;

			map<string,unsigned long long>::iterator it;
			if(dist->find(v)!=dist->end())
			{
				(*dist)[v]++;
			}else{
				(*dist)[v]=1;
			}
			
		}
		context[rr]=0;
		return 1;
	}else{
	    int res;
		for(int i=1;i<domain;i++)
		{
			context[rr]=i;
			
			res = ExploitRandoms(startTime, f,index-1, randoms,context, dist);
		}
		context[rr]=0;
		return res;
		
	}



}

static int count(clock_t startTime, ExpTree *f,list<string>* randoms,map<string,unsigned long long> context,map<string,unsigned long long> *dist)
{
	stringstream stream;
	if(randoms->size()==0)
	{
		int vv=f->evaluate(context);
		string v;
		stream<<vv;
		stream>>v;
		(*dist)[v]=1;
		
	}else{
		for(int i=0;i<randoms->size();i++)
		{
			int re = ExploitRandoms(startTime, f,i,randoms,context,dist);
			if(re == -1)
			    return -1;
		}
	}
	return 1;
}
static bool CompareMap(map<string,unsigned long long> &map1, map<string,unsigned long long> &map2) 
{ 
	map<string,unsigned long long>::iterator it1,it2;

	map<string,unsigned long long>::iterator it = map1.begin();
	while( it != map1.end()) 
	{ 
		map<string,unsigned long long>::iterator it2 = map2.find(it->first);
		if (it2 != map2.end())
		{ 
			if (it2->second != it->second) 
			{ 
				return false;
			}
		}else{ 
			return false;
		}
		it++; 
	} 
	return true; 
}

static double ExploitKey(clock_t startTime, ExpTree *f,int index,list<string> *keys,list<string> *randoms,map<string,unsigned long long> context,map<string,unsigned long long> *dist1,map<string,unsigned long long> *dist2,map<string,unsigned long long> maxdist)
{
	int k;
	string kk;
	list<string>::iterator it;
	it=keys->begin();
	for(int i=0;i<index;i++)
	{it++;}
	kk=*it;
	maxqms=0.0;

	clock_t current = clock();
	if((current - startTime)/1000000 > timeLimit) {
	    return -1;
	}

	if(index==0)
	{
		for(int i=0;i<domain;i++)
		{
			context[kk]=i;
			if(dist1->empty())
			{
				int re = count(startTime,f,randoms,context,dist1);
				if(re == -1) return -1;

			}else{
				int re = count(startTime,f,randoms,context,dist2);
                if(re == -1) return -1;

				if(ComputeQms)
				{
					double result=QMS(dist1,dist2,maxdist);
					if(result==Counting::upperbound)
					{
						return Counting::upperbound;
					}else{
						if(maxqms<result)
						{
							maxqms=result;
						}
					}
				}else{
					if(CompareMap(*dist1,*dist2)==false)
					{
						return 0.0;
					}else if(isRUD)
					{
						map<string,unsigned long long>::iterator i;
						for(i=dist1->begin();i!=dist1->end();i++)
						{
							if(i->second!=expectedProb)
							{
								isRUD=false;
								break;
							}
						}
						
					}

				}
				dist2->clear();
			}
		}
		context[kk]=0;
		
		
	}else{
		for(int i=1;i<domain;i++)
		{
            		context[kk]=i;
            		
            		double result = ExploitKey(startTime, f,index-1,keys,randoms,context,dist1, dist2,maxdist);
            		if(result == -1) return -1;
            		if(ComputeQms)
			{
				if(result==Counting::upperbound)
				{
					return Counting::upperbound;
				}
				
			}else if(not result)
			{
				return 0.0;
			}
		}
        	context[kk]=0;
	
	}
	if(ComputeQms)
	{
		return maxqms;
	}else{
		return 1.0;
	}
	




}
static double ExploitKeys(clock_t startTime, ExpTree *f,list<string> *keys,list<string> *randoms,map<string,unsigned long long> context,map<string,unsigned long long> *dist1,map<string,unsigned long long> *dist2,map<string,unsigned long long> maxdist)
{	
	maxqms=0;
	for(int i=0;i<keys->size();i++)
	{
		double result = ExploitKey(startTime, f,i,keys,randoms,context,dist1, dist2,maxdist);

		if(result == -1) {
		    return -1;
		}

		if(ComputeQms)
		{
			if(result==Counting::upperbound)
			{
				return Counting::upperbound;
			}else{
				if(maxqms<result)
				{
					maxqms=result;
				}
				
			}
		}else if(not result)
		{
			return 0.0;
		}
		
	}
	if(ComputeQms)
    	{
		return maxqms;
	}else{
        	return 1.0;
	}
}

static double ExploitPlain(clock_t startTime, ExpTree *f,int index,list<string> *plains,list<string> *keys,list<string> *randoms,map<string,unsigned long long> context,map<string,unsigned long long> *dist1, map<string,unsigned long long> *dist2,map<string,unsigned long long> maxdist)
{
	double result;
	list<string>::iterator it=plains->begin();
	for(int i=0;i<index;i++)
	{
		it++;
	}
	string pp=*it;
	maxqms=0.0;
	if(index==0)
	{
		for(int ii=0;ii<domain;ii++)
		{
			context[pp]=ii;
			
			result=ExploitKeys(startTime, f,keys,randoms,context,dist1, dist2,maxdist);
			
			if(ComputeQms)
			{
				if(result==Counting::upperbound)
				{
					return Counting::upperbound;
				}else{
					if(0<result)
					{
						maxqms=result;
					}
				
				}
			}else if(result==0)
			{
				return 0.0;
			}
			dist1->clear();
			dist2->clear();
			maxdist.clear();
		}
		
		context[pp]=0;
	}else{
		for(int i=1;i<domain;i++)
		{
            		context[pp]=i;
			
            		double result = ExploitPlain(startTime,f,index-1,plains,keys,randoms,context,dist1, dist2,maxdist);
            		if(ComputeQms)
			{
				if(result==Counting::upperbound)
				{
					return Counting::upperbound;
				}else{
					if(maxqms<result)
					{
						maxqms=result;
					}
				
				}
			}else if(not result)
			{
				return 0.0;
			}
		}
		context[pp]=0;
	}
	if(ComputeQms)
    	{
		return maxqms;
	}else{
        return 1.0;
	}
}
 
static string checkSID(time_t startTime, ExpTree *f,list<string> *plains,list<string> *keys,list<string> *randoms)
{
	stringstream stream;
	map<string,unsigned long long> con,dist1,dist2,maxdist;
	map<string,unsigned long long>::iterator i,ii;
	list<string>::iterator n;
	for(n=plains->begin();n!=plains->end();n++)
	{
		con[*n]=0;
	}
	for(n=keys->begin();n!=keys->end();n++)
	{
		con[*n]=0;
	}
	for(n=randoms->begin();n!=randoms->end();n++)
	{
		con[*n]=0;
	}
	if(plains->size()==0)
	{
		
		double result=ExploitKeys(startTime,f,keys,randoms,con,&dist1, &dist2,maxdist);

		if(result == -1) {
		    return "UKD";
		}
		
		if(ComputeQms)
		{
			string res;
			stringstream ss;
			ss<<result/Counting::upperbound<<endl;
			ComputeQms = false;
			return ss.str();
		}else if(not result)
		{
			return "SDD";
		}else{
			if(isRUD)
			{
				return "RUD";
			}else{
				return "SID";
			}
		}
	}else{
		maxqms=0.0;
		for(int i=0;i<plains->size();i++)
		{
			double result=ExploitPlain(startTime,f,i,plains,keys,randoms,con,&dist1, &dist2,maxdist);
			if(ComputeQms)
			{
				if(result==Counting::upperbound)
				{
					return "1";
				}else{
					if(maxqms<result)
					{
						maxqms=result;
					}
				
				}
			}else if(not result)
			{
				return "SDD";
			}
		
		}
		if(ComputeQms)
		{
			string s;
			stream<<maxqms/Counting::upperbound;
			stream>>s;
			return s;
		}else{
			if(isRUD)
			{
				return "RUD";
			}else{
				return "SID";
			}
		}
		
		
	
	}
	
	
}

static ExpTree parser(list<string> *s)
{
	stack<ExpTree> ss;
	ExpTree t(""),l(""),r("");
	s->reverse();
	list<string>::iterator it;
	for(it=s->begin();it!=s->end();it++)
	{
		
		if(*it=="("||*it==")"||*it==""||*it==" "||*it=="\n"||*it=="\t")
		{
			continue;
		}else if(*it=="~")
		{
			if(ss.size()<1)
			{
				cout<< "ExpSyntaxError"<<endl;
				exit(0);
			}
			t=ss.top();
			ExpTree expt(*it,new ExpTree(t));
			ss.pop();
			ss.push(expt);
		}else if(*it=="pow2")
		{
			if(ss.size()<1)
			{
				cout<< "ExpSyntaxError"<<endl;
				exit(0);
			}
			t=ss.top();
			ExpTree expt(*it,new ExpTree(t));
			ss.pop();
			ss.push(expt);
		}else if(*it=="pow4")
		{
			if(ss.size()<1)
			{
				cout<< "ExpSyntaxError"<<endl;
				exit(0);
			}
			t=ss.top();
			ExpTree expt(*it,new ExpTree(t));
			ss.pop();
			ss.push(expt);
		}else if(*it=="pow8")
		{
			if(ss.size()<1)
			{
				cout<< "ExpSyntaxError"<<endl;
				exit(0);
			}
			t=ss.top();
			ExpTree expt(*it,new ExpTree(t));
			ss.pop();
			ss.push(expt);
		}else if(*it=="pow16")
		{
			if(ss.size()<1)
			{
				cout<< "ExpSyntaxError"<<endl;
				exit(0);
			}
			t=ss.top();
			ExpTree expt(*it,new ExpTree(t));
			ss.pop();
			ss.push(expt);
		}else if(operators.find(*it)!=string::npos)
		{
			if(ss.size()<2)
			{
				cout<< "ExpSyntaxError"<<endl;
				exit(0);
			}
			l=ss.top();
			ss.pop();
			r=ss.top();
			ss.pop();
			ExpTree expt=ExpTree(*it,new ExpTree(l),new ExpTree(r));
			ss.push(expt);
		}else{
			ExpTree expt=ExpTree(*it);
			ss.push(expt);
			l=ss.top();

		}
	}
	if(ss.size()!=1)
	{
		
		cout<< "ExpSyntaxError"<<endl;
		exit(0);
	}
	t=ss.top();
	return t;
}




static list<string> split(const string& s,list<string> l,const char flag = ' ') {
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) 
    {
        l.push_back(temp);
       
    }
    return l;
}
static string D(time_t startTime, string str1,string oplains,string okeys,string orandoms)
{
	oplains=removeSpace(oplains);
	okeys=removeSpace(okeys);
	orandoms=removeSpace(orandoms);

	list<string> plains;
	plains=split(oplains,plains,',');
	list<string> keys;
	keys=split(okeys,keys,',');

	list<string> randoms;
	randoms=split(orandoms,randoms,',');
	list<string> s;
	s=split(str1,s,' ');

	if(keys.size()==0)
	{
		return "SID";
	}else{
		unsigned long long *uu;
		uu=&Counting::upperbound;
		*uu=pow(domain,randoms.size());
		unsigned long long *ex;
		ex=&expectedProb;
		*ex=pow(domain,randoms.size()-1);
		list<string> *ss;
		ss=&s;
		ExpTree f=parser(ss);
		list<string> *p,*k,*r;
		p=&plains;
		k=&keys;
		r=&randoms;

		clock_t startTime = clock();
		return checkSID(startTime, &f,p,k,r);
	}

}

static double Q(string str1,string oplains,string okeys,string orandoms)
{
	oplains=removeSpace(oplains);
	okeys=removeSpace(okeys);
	orandoms=removeSpace(orandoms);

	list<string> plains;
	plains=split(oplains,plains,',');
	list<string> keys;
	keys=split(okeys,keys,',');

	list<string> randoms;
	randoms=split(orandoms,randoms,',');
	list<string> s;
	s=split(str1,s,' ');

	int *c;
	c=&ComputeQms;
	*c=true;
	
	if(keys.size()==0)
	{
		return 0.0;
	}else{
		unsigned long long *uu;
		uu=&Counting::upperbound;
		*uu=pow(domain,randoms.size());
		unsigned long long *ex;
		ex=&expectedProb;
		*ex=pow(domain,randoms.size()-1);
		list<string> *ss;
		ss=&s;
		ExpTree f=parser(ss);
		list<string> *p,*k,*r;
		p=&plains;
		k=&keys;
		r=&randoms;

        clock_t startTime = clock();
		string res=checkSID(startTime, &f,p,k,r);
		istringstream s(res);
		double buf;
		s>>buf;
		return buf;
	}	

}

};
unsigned long long Counting::upperbound=1;
unsigned long long Counting::expectedProb=1;
double Counting::maxqms=0;
int Counting::bvwidth = ExpTree::bvwidth;
int Counting::domain=pow(2, Counting::bvwidth);
int Counting::ComputeQms=0;
int Counting::isRUD=1;
string Counting::operators="+-*&|~^>><<!";

