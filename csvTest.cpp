#include "quicksorts-revised.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <assert.h>
#include <fstream>
#include <chrono>
#include "csvLine.hpp"

using namespace std;
using namespace ExternalQuicksorts;
typedef chrono::high_resolution_clock Clock;

int main(int argc, char** argv){

    string readName(argv[1]);
    string outName(argv[2]);
    vector<int> colOrder;
    vector<bool> isInt;
    istringstream colLine(argv[3]);
    string colWd;
    while( getline(colLine, colWd, ',')){
        colOrder.push_back(stoi(colWd));
    }
    CsvWrapper::setColOrder(colOrder);

    int threads = stoi(argv[4]);

    auto startClock = Clock::now();

    ifstream readFile;
    ofstream outFile;
    readFile.open(readName);
    outFile.open(outName);
    int fileLength = 0;
    string line;
    if(readFile >> line){
        ++fileLength;
        outFile << line << endl;
        istringstream fmtLine(line);
        string el;
        while( getline(fmtLine, el, ',')){
            try{
                int sToi = stoi(el);
        
                isInt.push_back(true);
            }
            catch(invalid_argument& e){
           
                isInt.push_back(false);
            }
        }
        CsvWrapper::setDataTypes(isInt);
    }
    while(readFile >> line){
        ++fileLength;
        outFile << line << endl;
    }
    readFile.close();
    outFile.close();
    split_quicksort<CsvWrapper::CsvLine>(outName, threads, 100000);

    auto endClock = Clock::now();
    cout << chrono::duration_cast<chrono::seconds>(endClock - startClock).count() << endl;
    return 0;
}
