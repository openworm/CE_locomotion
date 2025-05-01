#include <vector>
#include <ostream>
#include <iomanip>
using namespace std;

template<class T>
ostream & writeVector(ostream & os, const vector<T> & vals)
{
    os << setprecision(32);
    for (size_t i=0;i<vals.size(); i++) {
        os << " " << vals[i];
        }

return os;        
        
}    