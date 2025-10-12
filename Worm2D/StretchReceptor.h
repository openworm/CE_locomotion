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
nsegs(nsegs_),srvars(nstretch_),nslD(nsegs_,0),nslV(nsegs_,0){}

void setFromBody(const WormBody & b);
void updateSegs();
vector<double> updateSegs1(const vector<toFromWeight> & seg_, vector<double> & nsl_);
virtual SRWeights makeSRWeights() const = 0;
virtual SRWeights makeNSSRWeights(shared_ptr<const Worm2Dbase> w_ptr) const = 0;
void setWeights(){srweights = makeSRWeights();}
void setNSWeights(shared_ptr<const Worm2Dbase> w_ptr){nssrweights = makeNSSRWeights(w_ptr);}
virtual ~SR(){}
void updateAll(const WormBody & b){setFromBody(b);updateSegs();}
virtual void addParsToJson(json & j);
virtual double transformSegs(const double & val){return val;}
void incNS(NSForW2D & ns);
void updateNS(const vector<toFromWeight> & seg_, NSForW2D & ns_);

SRVars srvars;
protected:
const int nsegs;


SRWeights srweights, nssrweights;
vector<double> nslD, nslV;



};

class SRCE : public SR
{

public:
SRCE(int nsegs_, int nstretch_):
SR(nsegs_,nstretch_),srcepars(make_shared<SRCEpars>()){}

SRWeights makeNSSRWeights(shared_ptr<const Worm2Dbase> w_ptr) const;
SRWeights makeSRWeights() const;
void addParsToJson(json & j);
void setParsFromJson(json & j);
double transformSegs(const double & val);

void setPars(shared_ptr<const CmdArgs> cmd){

    //assert(0);
    srcepars->setPars(cmd); 
    setWeights();

}


shared_ptr<SRCEpars> srcepars;
//int SRForm = 0;
double SR_A_gain;
double SR_B_gain;

protected:
SRCE(int nsegs_, int nstretch_,shared_ptr<SRCEpars> srcepars_):
SR(nsegs_,nstretch_),srcepars(srcepars_){}

//int nsegperstr;


};

class SRReg : public SRCE
{
public:
SRReg(int nsegs_, int nstretch_)
:SRCE(nsegs_,nstretch_,make_shared<SRRegpars>()),
srregpars(dynamic_pointer_cast<SRRegpars>(srcepars)){}
SRWeights makeSRWeights() const;
shared_ptr<SRRegpars> srregpars;

};