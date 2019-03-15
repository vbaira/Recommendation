

#ifndef CUBE_H
#define CUBE_H

#include "../../utility.h"
#include "h_family.h"
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cmath>



class Hypercube{
protected:
	int train_data_sz;								//size of train data
	std::vector<data_point>& train_data;			//reference to train data	
	std::vector< std::vector<data_point*> > cube;	//vector of size 2^k(d')
	int k;
	int M;
	int probes;

public:
	Hypercube(int n,std::vector<data_point>& td,int k,int M,int p) 
			 : k(k),M(M),probes(p),train_data_sz(n),train_data(td),cube(pow(2,k)){}
	virtual ~Hypercube(){}
	virtual double distance(std::vector<double>& a,std::vector<double>& b)=0;
	virtual std::pair<data_point&,double> approximateNN(data_point& query,double r,std::vector<data_point*>& within_range)=0;
	static void find_neighbor(std::string str,int change_pos,int hamming_dist,std::vector<std::string>& neighbors);
	std::pair<data_point&,double> trueNN(data_point& query);
};




class Hypercube_cosine : public Hypercube{
private:
	H_family_cosine H;
	std::string convert_key(std::vector<int>& g);

public:
	Hypercube_cosine(int n,std::vector<data_point>& td,int k,int M,int p,int d);
	double distance(std::vector<double>& a,std::vector<double>& b );
	std::pair<data_point&,double> approximateNN(data_point& query,double r,std::vector<data_point*>& within_range);
};




class Hypercube_eucledian : public Hypercube{
private:
	H_family_eucledian H;
	std::string convert_key(std::vector<int>& g);

public:
	Hypercube_eucledian(int n,std::vector<data_point>& td,int k,int M,int p,int d);
	double distance(std::vector<double>& a,std::vector<double>& b );
	std::pair<data_point&,double> approximateNN(data_point& query,double r,std::vector<data_point*>& within_range);
};







#endif
