/*************************************/
/*		File:clustering.h            */
/*                                   */
/* Definition of clustering          */
/*       class                       */
/*                                   */
/*************************************/

#ifndef CLUSTERING_H
#define CLUSTERING_H


#include "../utility.h"
#include "cluster_init.h"
#include "cluster_assignment.h"
#include "cluster_update.h"
#include <vector>


class Clustering{
private:
	typedef double (*func_t)(std::vector<double>& ,std::vector<double>& ); //typedef pointer to function
	
	Initialization* init;
	Assignment* assignment;
	Update* update;
	std::string metric;
	func_t dist_func;

	int* setup;
	int k,lsh_k,lsh_L;
	double clustering_time = 0;
	std::vector<data_point>& train_data; 
	std::vector<data_point*> centroids ;
	std::vector< std::vector<data_point*> > clusters;
	std::vector< std::vector<double> > sil;	//silhouette

	/*non api functions*/
	int second_best_center(data_point* p,int center);
	void silhouette();	
	double objective_function();
public:
	Clustering(int* stp,int k,int lsh_k,int L,std::vector<data_point>& td,std::string metric);
	~Clustering();
	void start();
	std::vector< std::vector<data_point*> > get_clusters();
	std::vector<data_point*> get_centroids(); 
	std::vector<double> average_silhouette();
	void output(std::string out);	
	static double cosine_distance(std::vector<double>& a , std::vector<double>& b);
	static double eucledian_distance( std::vector<double>& a , std::vector<double>& b);

};



#endif