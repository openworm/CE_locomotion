#pragma once
//#include "../TSearch.h"
//#include "../VectorMatrix.h"
//#include "../argUtils.h"
#include <functional>
#include <iomanip> 
#include <string.h>
//#include "jsonUtils.h"
#include "Worm2D.h"
#include "Simulation.h"
//#include "../utils.h"
//#include "Evolvable.h"

template <typename T>
struct Callback;

template <typename Ret, typename... Params>
struct Callback<Ret(Params...)> {
   template <typename... Args> 
   static Ret callback(Args... args) {                    
      return func(args...);  
   }
   static std::function<Ret(Params...)> func; 
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> Callback<Ret(Params...)>::func;

//struct evoPars;
//struct simPars;

/* struct simPars{

  double Duration;       //
  double Transient; 

}; */



const char* getParameter(int argc, const char* argv[], string parName, const char* defaultval);



class Evolution
{
    public:
    virtual void GenPhenMapping(TVector<double> &gen, TVector<double> &phen) = 0;
    virtual double EvaluationFunction(TVector<double> &v, RandomState &rs) = 0;
    virtual void RunSimulation(TVector<double> &v, RandomState &rs) 
    {cout << "RunSim not implemented" << endl; assert(0);}
    virtual void RunSimulation(Worm2Dbase & w, RandomState &rs)
    {cout << "RunSim not implemented" << endl; assert(0);}
    void RunStandardSimulation(Worm2Dm & w, RandomState &rs);
    TVector<double> & getBestGenotype();
    TVector<double> & getBestPhenotype();

    virtual void writeJson(TVector<double> &) = 0;
    
    void addParsToJson(json & j);
    
    void configure();
    
    
    const evoPars & itsEvoPars() const {return evoPars1;}

    virtual ~Evolution()
    {
      evolfile.close();
      if (s) delete s;
    }

    string rename_file(string filename);
    //evoParsNonConst evoParsNC;
    void setFromCPT();

    protected:
    void writeJson1(Worm2Dbase & w);
    

    //void writeJson(Worm2Dbase &);
    evoPars setPars(int argc, const char* argv[], evoPars ep1);
    evoPars setPars(int argc, const char* argv[], evoPars ep1, string prefix_);
    simPars setSimPars(int argc, const char* argv[]);
    void setUp();
    void setFromEvol(const Evolution & er, int offset);

    virtual void configure_p12(){return;}

    virtual void configure_p1();
    virtual void configure_p2();
    void EvolutionaryRunDisplay(int Generation, double BestPerf, double AvgPerf, double PerfVar);
    void ResultsDisplay(TSearch &s);
    
    Evolution(int argc, const char* argv[], evoPars ep1, int VectSize_);
    Evolution(int argc, const char* argv[], evoPars ep1, int VectSize_, string prefix_);

    virtual void addExtraParsToJson(json & j) {return;}
    TSearch* const s; 
    const evoPars evoPars1;
    const simPars simPars1;
    void checkPars();

    private:

    
    TVector<double> phenotype, phenprev, genprev; //(1, itsEvoPars().VectSize);   
    ofstream evolfile, genhistfile, genhistfile2;
    const bool writeBestFlag;
    bool doResume, doneFirst, setFromCPTflag;
    int popsize;

};


/* class EvolutionFull : public Evolution
{
public:
Evolvable * const evolvable1;

void GenPhenMapping(TVector<double> &gen, TVector<double> &phen) 
{return evolvable1->GenPhenMapping(gen,phen);}
//double EvaluationFunction(TVector<double> &v, RandomState &rs)
//{return evolvable1->EvaluationFunction(v,rs);}
//void writeJson(TVector<double> & v){return evolvable1->writeJson(v);}
//void writeJson(TVector<double> & v){return evolvable1->writeJson(v);}

virtual ~EvolutionFull(){}

protected:
EvolutionFull(int argc, const char* argv[], Evolvable * evol1_)
:Evolution(argc,argv,evol1_->getDefaultEvoPars(),evol1_->getVectSize()),evolvable1(evol1_)
{
//evolvable1->ep_ptr = &evoPars1;
}


};

template<class T>
class EvolutionFullW : public EvolutionFull
{
public:
EvolutionFullW(int argc, const char* argv[]):EvolutionFull(argc, argv, new T()){}
virtual ~EvolutionFullW(){if (evolvable1) delete evolvable1;}
void writeJson(TVector<double> & pheno){T w(pheno, true);writeJson1(w);}
double EvaluationFunction(TVector<double> &v, RandomState &rs);

}; */


template<class T>
class EvolutionFullW2 : public Evolution
{
public:
EvolutionFullW2(int argc, const char* argv[]):
Evolution(argc,argv,evoPars1,T::getVectSize()){}
void writeJson(TVector<double> & pheno){T w(pheno, true);writeJson1(w);}
double EvaluationFunction(TVector<double> &v, RandomState &rs);
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen) {return T::GenPhenMapping(gen,phen);}

static inline evoPars evoPars1 =  {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        100, 2000, 0.1, 0.5, UNIFORM, 
        1.1, 0.04, 1, 0, 0, 10, 40.0, 10.0, 0.005, 23, T::getVectSize()};
};



/* template<class T>
class EvolutionT : public Evolution
{
public:

EvolutionT(int argc, const char* argv[], T * w_ptr):
Evolution(argc,argv,evol1_->getDefaultEvoPars(),evol1_->getVectSize()),evolvable1(evol1_)
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen){return T::GenPhenMapping(gen,phen);}
void writeJson(TVector<double> & pheno){T w(pheno);writeJson1(w);}
}; */

template<class T>
double EvolutionFullW2<T>::EvaluationFunction(TVector<double> &genotype, RandomState &rs){

    const double & Duration = evoPars1.Duration;
    const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;
    const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = evoPars1.Transient;
    const int & skip_steps = evoPars1.skip_steps;

    const double OSCT = 0.25 * Duration; // Cap for oscillation evaluation
    const double agarfreq = 0.44;
    const double    AvgSpeed = 0.00022;             // Average speed of the worm in meters per seconds
    const double    BBCfit = AvgSpeed*Duration;

        // Fitness
        double fitness_tr = 0.0;
        double bodyorientation, anglediff;
        double movementorientation, distancetravelled = 0, displacement, temp;
        TVector<double> curvature(1, N_curvs);
        TVector<double> antpostcurv(1, 2);
        antpostcurv.FillContents(0.0);
    
        // Evaluation of B-class neuron oscillation,and frequency in segment 2.
        // The index of B class in this segment correspond to DBs2 = 10; VBs2 = 13
        double DBp, VBp, dDB, dVB;
        double oscDB = 0, oscVB = 0;
        double FoDB, FoVB, FfDB, FfVB;
    
        double freqDB=0, freqVB=0;
        int pDB = 0, pVB = 0, signtagDB, signtagVB, signDB, signVB;
        TVector<double> peaksDB(1, 2*Duration);
        TVector<double> peaksVB(1, 2*Duration);// longer vector if you want frequencies higer than 2 Hz.
    
        
        // Genotype-Phenotype Mapping
        //TVector<double> phenotype(1, VectSize);
        //GenPhenMapping(v, phenotype);

        T w(genotype, false);

        //TVector<double> phenotype(1, VectSize);
        //GenPhenMapping(geno, phenotype);
        //setPfaFromPheno(phenotype);
        //setParsFromPheno(phenotype);
        //construct(phenotype);
        //setUpMuscleConn();
        w.InitializeState(rs);
        w.initForSimulation(rs);
        w.setStepSize(StepSize);
        
        // Transient XXX
        //w.SetAVB(0.0);
        //w.SetAVA(0.0);
        
        for (double t = 0.0; t <= Transient; t += StepSize){
            w.Step();
        }    
        
        DBp = w.n.NeuronOutput(w.Epars1.dbunit);
        VBp = w.n.NeuronOutput(w.Epars1.vbunit);
    
        w.Step(); // determine sign of derivative
    

        dDB = w.n.NeuronOutput(w.Epars1.dbunit) - DBp;
        dVB = w.n.NeuronOutput(w.Epars1.vbunit) - VBp;
        signtagDB = (dDB  > 0) ? 1 : -1;
        signtagVB = (dVB  > 0) ? 1 : -1;
        DBp = w.n.NeuronOutput(w.Epars1.dbunit);
        VBp = w.n.NeuronOutput(w.Epars1.vbunit);
        
        double xt = w.CoMx(), xtp;
        double yt = w.CoMy(), ytp;
    
        // Time loop
        for (double t = 0.0; t <= Duration; t += StepSize) {
            // Step simulation
            w.Step();
            
            ///// Oscilation
            // check changes in sign of derivative
            dDB = w.n.NeuronOutput(w.Epars1.dbunit) - DBp;
            dVB = w.n.NeuronOutput(w.Epars1.vbunit) - VBp;
            signDB = (dDB  > 0) ? 1 : ((dDB  < 0) ? -1 : 0);
            signVB = (dVB  > 0) ? 1 : ((dVB  < 0) ? -1 : 0);
    
            oscDB += abs(DBp - w.n.NeuronOutput(w.Epars1.dbunit));
            oscVB += abs(VBp - w.n.NeuronOutput(w.Epars1.vbunit));
    
            if ((signDB == -1) and (signtagDB >= 0)){
                pDB +=1;
                peaksDB[pDB] = t;
                if (pDB >= 2*Duration){return 0;};
            }
            if ((signVB == -1) and (signtagVB >= 0)){
                pVB +=1;
                peaksVB[pVB] = t;
                if (pVB >= 2*Duration){return 0;};
            }
    
            signtagDB = signDB;
            signtagVB = signVB;
            DBp = w.n.NeuronOutput(w.Epars1.dbunit);
            VBp = w.n.NeuronOutput(w.Epars1.vbunit);
            
            //// Locomotion
            // Current and past centroid position
            xtp = xt; ytp = yt;
            xt = w.CoMx(); yt = w.CoMy();
            
            // Integration error check
            if (isnan(xt) || isnan(yt) || sqrt(pow(xt-xtp,2)+pow(yt-ytp,2)) > 100*AvgSpeed*StepSize){
                return 0.0;
            }
            
            // Fitness
            bodyorientation = w.Orientation();                  // Orientation of the body position
            movementorientation = atan2(yt-ytp,xt-xtp);         // Orientation of the movement
            anglediff = movementorientation - bodyorientation;  // Check how orientations align
            temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
            distancetravelled += temp * sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));
    
        }
        // B Oscillation evaluation
        if ((pDB < 2) or (pVB < 2)){return 0;};
        for (int i = 1; i<pDB; i+=1){freqDB += (1./(pDB-1))*(1./(peaksDB[i+1]- peaksDB[i]));} 
        for (int i = 1; i<pVB; i+=1){freqVB += (1./(pVB-1))*(1./(peaksVB[i+1]- peaksVB[i]));} 
    
        FfDB = fabs(freqDB - agarfreq)/agarfreq < 1 ? fabs(freqDB - agarfreq)/agarfreq : 1;
        FfVB = fabs(freqVB - agarfreq)/agarfreq < 1 ? fabs(freqVB - agarfreq)/agarfreq : 1;
    
        FoDB = oscDB > OSCT ? 1 : oscDB / OSCT;
        FoVB = oscVB > OSCT ? 1 : oscVB / OSCT;
    
        // Locomotion evaluation
        fitness_tr = (1 - (fabs(BBCfit-distancetravelled)/BBCfit));
    
   
        return fitness_tr * FoDB * FoVB * (1 - FfDB) * (1 - FfVB);
    

}

