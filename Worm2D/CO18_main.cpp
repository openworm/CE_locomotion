//#include "VectorMatrix.h"
//#include "WormRS18.h"
//#include "WormCE.h"
//#include "Worm21.h"
//#include "WormAgent.h"
#include "WormCO18.h"
//#include "Worm2DCE.h"
//#include "../argUtils.h"
#include <iomanip>  // cout precision
#include "EvolutionRS18.h"
//#include "EvolutionCE.h"
//#include "Evolution21.h"
#include "EvolutionCO.h"
#include "jsonUtils.h"
//#include "Simulation.h"

using json = nlohmann::json;

void write_json(Evolution* er,  Worm2Dm* w, string filename)
{
    RandomState rs;
    rs.SetRandomSeed(er->itsEvoPars().randomseed);
    w->InitializeState(rs); 

    ofstream json_out(er->rename_file(filename));
    json j;
    w->addParsToJson(j);
    er->addParsToJson(j);
    json_out << std::setw(4) << j << std::endl;
    json_out.close();

}


int main (int argc, const char* argv[])
{

    EvolutionCO erco(argc,argv);
    EvolutionRS18 er18(argc,argv);

    TVector<double> phenotypeCO(1, erco.itsEvoPars().VectSize);
    {
    TVector<double> bestVector(1, erco.itsEvoPars().VectSize);
    ifstream BestIndividualFile;
    BestIndividualFile.open(erco.rename_file("CO.best.gen.dat"));
    BestIndividualFile >> bestVector;
    BestIndividualFile.close();
    erco.GenPhenMapping(bestVector, phenotypeCO);}


    TVector<double> phenotype18(1, er18.itsEvoPars().VectSize);
    {
    TVector<double> bestVector(1, er18.itsEvoPars().VectSize);
    ifstream BestIndividualFile;
    BestIndividualFile.open(er18.rename_file("RS18.best.gen.dat"));
    BestIndividualFile >> bestVector;
    BestIndividualFile.close();
    er18.GenPhenMapping(bestVector, phenotype18);}

    InitializeBodyConstants();

    WormCO18 wco18(phenotype18, phenotypeCO, 10);
    
    const int simrandseed =  atoi(getParameter(argc,argv,"-R","-1"));
    if (simrandseed == -1) {cout << "Seed not set properly. Exiting." << endl; return 0;}
    double simduration = atof(getParameter(argc,argv,"-sd","60"));
    double simtransient = atof(getParameter(argc,argv,"-st","50"));


    RandomState rs;
    rs.SetRandomSeed(simrandseed);
    wco18.InitializeState(rs);
    //wco18.initForSimulation();
    double orient = 0;
    double gradSteep = 0.5;
    wco18.setSimPars(orient,gradSteep,simduration + simtransient,er18.itsEvoPars().StepSize);

    simPars sp1 = {er18.itsEvoPars().directoryName,
    er18.itsEvoPars().skip_steps, simduration, simtransient, er18.itsEvoPars().StepSize};
    Simulation s1(sp1);
    s1.runSimulation(wco18);

    
    return 0;
}