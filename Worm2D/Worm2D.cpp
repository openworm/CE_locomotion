#include "Worm2D.h"
#include <iomanip>

//using json = nlohmann::json;

void Worm2D::setMuscleInput()
{

    for (int i = 1; i<= vMuscConn.size; i++){
        double tot = 0;
    for (int j = 1; j <= vMuscConn.numConns(i); j++){
        tot +=  vMuscConn.weights[i][j].weight*n_ptr->NeuronOutput(vMuscConn.weights[i][j].from);
    }
    m.SetVentralMuscleInput(i, tot);
    }
    for (int i = 1; i<= dMuscConn.size; i++){
        double tot = 0;
    for (int j = 1; j <= dMuscConn.numConns(i); j++){
        tot +=  dMuscConn.weights[i][j].weight*n_ptr->NeuronOutput(dMuscConn.weights[i][j].from);
    }
    m.SetDorsalMuscleInput(i, tot);
    }
    //cout << "setMuscInp" << endl;
    //exit(1);
}

void Worm2D::setUpMuscleConn()
{
vMuscConn.setWeights(makeVentralMuscleConn());
dMuscConn.setWeights(makeDorsalMuscleConn());
}


Worm2D::Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_):par1(par1_),n_ptr(n_ptr_),
vMuscConn(par1_.N_muscles),dMuscConn(par1_.N_muscles)
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
    t = 0.0;
    b.InitializeBodyState();
    m.InitializeMuscleState();
    return;
}

int Worm2D::nn(int neuronNumber, int unitNumber)
{
    return neuronNumber+((unitNumber-1)*par1.N_neuronsperunit);
}


//WormIzq::WormIzq(wormIzqParams par1_):Worm2D(par1_, new NervousSystem()),
//n(dynamic_cast<NervousSystem&>(*n_ptr))
//{}


/* WormIzq::WormIzq(wormIzqParams par1_, const NervousSystemBase & n):par1(par1_),n_ptr(n.clone())
{
    setUp();
} */

/* void WormIzq::setUp()
{
    m.SetMuscleParams(par1.N_muscles, par1.T_muscle);
}
 */

/* int WormIzq::nn(int neuronNumber, int unitNumber)
{
    return neuronNumber+((unitNumber-1)*par1.N_neuronsperunit);
} */

/* void WormIzq::InitializeState(RandomState &rs)
{
    t = 0.0;
    b.InitializeBodyState();
    m.InitializeMuscleState();
}
 */

double Worm2D::CoMx()
{
    double temp = 0.0;
    for (int i = 1; i <= N_rods; i++) {
        temp += b.X(i);
    }
    return temp/N_rods;
}

double Worm2D::CoMy()
{
    double temp = 0.0;
    for (int i = 1; i <= N_rods; i++) {
        temp += b.Y(i);
    }
    return temp/N_rods;
}

void Worm2D::Curvature(TVector<double> &c)
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

double Worm2D::Orientation()
{
    return atan2(b.Y(Head)-b.Y(Tail),b.X(Head)-b.X(Tail));
}

void Worm2D::AngleCurvature(TVector<double> &c)
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

void Worm2D::DumpCurvature(ofstream &ofs, int skips)
{

  double dx1,dy1,dx2,dy2,a,a1,a2,seg;
  static int tt = skips;

  if (++tt >= skips) {
    tt = 0;
    //time
    ofs << t;

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
    ofs << "\n";
  }
}

double Worm2D::getVelocity()
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

void Worm2D::DumpNSOrdered(ofstream &ofs, int skips)
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


void Worm2D::DumpVal(ofstream &ofs, int skips, double val)
{
    static int tt = skips;

    if (++tt >= skips) {
        tt = 0;

        ofs << t << " " << val;
    
        ofs << "\n";
    }
}

void Worm2D::DumpBodyState(ofstream &ofs, int skips)
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

void Worm2D::writeJsonFile(ofstream & json_out)
{

    json j;
    addParsToJson(j);
    //ofstream json_out(supArgs1.rename_file("worm_data.json"));
    //ofstream json_out("worm_data.json");
    json_out << std::setw(4) << j << std::endl;
    //json_out.close();

}

void Worm2D::addParsToJson(json & j)
{  
     // addwormIzqParams
    doubIntParamsHead par1pars = par1.getParams();
    appendToJson<double>(j[par1pars.parDoub.head],par1pars.parDoub);
    appendToJson<long>(j[par1pars.parInt.head],par1pars.parInt);

    appendBodyToJson(j, b);
    appendMuscleToJson(j,m);
    
    vector<doubIntParamsHead> parvec = getWormParams();
    for (size_t i=0;i<parvec.size(); i++) {
        if (strcmp(parvec[i].parDoub.head.c_str(),"NULL")!=0)
        appendToJson<double>(j[parvec[i].parDoub.head],parvec[i].parDoub);
        if (strcmp(parvec[i].parInt.head.c_str(),"NULL")!=0)
        appendToJson<long>(j[parvec[i].parInt.head],parvec[i].parInt);
        }
    
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


    
    appendCellNamesToJson(j["Dorsal NMJ"], getDMuscNames(), 1);
    appendCellNamesToJson(j["Ventral NMJ"], getVMuscNames(), 1);
   
    //addExtraParsToJson(j);
}
