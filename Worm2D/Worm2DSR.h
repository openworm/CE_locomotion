#include "StretchReceptor.h"
#include "../neuromlLocal/c302ForW2D.h"




class Worm2DSRb
{

//void writeAct();
protected:

vector<doubIntParamsHead> getWormParams();
static shared_ptr<SR> getSR(const json & j, baseParameters * basePar1_);
//static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd);
//static wormIzqParams getIzqPars(const json & j);
shared_ptr<SR> w2dsr_ptr = nullptr;
//Worm2DSRb(const json & j);
Worm2DSRb(shared_ptr<SR> sr_ptr_);

void setParsFromJson(const json & j);
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


class Worm2DSR :  public Worm2D, public Worm2DSRb
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

Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_, 
  shared_ptr<const CmdArgs> cmd, const json & j);
Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_,
  shared_ptr<const CmdArgs> cmd, const json & j, bool forceNoOrigInputs);
Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_, 
  shared_ptr<const CmdArgs> cmd);

//shared_ptr<SR> w2dsr_ptr = nullptr;
void Step1();
const string getModelName() {return "W2DSR";}
//vector<doubIntParamsHead> getWormParams();
//static shared_ptr<SR> getSR(json & j);
//static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd, const json & j);
//static wormIzqParams getIzqPars(json & j);

//json jsects;

};

/* struct Worm2DSREpars
{

public:
vector<doubDoub> genPhenLims;
vector<vector<string> > TFnames, IPnames, singValnames;
vector<vector<fromToInt> > TFIvec;
vector<vector<intPair> > IPvec;
vector<int> singVals; 

};
 */

class Worm2DSRE : public Worm2DSR, public EvolvableS
{
    public:

//json itsJson;
const vector<intDoubDoub> genPhenLims;



void applyFuncables();
void applyFuncables(json & j1_);
void applyScheduledFuncable(
    const int function_index, const bool has_condval, const int condval) override;
virtual void resetScheduledFuncableFromJson(const json & j);

void setEvolPars(W2Dparameters & w2par_, string evotype_);
void setParsFromPheno(const TVector<double> &pheno);
//void setParsFromPheno(const vector<double> &pheno);
int getVectSize() {assert(genPhenLims.size()>0); return genPhenLims.size();}
void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
void PhenGenMapping(vector<double> &gen, const vector<double> &phen);
//void setNSEvoFromJson(const json & j, NervousSystem & n);
//void makeVals(const json & j);
//Worm2DSREpars makeVals(const json & j);
//vector<doubDoub> makeVals(const json & j);
vector<intDoubDoub> makeVals();
vector<string> getEvolvedUsedTags() const;
void testJson(json & j);
//void setInitGeno();
void setInitPheno();
//const vector<double> & getCurrentPheno();
//vector<double> getCurrentPheno();
vector<double> getInitGeno();
void addEvolvableToJson(json & j);
//void addFuncableToJson(json & j);
void writeOrigGen(shared_ptr<const CmdArgs> cmd);
void writeOrigGen(shared_ptr<const CmdArgs> cmd, const vector<double> & initGeno);
//vector<toFromInt> chem_weights_evo, elec_weights_evo;
//vector<intPair> biases_evo, taus_evo, gains_evo;
//vector<double> getInitGeno_old();
//void addParsToJson(json & j){j = itsJson;}
//vector<double> getInitGeno_old();
void setParsFromPheno_old(const TVector<double> &pheno);
void resetFromBPJson();
void resetFromJson(const json & js1);

protected:
Worm2DSRE(const json & j, shared_ptr<const CmdArgs> cmd, bool callInit = false);
Worm2DSRE(const json & j, shared_ptr<const CmdArgs> cmd, bool callInit, bool forceNoOrigInputs);
//Worm2DSR(json & j);
Worm2DSRE(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd);

//const Worm2DSREpars genPhenPars;
//vector<doubDoub> genPhenLims;
//vector<vector<string> > TFnames, IPnames;
//vector<vector<fromToInt> > TFIvec;
//vector<vector<intPair> > IPvec;
//vector<double> initialGeno;

//void getEvoNames(string & evoName, const json& j, vector<string> &);
//void getEvoNames1(string & evoName, json::const_iterator it2, vector<string>&);
//const vector<doubDoub> genPhenLims;
//vector<string> evoNames;
//string evoName;
//bool directMuscEvo = false;
//json itsJson;

//void callEfcond(const json & j);
};


class EnvironmentPars
{
  public:

string name;
double x_center, y_center, gradSteep;

void setParsFromJson(const string & key, const json & j);
void writeParsToJson(json & j) const;
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
	int extInp1 = -1, extInp2 = -1;
	double HSStepSize = 0.01;
	string name;
	string environmentName;

void setParsFromJson2(const json & j);
void setParsFromJson(const json & j);
void writeParsToJson(json & j) const;
void writeParsToJson2(json & j) const;
};

class Sensor  : public WormGrad
{
public:

//Sensor(const json & j, shared_ptr<gradParameters> CO2DSRpars_, Worm2Dbody & wb_):
Sensor(const json & j, Worm2Dm & wb_):
//CO2DSRpars(CO2DSRpars_),
wb(wb_)
{
  construct(j);
  //setParsFromJson(j,CO2DSRpars_);
  //setParsFromJson(j);
}

//void setParsFromJson(const json & j, shared_ptr<gradParameters> CO2DSRpars_);
void setParsFromJson(const json & j);
void addParsToJson(json & j) const;
void construct(const json & j);
double distanceToCenter() const {return wb.headDistanceToCenter();}
double headDistanceToLocation(const double & x, const double & y) const {return wb.headDistanceToLocation(x,y);}

void ResetChemCon();
void UpdateChemCon();
void InitialiseAgent();
void assignExternalInput(vector<double> & externalInputs);
void InitializeSensors(RandomState& rs);
void setGradientSteepness(const double & gradSteep) override;
void ResetAgentsBody(){
  //wb.ResetAgentsBody(CO2DSRpars);
  wb.ResetAgentsBody(wb);
}

Worm2Dm & wb;
//shared_ptr<gradParameters> CO2DSRpars;
//shared_ptr<baseParameters> sensor_basePars1;

vector<SensorPars> spvec;
vector<EnvironmentPars> environmentVec;

EnvironmentPars & getEnvironment(const string & name);
const EnvironmentPars & getEnvironment(const string & name) const;

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
  getNS(cmd, j), cmd, j), Worm2DSRE(j,cmd,callInit,true), Sensor(j, *this){}



void addParsToJson(json & j);
void applyFuncablesExt();
void resetScheduledFuncableFromJson(const json & j) override;


void setParsFromPheno(const TVector<double> &pheno);

//WormCO2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> sr_ptr_):
//Worm2DSR(par1_,n_ptr_,sr_ptr_),Worm2Dm(par1_, n_ptr_){}

const string getModelName() {return "COW2DSR";}
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

protected:




};
