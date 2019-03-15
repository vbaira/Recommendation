/*************************************/
/*     File:recommendation.h         */
/*	                                 */
/*    Definition of recommendation   */
/*				 class               */
/*                                   */
/*************************************/

#ifndef RECOM_H
#define RECOM_H

#include "utility.h"
#include <vector>
#include "./clustering/lsh_hypercube/lsh.h"
#include "./clustering/clustering.h"


/*****************************************************
Abstract class for recommendation.
Its purpose is to be inherited from classes 
which define a specific recommendation method.
******************************************************/
class Recommendation{
protected:
	std::vector<data_point>& train_data;

	double sim(std::vector<double>& u , std::vector<double>& v);
public:
	Recommendation(std::vector<data_point>& td) : train_data(td) {}
	virtual ~Recommendation(){}
	virtual void recommend(std::string output,std::vector<data_point>& queries,std::unordered_map<int,std::string> inverse_value_map)=0;
	//virtual void validate()=0;
};


/***********************************************
Class implementing the lsh recommendation method
***********************************************/
class Lsh_recommendation : public Recommendation{
private:
	int P;
	LSH_cosine lsh;
public:
	Lsh_recommendation(int p,int k,int L,int dim,int sz,std::vector<data_point>& td) : Recommendation(td), P(p) ,lsh(k,L,dim,sz,td) {}
	void recommend(std::string output,std::vector<data_point>& queries,std::unordered_map<int,std::string> inverse_value_map);
	//void validate();
};


/***********************************************
Class implementing the clustering recommendation method
***********************************************/
class Clustering_recommendation : Recommendation{
private:
	Clustering clustering;
public:
	Clustering_recommendation(int* stp,int p,int k,int L,int dim,int sz,std::vector<data_point>& td) : Recommendation(td),clustering(stp,sz/p,k,L,td,"eucledian"){}
	void recommend(std::string output,std::vector<data_point>& queries,std::unordered_map<int,std::string> inverse_value_map);
	//void validate();
};


#endif