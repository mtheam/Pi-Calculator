//Code written my Megan Theam 10154358 for CPSC 457 Winter 2021, using code from Pavol Federl/Michelle Nguyen. 
// ======================================================================
//
// count_pi() calculates the number of pixels that fall into a circle
//
// count_pixels() takes 2 paramters:
//  r         =  the radius of the circle
//  n_threads =  the number of threads you should create
//

#include "calcpi.h"
#include <pthread.h>    
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

std::vector<long long> CurrentCount; 
long long rSquare; 
long long radius;

//countStruct is the thread param struct, keeps track of the assigned rows and which thread it is
struct countStruct{
  int rowBegin; 
  int rowEnd; 
  int threadNumber;
};

void * calcRows(void *a){ //takes struct counStruct
  //std::cout << "Running thread..." << std::endl; 

  long long partialCount=0; 
  struct countStruct *countRows = ((struct countStruct *)a); //cast back to type

  //create local version of vars 
  int rowBegin = countRows->rowBegin; 
  int rowEnd = countRows->rowEnd; 
  int threadNumber = countRows->threadNumber;
  //std::cout << "thread:" << threadNumber << "rowBegin: " << rowBegin << "rowEnd: " << rowEnd << std::endl;
    
  for (long long x = rowBegin ; x <= rowEnd ; x++){
    for( long long y = 0 ; y <= radius ; y++){
      if( x*x + y*y <= rSquare) partialCount++;
    }
  }
  CurrentCount[threadNumber] = partialCount; //put solution in assigned lot in vector 

  pthread_exit(NULL);

}


//Basically implements Prof. Federl's suggested solution, although I add the partial sums using an external vector 
//where each thread has its own spot for a partial sum, in order to avoid race conditions/locks.
uint64_t count_pixels(int r, int n_threads)
{

  radius = (long long)r; 
  rSquare = radius*radius;
  //DEBUG figuring out number sizes 
  //std::cout << "radius in long long:" <<  radius << std::endl;
  //std::cout << "squared radius in long long: " << rSquare << std::endl;  
  long long count = 0;
  CurrentCount.resize(n_threads);

  if (n_threads > 256){
    std::cout << "Too many threads!" << std::endl; //not sure if i have to write a checker or not 
    return 1; 
  }
  
  int rowSection = r/n_threads-1; 

//init the param structure, keeping track of which row I'm on using rowMarker
  countStruct countRows[n_threads];
  long long rowMarker = 1; //don't start counting from 0! no! bad! 

  for (int i = 0 ; i < n_threads ; i++){ 
      countRows[i].rowBegin =  rowMarker;
      countRows[i].rowEnd = rowMarker + rowSection; 
      rowMarker = countRows[i].rowEnd+1; 

      countRows[i].threadNumber = i; 

      if (i==(n_threads-1)){ //the last thread, must add the remaining rows regardless of size
        countRows[i].rowEnd = r;  
      }
  } 

  //create the threads 
   pthread_t threads[n_threads]; //assuming <256 threads  
  for (int i = 0 ; i < n_threads ; i++){
    pthread_create(&threads[i], NULL, calcRows, &countRows[i]);
  }
   
  //and then join them
  for (int i =0; i<n_threads; i++){
    pthread_join(threads[i], NULL);
  }
  
  //DEBUG overflow testing
 /*  std::cout << "this is probably a bad idea: ";
  for (auto x : CurrentCount){
    std::cout << x << ", ";
  } */


  //sum the partials and then do final operations before passing 
  //long long accumulate = std::accumulate(CurrentCount.begin(), CurrentCount.end(), 0); 
  //std::cout << "accumulate: " << accumulate << std::endl; 

  long long manual = 0;
  for (auto x : CurrentCount){
    manual = manual + x;
  }
  //std::cout << "manual: " << manual << std::endl; 

  count = manual*4 + 1;   //the logic for the quadrant multiplication took me so long long to figure out 

  return count; 
}
