#include "../TSearch.h"
#include "../VectorMatrix.h"
#include "jsonUtils.h"

class CmdArgs {
    vector<string> args;
public:
    CmdArgs(int argc, const char* argv[]) 
        : args(argv, argv + argc) 
        {
          if (((argc-1) % 2) != 0)
         {cout << "The arguments are not configured correctly." << endl;exit(1);}
        }

    //int size() const { return static_cast<int>(args.size()); }

    //const string& operator[](int i) const { return args[i]; }

    //const vector<string>& all() const { return args; }

    const string getArgVal(const string & str, const  string & defaultstr) const
    { 
      for (int i = 1; i<args.size(); i+=2)
      //for (int i =0;i<args.size();i++)
        if (args[i]==str) return args[i+1];
      return defaultstr;
    }

    const int getArgVal(const string & str) const
    {
      for (int i = 1; i<args.size(); i+=2)
      //for (int i=0;i<args.size();i++)
        if (args[i]==str) return i;
      return -1;
    }

    const double getArgValDoub(const string & str, const double & val) const
    {
      const int arg = getArgVal(str);
      if (arg==-1) return val;
      return stod(args[arg+1].c_str());
    }

    const int getArgValInt(const string & str, const int & val) const
    {
      const int arg = getArgVal(str);
      if (arg==-1) return val;
      return stoi(args[arg+1].c_str());
    }

    const long getArgValLong(const string & str, const long & val) const
    {
      const int arg = getArgVal(str);
      if (arg==-1) return val;
      return stol(args[arg+1].c_str());
    }


    /* const double getArgValLong(const string & str, const string & defaultstr) const
    {
      return stol(getArgVal(str,defaultstr).c_str());
    }

    const double getArgValInt(const string & str, const string & defaultstr) const
    {
      return stoi(getArgVal(str,defaultstr).c_str());
    } */

};


struct evoParsNonConst{
string filePrefix;

};

struct evoPars{
   string directoryName;
   long randomseed;
   TSelectionMode SelectionMode;
   TReproductionMode ReproductionMode;
   int PopulationSize;
   int MaxGenerations;
   double MutationVariance;
   double CrossoverProbability;
   TCrossoverMode CrossoverMode;
   double MaxExpectedOffspring;
   double ElitistFraction;
   int SearchConstraint;
   int CheckpointInterval;
   bool ReEvaluationFlag;
   int skip_steps;
   // Integration parameters
   double Duration;       //
   double Transient;    //
   double StepSize;
   int N_curvs ;
   int VectSize ;
   string fileprefix  ;
   string evoType ;
   
  void addParsToJson(json &j) const
  {

    {vector<string> names = {"MutationVariance", "CrossoverProbability", 
         "MaxExpectedOffspring", "ElitistFraction",
         "Duration", "Transient", "StepSize"};
    vector<double> vals = {MutationVariance, CrossoverProbability, 
         MaxExpectedOffspring, ElitistFraction,
         Duration, Transient, StepSize};
    for (int i=0;i<names.size();i++) j[names[i]]["value"]=vals[i];}

    {vector<string> names = {"randomseed", "SelectionMode", "ReproductionMode", 
         "PopulationSize", "MaxGenerations", "CrossoverMode", "SearchConstraint", 
         "CheckpointInterval", "ReEvaluationFlag", "skip_steps", "N_curvs", "VectSize"};
    vector<int> vals = {(int) randomseed, SelectionMode, ReproductionMode, 
         PopulationSize, MaxGenerations, CrossoverMode, SearchConstraint, CheckpointInterval, 
         ReEvaluationFlag, skip_steps, N_curvs, VectSize};
    for (int i=0;i<names.size();i++) j[names[i]]["value"]=vals[i];}

      {vector<string> names = {"fileprefix", "evoType"};
      vector<string> vals = {fileprefix, evoType};
       for (int i=0;i<names.size();i++) j[names[i]]["value"]=vals[i];}


  }

   const doubIntParamsHead getParams() const
   {
       doubIntParamsHead var1;
       var1.parDoub.head = "Evolutionary Optimization Parameters";
       var1.parInt.head = "Evolutionary Optimization Parameters";
       var1.parDoub.names = 
       {"MutationVariance", "CrossoverProbability", 
         "MaxExpectedOffspring", "ElitistFraction",
         "Duration", "Transient", "StepSize"};
       var1.parDoub.vals = {MutationVariance, CrossoverProbability, 
         MaxExpectedOffspring, ElitistFraction,
         Duration, Transient, StepSize};

       var1.parInt.names = {"randomseed", "SelectionMode", "ReproductionMode", 
         "PopulationSize", "MaxGenerations", "CrossoverMode", "SearchConstraint", 
         "CheckpointInterval", "ReEvaluationFlag", "skip_steps", "N_curvs", "VectSize"};
       var1.parInt.vals = {randomseed, SelectionMode, ReproductionMode, 
         PopulationSize, MaxGenerations, CrossoverMode, SearchConstraint, CheckpointInterval, 
         ReEvaluationFlag, skip_steps, N_curvs, VectSize};

       return var1;
   }


  //void setFromArgs(int argc, const char* argv[]);
  //string rename_file(string filename);



string rename_file(string filename){return directoryName + "/" + 
    fileprefix + filename;}


void setFromArgs(shared_ptr<const CmdArgs> cmd)
{

bool seed_flag = 1;

MaxGenerations = cmd->getArgValInt("--maxgens", MaxGenerations);
MutationVariance = cmd->getArgValDoub("--MutVar", MutationVariance);
CrossoverProbability = cmd->getArgValDoub("--CrossProb", CrossoverProbability);
directoryName = cmd->getArgVal("--folder","HJUYGYT");
struct stat sb;
if (stat(directoryName.c_str(), &sb) != 0) 
{cout << "Directory doesn't exist." << endl;exit(1);}

PopulationSize = cmd->getArgValInt("-p",PopulationSize);
CheckpointInterval = cmd->getArgValInt("-cpt", CheckpointInterval);
Duration = cmd->getArgValDoub("-d", Duration);
Transient = cmd->getArgValDoub("-t", Transient);
evoType = cmd->getArgVal("--evoType", evoType);

if (seed_flag){ 
  long randomseed1 = cmd->getArgValLong("-R",-1);
  if (randomseed1!=-1)
  {seed_flag = 0;randomseed = randomseed1;}
}

if (seed_flag){ 
  long randomseed1 = cmd->getArgValLong("-r",-1);
  if (randomseed1!=-1)
  {seed_flag = 0;randomseed = randomseed1 + static_cast<long>(time(NULL));}
}


}

void setFromArgs(int argc, const char* argv[])
{

  if (((argc-1) % 2) != 0)
     {
      cout << "The arguments are not configured correctly." << endl;
      exit(1);
    }
    
    bool seed_flag = 1;

    for (int arg = 1; arg<argc; arg+=2)
    { 
    if (strcmp(argv[arg],"--maxgens")==0) MaxGenerations = stoi(argv[arg+1]);

    if (strcmp(argv[arg],"--MutVar")==0) MutationVariance = stod(argv[arg+1]);
    if (strcmp(argv[arg],"--CrossProb")==0) CrossoverProbability = stod(argv[arg+1]);

    //if (strcmp(argv[arg],"--dorandinit")==0) simRandomInit = stol(argv[arg+1]);
    //if (strcmp(argv[arg],"--skipOrigSim")==0) skipOrigSim = stol(argv[arg+1]);
    //if (strcmp(argv[arg],"--donml")==0) do_nml = stol(argv[arg+1]);


    if (strcmp(argv[arg],"--folder")==0) {
      directoryName= argv[arg+1];
      struct stat sb;
      if (stat(directoryName.c_str(), &sb) != 0) 
      {cout << "Directory doesn't exist." << endl;exit(1);}
    }

    if (seed_flag){ 
    if (strcmp(argv[arg],"-R")==0) randomseed = stol(argv[arg+1]);
    if (strcmp(argv[arg],"-r")==0) 
    {long randomseed1 = static_cast<long>(time(NULL));
           randomseed = randomseed1 + stol(argv[arg+1]);
    }
    seed_flag = 0;
    }
    //if (strcmp(argv[arg], "--modelname")==0) model_name = argv[arg+1];
    if (strcmp(argv[arg],"-p")==0) PopulationSize = stoi(argv[arg+1]);
    if (strcmp(argv[arg],"-d")==0) Duration = stod(argv[arg+1]);
    if (strcmp(argv[arg],"-t")==0) Transient = stod(argv[arg+1]);
    if (strcmp(argv[arg],"-cpt")==0) CheckpointInterval = stoi(argv[arg+1]);
    if (strcmp(argv[arg],"--evoType")==0) evoType = (string) argv[arg+1];
    

    //cout << "doCPT " << doCPT << endl;
    //if (strcmp(argv[arg],"--nervous")==0) nervousSystemNameForSim = argv[arg+1];
    
    }

   

}

};


/* class EvaluationFcn
{
public:
virtual double EvaluationFunction(TVector<double> &v, RandomState &rs) = 0;
virtual evoPars getDefaultEvoPars() = 0;
const evoPars * ep_ptr;
};

template<class T>
class Evaluation21 : EvaluationFcn
{
public:
double EvaluationFunction(TVector<double> &v, RandomState &rs);
evoPars getDefaultEvoPars();
}; */

/* template<typename Derived>
class Evolvable
{
  public:
  //Evolvable():ep_ptr(nullptr){}
  void itsGenPhenMapping(TVector<double> &gen, TVector<double> &phen) 
  {return Derived::GenPhenMapping(gen,phen);}
  
  //virtual void writeJson(TVector<double> &) = 0;
  //virtual evoPars getDefaultEvoPars() = 0;
  int itsVectSize() {return Derived::getVectSize();};
  virtual ~Evolvable(){}

  friend class EvolutionFull;
  protected:
  //virtual double EvaluationFunction(TVector<double> &v, RandomState &rs) = 0;
  //virtual double EvaluationFunction(TVector<double> &v, RandomState &rs) = 0;
  //const evoPars * ep_ptr;
}; */

class W2Dparameters
{
public:
virtual ~W2Dparameters(){}
virtual void setParsFromJson(json & j){}
virtual void addParsToJson(json & j) const {}
};




class EvolvableS
{
  public:
  
  virtual void GenPhenMapping(TVector<double> &gen, TVector<double> &phen) = 0;
  virtual int getVectSize() = 0;
  //static int getVectSize();
  virtual ~EvolvableS(){}
  virtual void setParsFromPheno(TVector<double> &pheno) = 0;
  void setParsFromFile(const string & genofilename_);
  void setParsFromGeno(TVector<double> &geno);
  virtual void setEvolPars(W2Dparameters & w2par_, string evotype_) = 0;
  //virtual W2Dparameters & getWormPars() {return;}
  
  //virtual void setWormPars(const W2Dparameters * w2par_) = 0; 
  //{
  //return this->setWormPars(w2par_);
  //}

  //{ return T::setWormPars(w2par_);}

  //virtual void setWormPars(int argc, const char* argv[]) {assert(0);}
  //virtual shared_ptr<const W2Dparameters> getWormPars() {return nullptr;}
  
  //virtual shared_ptr<const W2Dparameters> setWormPars(int argc, const char* argv[]) = 0; //{return nullptr;}

  //{return T::setWormPars(argc,argv);}

  //virtual shared_ptr<const W2Dparameters> setWormPars(int argc, const char* argv[]) = 0;
  //{return this->setWormPars(argc,argv);}
  
  //virtual shared_ptr<const W2Dparameters> setWormPars(shared_ptr<const CmdArgs> cmd) = 0;
  virtual void setWormPars(shared_ptr<const CmdArgs> cmd) = 0;

  //shared_ptr<W2Dparameters> evolvable_w2par_ptr;
  //Evolparameters & Epars1;
  //friend class EvolutionFull;
  //protected:
  //EvolvableS(shared_ptr<W2Dparameters> w2par_ptr_);

  //protected:
  //shared_ptr<W2Dparameters> evolvable_worm_pars_ptr;
};


/* template<typename T>
class EvolvableST : public EvolvableS
{
  public:
void setWormPars(const W2Dparameters * w2par_) override { return T::setWormPars(w2par_);}
shared_ptr<const W2Dparameters> setWormPars(int argc, const char* argv[]) override 
{return T::setWormPars(argc,argv);}
}; */

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






double getParameterDouble(int argc, const char* argv[], string parName, const string defaultval);
long getParameterLong(int argc, const char* argv[], string parName, const string defaultval);
int getParameterInt(int argc, const char* argv[], string parName, const string defaultval);
string getParameterString(int argc, const char* argv[], string parName, const string defaultval);

string rename_file(const string & filename, const string & directoryName, const string & fileprefix = "");
bool directoryExists(const string & directoryName);



//const char* getParameter(int argc, const char* argv[], string parName, const char* defaultval);
