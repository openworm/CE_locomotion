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
    
    Worm21(TVector<double> &pheno);
    Worm21(TVector<double> &pheno, bool isPheno);
    Worm21();
    Worm21(const string & filename_);

    void InitializeState(RandomState &rs);
    void DumpParams(ofstream &ofs);
    NervousSystem & n;

    template<class T> friend class EvolutionFullW;

    protected:
    void addParsToJson(json & j);
    void setPhenoNames();
    void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
    int getVectSize(){return 44;}
    void setParsFromPheno(TVector<double> &pheno);
    //void setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_);
    void setEvolPars(W2Dparameters & w2par_, string evotype_);
};
