/***************************************/
/*		File:cluster_assignment.cpp    */
/*                                     */
/*Definition of  cluster assignment    */
/*	classes' ,member functions         */
/*                                     */
/***************************************/

#include "cluster_assignment.h"
#include <vector>
#include <iostream>
//#include <algorithm>
//#include <cmath>


using namespace std;


/****************************************
Return minimum distance between centroids
*****************************************/
double Assignment ::centroid_min_dist(vector<data_point*>& centroids){
	double min=MAX_D;
	for (int c=0; c<k-1; c++){
		for(int i=c+1; i<k; i++){
			double dist = dist_func(centroids[c]->p,centroids[i]->p);
			if (dist == 0 ) continue;
			if (dist < min){
				min = dist;
			}
		}
	}
	return min;
}


/******************************************
Get centroids vector and assign each point
to its nearest center to form the clusters.
Centers are also included in clusters
*******************************************/
void Assignment_lloyd::get_clusters( vector<data_point*>& centroids , vector< vector<data_point*> >& clusters ){
	int train_set_sz = train_data.size();
	for( int i=0; i<train_set_sz; i++ ){	
		double min_dist = MAX_D;
		int nearest_centroid;

		if ( train_data[i].is_centroid == true ){
			for (int c=0; c<k; c++){
				if( train_data[i].id == centroids[c]->id ){
					nearest_centroid = c;
					min_dist = 0;
					break;
				}
			}
		}
		else{
			/*find nearest centroid*/
			for (int c=0; c<k; c++){
				double dist = dist_func( train_data[i].p , centroids[c]->p );
				if (dist < min_dist){
					min_dist = dist;
					nearest_centroid = c;	
				}
			}
		}
		/*assign point to its cluster*/
		clusters[nearest_centroid].push_back(&train_data[i]);
	}
}



/****************************************
Constructor
*****************************************/
Assignment_lsh :: Assignment_lsh(int cluster_num,vector<data_point>& td,func_t dist,string metric,int lsh_k,int lsh_L) : Assignment(cluster_num,td,dist),metric(metric),lsh_k(lsh_k),lsh_L(lsh_L){
	int dimension = train_data[0].p.size();
	int train_set_sz = train_data.size(); 
	if (metric=="eucledian"){
		lsh=new LSH_eucledian(lsh_k,lsh_L,dimension,train_set_sz,td);	
	}
	else{
		lsh=new LSH_cosine(lsh_k,lsh_L,dimension,train_set_sz,td) ;
	}
}



/********************************************
Destructor.Deallocate allocated data
********************************************/
Assignment_lsh :: ~Assignment_lsh(){
	delete lsh;
}


#define MAX_REP 5

/***********************************
Assignment by lsh range search
************************************/
void  Assignment_lsh :: get_clusters( vector<data_point*>& centroids , vector< vector<data_point*> >& clusters ){
	/*reset data and assign each centroid to its own cluster*/
	for (int i=0; i<train_data.size(); i++){
		train_data[i].is_assigned=false;
		train_data[i].contenders.clear();
		if ( train_data[i].is_centroid == true ){
			for (int c=0; c<k; c++){
				if( train_data[i].id == centroids[c]->id ){
					clusters[c].push_back(&train_data[i]);
					train_data[i].is_assigned=true;
					break;
				}
			}
		}
	}
	/*find min dist between centroids to calculate radius*/
	double centroid_min = centroid_min_dist(centroids);
	double radius = (centroid_min / 4);
	int r=0;
	do{
		for (int c=0; c<k; c++){
			/*preform range search for centroid */
			vector<data_point*> within_range;
			auto temp = lsh->approximateNN(*centroids[c],radius,within_range);
			/*for points within range set centroid as a contender*/
			for (int i=0; i<within_range.size(); i++){
				if( (within_range[i]->is_assigned) == true ){
					continue;
				}
				else{
					(within_range[i]->contenders).push_back(c);
				}
			}
		}
		/*assign points to respective cluster.If more than 1 centroid contenders(collision) assign to closest one*/
		for (int i=0; i<train_data.size(); i++){
			if ( (train_data[i].is_assigned == false) && (train_data[i].contenders.size() > 0) ){
				//cout << "collisions:" << train_data[i].contenders.size() <<"\n";
				double min_dist=MAX_D;
				int nearest_centroid;
				for (int x : train_data[i].contenders){
					double dist =  dist_func(train_data[i].p , centroids[x]->p);
					if (dist < min_dist){
						min_dist = dist;
						nearest_centroid = x;
					}
				}
				clusters[nearest_centroid].push_back(&train_data[i]);
				train_data[i].is_assigned = true;
			}
		}
		/*increase radius*/
		radius =radius*2;

		r++;
	}while(r < MAX_REP);//
	/*assign rest points with lloyd's*/
	int unassigned=0;
	for (int i=0; i<train_data.size(); i++){
		if(train_data[i].is_assigned == false){
			unassigned++;
			double min_dist = MAX_D;
			int nearest_centroid;
			for (int c=0; c<k; c++){
				double dist = dist_func( train_data[i].p , centroids[c]->p );
				if (dist < min_dist){
					min_dist = dist;
					nearest_centroid = c;	
				}
			}
			clusters[nearest_centroid].push_back(&train_data[i]);
		}
	}
	//cout <<"Unassigned points that have to be assigned with lloyd's:" <<unassigned <<"\n";
}




/****************************************
Constructor
*****************************************/
Assignment_hypercube :: Assignment_hypercube(int cluster_num,vector<data_point>& td,func_t dist,string metric,int hcube_k) : Assignment(cluster_num,td,dist),metric(metric),hcube_k(hcube_k){
	int dimension = train_data[0].p.size();
	int train_set_sz = train_data.size();
	int M=1000;
	int probes = 10; 
	if (metric=="eucledian"){
		hypercube=new Hypercube_eucledian(train_set_sz,td,hcube_k,M,probes,dimension);
	}
	else{
		hypercube=new Hypercube_cosine(train_set_sz,td,hcube_k,M,probes,dimension);
	}

}
	

/********************************************
Destructor.Deallocate allocated data
********************************************/
Assignment_hypercube ::	~Assignment_hypercube(){
	delete hypercube;	
}


/***********************************
Assignment by hypercube range search
************************************/
void  Assignment_hypercube :: get_clusters( vector<data_point*>& centroids , vector< vector<data_point*> >& clusters ){
	/*reset data and assign each centroid to its own cluster*/
	for (int i=0; i<train_data.size(); i++){
		train_data[i].is_assigned=false;
		train_data[i].contenders.clear();
		if ( train_data[i].is_centroid == true ){
			for (int c=0; c<k; c++){
				if( train_data[i].id == centroids[c]->id ){
					clusters[c].push_back(&train_data[i]);
					train_data[i].is_assigned=true;
					break;
				}
			}
		}
	}
	/*find min dist between centroids to calculate radius*/
	double centroid_min = centroid_min_dist(centroids);
	double radius = (centroid_min / 4);
	int r=0;
	do{
		for (int c=0; c<k; c++){
			/*preform range search for centroid */
			vector<data_point*> within_range;
			auto temp = hypercube->approximateNN(*centroids[c],radius,within_range);
			/*for points within range set centroid as a contender*/
			for (int i=0; i<within_range.size(); i++){
				if( (within_range[i]->is_assigned) == true ){
					continue;
				}
				else{
					(within_range[i]->contenders).push_back(c);
				}
			}
		}
		/*assign points to respective cluster.If more than 1 centroid contenders(collision) assign to closest one*/
		for (int i=0; i<train_data.size(); i++){
			if ( (train_data[i].is_assigned == false) && (train_data[i].contenders.size() > 0) ){
				//cout << "collisions:" << train_data[i].contenders.size() <<"\n";
				double min_dist=MAX_D;
				int nearest_centroid;
				for (int x : train_data[i].contenders){
					double dist =  dist_func(train_data[i].p , centroids[x]->p);
					if (dist < min_dist){
						min_dist = dist;
						nearest_centroid = x;
					}
				}
				clusters[nearest_centroid].push_back(&train_data[i]);
				train_data[i].is_assigned = true;
			}
		}
		/*increase radius*/
		radius =radius*2;

		r++;
	}while(r < MAX_REP);//
	/*assign rest points with lloyd's*/
	int unassigned=0;
	for (int i=0; i<train_data.size(); i++){
		if(train_data[i].is_assigned == false){
			unassigned++;
			double min_dist = MAX_D;
			int nearest_centroid;
			for (int c=0; c<k; c++){
				double dist = dist_func( train_data[i].p , centroids[c]->p );
				if (dist < min_dist){
					min_dist = dist;
					nearest_centroid = c;	
				}
			}
			clusters[nearest_centroid].push_back(&train_data[i]);
		}
	}
	//cout <<"Unassigned points that have to be assigned with lloyd's:" <<unassigned <<"\n";

}