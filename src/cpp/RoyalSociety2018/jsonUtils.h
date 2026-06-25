#include "Worm.h"

template<class T> 
vector<T> getVector(TVector<T> & vec, int size)
{ 
vector<T> retvec;    
for (int i = 1; i <= size; i++)
        retvec.push_back(vec[i]);   
return retvec;    
}

template<class T> 
TVector<T> getTVector(vector<T> & vec)
{ 
TVector<T> retvec;
retvec.SetBounds(1,vec.size());    
for (int i = 0; i < vec.size(); i++) retvec[i+1]=vec[i];
return retvec;    
} 

void writeParsToJson(Worm & w);