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


class EvolutionFull : public Evolution
{
public:
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen) 
{return evolvable1->GenPhenMapping(gen,phen);}
double EvaluationFunction(TVector<double> &v, RandomState &rs)
{return evolvable1->EvaluationFunction(v,rs);}
void writeJson(TVector<double> & v){return evolvable1->writeJson(v);}

virtual ~EvolutionFull(){if (evolvable1) delete evolvable1;}

protected:
EvolutionFull(int argc, const char* argv[], Evolvable * evol1_)
:evolvable1(evol1_),Evolution(argc,argv,evol1_->getDefaultEvoPars(),evol1_->getVectSize())
{
evolvable1->ep_ptr = &evoPars1;
}
//shared_ptr<Evolvable> evolvable1;
Evolvable * const evolvable1;
};

template<class T>
class EvolutionFullW : public EvolutionFull
{
public:
EvolutionFullW(int argc, const char* argv[]):EvolutionFull(argc, argv, new T()){}

};