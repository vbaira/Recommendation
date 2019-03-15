/*************************************/
/*		File:utility.cpp             */
/*                                   */
/* Definition of  utility class      */
/*     member functions              */
/*                                   */
/*************************************/

#include "./clustering/clustering.h"
#include "utility.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <vector> 
#include <stdio.h> 
#include <iterator> 
#include <fstream>
#include <sstream>
#include <regex>

#define DICT "./input/vader_lexicon.csv"
#define COIN_DICT "./input/coins_queries.csv"

using namespace std;


/*Parse command line options */
void Utility::parse_options(int argc , char* argv[],string& in,string& out,bool& validate){
	string s="USAGE : $ ./recomendation –d <input file> -o <output file> -v\n";
	if (argc>6){
		cerr << "Wrong number of arguments : "+s ;
		exit(EXIT_FAILURE);
	}
	int opt;
	while ( (opt = getopt(argc,argv,"d:o:v")) != -1 ){
		switch(opt){
			case 'd':
				in = optarg;
				break;
			case 'o':
				out = optarg;
				break;
			case 'v':
				validate = true;
				break;
			default:
				cerr << s ;
				exit(EXIT_FAILURE);	
		}
	}
	if ( in.empty() ){
		cout << "Please enter path for input file\n";
		cin >> in; 
	}
	if ( out.empty() ){
		cout << "Please enter path for output file\n";
		cin >> out;
	}
}


/*helper functions*/
int get_P(string& in);
void create_dictionary(unordered_map<string,float>& dict);
void create_coin_dictionary(unordered_map<string,int>& coin_dict,unordered_map<int,string>& i_coin_map,int& dim);

/*Function to read tweets from input file  and score them */
void Utility::read_input_file(string in,int& P,int& dim,unordered_map<string,tweet>& tweet_scores,unordered_map<int,string>& i_coin_map){
	/*get P*/
	P = get_P(in);
	/*open file*/
	ifstream fs;
	fs.open(in.c_str());
	if ( ! fs.is_open() ){
		cerr << "Unable to open input file\n";
		exit(EXIT_FAILURE);
	} 
	/*create dictionary to map tokens to sentiment score*/
	unordered_map<string,float> dictionary ;
	create_dictionary(dictionary);
	/*create coin dictionary to map coin terms to a numeric value*/
	unordered_map<string,int>  coin_dictionary ;
	create_coin_dictionary(coin_dictionary,i_coin_map,dim);
	/*read file line by line and get tweet tokens*/
	string buf;
	while( getline(fs,buf) ){
		if (buf.empty() || buf.size()==1 || buf[0]=='P'){
			continue; 	//ignore empty lines
		}
		if (buf.back()=='\r') buf.pop_back();
		tweet t;
		stringstream ss(buf);
		string token;
		int token_num=1;
		while (getline(ss,token,'\t')){
			if (token_num == 1){
				t.userID=token;
			}
			else if (token_num == 2){
				t.tweetID=token;
			}
			else{
				/*lookup token in dictionary and add to totalscore if found*/
				auto dict_iter = dictionary.find(token);
				if (dict_iter != dictionary.end()){
					//cout <<token+" found, with score "<< dict_iter->second << "\n";
					t.totalscore += dict_iter->second;
					continue; 
				}
				/*lookup token in coin dictionary*/
				auto c_dict_iter = coin_dictionary.find(token);
				if (c_dict_iter != coin_dictionary.end()){
					//cout <<token+" found, with index "<< c_dict_iter->second << "\n";
					t.reffered_coins.insert(c_dict_iter->second);
				}
			}
			token_num++;
		}
		/*calculate si*/
		t.si=( t.totalscore/sqrt( pow(t.totalscore,2) + 15 ) );
		//t.print();
		tweet_scores[t.tweetID]=t;
	}
	fs.close();		
}


/*Function to read preprocessed tweets from input file  and place them in list */
void Utility::read_input_file(vector<data_point>& l){
	string in;
	cout << "Please enter path for clustering input file(preprocessed tweets)\n";
	cin >> in;
	ifstream fs;
	fs.open(in.c_str());
	if ( ! fs.is_open() ){
		cerr << "Unable to open input file\n";
		exit(EXIT_FAILURE);
	} 
	/*tokenize input vectors by using a regular expression iterator*/
	regex delim("[^\\s,]+");
	/*read file line by line and insert data*/
	string buf;
	while( getline(fs,buf) ){
		if (buf.empty()){
			continue; 	//ignore empty lines
		}
		data_point dp;
		sregex_iterator start(buf.begin(),buf.end(),delim);
		sregex_iterator end;
		for (sregex_iterator i=start; i!=end; i++){
			if (i==start){
				dp.id=(*i).str();
			}
			else{
				dp.p.push_back( stod((*i).str()) );
			}
		}
		l.push_back(dp);
	}
	fs.close();		
}


/*Create user vectors by taking each user's tweets
 and adding their scores to the reffered coins*/
void Utility::create_user_vectors(string in,int dim,unordered_map<string,tweet>& tweet_scores,vector<data_point>& uv){
	cout<<"->Creating users...";
	/*open file*/
	ifstream fs;
	fs.open(in.c_str());
	if ( ! fs.is_open() ){
		cerr << "Unable to open input file\n";
		exit(EXIT_FAILURE);
	}
	/*read file line by line*/
	string buf,prev_userID="";
	while( getline(fs,buf) ){
		if (buf.empty() || buf.size()==1 || buf[0]=='P'){
			continue; 	//ignore empty lines
		}
		if (buf.back()=='\r') buf.pop_back();
		stringstream ss(buf);
		string userID,tweetID;					
		getline(ss,userID,'\t');				//get userID
		getline(ss,tweetID,'\t');				//get tweetID
		tweet t=tweet_scores[tweetID];			//get the tweet
		/*add new user or edit his vector if already added*/
		if (userID != prev_userID){				
			/*remove last user if agnostic*/
			if ( !uv.empty()) {
				uv.back().calculate_mean();
				if ( uv.back().is_agnostic() ) uv.pop_back();
			}
			data_point dp;
			dp.id = userID;
			dp.p.resize(dim,numeric_limits<double>::infinity());
			for (auto t_idx : t.reffered_coins){
				dp.p[t_idx] = t.si;
			}
			uv.push_back(dp);
		}
		else{									//edit current user's vector
			data_point& dp = uv.back();
			for (auto t_idx : t.reffered_coins){
				if ( isinf(dp.p[t_idx]) ){
					dp.p[t_idx] = t.si;
				}
				else{
					dp.p[t_idx] += t.si;
				}
			}
		}
		prev_userID = userID;
	}
	/*remove last user if agnostic*/
	uv.back().calculate_mean();
	if ( uv.back().is_agnostic() ) uv.pop_back();
	cout<<"DONE<-\n";
}


/*Create virtual user vectors by clustering tweets
and using each cluster's tweets as the virtual user's tweets*/
void Utility::create_virtual_user_vectors(int dim,unordered_map<string,tweet>& tweet_scores,vector<data_point>& v_uv){
	/*cluster preprocessed tweets(like in 2nd project)*/
	vector<data_point> train_set;
	Utility::read_input_file(train_set);
	cout<<"->Creating virtual users(via clustering)..."<<flush;
	int setup[3]={2,1,1};
	int cluster_count=350,k=5,L=5;
	Clustering clustering(setup,cluster_count,k,L,train_set,"cosine");
	clustering.start();
	vector< vector<data_point*> > clusters=clustering.get_clusters();
	/*for each cluster create a virtual user,using cluster's tweets*/
	for (int cl=0; cl < clusters.size(); cl++){
		data_point dp;
		string userID,tweetID;
		userID = "v"+to_string(cl);
		dp.id = userID;
		dp.p.resize(dim,numeric_limits<double>::infinity());
		for (int elem=0; elem < clusters[cl].size(); elem++){
			tweetID = clusters[cl][elem]->id;
			tweet t = tweet_scores[tweetID];
			for (auto t_idx : t.reffered_coins){
				if ( isinf(dp.p[t_idx]) ){
					dp.p[t_idx] = t.si;
				}
				else{
					dp.p[t_idx] += t.si;
				}
			}
		}
		/*calculate mean for virtual user vector and add it*/
		dp.calculate_mean();
		if ( ! dp.is_agnostic() ){
			v_uv.push_back(dp);
		}
	}
	cout<<"DONE<-\n";
}

/************************************************
			HELPER FUNCTIONS
*************************************************/

/*extract P from input file*/
int get_P(string& in){
	ifstream fs;
	fs.open(in.c_str());
	if ( ! fs.is_open() ){
		cerr << "Unable to open input file\n";
		exit(EXIT_FAILURE);
	} 
	string buf;
	/*go to first non empty line*/
	getline(fs,buf);
	while (buf.empty() || buf.size()==1){
		getline(fs,buf);
	}
	int P;
	if (buf[0]=='P'){
		stringstream ss(buf);
		string temp;
		getline(ss,temp,':');
		getline(ss,temp);
		P=stoi(temp);
	}
	else{
		P=20;
	}
	fs.close();
	return P;
}


/*Create a mapping between coin terms from COIN_DICT and and their respective numerical value.
Also create and inverse coin map(maps indexes to coins)*/
void create_coin_dictionary(unordered_map<string,int>& coin_dict,unordered_map<int,string>& i_coin_map,int& dim){
	/*open file*/
	ifstream fs;
	fs.open(COIN_DICT);
	if ( ! fs.is_open() ){
		cerr << "Unable to open coin dictionary\n";
		exit(EXIT_FAILURE);
	}
	/*read file line by line and insert data*/
	string buf;
	int numerical_value=0;
	while( getline(fs,buf) ){
		if (buf.empty() || buf.size()==1 ){
			continue; 	//ignore empty lines
		}
		if (buf.back()=='\r') buf.pop_back();
		stringstream ss(buf);
		string term;
		int rep=0;
		while (getline(ss,term,'\t')){
			if (rep==0) i_coin_map[numerical_value]=term;
			coin_dict[term]=numerical_value;
			rep++;
		}
		numerical_value++;
	}
	//for (auto& x: coin_dict) cout << x.first << " : " << x.second << "\n"; cout << coin_dict.size()<<"\n";
  	/*close file*/
	dim = numerical_value;	
	fs.close();
}


/*create a mapping between tokens and sentiment scores from DICT*/
void create_dictionary(unordered_map<string,float>& dict){
	/*open file*/
	ifstream fs;
	fs.open(DICT);
	if ( ! fs.is_open() ){
		cerr << "Unable to open dictionary\n";
		exit(EXIT_FAILURE);
	}
	/*tokenize input from file*/
	regex delim("[^\\t]+");
	/*read file line by line and insert data*/
	string buf;
	while( getline(fs,buf) ){
		if (buf.empty() || buf.size()==1){
			continue; 	//ignore empty lines
		}
		if (buf.back()=='\r') buf.pop_back();
		sregex_iterator start(buf.begin(),buf.end(),delim);
		sregex_iterator end;
		string token;
		float sentiment;
		for (sregex_iterator i=start; i!=end; i++){
			if (i==start) token = (*i).str();
			else 		  sentiment = stod((*i).str());
		}
		/*insert into dictionary*/
		dict[token]=sentiment;
	}
	//for (auto& x: dict) cout << x.first << ":" << x.second << "\n"; cout << dict.size()<<"\n";
	/*close file*/
	fs.close();
}


/*calculate mean of vector and replace unknowns with it*/
void data_point::calculate_mean(){
	double sum=0;
	int known_count=0,idx=0;
	for (auto& x : p){
		if ( std::isinf(x) ) { unkown_coins.push_back(idx); }
		else				 { sum+=x; known_count++; }
		idx++;
	}
	if (known_count==0) mean=std::numeric_limits<double>::infinity();
	else 				mean = sum / known_count;
	for (auto& x: p){
		if ( std::isinf(x) ) x = mean;
	}
}


/*determine if user is  agnostic*/
bool data_point::is_agnostic(){
	if ( std::isinf(mean) ){
		return true;
	}
	else{
		bool agnostic=true;
		for(auto& x : p){
			if (x != 0 ){
				agnostic=false;
				break;
			} 	 
		}
		return agnostic;
	}
}
