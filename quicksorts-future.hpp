#ifndef __EXTERNAL_PARALLEL_QUICKSORTS__
#define __EXTERNAL_PARALLEL_QUICKSORTS__

#include <vector>
#include <thread>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <mutex>
#include <future>

using namespace std;
namespace ExternalQuicksorts{
    
    static mutex countLock;
    static int threads; 

    void setThreadCount(int t){
        countLock.lock();
        threads = t;
        countLock.unlock();
    }

    template <class T>
    int median(vector<T>& arr, int l, int m, int r){
        T arr_l = arr[l], arr_r = arr[r], arr_m = arr[m];
        //cout << arr_l << " " << arr_m << " " << arr_r << endl;
        if( arr_l < arr_m){
            if (arr_m < arr_r){
                return m;
            }
            else{
                if(arr_l < arr_r){
                    return r;
                }
                else{
                    return l;
                }
            }
        } else {
            if (arr_r < arr_m){
                return m;
            }
            else{
                if( arr_r < arr_l){
                    return r;
                }
                else{
                    return l;
                }
            }
        }
    }

    //In memory partition
    template <class T>
    int partition(vector<T>& arr,int l, int r){
        int med = median(arr, l, (l + r)/2,r);
        swap(arr[l],arr[med]);
        T pivot = arr[l];
        int left = l - 1;
        int right = r + 1;
        while(true)
        {
            do{
                ++left;
            }
            while(arr[left] < pivot);
            do{
                --right;
            }
            while(arr[right] > pivot);
            if (left >= right){
                return right;
            }
            swap(arr[left],arr[right]);
        }
    }

    //In memory sort
    template <class T>
    void quicksortMem(vector<T>& arr,int left, int right){
        if (left < right)
        {
            //cout << "base" << left << " " << right << endl;
            int part = partition(arr,left,right);
            countLock.lock();
            if(threads > 0){
                --threads;
                countLock.unlock();
                future<void> sortResult(async(quicksortMem<T>, ref(arr),left, part ));
                quicksortMem(arr, part+1, right);
                sortResult.get();
                countLock.lock();
                ++threads;
                countLock.unlock();
                 
            }
            else{
                countLock.unlock();
                //cout << "left" << left << " " << part-1 << endl;                
                quicksortMem(arr, left, part);
                //cout << "right" << part+1 << " " << right << endl;
                quicksortMem(arr, part+1, right);

            }
        }
    }

    template <class T>
    T median(T l, T m, T r){
        if( l < m){
            if (m < r){
                return m;
            }
            else{
                if(l < r){
                    return r;
                }
                else{
                    return l;
                }
            }
        } else {
            if (r < m){
                return m;
            }
            else{
                if( r < l){
                    return r;
                }
                else{
                    return l;
                }
            }
        }
    }

    //external partition
    template <class T>
    void partition(string arrFile, int& lowerLength, int& higherLength, string& lowerName, string& higherName){
        ifstream readFile;
        readFile.open(arrFile);
        T pivot, leftPivot, middlePivot, rightPivot;
        ofstream lowerFile, higherFile;
        lowerName = arrFile + ":L";
        higherName = arrFile + ":R";
        lowerFile.open(lowerName);
        higherFile.open(higherName);
        if( readFile >> leftPivot && readFile >> middlePivot && readFile >> rightPivot)
            pivot = median(leftPivot, rightPivot, middlePivot);
        readFile.clear();
        readFile.seekg(0, ios::beg);
        //if( readFile >> pivot){
            T el;
            while( readFile >> el ){
                if( el < pivot ){
                    lowerFile << el << endl;
                    ++lowerLength;
                }
                else{
                    higherFile << el << endl; 
                    ++higherLength;
                }
            }
            //lowerFile << pivot << endl;
            //++lowerLength;
        //}
        cout << "2 splits" << endl;     
        lowerFile.close();
        higherFile.close();
        readFile.close();
    }    

    //external sort
    template <class T>
    void quicksort(string arrFile, int fileLength, int buffer=1000 ){
            cout << fileLength << endl;
            cout << arrFile << endl;
            if(fileLength <= buffer){
                vector<T> arr;
                T el;
                ifstream arrFileStream;
                arrFileStream.open(arrFile);
                cout << "read into memory" << endl;
                while( arrFileStream >> el ){
                    arr.push_back(el);

                }
                arrFileStream.close();
                cout << "read into memory complete" << endl;
                quicksortMem(arr, 0, arr.size() - 1);
                ofstream retFile;
                cout << "memory quicksort complete" << endl;
                retFile.open(arrFile);   
                for(const auto& v : arr)
                    retFile << v << endl;
                retFile.close();
                cout << "memory complete" << endl;
            } 
            else 
            {
                int lowerLen = 0;
                int higherLen = 0;
                string lowerName, higherName;
                partition<T>(arrFile, lowerLen, higherLen, lowerName, higherName);
                remove(arrFile.c_str());
                
                    quicksort<T>(lowerName, lowerLen, buffer);
                    quicksort<T>(higherName, higherLen, buffer);
                
                
                ofstream lowerFile;
                ifstream higherFile;
                lowerFile.open(lowerName, fstream::app);
                higherFile.open(higherName);
                T el;
                while( higherFile >> el )
                    lowerFile << el << endl;
                lowerFile.close();
                higherFile.close();
                remove(higherName.c_str());
                rename(lowerName.c_str(),arrFile.c_str());
            }
    }


}
#endif
