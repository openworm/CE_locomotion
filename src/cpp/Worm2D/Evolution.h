#pragma once
//#include "../TSearch.h"
//#include "../VectorMatrix.h"
//#include "../argUtils.h"
#include <functional>
#include <iomanip> 
#include <string.h>
//#include "jsonUtils.h"
//#include "WormCE.h"
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



class EvoBase
{

    protected:


    bool setFromCPTflag = false, doCPT = true;
    bool configP1Called = false;
    const evoPars evoPars1;
    TSearch* s  =  nullptr;
    const simPars simPars1;

    public:

    EvoBase(shared_ptr<const CmdArgs> cmd_, evoPars ep1, string prefix_);

    const TVector<double> & getBestGenotype();
    
    
    virtual void addParsToJson(json & j);
    
    
    
    const evoPars & itsEvoPars() const {return evoPars1;}

    virtual ~EvoBase()
    {
      evolfile.close();
      genhistfile.close();
      if (s) delete s;
    }

    string rename_file(string filename);
    //evoParsNonConst evoParsNC;
    void setFromCPT();
    void setFromCPT2(int vsize_, bool allowPreviousEvolutionFiles = true);
    const int itsVectSize() const {if (s) return s->VectorSize(); assert(0 && "s not set");}

    protected:
    void writeJson1(Worm2Dbase & w);
    void writeJson1(Worm2Dbase & w, json & j);

    //void writeJson(Worm2Dbase &);
    evoPars setPars(int argc, const char* argv[], evoPars ep1);
    evoPars setPars(int argc, const char* argv[], evoPars ep1, string prefix_);
    simPars setSimPars(int argc, const char* argv[]);
    simPars setSimPars(shared_ptr<const CmdArgs> cmd);
    evoPars setPars(shared_ptr<const CmdArgs> cmd, evoPars ep1);
    evoPars setPars(shared_ptr<const CmdArgs> cmd, evoPars ep1, string prefix_);
    evoPars getEffectiveEvoParsForJson() const;

    void setUp();
    
    void setFromEvol(const EvoBase & er, int offset, int vsize_);
    //void setPopFromBestGenoFile(int vecincsize, int offset);
    void setPopFromBestGenoFile(int offset = 0);
    //void setPopFromBestGenoFile2();
    void construct(int vsize_, int offset_);
    bool evolvedUsedMatchesActiveEvotags();
    void configure_p11();
    //void constructAll(int vsize_, int offset_);
    
    
    
    EvoBase(int argc, const char* argv[], evoPars ep1, int VectSize_);
    EvoBase(int argc, const char* argv[], evoPars ep1, int VectSize_, string prefix_);
    EvoBase(shared_ptr<const CmdArgs> cmd_, evoPars ep1, int VectSize_);
    EvoBase(shared_ptr<const CmdArgs> cmd_, evoPars ep1, int VectSize_, string prefix_);
    EvoBase(shared_ptr<const CmdArgs> cmd_, evoPars ep1);
    
   
   
    void checkPars();

    //int VectSize;

    virtual void addExtraParsToJson(json & j) {return;}


    TVector<double> phenotype;//, phenprev, genprev; //(1, itsEvoPars().VectSize);   
    ofstream evolfile, genhistfile;//, genhistfile2;
    const bool writeBestFlag;
    bool doResume;
    bool previousEvolutionFilesCompatible = true;
    int popsize;
    int initGenNum = 0;
};


class Evolution : public EvoBase
{
    protected:

    Evolution(int argc, const char* argv[], evoPars ep1, int VectSize_)
    :EvoBase(argc,argv,ep1,VectSize_){}
    Evolution(int argc, const char* argv[], evoPars ep1, int VectSize_, string prefix_)
    :EvoBase(argc,argv,ep1,VectSize_,prefix_){}
    Evolution(shared_ptr<const CmdArgs> cmd_, evoPars ep1, int VectSize_)
    :EvoBase(cmd_,ep1,VectSize_){}
    Evolution(shared_ptr<const CmdArgs> cmd_, evoPars ep1, int VectSize_, string prefix_)
    :EvoBase(cmd_,ep1,VectSize_,prefix_){}
    Evolution(shared_ptr<const CmdArgs> cmd_, evoPars ep1)
    :EvoBase(cmd_,ep1){}
    Evolution(shared_ptr<const CmdArgs> cmd_, evoPars ep1, string prefix_)
    :EvoBase(cmd_,ep1,prefix_){}


    public:

    virtual void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen) = 0;
    //{cout << "no GenPhenMapping" << endl; assert(0); return;}
    virtual void RunSimulation(TVector<double> &v, RandomState &rs) 
    {cout << "RunSim not implemented" << endl; assert(0);}
    virtual void RunSimulation(Worm2Dbase & w, RandomState &rs)
    {cout << "RunSim not implemented" << endl; assert(0);}


    void ResultsDisplay(TSearch &s);

    virtual double EvaluationFunction(TVector<double> &v, RandomState &rs) = 0;
    void RunStandardSimulation(Worm2Dm & w, RandomState &rs);

    virtual void writeJson(TVector<double> &) = 0;
    void EvolutionaryRunDisplay(int Generation, double BestPerf, double AvgPerf, double PerfVar);

    const TVector<double> & getBestPhenotype();
    
    void configure();

    protected:

    virtual void configure_p12(){return;}
    void configure_p1();
    virtual void configure_p2();

   


};

void EvolutionaryRunDisplay_try(int Generation, double BestPerf, double AvgPerf, double PerfVar);


template<class T>
class Evolvable_ptr
{
 
  

protected:
const shared_ptr<const json> json_ptr = nullptr;
shared_ptr<T> evolvable1 = nullptr;
shared_ptr<const CmdArgs> cmd = nullptr;
const shared_ptr<const W2Dparameters> evopar_ptr = nullptr;


//virtual ~Evolvable_ptr(){if (evolvable1) delete evolvable1;}
//Evolvable_ptr(shared_ptr<EvolvableS> evol1_):evolvable1(evol1_),{}

Evolvable_ptr(shared_ptr<T> evol1_, shared_ptr<const CmdArgs> cmd_):
evolvable1(evol1_),cmd(cmd_),evopar_ptr(getParameters(cmd_, evol1_)),json_ptr(nullptr){}
//{evolvable1->setWormPars(cmd_);}
Evolvable_ptr(shared_ptr<T> evol1_, shared_ptr<const CmdArgs> cmd_, shared_ptr<const json> json_ptr_):
evolvable1(evol1_),cmd(cmd_),evopar_ptr(getParameters(cmd_, evol1_, json_ptr_)),json_ptr(json_ptr_){}

//evoPars getDefaultEvoPars(int argc, const char* argv[]);
//evoPars getDefaultEvoPars(const string &);
static evoPars getDefaultEvoPars(shared_ptr<const CmdArgs> cmd, shared_ptr<T> evol1);
static evoPars getDefaultEvoPars(const string & evotype_, shared_ptr<T> evol1);

//shared_ptr<const W2Dparameters> getParameters(int argc, const char* argv[]);

//shared_ptr<const W2Dparameters> getParameters(shared_ptr<const CmdArgs> cmd_, 
//    shared_ptr<T> evol1_);

static shared_ptr<const W2Dparameters> getParameters(shared_ptr<const CmdArgs> cmd_, 
    shared_ptr<T> evol1T_, shared_ptr<const json> json_ptr_ = nullptr);


};

template<class T>
class Evolvable_ptrB: public Evolvable_ptr<T>, public Evolution
{

public:

 void addParsToJson(json & j){
    Evolution::addParsToJson(j); 
    this->evopar_ptr->addParsToJson(j["Evolutionary Optimization Parameters"]);
    this->evopar_ptr->addRootParsToJson(j);
}


protected:
    Evolvable_ptrB(shared_ptr<T> evol1_, shared_ptr<const CmdArgs> cmd_):
    Evolvable_ptr<T>(evol1_,cmd_), 
    Evolution(cmd_,this->getDefaultEvoPars(cmd_,evol1_),evol1_->getVectSize()){}

    Evolvable_ptrB(shared_ptr<T> evol1_, shared_ptr<const CmdArgs> cmd_, shared_ptr<const json> json_ptr_):
    Evolvable_ptr<T>(evol1_,cmd_,json_ptr_), 
    Evolution(cmd_,this->getDefaultEvoPars(cmd_,evol1_),evol1_->getVectSize()){}
    
    Evolvable_ptrB(shared_ptr<T> evol1_, shared_ptr<const CmdArgs> cmd_, const string & prefix_):
    Evolvable_ptr<T>(evol1_,cmd_), 
    Evolution(cmd_,this->getDefaultEvoPars(cmd_,evol1_),evol1_->getVectSize(), prefix_){}
   

    virtual shared_ptr<T> getTw() = 0;

    double Evaluation21(TVector<double> &geno, RandomState &rs);
    //double Evaluation21R(TVector<double> &genotype, RandomState &rs);
    double Evaluation21Rp1(TVector<double> &v, RandomState &rs, int direction, shared_ptr<T> w_ptr);

    double EvaluationFunction(TVector<double> &geno, RandomState &rs);
    //double Evaluation21(TVector<double> &geno, RandomState &rs);
    double Evaluation18(TVector<double> &genotype, RandomState &rs);
    double EvaluationCE(TVector<double> &genotype, RandomState &rs);
    double EvaluationCEp1(RandomState &rs, int direction, shared_ptr<T> w_ptr);
    double Evaluation21R(TVector<double> &genotype, RandomState &rs);
    //double Evaluation21Rp1(TVector<double> &v, RandomState &rs, int direction);
    double EvaluationCENZ(TVector<double> &genotype, RandomState &rs);
    double EvaluationCO(TVector<double> &genotype, RandomState &rs);
    double EvaluationCO2(TVector<double> &genotype, RandomState &rs);

    void writeJson(TVector<double> & pheno);

    void configure_p2(){
        string model_name = this->cmd->getArgVal("--modelname","");
        bool usj = this->cmd->getArgValInt("--useSupCPT",false);

        if (usj){
        EvoBase er18(this->cmd, this->getDefaultEvoPars("Evo18", this->evolvable1), "RS18_");
        setFromEvol(er18, 0, this->evolvable1->getVectSize()); 
       
       }

    Evolution::configure_p2();
    }
  
    void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen) 
    {this->evolvable1->GenPhenMapping(gen,phen);}

   

};



template<class T>
shared_ptr<const W2Dparameters> Evolvable_ptr<T>::getParameters(shared_ptr<const CmdArgs> cmd_, 
    shared_ptr<T> evol1T_, shared_ptr<const json> json_ptr_)
{
    string evotype_;
    evol1T_->template getValCJEvo<string>("evo_type", evotype_);
    //const string & evotype_ = evoPars1.evoType;

    shared_ptr<EvolvableS> evol1_ = dynamic_pointer_cast<EvolvableS>(evol1T_);

    if (json_ptr_!=nullptr){

       
    shared_ptr<W2Dparameters> w_ptr1 = nullptr;
    if (evotype_=="EvoCO" || evotype_=="EvoCO2")
    w_ptr1 = dynamic_pointer_cast<W2Dparameters>(make_shared<gradEvoPars>(cmd_));
    if (evotype_=="Evo21") 
    w_ptr1 = dynamic_pointer_cast<W2Dparameters>(
        make_shared<EvolparametersCER>(cmd_, evol1_, evotype_));
    if (evotype_=="Evo18") 
    w_ptr1 = dynamic_pointer_cast<W2Dparameters>(make_shared<AgarPars>(cmd_));
    if (evotype_=="EvoCE" || evotype_=="EvoCENZ") 
    w_ptr1 = dynamic_pointer_cast<W2Dparameters>(make_shared<EvolparametersCE>(cmd_));
    if (evotype_=="Evo21R") 
    w_ptr1 = dynamic_pointer_cast<W2Dparameters>(
        make_shared<EvolparametersCER>(cmd_, evol1_, evotype_));

    assert(w_ptr1!=nullptr);
    
    if (json_ptr_->contains("Evolutionary Optimization Parameters"))
    w_ptr1->setParsFromJson((*json_ptr_)["Evolutionary Optimization Parameters"]);
    w_ptr1->setRootParsFromJson(*json_ptr_);
    w_ptr1->setPars(cmd_);
    return w_ptr1;

    }


    if (evotype_=="EvoCO" || evotype_=="EvoCO2")
    return make_shared<const gradEvoPars>(cmd_);
    if (evotype_=="Evo21") 
    return make_shared<const EvolparametersCER>(cmd_, evol1_, evotype_);
    if (evotype_=="Evo18") 
    return make_shared<const AgarPars>(cmd_);
    if (evotype_=="EvoCE" || evotype_=="EvoCENZ") 
    return make_shared<const EvolparametersCE>(cmd_);
    if (evotype_=="Evo21R") 
    return make_shared<const EvolparametersCER>(cmd_, evol1_, evotype_);

    assert(0 && "evotype not implemented");
    return nullptr;
}


/* evoPars Evolvable_ptr::getDefaultEvoPars(int argc, const char* argv[]) 
{

    string evotype_ = getParameterString(argc,argv,"--evo_type","Evo21");
    return getDefaultEvoPars(evotype_); 

} */

template<class T>
evoPars Evolvable_ptr<T>::getDefaultEvoPars(const string & evotype_, shared_ptr<T> evol1) 
{

    //cout << "evotype " << evotype_ << endl;
    


    if (evotype_=="EvoCO" || evotype_=="EvoCO2")
        return {".", 1749493257, RANK_BASED, GENETIC_ALGORITHM, 
        26, 40, 0.05, 0.5, UNIFORM, 
        1.1, 0.1, 1, 1, 1, 1, 50, 50, evol1->itsStepSize(), 23, -1 , "", evotype_};

    if (evotype_=="Evo21" || evotype_=="Evo21R")
        return {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        100, 2000, 0.1, 0.5, UNIFORM, 
        1.1, 0.04, 1, 1, 0, 10, 40.0, 10.0, 0.005, 23, -1 , "", evotype_};

    if (evotype_=="Evo18")
        return {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        96, 1000, 0.1, 0.5, UNIFORM, 
        1.1, 0.04, 1, 1, 1, 4, 50.0, 10.0, 0.01, 23, -1 , "", evotype_ };
    
    if (evotype_== "EvoCE" || evotype_== "EvoCENZ")
        return {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        96, 10, 0.05, 0.5, UNIFORM, 
        1.1, 0.02, 1, 1, 0, 10, 24, 8.0, 0.005, 23, -1 , "", evotype_};

    assert(0 && "evotype not implemented");

}

template<class T>
evoPars Evolvable_ptr<T>::getDefaultEvoPars(shared_ptr<const CmdArgs> cmd_, shared_ptr<T> evol1) 
{
    string evotype_;
    evol1->template getValCJEvo<string>("evo_type", evotype_);
    evoPars ep1 = getDefaultEvoPars(evotype_,evol1);
    const json & j = evol1->itsBPjson();
    if (j.contains("Evolutionary Optimization Parameters"))
    {
        const json & j_evo = j.at("Evolutionary Optimization Parameters");
        int int_val;
        getJsonValTF<long>(j_evo, "randomseed", ep1.randomseed, true);
        if (getJsonValTF<int>(j_evo, "selection_mode", int_val, true) ||
            getJsonValTF<int>(j_evo, "SelectionMode", int_val, true))
            ep1.SelectionMode = static_cast<TSelectionMode>(int_val);
        if (getJsonValTF<int>(j_evo, "reproduction_mode", int_val, true) ||
            getJsonValTF<int>(j_evo, "ReproductionMode", int_val, true))
            ep1.ReproductionMode = static_cast<TReproductionMode>(int_val);
        getJsonValTF<int>(j_evo, "population_size", ep1.PopulationSize, true) ||
        getJsonValTF<int>(j_evo, "PopulationSize", ep1.PopulationSize, true);
        getJsonValTF<int>(j_evo, "max_generations", ep1.MaxGenerations, true) ||
        getJsonValTF<int>(j_evo, "MaxGenerations", ep1.MaxGenerations, true);
        getJsonValTF<double>(j_evo, "mutation_variance", ep1.MutationVariance, true) ||
        getJsonValTF<double>(j_evo, "MutationVariance", ep1.MutationVariance, true);
        getJsonValTF<double>(j_evo, "crossover_probability", ep1.CrossoverProbability, true) ||
        getJsonValTF<double>(j_evo, "CrossoverProbability", ep1.CrossoverProbability, true);
        if (getJsonValTF<int>(j_evo, "crossover_mode", int_val, true) ||
            getJsonValTF<int>(j_evo, "CrossoverMode", int_val, true))
            ep1.CrossoverMode = static_cast<TCrossoverMode>(int_val);
        getJsonValTF<double>(j_evo, "max_expected_offspring", ep1.MaxExpectedOffspring, true) ||
        getJsonValTF<double>(j_evo, "MaxExpectedOffspring", ep1.MaxExpectedOffspring, true);
        getJsonValTF<double>(j_evo, "elitist_fraction", ep1.ElitistFraction, true) ||
        getJsonValTF<double>(j_evo, "ElitistFraction", ep1.ElitistFraction, true);
        getJsonValTF<int>(j_evo, "search_constraint", ep1.SearchConstraint, true) ||
        getJsonValTF<int>(j_evo, "SearchConstraint", ep1.SearchConstraint, true);
        getJsonValTF<int>(j_evo, "checkpoint_interval", ep1.CheckpointInterval, true) ||
        getJsonValTF<int>(j_evo, "CheckpointInterval", ep1.CheckpointInterval, true);
        if (getJsonValTF<int>(j_evo, "re_evaluation_flag", int_val, true) ||
            getJsonValTF<int>(j_evo, "ReEvaluationFlag", int_val, true))
            ep1.ReEvaluationFlag = static_cast<bool>(int_val);
        getJsonValTF<int>(j_evo, "skip_steps", ep1.skip_steps, true);
        getJsonValTF<double>(j_evo, "duration", ep1.Duration, true) ||
        getJsonValTF<double>(j_evo, "Duration", ep1.Duration, true);
        getJsonValTF<double>(j_evo, "transient", ep1.Transient, true) ||
        getJsonValTF<double>(j_evo, "Transient", ep1.Transient, true);
        getJsonValTF<double>(j_evo, "step_size", ep1.StepSize, true) ||
        getJsonValTF<double>(j_evo, "StepSize", ep1.StepSize, true);
        getJsonValTF<int>(j_evo, "n_curvs", ep1.N_curvs, true) ||
        getJsonValTF<int>(j_evo, "N_curvs", ep1.N_curvs, true);
        getJsonValTF<int>(j_evo, "vect_size_temo", ep1.VectSize_temo, true) ||
        getJsonValTF<int>(j_evo, "VectSize_temo", ep1.VectSize_temo, true);
        getJsonValTF<string>(j_evo, "fileprefix", ep1.fileprefix, true);
        getJsonValTF<string>(j_evo, "evo_type", ep1.evoType, true) ||
        getJsonValTF<string>(j_evo, "evoType", ep1.evoType, true) ||
        getJsonValTF<string>(j_evo, "EvolutionType", ep1.evoType, true);
    }
    return ep1;    
}



template<class T>
class EvolutionFullW: public Evolvable_ptrB<T>
{
    public:
    
    EvolutionFullW(shared_ptr<const CmdArgs> cmd_):Evolvable_ptrB<T>(make_shared<T>(),cmd_)
    {
        this->evolvable1->setWormPars(cmd_);
    }
    //EvolutionFullW(shared_ptr<const CmdArgs> cmd_):Evolvable_ptrB<T>(getTW(),cmd_){}

    EvolutionFullW(shared_ptr<const CmdArgs> cmd_, const string & prefix_):
    Evolvable_ptrB<T>(make_shared<T>(),cmd_,prefix_)
    {
        this->evolvable1->setWormPars(cmd_);
    } 

    //EvolutionFullW(shared_ptr<const CmdArgs> cmd_, const string & prefix_):
    //Evolvable_ptrB<T>(getTW(),cmd_,prefix_){} 

    protected:
    
    shared_ptr<T> getTw(){
    //if (json_ptr!=nullptr) return make_shared<T>(*json_ptr,cmd);
    shared_ptr<T> w = make_shared<T>();
    w->setWormPars(this->cmd);
    return w;
    }
};

template<class T>
class EvolutionFullWJ: public Evolvable_ptrB<T>
{
    public:
    
    EvolutionFullWJ(const json & j, shared_ptr<const CmdArgs> cmd_):
    Evolvable_ptrB<T>(make_shared<T>(j,cmd_,true),cmd_,make_shared<const json>(j)){}
    EvolutionFullWJ(shared_ptr<json> j_ptr, shared_ptr<const CmdArgs> cmd_):
    Evolvable_ptrB<T>(make_shared<T>(j_ptr,cmd_,true),cmd_,j_ptr){}
    

    protected:

    //void writeOrigGen();
     

    shared_ptr<T> getTw(){
    if (this->json_ptr!=nullptr) return make_shared<T>(*this->json_ptr,this->cmd);
    assert(0);

    }
};



template<class T>
class EvolutionFullWC: public Evolvable_ptrB<T>
{

public:

    EvolutionFullWC(shared_ptr<const CmdArgs> cmd_):Evolvable_ptrB<T>(make_shared<T>(cmd_),cmd_){}
    EvolutionFullWC(shared_ptr<const CmdArgs> cmd_, const string & prefix_):
    Evolvable_ptrB<T>(make_shared<T>(cmd_),cmd_,prefix_){} 

    /* EvolutionFullWC(shared_ptr<const CmdArgs> cmd_):
    Evolvable_ptr<T>(make_shared<T>(cmd_),cmd_), //setFromEvolFlag(false),
    Evolution(cmd_,this->getDefaultEvoPars(cmd_),this->evolvable1->getVectSize()){}

    EvolutionFullWC(shared_ptr<const CmdArgs> cmd_, const string & prefix_):
    Evolvable_ptr<T>(make_shared<T>(cmd_),cmd_), //setFromEvolFlag(true),
    Evolution(cmd_,this->getDefaultEvoPars(cmd_),this->evolvable1->getVectSize(), prefix_){} */

    //double EvaluationFunction(TVector<double> &geno, RandomState &rs);
    //double EvaluationCO(TVector<double> &genotype, RandomState &rs);
    //double EvaluationCO2(TVector<double> &genotype, RandomState &rs);

    //void writeJson(TVector<double> & pheno);

    
    //void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen) 
    //{this->evolvable1->GenPhenMapping(gen,phen);}
    
    shared_ptr<T> getTw(){

    //if (json_ptr!=nullptr) return make_shared<T>(*json_ptr,cmd);
    return make_shared<T>(this->cmd);

    }

    //const bool setFromEvolFlag = false;
};

template<class T>
void Evolvable_ptrB<T>::writeJson(TVector<double> & pheno)
{
    
        //T w(pheno, true);
        shared_ptr<T> w_ptr = this->getTw();
        //T & w = *w_ptr; 
        //w.setWormPars(argc,argv);
        //w.setWormPars(this->cmd);


        w_ptr->setParsFromPheno(pheno);
        RandomState rs;
        rs.SetRandomSeed(evoPars1.randomseed);
    
    
        w_ptr->setStepSize(evoPars1.StepSize);
        w_ptr->setDataskips(evoPars1.skip_steps);
        w_ptr->setPrefix();
        w_ptr->InitializeData(evoPars1.directoryName);

        w_ptr->InitializeState(rs); 
        w_ptr->initForSimulation(rs);

        

        json j;
      
       
        w_ptr->addParsToJson(j);
        
        addParsToJson(j);

        //this->evopar_ptr->addParsToJson(j["Evolutionary Optimization Parameters"]);
        //wormpar_ptr->addParsToJson(j["Worm"]["Initial parameters"]);

        w_ptr->cleanLegacyParameterKeys(j);
        j.erase("Nervous system");
        j.erase("Dorsal NMJ");
        j.erase("Ventral NMJ");
        j.erase("Dorsal body");
        j.erase("Ventral body");
        j.erase("Stretch receptor");
        j.erase("VNC NMJ");
        j.erase("VNC 18");
        j.erase("Driving input");
        
        ofstream json_out(rename_file("worm_data_evo.json"));
        json_out << std::setw(4) << j << std::endl;
        json_out.close(); 




       // writeJson1(*w_ptr,j);
}


/* template<class T>
void EvolutionFullWC<T>::writeJson(TVector<double> & pheno){
        //T w(pheno, true);
        T w(this->cmd);
        //w.setWormPars(argc,argv);
        //w.setWormPars(cmd);
        w.setParsFromPheno(pheno);
        json j;
        this->evopar_ptr->addParsToJson(j["Evolutionary Optimization Parameters"]);
        //wormpar_ptr->addParsToJson(j["Worm"]["Initial parameters"]);
        writeJson1(w,j);
    }
 */




template<class T>
double Evolvable_ptrB<T>::EvaluationFunction(TVector<double> &genotype, RandomState &rs)
{
    if (evoPars1.evoType=="Evo21") return Evaluation21(genotype,rs);
    if (evoPars1.evoType=="Evo18") return Evaluation18(genotype,rs);
    if (evoPars1.evoType=="EvoCE") return EvaluationCE(genotype,rs);
    if (evoPars1.evoType=="Evo21R") return Evaluation21R(genotype,rs);
    if (evoPars1.evoType=="EvoCENZ") return EvaluationCENZ(genotype,rs);
    if (evoPars1.evoType=="EvoCO") return EvaluationCO(genotype,rs);
    if (evoPars1.evoType=="EvoCO2") return EvaluationCO2(genotype,rs);


    assert(0 && "Type not implemented");
    //if (evoPars1.evoType=="EvoCE") return EvaluationCE(genotype,rs);
    
}

/* template<class T>
double EvolutionFullWC<T>::EvaluationFunction(TVector<double> &genotype, RandomState &rs)
{
   
    assert(0 && "Type not implemented");
    //if (evoPars1.evoType=="EvoCE") return EvaluationCE(genotype,rs);
    
} */


template<class T>
//double EvolutionFullW<T>::Evaluation21(TVector<double> &genotype, RandomState &rs)
double Evolvable_ptrB<T>::Evaluation21(TVector<double> &genotype, RandomState &rs)
{
 
    shared_ptr<T> w_ptr = this->getTw();
    w_ptr->setParsFromGeno(genotype);
    //w_ptr->setInputOnce(2); 
   
    return Evaluation21Rp1(genotype,rs,2, w_ptr);

}




template<class T>
double Evolvable_ptrB<T>::Evaluation21R(TVector<double> &genotype, RandomState &rs)
{

    //vector<double> initial_genotype(genotype.Size());
    //for (int i=0;i<genotype.Size();i++) initial_genotype[i]=genotype(i+1);
   
    shared_ptr<T> w_ptr = this->getTw();




    shared_ptr<const EvolparametersCER> Epars1 = 
    dynamic_pointer_cast<const EvolparametersCER>(this->evopar_ptr);
    //const EvolparametersCER & Epars1 = dynamic_cast<const EvolparametersCER&>(*evopar_ptr);

    
    const int gen_num = s->Generation();
    //evoPars1.MaxGenerations;
    //Epars1.doAlternateEvo;

    const bool doalt1 = Epars1->doReverse==3 && (gen_num < evoPars1.MaxGenerations/2);
    const bool doalt2 = Epars1->doReverse==3 && (gen_num >= evoPars1.MaxGenerations/2);
    const bool doalt1f = Epars1->doReverse==2 || (doalt1);
    const bool doalt2f = Epars1->doReverse==2 || (doalt2);

    //cout << "reverse is " << Epars1->doReverse << endl;
   // assert(0);

    //double fitnessForward, fitnessBackward;

    //genotype(SR_A)= -1.0;
    //genotype(SR_B)= srb;
    //return EvaluationCEp1(genotype, rs, 1); 

   // double sra = genotype(1);
   // double srb = genotype(2);


    double fitness = 0;
    int count = 0;

    if (Epars1->doReverse==0 || doalt1f)
    {
    if (Epars1->zeroGainsType == 1) w_ptr->itsEf.itsJson["condval"] = 0;
    w_ptr->setParsFromGeno(genotype);
    w_ptr->setInputOnce(0);
    fitness += Evaluation21Rp1(genotype, rs, 1, w_ptr);
    count++;
    }

    if (Epars1->doReverse==1 || doalt2f)
    {
    if (Epars1->zeroGainsType == 1)  w_ptr->itsEf.itsJson["condval"] = 1;  
    w_ptr->setParsFromGeno(genotype);
    w_ptr->setInputOnce(1);
    fitness += Evaluation21Rp1(genotype, rs, -1, w_ptr);
    count++;
    }


    //for (int i=0;i<genotype.Size();i++) genotype(i+1)=initial_genotype[i];

    /* double fitness = 0;
    int count = 0;
    if (Epars1->doReverse==0 || doalt1f){
    if (Epars1->zeroGainsType == 1) genotype(1)= -1.0;
    genotype(2)= srb;
    fitness += Evaluation21Rp1(genotype, rs, 1);
    count++;
    }
    if (Epars1->doReverse==1 || doalt2f){
    genotype(1)= sra;
    if (Epars1->zeroGainsType == 1) genotype(2)= -1.0;
    fitness += Evaluation21Rp1(genotype, rs, -1);
    count++;
    }

    genotype(1) = sra;
    genotype(2) = srb; */

    return fitness/count;

    //if (Epars1->doReverse==0) return fitnessForward;
    //if (Epars1->doReverse==1) return fitnessBackward;
    //if (Epars1->doReverse==2) return (fitnessForward + fitnessBackward)/2;

    //assert(0 && "doReverse not set properly");
    // return fitnessBackward;
}


template<class T>
double Evolvable_ptrB<T>::Evaluation21Rp1(TVector<double> &genotype, 
    RandomState &rs, 
    const int direction,
    shared_ptr<T> w_ptr){

    const double & Duration = evoPars1.Duration;
    //const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;
    const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = evoPars1.Transient;
    const int & skip_steps = evoPars1.skip_steps;

  
    shared_ptr<const EvolparametersCER> EparsR = 
    dynamic_pointer_cast<const EvolparametersCER>(this->evopar_ptr);

    //const EvolparametersCER & EparsR = dynamic_cast<const EvolparametersCER&>(*(this->evopar_ptr));
    //const Evolparameters & EparsR = dynamic_cast<const Evolparameters&>(*evopar_ptr);

//    assert(0);
  
    if (EparsR == nullptr) assert(0);

    const double OSCT =  EparsR->OSCTbase* Duration;

    //const double OSCT = 0.25 * Duration; // Cap for oscillation evaluation
    //const double agarfreq = 0.44;
    //const double    AvgSpeed = 0.00022;
    const double AvgSpeed = EparsR->AvgSpeed;    // Average speed of the worm in meters per seconds
    const double BBCfit = AvgSpeed*Duration;
    const double agarfreq = EparsR->agarfreq;

    const bool doAngleDiff = EparsR->doAngleDiff;
    const int fitType = EparsR->fitType;

    const int dbunit = EparsR->dbunit;
    const int vbunit = EparsR->vbunit;
    const bool dodir = (direction == 1 || direction == 2);

        // Fitness
        double fitness_tr = 0.0;
        double bodyorientation, anglediff;
        double movementorientation, distancetravelled = 0, displacement, temp;
        //TVector<double> curvature(1, N_curvs);
        //TVector<double> antpostcurv(1, 2);
        //antpostcurv.FillContents(0.0);
    
        // Evaluation of B-class neuron oscillation,and frequency in segment 2.
        // The index of B class in this segment correspond to DBs2 = 10; VBs2 = 13
        double DBp, VBp, dDB, dVB;
        double oscDB = 0, oscVB = 0;
        double FoDB, FoVB, FfDB, FfVB;
    
        double freqDB=0, freqVB=0;
        int pDB = 0, pVB = 0, signtagDB, signtagVB, signDB, signVB;

        TVector<double> peaksDB(1, 2*Duration);
        TVector<double> peaksVB(1, 2*Duration);// longer vector if you want frequencies higer than 2 Hz.
        peaksDB.FillContents(0.0);
        peaksVB.FillContents(0.0);

        
        // Genotype-Phenotype Mapping
        //TVector<double> phenotype(1, VectSize);
        //GenPhenMapping(v, phenotype);

        //T w;
        //shared_ptr<T> w_ptr = this->getTw();

        T & w = *w_ptr; 


        //(genotype, false);
        //w.setWormPars(argc,argv);
       
        //w.setWormPars(&*wormpar_ptr);
        //w.setWormPars(this->cmd);

        //w.setParsFromGeno(genotype);
     
        //w.setEvolPars(EparsR,evoPars1.evoType);

        //TVector<double> phenotype(1, VectSize);
        //GenPhenMapping(geno, phenotype);
        //setPfaFromPheno(phenotype);
        //setParsFromPheno(phenotype);
        //construct(phenotype);
        //setUpMuscleConn();
    
        if (false)
    {NervousSystem * n_ptr1 = dynamic_cast<NervousSystem*>(w.n_ptr);
    cout << "evo21 states kkds " << n_ptr1->states << endl << endl;
    cout << "evo21 biases kkds " << n_ptr1->biases << endl;
    cout << "evo21 taus kkds " << n_ptr1->taus << endl << endl;
    //assert(0);
    }

    w.InitializeState(rs);
    w.initForSimulation(rs);
    w.setStepSize(StepSize);
if (false)
    {NervousSystem * n_ptr1 = dynamic_cast<NervousSystem*>(w.n_ptr);
    cout << "evo21 states jsjs " << n_ptr1->states << endl << endl;
    cout << "evo21 biases jsjs " << n_ptr1->biases << endl;
    cout << "evo21 taus jsjs " << n_ptr1->taus << endl << endl;
    //assert(0);
    }

      

        
     /*    shared_ptr<W2DCEparsA> w1 = dynamic_pointer_cast<W2DCEparsA>(w.W2Dbaseparameters1b);

       if (w1 != nullptr) {

        //shared_ptr<W2DCEparsA> w1 = dynamic_pointer_cast<W2DCEparsA>(w.W2)
        //W2DCEparsA w1(dynamic_cast<const W2DCEparsA&>(*wormpar_ptr));


        // Transient XXX
        //w.SetAVB(0.0);
        //w.SetAVA(0.0);
        
       if (direction == 1){
        w1->AVA_output =  0.0;
        w1->AVB_output =  1.0;
        }
        else if (direction == -1) {
        w1->AVA_output =  1.0;
        w1->AVB_output =  0.0; // Command Interneuron Activation Backward
        }
        else if (direction == 2)
        {
        w1->AVA_output =  0.0;
        w1->AVB_output =  0.0; 
        }

        else assert(0 && "direction not set properly");


        } */

        //w.setWormPars(&w1);
     
      

        for (double t = 0.0; t <= Transient; t += StepSize){
            w.Step();
        }    

        //cout << "EparsR->dbunit " << EparsR->dbunit << endl;
        //cout << "EparsR->vbunit " << EparsR->vbunit << endl;
       
        //assert(0);
       
        DBp = w.n_ptr->NeuronOutput(dbunit);
        VBp = w.n_ptr->NeuronOutput(vbunit);
    
        //cout << "db " << DBp << " " << VBp << endl;

        w.Step(); // determine sign of derivative
    

        dDB = w.n_ptr->NeuronOutput(dbunit) - DBp;
        dVB = w.n_ptr->NeuronOutput(vbunit) - VBp;
        signtagDB = (dDB  > 0) ? 1 : -1;
        signtagVB = (dVB  > 0) ? 1 : -1;
        DBp = w.n_ptr->NeuronOutput(dbunit);
        VBp = w.n_ptr->NeuronOutput(vbunit);
        
        double xt = w.CoMx(), xtp;
        double yt = w.CoMy(), ytp;
      
        // Time loop
        for (double t = 0.0; t <= Duration; t += StepSize) {
            // Step simulation
            w.Step();
            
            ///// Oscilation
            // check changes in sign of derivative
            dDB = w.n_ptr->NeuronOutput(dbunit) - DBp;
            dVB = w.n_ptr->NeuronOutput(vbunit) - VBp;
            signDB = (dDB  > 0) ? 1 : ((dDB  < 0) ? -1 : 0);
            signVB = (dVB  > 0) ? 1 : ((dVB  < 0) ? -1 : 0);
    
            oscDB += abs(DBp - w.n_ptr->NeuronOutput(dbunit));
            oscVB += abs(VBp - w.n_ptr->NeuronOutput(vbunit));
    
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
            DBp = w.n_ptr->NeuronOutput(dbunit);
            VBp = w.n_ptr->NeuronOutput(vbunit);
            
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
            movementorientation = atan2(yt-ytp,xt-xtp);
            
            if (doAngleDiff)
            anglediff = angle_diff(movementorientation,bodyorientation);
            else
            // Orientation of the movement
            anglediff = movementorientation - bodyorientation;  // Check how orientations align
            if (dodir){
            if (fitType == 0)
            temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
            else temp = cos(anglediff);
            }
            else{
            if (fitType == 0) 
            temp = cos(anglediff) > 0.0 ? -1.0 : 1.0;           // Add to fitness only movement backward
            else temp = cos(anglediff)*-1;
            }
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





template<class T>
double Evolvable_ptrB<T>::EvaluationCE(TVector<double> &genotype, RandomState &rs)
{
  
    //const EvolparametersCE & Epars1 = dynamic_cast<const EvolparametersCE&>(*this->evopar_ptr);

    //Epars1.show();
    //assert(0);

    //vector<double> initial_genotype(genotype.Size());
    //for (int i=0;i<genotype.Size();i++) initial_genotype[i]=genotype(i+1);
   
    shared_ptr<T> w_ptr = this->getTw();
 

    int zeroGainsType;
    w_ptr->getValCJ(
        "sr_zero_gains_type", zeroGainsType, "stretch_receptor"
    );
    int doReverse;
    w_ptr->getValCJWorm("do_reverse", doReverse);

    

    //cout << "zeroGainsType " << zeroGainsType << endl;
    //cout << "doReverse " << doReverse << endl;

   
    
    //const int SR_A = 1;
    //const int SR_B = 2;
 
    const int gen_num = s->Generation();
    //evoPars1.MaxGenerations;
    //Epars1.doAlternateEvo;

    //const bool doalt1 = Epars1.doReverse==3 && (gen_num < evoPars1.MaxGenerations/2);
    //const bool doalt2 = Epars1.doReverse==3 && (gen_num >= evoPars1.MaxGenerations/2);
    //const bool doalt1f = Epars1.doReverse==2 || (doalt1);
    //const bool doalt2f = Epars1.doReverse==2 || (doalt2);


    const bool doalt1 = doReverse==3 && (gen_num < evoPars1.MaxGenerations/2);
    const bool doalt2 = doReverse==3 && (gen_num >= evoPars1.MaxGenerations/2);
    const bool doalt1f = doReverse==2 || (doalt1);
    const bool doalt2f = doReverse==2 || (doalt2);


    //cout << "reverse is " << Epars1.doReverse << endl;

    //double sra = genotype(SR_A);
    //double srb = genotype(SR_B);

   // assert(0);

    //double fitnessForward, fitnessBackward;

    //genotype(SR_A)= -1.0;
    //genotype(SR_B)= srb;
    //return EvaluationCEp1(genotype, rs, 1); 

   
   // w_ptr->itsEf.itsJson["f_ind"] = 2;
       

    double fitness = 0;
    int count = 0;

    if (doReverse==0 || doalt1f)
    {
    if (zeroGainsType == 1) w_ptr->itsEf.itsJson["condval"] = 0;
    w_ptr->setParsFromGeno(genotype);
    w_ptr->setInputOnce(0);
    fitness += EvaluationCEp1(rs, 1, w_ptr);
    count++;
    }

    if (doReverse==1 || doalt2f)
    {
    if (zeroGainsType == 1) w_ptr->itsEf.itsJson["condval"] = 1;  
    w_ptr->setParsFromGeno(genotype);
    w_ptr->setInputOnce(1);
    fitness += EvaluationCEp1(rs, -1, w_ptr);
    count++;
    }

    //genotype(SR_A) = sra;
    //genotype(SR_B) = srb;
  
    //for (int i=0;i<genotype.Size();i++) genotype(i+1)=initial_genotype[i];
   
    return fitness/count;

    //if (Epars1.doReverse==0) return fitnessForward;
    //if (Epars1.doReverse==1) return fitnessBackward;
    //if (Epars1.doReverse==2) return (fitnessForward + fitnessBackward)/2;

    //assert(0 && "doReverse not set properly");
    // return fitnessBackward;
}

template<class T>
double Evolvable_ptrB<T>::EvaluationCENZ(TVector<double> &genotype, RandomState &rs)
{

    const EvolparametersCE & Epars1 = dynamic_cast<const EvolparametersCE&>(*this->evopar_ptr);

    //Epars1.show();
    //assert(0);
    shared_ptr<T> w_ptr = this->getTw();

    //const int SR_A = 1;
    //const int SR_B = 2;
 
    const int gen_num = s->Generation();
    //evoPars1.MaxGenerations;
    //Epars1.doAlternateEvo;

    const bool doalt1 = Epars1.doReverse==3 && (gen_num < evoPars1.MaxGenerations/2);
    const bool doalt2 = Epars1.doReverse==3 && (gen_num >= evoPars1.MaxGenerations/2);
    const bool doalt1f = Epars1.doReverse==2 || (doalt1);
    const bool doalt2f = Epars1.doReverse==2 || (doalt2);

    //cout << "reverse is " << Epars1.doReverse << endl;

    //double sra = genotype(SR_A);
    //double srb = genotype(SR_B);

   // assert(0);

    //double fitnessForward, fitnessBackward;

    //genotype(SR_A)= -1.0;
    //genotype(SR_B)= srb;
    //return EvaluationCEp1(genotype, rs, 1); 

    double fitness = 0;
    int count = 0;
    if (Epars1.doReverse==0 || doalt1f){
        //  assert(0 && "dorev0");
    //genotype(SR_A)= -1.0;
    //genotype(SR_B)= srb;
      w_ptr->setParsFromGeno(genotype);
    w_ptr->setInputOnce(0);
    fitness += EvaluationCEp1(rs, 1, w_ptr);
    count++;
    }
    if (Epars1.doReverse==1 || doalt2f){
       // assert(0 && "dorev1");
    //genotype(SR_A)= sra;
    //genotype(SR_B)= -1.0;
      w_ptr->setParsFromGeno(genotype);
    w_ptr->setInputOnce(1);
    fitness += EvaluationCEp1(rs, -1, w_ptr);
    count++;
    }

    //genotype(SR_A) = sra;
    //genotype(SR_B) = srb;

    return fitness/count;

    //if (Epars1.doReverse==0) return fitnessForward;
    //if (Epars1.doReverse==1) return fitnessBackward;
    //if (Epars1.doReverse==2) return (fitnessForward + fitnessBackward)/2;

    //assert(0 && "doReverse not set properly");
    // return fitnessBackward;
}






template<class T>
double Evolvable_ptrB<T>::EvaluationCEp1(
    RandomState &rs, int direction,
    shared_ptr<T> w_ptr)
{

   
   

  const double & Duration = evoPars1.Duration;
  //const int & VectSize = evoPars1.VectSize;
  const double & StepSize = evoPars1.StepSize;
  const double & Transient = evoPars1.Transient;

  //const AgarPars & EparsR = dynamic_cast<const AgarPars&>(*evopar_ptr);
    //const EvolparametersCE & EparsR = dynamic_cast<const EvolparametersCE&>(*evopar_ptr);

    //shared_ptr<const EvolparametersCE> EparsR = 
    //dynamic_pointer_cast<const EvolparametersCE>(this->evopar_ptr);
  
    int fitType; //, doAngleDiff;
    w_ptr->getValCJWorm("fit_type", fitType);
    //w_ptr->getValCJWorm("do_angle_diff", doAngleDiff);

 
    double AvgSpeed;
    w_ptr->getValCJWorm("avg_speed", AvgSpeed);


    //const double AvgSpeed = EparsR->AvgSpeed;
    
    //const double    AvgSpeed = 0.0001; //0.00022;              // Average speed of the worm in meters per seconds
    

    //cout << "AS " << AvgSpeed << " " << fitType << endl;

    
    //assert(0);
    //const double    AvgSpeed = 0.0001;


    const double    BBCfit = AvgSpeed*evoPars1.Duration;

    double fitA,fitB;
    double bodyorientation, anglediff;
    double movementorientation, distancetravelled = 0, temp;
    double distance;
    double xt, xtp, oxt, fxt;
    double yt, ytp, oyt, fyt;

    // Genotype-Phenotype Mapping
    //TVector<double> phenotype(1, VectSize);
    //GenPhenMapping(v, phenotype);
    //WormCE w(phenotype, 1);
    //w.InitializeState(rs);
    //assert(0);

    //T w(argc,argv,genotype);

    //T w;
    //shared_ptr<T> w_ptr = this->getTw();
    T & w = *w_ptr; 
   
        if (false)
    {NervousSystem * n_ptr1 = dynamic_cast<NervousSystem*>(w.n_ptr);
    cout << "evoCE states kkds " << n_ptr1->states << endl << endl;
    cout << "evoCE biases kkds " << n_ptr1->biases << endl;
    cout << "evoCE taus kkds " << n_ptr1->taus << endl << endl;
    //assert(0);
    }
    

    //T w(genotype, false);
    //w.setWormPars(&*wormpar_ptr);
    //w.setWormPars(argc,argv);
  
    //w.setWormPars(this->cmd);
   // w.setParsFromGeno(genotype);
 
    

    //EvolparametersCE & Epars1 = w.getWormPars();

    //EvolparametersCE & Epars1 = dynamic_cast<EvolparametersCE&>(*evopar_ptr);
        //TVector<double> phenotype(1, VectSize);
        //GenPhenMapping(geno, phenotype);
        //setPfaFromPheno(phenotype);
        //setParsFromPheno(phenotype);
        //construct(phenotype);
        //setUpMuscleConn();

    w.InitializeState(rs);
    w.initForSimulation(rs);
    w.setStepSize(StepSize);


    if (false)
    {NervousSystem * n_ptr1 = dynamic_cast<NervousSystem*>(w.n_ptr);
    cout << "evo states kkds " << n_ptr1->states << endl << endl;
    cout << "evo biases kkds " << n_ptr1->biases << endl;
    cout << "evo taus kkds " << n_ptr1->taus << endl << endl;
    //assert(0);
    }

    if (false)
    {
    json j1;
        w.addParsToJson(j1);
        cout << "j1 ss" << j1["Stretch receptor"]["SR_A_gain"] << endl;
        cout << "j1 ss" << j1["Stretch receptor"]["SR_B_gain"] << endl;
         cout << "j1 ss" << j1["Driving input"] << endl;

    }

    //shared_ptr<Worm2DSRb> ws1 = dynamic_pointer_cast<Worm2DSRb>(w_ptr);
    //const SRCE* srptr = dynamic_cast<SRCE*>(w_ptr->w2dsr_ptr);

    //EvolparametersCE & Epars1 = dynamic_cast<EvolparametersCE&>(*evopar_ptr);
    //WormCE & w2 = dynamic_cast<WormCE&>(w);

    //W2DCEparsA w1(dynamic_cast<const W2DCEparsA&>(*wormpar_ptr));

    //shared_ptr<W2DCEparsA> w1 = dynamic_pointer_cast<W2DCEparsA>(w.W2Dbaseparameters1b);
    //assert(w1!=nullptr);

    //w1.show();
    //assert(0);

  /*   if (false){
    if (direction == 1){
        w.setInputOnce(0);
     
    }
    else if  (direction == -1) {
        w.setInputOnce(1);
    }
    else if  (direction == 2)
    {
        w.setInputOnce(2);
    }
    else assert(0 && "direction not set properly");
    } */
   

    //w1.show();
    //assert(0);

    //w.setWormPars(&w1);

    //w.setEvolPars(evopar_ptr, evoPars1.evoType);

    // Transient
    for (double t = 0.0; t <= Transient; t += StepSize){
        w.Step(StepSize);
    }

   

    xt = w.CoMx(); yt = w.CoMy();
    oxt = w.CoMx(); oyt = w.CoMy();
    // Run
    for (double t = 0.0; t <= Duration; t += StepSize) {
        w.Step(StepSize);
        // Current and past centroid position
        xtp = xt; ytp = yt;
        xt = w.CoMx(); yt = w.CoMy();
        // Integration error check
        if (isnan(xt) || isnan(yt) || sqrt(pow(xt-xtp,2)+pow(yt-ytp,2)) > 10*AvgSpeed*StepSize) {return 0.0;}
        // Velocity Fitness
        bodyorientation = w.Orientation();                  // Orientation of the body position
        movementorientation = atan2(yt-ytp,xt-xtp);         // Orientation of the movement
        anglediff = movementorientation - bodyorientation;  // Check how orientations align
         if (direction == 1 || direction == 2){
            if (fitType == 0)
            temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
            else temp = cos(anglediff);
            }
            else{
            if (fitType == 0) 
            temp = cos(anglediff) > 0.0 ? -1.0 : 1.0;           // Add to fitness only movement backward
            else temp = cos(anglediff)*-1;
            }

        distancetravelled += temp * sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));
    }
    fxt = w.CoMx(); fyt = w.CoMy();
    distance = sqrt(pow(oxt-fxt,2)+pow(oyt-fyt,2));
    fitA = 1 - (fabs(BBCfit - distance)/BBCfit);
    fitA = (fitA > 0)? fitA : 0.0;

    fitB = 1 - (fabs(BBCfit-distancetravelled)/BBCfit);
    fitB = (fitB > 0)? fitB : 0.0;


    //cout << "evoCE fitness " << fitB << endl;
    return fitB;


}

template<class T>
double Evolvable_ptrB<T>::EvaluationCO(TVector<double> &genotype, RandomState &rs)
{

//	cout << "EvolutionCO::EvaluationFunction(TVector<double> &v, RandomState &rs, WormAgent * Worm)" << endl;

	//TVector<double> phenotype;
	//phenotype.SetBounds(1, evoPars1.VectSize);
	//GenPhenMapping(v, phenotype);
	
	//WormAgent Worm(CircuitSize);

    const double & Duration = evoPars1.Duration;
    const double & StepSize = evoPars1.StepSize;
    const double & Transient = evoPars1.Transient;
    
    shared_ptr<T> w_ptr = this->getTw();
    T & w = *w_ptr; 

    w.setStepSize(StepSize);

    w.InitializeState(rs);
    w.setParsFromGeno(genotype);

        
    //w.initForSimulation(rs);
    
    
    //shared_ptr<gradParameters> w1 = dynamic_pointer_cast<gradParameters>(w.W2Dbaseparameters1b);
    //assert(w1!=nullptr);

	//RandomState rs2 = rs;
	//Worm->InitializeState(rs2);
	//Worm->SetParameters(phenotype);
	//Worm->setStepSize(evoPars1.StepSize);

    WormGrad & wg = dynamic_cast<WormGrad&>(w);
    
    const double Pi	=	3.1415926;

    double MaxDist;
    w.getValCJWorm("max_dist", MaxDist);

    double rundur = Transient + Duration;
    w.setValCJWorm("RunDuration", rundur);
    

    //double rd;
    //w.getValCJWorm("run_duration",rd);


    //cout << "rd ... " << Transient + Duration << " " << rd << " " << MaxDist << endl;
    //assert(0);

	double f = 0, accdist = 0, totaldist = 0;
	int k = 0;
	double fitness = 0.0;
	int taxis =0 ,kinesis = 0;
	for (int mode = 1; mode <= 1; mode++)
	{
		if (mode==0){taxis = 0;kinesis = 1;}
		else {taxis = 1;kinesis = 0;}

        w.setValCJWorm("taxis",taxis);
        w.setValCJWorm("kinesis",kinesis);

		for (double gradSteep = 0.5; gradSteep <= 0.5; gradSteep += 0.2)
            {

            w.setValCJWorm("gradSteep",gradSteep);
            wg.setGradientSteepness(gradSteep);

			for (double orient = 0.0; orient < 2*Pi; orient += Pi/2)
			{

                w.setValCJWorm("orient",orient);
      
                

                //w1->worm_rotation = orient;
              /*   w1->orient_orig = orient;
                w1->gradSteep = gradSteep;
                w1->RunDuration = Transient + Duration;
                w1->HSStepSize = StepSize;
                w1->taxis = taxis;
                w1->kinesis = kinesis; */

				//Worm->setSimPars(orient,
				//	gradSteep,
				//	evoPars1.Transient + evoPars1.Duration,
				//	evoPars1.StepSize, taxis, kinesis);

				//Worm->InitializeSimulation(rs);
				//Worm->initForSimulation(rs);

                w.initForSimulation(rs);

                //wg.ResetAgentsBody();
                //wg.InitializeSensors(rs);

				/* Worm->InitialiseAgent(2*RunDuration, evoPars1.StepSize);
				Worm->ResetAgentsBody(orient, rs);
				Worm->ResetChemCon(gradSteep);
				Worm->ResetAgentIntState(rs);
				Worm->UpdateChemCon(gradSteep); */
 
				for (int repeats = 1; repeats <= 2; repeats++)
				{
                    wg.ResetAgentsBody();
                    //wg.InitializeSensors(rs);
				
					w.setTime(0);
					for (double t = StepSize; t <= Transient; t += StepSize)
					{
						//Worm->setStepPars(gradSteep,rs,t,taxis,kinesis);
						//Worm->Step(evoPars1.StepSize);
						w.Step();

						//Worm->UpdateSensors();
						//Worm->Step(evoPars1.StepSize,rs,t,taxis,kinesis);
						//Worm->UpdateChemCon(gradSteep);
					}
					accdist = 0.0;
					w.setTime(0);
					for (double t = StepSize; t <= Duration; t += StepSize)
					{
						//Worm->setStepPars(gradSteep,rs,t,taxis,kinesis);
						//Worm->Step(evoPars1.StepSize);
						w.Step();
						
						//Worm->UpdateSensors();
						//Worm->Step(evoPars1.StepSize,rs,t,taxis,kinesis);
						//Worm->UpdateChemCon(gradSteep);
                    
                        accdist += wg.distanceToCenter();
                        //accdist += wg.DistanceToCentre();

						//accdist += Worm->DistanceToCentre();
						//cout << "D " << Worm->DistanceToCentre() << endl;
					}
					totaldist = (accdist/(Duration/StepSize));
					f = (MaxDist - totaldist)/MaxDist;
					f = f < 0 ? 0.0 : f;
					fitness += f;
					k++;
					//cout << k << " " << totaldist << endl;
				}
			}
		}
	}

   
	return fitness/k;
}




template<class T>
double Evolvable_ptrB<T>::EvaluationCO2(TVector<double> &genotype, RandomState &rs)
{

//	cout << "EvolutionCO::EvaluationFunction(TVector<double> &v, RandomState &rs, WormAgent * Worm)" << endl;

	//TVector<double> phenotype;
	//phenotype.SetBounds(1, evoPars1.VectSize);
	//GenPhenMapping(v, phenotype);
	
	//WormAgent Worm(CircuitSize);

    const double & Duration = evoPars1.Duration;
    const double & StepSize = evoPars1.StepSize;
    const double & Transient = evoPars1.Transient;

    //T w(this->cmd);
    //w.setStepSize(StepSize);

    shared_ptr<T> w_ptr = this->getTw();
    T & w = *w_ptr; 

    //w.setWormPars(cmd);
    w.setParsFromGeno(genotype);


    w.setStepSize(StepSize);
    w.InitializeState(rs);
    w.initForSimulation(rs);
    
    //shared_ptr<gradParameters> w1 = dynamic_pointer_cast<gradParameters>(w.W2Dbaseparameters1b);
    //assert(w1!=nullptr);
    
	//RandomState rs2 = rs;
	//Worm->InitializeState(rs2);
	//Worm->SetParameters(phenotype);
	//Worm->setStepSize(evoPars1.StepSize);

    const double Pi	=	3.1415926;

    //w1->resetAgentBody = true;         
    //w1->orient_orig = Pi;
    //w1->RunDuration = Transient + Duration;
    //w1->HSStepSize = StepSize;
              
    double rundur = Transient + Duration;
    w.setValCJWorm("RunDuration",rundur);
    w.setValCJWorm("resetAgentBody",true);
    w.setValCJWorm("orient",Pi);

    double MaxDist;
    w.getValCJWorm("max_dist", MaxDist);

    WormGrad & wg = dynamic_cast<WormGrad&>(w);
    
    

	double f, accdist, totaldist;
	int k = 0;
	double fitness = 0.0;
	int taxis,kinesis;
	for (int mode = 1; mode <= 1; mode++)
	{
		if (mode==0){taxis = 0;kinesis = 1;}
		else {taxis = 1;kinesis = 0;}

        w.setValCJWorm("taxis",taxis);
        w.setValCJWorm("kinesis",kinesis);

		for (double gradSteep = 0.5; gradSteep <= 0.5; gradSteep += 0.2)
			{
	                  w.setValCJWorm("gradSteep",gradSteep);
                  wg.setGradientSteepness(gradSteep);

			for (double orient = 0.0; orient < 2*Pi; orient += Pi/2)
            //for (double orient = 0.0; orient < 2*Pi; orient +=2* Pi)
            //for (int i1=0;i1<2;i1++)
			{

                //double orient = 0.0;
                w.setValCJWorm("rotation",orient);

                //w1->worm_rotation = orient;
                //w1->gradSteep = gradSteep;
                //w1->taxis = taxis;
                //w1->kinesis = kinesis;

				//Worm->setSimPars(orient,
				//	gradSteep,
				//	evoPars1.Transient + evoPars1.Duration,
				//	evoPars1.StepSize, taxis, kinesis);

				//Worm->InitializeSimulation(rs);
				//Worm->initForSimulation(rs);

                

				/* Worm->InitialiseAgent(2*RunDuration, evoPars1.StepSize);
				Worm->ResetAgentsBody(orient, rs);
				Worm->ResetChemCon(gradSteep);
				Worm->ResetAgentIntState(rs);
				Worm->UpdateChemCon(gradSteep); */
 
				for (int repeats = 1; repeats <= 1; repeats++)
				{   
                    w.InitializeState(rs);
                    w.initForSimulation(rs);
					//wg.ResetAgentsBody();
                    //wg.InitializeSensors(rs);
					w.setTime(0);
					for (double t = StepSize; t <= Transient; t += StepSize)
					{
						//Worm->setStepPars(gradSteep,rs,t,taxis,kinesis);
						//Worm->Step(evoPars1.StepSize);
						w.Step();

						//Worm->UpdateSensors();
						//Worm->Step(evoPars1.StepSize,rs,t,taxis,kinesis);
						//Worm->UpdateChemCon(gradSteep);
					}
					accdist = 0.0;
					w.setTime(0);
					for (double t = StepSize; t <= Duration; t += StepSize)
					{
						//Worm->setStepPars(gradSteep,rs,t,taxis,kinesis);
						//Worm->Step(evoPars1.StepSize);
						w.Step();
						
						//Worm->UpdateSensors();
						//Worm->Step(evoPars1.StepSize,rs,t,taxis,kinesis);
						//Worm->UpdateChemCon(gradSteep);
                        double dtc = wg.distanceToCenter();
                        accdist += dtc;
                        //cout << "dtc " << w1->MaxDist << " " << dtc << endl;
						//accdist += Worm->DistanceToCentre();
						//cout << "D " << Worm->DistanceToCentre() << endl;
					}
					totaldist = (accdist/(Duration/StepSize));
					f = (MaxDist - totaldist)/MaxDist;
					f = f < 0 ? 0.0 : f;
                    //cout << "f " << f << endl;
					fitness += f;
					k++;
					//cout << k << " " << totaldist << endl;
				}
			}
		}
	}

  
    //assert(0);
	return fitness/k;
}


template<class T>
double Evolvable_ptrB<T>::Evaluation18(TVector<double> &genotype, RandomState & rs)
{
    double fitness;
    //ofstream fitfile;
  
    const double & Duration = evoPars1.Duration;
    //const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;
    const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = evoPars1.Transient;
    //const int & skip_steps = evoPars1.skip_steps;


    //assert(0);

    shared_ptr<const AgarPars> EparsR = dynamic_pointer_cast<const AgarPars>(this->evopar_ptr);

    if (EparsR == nullptr) assert(0);

    const double AvgSpeed = EparsR->AvgSpeed;
    const double BBCfit = AvgSpeed*Duration;


    fitness = 0.0;
    double bodyorientation, anglediff;
    double movementorientation, distancetravelled = 0, temp;
 
    shared_ptr<T> w_ptr = this->getTw();
    T & w = *w_ptr; 
    //w.setWormPars(this->cmd);

    //cout << "evo18 geno " << genotype << endl;
    //assert(0);

    w.setParsFromGeno(genotype);

    if (false)
    {
    NervousSystem * n_ptr1 = dynamic_cast<NervousSystem*>(w.n_ptr);
    json j1;   
    appendElecNSToJson(j1, *n_ptr1);
    //j1["vbc"]  = n_ptr1->chemicalweights;
    cout << j1 << endl;
    }
    
    if (false)
    {NervousSystem * n_ptr1 = dynamic_cast<NervousSystem*>(w.n_ptr);
    cout << "evo18 states kkds " << n_ptr1->states << endl << endl;
    cout << "evo18 biases kkds " << n_ptr1->biases << endl;
    cout << "evo18 taus kkds " << n_ptr1->taus << endl << endl;
    //assert(0);
    }

    w.InitializeState(rs);
    w.initForSimulation(rs);
    w.setStepSize(StepSize);

    if (false)
    {NervousSystem * n_ptr1 = dynamic_cast<NervousSystem*>(w.n_ptr);
    cout << "evo18 states jsjs " << n_ptr1->states << endl << endl;
    cout << "evo18 biases jsjs " << n_ptr1->biases << endl;
    cout << "evo18 taus jsjs " << n_ptr1->taus << endl << endl;
    //assert(0);
    }

    if (false){
    Worm2D * const w2d = dynamic_cast<Worm2D *>(&w);
    if (w2d){
    json j1;
    j1["vbc"]  = w2d->itsvBodyConnvec();
    cout << j1 << endl;
    }
}

    

    // Transient
    for (double t = 0.0; t <= Transient; t += StepSize)
    {
        w.Step();

    }

    if (false)
    {NervousSystem * n_ptr1 = dynamic_cast<NervousSystem*>(w.n_ptr);
    cout << "evo18 states iqw " << n_ptr1->states << endl << endl;
    cout << "evo18 biases iqw " << n_ptr1->biases << endl;
    cout << "evo18 taus iqw " << n_ptr1->taus << endl << endl;
    //assert(0);
    }


    double xt = w.CoMx(), xtp;
    double yt = w.CoMy(), ytp;

    //cout << "xxs " << xt << " " << yt << endl;

    

    // Time loop
    for (double t = 0.0; t <= Duration; t += StepSize) {

        w.Step();

        // Current and past centroid position
        xtp = xt; ytp = yt;
        xt = w.CoMx(); yt = w.CoMy();

        // Integration error check
        if (isnan(xt) || isnan(yt) || sqrt(pow(xt-xtp,2)+pow(yt-ytp,2)) > 100*AvgSpeed*StepSize)
        {
            return 0.0;
        }

        // Fitness
        bodyorientation = w.Orientation();                  // Orientation of the body position
        movementorientation = atan2(yt-ytp,xt-xtp);         // Orientation of the movement
        anglediff = movementorientation - bodyorientation;  // Check how orientations align
        temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
        distancetravelled += temp * sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));

    }
    fitness = 1 - (fabs(BBCfit-distancetravelled)/BBCfit);


    //assert(0);

    //cout << "fitness " << fitness << endl;

    return fitness;
}
