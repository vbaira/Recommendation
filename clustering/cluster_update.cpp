/***************************************/
/*		File:cluster_update.cpp        */
/*                                     */
/*Definition of  cluster update        */
/*	classes' ,member functions         */
/*                                     */
/***************************************/


#include "cluster_update.h"
#include <vector>
#include <iostream>
#include <algorithm>


using namespace std;


/*******************************************
Calculate the mean of each cluster and make
this mean the new cecter
*******************************************/
void Update_kmeans :: update_centers(vector<data_point*>& centroids , vector< vector<data_point*> >& clusters){
	for (int c=0; c<k; c++){
		if (clusters[c].size()==0) continue;
		/*manage previous centroid(existing point or made up )*/
		if ( (centroids[c]->id) == "-1" ){
			delete centroids[c];
		}
		else{
			centroids[c]->is_centroid=false;
		}
		centroids[c] = new data_point;
		centroids[c]->id= "-1";
		centroids[c]->is_centroid=true;
		for ( int t=0; t<clusters[c].size(); t++){
			if (t==0){
				(centroids[c]->p)=(clusters[c][t]->p);
			}
			else{
				/*add the two vectors element by element*/
				transform((centroids[c]->p).begin(),(centroids[c]->p).end(),(clusters[c][t]->p).begin(),(centroids[c]->p).begin(), plus<double>() );
			}
		}
		/*divide each element(dimension) of the vector with  cluster size to get the mean*/
		for (auto &x : (centroids[c]->p)){
			x = x/clusters[c].size();
		}
	} 
}



/***********************************************
For each cluster choose as the new center,the point of it,
which minimizes the sum of distances between 
the said point and all the other cluster points
***********************************************/
void Update_ipam :: update_centers(vector<data_point*>& centroids , vector< vector<data_point*> >& clusters){
	for (int c=0; c<k; c++){
		if (clusters[c].size()==0) continue;
		centroids[c]->is_centroid = false;
		data_point* medoid;
		double min_sum = MAX_D;
		for(int t=0; t<clusters[c].size(); t++){
			double sum=0;
			/*calculate sum of distances between the selected point and other cluster points*/
			for (int i=0; i<clusters[c].size(); i++){
				if ( i==t ) continue;
				sum += dist_func(clusters[c][t]->p, clusters[c][i]->p);
			}
			/*keep minimum sum*/
			if (sum < min_sum){
				min_sum = sum;
				medoid = clusters[c][t];
			}
		}
		/*new centroid is the point with the min sum*/
		centroids[c] = medoid;
		centroids[c]->is_centroid = true;
	}
}