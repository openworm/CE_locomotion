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


////////////////////////////
/////////////////////////////


Worm2Dbase::Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_):
par1(par1_),m_ptr(m_ptr_),n_ptr(n_ptr_),
W2Dbaseparameters1b(make_shared<W2DbaseparametersNML>())
//muscForWDconst(false)
{//zeroAllInputs();
}

/* Worm2Dbase::Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc):
par1(par1_),m_ptr(m_ptr_),n_ptr(n_ptr_),W2Dbaseparameters1(make_shared<W2Dbaseparameters>()),
muscForWDconst(mfwc){} */

/* Worm2Dbase::Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc, 
    shared_ptr<W2Dbaseparameters> w2dpar_):
par1(par1_),m_ptr(m_ptr_),n_ptr(n_ptr_),W2Dbaseparameters1(w2dpar_),
muscForWDconst(mfwc)
{
 if (W2Dbaseparameters1 == nullptr) assert(0);
} */

Worm2Dbase::Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, 
    shared_ptr<W2Dparameters> w2dpar_):par1(par1_),m_ptr(m_ptr_),n_ptr(n_ptr_),
    W2Dbaseparameters1b(w2dpar_)//,W2Dbaseparameters1(new W2Dbaseparameters())
    //,muscForWDconst(false)
{
    if (W2Dbaseparameters1b == nullptr) assert(0);
    //zeroAllInputs();
}

Worm2Dm::Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_):
Worm2Dbase(par1_,n_ptr_,new Muscles()),W2Dmparscalled(false),W2Dminitcalled(false)
{setUpBodyConn();}

Worm2Dm::Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<W2Dparameters> w2dpar_):
Worm2Dbase(par1_,n_ptr_,new Muscles(), w2dpar_),W2Dmparscalled(false),W2Dminitcalled(false)
{setUpBodyConn();} 

Worm2Dm::Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, 
    shared_ptr<W2Dparameters> w2dpar_, bool dumval):
Worm2Dbase(par1_,n_ptr_,0,w2dpar_),
W2Dmparscalled(false),W2Dminitcalled(false){
    m_ptr = new c302muscForW2D(dynamic_cast<c302ForW2D&>(*n_ptr));
    setUpBodyConn();
}

//Worm2Dm::Worm2Dm(wormIzqParams par1_, shared_ptr<W2Dbaseparameters> w2dpar_):
//Worm2Dbase(par1_,new NervousSystem(),new Muscles(), w2dpar_),
//W2Dmparscalled(false),W2Dminitcalled(false){} 

/* Worm2Dm::Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc):
Worm2Dbase(par1_,n_ptr_,m_ptr_,mfwc),W2Dmparscalled(false),W2Dminitcalled(false){setUpBodyConn();} */

/* Worm2Dm::Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, 
    bool mfwc, shared_ptr<W2Dbaseparameters> w2dpar_):
Worm2Dbase(par1_,n_ptr_,m_ptr_,mfwc, w2dpar_),W2Dmparscalled(false),W2Dminitcalled(false){setUpBodyConn();} */



//Worm2D::Worm2D():m(dynamic_cast<Muscles&>(*m_ptr)){}

Worm2D::Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_):
Worm2Dm(par1_, n_ptr_),m(dynamic_cast<Muscles&>(*m_ptr)),
W2Dbaseparameters1(dynamic_pointer_cast<W2Dbaseparameters>(W2Dbaseparameters1b))
{
    //assert(0 && "what is calling this?");
    //cout << "Worm2D const" << endl;
    setUp();
    //setUpBodyConn();
}


void Worm2Dbody::InitializeState(RandomState &rs)
{
    //cout << "Worm2Dbody init state" << endl;
    InitializeBodyConstants();
    b.InitializeBodyState();
    //writeDataCheck();
    return;
}

void Worm2Dbase::InitializeState(RandomState &rs)
{  
    //cout << "Worm2Dbase init state" << endl;
    setTime(0);
    //t = 0.0;
    //datatime =  0.0;
    //writeDataCheck();
    //dataReset();
    return;
}


void Worm2Dm::InitializeState(RandomState &rs)
{
    //if (W2Dminitcalled) return;
    //cout << "Worm2Dm init state" << endl;
    Worm2Dbase::InitializeState(rs);
    Worm2Dbody::InitializeState(rs);

    //W2Dminitcalled = true;

    return;
}



void Worm2D::setUp()
{
    m.SetMuscleParams(par1.N_muscles, par1.T_muscle);  
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

double Worm2Dbody::headDistanceToCenter()
{
	//cout << "WormAgent::setDistanceToCentre()" << endl;
	return sqrt(pow(PositionX(),2) + pow(PositionY(),2));
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
    Step1(); 

    incSimTimes();
    //datatime = t;
    //datatime += StepSize_; 
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
            ofs <<  " " << b.X(i)*100.0 << " " << b.Y(i)*100.0 << " " << b.Phi(i);
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



vector<toFromWeight> vec1 = j["Driving input"]["weights"]["value"].template get< vector<toFromWeight> >();
vector<double> exvec = j["Driving input"]["strengths"]["value"].template get< vector<double> >();
externalInputs.swap(exvec);
externalInputConn.swap(vec1);

}


void Worm2Dbase::addParsToJson(json & j)
{  

    addEvolvableToJson(j);

    doubIntParamsHead par1pars = par1.getParams();
    appendToJson<double>(j[par1pars.parDoub.head],par1pars.parDoub);
    appendToJson<long>(j[par1pars.parInt.head],par1pars.parInt);

    string nsHead = "Nervous system";
   
    {Params< string > par;
    par.names = {"Model name"};
    par.vals = {getModelName()};
    appendToJson<string>(j[nsHead],par);
    }

    vector<doubIntParamsHead> parvec = getWormParams();
    for (size_t i=0;i<parvec.size(); i++) {
        if (strcmp(parvec[i].parDoub.head.c_str(),"NULL")!=0)
        appendToJson<double>(j[parvec[i].parDoub.head],parvec[i].parDoub);
        if (strcmp(parvec[i].parInt.head.c_str(),"NULL")!=0)
        appendToJson<long>(j[parvec[i].parInt.head],parvec[i].parInt);
        }

    cout << "worm2dbase add pars to json" << endl;

    setPhenoNames();
    if (phenoNames.size()>0) 
    {
        for (int i=0; i<phenoNames.size();i++) phenoNames[i] = getModelName() + "_" + phenoNames[i];
        appendVectorToJson<string>(j["PhenoNames"], phenoNames);
        appendVectorToJson<int>(j["PhenoNamesNums"], phenoNamesNums);
    }

//if (false){
    appendVectorToJson<toFromWeight>(j["Driving input"]["weights"], externalInputConn);
    j["Driving input"]["weights"]["message"] = "Weights of driving inputs to Nervous System in sparse format";
    appendVectorToJson<double>(j["Driving input"]["strengths"], externalInputs);
    j["Driving input"]["strengths"]["message"] = "Driving input strength to Nervous System in sparse format";
//}


    appendVectorToJson<toFromWeight>(j["InputNS"]["weights"], NSInputConn);
    j["InputNS"]["weights"]["message"] = "Weights of driving inputs to NS from another NS";

    appendVectorToJson<toFromWeight>(j["OutputNS"]["weights"], NSOutputConn);
    j["OutputNS"]["weights"]["message"] = "Weights of driving inputs from NS to another NS";

    W2Dbaseparameters1b->addParsToJson(j["Worm"]);
    //W2Dbaseparameters1->addParsToJson(j);

}

void Worm2Dm::addParsToJson(json & j)
{  
    if (W2Dmparscalled) return;
   
    appendVectorToJson<toFromWeight>(j["Dorsal body"]["weights"], dBodyConnvec);
    appendVectorToJson<toFromWeight>(j["Ventral body"]["weights"], vBodyConnvec);
    j["Ventral body"]["weights"]["message"] = "Ventral muscle to body weights weights in sparse format";
    j["Dorsal body"]["weights"]["message"] = "Dorsal muscle to body weights weights in sparse format";

    Worm2Dbody::addParsToJson(j);
    Worm2Dbase::addParsToJson(j);

    string nsHead = "Nervous system";
    //appendCellNamesToJson(j[nsHead], getCellNames(), par1.N_units);
    appendCellNamesToJson(j[nsHead], getCellNames(), 1);

    W2Dmparscalled = true;
}


void Worm2D::addParsToJson(json & j)
{  
     // addwormIzqParams
    Worm2Dm::addParsToJson(j);

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
   
    //addExtraParsToJson(j);
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
        for (int i = 1; i <= par1.N_units; i++) {
            for (int j = 1; j <= par1.N_neuronsperunit; j++) {
                ofs <<  " " << n_ptr->NeuronOutput(nn(j,i));
            }
        }
        // Muscles
        //ofs << "\nM: ";

        if (m_ptr){
        for (int i = 1; i <= par1.N_muscles; i++) {
            ofs <<  " " << m_ptr->DorsalMuscleOutput(i) << " " << m_ptr->VentralMuscleOutput(i);
        }
    }
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

  //setExternalInputOrig();

  n_ptr->EulerStep(settedStepSize);
  
  if (W2Dbaseparameters1->doOrigMuscInput) setMuscleInputOrig();
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
    if (j.contains("Evolutionary Optimization Parameters")){
    StepSize = j["Evolutionary Optimization Parameters"]["StepSize"]["value"]; 
    cout << "stepsize " << StepSize << endl;}
    StepSize = cmd->getArgValDoub("--StepSize",StepSize);
    if (StepSize == 0) return new c302ForW2D();
    return new c302ForW2D(StepSize);

  }
  return new NervousSystem();

}

void Worm2D::setMuscleInput()
{

    setMuscleInputVec();
    //setMuscleInputVent();
    //setMuscleInputDors();

    m.EulerStep(settedStepSize);
    //cout << "setMuscInp" << endl;
    //exit(1);
}

void Worm2D::setUpMuscleConn(const json & j)
{
    
vector<toFromWeight> vMuscConnvec1 = j["Ventral NMJ"]["weights"]["value"].template get< vector<toFromWeight> >();
vector<toFromWeight> dMuscConnvec1 = j["Dorsal NMJ"]["weights"]["value"].template get< vector<toFromWeight> >();
vMuscConnvec.swap(vMuscConnvec1);
dMuscConnvec.swap(dMuscConnvec1);

}

void Worm2D::setUpMuscleConn()
{

vector<toFromWeight> vMuscConnvec1 = makeVentralMuscleConn();
vector<toFromWeight> dMuscConnvec1 = makeDorsalMuscleConn();
vMuscConnvec1.swap(vMuscConnvec);
dMuscConnvec1.swap(dMuscConnvec);

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
    
vector<toFromWeight> vBodyConnvec1 = j["Ventral body"]["weights"]["value"].template get< vector<toFromWeight> >();
vector<toFromWeight> dBodyConnvec1 = j["Dorsal body"]["weights"]["value"].template get< vector<toFromWeight> >();
vBodyConnvec.swap(vBodyConnvec1);
dBodyConnvec.swap(dBodyConnvec1);

}




void Worm2D::makeMuscleConnHelp(vector<toFromWeight> & vec1, 
    vector<int> neurons, vector<double> NMJs, int unit, int to_muscle, TVector<double> & NMJ_Gain)
{

    return makeMuscleConnHelp1(vec1, neurons, NMJs, unit, to_muscle, NMJ_Gain, par1.N_neuronsperunit);
}

void makeMuscleConnHelp1(vector<toFromWeight> & vec1, 
    vector<int> neurons, vector<double> NMJs, int unit, int to_muscle, 
    TVector<double> & NMJ_Gain, int N_neuronsperunit)
{

    for (int j = 0; j<neurons.size();j++){
        double weight = NMJs[j]*NMJ_Gain(to_muscle); 
        int from_neuron = nn1(neurons[j],unit,N_neuronsperunit);
        toFromWeight tv({from_neuron,weight},to_muscle);
        vec1.push_back(tv);

}
}

void makeMuscleConnHelp1(vector<toFromWeight> & vec1, 
    const vector<int> & neurons, const vector<double> & NMJs, int unit, int to_muscle, 
    const vector<double> & NMJ_Gain, int N_neuronsperunit)
{

    for (int j = 0; j<neurons.size();j++){
        double weight = NMJs[j]*NMJ_Gain[to_muscle-1]; 
        int from_neuron = nn1(neurons[j],unit,N_neuronsperunit);
        toFromWeight tv({from_neuron,weight},to_muscle);
        vec1.push_back(tv);

}
}


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



//const string Worm2Dbase::getModelName() {return "Unspecified";}