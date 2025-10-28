//#include "../TSearch.h"
#include "../VectorMatrix.h"
#include "jsonUtils.h"





class W2Dparameters
{
public:
virtual ~W2Dparameters(){}
virtual void setParsFromJson(json & j) = 0;
virtual void addParsToJson(json & j) const = 0;
virtual void setPars(shared_ptr<const CmdArgs> cmd) = 0;
};




class EvolvableS
{
  public:
  
  virtual void GenPhenMapping(TVector<double> &gen, TVector<double> &phen) = 0;
  virtual int getVectSize() = 0;
  virtual void setParsFromPheno(const TVector<double> &pheno) = 0;
  virtual void setEvolPars(W2Dparameters & w2par_, string evotype_) = 0;
  virtual void setWormPars(shared_ptr<const CmdArgs> cmd) = 0;

  virtual ~EvolvableS(){}
  
  void setParsFromFile(const string & genofilename_);
  void setParsFromGeno(TVector<double> &geno);
  

};


class W2Dbaseparameters : virtual public W2Dparameters
{

public:
W2Dbaseparameters(){}
W2Dbaseparameters(int argc, const char* argv[]);
//W2Dbaseparameters(shared_ptr<const CmdArgs> cmd);
bool randomInitialState = 0;
bool doOrigMuscInput = 1;
void setParsFromJson(json & j){
  randomInitialState = j["randomInitialState"]["value"];
  doOrigMuscInput = j["doOrigMuscInput"]["value"];
}
void addParsToJson(json & j) const {
  j["randomInitialState"]["value"] = randomInitialState;
  j["doOrigMuscInput"]["value"] = doOrigMuscInput;
}

void setPars(shared_ptr<const CmdArgs> cmd);



};


class AgarPars : virtual public W2Dparameters
{
  public:
  AgarPars(){}
  AgarPars(int argc, const char* argv[]);
  AgarPars(shared_ptr<const CmdArgs> cmd);

  void setPars(shared_ptr<const CmdArgs> cmd);

double OSCTbase = 0.25; // Cap for oscillation evaluation
double agarfreq = 0.44;
double AvgSpeed = 0.00022; 

void setParsFromJson(json & j){

  OSCTbase = j["OSCTbase"]["value"]; 
  agarfreq = j["agarfreq"]["value"];
  AvgSpeed = j["AvgSpeed"]["value"];

}
void addParsToJson(json & j) const {
  j["OSCTbase"]["value"] = OSCTbase;
  j["agarfreq"]["value"] = agarfreq;
  j["AvgSpeed"]["value"] = AvgSpeed;
  
}
void show() const {cout << "agar pars " << OSCTbase  << " " << agarfreq << " " << AvgSpeed << endl;}

};


class Evolparameters : virtual public AgarPars
{
public:
Evolparameters(int argc, const char* argv[], shared_ptr<EvolvableS> & evol1_, string evotype_);
Evolparameters(shared_ptr<const CmdArgs> cmd, shared_ptr<EvolvableS> & evol1_, string evotype_);


int dbunit = 0;
int vbunit = 0;
void setParsFromJson(json & j){
  dbunit = j["dbunit"]["value"]; vbunit = j["vbunit"]["value"];
  AgarPars::setParsFromJson(j);
}
void addParsToJson(json & j) const {
  j["dbunit"]["value"] = dbunit; j["vbunit"]["value"] = vbunit;
  AgarPars::addParsToJson(j);

  

}
};



class EvolparametersCE : virtual public AgarPars   //: public W2DCEpars
{
public:
EvolparametersCE(){}
EvolparametersCE(int argc, const char* argv[]);
EvolparametersCE(shared_ptr<const CmdArgs> cmd);

void setPars(shared_ptr<const CmdArgs> cmd);

int doReverse = 0;
int fitType = 0;
int zeroGainsType = 1;

void setParsFromJson(json & j){
  doReverse =  j["doReverse"]["value"];
  fitType = j["fitType"]["value"];
  zeroGainsType = j["zeroGainsType"]["value"];
  AgarPars::setParsFromJson(j);
}
void addParsToJson(json & j) const {
  j["doReverse"]["value"] = doReverse;
  j["fitType"]["value"] = fitType;
  j["zeroGainsType"]["value"] = zeroGainsType;
  AgarPars::addParsToJson(j);
}

void show() const {cout << " eparsCE doReverse " <<  doReverse << endl; AgarPars::show();}

};




class EvolparametersCER : public EvolparametersCE, public Evolparameters
{
public:
EvolparametersCER(int argc, const char* argv[], shared_ptr<EvolvableS> & evol1_, string evotype_):
Evolparameters(argc,argv,evol1_,evotype_),EvolparametersCE(argc,argv),AgarPars(argc,argv){}
EvolparametersCER(shared_ptr<const CmdArgs> cmd, shared_ptr<EvolvableS> & evol1_, string evotype_):
Evolparameters(cmd,evol1_,evotype_),EvolparametersCE(cmd),AgarPars(cmd){}

void setPars(shared_ptr<const CmdArgs> cmd){
  EvolparametersCE::setPars(cmd);
  Evolparameters::setPars(cmd);
}

void setParsFromJson(json & j){
  EvolparametersCE::setParsFromJson(j);
  Evolparameters::setParsFromJson(j);

  //doReverse =  j["doReverse"]["value"];
  //dbunit = j["dbunit"]["value"]; vbunit = j["vbunit"]["value"];
  //AgarPars::setParsFromJson(j);
}

void addParsToJson(json & j) const {
  EvolparametersCE::addParsToJson(j);
  Evolparameters::addParsToJson(j);

  //j["doReverse"]["value"] = doReverse;
  //j["dbunit"]["value"] = dbunit; j["vbunit"]["value"] = vbunit;
  //AgarPars::addParsToJson(j);
}

};
 




class W2DCEparsA : public W2Dbaseparameters
{
public:
W2DCEparsA(){}
W2DCEparsA(int argc, const char* argv[]);
//W2DCEparsA(shared_ptr<const CmdArgs> cmd);

void setPars(shared_ptr<const CmdArgs> cmd);

double AVA_output = 0, AVB_output = 0;
double AB_output_level = 1;

void show() const {cout << 
  " AVA_output_level "  << AB_output_level << " AVA_output " << 
  AVA_output << " AVB_output " << AVB_output << " randInitState " << randomInitialState << endl;}


void setParsFromJson(json & j){
  AB_output_level = getJsonVal<double>(j, "AB_output_level", AB_output_level, true);
  //AB_output_level = j["AB_output_level"]["value"];
  AVA_output = j["AVA_output"]["value"]; 
  AVB_output = j["AVB_output"]["value"]; 
  W2Dbaseparameters::setParsFromJson(j);
  
}

void addParsToJson(json & j) const {
  j["AB_output_level"]["value"] = AB_output_level;
  j["AVA_output"]["value"] = AVA_output; 
  j["AVB_output"]["value"] = AVB_output;
  W2Dbaseparameters::addParsToJson(j);
}

};


class W2DCEpars : public W2DCEparsA //, public SRCEpars
{
public:
W2DCEpars(){}
W2DCEpars(int argc, const char* argv[]);
//W2DCEpars(shared_ptr<const CmdArgs> cmd);

void setPars(shared_ptr<const CmdArgs> cmd);

double SREvoBot = 0, SREvoTop = 200;
double SREvoBotA = 0, SREvoTopA = 200;

void show(){ W2DCEparsA::show();}

void setParsFromJson(json & j){
  //assert(0);
  SREvoBot = getJsonVal<double>(j, "SREvoBot", SREvoBot, true);
  SREvoTop = getJsonVal<double>(j, "SREvoTop", SREvoTop, true);
  SREvoBotA = getJsonVal<double>(j, "SREvoBotA", SREvoBotA, true);
  SREvoTopA = getJsonVal<double>(j, "SREvoTopA", SREvoTopA, true);

  //if (j.contains("SREvoBot"))
  //SREvoBot = j["SREvoBot"]["value"];
  
 // assert(0);
  
  W2DCEparsA::setParsFromJson(j);
  //SRCEpars::setParsFromJson(j);
}
void addParsToJson(json & j) const {
  j["SREvoBot"]["value"] = SREvoBot;
  j["SREvoTop"]["value"] = SREvoTop;
  j["SREvoBotA"]["value"] = SREvoBotA;
  j["SREvoTopA"]["value"] = SREvoTopA;

   W2DCEparsA::addParsToJson(j);
   //SRCEpars::addParsToJson(j);
}

};



class SRCEpars : public W2Dparameters
{
public:
SRCEpars();
SRCEpars(shared_ptr<const CmdArgs> cmd);
string sr_type = "None";
int SRForm = 0;
int nsegperstr = 6;
int zeroGainsType = 0;

virtual ~SRCEpars(){}
virtual void setPars(shared_ptr<const CmdArgs> cmd);

void setParsFromJson(json & j){
  sr_type = getJsonVal<string>(j, "SRType" , sr_type, true);
  SRForm = getJsonVal<int>(j, "SRForm" , SRForm , true);
  //sr_type = j["SRType"]["value"]; 
  //SRForm = j["SRForm"]["value"];
  nsegperstr = getJsonVal<int>(j, "SRSegPerSR"  , nsegperstr , true);
  zeroGainsType = getJsonVal<int>(j, "SRZeroGainsType"  , zeroGainsType , true);

  //nsegperstr = j["SRSegPerSR"]["value"];
  //assert(0);
}
void addParsToJson(json & j) const {
  j["SRType"]["value"] = sr_type;
  j["SRForm"]["value"] = SRForm;
  j["SRSegPerSR"]["value"] = nsegperstr;
  j["SRZeroGainsType"]["value"] = zeroGainsType;

}
};

class SRRegpars :  public SRCEpars
{
public:
SRRegpars();//{}
SRRegpars(shared_ptr<const CmdArgs> cmd);
void setPars(shared_ptr<const CmdArgs> cmd);

int offset = 0;

void setParsFromJson(json & j){
  SRCEpars::setParsFromJson(j);
  offset = j["SROffset"]["value"];
}

void addParsToJson(json & j) const {

  SRCEpars::addParsToJson(j);
  j["SROffset"]["value"] = offset;
 
}



};







