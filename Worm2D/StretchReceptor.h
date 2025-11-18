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
SRVars(int nstretch_):nstretch(nstretch_){}

virtual ~SRVars(){}
const int nstretch;
};


class SRVars18 : public SRVars
{
public:
SRVars18(int nstretch_):SRVars(nstretch_),D_sr(nstretch_,0),V_sr(nstretch_,0){}

vector<double> D_sr, V_sr;
//vector<double> D_sr, V_sr;

};

class SRVarsCE : public SRVars
{
public:
SRVarsCE(int nstretch_):SRVars(nstretch_),
A_D_sr(nstretch_,0),A_V_sr(nstretch_,0), B_D_sr(nstretch_,0), B_V_sr(nstretch_,0){}

vector<double> A_D_sr, A_V_sr, B_D_sr, B_V_sr;
//vector<double> D_sr, V_sr;

};

struct SRWeights
{
public:
vector<toFromWeight> segToA_D, segToA_V, segToB_D, segToB_V;

void swapAll(SRWeights & srw){
    segToA_D.swap(srw.segToA_D);
    segToA_V.swap(srw.segToA_V);
    segToB_D.swap(srw.segToB_D);
    segToB_V.swap(srw.segToB_V);
}
//vector<toFromWeight> segToD, segToV;
};

struct SRWeightsSimp
{
public:
vector<toFromWeight> segToD, segToV;

void swapAll(SRWeightsSimp & srw){
    segToD.swap(srw.segToD);
    segToV.swap(srw.segToV);
}


};


class SR{
public:

//SR(int nsegs_, int nstretch_):SR(nsegs_, nstretch_, nullptr){}

void setFromBody(const WormBody & b);
virtual void updateSegs() = 0;
vector<double> updateSegs1(const vector<toFromWeight> & seg_, vector<double> & nsl_);

virtual void makeSRWeights() = 0; // {assert(0);}
virtual void makeNSSRWeights(const Worm2Dbase & w_ptr) = 0; //{assert(0);}

virtual void setParsFromJson(json & j) = 0;


void setWeights(){makeSRWeights();}

void setNSWeights(const Worm2Dbase & w_ptr){
    makeNSSRWeights(w_ptr);
//assert(0);
}

virtual ~SR(){}
void updateAll(const WormBody & b){setFromBody(b);updateSegs();}
virtual void addParsToJson(json & j) const = 0;

virtual double transformSegs(const double & val) {return val;}
virtual void incNS(NSForW2D & ns) = 0;
void updateNS(const vector<toFromWeight> & seg_, const vector<double> & sr_, NSForW2D & ns_);



void setPars(shared_ptr<const CmdArgs> cmd){

    //assert(0);
    if (srpars!=nullptr){
    srpars->setPars(cmd); 
    setWeights();
    }

}

//SRVars srvars;
shared_ptr<W2Dparameters> srpars;
shared_ptr<SRVars> srvars_ptr;

protected:
const int nsegs;
SR(int nsegs_, int nstretch_, shared_ptr<W2Dparameters> srpars_, shared_ptr<SRVars> srvars_ptr_):
nsegs(nsegs_),srvars_ptr(srvars_ptr_),nslD(nsegs_,0),nslV(nsegs_,0),srpars(srpars_){}

//SRWeights srweights, nssrweights;
vector<double> nslD, nslV;


};


class SR18 : public SR //bug should be 6 not 7 streatch receptors?
{
public:
    SR18(shared_ptr<const CmdArgs> cmd):
    SR(50,7, nullptr, make_shared<SRVars18>(7)),
    srvars(dynamic_pointer_cast<SRVars18>(srvars_ptr))
    {
        SRvncgain = cmd->getArgValDoub("--SRvncgain",SRvncgain);
        SRheadgain = cmd->getArgValDoub("--SRheadgain",SRheadgain);
    }



    //int NSEGS = nSegs;                  // Number of segments
    //int NSR = nSR;                      // Number of stretch receptors
    //int nsegperstr = 6;

    const int NSEGSSR = 6;                    // Number of segments that go into a stretch receptor
    double SRvncgain = 0;   //srvncgain;                // Stretch receptor gain
    double SRheadgain = 0;   // srheadgain;                // Stretch receptor gain

    const int NSEGSHEADSTART = 7;             // 7-12
    const int NSEGSHEAD = 14;                 // Number of segments for the sublateral head motorneurons
    const int NSEGSVNCSTART = 7;              // Segment where VNC starts

    bool vncsr = 1;
    bool headsr = 1;
    void makeNSSRWeights(const Worm2Dbase & w_ptr);
    void makeSRWeights();
    void updateSegs();
    shared_ptr<SRVars18> srvars;

    SRWeightsSimp srweights, nssrweights;
};


class SRCE : public SR
{

public:
SRCE(int nsegs_, int nstretch_):
SR(nsegs_,nstretch_,make_shared<SRCEpars>(), make_shared<SRVarsCE>(nstretch_))
,srcepars(dynamic_pointer_cast<SRCEpars>(srpars)), 
srvars(dynamic_pointer_cast<SRVarsCE>(srvars_ptr))
{}

void makeNSSRWeights(const Worm2Dbase & w_ptr);
void makeSRWeights();
//void addParsToJson(json & j) const;
//void setParsFromJson(json & j);
double transformSegs(const double & val);
void addParsToJson(json & j) const;
void setParsFromJson(json & j);


void updateSegs();
void incNS(NSForW2D & ns);

shared_ptr<SRCEpars> srcepars;
//int SRForm = 0;
shared_ptr<SRVarsCE> srvars;

double SR_A_gain = 0;
double SR_B_gain = 0;

protected:
SRCE(int nsegs_, int nstretch_,shared_ptr<SRCEpars> srcepars_):
SR(nsegs_,nstretch_,srcepars_,make_shared<SRVarsCE>(nstretch_)),
srvars(dynamic_pointer_cast<SRVarsCE>(srvars_ptr)){}

//int nsegperstr;

//SRVarsCE srvars;
SRWeights srweights, nssrweights;



};

class SRReg : public SRCE
{
public:
SRReg(int nsegs_, int nstretch_)
:SRCE(nsegs_,nstretch_,make_shared<SRRegpars>())
,srregpars(dynamic_pointer_cast<SRRegpars>(srcepars))
{}
void makeSRWeights();
shared_ptr<SRRegpars> srregpars;

};