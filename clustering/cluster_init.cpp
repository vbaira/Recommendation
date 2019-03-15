/***************************************/
/*		File:cluster_init.cpp          */
/*                                     */
/*Definition of  cluster initialization*/
/*	classes' ,member functions          */
/*                                     */
/***************************************/


#include "cluster_init.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>



using namespace std;


/***************************************************
Choose k randomly selected points as initial cluster centers
and fill centroids vector with them
****************************************************/
void Initialization_random::get_centers(vector<data_point*>& centroids){
	int train_set_sz = train_data.size();
	random_device rd; 			//random device to generate random seed
	mt19937 generator( rd() );	//random generator
	uniform_int_distribution<int> uniform_distr(0,train_set_sz); 	//uniform distr for in range[0,train_set_sz) 
	for (int i=0; i<k; i++){
		/*choose a random point*/
		int random_index;
		/*make sure that a point is not reselected*/
		do{
			random_index = uniform_distr(generator);
		}while (train_data[random_index].is_centroid == true);
		centroids[i] = &train_data[random_index];
		train_data[random_index].is_centroid=true;
		
		//train_data[random_index].print();
	}
}



/***************************************************
Choose k points as initial cluster centers 
using the kmeans++ method,and fill centroids vector with them
****************************************************/
void Initialization_kmeanspp::get_centers(vector<data_point*>& centroids){
	int train_set_sz = train_data.size();
	random_device rd; 			//random device to generate random seed
	mt19937 generator( rd() );	//random generator
	uniform_int_distribution<int> uniform_i_distr(0,train_set_sz); 	//uniform distr for in range[0,train_set_sz) 

	/*choose first centroid uniformly at random*/
	int random_index = uniform_i_distr(generator);
	centroids[0] = &train_data[random_index];
	train_data[random_index].is_centroid = true;
	vector<double> min_dist(train_set_sz,MAX_D);

	/*choose the rest centroids according to kmeans++*/
	for (int t=1; t<k; t++){
		vector<pair<int,double>> partial_sum;	//vector of (point's index,point's partial sum)
		partial_sum.push_back(pair<int,double>(-1,0));
		/*for every non centroid point ,find min dist from chosen centroids and add to partial sum*/
		for(int i=0; i<train_set_sz; i++){
			if ( train_data[i].is_centroid == true ){
				continue; //ignore already chosen points
			}
			double dist = dist_func( train_data[i].p , centroids[t-1]->p );
			if (dist < min_dist[i]){
				min_dist[i]=dist;
			}
			pair<int,double> p(i, partial_sum.back().second + pow(min_dist[i],2) ) ;
			partial_sum.push_back( p );		
		}

		/*pick a number x uniformly at random between 0 and full sum of squared distances*/
		uniform_real_distribution<double> uniform_r_distr(0, partial_sum.back().second );
		double x = uniform_r_distr(generator);
		/*chose next centroid based on x's projection on [0 , Sum(d(i))^2] range */
		for (int i=1; i<partial_sum.size(); i++){
			if ( x <= partial_sum[i].second ){
				int point_index= partial_sum[i].first;
				centroids[t] = &train_data[point_index];
				train_data[point_index].is_centroid=true;
				break;
			}
		}
	}
}

