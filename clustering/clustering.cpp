/***************************************/
/*		File:clustering.cpp            */
/*                                     */
/***************************************/

#include "clustering.h"
#include <vector>
#include <iostream>
#include <chrono>
#include <fstream>
#include <algorithm>

using namespace std;

#define MAX_REPS 15


/**********************************
Constructor.Chose the desired algorithms
for each stage of clustering
***********************************/
Clustering :: Clustering(int* stp,int k,int lsh_k,int L,vector<data_point>& td,string metric) : setup(stp),k(k),lsh_k(lsh_k),lsh_L(L),train_data(td),centroids(k),clusters(k),metric(metric),sil(k){
	if (metric == "eucledian") dist_func = eucledian_distance;
	else 					   dist_func = cosine_distance;

	if (setup[0]==1) init = new Initialization_random(k,train_data, dist_func);
	else			 init = new Initialization_kmeanspp(k,train_data, dist_func);
	

	if (setup[1]==1) 		assignment = new Assignment_lloyd(k,train_data, dist_func);
	else if (setup[1]==2) 	assignment = new Assignment_lsh(k,train_data,dist_func,metric,lsh_k,L);
	else					assignment = new Assignment_hypercube(k,train_data,dist_func,metric,lsh_k);

	if(setup[2]==1)	update = new Update_kmeans(k,train_data, dist_func);
	else			update = new Update_ipam(k,train_data, dist_func);
}



/**********************************
Destructor.Cleanup of allocated data
***********************************/
Clustering :: ~Clustering(){
	delete init;
	delete assignment;
	delete update;
	if (centroids[0]==NULL) return;
	for(int c=0; c<k; c++){
		if ( (centroids[c]->id) == "-1" ){
			delete centroids[c];
		}
	}
}



/**************************************
Start the clustering operation
*************************************/
void Clustering :: start(){
	auto start = chrono::high_resolution_clock::now();
	/*get initial centers*/
	init->get_centers(centroids);
	int reps=0;
	double obj_f_prev,obj_f;
	//cout << "MAX REPETITIONS:" << MAX_REPS << "\n";
	while(1){
		if ( reps >= MAX_REPS ) break; 			 						//stop condition #1
		if ( reps > 1 ){
			if ( ((obj_f_prev - obj_f) / obj_f_prev) < 0.001 ) break;	//stop condition #2 (obj function variation less than 0.1%)
		}
		//cout<<"Assign-Update iteration #"<< reps <<"\n";
		for (int c=0; c<k; c++){
			clusters[c].clear();
		}
		/*assign points to clusters*/
		assignment->get_clusters(centroids , clusters);
		/*update centroids*/
		update->update_centers(centroids , clusters);
		
		obj_f_prev = obj_f;
		obj_f=objective_function();
		//cout << "Ojective function score:" <<obj_f << "\n";
		reps++;
	}
	auto stop	= chrono::high_resolution_clock::now();
	chrono::duration<double> t = stop - start;
	clustering_time = t.count();

	/*calculate silhouette score*/
	//this->silhouette();
}



/*Write output to file*/
void  Clustering :: output(string output){
	vector<double> s = this->average_silhouette();
	/**open output filestream**/
	ofstream out(output);
	if ( ! out.is_open() ){
		cerr << "Unable to open output file\n";
		exit(EXIT_FAILURE);
	}

	/*print general info*/
	out << "Algorithm : I" << setup[0] << "-A" << setup[1] << "-U" <<setup[2] << "\n";
	out << "Metric : " + metric + "\n\n";

	/*print cluster data*/
	for (int c=0; c<k; c++){
		out << "CLUSTER-" << c << " { size : " << clusters[c].size() << ", centroid : " ;
		if  (setup[2]==1){
			out <<"[";
			for (auto x : centroids[c]->p){
				out << x <<" ";
			}
			out << "] }\n\n";
		}
		else{
			out << centroids[c]->id + "}\n\n"; 
		}
	}

	/*print score data*/
	out << "Clustering time : "<< clustering_time << " sec\n";
	out << "Silhouette : [ ";
	for (auto x : (this->average_silhouette()) ){
		out << x << " ";
	}
	out << "]\n\n";

	/*print clusters*/
	for (int c=0; c<k; c++){
		out << "CLUSTER-" << c << " { ";
		for ( auto x : clusters[c] ){
			out << x->id + " ";
		} 
		out << "}\n";
	}

	out.close();
}



/****************************************
Return vector with average silhouette for 
each cluster and total average silhouette
****************************************/
vector<double> Clustering :: average_silhouette(){
	vector<double> av_s;
	double total_sum=0;
	for (int c=0; c<k; c++){
		double cluster_sum=0;
		for(int i=0; i<sil[c].size(); i++){
			cluster_sum += sil[c][i];
		}
		total_sum += cluster_sum;
		if ( sil[c].size() == 1){//
			av_s.push_back( 0 );
		}
		else if( sil[c].size() == 0 ){
			av_s.push_back ( -1 );
		}
		else{
			av_s.push_back( cluster_sum / sil[c].size() );
		}
	}
	av_s.push_back( total_sum / train_data.size() );
	return av_s;
}



/****************************************
Calculate silhouette score for each point
*****************************************/
void Clustering :: silhouette(){
	for (int c=0; c<k; c++){
		for(int i=0; i<clusters[c].size(); i++){

			/*calculate average dist from points of the same cluster*/
			double a_i=0;
			for (int t=0; t<clusters[c].size(); t++){
				if ( i==t ) continue;
				a_i += dist_func(clusters[c][i]->p, clusters[c][t]->p);
			}
			if (clusters[c].size()>1){
				a_i = a_i / (clusters[c].size()-1);
			}
			else{
				a_i = 0;
			}

			/*calculate average dist from points of second nearest cluster*/
			double b_i=0;
			int c2 = second_best_center(clusters[c][i],c);
			for (int t=0; t<clusters[c2].size(); t++){
				b_i += dist_func(clusters[c][i]->p , clusters[c2][t]->p);
			}
			if (clusters[c2].size()>0){
				b_i = b_i / (clusters[c2].size());	
			}
			else{
				b_i = dist_func(clusters[c][i]->p, centroids[c2]->p );
			}
			
			/*calculate silhouette*/
			if (a_i == 0 && b_i == 0){
				sil[c].push_back( 0 ) ;
			}
			else{
				sil[c].push_back( (b_i - a_i) / max(a_i,b_i) ) ;	
			}
		}
	}
}


/**************************************
Cluster getter function
***************************************/
vector< vector<data_point*> > Clustering::get_clusters(){
	return clusters;
}



/**************************************
Cluster getter function
***************************************/
vector<data_point*> Clustering::get_centroids(){
	return centroids;
}


/******************************************
Sum of square distances of each point from 
its center
*****************************************/
double Clustering :: objective_function(){
	double sum=0;
	for (int c=0; c<k; c++){
		for(int i=0; i<clusters[c].size(); i++){
			sum += pow( dist_func(clusters[c][i]->p,centroids[c]->p) ,2);
		}
	}
}



/*******************************************
Return second nearest center of point p
********************************************/
int Clustering :: second_best_center(data_point* p,int center){
	double min_dist = MAX_D;
	int second_nearest_centroid;	
	for (int c=0; c<k; c++){
		if (c==center) continue;
		double dist = dist_func( p->p , centroids[c]->p );
		if (dist < min_dist){
			min_dist = dist;
			second_nearest_centroid = c;	
		}
	}
	return second_nearest_centroid;
}



/************************************
Cosine distance definition
************************************/
double Clustering::cosine_distance(std::vector<double>& a , std::vector<double>& b){
	double inner_prod = inner_product(a,b);
	int dim=a.size();
	double tmp1=0,tmp2=0;
	for (int i=0; i<dim; i++){
		tmp1 += pow(a[i],2);
		tmp2 += pow(b[i],2);
	}
	//cout << "cosine dist: " << 1 - ( inner_prod/( (sqrt(tmp1)) * (sqrt(tmp2)) ) ) << "\n";
	return 1 - ( inner_prod/( (sqrt(tmp1)) * (sqrt(tmp2)) ) );
}



/************************************
Eucledian distance definition
************************************/
double  Clustering::eucledian_distance( std::vector<double>& a , std::vector<double>& b){
	double dist=0;
	int dim=a.size();
	for (int i=0; i<dim; i++){
		dist+= pow(b[i]-a[i],2);
	}
	//cout << "eucledian dist: "<< sqrt(dist) << "\n";
	if(dist==0){
		return 0;
	}
	else{
		return sqrt(dist);
	}
} 