/*************************************/
/*		File:cluster_update.h        */
/*                                   */
/* Definition of cluster             */
/*	update classes                   */
/*                                   */
/*************************************/


#ifndef CLUSTER_UPDT_H
#define CLUSTER_UPDT_H


#include "../utility.h"
#include <vector>


/*****************************************************
Abstract class for cluster center update.
Its purpose is to be inherited from classes 
which define a specific center update method.
******************************************************/
class Update{
protected:
	typedef double (* func_t)(std::vector<double>& ,std::vector<double>& ); //typedef pointer to function
	
	int k;
	std::vector<data_point>& train_data;
	func_t dist_func; 	//pointer to distance function
public:
	Update(int cluster_num,std::vector<data_point>& td,func_t dist) : k(cluster_num),train_data(td),dist_func(dist){} 
	virtual ~Update(){}
	virtual void update_centers(std::vector<data_point*>& centroids , std::vector< std::vector<data_point*> >& clusters)=0;
};



/***********************************************
Class implementing the center update of 
lloyd's algorith (k-means)
***********************************************/
class Update_kmeans : public Update {
public:
	Update_kmeans(int cluster_num,std::vector<data_point>& td,func_t dist) : Update(cluster_num,td,dist){}
	void update_centers(std::vector<data_point*>& centroids , std::vector< std::vector<data_point*> >& clusters);
};


/***********************************************
Class implementing the improvedPam(a la lloyd's)
center update 
***********************************************/
class Update_ipam : public Update {
public:
	Update_ipam(int cluster_num,std::vector<data_point>& td,func_t dist) : Update(cluster_num,td,dist){}
	void update_centers(std::vector<data_point*>& centroids , std::vector< std::vector<data_point*> >& clusters);
};




#endif