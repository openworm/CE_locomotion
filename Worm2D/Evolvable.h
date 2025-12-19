//#include "../TSearch.h"
//#include "../VectorMatrix.h"
#include "jsonUtils.h"



template<class T>
void addParsToJson1(json & j, const vector<string> & names, const vector<T> & vals)
{

  assert(names.size() == vals.size());

  for (int i=0; i<names.size(); i++) j[names[i]]["value"] = vals[i];

}

template<class T>
T getParFromJson1(json & j, const string & name)
{

  return j[name]["value"];

}


class W2Dparameters
{
public:
virtual ~W2Dparameters(){}
virtual void setParsFromJson(const json & j) = 0;
virtual void addParsToJson(json & j) const = 0;
virtual void setPars(shared_ptr<const CmdArgs> cmd) = 0;
};





class EvolvableS
{
  public:
  
  virtual void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen) = 0;
  virtual int getVectSize() = 0;
  virtual void setParsFromPheno(const TVector<double> &pheno) = 0;
  virtual void setEvolPars(W2Dparameters & w2par_, string evotype_) = 0;
  virtual void setWormPars(shared_ptr<const CmdArgs> cmd) = 0;

  virtual ~EvolvableS(){}
  
  void setParsFromFile(const string & genofilename_);
  void setParsFromGeno(const TVector<double> &geno);
  void setParsFromPhenGen(const TVector<double> &phengen, const bool & isPheno);

};


class gradEvoPars : virtual public W2Dparameters
{
public:

gradEvoPars(shared_ptr<const CmdArgs> cmd);

double HSStepSize = 0.01;

void setParsFromJson(const json & j){

  HSStepSize = j["HSStepSize"]["value"];
  
}
void addParsToJson(json & j) const {

  addParsToJson1<double>(j,{"HSStepSize"},{HSStepSize});

}

virtual void setPars(shared_ptr<const CmdArgs> cmd);
};

class W2DbaseparametersNML : virtual public W2Dparameters
{

public:
W2DbaseparametersNML(){}
//W2DbaseparametersNML(int argc, const char* argv[]);
//W2Dbaseparameters(shared_ptr<const CmdArgs> cmd);
bool randomInitialState = false;


void setParsFromJson(const json & j){
  randomInitialState = j["randomInitialState"]["value"];
}

void addParsToJson(json & j) const {
  j["randomInitialState"]["value"] = randomInitialState;
}

void setPars(shared_ptr<const CmdArgs> cmd);

};

class W2Dbaseparameters : public W2DbaseparametersNML
{

public:
W2Dbaseparameters(){}
W2Dbaseparameters(int argc, const char* argv[]);
//W2Dbaseparameters(shared_ptr<const CmdArgs> cmd);

bool doOrigSRInput = 1;
bool doOrigMuscInput = 1;

void setParsFromJson(const json & j){
  doOrigMuscInput = j["doOrigMuscInput"]["value"];
  doOrigSRInput = j["doOrigSRInput"]["value"];
  W2DbaseparametersNML::setParsFromJson(j);
}

void addParsToJson(json & j) const {
  j["doOrigMuscInput"]["value"] = doOrigMuscInput;
  j["doOrigSRInput"]["value"] = doOrigSRInput;
  W2DbaseparametersNML::addParsToJson(j);
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

void setParsFromJson(const json & j){

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
void setParsFromJson(const json & j){
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
int doAngleDiff = 0;

void setParsFromJson(const json & j){
  doReverse =  j["doReverse"]["value"];
  fitType = j["fitType"]["value"];
  zeroGainsType = j["zeroGainsType"]["value"];
  doAngleDiff = j["doAngleDiff"]["value"];
  AgarPars::setParsFromJson(j);
}
void addParsToJson(json & j) const {
  j["doReverse"]["value"] = doReverse;
  j["fitType"]["value"] = fitType;
  j["zeroGainsType"]["value"] = zeroGainsType;
   j["doAngleDiff"]["value"] = doAngleDiff;
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

void setParsFromJson(const json & j){
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

 ///////////////////////
/////////////////////
/////////////////// worm parameters
///////////////////////


class gradParameters : public W2Dbaseparameters
{

  public:
  gradParameters(){}
  
  void setPars(shared_ptr<const CmdArgs> cmd);

  double orient_orig = 0, gradSteep = 0.5, RunDuration = 1000, HSStepSize = 0.01, 
  MaxDist = 4.5, worm_rotation = 0.0;
  //double orient_orig = 0, gradSteep = 0.5, RunDuration = 100,  MaxDist = 4.5;
  int taxis = 1, kinesis = 0;
	
  void setParsFromJson(const json & j){

  worm_rotation = j["rotation"]["value"];
  orient_orig = j["orient"]["value"]; 
  gradSteep = j["gradSteep"]["value"];
  RunDuration = j["RunDuration"]["value"];
  HSStepSize = j["HSStepSize"]["value"];
  taxis = j["taxis"]["value"];
  kinesis = j["kinesis"]["value"];
  MaxDist = j["MaxDist"]["value"];
}

void addParsToJson(json & j) const {

  addParsToJson1<double>(j,{"orient", "gradSteep", "RunDuration", 
    "HSStepSize", "MaxDist", "rotation"},
    {orient_orig,gradSteep,RunDuration, HSStepSize, 
      MaxDist, worm_rotation});

  addParsToJson1<int>(j,{"taxis", "kinesis"}, {taxis,kinesis});

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


void setParsFromJson(const json & j){
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

void setParsFromJson(const json & j){
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

/* class SR18pars : public W2Dparameters
{
public:
SR18pars();
SR18pars(shared_ptr<const CmdArgs> cmd);

int NSEGSSR = 6;                    // Number of segments that go into a stretch receptor
double SRvncgain = srvncgain;                // Stretch receptor gain
double SRheadgain = srheadgain;                // Stretch receptor gain

int NSEGSHEADSTART = 7;             // 7-12
int NSEGSHEAD = 14;                 // Number of segments for the sublateral head motorneurons
int NSEGSVNCSTART = 7;              // Segment where VNC starts

};
 */


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

void setParsFromJson(const json & j){
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

void setParsFromJson(const json & j){
  SRCEpars::setParsFromJson(j);
  offset = j["SROffset"]["value"];
}

void addParsToJson(json & j) const {

  SRCEpars::addParsToJson(j);
  j["SROffset"]["value"] = offset;
 
}



};







