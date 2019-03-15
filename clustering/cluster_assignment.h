/*************************************/
/*		File:cluster_assignment.h    */
/*                                   */
/* Definition of cluster             */
/*	assignment classes               */
/*                                   */
/*************************************/


#ifndef CLUSTER_ASSGN_H
#define CLUSTER_ASSGN_H


#include "../utility.h"
#include "lsh_hypercube/lsh.h"
#include "lsh_hypercube/cube.h"
#include <vector>


/*****************************************************
Abstract class for assignment of points to a cluster .
Its purpose is to be inherited from classes 
which define a specific assignment method.
******************************************************/
class Assignment{
protected:
	typedef double (* func_t)(std::vector<double>& ,std::vector<double>& ); //typedef pointer to function
	
	int k;
	std::vector<data_point>& train_data;
	func_t dist_func; 	//pointer to distance function

	double centroid_min_dist(std::vector<data_point*>& centroids);
public:
	Assignment(int cluster_num,std::vector<data_point>& td,func_t dist) : k(cluster_num),train_data(td),dist_func(dist){} 
	virtual ~Assignment(){}
	virtual void get_clusters( std::vector<data_point*>& centroids , std::vector< std::vector<data_point*> >& clusters )=0; 
};




/***********************************************
Class implementing the assignment portion
of lloyd's algorithm
***********************************************/
class Assignment_lloyd : public Assignment{
public:
	Assignment_lloyd(int cluster_num,std::vector<data_point>& td,func_t dist) : Assignment(cluster_num,td,dist){}
	void get_clusters( std::vector<data_point*>& centroids , std::vector< std::vector<data_point*> >& clusters );
};






class Assignment_lsh : public Assignment{
private:
	std::string metric;
	int lsh_k;
	int lsh_L;
	LSH* lsh;

public:
	Assignment_lsh(int cluster_num,std::vector<data_point>& td,func_t dist,std::string metric,int lsh_k,int lsh_L);
	~Assignment_lsh();
	void get_clusters( std::vector<data_point*>& centroids , std::vector< std::vector<data_point*> >& clusters );
};




class Assignment_hypercube : public Assignment{
private:
	std::string metric;
	int hcube_k;
	Hypercube* hypercube;

public:
	Assignment_hypercube(int cluster_num,std::vector<data_point>& td,func_t dist,std::string metric,int hcube_k);
	~Assignment_hypercube();
	void get_clusters( std::vector<data_point*>& centroids , std::vector< std::vector<data_point*> >& clusters );
};




#endif
