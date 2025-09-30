#pragma once
//#include "../utils.h"
//#include "../VectorMatrix.h"
//#include "../random.h"
//#include <cmath>
#include "Worm2D.h"
class StretchReceptor {
    public:
        
        virtual void SetStretchReceptorParams(int, int, double, double) = 0;
        virtual void Update() = 0;
        virtual Params<double> getStretchReceptorParams() = 0;
};


class SRVars
{
public:
SRVars(int nstretch_):nstretch(nstretch_),
A_D_sr(nstretch_,0),A_V_sr(nstretch_,0), B_D_sr(nstretch_,0), B_V_sr(nstretch_,0){}

vector<double> A_D_sr, A_V_sr, B_D_sr, B_V_sr;
const int nstretch;
};

struct SRWeights
{
public:
vector<toFromWeight> segToA_D, segToA_V, segToB_D, segToB_V;
};

class SR{
public:
SR(int nsegs_, int nstretch_):
nsegs(nsegs_),srvars(nstretch_),nslD(nsegs_,0),nslV(nsegs_,0)
{
    //normSegLenD.SetBounds(1,nsegs_);
    //normSegLenV.SetBounds(1,nsegs_);
}

void setFromBody(const WormBody & b);
void updateSegs();
vector<double> updateSegs1(const vector<toFromWeight> & seg_, vector<double> & nsl_);
virtual SRWeights makeSRWeights() const = 0;
void setWeights(){srweights = makeSRWeights();}
virtual ~SR(){}
void updateAll(const WormBody & b){setFromBody(b);updateSegs();}
virtual void addParsToJson(json & j);

SRVars srvars;
private:
SRWeights srweights;
vector<double> nslD, nslV;

const int nsegs;

};

class SRCE : public SR
{

public:
SRCE(int nsegs_, int nstretch_):SR(nsegs_,nstretch_){}
SRWeights makeSRWeights() const;
void addParsToJson(json & j);

int SRForm = 0;
double SR_A_gain;
double SR_B_gain;
const int NSEGSSR = 6;

};