//
//  Worm.hpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

//#include "VectorMatrix.h"
//#include "random.h"
//#include "WormBody21.h"
//#include "NervousSystem.h"
//#include "Muscles.h"
#include "Worm2D21.h"

#include <cmath>

#define PI 3.14159265

using namespace std;

// Parameters
//const int N_muscles = 24;           // Number of muscles alongside the body
//const int N_units = 7;              // Number of neural units
//const int N_neuronsperunit = 7;     // Number of neurons in a neural unit

//const double T_muscle = 0.1;        // Muscle time constant


// Body segment name conventions
//const int Head = 1;
//const int Tail = N_segments;

class Worm21 : public Worm2D21, public EvolvableS
{
public:
    
    Worm21(TVector<double> &pheno, shared_ptr<const CmdArgs> cmd_ = nullptr);
    Worm21(TVector<double> &pheno, bool isPheno, shared_ptr<const CmdArgs> cmd_);
    Worm21(shared_ptr<const CmdArgs> cmd_);
    Worm21(const string & filename_, shared_ptr<const CmdArgs> cmd_);

    void InitializeState(RandomState &rs);
    void DumpParams(ofstream &ofs);
    NervousSystem & n;

    template<class T> friend class EvolutionFullW;
    template<class T> friend class Evolvable_ptr;
    template<class T> friend class Evolvable_ptrB;

    //virtual shared_ptr<const W2Dparameters> setWormPars(int argc, const char* argv[]) 
    //{return Worm2Dbase::setWormPars(argc,argv);}
    //virtual void setWormPars(const W2Dparameters & w2par_)
    //{return Worm2Dbase::setWormPars(w2par_);}

    //void setWormPars(shared_ptr<const CmdArgs> cmd);
    //shared_ptr<const W2Dparameters> setWormPars(int argc, const char* argv[]);
    //void setWormPars(const W2Dparameters * w2par_);

    

    protected:
    void addEvolvableToJson(json & j);
    void addParsToJson(json & j);
    void setPhenoNames();
    void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
    int getVectSize(){return 44;}
    void setParsFromPheno(const TVector<double> &pheno);
    //void setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_);
    void setEvolPars(W2Dparameters & w2par_, string evotype_);
};

class Worm21R : public Worm21
{
public:
    
    Worm21R(TVector<double> &pheno, shared_ptr<const CmdArgs> cmd_ = nullptr);
    Worm21R(TVector<double> &pheno, bool isPheno, shared_ptr<const CmdArgs> cmd_);
    Worm21R(shared_ptr<const CmdArgs> cmd_);
    Worm21R(const string & filename_, shared_ptr<const CmdArgs> cmd_);

    template<class T> friend class EvolutionFullW;
    template<class T> friend class EvolutionFullWC;
    template<class T> friend class Evolvable_ptrB;

    template<class T> friend class Evolvable_ptr;
    
    protected:
    void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
    int getVectSize(){return 48;}
    void setParsFromPheno(TVector<double> &pheno);
    void setPhenoNames();

};