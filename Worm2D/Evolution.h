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
    const evoPars evoPars1;
    TSearch* s  =  nullptr;
    const simPars simPars1;

    public:

    EvoBase(shared_ptr<const CmdArgs> cmd_, evoPars ep1, string prefix_);

    const TVector<double> & getBestGenotype();
    

    
    
    void addParsToJson(json & j);
    
    
    
    
    const evoPars & itsEvoPars() const {return evoPars1;}

    virtual ~EvoBase()
    {
      evolfile.close();
      if (s) delete s;
    }

    string rename_file(string filename);
    //evoParsNonConst evoParsNC;
    void setFromCPT();
    void setFromCPT2();
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

    void setUp();
    
    void setFromEvol(const EvoBase & er, int offset);
    //void setPopFromBestGenoFile(int vecincsize, int offset);
    void setPopFromBestGenoFile(int offset = 0);
    //void setPopFromBestGenoFile2();
    void construct(int vsize_, int offset_);
    void configure_p11();
    
    
    
    
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
    bool doResume, setFromCPTflag = false, doCPT;//, doneFirst
    int popsize;
    bool configP1Called = false;
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
shared_ptr<T> evolvable1;

//virtual ~Evolvable_ptr(){if (evolvable1) delete evolvable1;}
//Evolvable_ptr(shared_ptr<EvolvableS> evol1_):evolvable1(evol1_),{}

Evolvable_ptr(shared_ptr<T> evol1_, shared_ptr<const CmdArgs> cmd_):
evolvable1(evol1_),cmd(cmd_),evopar_ptr(getParameters(cmd_, evol1_)){}
//{evolvable1->setWormPars(cmd_);}




//evoPars getDefaultEvoPars(int argc, const char* argv[]);
//evoPars getDefaultEvoPars(const string &);
evoPars getDefaultEvoPars(shared_ptr<const CmdArgs> cmd);
evoPars getDefaultEvoPars(const string & evotype_);

//shared_ptr<const W2Dparameters> getParameters(int argc, const char* argv[]);
static shared_ptr<const W2Dparameters> getParameters(shared_ptr<const CmdArgs> cmd_, 
    shared_ptr<T> evol1_);

shared_ptr<const CmdArgs> cmd;
const shared_ptr<const W2Dparameters> evopar_ptr;

};





template<class T>
shared_ptr<const W2Dparameters> Evolvable_ptr<T>::getParameters(shared_ptr<const CmdArgs> cmd_, 
    shared_ptr<T> evol1T_)
{
    string evotype_ = cmd_->getArgVal("--evoType","Evo21");
    //const string & evotype_ = evoPars1.evoType;

    shared_ptr<EvolvableS> evol1_ = dynamic_pointer_cast<EvolvableS>(evol1T_);

    if (evotype_=="EvoCO" || evotype_=="EvoCO2") 
    return shared_ptr<const gradEvoPars>(new const gradEvoPars(cmd_));
    if (evotype_=="Evo21") 
    return shared_ptr<const EvolparametersCER>(new const EvolparametersCER(cmd_, evol1_, evotype_));
    if (evotype_=="Evo18") 
    return shared_ptr<const AgarPars>(new const AgarPars(cmd_));
    if (evotype_=="EvoCE" || evotype_=="EvoCENZ") 
    return shared_ptr<const EvolparametersCE>(new const EvolparametersCE(cmd_));
    if (evotype_=="Evo21R") 
    return shared_ptr<const EvolparametersCER>(new const EvolparametersCER(cmd_, evol1_, evotype_));

    assert(0 && "evotype not implemented");
    return nullptr;
}


/* evoPars Evolvable_ptr::getDefaultEvoPars(int argc, const char* argv[]) 
{

    string evotype_ = getParameterString(argc,argv,"--evoType","Evo21");
    return getDefaultEvoPars(evotype_); 

} */

template<class T>
evoPars Evolvable_ptr<T>::getDefaultEvoPars(const string & evotype_) 
{

    if (evotype_=="EvoCO" || evotype_=="EvoCO2")
        return {".", 1749493257, RANK_BASED, GENETIC_ALGORITHM, 
        26, 40, 0.05, 0.5, UNIFORM, 
        1.1, 0.1, 1, 0, 1, 1, 50, 50, evolvable1->itsStepSize(), 23, -1 , "", evotype_};

    if (evotype_=="Evo21" || evotype_=="Evo21R")
        return {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        100, 2000, 0.1, 0.5, UNIFORM, 
        1.1, 0.04, 1, 0, 0, 10, 40.0, 10.0, 0.005, 23, -1 , "", evotype_};

    if (evotype_=="Evo18")
        return {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        96, 1000, 0.1, 0.5, UNIFORM, 
        1.1, 0.04, 1, 0, 1, 4, 50.0, 10.0, 0.01, 23, -1 , "", evotype_ };
    
    if (evotype_== "EvoCE" || evotype_== "EvoCENZ")
        return {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        96, 10, 0.05, 0.5, UNIFORM, 
        1.1, 0.02, 1, 0, 0, 10, 24, 8.0, 0.005, 23, -1 , "", evotype_};

    assert(0 && "evotype not implemented");

}

template<class T>
evoPars Evolvable_ptr<T>::getDefaultEvoPars(shared_ptr<const CmdArgs> cmd_) 
{
    string evotype_ = cmd_->getArgVal("--evoType","Evo21");
    return getDefaultEvoPars(evotype_);    
}

template<class T>
class EvolutionFullW: public Evolvable_ptr<T>, public Evolution
{
    public:
    
    EvolutionFullW(shared_ptr<const CmdArgs> cmd_):
    Evolvable_ptr<T>(make_shared<T>(),cmd_), 
    Evolution(cmd_,this->getDefaultEvoPars(cmd_),this->evolvable1->getVectSize())
    {this->evolvable1->setWormPars(cmd_);}
    EvolutionFullW(shared_ptr<const CmdArgs> cmd_, const string & prefix_):
    Evolvable_ptr<T>(make_shared<T>(),cmd_), 
    Evolution(cmd_,this->getDefaultEvoPars(cmd_),this->evolvable1->getVectSize(), prefix_)
    {this->evolvable1->setWormPars(cmd_);}


    double EvaluationFunction(TVector<double> &geno, RandomState &rs);
    double Evaluation21(TVector<double> &geno, RandomState &rs);
    double Evaluation18(TVector<double> &genotype, RandomState &rs);
    double EvaluationCE(TVector<double> &genotype, RandomState &rs);
    double EvaluationCEp1(TVector<double> &v, RandomState &rs, int direction);
    double Evaluation21R(TVector<double> &genotype, RandomState &rs);
    double Evaluation21Rp1(TVector<double> &v, RandomState &rs, int direction);
    double EvaluationCENZ(TVector<double> &genotype, RandomState &rs);
    

    void writeJson(TVector<double> & pheno);

  
    void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen) 
    {this->evolvable1->GenPhenMapping(gen,phen);}



    protected:
    

};


template<class T>
class EvolutionFullWC: public Evolvable_ptr<T>, public Evolution
{

public:

    EvolutionFullWC(shared_ptr<const CmdArgs> cmd_):
    Evolvable_ptr<T>(make_shared<T>(cmd_),cmd_), //setFromEvolFlag(false),
    Evolution(cmd_,this->getDefaultEvoPars(cmd_),this->evolvable1->getVectSize()){}

    EvolutionFullWC(shared_ptr<const CmdArgs> cmd_, const string & prefix_):
    Evolvable_ptr<T>(make_shared<T>(cmd_),cmd_), //setFromEvolFlag(true),
    Evolution(cmd_,this->getDefaultEvoPars(cmd_),this->evolvable1->getVectSize(), prefix_){}

    double EvaluationFunction(TVector<double> &geno, RandomState &rs);
    double EvaluationCO(TVector<double> &genotype, RandomState &rs);
    double EvaluationCO2(TVector<double> &genotype, RandomState &rs);
    void writeJson(TVector<double> & pheno);

    void configure_p2(){
        string model_name = this->cmd->getArgVal("--modelname","");

        //if (false){
        if (model_name == "CO18Full"){
        EvoBase er18(this->cmd, this->getDefaultEvoPars("Evo18"), "RS18_");
        setFromEvol(er18, 0); 
       
       }

    Evolution::configure_p2();
    }

    void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen) 
    {this->evolvable1->GenPhenMapping(gen,phen);}

    //const bool setFromEvolFlag = false;
};

template<class T>
void EvolutionFullW<T>::writeJson(TVector<double> & pheno){
        //T w(pheno, true);
        T w;
        //w.setWormPars(argc,argv);
        w.setWormPars(this->cmd);
        w.setParsFromPheno(pheno);
        json j;
        this->evopar_ptr->addParsToJson(j["Evolutionary Optimization Parameters"]);
        //wormpar_ptr->addParsToJson(j["Worm"]["Initial parameters"]);
        writeJson1(w,j);
    }


template<class T>
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





template<class T>
double EvolutionFullW<T>::EvaluationFunction(TVector<double> &genotype, RandomState &rs)
{
    if (evoPars1.evoType=="Evo21") return Evaluation21(genotype,rs);
    if (evoPars1.evoType=="Evo18") return Evaluation18(genotype,rs);
    if (evoPars1.evoType=="EvoCE") return EvaluationCE(genotype,rs);
    if (evoPars1.evoType=="Evo21R") return Evaluation21R(genotype,rs);
    if (evoPars1.evoType=="EvoCENZ") return EvaluationCENZ(genotype,rs);

    assert(0 && "Type not implemented");
    //if (evoPars1.evoType=="EvoCE") return EvaluationCE(genotype,rs);
    
}

template<class T>
double EvolutionFullWC<T>::EvaluationFunction(TVector<double> &genotype, RandomState &rs)
{
    if (evoPars1.evoType=="EvoCO") return EvaluationCO(genotype,rs);
    if (evoPars1.evoType=="EvoCO2") return EvaluationCO2(genotype,rs);

    assert(0 && "Type not implemented");
    //if (evoPars1.evoType=="EvoCE") return EvaluationCE(genotype,rs);
    
}


template<class T>
double EvolutionFullW<T>::Evaluation21(TVector<double> &genotype, RandomState &rs)
{

    return Evaluation21Rp1(genotype,rs,2);

}




template<class T>
double EvolutionFullW<T>::Evaluation21R(TVector<double> &genotype, RandomState &rs)
{

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

    double sra = genotype(1);
    double srb = genotype(2);


    double fitness = 0;
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
    genotype(2) = srb;

    return fitness/count;

    //if (Epars1->doReverse==0) return fitnessForward;
    //if (Epars1->doReverse==1) return fitnessBackward;
    //if (Epars1->doReverse==2) return (fitnessForward + fitnessBackward)/2;

    //assert(0 && "doReverse not set properly");
    // return fitnessBackward;
}


template<class T>
double EvolutionFullW<T>::Evaluation21Rp1(TVector<double> &genotype, RandomState &rs, int direction){

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

       
        T w; //(genotype, false);
        //w.setWormPars(argc,argv);
       
        //w.setWormPars(&*wormpar_ptr);
        w.setWormPars(this->cmd);
        w.setParsFromGeno(genotype);
     
        //w.setEvolPars(EparsR,evoPars1.evoType);

        //TVector<double> phenotype(1, VectSize);
        //GenPhenMapping(geno, phenotype);
        //setPfaFromPheno(phenotype);
        //setParsFromPheno(phenotype);
        //construct(phenotype);
        //setUpMuscleConn();
        w.InitializeState(rs);
        w.initForSimulation(rs);
        w.setStepSize(StepSize);

        
        shared_ptr<W2DCEparsA> w1 = dynamic_pointer_cast<W2DCEparsA>(w.W2Dbaseparameters1);

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


        }

        //w.setWormPars(&w1);
     
      

        for (double t = 0.0; t <= Transient; t += StepSize){
            w.Step();
        }    

        //cout << "EparsR->dbunit " << EparsR->dbunit << endl;
        //cout << "EparsR->vbunit " << EparsR->vbunit << endl;
       
        //assert(0);
       
        DBp = w.n.NeuronOutput(EparsR->dbunit);
        VBp = w.n.NeuronOutput(EparsR->vbunit);
    
        w.Step(); // determine sign of derivative
    

        dDB = w.n.NeuronOutput(EparsR->dbunit) - DBp;
        dVB = w.n.NeuronOutput(EparsR->vbunit) - VBp;
        signtagDB = (dDB  > 0) ? 1 : -1;
        signtagVB = (dVB  > 0) ? 1 : -1;
        DBp = w.n.NeuronOutput(EparsR->dbunit);
        VBp = w.n.NeuronOutput(EparsR->vbunit);
        
        double xt = w.CoMx(), xtp;
        double yt = w.CoMy(), ytp;
      
        // Time loop
        for (double t = 0.0; t <= Duration; t += StepSize) {
            // Step simulation
            w.Step();
            
            ///// Oscilation
            // check changes in sign of derivative
            dDB = w.n.NeuronOutput(EparsR->dbunit) - DBp;
            dVB = w.n.NeuronOutput(EparsR->vbunit) - VBp;
            signDB = (dDB  > 0) ? 1 : ((dDB  < 0) ? -1 : 0);
            signVB = (dVB  > 0) ? 1 : ((dVB  < 0) ? -1 : 0);
    
            oscDB += abs(DBp - w.n.NeuronOutput(EparsR->dbunit));
            oscVB += abs(VBp - w.n.NeuronOutput(EparsR->vbunit));
    
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
            DBp = w.n.NeuronOutput(EparsR->dbunit);
            VBp = w.n.NeuronOutput(EparsR->vbunit);
            
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
            
            if (EparsR->doAngleDiff)
            anglediff = angle_diff(movementorientation,bodyorientation);
            else
            // Orientation of the movement
            anglediff = movementorientation - bodyorientation;  // Check how orientations align
            if (direction == 1 || direction == 2){
            if (EparsR->fitType == 0)
            temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
            else temp = cos(anglediff);
            }
            else{
            if (EparsR->fitType == 0) 
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
double EvolutionFullW<T>::Evaluation18(TVector<double> &genotype, RandomState &rs)
{
    double fitness;
    //ofstream fitfile;
  
    const double & Duration = evoPars1.Duration;
    //const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;
    const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = evoPars1.Transient;
    //const int & skip_steps = evoPars1.skip_steps;


    shared_ptr<const AgarPars> EparsR = dynamic_pointer_cast<const AgarPars>(this->evopar_ptr);

    if (EparsR == nullptr) assert(0);

    const double AvgSpeed = EparsR->AvgSpeed;
    const double BBCfit = AvgSpeed*Duration;


    

    //const double    AvgSpeed = 0.00022;             // Average speed of the worm in meters per seconds
    //const double    BBCfit = AvgSpeed*Duration;
    // Fitness
    fitness = 0.0;
    double bodyorientation, anglediff;
    double movementorientation, distancetravelled = 0, temp;
    TVector<double> curvature(1, N_curvs);
    TVector<double> antpostcurv(1, 2);
    antpostcurv.FillContents(0.0);

    // Genotype-Phenotype Mapping
    //TVector<double> phenotype(1, VectSize);
    //GenPhenMapping(v, phenotype);

 

    T w;//(genotype, false);
    //w.setWormPars(argc,argv);
    
    w.setWormPars(this->cmd);

    
    w.setParsFromGeno(genotype);
   
        //TVector<double> phenotype(1, VectSize);
        //GenPhenMapping(geno, phenotype);
        //setPfaFromPheno(phenotype);
        //setParsFromPheno(phenotype);
        //construct(phenotype);
        //setUpMuscleConn();
    w.InitializeState(rs);
    w.initForSimulation(rs);
    w.setStepSize(StepSize);

    

    // Transient
    for (double t = 0.0; t <= Transient; t += StepSize)
    {
        w.Step();

    }

    double xt = w.CoMx(), xtp;
    double yt = w.CoMy(), ytp;

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


    return fitness;
}


/* int finish_Bosc(int Generation,double BestPerf,double AvgPerf,double PerfVar){
    if (BestPerf > 0.99) return 1;
    else return 0;
}
 */
/* template<class T>
void EvolutionFullW<T>::configure_p2_Net21()
{
  
    // Stage 1 //
    {typedef int (*callback_t)(int, double, double, double);
        Callback<int(int, double, double, double)>::func 
        = std::bind(&finish_Bosc, this, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        callback_t func = static_cast<callback_t>(Callback<int(int, double, double, double)>::callback); 
        s->SetSearchTerminationFunction(func);}

    //s->SetSearchTerminationFunction(finish_Bosc);

    {typedef double (*callback_t)(TVector<double> &, RandomState &);
        Callback<double(TVector<double> &, RandomState &)>::func = std::bind(&Evolution21::EvaluationFunction1, this, 
                std::placeholders::_1, std::placeholders::_2);
        callback_t func = static_cast<callback_t>(Callback<double(TVector<double> &, RandomState &)>::callback);
    s->SetEvaluationFunction(func);}

    //s->SetEvaluationFunction(EvaluationFunction1);
  

    s->ExecuteSearch();

    // Stage 2 //
    s->SetSearchTerminationFunction(NULL);

    {typedef double (*callback_t)(TVector<double> &, RandomState &);
        Callback<double(TVector<double> &, RandomState &)>::func = std::bind(&Evolution21::EvaluationFunction2, this, 
                std::placeholders::_1, std::placeholders::_2);
        callback_t func = static_cast<callback_t>(Callback<double(TVector<double> &, RandomState &)>::callback);
    s->SetEvaluationFunction(func);}

    //s->SetEvaluationFunction(EvaluationFunction2);
    InitializeBodyConstants();
    s->ExecuteSearch();
}


template<class T>
void EvolutionFullW<T>::configure_p12_RS18()
{
   // configure_p1();
    if (evo_seed)
    {
        ifstream BestIndividualFile;
        TVector<double> bestVector(1, evoPars1.VectSize);
        BestIndividualFile.open(rename_file("best.gen.dat"));
        BestIndividualFile >> bestVector;
        s->InitializeSearch();
        for (int i = 1; i <= s->PopulationSize(); i++){
            for (int j = 1; j <= evoPars1.VectSize; j++)
            {
                s->Individual(i)[j] = bestVector[j];
            }
        }
    }
   // configure_p2();

}
 */

template<class T>
double EvolutionFullW<T>::EvaluationCE(TVector<double> &genotype, RandomState &rs)
{
  
    const EvolparametersCE & Epars1 = dynamic_cast<const EvolparametersCE&>(*this->evopar_ptr);

    //Epars1.show();
    //assert(0);


    const int SR_A = 1;
    const int SR_B = 2;
 
    const int gen_num = s->Generation();
    //evoPars1.MaxGenerations;
    //Epars1.doAlternateEvo;

    const bool doalt1 = Epars1.doReverse==3 && (gen_num < evoPars1.MaxGenerations/2);
    const bool doalt2 = Epars1.doReverse==3 && (gen_num >= evoPars1.MaxGenerations/2);
    const bool doalt1f = Epars1.doReverse==2 || (doalt1);
    const bool doalt2f = Epars1.doReverse==2 || (doalt2);

    //cout << "reverse is " << Epars1.doReverse << endl;

    double sra = genotype(SR_A);
    double srb = genotype(SR_B);

   // assert(0);

    //double fitnessForward, fitnessBackward;

    //genotype(SR_A)= -1.0;
    //genotype(SR_B)= srb;
    //return EvaluationCEp1(genotype, rs, 1); 

    double fitness = 0;
    int count = 0;
    if (Epars1.doReverse==0 || doalt1f){
        //  assert(0 && "dorev0");
    if (Epars1.zeroGainsType == 1) genotype(SR_A)= -1.0;
    genotype(SR_B)= srb;
    fitness += EvaluationCEp1(genotype, rs, 1);
    count++;
    }
    if (Epars1.doReverse==1 || doalt2f){
       // assert(0 && "dorev1");
    genotype(SR_A)= sra;
    if (Epars1.zeroGainsType == 1) genotype(SR_B)= -1.0;
    fitness += EvaluationCEp1(genotype, rs, -1);
    count++;
    }

    genotype(SR_A) = sra;
    genotype(SR_B) = srb;


    return fitness/count;



    //if (Epars1.doReverse==0) return fitnessForward;
    //if (Epars1.doReverse==1) return fitnessBackward;
    //if (Epars1.doReverse==2) return (fitnessForward + fitnessBackward)/2;

    //assert(0 && "doReverse not set properly");
    // return fitnessBackward;
}

template<class T>
double EvolutionFullW<T>::EvaluationCENZ(TVector<double> &genotype, RandomState &rs)
{

    const EvolparametersCE & Epars1 = dynamic_cast<const EvolparametersCE&>(*this->evopar_ptr);

    //Epars1.show();
    //assert(0);


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
    fitness += EvaluationCEp1(genotype, rs, 1);
    count++;
    }
    if (Epars1.doReverse==1 || doalt2f){
       // assert(0 && "dorev1");
    //genotype(SR_A)= sra;
    //genotype(SR_B)= -1.0;
    fitness += EvaluationCEp1(genotype, rs, -1);
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
double EvolutionFullW<T>::EvaluationCEp1(TVector<double> &genotype, RandomState &rs, int direction){

  const double & Duration = evoPars1.Duration;
  //const int & VectSize = evoPars1.VectSize;
  const double & StepSize = evoPars1.StepSize;
  const double & Transient = evoPars1.Transient;

  //const AgarPars & EparsR = dynamic_cast<const AgarPars&>(*evopar_ptr);
    //const EvolparametersCE & EparsR = dynamic_cast<const EvolparametersCE&>(*evopar_ptr);
    shared_ptr<const EvolparametersCE> EparsR = 
    dynamic_pointer_cast<const EvolparametersCE>(this->evopar_ptr);
  
    

    const double AvgSpeed = EparsR->AvgSpeed;
    
    //const double    AvgSpeed = 0.0001; //0.00022;              // Average speed of the worm in meters per seconds
    

    //cout << "AS " << AvgSpeed << endl;
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

    T w;

   

    //T w(genotype, false);
    //w.setWormPars(&*wormpar_ptr);
    //w.setWormPars(argc,argv);
  
    w.setWormPars(this->cmd);
    w.setParsFromGeno(genotype);


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

    //EvolparametersCE & Epars1 = dynamic_cast<EvolparametersCE&>(*evopar_ptr);
    //WormCE & w2 = dynamic_cast<WormCE&>(w);

    //W2DCEparsA w1(dynamic_cast<const W2DCEparsA&>(*wormpar_ptr));

    shared_ptr<W2DCEparsA> w1 = dynamic_pointer_cast<W2DCEparsA>(w.W2Dbaseparameters1);

    //w1.show();
    //assert(0);

    if (direction == 1){
    w1->AVA_output =  0.0;
    w1->AVB_output =  1.0;
    }
    else if  (direction == -1) {
        w1->AVA_output =  1.0;
        w1->AVB_output =  0.0; // Command Interneuron Activation Backward
    }
    else if  (direction == 2)
    {
    w1->AVA_output =  0.0;
    w1->AVB_output =  0.0; 
    }
    else assert(0 && "direction not set properly");

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
            if (EparsR->fitType == 0)
            temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
            else temp = cos(anglediff);
            }
            else{
            if (EparsR->fitType == 0) 
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
    return fitB;


}

template<class T>
double EvolutionFullWC<T>::EvaluationCO(TVector<double> &genotype, RandomState &rs)
{

//	cout << "EvolutionCO::EvaluationFunction(TVector<double> &v, RandomState &rs, WormAgent * Worm)" << endl;

	//TVector<double> phenotype;
	//phenotype.SetBounds(1, evoPars1.VectSize);
	//GenPhenMapping(v, phenotype);
	
	//WormAgent Worm(CircuitSize);

    const double & Duration = evoPars1.Duration;
    const double & StepSize = evoPars1.StepSize;
    const double & Transient = evoPars1.Transient;

    T w(this->cmd);
    //w.setStepSize(StepSize);

    //w.setWormPars(cmd);
    w.setParsFromGeno(genotype);


    w.InitializeState(rs);
    //w.initForSimulation(rs);
    
    
    shared_ptr<gradParameters> w1 = dynamic_pointer_cast<gradParameters>(w.W2Dbaseparameters1b);

	//RandomState rs2 = rs;
	//Worm->InitializeState(rs2);
	//Worm->SetParameters(phenotype);
	//Worm->setStepSize(evoPars1.StepSize);

    WormGrad & wg = dynamic_cast<WormGrad&>(w);
    
    const double Pi	=	3.1415926;

	double f, accdist, totaldist;
	int k = 0;
	double fitness = 0.0;
	int taxis,kinesis;
	for (int mode = 1; mode <= 1; mode++)
	{
		if (mode==0){taxis = 0;kinesis = 1;}
		else {taxis = 1;kinesis = 0;}

		for (double gradSteep = 0.5; gradSteep <= 0.5; gradSteep += 0.2)
		{
			for (double orient = 0.0; orient < 2*Pi; orient += Pi/2)
			{

                //w1->worm_rotation = orient;
                w1->orient_orig = orient;
                w1->gradSteep = gradSteep;
                w1->RunDuration = Transient + Duration;
                w1->HSStepSize = StepSize;
                w1->taxis = taxis;
                w1->kinesis = kinesis;

				//Worm->setSimPars(orient,
				//	gradSteep,
				//	evoPars1.Transient + evoPars1.Duration,
				//	evoPars1.StepSize, taxis, kinesis);

				//Worm->InitializeSimulation(rs);
				//Worm->initForSimulation(rs);

                w.initForSimulation(rs);

				/* Worm->InitialiseAgent(2*RunDuration, evoPars1.StepSize);
				Worm->ResetAgentsBody(orient, rs);
				Worm->ResetChemCon(gradSteep);
				Worm->ResetAgentIntState(rs);
				Worm->UpdateChemCon(gradSteep); */
 
				for (int repeats = 1; repeats <= 2; repeats++)
				{
					wg.ResetAgentsBody();
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

						//accdist += Worm->DistanceToCentre();
						//cout << "D " << Worm->DistanceToCentre() << endl;
					}
					totaldist = (accdist/(Duration/StepSize));
					f = (w1->MaxDist - totaldist)/w1->MaxDist;
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
double EvolutionFullWC<T>::EvaluationCO2(TVector<double> &genotype, RandomState &rs)
{

//	cout << "EvolutionCO::EvaluationFunction(TVector<double> &v, RandomState &rs, WormAgent * Worm)" << endl;

	//TVector<double> phenotype;
	//phenotype.SetBounds(1, evoPars1.VectSize);
	//GenPhenMapping(v, phenotype);
	
	//WormAgent Worm(CircuitSize);

    const double & Duration = evoPars1.Duration;
    const double & StepSize = evoPars1.StepSize;
    const double & Transient = evoPars1.Transient;

    T w(this->cmd);
    //w.setStepSize(StepSize);

    //w.setWormPars(cmd);
    w.setParsFromGeno(genotype);


    w.InitializeState(rs);
    //w.initForSimulation(rs);
    
    
    shared_ptr<gradParameters> w1 = dynamic_pointer_cast<gradParameters>(w.W2Dbaseparameters1b);

	//RandomState rs2 = rs;
	//Worm->InitializeState(rs2);
	//Worm->SetParameters(phenotype);
	//Worm->setStepSize(evoPars1.StepSize);

    WormGrad & wg = dynamic_cast<WormGrad&>(w);
    
    const double Pi	=	3.1415926;

	double f, accdist, totaldist;
	int k = 0;
	double fitness = 0.0;
	int taxis,kinesis;
	for (int mode = 1; mode <= 1; mode++)
	{
		if (mode==0){taxis = 0;kinesis = 1;}
		else {taxis = 1;kinesis = 0;}

		for (double gradSteep = 0.5; gradSteep <= 0.5; gradSteep += 0.2)
		{
			for (double orient = 0.0; orient < 2*Pi; orient += Pi/2)
			{

                w1->worm_rotation = orient;
                w1->orient_orig = Pi;
                w1->gradSteep = gradSteep;
                w1->RunDuration = Transient + Duration;
                w1->HSStepSize = StepSize;
                w1->taxis = taxis;
                w1->kinesis = kinesis;

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
                    w.initForSimulation(rs);
					//wg.ResetAgentsBody();
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

						//accdist += Worm->DistanceToCentre();
						//cout << "D " << Worm->DistanceToCentre() << endl;
					}
					totaldist = (accdist/(Duration/StepSize));
					f = (w1->MaxDist - totaldist)/w1->MaxDist;
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
