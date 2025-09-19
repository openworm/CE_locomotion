#include "Evolvable.h"

double getParameterDouble(int argc, const char* argv[], string parName, const string defaultval)
{    

   if (((argc-1) % 2) != 0)
   {cout << "The arguments are not configured correctly." << endl;exit(1);}

   for (int arg = 1; arg<argc; arg+=2) 
   if (strcmp(argv[arg], parName.c_str())==0) return stod(argv[arg+1]);
   return stod(defaultval.c_str());
}

int getParameterInt(int argc, const char* argv[], string parName, const string defaultval)
{    

   if (((argc-1) % 2) != 0)
   {cout << "The arguments are not configured correctly." << endl;exit(1);}

   for (int arg = 1; arg<argc; arg+=2) 
   if (strcmp(argv[arg], parName.c_str())==0) return stoi(argv[arg+1]);
   return stoi(defaultval.c_str());
}

long getParameterLong(int argc, const char* argv[], string parName, const string defaultval)
{    

   if (((argc-1) % 2) != 0)
   {cout << "The arguments are not configured correctly." << endl;exit(1);}

   for (int arg = 1; arg<argc; arg+=2) 
   if (strcmp(argv[arg], parName.c_str())==0) return stol(argv[arg+1]);
   return stol(defaultval.c_str());
}

string getParameterString(int argc, const char* argv[], string parName, const string defaultval)
{    

   if (((argc-1) % 2) != 0)
   {cout << "The arguments are not configured correctly." << endl;exit(1);}

   for (int arg = 1; arg<argc; arg+=2) 
   if (strcmp(argv[arg], parName.c_str())==0) return (string) argv[arg+1];
   return defaultval;
}



W2Dbaseparameters::W2Dbaseparameters(int argc, const char* argv[])
{
    randomInitialState = getParameterInt(argc,argv,"--randInitState","0");;
    //cout << "ran " << randomInitialState << endl;
    //assert(0);
}


W2DCEparsA::W2DCEparsA(int argc, const char* argv[]):W2Dbaseparameters(argc,argv)
{
  AB_output_level = getParameterDouble(argc,argv,"--ABLevel","1");
}

W2DCEpars::W2DCEpars(int argc, const char* argv[]):W2DCEparsA(argc,argv)
{
  sr_type = getParameterString(argc,argv,"--SRType","None");
}

AgarPars::AgarPars(int argc, const char* argv[])
{

    OSCTbase = getParameterDouble(argc,argv,"--OSCTbase","0.25");
    agarfreq = getParameterDouble(argc,argv,"--agarfreq","0.44");
    AvgSpeed = getParameterDouble(argc,argv,"--AvgSpeed","0.00022");

}

EvolparametersCE::EvolparametersCE(int argc, const char* argv[]):AgarPars(argc,argv)
{
    //doAlternateEvo = atoi(getParameter(argc,argv,"--doAlternateEvo","0"));
    doReverse = getParameterInt(argc,argv,"--doReverse","0");
    //sr_type = getParameter(argc,argv,"--SRType","None");
}

//EvolvableS::EvolvableS(shared_ptr<W2Dparameters> w2par_ptr_):evolvable_w2par_ptr(w2par_ptr_)
//, Epars1(dynamic_cast<Evolparameters&>(*w2par_ptr))
//{}

//EvolvableS::EvolvableS(){}

Evolparameters::Evolparameters(int argc, const char* argv[], 
    shared_ptr<EvolvableS> & evol1_, string evotype_):AgarPars(argc,argv)
{evol1_->setEvolPars(*this, evotype_);}


void EvolvableS::setParsFromFile(const string & genofilename_)
{
    ifstream ifs;
    ifs.open(genofilename_);
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