//#include "Worm2D.h"
#include "Worm2DSR.h"
#include <iomanip>

//using json = nlohmann::json;

string main_directoryname, main_modelname;








void Worm2Dbase::addPhenoName(string name, int k)
{phenoNamesNums.push_back(k);phenoNames.push_back(name);}




vector<double> Worm2Dbase::readPhenotype()
{
    cout << "readPhenotype" << endl;
    vector<double> vec1;
    double val;
    //string basename = main_directoryname;
    //string modelname = main_modelname;

    //cout << "ksks " << basename << " " << getModelName() << endl;
    string filename = main_directoryname + "/" + main_modelname + "_best.pheno.dat";
    //string filename = basename + "/" + getModelName() + "_best.gen.dat";
    cout << filename << endl;
    //throw std::exception();
    //exit(0);

    
    ifstream infile(filename);
    
    while(infile >> val) vec1.push_back(val);
    //TVector<double> phenovec = getTVector<double>(vec1);
    infile.close();
    //for (int i=0; i<vec1.size();i++) cout << vec1[i] << " ";
    //cout << endl;
    return vec1;

}


vector<toFromWeight> dummyVec()
{
   toFromWeight tv({0,0},0);
   vector<toFromWeight> vec1;
    vec1.push_back(tv);
    return vec1;
}


size_t DataWriter::getPos(string name_)
{

    string filename = getName(name_);
    for (size_t i=0;i<ofnames.size();i++)
    if (ofnames[i]==filename) return i;
    ofnames.push_back(filename);
    ofsvec.push_back(ofstream(filename));
    isOpen.push_back(true);
    tts.push_back(dataskips);
    return ofnames.size()-1;

}


void DataWriter::closeAll()
{
    for (int i=0; i<ofsvec.size(); i++) 
    ofsvec[i].close(); 
    isOpen.clear();
    ofsvec.clear();
    ofnames.clear();
    tts.clear();
}

    
string DataWriter::getName(string name_){
        if (prefix!="") return basename + "/" + prefix + "_" + name_;
        else return basename + "/" + name_;
 }

void  DataWriter::InitializeData(string basename_)
{
    //cout << "DataWriter init state" << endl;
    setBasename(basename_);
    //if (doFirstCall) dataReset();
}

void DataWriter::dataReset(){closeAll();
//    writeDataCheck();
}


double Efunctor::eFunc(const double & val, const json & j, bool setItsJson)
{

  //cout << "eFunc " << " " << val << endl;
  //cout << j << endl;


  //if (setItsJson) itsJson["f_ind"] =  j.at("f_ind").get<int>();

  if (j.at("f_ind").get<int>() == 1) 
  {
    const bool scheduled =
      itsJson.contains("functions")
      && itsJson.at("functions").contains(to_string(1));
    const bool cond1 =
      scheduled
      || (itsJson.contains("f_ind") && itsJson.at("f_ind").get<int>() == 1)
      || setItsJson;
    const bool cond2 = condf && bp.BPitsJson.at("Funcable").contains(to_string(1));

    if (!(cond1 || cond2)) return val;
    if (j.contains("doInverse") && j.at("doInverse") == true) 
    return val / j.at("fact").get<double>();

    //cout << "hsh " << cond1 << " " << cond2 << " " << j.at("fact").get<double>() << endl;
    //if (!setItsJson) assert(0);
    return val * j.at("fact").get<double>();
  }

  if (j.at("f_ind").get<int>() == 2) {
   
    if (j.contains("doInverse") && j.at("doInverse") == true) return val;

    const int cond = j.at("cond").get<int>();

    const bool scheduled =
      itsJson.contains("functions")
      && itsJson.at("functions").contains(to_string(2));
    const bool cond1 =
      scheduled
      || (itsJson.contains("f_ind") && itsJson.at("f_ind").get<int>() == 2)
      || setItsJson;
    if (scheduled)
    {
      const json & function =
        itsJson.at("functions").at(to_string(2));
      if (
          function.contains("condval")
          && cond == function.at("condval").get<int>())
        return 0;
    }
    else if (
        cond1
        && itsJson.contains("condval")
        && cond == itsJson.at("condval").get<int>())
      return 0;
 
    const bool cond2 = condf && bp.BPitsJson.at("Funcable").contains(to_string(2));

    if (cond2) 
    {
    json & bpj = bp.BPitsJson.at("Funcable").at(to_string(2));
    if (bpj.contains("condval") && cond == bpj.at("condval").get<int>()) return 0;
    }
      //return val;
    
    return val;
 
    
  }

  assert(0);

}

void Efunctor::setFunctionCondition(
    const int function_index, const bool has_condval, const int condval)
{
  json & function = itsJson["functions"][to_string(function_index)];
  function = json::object();
  if (has_condval) function["condval"] = condval;
}

////////////////////////////
/////////////////////////////



baseConsts Worm2Dbase::makeBaseConsts()
{

    baseConsts b1;
    getValCJWorm<bool>("debug",b1.debug);

    return b1;

}


Worm2Dbase::Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, 
    muscForW2D * m_ptr_, shared_ptr<const CmdArgs> cmd_):
par1(par1_),m_ptr(m_ptr_),n_ptr(n_ptr_), baseParameters(cmd_),
itsEf(*this),baseconsts(makeBaseConsts())
{

    
}

Worm2Dbase::Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, 
    muscForW2D * m_ptr_, shared_ptr<const CmdArgs> cmd_, const json & j):
par1(par1_),m_ptr(m_ptr_),n_ptr(n_ptr_), baseParameters(j,cmd_), InputSwitcher(j),
itsEf(*this),baseconsts(makeBaseConsts())
{
    constructFuncableSchedules(j);
}


//////////////////////


Worm2Dm::Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<const CmdArgs> cmd_):
Worm2Dbase(par1_,n_ptr_,new Muscles(),cmd_),W2Dmparscalled(false),W2Dminitcalled(false)
{//setUpBodyConn();
    }

Worm2Dm::Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<const CmdArgs> cmd_, const json & j):
Worm2Dbase(par1_,n_ptr_,new Muscles(),cmd_, j),W2Dmparscalled(false),W2Dminitcalled(false)
{
    //setUpBodyConn(j);
}



Worm2Dm::Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, bool dumval, shared_ptr<const CmdArgs> cmd_):
Worm2Dbase(par1_,n_ptr_,0, cmd_),
W2Dmparscalled(false),W2Dminitcalled(false){
    m_ptr = new c302muscForW2D(dynamic_cast<c302ForW2D&>(*n_ptr));
    //setUpBodyConn();
}

Worm2Dm::Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, bool dumval, 
    shared_ptr<const CmdArgs> cmd_, const json & j):
Worm2Dbase(par1_,n_ptr_,0, cmd_,j),
W2Dmparscalled(false),W2Dminitcalled(false){
    m_ptr = new c302muscForW2D(dynamic_cast<c302ForW2D&>(*n_ptr));
    //setUpBodyConn(j);
}

////////////////////////////


Worm2D::Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_):
Worm2Dm(par1_, n_ptr_),m(dynamic_cast<Muscles&>(*m_ptr)),
doOrigMuscInput(getValCJWorm<bool>("do_orig_musc_input")),
doOrigSRInput(getValCJWorm<bool>("do_orig_sr_input"))
//,W2Dbaseparameters1(dynamic_pointer_cast<W2Dbaseparameters>(W2Dbaseparameters1b))
{

    //assert(0 && "what is calling this?");
    //cout << "Worm2D const" << endl;
    //cout << "dom " << doOrigMuscInput << " " << doOrigSRInput << endl;
  
    setUp();
    //setUpBodyConn();
}

Worm2D::Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_, bool forceNoOrigInputs):
Worm2Dm(par1_, n_ptr_),m(dynamic_cast<Muscles&>(*m_ptr)),
doOrigMuscInput(forceNoOrigInputs ? false : getValCJWorm<bool>("do_orig_musc_input")),
doOrigSRInput(forceNoOrigInputs ? false : getValCJWorm<bool>("do_orig_sr_input"))
{
    setUp();
}


void Worm2Dbody::InitializeState(RandomState &rs)
{
    //cout << "Worm2Dbody init state" << endl;
    //InitializeBodyConstants();
    b.InitializeBodyState();
    //writeDataCheck();
    return;
}



void Worm2Dbase::InitializeState(RandomState &rs)
{  
    //cout << "Worm2Dbase init state" << endl;
    //setTime(0);

   // shared_ptr<W2DbaseparametersNML> l1 = dynamic_pointer_cast<W2DbaseparametersNML>(W2Dbaseparameters1b);
    
   
    bool randomInitialState;
    getValCJWorm<bool>("random_initial_state",randomInitialState);

    if (randomInitialState)
    {
        NervousSystem * n = dynamic_cast<NervousSystem*>(n_ptr);
        if (n!=nullptr){

        n->RandomizeCircuitState(-1, 1, rs);
        n->RandomizeCircuitOutput(0.2, 0.8, rs);
        }
    }



    //t = 0.0;
    //datatime =  0.0;
    //writeDataCheck();
    //dataReset();
    return;
}


void Worm2Dm::InitializeState(RandomState &rs)
{
    //if (W2Dminitcalled) {W2Dminitcalled=false;return;}
    //cout << "Worm2Dm init state" << endl;

    Worm2Dbase::InitializeState(rs);
    Worm2Dbody::InitializeState(rs);

    //W2Dminitcalled = true;

    return;
}



void Worm2D::setUp()
{
    m.SetMuscleParams(par1.N_muscles, par1.T_muscle);
    m.InitializeMuscleState();
  
    //InitializeState(rs);
}

void Worm2D::InitializeState(RandomState &rs)
{
    //cout << "Worm2D init state" << endl;
    Worm2Dm::InitializeState(rs);
    m.InitializeMuscleState();
   
    return;
}

int Worm2Dbase::nn(int neuronNumber, int unitNumber) const
{   
    return nn1(neuronNumber,unitNumber,par1.N_neuronsperunit);
    
}

int nn1(int neuronNumber, int unitNumber, int N_neuronsperunit)
{   
    if (unitNumber==1) return neuronNumber;
    return neuronNumber+((unitNumber-1)*N_neuronsperunit);
}


void Worm2Dbody::writeData()
{
writeBody();
writeCurvature();
}

void Worm2Dm::writeData()
{
Worm2Dbase::writeData();
Worm2Dbody::writeData();
}


void Worm2Dbase::writeData()
{
   // cout << "Worm2Dbase write data" << endl;
writeAct();
writeState();
}

void Worm2Dbody::shiftX(double shiftdist_)
{
    shiftdist_=  shiftdist_/100.0;
    for (int i = 1; i <= N_rods; i++) b.X(i)+=shiftdist_;
}

void Worm2Dbody::shiftY(double shiftdist_)
{
    shiftdist_=  shiftdist_/100.0; //change to meters
    for (int i = 1; i <= N_rods; i++) b.Y(i)+=shiftdist_;
}

void Worm2Dbody::ResetAgentsBody(baseParameters & basePar_)

//void Worm2Dbody::ResetAgentsBody(shared_ptr<baseParameters> CO2DSRpars)
//void Worm2Dbody::ResetAgentsBody(shared_ptr<gradParameters> CO2DSRpars)
{
    //assert(0);
    //orient = gradPars->orient_orig;
    
    b.InitializeBodyState();
    bool resetAgentBody;
    basePar_.getValCJWorm<bool>("reset_agent_body",resetAgentBody);

    if (resetAgentBody)
    {
    zeroX();
    zeroY();
    //w18->shiftX(-4.5);
    double orient, MaxDist, worm_rotation;
    basePar_.getValCJWorm<double>("orient", orient);
    basePar_.getValCJWorm<double>("max_dist", MaxDist);
    basePar_.getValCJWorm<double>("rotation", worm_rotation);

    shiftX(cos(orient)*MaxDist*-1);
    shiftY(sin(orient)*MaxDist*-1);
	rotateBody(worm_rotation);
    }

}

void Worm2Dbody::rotateBody(double theta)
{
   
    for (int i = 1; i <= N_rods; i++) {

        double x = (b.X(i)-b.X(Head))*cos(theta) - (b.Y(i)-b.Y(Head))*sin(theta);
        double y = (b.X(i)-b.X(Head))*sin(theta) + (b.Y(i)-b.Y(Head))*cos(theta);
        b.X(i) = b.X(Head) + x;
        b.Y(i) = b.Y(Head) + y;
        b.Phi(i) = b.Phi(i) + theta;
        //b.X(i) = i*L_seg*cos(angle);
        //b.Y(i) = i*L_seg*sin(angle);
        //int i3 = 3*i;
        //Z[i3] = i*L_seg; Z[i3+1] = 0.0; Z[i3+2] = M_PI/2
    }
}

void Worm2Dbody::zeroX()
{
    shiftX(-PositionX());
}

void Worm2Dbody::zeroY()
{
    shiftY(-PositionY());
}

double Worm2Dbody::CoMx()
{
    double temp = 0.0;
    for (int i = 1; i <= N_rods; i++) {
        temp += b.X(i);
    }
    return temp/N_rods;
}

double Worm2Dbody::CoMy()
{
    double temp = 0.0;
    for (int i = 1; i <= N_rods; i++) {
        temp += b.Y(i);
    }
    return temp/N_rods;
}

double Worm2Dbody::headDistanceToCenter() const
{
	//cout << "WormAgent::setDistanceToCentre()" << endl;
	return sqrt(pow(PositionX(),2) + pow(PositionY(),2));
}

double Worm2Dbody::headDistanceToLocation(const double & x, const double & y) const
{
	//cout << "WormAgent::setDistanceToCentre()" << endl;
	return sqrt(pow(PositionX()-x,2) + pow(PositionY()-y,2));
}

wormIzqParams Worm2Dbase::getIzqPars(const json & j)
{
    json worm = getSectionCopyWithLegacy(j, "worm");
    int n_size = 0;
    if (j.contains("nervous_system")
        && j.at("nervous_system").contains("cell_names")
        && j.at("nervous_system").at("cell_names").contains("value"))
    {
        n_size = j.at("nervous_system").at("cell_names").at("value").size();
    }
    else if (j.contains("Nervous system")
        && j.at("Nervous system").contains("size"))
    {
        n_size = j.at("Nervous system").at("size").at("value");
    }
    else
    {
        n_size = worm.at("N_size").at("value");
    }

    return
  {worm["N_neuronsperunit"]["value"], 
    worm["N_muscles"]["value"], 
    worm["T_muscle"]["value"],
    worm["N_units"]["value"],
    n_size
  };
}


double Worm2Dbody::Orientation()
{
    return atan2(b.Y(Head)-b.Y(Tail),b.X(Head)-b.X(Tail));
}

void Worm2Dbody::AngleCurvature(TVector<double> &c)
{
  double dx1,dy1,dx2,dy2,a,a1,a2,seg;
  int k=1;

  for (int i = 3; i < N_segments-1; i+=2)
  {
    dx1 = b.X(i) - b.X(i-2);
    dy1 = b.Y(i) - b.Y(i-2);
    dx2 = b.X(i+2) - b.X(i);
    dy2 = b.Y(i+2) - b.Y(i);

    a1 = atan2(dy1,dx1);
    a2 = atan2(dy2,dx2);

    if (a1 > PI/2 and a2 < -PI/2)
    a = (a1 - 2*PI) - a2;
    else
    if (a1 < -PI/2 and a2 > PI/2)
    a = a1 - (a2 - 2*PI);
    else
    a = a1-a2;
    c(k) = a;
    k++;
  }
}


void Worm2Dbase::Step() 
{ //StepSize = StepSize_; 
    InputSwitcher::updateScheduledInput(t, externalInputs);
    updateScheduledFuncables(t);
    Step1(); 

    incSimTimes();
    //datatime = t;
    //datatime += StepSize_; 
}

void Worm2Dbase::applyScheduledFuncable(
    const int function_index, const bool has_condval, const int condval)
{
    itsEf.setFunctionCondition(function_index, has_condval, condval);
}

void Worm2Dbase::resetFuncableSchedules()
{
    for (FuncableSchedule & schedule : funcableSchedules)
    {
        schedule.current_entry = -1;
        schedule.previous_time = -1;
    }
}

void Worm2Dbase::activateFuncableSchedulesForSimulation()
{
    for (FuncableSchedule & schedule : funcableSchedules)
        schedule.active = true;
    resetFuncableSchedules();
}

void Worm2Dbase::updateScheduledFuncables(const double current_time)
{
    for (FuncableSchedule & schedule : funcableSchedules)
    {
        if (!schedule.active || current_time < schedule.time_offset)
        {
            schedule.previous_time = current_time;
            continue;
        }

        if (
            schedule.previous_time >= 0
            && current_time < schedule.previous_time)
          schedule.current_entry = -1;
        schedule.previous_time = current_time;

        double cycle_time =
            fmod(current_time - schedule.time_offset, schedule.total_period);
        if (cycle_time < 0) cycle_time += schedule.total_period;

        int entry = 0;
        double boundary = schedule.time_intervals[0];
        while (
            entry + 1 < static_cast<int>(schedule.time_intervals.size())
            && cycle_time >= boundary)
        {
            entry++;
            boundary += schedule.time_intervals[entry];
        }

        if (entry == schedule.current_entry) continue;
        schedule.current_entry = entry;
        const bool has_condval = !schedule.condvals.empty();
        applyScheduledFuncable(
            schedule.function_index,
            has_condval,
            has_condval ? schedule.condvals[entry] : 0);
    }
}

void Worm2Dbase::constructFuncableSchedules(const json & j)
{
    funcableSchedules.clear();
    if (
        !j.contains("Funcable")
        || !j.at("Funcable").is_object()
        || !j.at("Funcable").contains("schedules"))
      return;

    const json & schedules = j.at("Funcable").at("schedules");
    if (!schedules.is_object())
      throw runtime_error("Funcable.schedules must be an object");

    for (const auto & item : schedules.items())
    {
        const json & value = item.value();
        if (!value.is_object())
          throw runtime_error("Each Funcable schedule must be an object");

        FuncableSchedule schedule;
        if (
            !value.contains("function_index")
            || !value.at("function_index").contains("value"))
          throw runtime_error(
              "Funcable schedule " + item.key()
              + " requires function_index.value");
        schedule.function_index =
            value.at("function_index").at("value").get<int>();
        if (schedule.function_index < 1)
          throw runtime_error(
              "Funcable schedule function_index must be positive");

        if (
            !value.contains("time_intervals")
            || !value.at("time_intervals").contains("value"))
          throw runtime_error(
              "Funcable schedule " + item.key()
              + " requires time_intervals.value");
        schedule.time_intervals =
            value.at("time_intervals").at("value").get<vector<double> >();
        if (schedule.time_intervals.empty())
          throw runtime_error(
              "Funcable schedule time_intervals must not be empty");
        for (const double interval : schedule.time_intervals)
        {
            if (!isfinite(interval) || interval <= 0)
              throw runtime_error(
                  "Funcable schedule intervals must be finite and positive");
            schedule.total_period += interval;
        }

        if (value.contains("condvals"))
        {
            schedule.condvals =
                value.at("condvals").at("value").get<vector<int> >();
            if (
                schedule.condvals.size()
                != schedule.time_intervals.size())
              throw runtime_error(
                  "Funcable schedule condvals and time_intervals must "
                  "have the same length");
        }

        if (value.contains("time_offset"))
          schedule.time_offset =
              value.at("time_offset").at("value").get<double>();
        if (!isfinite(schedule.time_offset) || schedule.time_offset < 0)
          throw runtime_error(
              "Funcable schedule time_offset must be finite and non-negative");

        if (value.contains("doEvolution"))
          schedule.doEvolution =
              value.at("doEvolution").at("value").get<bool>();
        schedule.active = schedule.doEvolution;
        funcableSchedules.push_back(schedule);
    }
}

void Worm2Dbase::randomizeNS(RandomState &rs)
{
 //shared_ptr<W2Dbaseparameters> w1parss = dynamic_pointer_cast<W2Dbaseparameters>(W2Dbaseparameters1b);

 //assert(w1parss!=nullptr);

  NervousSystem * n = dynamic_cast<NervousSystem*>(n_ptr);
  
  if (n){
   
  bool randomInitialState;
  getValCJWorm<bool>("random_initial_state",randomInitialState);


  if (randomInitialState) {
  n->RandomizeCircuitState(-1, 1, rs);
  n->RandomizeCircuitOutput(0.2, 0.8, rs);
  }
  }

}

void Worm2Dbase::incSimTimes()
{
    t += settedStepSize; 
    datatime += settedStepSize; 

}


void Worm2Dbase::Step(double StepSize_) 
{ //StepSize = StepSize_; 
    setStepSize(StepSize_);
    //settedStepSize = StepSize_;
    Step();
}


double Worm2Dbody::getVelocity()
{
    if (first_call){
    xtp =  CoMx();
    ytp =  CoMy();
    first_call = false;
    }

    double xt = CoMx(); 
    double yt = CoMy();
    double vel = sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));
    xtp = xt;
    ytp = yt;
    return vel;

}

void Worm2Dbase::DumpNSOrdered()
{

    //const int NSsize = dynamic_cast<NervousSystem&>(*n_ptr).size;

    /* static bool firstcall = true;
    static size_t pos;
    static int tt;

    if (resetStats(firstcall,pos,tt,"ns.dat")) return; */

    size_t pos = getPos("ns.dat");
    ofstream & ofs = ofsvec[pos];  
    int & tt = tts[pos];

    if (++tt >= dataskips) {
        tt = 0;
        ofs << datatime;
        for (int i = 1; i <= par1.N_size; i++) ofs <<  " " << n_ptr->NeuronOutput(i);
        ofs << endl;
    }
}


void Worm2Dbase::DumpVal(string filename_, double val)
{

   /*  static bool firstcall = true;
    static size_t pos;
    static int tt;

    if (resetStats(firstcall,pos,tt,filename_)) return; */

    size_t pos = getPos(filename_);
    ofstream & ofs = ofsvec[pos];  
    int & tt = tts[pos];

    if (++tt >= dataskips) {
        tt = 0;

        ofs << datatime << " " << val;
    
        ofs << endl;
    }
}





void Worm2Dbody::Curvature(TVector<double> &c)
{
    double dx1,dy1,dx2,dy2,a,a1,a2,seg;
    int k=1;

    for (int i = 3; i < N_segments-1; i+=2)
    {
        dx1 = b.X(i) - b.X(i-2);
        dy1 = b.Y(i) - b.Y(i-2);
        dx2 = b.X(i+2) - b.X(i);
        dy2 = b.Y(i+2) - b.Y(i);

        a1 = atan2(dy1,dx1);
        a2 = atan2(dy2,dx2);

        if (a1 > PI/2 and a2 < -PI/2)
            a = (a1 - 2*PI) - a2;
        else
            if (a1 < -PI/2 and a2 > PI/2)
                a = a1 - (a2 - 2*PI);
            else
                a = a1-a2;

        seg = sqrt(pow(b.X(i-2)-b.X(i+2),2) + pow(b.Y(i-2)-b.Y(i+2),2));
        c(k) = (2*sin(a)/seg)/1000;
        k++;
    }
}

void Worm2Dbody::writeCurvature()
{

    /* static bool firstcall = true;
    static size_t pos;
    static int tt;

    if (resetStats(firstcall,pos,tt,"curv.dat")) return; */
    

    size_t pos = getPos("curv.dat");
    ofstream & ofs = ofsvec[pos];  
    int & tt = tts[pos];

  double dx1,dy1,dx2,dy2,a,a1,a2,seg;
 
  if (++tt >= dataskips) {
    tt = 0;
    //time
    ofs << datatime;

    for (int i = 3; i < N_segments-1; i+=2)
    {
      dx1 = b.X(i) - b.X(i-2);
      dy1 = b.Y(i) - b.Y(i-2);
      dx2 = b.X(i+2) - b.X(i);
      dy2 = b.Y(i+2) - b.Y(i);

      a1 = atan2(dy1,dx1);
      a2 = atan2(dy2,dx2);

      if (a1 > PI/2 and a2 < -PI/2)
      a = (a1 - 2*PI) - a2;
      else
      if (a1 < -PI/2 and a2 > PI/2)
      a = a1 - (a2 - 2*PI);
      else
      a = a1-a2;

      seg = sqrt(pow(b.X(i-2)-b.X(i+2),2) + pow(b.Y(i-2)-b.Y(i+2),2));
      ofs <<  " " << (2*sin(a)/seg)/1000;
    }
    ofs << endl;
  }
}


void Worm2Dbody::writeBody()
{
   /*  static bool firstcall = true;
    static size_t pos;
    static int tt;
    
    if (resetStats(firstcall,pos,tt,"body.dat")) return; */

    size_t pos = getPos("body.dat");
    ofstream & ofs = ofsvec[pos];  
    int & tt = tts[pos];

    if (++tt >= dataskips) {
        tt = 0;

        ofs << datatime;
        // Body
        for (int i = 1; i <= N_rods; i++)
        {
            ofs <<  " " << b.X(i) << " " << b.Y(i) << " " << b.Phi(i);
        }
        ofs << endl;
    }
    return;
}

void Worm2Dbase::writeJsonFile(ofstream & json_out)
{

    json j;
    addParsToJson(j);
    //ofstream json_out(supArgs1.rename_file("worm_data.json"));
    //ofstream json_out("worm_data.json");
    json_out << std::setw(4) << j << std::endl;
    //json_out.close();

}


void Worm2Dbody::addParsToJson(json & j)
{  
 appendBodyToJson(j, b);
}

void Worm2Dbase::makeExternalInputConnFromJson(const json & j)
{

    if (j.contains("InputNS")){
        vector<toFromWeight> vec1 = j["InputNS"]["weights"]["value"].template get< vector<toFromWeight> >();
        NSInputConn.swap(vec1);
    }

    if (j.contains("OutputNS")){
        vector<toFromWeight> vec1 = j["OutputNS"]["weights"]["value"].template get< vector<toFromWeight> >();
        NSOutputConn.swap(vec1);
    }

    vector<string> names;
    if (j.contains("nervous_system")
    && j.at("nervous_system").contains("cell_names")
    && j.at("nervous_system").at("cell_names").contains("value")
    && j.at("nervous_system").at("cell_names").at("value").is_array())
    {
        names = j.at("nervous_system").at("cell_names").at("value").
        template get< vector<string> >();
    }
    else names = getDistinctCellNames();

    if ((names.empty() || names[0]=="not implemented") && j.contains("Nervous system")
    && j.at("Nervous system").contains("Cell name")
    && j.at("Nervous system").at("Cell name").contains("value")
    && j.at("Nervous system").at("Cell name").at("value").is_array())
    {
        names = makeUnique(j.at("Nervous system").
        at("Cell name").at("value").template get< vector<string> >());
    }
    if ((names.empty() || names[0]=="not implemented") && j.contains("Nervous system"))
    {
        int size = j.at("Nervous system").at("size").at("value").get<int>();
        names.clear();
        for (int i=1; i<=size; i++) names.push_back("cell_"+to_string(i-1));
    }

    unordered_map<string, int> name_index;
    for (size_t i = 0; i < names.size(); ++i)
        name_index[names[i]] = static_cast<int>(i) + 1;

    vector<double> exvec;
    vector<toFromWeight> vec1;

    if (j.contains("driving_inputs")){
        for (const auto& input : j.at("driving_inputs").at("inputs").at("value"))
        {
            int input_num = input.at("input_num").get<int>();
            if (input_num > static_cast<int>(exvec.size())) exvec.resize(input_num, 0.0);
            exvec[input_num-1] = input.at("strength").at("value").get<double>();
        }

        for (const auto& conn : j.at("driving_inputs").at("weights").at("value"))
        {
            toFromWeight val;
            val.w.from = conn.at("from_input").get<int>();
            val.to = name_index.at(conn.at("to_cell").get<string>());
            val.w.weight = conn.at("weight").at("value").get<double>();
            vec1.push_back(val);
        }
    }
    else if (j.contains("Driving input"))
    {
        vec1 =
            j["Driving input"]["weights"]["value"].
            template get< vector<toFromWeight> >();
        exvec =
            j["Driving input"]["strengths"]["value"].
            template get< vector<double> >();
    }

    if (j.contains("sensors"))
    {
        const json & sensors = j.at("sensors");
        for (const auto& sensorItem : sensors.items())
        {
            const json & sensor = sensorItem.value();
            if (sensor.contains("weights"))
            {
                const int firstInput = static_cast<int>(exvec.size());
                exvec.resize(exvec.size() + 2, 0.0);
                const json & weights = sensor.at("weights").at("value");
                for (const auto & conn : weights)
                {
                    const int output = conn.at("from_output").get<int>();
                    if (output < 1 || output > 2)
                        throw runtime_error(
                            "Sensor output number must be 1 or 2");
                    toFromWeight val;
                    val.w.from = firstInput + output;
                    val.to = name_index.at(conn.at("to_cell").get<string>());
                    val.w.weight = conn.at("weight").at("value").get<double>();
                    vec1.push_back(val);
                }
            }
        }
    }

    externalInputs.swap(exvec);
    externalInputConn.swap(vec1);

}

void Worm2Dbase::addParsToJson()
{

addParsToJson(BPitsJson);

}

void Worm2Dbase::addParsToJson(json & j)
{  

    doubIntParamsHead par1pars = par1.getParams();
    appendToJson<double>(j[par1pars.parDoub.head],par1pars.parDoub);
    appendToJson<long>(j[par1pars.parInt.head],par1pars.parInt);

    if (false){
    string nsHead = "Nervous system";
   
    {Params< string > par;
    par.names = {"Model name"};
    par.vals = {getModelName()};
    appendToJson<string>(j[nsHead],par);
    }
    }

    j["nervous_system"]["model_name"]["value"] = getModelName();
    j["nervous_system"]["model_name"]["message"] = "Name of the model used for the nervous system.";

    vector<doubIntParamsHead> parvec = getWormParams();
    for (size_t i=0;i<parvec.size(); i++) {
        if (strcmp(parvec[i].parDoub.head.c_str(),"NULL")!=0)
        appendToJson<double>(j[parvec[i].parDoub.head],parvec[i].parDoub);
        if (strcmp(parvec[i].parInt.head.c_str(),"NULL")!=0)
        appendToJson<long>(j[parvec[i].parInt.head],parvec[i].parInt);
        }

    removeLegacyParameterKeys(j);

    //cout << "worm2dbase add pars to json" << endl;

    setPhenoNames();
    if (phenoNames.size()>0) 
    {
        for (int i=0; i<phenoNames.size();i++) phenoNames[i] = getModelName() + "_" + phenoNames[i];
        appendVectorToJson<string>(j["PhenoNames"], phenoNames);
        appendVectorToJson<int>(j["PhenoNamesNums"], phenoNamesNums);
    }

    vector<string> names;
    if (j.contains("nervous_system")
    && j.at("nervous_system").contains("cell_names")
    && j.at("nervous_system").at("cell_names").contains("value")
    && j.at("nervous_system").at("cell_names").at("value").is_array())
    names = j.at("nervous_system").at("cell_names").at("value").template get< vector<string> >();
    else names = getDistinctCellNames();

    if ((names.empty() || names[0]=="not implemented") && j.contains("Nervous system") 
    && j.at("Nervous system").contains("Cell name") 
    && j.at("Nervous system").at("Cell name").contains("value") 
    && j.at("Nervous system").at("Cell name").at("value").is_array())
    {
    names = makeUnique(j.at("Nervous system").
    at("Cell name").at("value").template get< vector<string> >());
    }
    if ((names.empty() || names[0]=="not implemented") && j.contains("Nervous system"))
    {
        int size = j.at("Nervous system").at("size").at("value").get<int>();
        names.clear();
        for (int i=1; i<=size; i++) names.push_back("cell_"+to_string(i-1));
    }
    if (names.empty() || names[0]=="not implemented")
    {
        names.clear();
        for (int i=0; i<par1.N_size; i++) names.push_back("cell_"+to_string(i));
    }
    assert(!names.empty() && names[0]!="not implemented");


    if (!externalInputs.empty() || !externalInputConn.empty())
    {
        {
            json & j22 = j["driving_inputs"]["weights"]["value"];
            j["driving_inputs"]["weights"]["message"] =
                "Weights of driving inputs to Nervous System in sparse format";
            for (const toFromWeight & val : externalInputConn)
            {
                assert(val.to-1<names.size() && val.to-1>=0);
                bool found = false;
                for (auto it = j22.begin(); it != j22.end(); ++it)
                    if (it->at("to_cell")==names[val.to-1]
                        && it->at("from_input")==val.w.from)
                    {
                        it->at("weight").at("value")=val.w.weight;
                        found = true;
                        break;
                    }
                if (found) continue;

                json j2 = json::object();
                j2["from_input"] = val.w.from;
                j2["to_cell"] = names[val.to-1];
                j2["weight"]["value"] = val.w.weight;
                j22.push_back(j2);
            }
        }

        {
            json & j22 = j["driving_inputs"]["inputs"]["value"];
            j["driving_inputs"]["inputs"]["message"] =
                "Driving input strength to Nervous System";
            for (int i=0;i<externalInputs.size();i++)
            {
                bool found = false;
                for (auto it = j22.begin(); it != j22.end(); ++it)
                    if (it->at("input_num")==i+1)
                    {
                        it->at("strength").at("value")=externalInputs[i];
                        found = true;
                        break;
                    }
                if (found) continue;

                json j2 = json::object();
                j2["input_num"] = i+1;
                j2["strength"]["value"] = externalInputs[i];
                j22.push_back(j2);
            }
        }

        appendVectorToJson<toFromWeight>(
            j["Driving input"]["weights"], externalInputConn);
        j["Driving input"]["weights"]["message"] =
            "Weights of driving inputs to Nervous System in sparse format";
        appendVectorToJson<double>(
            j["Driving input"]["strengths"], externalInputs);
        j["Driving input"]["strengths"]["message"] =
            "Driving input strength to Nervous System in sparse format";
        j["Driving input"]["size"]["value"] = externalInputs.size();
    }
    else
    {
        j.erase("driving_inputs");
        j.erase("Driving input");
    }


    appendVectorToJson<toFromWeight>(j["InputNS"]["weights"], NSInputConn);
    j["InputNS"]["weights"]["message"] = "Weights of driving inputs to NS from another NS";

    appendVectorToJson<toFromWeight>(j["OutputNS"]["weights"], NSOutputConn);
    j["OutputNS"]["weights"]["message"] = "Weights of driving inputs from NS to another NS";

    //basePar1->addParsToJson(j);

    //W2Dbaseparameters1b->addParsToJson(j["Worm"]);
    //W2Dbaseparameters1->addParsToJson(j);

    //cout << "idi " << endl;
    InputSwitcher::addParsToJson(j);
    addEvolvableToJson(j);
    addFuncableToJson(j);
}

void Worm2Dm::addParsToJson(json & j)
{  
    //if (W2Dmparscalled) {W2Dmparscalled=false;return;}


    appendVectorToJson<toFromWeight>(j["Dorsal body"]["weights"], dBodyConnvec);
    appendVectorToJson<toFromWeight>(j["Ventral body"]["weights"], vBodyConnvec);
    j["Ventral body"]["weights"]["message"] = "Ventral muscle to body weights weights in sparse format";
    j["Dorsal body"]["weights"]["message"] = "Dorsal muscle to body weights weights in sparse format";

    {json & j22 =  j["dorsal_body"]["weights"]["value"];
   
    j["dorsal_body"]["weights"]["message"] = "Dorsal muscle to body weights in sparse format";
    for (const toFromWeight & val : dBodyConnvec)
    {
       // assert(val.w.from-1<names.size() && val.w.from-1>=0);
        bool found = false;
        for (auto it = j22.begin(); it != j22.end(); ++it)
            if (it->at("to_seg")==val.to && it->at("from_musc")==val.w.from)
        {it->at("weight").at("value")=val.w.weight;found = true;break;}
        if (found) continue;

        json j2 = json::object();
        j2["from_musc"] = val.w.from;
        j2["to_seg"] = val.to;
        j2["weight"]["value"] = val.w.weight;
        j["dorsal_body"]["weights"]["value"].push_back(j2);
    }  
    }

    {json & j22 =  j["ventral_body"]["weights"]["value"];
   
    j["ventral_body"]["weights"]["message"] = "Ventral muscle to body weights in sparse format";
    for (const toFromWeight & val : vBodyConnvec)
    {
       // assert(val.w.from-1<names.size() && val.w.from-1>=0);
        bool found = false;
        for (auto it = j22.begin(); it != j22.end(); ++it)
            if (it->at("to_seg")==val.to && it->at("from_musc")==val.w.from)
        {it->at("weight").at("value")=val.w.weight;found = true;break;}
        if (found) continue;

        json j2 = json::object();
        j2["from_musc"] = val.w.from;
        j2["to_seg"] = val.to;
        j2["weight"]["value"] = val.w.weight;
        j["ventral_body"]["weights"]["value"].push_back(j2);
    }  
    }




    //string nsHead = "Nervous system";
    //appendCellNamesToJson(j[nsHead], getCellNames(), 1);
    //appendCellNamesToJson(j[nsHead], getCellNames(), par1.N_units);

    //W2Dmparscalled = true;

    Worm2Dbody::addParsToJson(j);
    Worm2Dbase::addParsToJson(j);
    appendNSCellClassesToJson(j, getSectionNames());



}


void Worm2D::addParsToJson(json & j)
{  
    
    j["worm"]["do_orig_musc_input"]["value"] = doOrigMuscInput;
    j["worm"]["do_orig_sr_input"]["value"] = doOrigSRInput;
    
    vector<string> names;
    if (j.contains("nervous_system")
    && j.at("nervous_system").contains("cell_names")
    && j.at("nervous_system").at("cell_names").contains("value")
    && j.at("nervous_system").at("cell_names").at("value").is_array())
    names = j.at("nervous_system").at("cell_names").at("value").template get< vector<string> >();
    else names = getDistinctCellNames();

    if ((names.empty() || names[0]=="not implemented") && j.contains("Nervous system") 
    && j.at("Nervous system").contains("Cell name") 
    && j.at("Nervous system").at("Cell name").contains("value") 
    && j.at("Nervous system").at("Cell name").at("value").is_array())
    {
    names = makeUnique(j.at("Nervous system").
    at("Cell name").at("value").template get< vector<string> >());
    }
    if ((names.empty() || names[0]=="not implemented") && j.contains("Nervous system"))
    {
        int size = j.at("Nervous system").at("size").at("value").get<int>();
        names.clear();
        for (int i=1; i<=size; i++) names.push_back("cell_"+to_string(i-1));
    }
    if (names.empty() || names[0]=="not implemented")
    {
        names.clear();
        for (int i=0; i<par1.N_size; i++) names.push_back("cell_"+to_string(i));
    }
    assert(!names.empty() && names[0]!="not implemented");

    vector<string> names_no_suffix = removeSuffixIndices(names);
    j["nervous_system"]["cell_names"]["value"] = names;
    j["nervous_system"]["cell_names_no_suffix"]["value"] = names_no_suffix;

    NervousSystem * n_ptr1 = dynamic_cast<NervousSystem*>(n_ptr);
    if (n_ptr1){
    string nsHead = "Nervous system";
    appendAllNSJson(j[nsHead], *n_ptr1);
    appendNSToJsonByCell(j, *n_ptr1, names, getSectionNames());
    }

   
    appendMuscleToJson(j,m);

    NSToMuscles vMuscConn(par1.N_muscles);
    NSToMuscles dMuscConn(par1.N_muscles);
    vMuscConn.setWeights(vMuscConnvec);
    dMuscConn.setWeights(dMuscConnvec);
   
    j["Ventral NMJ"]["weights"]["message"] = "Ventral NMJ weights weights in sparse format";
    //appendMatrixToJson(j["Ventral NMJ"]["weights"], vMuscConn.weights, vMuscConn.numConns, vMuscConn.size);
    j["Dorsal NMJ"]["weights"]["message"] = "Dorsal NMJ weights weights in sparse format";
    //appendMatrixToJson(j["Dorsal NMJ"]["weights"], dMuscConn.weights, dMuscConn.numConns, dMuscConn.size);
    
    {Params<int> par;    
    par.names = {"size", "maxcons"};
    par.vals = {vMuscConn.size, vMuscConn.maxconns};
    appendToJson<int>(j["Ventral NMJ"],par);}
    {Params<int> par;    
    par.names = {"size", "maxcons"};
    par.vals = {dMuscConn.size, dMuscConn.maxconns};
    appendToJson<int>(j["Dorsal NMJ"],par);}
    
    {Params< vector<int> > par;
    par.names = {"NumConns"};
    par.vals = {getVector<int>(vMuscConn.numConns, vMuscConn.size),};
    appendToJson<vector<int> >(j["Ventral NMJ"],par);}

    {Params< vector<int> > par;
    par.names = {"NumConns"};
    par.vals = {getVector<int>(dMuscConn.numConns, dMuscConn.size),};
    appendToJson<vector<int> >(j["Dorsal NMJ"],par);}
    
    vector<doubIntParamsHead> parvec = getWormParams();
    for (size_t i=0;i<parvec.size(); i++) {
        if (strcmp(parvec[i].parDoub.head.c_str(),"NULL")!=0)
        appendToJson<double>(j[parvec[i].parDoub.head],parvec[i].parDoub);
        if (strcmp(parvec[i].parInt.head.c_str(),"NULL")!=0)
        appendToJson<long>(j[parvec[i].parInt.head],parvec[i].parInt);
        }
    

    appendVectorToJson<toFromWeight>(j["Dorsal NMJ"]["weights"], dMuscConnvec);
    appendVectorToJson<toFromWeight>(j["Ventral NMJ"]["weights"], vMuscConnvec);
   
    appendCellNamesToJson(j["Dorsal NMJ"], getDMuscNames(), 1);
    appendCellNamesToJson(j["Ventral NMJ"], getVMuscNames(), 1);

  
    

   
    if (!j.contains("dorsal_nmj")) {
    j["dorsal_nmj"]["weights"]["value"] = json::array();
    j["ventral_nmj"]["weights"]["value"] = json::array();
    }

    {json & j22 =  j["dorsal_nmj"]["weights"]["value"];

    for (const toFromWeight & val : dMuscConnvec)
    {
        assert(val.w.from-1<names.size() && val.w.from-1>=0);
        bool found = false;
        for (auto it = j22.begin(); it != j22.end(); ++it)
            if (it->at("to_musc")==val.to && it->at("from_cell")==names[val.w.from-1])
        {it->at("weight").at("value")=val.w.weight;found = true;break;}
        if (found) continue;

        json j2 = json::object();
        j2["from_cell"] = names[val.w.from-1];
        j2["to_musc"] = val.to;
        j2["weight"]["value"] = val.w.weight;
        j["dorsal_nmj"]["weights"]["value"].push_back(j2);
    }  
    }

    //assert(0); 

    {json & j22 =  j["ventral_nmj"]["weights"]["value"];

    for (const toFromWeight & val : vMuscConnvec)
    {

        assert(val.w.from-1<names.size() && val.w.from-1>=0);
        bool found = false;
        for (auto it = j22.begin(); it != j22.end(); ++it)
            if (it->at("to_musc")==val.to && it->at("from_cell")==names[val.w.from-1])
        {it->at("weight").at("value")=val.w.weight;found = true;break;}
        if (found) continue;

        json j2 = json::object();
        j2["from_cell"] = names[val.w.from-1];
        j2["to_musc"] = val.to;
        j2["weight"]["value"] = val.w.weight;
        j["ventral_nmj"]["weights"]["value"].push_back(j2);
    }

    }
   
    j["ventral_nmj"]["weights"]["message"] = "Ventral NMJ weights in sparse format";
    j["dorsal_nmj"]["weights"]["message"] = "Dorsal NMJ weights in sparse format";

    

    if (hasVNCNMJ){

        {if (!j.contains("VNC NMJ")) j["VNC NMJ"] = {};

        json & j2 = j["VNC NMJ"];
    
        j2["setFromThis"]["value"] = true;
        j2["V inds"]["value"] = ventinds;
        j2["D inds"]["value"] = dorsinds;
        j2["V conns"]["value"] = unitToMuscV;
        j2["D conns"]["value"] = unitToMuscD;
        j2["NMJ gain map V"]["value"] = namedVars["NMJ gain map V"];
        j2["NMJ gain map D"]["value"] = namedVars["NMJ gain map D"]; //NMJ_gain_map_D;
        j2["NMJ gain fact"]["value"] = namedVars["NMJ gain fact"];
        }

        
        {
            
            if (!j.contains("vnc_nmj")) 
            {

                
                j["vnc_nmj"] = json::object();
                //vector<string> names = getCellNamesUnit();
                const vector<string> & names = names_no_suffix;
                json & j2 = j["vnc_nmj"];
                j2["ventral_conns"] = json::object();
                j2["dorsal_conns"]  = json::object();
                for (const weightentry & val : ventinds){
                    j2["ventral_conns"][names_no_suffix[val.from-1]]["weight"]["value"]=val.weight;
                    j2["ventral_conns"][names_no_suffix[val.from-1]]["cell_ind"]=val.from;
                 }
        
                for (const weightentry & val : dorsinds){
                    j2["dorsal_conns"][names_no_suffix[val.from-1]]["weight"]["value"]=val.weight;
                    j2["dorsal_conns"][names_no_suffix[val.from-1]]["cell_ind"]=val.from;
                }
            }
            else{

              json & j2 = j["vnc_nmj"];

              for (const weightentry & val : ventinds)
              {
                  const string & name = names_no_suffix[val.from-1];
                  j2["ventral_conns"][name]["cell_ind"] = val.from;
                  j2["ventral_conns"][name]["weight"]["value"] = val.weight;
              }
              for (const weightentry & val : dorsinds)
              {
                  const string & name = names_no_suffix[val.from-1];
                  j2["dorsal_conns"][name]["cell_ind"] = val.from;
                  j2["dorsal_conns"][name]["weight"]["value"] = val.weight;
              }

            }


        json & j2 = j["vnc_nmj"];
        j2["set_from_this"]["value"] = true;
          

        j2["ventral_units"]["value"] = json::array();
        j2["dorsal_units"]["value"] = json::array();
        for (const intPair & val : unitToMuscV)
            j2["ventral_units"]["value"].push_back({{"from_unit", val.ind},{"to_musc" , val.val}});
          
        for (const intPair & val : unitToMuscD)
            j2["dorsal_units"]["value"].push_back({{"from_unit", val.ind},{"to_musc" , val.val}});
          
        j2["gain_map_v"]["value"] = namedVars["NMJ gain map V"];
        j2["gain_map_d"]["value"] = namedVars["NMJ gain map D"]; //NMJ_gain_map_D;
        j2["gain_fact"]["value"] = namedVars["NMJ gain fact"];
    

      
        }

    }

     if (hasVNC18){


        {
            if (!j.contains("VNC 18")) j["VNC 18"] = {};
        json & j2 = j["VNC 18"];
    
        j2["setFromThis"]["value"] = true;
        j2["V inds"]["value"] = ventinds;
        j2["D inds"]["value"] = dorsinds;
        j2["NMJ gain map V"]["value"] = namedVars["NMJ gain map V"];
        j2["NMJ gain map D"]["value"] = namedVars["NMJ gain map D"];
        j2["NMJ gain fact"]["value"] = namedVars["NMJ gain fact"];
        }


         if (!j.contains("vnc_18")) 
            {
                j["vnc_18"] = json::object();
                const vector<string> & names = names_no_suffix;
                json & j2 = j["vnc_18"];
                j2["ventral_conns"] = json::object();
                j2["dorsal_conns"]  = json::object();
                for (const weightentry & val : ventinds){
                    j2["ventral_conns"][names_no_suffix[val.from-1]]["weight"]["value"]=val.weight;
                    j2["ventral_conns"][names_no_suffix[val.from-1]]["cell_ind"]=val.from;
                 }
        
                for (const weightentry & val : dorsinds){
                    j2["dorsal_conns"][names_no_suffix[val.from-1]]["weight"]["value"]=val.weight;
                    j2["dorsal_conns"][names_no_suffix[val.from-1]]["cell_ind"]=val.from;
                }
            }
            else{

              json & j2 = j["vnc_18"];
        

            for (const weightentry & val : ventinds)
               for (auto it = j2["ventral_conns"].begin(); it != j2["ventral_conns"].end(); ++it)
                 if (it->at("cell_ind")==val.from) {it->at("weight").at("value")=val.weight;break;}
              for (const weightentry & val : dorsinds)
                  for (auto it = j2["dorsal_conns"].begin(); it != j2["dorsal_conns"].end(); ++it)
                     if (it->at("cell_ind")==val.from) {it->at("weight").at("value")=val.weight;break;}

            }

            json & j2 = j["vnc_18"];
        j2["set_from_this"]["value"] = true;
          

        
          
        j2["gain_map_v"]["value"] = namedVars["NMJ gain map V"];
        j2["gain_map_d"]["value"] = namedVars["NMJ gain map D"]; //NMJ_gain_map_D;
        j2["gain_fact"]["value"] = namedVars["NMJ gain fact"];



    }
    
   


    Worm2Dm::addParsToJson(j);

   
    //addExtraParsToJson(j);
}

void Worm2Dbase::writeExtInp(ofstream & ofs)
{

    for (int i=0; i<externalInputs.size(); i++)
    ofs <<  " " << externalInputs[i];

}

void Worm2Dbase::writeVNC(ofstream & ofs)
{

if (n_ptr)
for (int i = 1; i <= par1.N_units; i++) 
    for (int j = 1; j <= par1.N_neuronsperunit; j++) 
        ofs <<  " " << n_ptr->NeuronOutput(nn(j,i));
        
    
}

void Worm2Dbase::writeMusc(ofstream & ofs)
{

    if (m_ptr)
    for (int i = 1; i <= par1.N_muscles; i++) 
    ofs <<  " " << m_ptr->DorsalMuscleOutput(i) << " " << m_ptr->VentralMuscleOutput(i);
        
}


void Worm2Dbase::writeAct()
{
   /*  static bool firstcall = true;
    static size_t pos;
    static int tt;
    

    cout << "Worm2Dbase writeact 1 " << endl;

    if (resetStats(firstcall,pos,tt,"act.dat")) return;

    cout << "Worm2Dbase writeact 2 " << endl;  */
   
    size_t pos = getPos("act.dat");
    ofstream & ofs = ofsvec[pos];  
    int & tt = tts[pos];

    if (++tt >= dataskips) {
        tt = 0;
        //time
        ofs << datatime;

        // Ventral Cord Motor Neurons
        //ofs << "\nV: ";

        writeVNC(ofs);
     
        // Muscles
        //ofs << "\nM: ";
        writeMusc(ofs);
        
        writeExtInp(ofs);

        ofs << endl;
    }
}


void Worm2Dbase::writeState()
{
/* 
    static bool firstcall = true;
    static size_t pos;
    static int tt;

    if (resetStats(firstcall,pos,tt,"state.dat")) return;
  */
   
    size_t pos = getPos("state.dat");
    ofstream & ofs = ofsvec[pos];  
    int & tt = tts[pos];

    if (++tt >= dataskips) {
        tt = 0;
        //time
        ofs << datatime;

        // Ventral Cord Motor Neurons
        //ofs << "\nV: ";
        for (int i = 1; i <= par1.N_units; i++) {
            for (int j = 1; j <= par1.N_neuronsperunit; j++) {
                ofs <<  " " << n_ptr->NeuronState(nn(j,i));
            }
        }
        ofs << endl;
    }
}

/* void Worm2D::setMuscleInputVent()
{

for (int i = 1; i<= vMuscConn.size; i++){
        double tot = 0;
    for (int j = 1; j <= vMuscConn.numConns(i); j++){
        tot +=  vMuscConn.weights[i][j].weight*n_ptr->NeuronOutput(vMuscConn.weights[i][j].from);
    }
    m.SetVentralMuscleInput(i, tot);
}

}

void Worm2D::setMuscleInputDors()
{

    for (int to = 1; to<= dMuscConn.size; to++){
        double tot = 0;
    for (int j = 1; j <= dMuscConn.numConns(to); j++){
        tot +=  dMuscConn.weights[to][j].weight*n_ptr->NeuronOutput(dMuscConn.weights[to][j].from);
    }
    m.SetDorsalMuscleInput(to, tot);
    }

} */



void Worm2Dm::Step1()
{
  
  b.StepBody(settedStepSize);

  zeroAllInputs();
  setExternalInput();

  //setExternalInputOrig();

  n_ptr->EulerStep(settedStepSize);
  
  
  setBodyInput();
  
}

void Worm2D::Step1()
{
  
   
  b.StepBody(settedStepSize);

  zeroAllInputs();
  setExternalInput();


  n_ptr->EulerStep(settedStepSize);

    
  //cout << "sds " << doOrigMuscInput << endl;
  //assert(0);

  if (doOrigMuscInput) setMuscleInputOrig();
  else setMuscleInput();

  setBodyInput();
  
}

void Worm2Dm::setBodExt(const json & j)
{
        setUpBodyConn(j);
        makeExternalInputConnFromJson(j);
}

void Worm2Dm::setBodExt()
{
        setUpBodyConn();
        makeExternalInputConn();
}

void Worm2D::setMuscBodExt(const json & j){ 
        setUpMuscleConn(j);
        Worm2Dm::setBodExt(j);
}

void Worm2D::setMuscBodExt(){
         
        setUpMuscleConn();
        Worm2Dm::setBodExt();
    

        //setUpBodyConn();
        //makeExternalInputConn();
}

void Worm2D::setMuscleInputVec()
{
    {vector<double> vtot(par1.N_muscles, 0.0);

    //for (int i=0;i<vtot.size();i++) vtot[i]=0;

    for (int i=0;i<vMuscConnvec.size();i++)
    {
        const toFromWeight & tfw = vMuscConnvec[i];
        vtot[tfw.to-1] += tfw.w.weight*n_ptr->NeuronOutput(tfw.w.from);
    }
    
    for (int i=0;i<vtot.size();i++) m.SetVentralMuscleInput(i+1, vtot[i]);}

    
    {vector<double> vtot(par1.N_muscles, 0.0);
    //for (int i=0;i<vtot.size();i++) vtot[i]=0;


    for (int i=0;i<dMuscConnvec.size();i++)
    {
        const toFromWeight & tfw = dMuscConnvec[i];
        vtot[tfw.to-1] += tfw.w.weight*n_ptr->NeuronOutput(tfw.w.from);
    }
    for (int i=0;i<vtot.size();i++) m.SetDorsalMuscleInput(i+1, vtot[i]);}

    //m.EulerStep(settedStepSize);
}

void Worm2Dm::setBodyInput()
{

    {
    vector<double> vtot(N_segments, 0.0);
    for (int i=0;i<vBodyConnvec.size();i++)
    {
        const toFromWeight & tfw = vBodyConnvec[i];
        vtot[tfw.to-1] += tfw.w.weight*m_ptr->VentralMuscleOutput(tfw.w.from);
    }
    for (int i=0;i<vtot.size();i++) b.SetVentralSegmentActivation(i+1, vtot[i]);
    }
    
    {
    vector<double> vtot(N_segments, 0.0);
    for (int i=0;i<dBodyConnvec.size();i++)
    {
        const toFromWeight & tfw = dBodyConnvec[i];
        vtot[tfw.to-1] += tfw.w.weight*m_ptr->DorsalMuscleOutput(tfw.w.from);
    }
    for (int i=0;i<vtot.size();i++) b.SetDorsalSegmentActivation(i+1, vtot[i]);
    }
    


}

void Worm2Dbase::incOutputToNS(Worm2Dbase & w_)
{

   vector<double> vtot(w_.par1.N_size, 0.0);

    for (int i=0;i<NSOutputConn.size();i++)
    {
        const toFromWeight & tfw = NSOutputConn[i];
        vtot[tfw.to-1] += tfw.w.weight*n_ptr->NeuronOutput(tfw.w.from);
    }

    for (int i=0;i<vtot.size();i++) w_.itsNS().IncNeuronExternalInput(i+1, vtot[i]); 

}


void Worm2Dbase::incInputFromNS(NSForW2D & ns_)
{
    
    if (NSInputConn.size()<=0) return;

    vector<double> vtot(par1.N_size, 0.0);
   
    for (int i=0;i<NSInputConn.size();i++)
    {
        const toFromWeight & tfw = NSInputConn[i];
        vtot[tfw.to-1] += tfw.w.weight*ns_.NeuronOutput(tfw.w.from);
    
    }
    
    for (int i=0;i<vtot.size();i++) n_ptr->IncNeuronExternalInput(i+1, vtot[i]); 

}


void Worm2Dbase::setExternalInput()
{

    if (externalInputConn.size()<=0) return;

    assignExternalInput();

    
    vector<double> vtot(par1.N_size, 0.0);
   
    for (int i=0;i<externalInputConn.size();i++)
    {
        const toFromWeight & tfw = externalInputConn[i];
        vtot[tfw.to-1] += tfw.w.weight*externalInputs[tfw.w.from-1];
       
    }
    
    for (int i=0;i<vtot.size();i++) n_ptr->IncNeuronExternalInput(i+1, vtot[i]); 

}

NSForW2D * Worm2Dbase::getNS(shared_ptr<const CmdArgs> cmd, const json & j)
{
  bool do_nml =  cmd->getArgValInt("--donml",0);
  if (do_nml) {
    double StepSize = 0;
    if (j.contains("Simulation")){
    StepSize = j["Simulation"]["StepSize"]["value"]; 
    cout << "stepsize " << StepSize << endl;}
    else if (j.contains("Evolutionary Optimization Parameters")){
    StepSize = j["Evolutionary Optimization Parameters"]["StepSize"]["value"]; 
    cout << "stepsize " << StepSize << endl;}
    StepSize = cmd->getArgValDoub("--StepSize",StepSize);
    if (StepSize == 0) return new c302ForW2D();
    return new c302ForW2D(StepSize);

  }
  return new NervousSystem();

}

///////////////////
////////////////////////
////////////////////////


void Worm2D::setMuscleInput()
{

   
    setMuscleInputVec();
    //setMuscleInputVent();
    //setMuscleInputDors();

    m.EulerStep(settedStepSize);
    //cout << "setMuscInp" << endl;
    //exit(1);
}

vector<toFromWeight> Worm2D::makeVentralMuscleConn18()
{
    //cout << "making ventral muscle con" << endl;

    const double NMJ_gain_map_V = namedVars["NMJ gain map V"].get<double>();
    const double NMJ_gain_fact = namedVars["NMJ gain fact"].get<double>();

    const int HeadMotorNeuronMuscles = 6;  // Head motorneurons innervate first 8 muscles (temporarily first 6)
    const int VNCMuscleStart = 7;           // VNC motorneurons innervate starting from 7th muscle
    const int NmusclePerNU = 3;   

    TVector<double> NMJ_Gain(1, par1.N_muscles);
    for (int i=1; i<=par1.N_muscles; i++)
    NMJ_Gain(i) = NMJ_gain_fact*(1.0 - (((i-1)*NMJ_gain_map_V)/par1.N_muscles));
    vector<int> units;
    vector<double> weights;
    sortAsc(ventinds);
    splitWeightEntry(ventinds,units,weights);


    //if (false){
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

    //std::vector<int> v = {DB, DD, VBA, VDA, VBP, VDP, SMDD, RMDD, SMDV, RMDV};
    //std::vector<int> v = units;

    std::unordered_map<int, int> index_of;

    for (int i = 0; i < units.size(); ++i) index_of[units[i]] = i;
      //cout << " ds d " << i << " " << units[i] << " " <<  index_of[units[i]] << endl;
    //}
    
//assert(0);

    //int index = index_of[42]; 

    //RMDV and RMDV swapped in original!

    const double NMJ_SMDV = weights[index_of[SMDV]], NMJ_RMDD = weights[index_of[RMDV]], 
    NMJ_VDa = weights[index_of[VDA]], NMJ_VBa = weights[index_of[VBA]], 
    NMJ_VDp = weights[index_of[VDP]], NMJ_VBp = weights[index_of[VBP]]; 

    

    //}

    //const int SMDV = units[0], RMDV = units[1], VDA = units[2], VBA = units[3], VDP = units[4], VBP = units[5];
    //const double NMJ_SMDV = weights[0], NMJ_RMDD = weights[1], 
    //NMJ_VDa = weights[2], NMJ_VBa = weights[3], NMJ_VDp = weights[4], NMJ_VBp = weights[5]; 



   /*  vector<int> ventralNeurons({SMDV, RMDV, VDA, VBA, VDP, VBP});
    vector<double> ventralNMJ({NMJ_SMDV, NMJ_RMDD, NMJ_VDa, NMJ_VBa, NMJ_VDp, NMJ_VBp});
    hasVNCNMJ = true;
    NMJ_gain_map_V = 0.5;
    NMJ_gain_fact = 0.7;
    vector<weightentry> v1 = makeWeightEntry(ventralNeurons,ventralNMJ);
    ventinds.swap(v1); */

    vector<toFromWeight> vec1;

        {vector<int> neurons({SMDV, RMDV});
        vector<double> NMJ({NMJ_SMDV, NMJ_RMDD});
        for (int i = 1; i <= HeadMotorNeuronMuscles; i++)
        makeMuscleConnHelp(vec1, neurons, NMJ, 1, i, NMJ_Gain);}

        vector<int> neuronsA({VDA, VBA});
        vector<int> neuronsP({VDP, VBP});
        vector<double> NMJA({NMJ_VDa, NMJ_VBa});
        vector<double> NMJP({NMJ_VDp, NMJ_VBp});
        vector<double> NMJA2({NMJ_VDa/2.0, NMJ_VBa/2.0});
        vector<double> NMJP2({NMJ_VDp/2.0, NMJ_VBp/2.0});

        for (int i = VNCMuscleStart; i <= par1.N_muscles; i++){
        int mi = (int) ((i-VNCMuscleStart)/NmusclePerNU)+1;
        int mt = (i-VNCMuscleStart)%NmusclePerNU;
        //ventralHeadInputA = NMJ_VDa*n.NeuronOutput(nn(VDA,mi)) + NMJ_VBa*n.NeuronOutput(nn(VBA,mi));
        //ventralHeadInputP = NMJ_VDp*n.NeuronOutput(nn(VDP,mi)) + NMJ_VBp*n.NeuronOutput(nn(VBP,mi));

        //int mt = 1;
        switch(mt){
            case 0:
                makeMuscleConnHelp(vec1, neuronsA, NMJA, mi, i, NMJ_Gain);
                //m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralHeadInputA);
                break;
            case 1:
                makeMuscleConnHelp(vec1, neuronsA, NMJA2, mi, i, NMJ_Gain);
                makeMuscleConnHelp(vec1, neuronsP, NMJP2, mi, i, NMJ_Gain);
                //m.SetVentralMuscleInput(i, NMJ_Gain(i)*((ventralHeadInputA + ventralHeadInputP)/2));
                break;
            case 2:
                makeMuscleConnHelp(vec1, neuronsP, NMJP, mi, i, NMJ_Gain);
                //m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralHeadInputP);
                break;
        }

    }
    
     return vec1;
}



vector<toFromWeight> Worm2D::makeDorsalMuscleConn18()
{

    const double NMJ_gain_map_D = namedVars["NMJ gain map D"].get<double>();
    const double NMJ_gain_fact = namedVars["NMJ gain fact"].get<double>();
    //cout << "making ventral muscle con" << endl;
    vector<toFromWeight> vec1;
    const int HeadMotorNeuronMuscles = 6;  // Head motorneurons innervate first 8 muscles (temporarily first 6)
    const int VNCMuscleStart = 7;           // VNC motorneurons innervate starting from 7th muscle
    const int NmusclePerNU = 3;   

    TVector<double> NMJ_Gain(1, par1.N_muscles);
    for (int i=1; i<=par1.N_muscles; i++)
    NMJ_Gain(i) = NMJ_gain_fact*(1.0 - (((i-1)*NMJ_gain_map_D)/par1.N_muscles));
    vector<int> units;
    vector<double> weights;
    sortAsc(dorsinds);
    splitWeightEntry(dorsinds,units,weights);


    //if (false){
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

    //std::vector<int> v = {DB, DD, VBA, VDA, VBP, VDP, SMDD, RMDD, SMDV, RMDV};
    //std::vector<int> v = units;

    // RMDD and RMDV are swapped in original code!


    std::unordered_map<int, int> index_of;

    for (int i = 0; i < units.size(); ++i) index_of[units[i]] = i;

    const double NMJ_SMDD = weights[index_of[SMDD]], NMJ_RMDV = weights[index_of[RMDD]], 
    NMJ_DD = weights[index_of[DD]], NMJ_DB = weights[index_of[DB]];


    //}

    //const int SMDD = units[0], RMDD = units[1], DB = units[2], DD = units[3];
    //const double NMJ_SMDD = weights[0], NMJ_RMDV = weights[1], 
    //NMJ_DD = weights[2], NMJ_DB = weights[3];


    {vector<int> neurons({SMDD, RMDD});
    vector<double> NMJ({NMJ_SMDD, NMJ_RMDV});
    for (int i = 1; i <= HeadMotorNeuronMuscles; i++)
    makeMuscleConnHelp(vec1, neurons, NMJ, 1, i, NMJ_Gain);}

    {vector<int> neurons({DD,DB});
    vector<double> NMJ({NMJ_DD, NMJ_DB});
    for (int i = VNCMuscleStart; i <= par1.N_muscles; i++){
        int mi = (int) ((i-VNCMuscleStart)/NmusclePerNU)+1;
        makeMuscleConnHelp(vec1, neurons, NMJ, mi, i, NMJ_Gain);
    }}

    return vec1;
}



vector<toFromWeight> Worm2D::makeMuscleConnVNCV()
{

    //cout << namedVars << endl;

//doubVars.show();
//assert(0);

     //const double & NMJ_gain_map_V = doubVars.getVal("NMJ gain map V");
    //const double & NMJ_gain_fact = doubVars.getVal("NMJ gain fact");
const double NMJ_gain_map_V = namedVars["NMJ gain map V"].get<double>(); //doubVars.getVal("NMJ gain map D");
const double NMJ_gain_fact = namedVars["NMJ gain fact"].get<double>();
    

TVector<double> NMJ_Gain(1, par1.N_muscles);
for (int i=1; i<=par1.N_muscles; i++){
NMJ_Gain(i) = NMJ_gain_fact*(1.0 - (((i-1)*NMJ_gain_map_V)/par1.N_muscles));
//cout << "NMJgain " << NMJ_Gain(i) << endl;
}
//assert(0);
vector<int> units;
vector<double> weights;
sortAsc(ventinds);
splitWeightEntry(ventinds,units,weights);
return makeMuscleConnW2D(units,weights,NMJ_Gain,unitToMuscV);

}

vector<toFromWeight> Worm2D::makeMuscleConnVNCD()
{

const double NMJ_gain_map_D = namedVars["NMJ gain map D"].get<double>(); //doubVars.getVal("NMJ gain map D");
const double NMJ_gain_fact = namedVars["NMJ gain fact"].get<double>();

TVector<double> NMJ_Gain(1, par1.N_muscles);
for (int i=1; i<=par1.N_muscles; i++)
NMJ_Gain(i) = NMJ_gain_fact*(1.0 - (((i-1)*NMJ_gain_map_D)/par1.N_muscles));
vector<int> units;
vector<double> weights;
sortAsc(dorsinds);
splitWeightEntry(dorsinds,units,weights);
return makeMuscleConnW2D(units,weights,NMJ_Gain,unitToMuscD);

}


void Worm2D::setUpMuscleConn(const json & j)
{
    //if (false)
    if (j.contains("vnc_nmj"))
    {
    const json & j2 = j["vnc_nmj"];
    if (!(j2.contains("set_from_this") && j2.at("set_from_this").at("value").get<bool>() == false)){
    hasVNCNMJ = true;

    namedVars["NMJ gain map V"] = 0.5;
    namedVars["NMJ gain map D"] = 0.5;
    namedVars["NMJ gain fact"] = 0.7;

    
    {vector<weightentry> ventinds1;
    for (const auto& conn : j2["ventral_conns"])
    {
        weightentry val;
        val.weight = conn.at("weight").at("value").get<double>();
        val.from = conn.at("cell_ind").get<int>();
        ventinds1.push_back(val);

    }
    //sortAsc(ventinds1);
    //for (const weightentry & val : ventinds1)
    //cout << "hh 1 " << val.from << " " << val.weight << endl;
    //ventinds = ventinds1;
    ventinds.swap(ventinds1);
    }

    {vector<weightentry> dorsinds1;
    for (const auto& conn : j2["dorsal_conns"])
    {
        weightentry val;
        val.weight = conn.at("weight").at("value").get<double>();
        val.from = conn.at("cell_ind").get<int>();
        dorsinds1.push_back(val);

    }
    dorsinds.swap(dorsinds1);}


    {vector<intPair> unitToMuscD1;
    for (const auto& conn : j2["dorsal_units"]["value"])
    {
        intPair val;
        val.val = conn.at("to_musc").get<int>();
        val.ind = conn.at("from_unit").get<int>();
        unitToMuscD1.push_back(val);
    }
    unitToMuscD.swap(unitToMuscD1);
    }
    {vector<intPair> unitToMuscV1;
    for (const auto& conn : j2["ventral_units"]["value"])
    {
        intPair val;
        val.val = conn.at("to_musc").get<int>();
        val.ind = conn.at("from_unit").get<int>();
        unitToMuscV1.push_back(val);
    }
    unitToMuscV.swap(unitToMuscV1);
    }

    if (j2.contains("gain_map_v")) namedVars["NMJ gain map V"] = j2["gain_map_v"]["value"];
    if (j2.contains("gain_map_d")) namedVars["NMJ gain map D"] = j2["gain_map_d"]["value"];
    if (j2.contains("gain_fact")) namedVars["NMJ gain fact"] = j2["gain_fact"]["value"];
    

    vector<toFromWeight> vMuscConnvec1 = makeMuscleConnVNCV();
    vMuscConnvec.swap(vMuscConnvec1);
       
    vector<toFromWeight> dMuscConnvec1 = makeMuscleConnVNCD();
    dMuscConnvec.swap(dMuscConnvec1);

    
    return;
    }
    }

    
    if (j.contains("VNC NMJ"))
    {

    const json & j2 = j["VNC NMJ"];
    if (!(j2.contains("setFromThis") && j2.at("setFromThis").at("value").get<bool>() == false)){
    hasVNCNMJ = true;
    
    
    namedVars["NMJ gain map V"] = 0.5;
    namedVars["NMJ gain map D"] = 0.5;
    namedVars["NMJ gain fact"] = 0.7;

    //doubVars.setVal("NMJ gain map V", 0.5);
    //doubVars.setVal("NMJ gain map D", 0.5);
    //doubVars.setVal("NMJ gain fact", 0.7);

    
    //NMJ_gain_map_V = 0.5;
    //NMJ_gain_map_D = 0.5;
    //NMJ_gain_fact = 0.7;
 
    vector<weightentry> vent1 = j2["V inds"]["value"].template get< vector<weightentry> >();
    //sortAsc(vent1);
    //for (const weightentry & val : vent1)
    //cout << "hh 2 " << val.from << " " << val.weight << endl;
    ventinds.swap(vent1);
    vector<weightentry> dors1 = j2["D inds"]["value"].template get< vector<weightentry> >();
    dorsinds.swap(dors1);

    //unitToMuscV = j2["V conns"]["value"].template get< vector<intPair> >();
    //unitToMuscD = j2["D conns"]["value"].template get< vector<intPair> >();
    
    if (j2.contains("NMJ gain map V"))  namedVars["NMJ gain map V"] = j2["NMJ gain map V"]["value"];
    if (j2.contains("NMJ gain map D")) namedVars["NMJ gain map D"] = j2["NMJ gain map D"]["value"];
    if (j2.contains("NMJ gain fact")) namedVars["NMJ gain fact"] = j2["NMJ gain fact"]["value"];
    

    vector<toFromWeight> vMuscConnvec1 = makeMuscleConnVNCV();
    vMuscConnvec.swap(vMuscConnvec1);
       
    vector<toFromWeight> dMuscConnvec1 = makeMuscleConnVNCD();
    dMuscConnvec.swap(dMuscConnvec1);
     

    return;
    }

    }

    //if (false)
    if (j.contains("vnc_18"))
    {
    
    const json & j2 = j["vnc_18"];
    if (!(j2.contains("set_from_this") && j2.at("set_from_this").at("value").get<bool>() == false)){
    hasVNC18 = true;

    namedVars["NMJ gain map V"] = 0.5;
    namedVars["NMJ gain map D"] = 0.5;
    namedVars["NMJ gain fact"] = 0.7;

    {vector<weightentry> ventinds1;
    for (const auto& conn : j2["ventral_conns"])
    {
        weightentry val;
        val.weight = conn.at("weight").at("value").get<double>();
        val.from = conn.at("cell_ind").get<int>();
        ventinds1.push_back(val);

    }
    ventinds.swap(ventinds1);}

    {vector<weightentry> dorsinds1;
    for (const auto& conn : j2["dorsal_conns"])
    {
        weightentry val;
        val.weight = conn.at("weight").at("value").get<double>();
        val.from = conn.at("cell_ind").get<int>();
        dorsinds1.push_back(val);

    }
    dorsinds.swap(dorsinds1);}

    if (j2.contains("gain_map_v")) namedVars["NMJ gain map V"] = j2["gain_map_v"]["value"];
    if (j2.contains("gain_map_d")) namedVars["NMJ gain map D"] = j2["gain_map_d"]["value"];
    if (j2.contains("gain_fact")) namedVars["NMJ gain fact"] = j2["gain_fact"]["value"];
    

    vector<toFromWeight> vMuscConnvec1 = makeVentralMuscleConn18();//is this correct? ok
    vMuscConnvec.swap(vMuscConnvec1);
       
    vector<toFromWeight> dMuscConnvec1 = makeDorsalMuscleConn18();
    dMuscConnvec.swap(dMuscConnvec1);

    return;



    }
    }   

    if (j.contains("VNC 18"))
    {
    //assert(0);
    const json & j2 = j["VNC 18"];
    if (!(j2.contains("setFromThis") && j2.at("setFromThis").at("value").get<bool>() == false)){
    hasVNC18 = true;

   
    namedVars["NMJ gain map V"] = 0.5;
    namedVars["NMJ gain map D"] = 0.5;
    namedVars["NMJ gain fact"] = 0.7;

    //NMJ_gain_map_V = 0.5;
    //NMJ_gain_map_D = 0.5;
    //NMJ_gain_fact = 0.7;
 
    ventinds = j2["V inds"]["value"].template get< vector<weightentry> >();
    dorsinds = j2["D inds"]["value"].template get< vector<weightentry> >();

    
    if (j2.contains("NMJ gain map V"))  namedVars["NMJ gain map V"] = j2["NMJ gain map V"]["value"];
    if (j2.contains("NMJ gain map D")) namedVars["NMJ gain map D"] = j2["NMJ gain map D"]["value"];
    if (j2.contains("NMJ gain fact")) namedVars["NMJ gain fact"] = j2["NMJ gain fact"]["value"];
   
    

    vector<toFromWeight> vMuscConnvec1 = makeVentralMuscleConn18(); //is this correct? ok
    vMuscConnvec.swap(vMuscConnvec1);
       
    vector<toFromWeight> dMuscConnvec1 = makeDorsalMuscleConn18();
    dMuscConnvec.swap(dMuscConnvec1);
     

    return;

    }
    }


    //if (false){
    if (j.contains("dorsal_nmj")){
    vector<string> names;
    if (j.contains("nervous_system")
    && j.at("nervous_system").contains("cell_names")
    && j.at("nervous_system").at("cell_names").contains("value")
    && j.at("nervous_system").at("cell_names").at("value").is_array())
    names = j.at("nervous_system").at("cell_names").at("value").template get< vector<string> >();
    else names = getDistinctCellNames();
    
    unordered_map<string, int> name_index;

    for (std::size_t i = 0; i < names.size(); ++i)
       name_index[names[i]] = static_cast<int>(i) + 1;
      


    {vector<toFromWeight> dMuscConnvec1; 
    for (const auto& conn : j["dorsal_nmj"]["weights"]["value"])
    {
    toFromWeight val;
    val.w.from = name_index[conn.at("from_cell").get<string>()];
    val.to = conn.at("to_musc").get<int>();
    val.w.weight = conn.at("weight").at("value").get<double>();
    dMuscConnvec1.push_back(val);
    }
    dMuscConnvec.swap(dMuscConnvec1);
    }

    {vector<toFromWeight> vMuscConnvec1; 
    for (const auto& conn : j["ventral_nmj"]["weights"]["value"])
    {
    toFromWeight val;
    val.w.from = name_index[conn.at("from_cell").get<string>()];
    val.to = conn.at("to_musc").get<int>();
    val.w.weight = conn.at("weight").at("value").get<double>();
    vMuscConnvec1.push_back(val);
    }
    vMuscConnvec.swap(vMuscConnvec1);
    }


    return;

    }

    if (j.contains("Dorsal NMJ")){

    vector<toFromWeight> vMuscConnvec1 = j["Ventral NMJ"]["weights"]["value"].template get< vector<toFromWeight> >();
    vector<toFromWeight> dMuscConnvec1 = j["Dorsal NMJ"]["weights"]["value"].template get< vector<toFromWeight> >();
    vMuscConnvec.swap(vMuscConnvec1);
    dMuscConnvec.swap(dMuscConnvec1);

    return;

    }
    assert(0);

}




void Worm2D::setUpMuscleConn()
{



if (hasVNCNMJ){

    assert(!hasVNC18);

    vector<toFromWeight> vMuscConnvec1 = makeMuscleConnVNCV();
    vMuscConnvec.swap(vMuscConnvec1);
       
    vector<toFromWeight> dMuscConnvec1 = makeMuscleConnVNCD();
    dMuscConnvec.swap(dMuscConnvec1);
    
    return;

}
if (hasVNC18)

{

    assert(!hasVNCNMJ);

    vector<toFromWeight> vMuscConnvec1 = makeVentralMuscleConn18();
    vMuscConnvec.swap(vMuscConnvec1);
       
    vector<toFromWeight> dMuscConnvec1 = makeDorsalMuscleConn18();
    dMuscConnvec.swap(dMuscConnvec1);
     
 return;

}


vector<toFromWeight> vMuscConnvec1 = makeVentralMuscleConn();
vector<toFromWeight> dMuscConnvec1 = makeDorsalMuscleConn();
vMuscConnvec1.swap(vMuscConnvec);
dMuscConnvec1.swap(dMuscConnvec);

}

vector<toFromWeight> Worm2D::makeMuscleConnW2D(const vector<int> & neurons, const vector<double> & NMJ,
const TVector<double> & NMJ_Gain, const vector<intPair> & unitToMusc)
{
    vector<toFromWeight> vec1;
    for (int i=0; i<unitToMusc.size();i++)
    makeMuscleConnHelp(vec1, neurons, NMJ, unitToMusc[i].ind, unitToMusc[i].val, NMJ_Gain);

    //cout << "made muscle con" << endl;
    return vec1;

}



void Worm2D::makeMuscleConnHelp(vector<toFromWeight> & vec1, 
    const vector<int> & neurons, const vector<double> & NMJs, 
    const int & unit, const int & to_muscle, const TVector<double> & NMJ_Gain)
{

    return makeMuscleConnHelp1(vec1, neurons, NMJs, unit, to_muscle, NMJ_Gain, par1.N_neuronsperunit);
}

void makeMuscleConnHelp1(vector<toFromWeight> & vec1, 
    const vector<int> & neurons, const vector<double> & NMJs, const int & unit, const int & to_muscle, 
    const TVector<double> & NMJ_Gain, const int & N_neuronsperunit)
{
    const double nmjgain = NMJ_Gain(to_muscle);
    for (int j = 0; j<neurons.size();j++){
        double weight = NMJs[j]*nmjgain; 
        int from_neuron = nn1(neurons[j],unit,N_neuronsperunit);
        toFromWeight tv({from_neuron,weight},to_muscle);
        vec1.push_back(tv);

}
}

void makeMuscleConnHelp1(vector<toFromWeight> & vec1, 
    const vector<int> & neurons, const vector<double> & NMJs, const int & unit, const int & to_muscle, 
    const vector<double> & NMJ_Gain, const int & N_neuronsperunit)
{

    for (int j = 0; j<neurons.size();j++){
        double weight = NMJs[j]*NMJ_Gain[to_muscle-1]; 
        int from_neuron = nn1(neurons[j],unit,N_neuronsperunit);
        toFromWeight tv({from_neuron,weight},to_muscle);
        vec1.push_back(tv);

}
}

///////////////////////////////
////////////////////////////////
////////////////////////////


vector<toFromWeight> Worm2Dm::makeDorsalBodyConn()
{

    return makeBodyConn();

}

vector<toFromWeight> Worm2Dm::makeVentralBodyConn()
{

    return makeBodyConn();

}

vector<toFromWeight> Worm2Dm::makeBodyConn()
{
vector<toFromWeight> vec1;

for (int to_seg = 1; to_seg<=2; to_seg++){
  int from_musc = 1;
  double weight = 0.5;
  toFromWeight tv({from_musc,weight},to_seg);
  vec1.push_back(tv);
}

for (int to_seg = 3; to_seg <= N_segments-2; to_seg++)
  {
  int from_musc = (int) ((to_seg-1)/2);
  double weight = 0.5;
  {toFromWeight tv({from_musc,weight},to_seg);
  vec1.push_back(tv);}
  toFromWeight tv({from_musc+1,weight},to_seg);
  vec1.push_back(tv);
  }

for (int to_seg = N_segments-1; to_seg<=N_segments; to_seg++){
  int from_musc = par1.N_muscles;
  double weight = 0.5;
  toFromWeight tv({from_musc,weight},to_seg);
  vec1.push_back(tv);
}

return vec1;

}

void Worm2Dm::setUpBodyConn()
{

vector<toFromWeight> vBodyConnvec1 = makeVentralBodyConn();
vector<toFromWeight> dBodyConnvec1 = makeDorsalBodyConn();
vBodyConnvec1.swap(vBodyConnvec);
dBodyConnvec1.swap(dBodyConnvec);

}

void Worm2Dm::setUpBodyConn(const json & j)
{
    
if (j.contains("dorsal_body") && j.contains("ventral_body")){
    {vector<toFromWeight> dBodyConnvec1; 
    for (const auto& conn : j["dorsal_body"]["weights"]["value"])
    {
    toFromWeight val;
    val.w.from = conn.at("from_musc").get<int>();
    val.to = conn.at("to_seg").get<int>();
    val.w.weight = conn.at("weight").at("value").get<double>();
    dBodyConnvec1.push_back(val);
    }
    dBodyConnvec.swap(dBodyConnvec1);
    }

    {vector<toFromWeight> vBodyConnvec1; 
    for (const auto& conn : j["ventral_body"]["weights"]["value"])
    {
    toFromWeight val;
    val.w.from = conn.at("from_musc").get<int>();
    val.to = conn.at("to_seg").get<int>();
    val.w.weight = conn.at("weight").at("value").get<double>();
    vBodyConnvec1.push_back(val);
    }
    vBodyConnvec.swap(vBodyConnvec1);
    }
}

else{
vector<toFromWeight> vBodyConnvec1 = j["Ventral body"]["weights"]["value"].template get< vector<toFromWeight> >();
vector<toFromWeight> dBodyConnvec1 = j["Dorsal body"]["weights"]["value"].template get< vector<toFromWeight> >();
vBodyConnvec.swap(vBodyConnvec1);
dBodyConnvec.swap(dBodyConnvec1);

}
}

//const string Worm2Dbase::getModelName() {return "Unspecified";}



void InputSwitcher::setInputOnce(const int & ind, vector<double> & externalInputs)
{
     
if (ind<0) return;
  if (static_cast<size_t>(ind) >= inds.size())
    throw runtime_error(
        "Input-switcher index " + to_string(ind)
        + " is outside the configured input range");
  vector<int> & indvec = inds[ind];
  vector<double> & valvec = vals[ind];

  //for (int i=0;i<indvec.size();i++) cout << indvec[i] << " " << valvec[i] << endl;
  //assert(0);

  for (int i=0;i<indvec.size();i++)
  {
    if (
        indvec[i] < 0
        || static_cast<size_t>(indvec[i]) >= externalInputs.size())
      throw runtime_error(
          "Input-switcher input number " + to_string(indvec[i] + 1)
          + " is outside the available driving-input range");
    externalInputs[indvec[i]] = valvec[i];
  }
  //cout << " exx ";
  //for (int i=0;i<externalInputs.size();i++) cout << " " << externalInputs[i];
  //cout << endl;

}

void InputSwitcher::resetScheduledInput()
{
  current_schedule_entry = -1;
  previous_schedule_time = -1;
}

void InputSwitcher::activateScheduleForSimulation()
{
  scheduleActive = true;
  resetScheduledInput();
}

void InputSwitcher::updateScheduledInput(
    const double & current_time, vector<double> & externalInputs)
{
  if (
      !scheduleActive
      || scheduled_input_indices.empty()
      || current_time < time_offset)
  {
    previous_schedule_time = current_time;
    return;
  }

  if (previous_schedule_time >= 0 && current_time < previous_schedule_time)
    current_schedule_entry = -1;
  previous_schedule_time = current_time;

  const double elapsed_time = current_time - time_offset;
  double cycle_time = fmod(elapsed_time, total_period);
  if (cycle_time < 0) cycle_time += total_period;

  int schedule_entry = 0;
  double boundary = timeperiods[0];
  while (
      schedule_entry + 1 < static_cast<int>(timeperiods.size())
      && cycle_time >= boundary)
  {
    schedule_entry++;
    boundary += timeperiods[schedule_entry];
  }

  if (schedule_entry != current_schedule_entry)
  {
    setInputOnce(scheduled_input_indices[schedule_entry], externalInputs);
    current_schedule_entry = schedule_entry;
  }
}

void InputSwitcher::setInputOnce(const json & j, const int & ind, vector<double> & externalInputs)
{

    assert(0 && "unfinished");
    const json & j2 = j["input_switcher"]["inputs"]["value"];
    for (auto it = j2.begin(); it != j2.end(); ++it)
    {
      //vector<int> & indvec = inds1[it->at("ind").get<int>()-1];
      //vector<double> & valvec = vals1[it->at("ind").get<int>()-1];
      const json & j3 = j2["value"];
      for (auto it2 = j3.begin(); it2 != j3.end(); ++it2)
      {
        //indvec.push_back(it2->at("ind").get<int>());
        //valvec.push_back(it2->at("val").get<double>());
      } 

    }

}



void InputSwitcher::addParsToJson(json & j) const
{
    if (inds.size()<=0) return;
    if (
        !j.contains("input_switcher")
        || !j["input_switcher"].is_object())
      j["input_switcher"] = json::object();
    json & j2 = j["input_switcher"];
    j2.erase("input_index");
    j2["doEvolution"]["value"] = doEvolution;
    j2["size"]["value"] = inds.size();
    if (timeperiods.size()>0){
    j2["time_offset"]["value"] = time_offset;
    j2["input_indices"]["value"] = scheduled_input_indices;
    j2["time_periods"]["value"] = timeperiods;
    }

  


    {
    json arr1 = json::array();
    for (int i=0;i<inds.size();i++)
    {
    const vector<int> & indvec = inds[i];
    const vector<double> & valvec = vals[i];
    json arr2 = json::array();
    for (int j=0;j<indvec.size();j++)
    arr2.push_back({{"input_num", indvec[j] + 1}, {"value", valvec[j]}});
    //arr1.push_back({{"value", arr2},{"ind", i+1}});
    arr1.push_back({{"value", arr2},{"input_index", i}});
    }
    j["input_switcher"]["inputs"]["value"] = arr1;

    }

  

}


void InputSwitcher::construct(const json & j)
{
  scheduled_input_indices.clear();
  timeperiods.clear();
  time_offset = 0;
  total_period = 0;
  doEvolution = false;
  scheduleActive = false;
  resetScheduledInput();

   if (!j.contains("input_switcher")) return;
 
  const json & input_switcher = j["input_switcher"];
  if (!input_switcher.contains("size") || !input_switcher.contains("inputs")) return;

  if (input_switcher.contains("doEvolution"))
  {
    const json & do_evolution = input_switcher.at("doEvolution");
    if (
        !do_evolution.is_object()
        || !do_evolution.contains("value")
        || !do_evolution.at("value").is_boolean())
      throw runtime_error(
          "input_switcher.doEvolution.value must be a boolean");
    doEvolution = do_evolution.at("value").get<bool>();
  }
  scheduleActive = doEvolution;

  int size = input_switcher["size"]["value"].get<int>();

  {
  
    vector<vector<int> > inds1(size);
    vector<vector<double> > vals1(size);
    const json & j2 = input_switcher["inputs"]["value"];
    bool legacy_zero_based = false;
    for (auto it = j2.begin(); it != j2.end(); ++it)
      for (const auto & input : it->at("value"))
        if (input.at("input_num").get<int>() == 0)
          legacy_zero_based = true;

    for (auto it = j2.begin(); it != j2.end(); ++it)
    {
     // vector<int> & indvec = inds1[it->at("ind").get<int>()-1];
     // vector<double> & valvec = vals1[it->at("ind").get<int>()-1];

    vector<int> & indvec = inds1[it->at("input_index").get<int>()];
    vector<double> & valvec = vals1[it->at("input_index").get<int>()];

      const json & j3 = it->at("value");
      for (auto it2 = j3.begin(); it2 != j3.end(); ++it2)
      {
        const int input_num = it2->at("input_num").get<int>();
        if (
            (!legacy_zero_based && input_num < 1)
            || (legacy_zero_based && input_num < 0))
          throw runtime_error(
              "input_switcher input_num values must be positive integers");
        indvec.push_back(
            legacy_zero_based ? input_num : input_num - 1);
        valvec.push_back(it2->at("value").get<double>());
      } 

    }

    inds.swap(inds1);
    vals.swap(vals1);
    
    
    //for (int i=0;i<inds.size();i++) 
    //for (int j=0;j<inds[i].size();j++)
    //cout << "is inds " << i << " " << j << " " <<  " iis " << inds[i][j] << " jis " <<  vals[i][j]  << endl;
    

   //assert(0);

  }

  const bool has_input_indices = input_switcher.contains("input_indices");
  const bool has_time_periods = input_switcher.contains("time_periods");
  if (has_input_indices != has_time_periods)
    throw runtime_error(
        "input_switcher.input_indices and input_switcher.time_periods "
        "must either both be present or both be absent");

  if (has_input_indices)
  {
    if (!input_switcher.contains("time_offset"))
      throw runtime_error(
          "input_switcher.time_offset is required for a timed schedule");

    time_offset =
        input_switcher["time_offset"]["value"].get<double>();
    scheduled_input_indices =
        input_switcher["input_indices"]["value"].get<vector<int> >();
    timeperiods =
        input_switcher["time_periods"]["value"].get<vector<double> >();

    if (!isfinite(time_offset) || time_offset < 0)
      throw runtime_error(
          "input_switcher.time_offset must be a finite non-negative value");
    if (
        scheduled_input_indices.empty()
        || scheduled_input_indices.size() != timeperiods.size())
      throw runtime_error(
          "input_switcher.input_indices and input_switcher.time_periods "
          "must be non-empty lists of the same length");

    for (size_t i = 0; i < scheduled_input_indices.size(); i++)
    {
      const int input_index = scheduled_input_indices[i];
      if (input_index < 0 || input_index >= size)
        throw runtime_error(
            "input_switcher.input_indices contains index "
            + to_string(input_index)
            + ", which is outside the configured input range");
      if (!isfinite(timeperiods[i]) || timeperiods[i] <= 0)
        throw runtime_error(
            "input_switcher.time_periods must contain finite positive "
            "durations in seconds");
      total_period += timeperiods[i];
    }
  }

}
