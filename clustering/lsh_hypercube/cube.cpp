#include "cube.h"
#include <vector>
#include <utility>
#include <limits>
#include <cmath>

using namespace std;




/*********************************************************
Find true nearest neighbor of query using exhaustive search.
Return a pair of the nn and the distance from it.
*********************************************************/
std::pair<data_point&,double> Hypercube::trueNN(data_point& query){
	int nn_index=0;
	double nn_dist=distance(query.p, train_data[0].p);
	for(int i=1; i<train_data_sz; i++){
		double dist = distance(query.p, train_data[i].p );
		if (dist < nn_dist){
			nn_dist = dist ;
			nn_index = i ;
		}
	}
	pair<data_point&,double> result(train_data[nn_index],nn_dist);
	return result;
}


/***********************************************************************
return vector of bitstring with hamming distance hamming_dist from str
************************************************************************/
void Hypercube::find_neighbor(string str,int change_pos,int hamming_dist,vector<string>& neighbors){
	if (hamming_dist == 0){
		neighbors.push_back(str);
		return;
	}
	if (change_pos<0){
		return;
	}
	//change bit 
	if (str[change_pos]=='0') str[change_pos]='1';
	else str[change_pos]='0';
	find_neighbor(str,change_pos-1,hamming_dist-1,neighbors);
	//undo change and change next bit
	if (str[change_pos]=='0') str[change_pos]='1';
	else str[change_pos]='0';
	find_neighbor(str,change_pos-1,hamming_dist,neighbors);

}


/*********************************************************************************************************/


/************************************************************
Constructor.Insert train data to hypercube structure.
Each data point is inserted to a node with number same as
the number that the bitstring that g() returns,represents.
*************************************************************/
Hypercube_cosine::Hypercube_cosine(int n,std::vector<data_point>& td,int k,int M,int p,int d)
								   :Hypercube(n,td,k,M,p),H(k,1,d)
{
	for (int i=0; i<train_data_sz; i++){
		/*get g vector and convert it to int to use it as hash key*/
		vector<int> g=H.g( train_data[i].p , 0 );
		string key_str = convert_key(g);
		int key = stoi(key_str,NULL,2);
		//cout<<key<<"\n";
		/*insert to node(vertex) with number key*/
		cube[key].push_back(&train_data[i]);
	}	
}



/******************************************************************************
Find approximate nearest neighbor of query.
Find  the corresponding g() of query which is a bitstring,
convert it to int which represents the hypercube node that similar vectors are in,  
and calculate distance from M of those vectors.If node doesnt have enougn vectors
continue searching in neighboring nodes with hamming distance 1,2.Stop when you 
find M vectors of visit probes number of nodes. 
******************************************************************************/
std::pair<data_point&,double> Hypercube_cosine::approximateNN(data_point& query,double r,std::vector<data_point*>& within_range){
	data_point* ann=NULL;
	double nn_dist=numeric_limits<double>::max();
	/*get node number of query*/
	vector<int> g=H.g( query.p , 0 );
	string query_key_str=convert_key(g);
	int query_key = stoi(query_key_str,NULL,2);
	int m=0,prb=0;
	/*calculate neighboring nodes(max hamming dist=2)*/
	vector<string> neighbors;
	find_neighbor(query_key_str,query_key_str.size()-1,1,neighbors);
	find_neighbor(query_key_str,query_key_str.size()-1,2,neighbors);
	/*continue as long as m<M and prb<probes*/
	while( m<M && prb<probes){	
		for (auto i=cube[query_key].begin(); i!=cube[query_key].end(); ++i){
			if (m==M) break;
			string neighbor_id = (*i)->id;
			double dist = distance(query.p, (*i)->p);
			if (dist < r){
				within_range.push_back( *i );
			} 
			if ( dist < nn_dist){
				nn_dist = dist;
				ann = *i;
			}
			m++;
		}
		/*search next neighbor*/
		if (prb < neighbors.size()){
			query_key_str=neighbors[prb];
			query_key = stoi(query_key_str,NULL,2);
			prb++;
		}
		else{
			break;
		}
		
	}

	/*if a nn not found(run out of probes) return dist==-1*/
	if(ann!=NULL){
		return pair<data_point&,double>(*ann,nn_dist);
	}
	else{
		return pair<data_point&,double>(query,-1);
	}
	
}



/******************************************************
Convert the vector of ints that g() returns to a 
bitstring 
*******************************************************/
string Hypercube_cosine::convert_key(vector<int>& g){
	string key;
	for (int i=0; i<g.size(); i++){
		key += to_string(g[i]);
	}
	//cout << key +":"<< stoi(key,NULL,2) <<"\n";
	//return stoi(key,NULL,2);
	return key;
}



/****************************************
Cosine distance between two vectors
****************************************/
double Hypercube_cosine:: distance(vector<double>& a , vector<double>& b){
	double inner_prod = inner_product(a,b);
	int dim=a.size();
	double tmp1=0,tmp2=0;
	for (int i=0; i<dim; i++){
		tmp1 += pow(a[i],2);
		tmp2 += pow(b[i],2);
	}
	return 1 - ( inner_prod/( (sqrt(tmp1)) * (sqrt(tmp2)) ) );
}




/*********************************************************************************************************/


/************************************************************
Constructor.Insert train data to hypercube structure.
Each data point is inserted to a node with number same as
the number that the bitstring that g() returns,represents.
*************************************************************/
Hypercube_eucledian::Hypercube_eucledian(int n,std::vector<data_point>& td,int k,int M,int p,int d)
										:Hypercube(n,td,k,M,p),H(k,1,d)
{
	for (int i=0; i<train_data_sz; i++){
		/*get g vector and convert it to int to use it as hash key*/
		vector<int> g=H.g( train_data[i].p , 0 );
		string key_str = convert_key(g);
		int key = stoi(key_str,NULL,2);
		//cout<<key<<"\n";
		/*insert to node(vertex) with number key*/
		cube[key].push_back(&train_data[i]);
	}	

}


std::pair<data_point&,double> Hypercube_eucledian::approximateNN(data_point& query,double r,std::vector<data_point*>& within_range){
	data_point* ann=NULL;
	double nn_dist=numeric_limits<double>::max();
	/*get node number of query*/
	vector<int> g=H.g( query.p , 0 );
	string query_key_str=convert_key(g);
	int query_key = stoi(query_key_str,NULL,2);
	int m=0,prb=0;
	/*calculate neighboring nodes(max hamming dist=2)*/
	vector<string> neighbors;
	find_neighbor(query_key_str,query_key_str.size()-1,1,neighbors);
	find_neighbor(query_key_str,query_key_str.size()-1,2,neighbors);
	/*continue as long as m<M and prb<probes*/
	while( m<M && prb<probes){	
		for (auto i=cube[query_key].begin(); i!=cube[query_key].end(); ++i){
			if (m==M) break;
			string neighbor_id = (*i)->id;
			double dist = distance(query.p, (*i)->p);
			if (dist < r){
				within_range.push_back( *i );
			} 
			if ( dist < nn_dist){
				nn_dist = dist;
				ann = *i;
			}
			m++;
		}
		/*search next neighbor*/
		if (prb < neighbors.size()){
			query_key_str=neighbors[prb];
			query_key = stoi(query_key_str,NULL,2);
			prb++;
		}
		else{
			break;
		}
		
	}

	/*if a nn not found(run out of probes) return dist==-1*/
	if(ann!=NULL){
		return pair<data_point&,double>(*ann,nn_dist);
	}
	else{
		return pair<data_point&,double>(query,-1);
	}
}



/******************************************************
Convert the vector of ints that g() returns to a 
bitstring 
*******************************************************/
string Hypercube_eucledian::convert_key(vector<int>& g){
	string key;
	for (int i=0; i<g.size(); i++){
		key += to_string( abs( g[i]%2 ) );
	}
	//cout << key +":"<< stoi(key,NULL,2) <<"\n";
	//return stoi(key,NULL,2);
	return key;
}



/****************************************
Eucledian distance between two vectors
****************************************/
double Hypercube_eucledian::distance(std::vector<double>& a,std::vector<double>& b ){
	double dist=0;
	int dim=a.size();
	for (int i=0; i<dim; i++){
		dist+= pow(b[i]-a[i],2);
	}
	if(dist==0){
		return 0;
	}
	else{
		return sqrt(dist);
	}
} 






