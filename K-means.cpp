#include "stdafx.h"
#include <iostream>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include "math.h"
#include "time.h"
#include <vector>
#include <ctime>
using namespace std;
#define NUM_THREADS 4
#define SIZE 2000
#define K 2
#define ITERATION 4
 
struct DataPoint {
    float  value;
    //float distance;
};
 
struct Centeroid {
    vector <DataPoint> datap;
    float value;
};
 
float getDistance(float x1 , float x2) {
    return abs(x1 - x2);
}
 
void minimumDistance(vector<Centeroid> *cent, DataPoint* dp) {
    float min = 10000000000;
    float distance;
    //Centeroid closestCent;
    int index;
    for (int i = 0; i < cent->size(); i++)
    {
        distance = getDistance(dp->value, cent->at(i).value);
        if (distance < min)
        {
            min = distance;
            index = i;
        }
    }
    #pragma omp critical
    cent->at(index).datap.push_back(*dp);
 
}
 
void newMeans (vector<Centeroid>* cent){
    float avg;
    int centSize =0;
    for(int i=0; i<cent->size(); i++){
        float sum=0;
        centSize = cent->at(i).datap.size();
        cout << "Centroid :" << i<< " equals"<<centSize <<endl;
        for(int j=0; j<centSize; j++){
            sum+=cent->at(i).datap.at(j).value;
        }
        if (centSize == 0){
            continue;
        }
       
        avg = sum/centSize;
       
       
        cent->at(i).value = avg;
        cout << "centroid :" << i << " average "<< avg<<endl;
    }
}
 
void main()
{
    clock_t t1,t2;
    t1=clock();
    vector<DataPoint> arrDataItems;
    vector<Centeroid> kCentroids;
    srand((signed)time(0));
 
    // initialize array of data by random values.
 
    for(int i=0; i<SIZE; i++){
        DataPoint dp;
        //dp.value=float((rand()%10000)+1);
        dp.value = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 1000;
        arrDataItems.push_back(dp);
        cout << "DataItem: "<<i <<" :equals    "<< arrDataItems.at(i).value <<endl;
    }
 
    // choose randomly the initial centroids.
    for(int j=0; j<K; j++){
        Centeroid centr;
        centr.value = arrDataItems.at(rand()%SIZE).value;
        kCentroids.push_back(centr);
        cout << kCentroids.at(j).value << "   :centroid " <<endl;
    }
   
    //SEQUENTIAL CODE
/*  for(int i=0; i<ITERATION; i++){
 
        for(int k=0; k<arrDataItems.size(); k++){
 
            minimumDistance(&kCentroids, &arrDataItems.at(k));
        }
        newMeans(&kCentroids);
 
        for(int j=0 ; j < K ; j++){
            kCentroids.at(j).datap.clear();
        }
    }
    */
   
     for (int i = 0; i < ITERATION; i++) {
               
        #pragma omp parallel num_threads(NUM_THREADS)
        {
            #pragma omp for schedule(static)
            for(int j = 0 ; j < SIZE ; j++){
                //printf(" thread num inner:%d\n", omp_get_thread_num());
                //#pragma omp critical
                minimumDistance(&kCentroids, &arrDataItems.at(j));
               
 
            }
 
        }
        newMeans(&kCentroids);
 
        for(int j=0 ; j < K ; j++){
            kCentroids.at(j).datap.clear();
        }
       
    }
   t2=clock();
   float diff ((float)t2-(float)t1);
    float seconds = diff / CLOCKS_PER_SEC;
    cout<<"Execution Time equals "<<seconds<<endl;
    system("pause");
   
 
}