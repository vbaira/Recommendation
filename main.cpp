#include "utility.h"
#include "recommendation.h"
#include <stdlib.h>
#include <iostream>
#include <utility>
#include <fstream>

using namespace std;

int main(int argc,char* argv[]){
	/*parse command line options*/
	string input_file,output_file;
	bool validate = false;
	Utility::parse_options(argc,argv,input_file,output_file,validate);
	//cout << input_file+" "+output_file+" "<< validate <<"\n";

	/*read tweets from input and determine score for them*/
	int P=20,dim;
	unordered_map<string,tweet> tweet_scores;
	unordered_map<int,string> inverse_coin_map;
	Utility::read_input_file(input_file,P,dim,tweet_scores,inverse_coin_map);
	//cout << P <<" "<< dim<<"\n";
	//for (auto& x: tweet_scores){ cout<<x.first<<" "; x.second.print(); }
	//for (auto& x:inverse_coin_map) {cout<<x.first<<" : "<<x.second<<"\n";}

	/*create user vectors*/
	vector<data_point> users;
	Utility::create_user_vectors(input_file,dim,tweet_scores,users);
	//for (auto& x: users) x.print(); cout<<users.size()<<"\n";

	/*create virtual user vectors*/
	vector<data_point> virtual_users;
	Utility::create_virtual_user_vectors(dim,tweet_scores,virtual_users);
	//for (auto& x: virtual_users) x.print(); cout<<virtual_users.size()<<"\n";

	/*lsh recommendation using real users*/
	cout << "->LSH recommendation(users)..."<<flush;
	Lsh_recommendation lsh_u(P,10,3,dim,users.size(),users);
	lsh_u.recommend(output_file,users,inverse_coin_map);
	cout << "DONE<-\n";

	/*clustering recommendation using real users*/
	cout << "->Clustering recommendation(users)..."<<flush;
	int setup[3]={2,1,1};
	Clustering_recommendation cl_u(setup,P,4,5,dim,users.size(),users);
	cl_u.recommend(output_file,users,inverse_coin_map);
	cout << "DONE<-\n";

	/*lsh recomendation using virtual users*/
	cout << "->LSH recommendation(virtual users)..."<<flush;
	Lsh_recommendation lsh_vu(P,10,3,dim,virtual_users.size(),virtual_users);
	lsh_vu.recommend(output_file,users,inverse_coin_map);
	cout << "DONE<-\n";

	/*clustering recommendation using virtual users*/
	cout << "->Clustering recommendation(virtual users)..."<<flush;
	Clustering_recommendation cl_vu(setup,P,4,5,dim,virtual_users.size(),virtual_users);
	cl_u.recommend(output_file,users,inverse_coin_map);
	cout << "DONE<-\n";

}



