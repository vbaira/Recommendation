/*************************************/
/*		File:cluster_init.h          */
/*                                   */
/* Definition of cluster             */
/*	initialization classes           */
/*                                   */
/*************************************/


#ifndef CLUSTER_INIT_H
#define CLUSTER_INIT_H


#include "../utility.h"
#include <vector>


/*****************************************************
Abstract class for cluster center initialization.
Its purpose is to be inherited from classes 
which define a specific initialization method.
******************************************************/
class Initialization{
protected:
	typedef double (* func_t)(std::vector<double>& ,std::vector<double>& ); //typedef pointer to function

	int k;
	std::vector<data_point>& train_data;
	func_t dist_func; 	//pointer to distance function

public:
	Initialization(int cluster_num,std::vector<data_point>& td,func_t dist) : k(cluster_num),train_data(td),dist_func(dist){} 
	virtual ~Initialization(){}
	virtual void get_centers(std::vector<data_point*>& cluster_centers)=0;
};



/***********************************************
Random cluster initialization,class
***********************************************/
class Initialization_random : public Initialization{
public:
	Initialization_random(int cluster_num,std::vector<data_point>& td,func_t dist) : Initialization(cluster_num,td,dist){}
	void get_centers(std::vector<data_point*>& cluster_centers);
};


/*********************************************
K-means++ cluster initialization ,class
*********************************************/
class Initialization_kmeanspp : public Initialization{
public:
	Initialization_kmeanspp(int cluster_num,std::vector<data_point>& td,func_t dist) : Initialization(cluster_num,td,dist){}
	void get_centers(std::vector<data_point*>& cluster_centers);
};


#endif