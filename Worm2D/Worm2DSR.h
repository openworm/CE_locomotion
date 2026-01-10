#include "StretchReceptor.h"
#include "../neuromlLocal/c302ForW2D.h"


class Worm2DSRb
{

//void writeAct();
protected:

vector<doubIntParamsHead> getWormParams();
static shared_ptr<SR> getSR(const json & j);
//static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd);
static wormIzqParams getIzqPars(const json & j);
shared_ptr<SR> w2dsr_ptr = nullptr;
Worm2DSRb(const json & j);
Worm2DSRb(shared_ptr<SR> sr_ptr_);
void addParsToJson(json & j);
};

class Worm2DSRm : public Worm2Dm, public Worm2DSRb
{
public:
//Worm2DSRm(json & j, shared_ptr<const CmdArgs> cmd);
Worm2DSRm(const json & j, shared_ptr<const CmdArgs> cmd);
Worm2DSRm(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd);

//void setWormPars(shared_ptr<const CmdArgs> cmd){Worm2Dm::setWormPars(cmd);}

void addParsToJson(json & j);
void writeAct();
protected:

//Worm2DSRm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_);
//shared_ptr<SR> w2dsr_ptr = nullptr;
void Step1();
const string getModelName() {return "W2DSRm";}
//vector<doubIntParamsHead> getWormParams();
//static shared_ptr<SR> getSR(json & j);
//static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd);
//static wormIzqParams getIzqPars(json & j);
};


class Worm2DSR : public Worm2D, public Worm2DSRb
{
public:
Worm2DSR(const json & j, shared_ptr<const CmdArgs> cmd);
//Worm2DSR(json & j);
Worm2DSR(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd);

//void setWormPars(shared_ptr<const CmdArgs> cmd){Worm2D::setWormPars(cmd);}

void addParsToJson(json & j);

void writeAct();

//void writeAct(){return Worm2DSRm::writeAct();}
protected:

Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_);
//shared_ptr<SR> w2dsr_ptr = nullptr;
void Step1();
const string getModelName() {return "W2DSR";}
//vector<doubIntParamsHead> getWormParams();
//static shared_ptr<SR> getSR(json & j);
//static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd, const json & j);
//static wormIzqParams getIzqPars(json & j);



};

struct Worm2DSREpars
{

public:
vector<doubDoub> genPhenLims;
vector<vector<string> > TFnames, IPnames;
vector<vector<fromToInt> > TFIvec;
vector<vector<intPair> > IPvec;

};


class Worm2DSRE : public Worm2DSR, public EvolvableS
{
    public:
Worm2DSRE(const json & j, shared_ptr<const CmdArgs> cmd, bool callInit = false);
//Worm2DSR(json & j);
Worm2DSRE(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd);

void setEvolPars(W2Dparameters & w2par_, string evotype_);
void setParsFromPheno(const TVector<double> &pheno);
int getVectSize() {assert(genPhenPars.genPhenLims.size()>0); return genPhenPars.genPhenLims.size();}
void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
void PhenGenMapping(vector<double> &gen, const vector<double> &phen);
//void setNSEvoFromJson(const json & j, NervousSystem & n);
//void makeVals(const json & j);
Worm2DSREpars makeVals(const json & j);
void testJson(json & j);
//void setInitGeno();
vector<double> getInitGeno();
void addEvolvableToJson(json & j);
void writeOrigGen(shared_ptr<const CmdArgs> cmd);
//vector<toFromInt> chem_weights_evo, elec_weights_evo;
//vector<intPair> biases_evo, taus_evo, gains_evo;

const Worm2DSREpars genPhenPars;
//vector<doubDoub> genPhenLims;
//vector<vector<string> > TFnames, IPnames;
//vector<vector<fromToInt> > TFIvec;
//vector<vector<intPair> > IPvec;
//vector<double> initialGeno;
};


class SensorPars
{
  public:

vector<double> chemConHistory;
//TVector<double> chemConHistory;
double sensorN, sensorM;
//double dSensorN, dSensorM;
int iSensorN, iSensorM;
//double chemCon, presentAvgCon, pastAvgCon;
double presentAvgCon, pastAvgCon;
int extInp1, extInp2;
double gradSteep, HSStepSize, x_center, y_center;

void setParsFromJson(const json & j);
void writeParsToJson(json & j) const;

};

class Sensor
{
public:

Sensor(const json & j, shared_ptr<gradParameters> CO2DSRpars_, Worm2Dbody & wb_):
CO2DSRpars(CO2DSRpars_),
wb(wb_)
{

  setParsFromJson(j,CO2DSRpars_);

}

void setParsFromJson(const json & j, shared_ptr<gradParameters> CO2DSRpars_);
//void setParsFromJson(const json & j);
void writeParsToJson(json & j) const;

double headDistanceToCenter() {return wb.headDistanceToCenter();}
double headDistanceToLocation(const double & x, const double & y) {return wb.headDistanceToLocation(x,y);}

void ResetChemCon();
void UpdateChemCon();
void InitialiseAgent();
void assignExternalInput(vector<double> & externalInputs);

Worm2Dbody & wb;
shared_ptr<gradParameters> CO2DSRpars;

vector<SensorPars> spvec;

//vector<double> chemConHistory;
//TVector<double> chemConHistory;
//double sensorN, sensorM;
//double dSensorN, dSensorM;
//int iSensorN, iSensorM;
//double chemCon, presentAvgCon, pastAvgCon;
//double presentAvgCon, pastAvgCon;

//int timer;
};


class WormCO2DSR : public Worm2DSRE, public Sensor
{
public:
//WormCO18Full(const string & filename_, shared_ptr<const CmdArgs> cmd_):    
//Worm2DSR(jsonfilename_,cmd){}
WormCO2DSR(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd):
WormCO2DSR(getJsonFromFile(jsonfilename_),cmd){}

WormCO2DSR(const json & j, shared_ptr<const CmdArgs> cmd, bool callInit = false):Worm2Dm(getIzqPars(j),
  getNS(cmd, j), shared_ptr<gradParameters>(make_shared<gradParameters>())),
  Worm2DSRE(j,cmd,callInit),Sensor(j, dynamic_pointer_cast<gradParameters>(W2Dbaseparameters1b), *this)
  {}



//WormCO2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> sr_ptr_):
//Worm2DSR(par1_,n_ptr_,sr_ptr_),Worm2Dm(par1_, n_ptr_){}


void initForSimulation(RandomState& rs);
void InitializeState(RandomState &rs);
//void ResetAgentsBody();
//void ResetChemCon();
//void UpdateChemCon();
//void UpdateSensors();
//void ResetAgentIntState(RandomState &rs);
//virtual void SetParameters(const TVector<double> &v);
	//void InitialiseAgent(double runduration, double stepsize);
//void InitialiseAgent();
void Step1();
void assignExternalInput();



};
