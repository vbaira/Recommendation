# Recommendation  
Implementation of Clustering and Cosine LSH Recommendation algorithms.  
**Clustering Recommendation** takes into consideration user data points that fall into the same cluster as the user in order to calculate the recommended items.  
**Cosine LSH Recommendation** takes into consideration the P nearest neighbors of the user (that are calculated using the Cosine LSH algorithm) in order to calculate the recommended items.


## Compilation
~~~
make
~~~

## Execution 
To execute the program :
~~~
./recommendation –d <input file> -o <output file>
~~~
- Execution can also be performed without all or some of the arguments.In that case the user is prompted to enter the value of the missing argument at runtime (input and output file).  
