#include "Evolvable.h"

const char* getParameter(int argc, const char* argv[], string parName, const char* defaultval)
{    
   const char* retval = defaultval;
   if (((argc-1) % 2) != 0)
   {cout << "The arguments are not configured correctly." << endl;exit(1);}
   for (int arg = 1; arg<argc; arg+=2) 
   if (strcmp(argv[arg],parName.c_str())==0) {retval = argv[arg+1];break;}
   return retval;
}

W2DCEpars::W2DCEpars(int argc, const char* argv[])
{

  AB_output_level = atoi(getParameter(argc,argv,"--ABLevel","1"));
  sr_type = getParameter(argc,argv,"--SRType","None");
}


EvolparametersCE::EvolparametersCE(int argc, const char* argv[]):W2DCEpars(argc,argv)
{
    //doAlternateEvo = atoi(getParameter(argc,argv,"--doAlternateEvo","0"));
    doReverse = atoi(getParameter(argc,argv,"--doReverse","0"));
    //sr_type = getParameter(argc,argv,"--SRType","None");
}

//EvolvableS::EvolvableS(shared_ptr<W2Dparameters> w2par_ptr_):evolvable_w2par_ptr(w2par_ptr_)
//, Epars1(dynamic_cast<Evolparameters&>(*w2par_ptr))
//{}

//EvolvableS::EvolvableS(){}


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