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


class SRVarsSimp : public SRVars
{
public:
SRVarsSimp(int nstretch_):SRVars(nstretch_),D_sr(1 + nstretch_,0),V_sr(1 + 2*nstretch_,0){}


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

void setFromBody(const WormBody & b);
virtual void updateSegs() = 0;
vector<double> updateSegs1(const vector<toFromWeight> & seg_, vector<double> & nsl_);
void updateSegs2(const vector<toFromWeight> & seg_, vector<double> & nsl_, vector<double> & sr_);

virtual void makeSRWeights() = 0; 
virtual void makeNSSRWeights() = 0; 
virtual void setParsFromJson(const json & j) = 0;

void setWeights(){makeSRWeights();}
void setNSWeights(const Worm2Dbase & w_ptr){makeNSSRWeights();}

virtual ~SR(){}
void updateAll(const WormBody & b){setFromBody(b);updateSegs();}
virtual void addParsToJson(json & j) const = 0;

virtual double transformSegs(const double & val) {return val;}
virtual void incNS(NSForW2D & ns) = 0;
void updateNS(const vector<toFromWeight> & seg_, const vector<double> & sr_, NSForW2D & ns_);

virtual void writeAct(ofstream & ofs) = 0;

void setPars(shared_ptr<const CmdArgs> cmd){

    setWeights();

    /* if (srpars!=nullptr){
    srpars->setPars(cmd); 
    setWeights();
    } */

}

baseParameters * const basePar1 = nullptr;
//SRVars srvars;
//shared_ptr<W2Dparameters> srpars;
shared_ptr<SRVars> srvars_ptr;
string SRType = "Base";

protected:
const int nsegs;
SR(int nsegs_, int nstretch_, baseParameters * basePar1_, shared_ptr<SRVars> srvars_ptr_):
nsegs(nsegs_),srvars_ptr(srvars_ptr_),nslD(nsegs_,0),nslV(nsegs_,0),basePar1(basePar1_){}

SR(int nsegs_, int nstretch_, shared_ptr<SRVars> srvars_ptr_):
nsegs(nsegs_),srvars_ptr(srvars_ptr_),nslD(nsegs_,0),nslV(nsegs_,0),basePar1(nullptr)
{
    //assert(0);

}


//SRWeights srweights, nssrweights;
vector<double> nslD, nslV;



};


class SR18 : public SR //bug should be 6 not 7 streatch receptors?
{
public:
    SR18():SR(50, 6, make_shared<SRVarsSimp>(6)),
    srvars(dynamic_pointer_cast<SRVarsSimp>(srvars_ptr)){SRType = "SR18";}

    SR18(shared_ptr<const CmdArgs> cmd):SR18()
    {
        SRvncgain = cmd->getArgValDoub("--SRvncgain",SRvncgain);
        SRheadgain = cmd->getArgValDoub("--SRheadgain",SRheadgain);
        vncsr = cmd->getArgValInt("--SRvncsr",vncsr);
        headsr = cmd->getArgValInt("--SRheadsr",headsr);
    }


    void addParsToJson(json & j) const;
    void setParsFromJson(const json & j);

    //int NSEGS = nSegs;                  // Number of segments
    //int NSR = nSR;                      // Number of stretch receptors
    //int nsegperstr = 6;

    double SRvncgain = 0;   //srvncgain;                // Stretch receptor gain
    double SRheadgain = 0;   // srheadgain;                // Stretch receptor gain
  //  double oldSRvncgain = 0, oldSRheadgain = 0;

    double HeadDorsalOutput(){return srvars->D_sr[0];}
    double HeadVentralOutput(){return srvars->V_sr[0];}
        

    double VCDorsalOutput(int i){return  srvars->D_sr[i];}
        
    double VCVentralAOutput(int i){ return  srvars->V_sr[i];}     
    double VCVentralPOutput(int i){return  srvars->V_sr[i + srvars_ptr->nstretch];}
        
    void incNS(NSForW2D & ns_);


    void writeAct(ofstream & ofs);
    protected:
    const int NSEGSSR = 6;                    // Number of segments that go into a stretch receptor
    
    const int NSEGSHEADSTART = 7;             // 7-12
    const int NSEGSHEAD = 14;                 // Number of segments for the sublateral head motorneurons
    const int NSEGSVNCSTART = 7;              // Segment where VNC starts

    bool vncsr = 1;
    bool headsr = 1;
    void makeNSSRWeights();
    void makeSRWeights();
    void updateSegs();

    const int N_units = 6;
    const int N_neuronsperunit = 6;
    const int DB = 1;
    const int DD = 2;
    const int VBA = 3;
    const int VDA = 4;
    const int VBP = 5;
    const int VDP = 6;
    

    // Neuron name conventions
    const int SMDD = 37;
    const int RMDD = 38;
    const int SMDV = 39;
    const int RMDV = 40;

    
    shared_ptr<SRVarsSimp> srvars;

    SRWeightsSimp srweights, nssrweights;
};


class SRCE : public SR
{

public:
SRCE(int nsegs_, int nstretch_, baseParameters* basePar1_):
SR(nsegs_,nstretch_, basePar1_, make_shared<SRVarsCE>(nstretch_))
,//srcepars(dynamic_pointer_cast<SRCEpars>(srpars)), 
srvars(dynamic_pointer_cast<SRVarsCE>(srvars_ptr)),sr_type(getSRtype()),
zeroGainsType(getSRZeroGainsType())
{
    //assert(0);
    SRType = "SRCE";

}

//static shared_ptr<SRCE> makeSRCE();

/* SRCE(int nsegs_, int nstretch_):
SR(nsegs_,nstretch_, make_shared<SRVarsCE>(nstretch_)),
srvars(dynamic_pointer_cast<SRVarsCE>(srvars_ptr))
{SRType = "SRCE";}
 */

void makeNSSRWeights();
void makeSRWeights();
//void addParsToJson(json & j) const;
//void setParsFromJson(json & j);
double transformSegs(const double & val);
void addParsToJson(json & j) const;
void setParsFromJson(const json & j);


void updateSegs();
void incNS(NSForW2D & ns);

//shared_ptr<SRCEpars> srcepars;
//int SRForm = 0;
shared_ptr<SRVarsCE> srvars;

double SR_A_gain = 0;
double SR_B_gain = 0;
void writeAct(ofstream & ofs);
protected:
/* SRCE(int nsegs_, int nstretch_,shared_ptr<baseParameters> basePar1_):
SR(nsegs_,nstretch_,basePar1_,make_shared<SRVarsCE>(nstretch_)),
srvars(dynamic_pointer_cast<SRVarsCE>(srvars_ptr)){} */

const string sr_type;
int zeroGainsType;

string getSRtype(){

    string sr_type;
    basePar1->getValCJ<string>("sr_type",sr_type,"Stretch receptor");
    return sr_type;

}

int getSRZeroGainsType(){

    int zero_gains_type;
    basePar1->getValCJ<int>(
        "sr_zero_gains_type", zero_gains_type, "stretch_receptor"
    );
    return zero_gains_type;

}

//int nsegperstr;

//SRVarsCE srvars;
SRWeights srweights, nssrweights;

    const int N_units = 10;
    const int N_neuronsperunit = 6;
    const int DA = 1;
    const int DB = 2;
    const int DD = 3;
    const int VD = 4;
    const int VA = 5;
    const int VB = 6;
};

class SRReg : public SRCE
{
public:
SRReg(int nsegs_, int nstretch_, baseParameters* basePar1_)
:SRCE(nsegs_,nstretch_,basePar1_)
//,srregpars(dynamic_pointer_cast<SRRegpars>(srcepars))
{}
void makeSRWeights();
//shared_ptr<SRRegpars> srregpars;

};
