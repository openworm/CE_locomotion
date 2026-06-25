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
T getParFromJson1(const json & j, const string & name)
{

  return j[name]["value"];

}

template<class T>
bool getParFromJson1(const json & j, const string & name, T & val)
{

  if (!j.contains(name)) return false;
  val = j.at(name).at("value").get<T>();
  return true;

}

template<class T>
bool getParFromJsonAny(const json & j, const vector<string> & names, T & val)
{

  for (const auto & name : names) {
    if (getParFromJson1<T>(j, name, val)) return true;
  }
  return false;

}

template<class T>
bool getParFromCmdAny(shared_ptr<const CmdArgs> cmd, const vector<string> & names, T & val)
{

  for (const auto & name : names) {
    if (cmd->getArgValT<T>("--" + name, val)) return true;
  }
  return false;

}


class W2Dparameters
{
public:
virtual ~W2Dparameters(){}
virtual void setParsFromJson(const json & j) = 0;
virtual void addParsToJson(json & j) const = 0;
virtual void setPars(shared_ptr<const CmdArgs> cmd) = 0;
virtual void setRootParsFromJson(const json &) {}
virtual void addRootParsToJson(json &) const {}
};





class EvolvableS
{
  public:
  
  virtual void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen) = 0;
  virtual int getVectSize() = 0;
  void setParsFromPhenoNZ(const TVector<double> &pheno);
  virtual void setParsFromPheno(const TVector<double> &pheno) = 0;
  //void setParsFromPhenoAndSet(const TVector<double> &pheno);
  //void setParsFromPhenoAndSet(const vector<double> &pheno);
  virtual void setEvolPars(W2Dparameters & w2par_, string evotype_) = 0;
  //virtual const vector<double> & getCurrentPheno() const {return current_pheno;}
  //virtual void setWormPars(shared_ptr<const CmdArgs> cmd) = 0;
 
  virtual ~EvolvableS(){}
  
  void setParsFromFile(const string & genofilename_);
  void setParsFromGeno(const TVector<double> &geno);
  void setParsFromPhenGen(const TVector<double> &phengen, const bool & isPheno);
  //void callEfcond(const json & j1_);
  //Efunctor itsEf;
  vector<double> current_pheno;
  void callEfcond();

  void setCurrentPheno(const vector<double> & pheno1){
     
    current_pheno.clear();
    for (int i=0;i<pheno1.size(); i++) current_pheno.push_back(pheno1[i]);
    //current_pheno = pheno1;
  
  }

  void setCurrentPheno(const TVector<double> &pheno);
  virtual const vector<double> & getCurrentPheno(){return current_pheno;}

  private:
  //void setParsFromPheno(const vector<double> &pheno);

};


class gradEvoPars : virtual public W2Dparameters
{
public:

gradEvoPars(shared_ptr<const CmdArgs> cmd);

void setParsFromJson(const json &) {}
void addParsToJson(json & j) const {
  j.erase("hs_step_size");
  j.erase("HSStepSize");
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
  getParFromJsonAny<bool>(j, {"random_initial_state", "randomInitialState"}, randomInitialState);
}

void addParsToJson(json & j) const {
  j["random_initial_state"]["value"] = randomInitialState;
}

void setPars(shared_ptr<const CmdArgs> cmd);

};

class W2Dbaseparameters : public W2DbaseparametersNML
{

public:
W2Dbaseparameters(){}
W2Dbaseparameters(int argc, const char* argv[]);
//W2Dbaseparameters(shared_ptr<const CmdArgs> cmd);

bool doOrigSRInput = true;
bool doOrigMuscInput = true;

void setParsFromJson(const json & j){
  getParFromJsonAny<bool>(j, {"do_orig_musc_input", "doOrigMuscInput"}, doOrigMuscInput);
  getParFromJsonAny<bool>(j, {"do_orig_sr_input", "doOrigSRInput"}, doOrigSRInput);
  W2DbaseparametersNML::setParsFromJson(j);
}

void addParsToJson(json & j) const {

  //cout << "osmout " << doOrigSRInput << " " << doOrigMuscInput << endl;
  //assert(0);
  j["do_orig_musc_input"]["value"] = doOrigMuscInput;
  j["do_orig_sr_input"]["value"] = doOrigSRInput;
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



  getParFromJsonAny<double>(j, {"osc_tbase", "OSCTbase"}, OSCTbase);
  getParFromJson1<double>(j,"agarfreq",agarfreq);
  getParFromJsonAny<double>(j, {"avg_speed", "AvgSpeed"}, AvgSpeed);

  //OSCTbase = j["OSCTbase"]["value"]; 
  //agarfreq = j["agarfreq"]["value"];
  //AvgSpeed = j["AvgSpeed"]["value"];

}
void addParsToJson(json & j) const {
  j["osc_tbase"]["value"] = OSCTbase;
  j["agarfreq"]["value"] = agarfreq;
  j["avg_speed"]["value"] = AvgSpeed;
  
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
  getParFromJson1<int>(j, "dbunit", dbunit);
  getParFromJson1<int>(j, "vbunit", vbunit);
  AgarPars::setParsFromJson(j);
}
void addParsToJson(json & j) const {
  j["dbunit"]["value"] = dbunit; j["vbunit"]["value"] = vbunit;
  AgarPars::addParsToJson(j);

  

}

void setPars(shared_ptr<const CmdArgs> cmd){
  getParFromCmdAny<int>(cmd, {"dbunit"}, dbunit);
  getParFromCmdAny<int>(cmd, {"vbunit"}, vbunit);
  AgarPars::setPars(cmd);
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
  
  getParFromJsonAny<int>(j, {"do_reverse", "doReverse"}, doReverse);
  getParFromJsonAny<int>(j, {"fit_type", "fitType"}, fitType);
  getParFromJsonAny<int>(
      j, {"sr_zero_gains_type", "SRZeroGainsType"}, zeroGainsType);
  getParFromJsonAny<int>(j, {"do_angle_diff", "doAngleDiff"}, doAngleDiff);

  AgarPars::setParsFromJson(j);
}
void setRootParsFromJson(const json & j) {
  if (j.contains("stretch_receptor"))
    getParFromJsonAny<int>(
      j.at("stretch_receptor"),
      {"sr_zero_gains_type", "SRZeroGainsType"},
      zeroGainsType
    );
  else if (j.contains("Stretch receptor"))
    getParFromJsonAny<int>(
      j.at("Stretch receptor"),
      {"sr_zero_gains_type", "SRZeroGainsType"},
      zeroGainsType
    );
}
void addParsToJson(json & j) const {
  if (!j.is_object()) j = json::object();
  j.erase("zeroGainsType");
  j.erase("zero_gains_type");
  j.erase("sr_zero_gains_type_evo");
  j.erase("SRZeroGainsTypeEvo");
  j["do_reverse"]["value"] = doReverse;
  j["fit_type"]["value"] = fitType;
   j["do_angle_diff"]["value"] = doAngleDiff;
  AgarPars::addParsToJson(j);
}
void addRootParsToJson(json & j) const {
  if (
      j.contains("stretch_receptor")
      && j.at("stretch_receptor").is_object())
  {
    j["stretch_receptor"].erase("sr_zero_gains_type_evo");
    j["stretch_receptor"].erase("SRZeroGainsTypeEvo");
    j["stretch_receptor"]["sr_zero_gains_type"]["value"] =
      zeroGainsType;
  }
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

  double orient_orig = 0, gradSteep = 0.5, RunDuration = 1000,
  MaxDist = 4.5, worm_rotation = 0.0;
  //double orient_orig = 0, gradSteep = 0.5, RunDuration = 100,  MaxDist = 4.5;
  int taxis = 1, kinesis = 0;
	bool resetAgentBody = false;

void setParsFromJson(const json & j){

  //assert(0);
  getParFromJsonAny<bool>(j, {"reset_agent_body", "resetAgentBody"}, resetAgentBody);
  worm_rotation = getJsonVal<double>(j, "rotation", worm_rotation , true);
  orient_orig = getJsonVal<double>(j, "orient", orient_orig, true);
  getParFromJsonAny<double>(j, {"grad_steep", "gradSteep"}, gradSteep);
  getParFromJsonAny<double>(j, {"run_duration", "RunDuration"}, RunDuration);
  getParFromJsonAny<double>(j, {"max_dist", "MaxDist"}, MaxDist);
  taxis = getJsonVal<int>(j, "taxis", taxis,true); 
  kinesis = getJsonVal<int>(j, "kinesis", kinesis,true); 

  W2Dbaseparameters::setParsFromJson(j);

 /*  worm_rotation = j["rotation"]["value"];
  orient_orig = j["orient"]["value"]; 
  gradSteep = j["gradSteep"]["value"];
  RunDuration = j["RunDuration"]["value"];
  HSStepSize = j["HSStepSize"]["value"];
  taxis = j["taxis"]["value"];
  kinesis = j["kinesis"]["value"];
  MaxDist = j["MaxDist"]["value"]; */

}

void addParsToJson(json & j) const {

  addParsToJson1<double>(j,{"orient", "grad_steep", "run_duration",
    "max_dist", "rotation"},
    {orient_orig,gradSteep,RunDuration, MaxDist, worm_rotation});

  addParsToJson1<int>(j,{"taxis", "kinesis"}, {taxis,kinesis});
  addParsToJson1<bool>(j,{"reset_agent_body"}, {resetAgentBody});
  j.erase("hs_step_size");
  j.erase("HSStepSize");

  W2Dbaseparameters::addParsToJson(j);

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
  getParFromJsonAny<double>(j, {"ab_output_level", "AB_output_level"}, AB_output_level);
  //AB_output_level = j["AB_output_level"]["value"];
  getParFromJsonAny<double>(j, {"ava_output", "AVA_output"}, AVA_output);
  getParFromJsonAny<double>(j, {"avb_output", "AVB_output"}, AVB_output);
  W2Dbaseparameters::setParsFromJson(j);
  
}

void addParsToJson(json & j) const {
  j["ab_output_level"]["value"] = AB_output_level;
  j["ava_output"]["value"] = AVA_output;
  j["avb_output"]["value"] = AVB_output;
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
  getParFromJsonAny<double>(j, {"sr_evo_bot", "SREvoBot"}, SREvoBot);
  getParFromJsonAny<double>(j, {"sr_evo_top", "SREvoTop"}, SREvoTop);
  getParFromJsonAny<double>(j, {"sr_evo_bot_a", "SREvoBotA"}, SREvoBotA);
  getParFromJsonAny<double>(j, {"sr_evo_top_a", "SREvoTopA"}, SREvoTopA);

  //if (j.contains("SREvoBot"))
  //SREvoBot = j["SREvoBot"]["value"];
  
 // assert(0);
  
  W2DCEparsA::setParsFromJson(j);
  //SRCEpars::setParsFromJson(j);
}
void addParsToJson(json & j) const {
  j["sr_evo_bot"]["value"] = SREvoBot;
  j["sr_evo_top"]["value"] = SREvoTop;
  j["sr_evo_bot_a"]["value"] = SREvoBotA;
  j["sr_evo_top_a"]["value"] = SREvoTopA;

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
  getParFromJsonAny<string>(j, {"sr_type", "SRType"}, sr_type);
  getParFromJsonAny<int>(j, {"sr_form", "SRForm"}, SRForm);
  //sr_type = j["SRType"]["value"]; 
  //SRForm = j["SRForm"]["value"];
  getParFromJsonAny<int>(j, {"sr_seg_per_sr", "SRSegPerSR"}, nsegperstr);
  getParFromJsonAny<int>(j, {"sr_zero_gains_type", "SRZeroGainsType"}, zeroGainsType);

  //nsegperstr = j["SRSegPerSR"]["value"];
  //assert(0);
}
void addParsToJson(json & j) const {
  j["sr_type"]["value"] = sr_type;
  j["sr_form"]["value"] = SRForm;
  j["sr_seg_per_sr"]["value"] = nsegperstr;
  j["sr_zero_gains_type"]["value"] = zeroGainsType;

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
  getParFromJsonAny<int>(j, {"sr_offset", "SROffset"}, offset);
}

void addParsToJson(json & j) const {

  SRCEpars::addParsToJson(j);
  j["sr_offset"]["value"] = offset;
 
}



};
