#include "utils.h"
#include <cmath>


void push_back_double(const fromToInt & val, vector<fromToInt> & vec)
{
vec.push_back(val);
vec.push_back({val.to,val.from,val.val});
}

bool check123456(const double & val, const double & val2)
{

if (val<123456.001 && val>123455.999) return true;
if (val==val2) return false;

cout << "che12 " << val << " che11 " << val2 << endl; 
assert(0);

}

bool check123456(const double & val)
{
return (val<123456.001 && val>123455.999);
}


double angle_diff(double a, double b)
{
    const double pi = 3.14159265358979323846;
    double d = a - b;
    d = std::fmod(d + pi, 2.0 * pi);
    if (d < 0)
        d += 2.0 * pi;
    return d - pi;
}

vector<intDoubDoub> toIntDoubDoub(const vector<doubDoub> & vec)
{

    vector<intDoubDoub> vec2;
    for (int i=0;i<vec.size();i++) vec2.push_back({i+1,vec[i].val1,vec[i].val2});
    return vec2;

}

vector<doubDoub> todoubDoub(const vector<intDoubDoub> & vec)
{

    vector<doubDoub> vec2(vec.size(), {123456,123456});
    for (int i=0;i<vec.size();i++) vec2[vec[i].ind-1] = {vec[i].val1,vec[i].val2};
    
    for (int i=0;i<vec2.size();i++) assert(vec2[i].val1 != 123456);
    return vec2;

}