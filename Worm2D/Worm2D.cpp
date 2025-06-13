#include "Worm2D.h"
#include <iomanip>

//using json = nlohmann::json;

vector<toFromWeight> dummyVec()
{
   toFromWeight tv({0,0},0);
   vector<toFromWeight> vec1;
    vec1.push_back(tv);
    return vec1;
}

void DataWriter::resetStats(bool & firstcall, size_t & pos, int & tt, string name_)
{
 
    if (firstcall || !isOpen[pos]){
      ofsvec.push_back(ofstream(getName(name_)));
      pos = ofsvec.size() - 1;
      isOpen.push_back(true);
      firstcall = false;
      tt = dataskips;
     }

}

void DataWriter::closeAll(){for (int i=0; i<ofsvec.size(); i++) {ofsvec[i].close(); isOpen[i]=false;}}
    
string DataWriter::getName(string name_){
        if (prefix!="") return basename + "/" + prefix + "_" + name_;
        else return basename + "/" + name_;
 }


Worm2Dm::Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_):
par1(par1_),m_ptr(m_ptr_),n_ptr(n_ptr_),
muscForWDconst(false){}

Worm2Dm::Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc):
par1(par1_),m_ptr(m_ptr_),n_ptr(n_ptr_),
muscForWDconst(mfwc){}




void Worm2Dbody::InitializeState(RandomState &rs)
{

    b.InitializeBodyState();
    return;
}

void Worm2Dm::InitializeState(RandomState &rs)
{
    t = 0.0;
    Worm2Dbody::InitializeState(rs);
    return;
}

int Worm2Dm::nn(int neuronNumber, int unitNumber)
{   
    if (unitNumber==1) return neuronNumber;
    return neuronNumber+((unitNumber-1)*par1.N_neuronsperunit);
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

void Worm2Dm::Step(double StepSize_) { //StepSize = StepSize_; 
    Step1(StepSize_); t += StepSize_; datatime = t;}


double Worm2Dbody::getVelocity()
{
   static double xtp =  CoMx();
   static double ytp =  CoMy();

    double xt = CoMx(); 
    double yt = CoMy();
    double vel = sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));
    xtp = xt;
    ytp = yt;
    return vel;

}

void Worm2Dm::DumpNSOrdered(ofstream &ofs, int skips)
{

    //const int NSsize = dynamic_cast<NervousSystem&>(*n_ptr).size;
    static int tt = skips;

    if (++tt >= skips) {
        tt = 0;
        ofs << t;
        for (int i = 1; i <= par1.N_size; i++) ofs <<  " " << n_ptr->NeuronOutput(i);
        ofs << "\n";
    }
}


void Worm2Dm::DumpVal(ofstream &ofs, int skips, double val)
{
    static int tt = skips;

    if (++tt >= skips) {
        tt = 0;

        ofs << t << " " << val;
    
        ofs << "\n";
    }
}


void Worm2Dbody::writeData()
{
writeBody();
writeCurvature();
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

    static bool firstcall = true;
    static size_t pos;
    static int tt;

    resetStats(firstcall,pos,tt,"curv.dat");


    ofstream & ofs = ofsvec[pos];  
   

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
    static bool firstcall = true;
    static size_t pos;
    static int tt;
    
    resetStats(firstcall,pos,tt,"body.dat");

 
    ofstream & ofs = ofsvec[pos];  
   

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


/* void Worm2Dm::DumpBodyState(ofstream &ofs, int skips)
{
    static int tt = skips;

    if (++tt >= skips) {
        tt = 0;

        ofs << t;
        // Body
        for (int i = 1; i <= N_rods; i++)
        {
            ofs <<  " " << b.X(i) << " " << b.Y(i) << " " << b.Phi(i);
        }
        ofs << "\n";
    }
}
 */
void Worm2Dm::writeJsonFile(ofstream & json_out)
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

void Worm2Dm::addParsToJson(json & j)
{  
    doubIntParamsHead par1pars = par1.getParams();
    appendToJson<double>(j[par1pars.parDoub.head],par1pars.parDoub);
    appendToJson<long>(j[par1pars.parInt.head],par1pars.parInt);

    Worm2Dbody::addParsToJson(j);
    
    string nsHead = "Nervous system";
    //appendCellNamesToJson(j[nsHead], getCellNames(), par1.N_units);
    appendCellNamesToJson(j[nsHead], getCellNames(), 1);

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
}


void Worm2D::addParsToJson(json & j)
{  
     // addwormIzqParams
    Worm2Dm::addParsToJson(j);


    appendMuscleToJson(j,m);
    
   
    j["Ventral NMJ"]["weights"]["message"] = "Ventral NMJ weights weights in sparse format";
    appendMatrixToJson(j["Ventral NMJ"]["weights"], vMuscConn.weights, vMuscConn.numConns, vMuscConn.size);
    j["Dorsal NMJ"]["weights"]["message"] = "Dorsal NMJ weights weights in sparse format";
    appendMatrixToJson(j["Dorsal NMJ"]["weights"], dMuscConn.weights, dMuscConn.numConns, dMuscConn.size);
    
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


    
    appendCellNamesToJson(j["Dorsal NMJ"], getDMuscNames(), 1);
    appendCellNamesToJson(j["Ventral NMJ"], getVMuscNames(), 1);
   
    //addExtraParsToJson(j);
}

void Worm2Dm::writeData()
{
writeAct();
writeState();
Worm2Dbody::writeData();
}

void Worm2Dm::writeAct()
{
    static bool firstcall = true;
    static size_t pos;
    static int tt;
    
    resetStats(firstcall,pos,tt,"act.dat");

  
   
    ofstream & ofs = ofsvec[pos];  


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


void Worm2Dm::writeState()
{

    static bool firstcall = true;
    static size_t pos;
    static int tt;
    resetStats(firstcall,pos,tt,"state.dat");
 
    ofstream & ofs = ofsvec[pos];  

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

void Worm2D::setMuscleInputVent()
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

    for (int i = 1; i<= dMuscConn.size; i++){
        double tot = 0;
    for (int j = 1; j <= dMuscConn.numConns(i); j++){
        tot +=  dMuscConn.weights[i][j].weight*n_ptr->NeuronOutput(dMuscConn.weights[i][j].from);
    }
    m.SetDorsalMuscleInput(i, tot);
    }

}




void Worm2D::setMuscleInput(double StepSize)
{

    
    setMuscleInputVent();
    setMuscleInputDors();

    m.EulerStep(StepSize);
    //cout << "setMuscInp" << endl;
    //exit(1);
}

void Worm2D::setUpMuscleConn()
{
vMuscConn.setWeights(makeVentralMuscleConn());
dMuscConn.setWeights(makeDorsalMuscleConn());
}

void Worm2D::makeMuscleConnHelp(vector<toFromWeight> & vec1, 
    vector<int> neurons, vector<double> NMJs, int mi, int to, TVector<double> & NMJ_Gain)
{

    for (int j = 0; j<neurons.size();j++){
        double weight = NMJs[j]*NMJ_Gain(to); 
        int from = nn(neurons[j],mi);
        toFromWeight tv({from,weight},to);
        vec1.push_back(tv);

}
}


Worm2D::Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_):Worm2Dm(par1_, n_ptr_, new Muscles),
m(dynamic_cast<Muscles&>(*m_ptr)),vMuscConn(par1_.N_muscles),dMuscConn(par1_.N_muscles)
{
    //cout << "Worm2D const" << endl;
    setUp();
}


void Worm2D::setUp()
{
    m.SetMuscleParams(par1.N_muscles, par1.T_muscle);  
    //InitializeState(rs);
}

void Worm2D::InitializeState(RandomState &rs)
{
    Worm2Dm::InitializeState(rs);
    m.InitializeMuscleState();
    return;
}

const string Worm2Dm::getModelName() {return "Unspecified";}