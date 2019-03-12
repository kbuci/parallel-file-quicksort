#ifndef __EXTERNAL_PARALLEL_QUICKSORTS__
#define __EXTERNAL_PARALLEL_QUICKSORTS__

#include <vector>
#include <thread>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <memory>
using namespace std;
namespace ExternalQuicksorts{
    

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

          
                quicksortMem(arr, left, part);                //cout << "right" << part+1 << " " << right << endl;
                quicksortMem(arr, part+1, right);

        
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
        T pivot;
        ofstream lowerFile, higherFile;
        lowerName = arrFile + ":L";
        higherName = arrFile + ":R";
        lowerFile.open(lowerName);
        higherFile.open(higherName);
        if( readFile >> pivot){
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
            lowerFile << pivot << endl;
            ++lowerLength;
        } 
        cout << pivot << " split " << lowerLength << " " << higherLength << endl;
        lowerFile.close();
        higherFile.close();
        readFile.close();
    }    

    //external sort
    template <class T>
    void quicksort(string arrFile, int fileLength, int buffer ){
            if(fileLength <= buffer){
                vector<T> arr;
                T el;
                ifstream arrFileStream;
                arrFileStream.open(arrFile);
                while( arrFileStream >> el ){
                    arr.push_back(el);

                }
                arrFileStream.close();
                quicksortMem(arr, 0, arr.size() - 1);
                ofstream retFile;
                retFile.open(arrFile);   
                for(const auto& v : arr)
                    retFile << v << endl;
                retFile.close();
    
            } 
            else 
            {
                int lowerLen = 0;
                int higherLen = 0;
                string lowerName, higherName;

                partition<T>(arrFile, lowerLen, higherLen, lowerName, higherName);
                cout << arrFile << endl;
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

    template <class T>
    void split_quicksort(string arrFile, int threads, int buffer){
        vector<T> pivots;
        vector<string> partitionFiles;
        vector<ofstream> partitionStreams;
        vector<int> partitionFileLengths;
        T pivot;
        int line = 0;
        ifstream arrStream;
        arrStream.open(arrFile);

        partitionStreams.emplace_back(ofstream{});
        partitionStreams[0].open(arrFile + to_string(line));
        partitionFileLengths.push_back(line);
        partitionFiles.push_back(arrFile + to_string(line));
        while(arrStream >> pivot && line++ <= threads){
            cout << "pivot " << pivot << endl;
            pivots.push_back(pivot);
            partitionFiles.push_back(arrFile + to_string(line));
            partitionStreams.emplace_back(ofstream{});
            partitionStreams[partitionStreams.size() - 1].open(arrFile + to_string(line));
            partitionFileLengths.push_back(0);
        }
        sort(pivots.begin(), pivots.end());
        
        arrStream.clear();
        arrStream.seekg(0, ios::beg);

        T el;
        while(arrStream >> el){
            int i = 0;
            for(; i < pivots.size() && el > pivots[i]; ++i );
           // cout << i << endl;
            partitionStreams[i] << el << endl;
            ++partitionFileLengths[i];
        }
    

        for(ofstream& stream : partitionStreams){
            stream.close();
        }
        arrStream.close();
        vector<thread> qthreads;
        cout << "T" << endl;
        cout << partitionStreams.size() << endl;
        cout << partitionFiles.size() << endl;
        for(int i = 2; i < partitionFiles.size(); ++i){
            qthreads.push_back(thread(quicksort<T>, partitionFiles[i],partitionFileLengths[i],buffer ));
        }
        cout << "T" << endl;
        
        cout << partitionFileLengths[0] << endl;
        cout << partitionFileLengths[1] << endl;
        cout << partitionFiles[0] << endl;
        cout << partitionFiles[1] << endl;
        
        quicksort<T>(partitionFiles[0],partitionFileLengths[0], buffer);
        quicksort<T>(partitionFiles[1],partitionFileLengths[1], buffer);
        

        for(thread& qthread : qthreads)
            qthread.join();

        ofstream arrOut;
        arrOut.open(arrFile);
        for(string partitionFile : partitionFiles){
            ifstream partStream;
            partStream.open(partitionFile);
            T partEl;
            while(partStream >> partEl)
                arrOut << partEl << endl;
            partStream.close();
        }
        arrOut.close();




    }


}
#endif