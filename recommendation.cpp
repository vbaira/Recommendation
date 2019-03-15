/***************************************/
/*		File:recommendation.cpp        */
/*                                     */
/*Definition of  recommendation        */
/*	classes' ,member functions         */
/*                                     */
/***************************************/

#include "recommendation.h"
#include <algorithm>
#include <fstream>
#include <chrono>

using namespace std;


/***************************************
For each item in queries recommend 5 items 
using lsh and write result in output
***************************************/
void Lsh_recommendation::recommend(string output,vector<data_point>& queries,unordered_map<int,string> inverse_value_map){
	/**open output filestream**/
	ofstream out(output,ios_base::app);
	if ( ! out.is_open() ){
		cerr << "Unable to open output file\n";
		exit(EXIT_FAILURE);
	}
	out << "Cosine LSH\n"; 
	/*for each query,calculate reccomendations accordind to P closest neighbors*/
	auto start = chrono::high_resolution_clock::now();
	for (auto& user:queries){
		vector<pair<double,data_point*>> neighbors=lsh.n_NN(P,user);	//get P closest neighbors
		vector<pair<double,int>> calculated_ratings;
		/*for each unkown item derive a rating from neighbors*/
		for (auto& unkown_item : user.unkown_coins ){
			double sum=0,z_sum=0;
			for (auto& neighbor : neighbors){
				double similarity=(1 - neighbor.first); 				//cosine_dist=1-cosine_sim <==> cosine_sim=1-cosine_dist
				double Ri = (neighbor.second)->p[unkown_item];
				double R  = (neighbor.second)->mean;
				sum += ( similarity*(Ri-R) );
				z_sum+=abs(similarity);
			}
			double z = 1/z_sum;
			double rating = user.mean + z*sum;
			/*keep item's calculated rating and index in a pair*/
			calculated_ratings.push_back(make_pair(rating,unkown_item));
		}
		/*sort rating-index pairs accordind to rating*/
		sort(calculated_ratings.begin(),calculated_ratings.end());
		out << user.id+" ";
		/*present 5 recommendations with the biggest rating*/
		for(int i=0; i<5; i++){
			int recommendedation_index=calculated_ratings.back().second;
			string recommendedation = inverse_value_map[recommendedation_index];
			calculated_ratings.pop_back();
			out << recommendedation+" ";
		}
		out<<"\n";
	}
	auto stop	= chrono::high_resolution_clock::now();
	auto t = chrono::duration_cast<chrono::milliseconds>(stop - start);
	out << "Execution Time: "<< t.count() <<" ms\n\n";
	/*close file*/
	out.close();
}


/***************************************
For each item in queries recommend 2 items 
using clustering and write result in output
***************************************/
void Clustering_recommendation::recommend(std::string output,std::vector<data_point>& queries,std::unordered_map<int,std::string> inverse_value_map){
	/**open output filestream**/
	ofstream out(output,ios_base::app);
	if ( ! out.is_open() ){
		cerr << "Unable to open output file\n";
		exit(EXIT_FAILURE);
	}
	out << "Clustering\n"; 
	clustering.start();
	vector< vector<data_point*> > clusters=clustering.get_clusters();
	vector<data_point*> cluster_centers=clustering.get_centroids();
	/*for each query,calculate reccomendations accordind to query's closest cluster*/
	auto start = chrono::high_resolution_clock::now();
	for (auto& user:queries){
		double min_dist = MAX_D;
		int closest_center;
		/*find closest cluster center*/
		for( int i=0; i<cluster_centers.size(); i++ ){
			double dist = clustering.eucledian_distance(user.p,cluster_centers[i]->p);
			if ( dist < min_dist){
				min_dist = dist;
				closest_center = i;
			}
		}
		/*for each unkown item derive a rating from cluster's points*/
		vector<pair<double,int>> calculated_ratings;
		for (auto& unkown_item : user.unkown_coins ){
			double sum=0,z_sum=0;
			for (auto& neighbor : clusters[closest_center]){
				double similarity = sim(user.p,neighbor->p);
				double Ri = neighbor->p[unkown_item];
				double R  = neighbor->mean;
				sum += ( similarity*(Ri-R) );
				z_sum+=abs(similarity);
			}
			double z = 1/z_sum;
			double rating = user.mean + z*sum;
			/*keep item's calculated rating and index in a pair*/
			calculated_ratings.push_back(make_pair(rating,unkown_item));
		}
		/*sort rating-index pairs accordind to rating*/
		sort(calculated_ratings.begin(),calculated_ratings.end());
		out << user.id+" ";
		/*present 2 recommendations with the biggest rating*/
		for(int i=0; i<2; i++){
			int recommendedation_index=calculated_ratings.back().second;
			string recommendedation = inverse_value_map[recommendedation_index];
			calculated_ratings.pop_back();
			out << recommendedation+" ";
		}
		out<<"\n";
	}
	auto stop	= chrono::high_resolution_clock::now();
	auto t = chrono::duration_cast<chrono::milliseconds>(stop - start);
	out << "Execution Time: "<< t.count() <<" ms\n\n";
	/*close file*/
	out.close();
}


/*Cosine similarity*/
double Recommendation::sim(vector<double>& u , vector<double>& v){
	double inner_prod = inner_product(u,v);
	int dim=u.size();
	double tmp1=0,tmp2=0;
	for (int i=0; i<dim; i++){
		tmp1 += pow(u[i],2);
		tmp2 += pow(v[i],2);
	}
	double t=sqrt(tmp1) * sqrt(tmp2); 
	double sim=inner_prod/t;
	return sim;
}



