#include "Evolvable.h"

EvolvableS::EvolvableS(shared_ptr<W2Dparameters> w2par_ptr_):w2par_ptr(w2par_ptr_)
//, Epars1(dynamic_cast<Evolparameters&>(*w2par_ptr))
{}

EvolvableS::EvolvableS(){}
 

void EvolvableS::setParsFromFile(const string & filename_)
{
    ifstream ifs;
    ifs.open(filename_);
    TVector<double> bestVector(1, getVectSize());
    //assert(0);
    ifs >> bestVector;
    ifs.close();
    setParsFromGeno(bestVector);
}


void EvolvableS::setParsFromGeno(TVector<double> &geno)
{
    
    //cout << v << endl;
    TVector<double> phenotype(1, geno.Size());
    //cout << phenotype.Size() << endl;
    GenPhenMapping(geno, phenotype);
 
    setParsFromPheno(phenotype);
 
}