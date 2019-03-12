#ifndef __EXTERNAL_PARALLEL_CSVLINE__
#define __EXTERNAL_PARALLEL_CSVLINE__

#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
namespace CsvWrapper{
    static vector<int> colOrder;
    static vector<bool> isInt;
    static void setColOrder(const vector<int>& order){
        colOrder = order;
    }
    static void setDataTypes(const vector<bool>& arr){
        isInt = arr;
    }

class CsvLine{
public:
    vector<string> comp;
    string data;

    CsvLine() {
    }

    CsvLine(const CsvLine& c){
        comp = c.comp;
        data = c.data;
    }
    CsvLine(const string& line) : data(line)
    {
        
        istringstream s(data);
        string wd;
        while (getline(s, wd, ',')){
        
            comp.push_back(wd);
        }
     



    }

    ~CsvLine(){}

    CsvLine& operator= (const CsvLine& rhs){
        comp.clear();
        data = rhs.data;
        istringstream s(rhs.data);
        string wd;
        while (getline(s, wd, ',')){
            comp.push_back(wd);
        }
        return *this;
    }
    
    friend bool operator> (const CsvLine& l, const CsvLine& r){
        for(int i : colOrder){   
            if(isInt[i]){
                int li = stoi(l.comp[i]), lr = stoi(r.comp[i]);
                if(li > lr)
                    return true;
                else if(li < lr)
                    return false;
            } 
            else {
                if(l.comp[i] > r.comp[i])
                    return true;
                else if(l.comp[i] < r.comp[i])
                    return false;
            }

        }
        return false;
    }

    friend bool operator== (const CsvLine& r, const CsvLine& l){
        for(int i : colOrder)
        {
            if(isInt[i]){
                int il = stoi(l.comp[i]), ir = stoi(r.comp[i]);
                if(il != ir)
                    return false;
            }
            else {
                if(l.comp[i] != r.comp[i])
                    return false;
            }
            
        }
        return true;
    }

    friend bool operator< (const CsvLine& l, const CsvLine& r){
        for(int i : colOrder){   
            if(isInt[i]){
                int li = stoi(l.comp[i]), lr = stoi(r.comp[i]);
                if(li < lr)
                    return true;
                else if(li > lr)
                    return false;
            } 
            else {
                if(l.comp[i] < r.comp[i])
                    return true;
                else if(l.comp[i] > r.comp[i])
                    return false;
            }

        }
        return false;
    }

    friend ostream& operator <<(ostream &os, const CsvLine &c){
        os << c.data;
        return os;
    }

    friend istream& operator >>(istream &is, CsvLine &c){
        c.comp.clear();
        is >> c.data;
        istringstream s(c.data);
        string wd;
        while (getline(s, wd, ',')){
            c.comp.push_back(wd);
        }
        return is;

    } 


};
}
#endif
