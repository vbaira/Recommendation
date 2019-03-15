/*************************************/
/*     File:utility.h                */
/*	                                 */
/*    Definition of utility class    */
/*                                   */
/*************************************/

#ifndef USAGE_H
#define USAGE_H

#include <string>
#include <vector>
#include <iostream>
#include <limits>
#include <cmath>
#include <set>
#include <unordered_map>


#define MAX_D  numeric_limits<double>::max()
#define MIN_D  numeric_limits<double>::min()


/***********************
Structure to represent
input data points
************************/
struct data_point{
	std::string id;
	std::vector<double> p;

	/*recommendation data*/
	double mean;
	std::vector<int> unkown_coins;

	/*clustering data*/
	bool is_centroid=false;
	bool is_assigned=false;
	std::vector<int> contenders;

	/*print function*/
	void print(){
		//std::cout.precision(std::numeric_limits<double>::digits);
		std::cout << id << "\n";
		for (auto i=p.begin(); i!=p.end(); i++){
			std::cout << *i << " ";
		}
		std::cout << "\n";
		std::cout << "mean:" <<mean<< "\n";
		//for (auto& x:unkown_coins) {std::cout<<x<<" ";} std::cout<<"\n";
	}

	bool is_agnostic();
	void calculate_mean();
};



/******************************
Structure to represent 
tweets
******************************/
struct tweet{
	std::string tweetID;
	std::string userID;
	std::set<int> reffered_coins;
	float totalscore=0;
	double si=0;

	void print(){
		//std::cout.precision(std::numeric_limits<double>::digits);
		std::cout <<"uID:"<< userID+"  tID:"+tweetID+" " << totalscore << " " << si << " [ " ;
		for (auto x:reffered_coins){
			std::cout << x <<" ";
		}
		std::cout<<"]\n";
	}
};



/********************************************
Utility class. 
Consists of static member functions for various 
utility tasks like i/o and data pre processing.
*********************************************/
class Utility{
public:
	static void parse_options(int argc , char* argv[],std::string& in,std::string& out,bool& validate);

	static void read_input_file(std::string in,int& P,int& dim,std::unordered_map<std::string,tweet>& tweet_scores,std::unordered_map<int,std::string>& i_coin_map);

	static void read_input_file(std::vector<data_point>& l);

	static void create_user_vectors(std::string in,int dim,std::unordered_map<std::string,tweet>& tweet_scores,std::vector<data_point>& uv);

	static void create_virtual_user_vectors(int dim,std::unordered_map<std::string,tweet>& tweet_scores,std::vector<data_point>& v_uv);
};


/****************************************
Calculate the dot product of two vectors
*****************************************/
template <typename T,typename V>
T inner_product(std::vector<T>& a, std::vector<V>& b){ 
	if (a.size() != b.size()){
		std::cerr << "Inner product:vectors dont have the same dimension\n";
		exit(EXIT_FAILURE);
	}

    T product = 0;  
    for (int i = 0; i < a.size(); i++){
		product += (a[i] * b[i]); 
    }

    return product; 
}


#endif