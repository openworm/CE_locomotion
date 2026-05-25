//#include "VectorMatrix.h"
#include "WormRS18.h"
#include "Worm2DCE.h"
#include "Worm21.h"
#include "WormAgent.h"
//#include "Worm2DCE.h"
//#include "../argUtils.h"
#include <iomanip>  // cout precision
#include "EvolutionRS18.h"
#include "EvolutionCE.h"
#include "Evolution21.h"
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

    shared_ptr<const CmdArgs> cmd = make_shared<const CmdArgs>(argc, argv);

    std::cout << std::setprecision(10);
    string model_name =  getParameterString(argc,argv,"--modelname","");
    if (model_name == "")
    {
    cout << "Model name required for Worm2D. Exiting." << endl;
    return 0;
    }

  
    Evolution* er = 0;
    if (model_name == "CE") er = new EvolutionCE(cmd);
    if (model_name == "RS18") er = new EvolutionRS18(cmd);
    if (model_name == "Net21") er = new Evolution21(cmd);
    if (model_name == "CO") {
     double stepsize = 0.01;
     int circuitsize = 10;   
        er = new EvolutionCO(cmd,stepsize,circuitsize);
    }
    const evoPars & ep1 = er->itsEvoPars();

    
    InitializeBodyConstants();
   
    
    bool do_evol = getParameterInt(argc,argv,"--doevol","0");
    if (do_evol) 
    {
        er->configure();
        ofstream seedfile;
        seedfile.open(er->rename_file("seed.dat"));
        seedfile << ep1.randomseed << endl;
        seedfile.close();
    }
   
    
    //get vector of best individual
   
    TVector<double> bestVector(1, er->itsVectSize());

    {ifstream BestIndividualFile;
    BestIndividualFile.open(er->rename_file("best.gen.dat"));
    BestIndividualFile >> bestVector;
    BestIndividualFile.close();}
    
    TVector<double> phenotype(1, er->itsVectSize());
    er->GenPhenMapping(bestVector, phenotype);

    bool do_json = 1;

    // write worm_data.json 
    if (do_json) {

        Worm2Dbase* w = 0;
        

        if (model_name == "CE") w = new WormCE(cmd,phenotype);
        if (model_name == "RS18") w = new Worm18(phenotype,0);
        if (model_name == "Net21") w = new Worm21(phenotype,cmd);
        if (model_name == "CO") {

        w = new WormAgent(phenotype, 10);
        double orient = 0;
        double gradSteep = 0.5;
        int taxis = 1;
        int kinesis = 0;
        dynamic_cast<WormAgent&>(*w).setSimPars(orient,gradSteep,
            ep1.Transient + ep1.Duration,
            ep1.StepSize, taxis, kinesis);

        }
        cout << "making json from main" << endl;
        //write_json(er,w, "worm_data_2.json");

        RandomState rs;
        rs.SetRandomSeed(er->itsEvoPars().randomseed);
        w->InitializeState(rs); 

        cout << "making json from main 2" << endl;
        ofstream json_out(er->rename_file("worm_data.json"));
        //json_out << setprecision(32);
        json j;
        w->addParsToJson(j);
        er->addParsToJson(j);
        json_out << std::setw(4) << j << std::endl;
        json_out.close();

        ofstream nsdump(er->rename_file("NSdump.dat"));
        //if (model_name == "CO") nsdump << dynamic_cast<CTRNN&>(w->itsNS());
        //else 
        nsdump << dynamic_cast<NervousSystem&>(w->itsNS());
        nsdump.close();

        delete w;
        
    }

    bool do_nml =  getParameterInt(argc,argv,"--donml","0");
    bool do_musclesim = getParameterInt(argc,argv,"--domusc","0");

    //run simulation with possibly different seed
    
    const long simrandseed =  getParameterLong(argc,argv,"-R","-1");
    if (simrandseed == -1) {cout << "Seed not set properly. Exiting." << endl; return 0;}
    
    if (!do_nml){
    
    //er->RunSimulation(bestVector, rs);

    {
        Worm2Dbase* w = 0;

    cout << "making worm" << endl;

    if (model_name == "CE") w = new WormCE(cmd,phenotype);
    if (model_name == "RS18") w = new Worm18(phenotype,0);
    if (model_name == "Net21") w = new Worm21(phenotype);
    if (model_name == "CO") w = new WormAgent(phenotype,10);
   

    //write_json(er,w, "worm_data_3.json");
    //w->setBasename(er->itsEvoPars().directoryName);
    w->setDataskips(er->itsEvoPars().skip_steps);
    w->InitializeData(er->itsEvoPars().directoryName);
    //w->dataReset();

    
    w->setStepSize(er->itsEvoPars().StepSize);
    
    //w->setDataskips(er->itsEvoPars().skip_steps);
    //w->setPrefix("sim");
    //w->InitializeData(er->itsEvoPars().directoryName);


    cout << "making simulation simrandseed " << simrandseed << endl;
    {RandomState rs;
    rs.SetRandomSeed(simrandseed);
    
    w->initForSimulation(rs);

    er->RunSimulation(*w, rs);

    {ofstream phenfile(er->rename_file("phenotype.dat"));
    w->DumpParams(phenfile);
    phenfile.close();}

    delete w;
    }

    }

    //double simduration = atof(getParameter(argc,argv,"-sd","60"));
    //double simtransient = atof(getParameter(argc,argv,"-st","50"));


    Worm2Dbase* w = 0;

    cout << "making worm 2" << endl;

    if (model_name == "CE") w = new WormCE(cmd,phenotype);
    if (model_name == "RS18") w = new Worm18(phenotype,0);
    if (model_name == "Net21") w = new Worm21(phenotype);
    if (model_name == "CO") w = new WormAgent(phenotype,10);

   
    RandomState rs;
    rs.SetRandomSeed(simrandseed);
    w->InitializeState(rs);

    double simduration = 60;
    double simtransient = 50;

    if (model_name == "CO") {

        //simduration = 10;
        double orient = 0;
        double gradSteep = 0.5;
        int taxis = 1;
        int kinesis = 0;
        dynamic_cast<WormAgent&>(*w).setSimPars(orient,gradSteep,
            simtransient + simduration,
            ep1.StepSize, taxis, kinesis);
       // dynamic_cast<WormAgent&>(*w).InitializeSimulation(rs);

    }
    

    


    simPars sp1 = {er->itsEvoPars().directoryName,
        //er->itsEvoPars().skip_steps, 
        simduration, simtransient, er->itsEvoPars().StepSize};
    Simulation s1(sp1);
    
    
    w->initForSimulation(rs);
    w->setStepSize(er->itsEvoPars().StepSize);
    w->setDataskips(er->itsEvoPars().skip_steps);
    w->setPrefix("sim");
    w->InitializeData(er->itsEvoPars().directoryName);

    
    s1.runSimulation(*w);

    delete w;

    }
    else{
       
    Worm2Dm* w = 0; 

    const bool makeWormFromJson = true;
    if (makeWormFromJson){
    ifstream json_in(er->rename_file("worm_data.json"));
    json j;
    json_in >> j;
    json_in.close();


    if (model_name == "CE") w = new Worm2DCE(j);
    //if (model_name == "Net21") w = new Worm2D21(j);
    
    if (model_name == "Net21") {
    if (do_musclesim)
    w = new Worm2D21m(cmd);
    else
    w = new Worm2D21(j,cmd);
    }
    }
    else{
    
    if (model_name == "Net21") w = new Worm2D21(phenotype,cmd);

    }

   
    w->setDataskips(er->itsEvoPars().skip_steps);
    w->InitializeData(er->itsEvoPars().directoryName);
    w->setStepSize(er->itsEvoPars().StepSize);
    
    write_json(er,w, "worm_data_nml.json");

    {RandomState rs;
    rs.SetRandomSeed(simrandseed);
    er->RunSimulation(*w, rs);}

    {RandomState rs;
    rs.SetRandomSeed(simrandseed);

    w->InitializeState(rs);
    w->initForSimulation(rs);

    
    //w->setDataskips(er->itsEvoPars().skip_steps);
    w->setPrefix("sim");
    //w->InitializeData(er->itsEvoPars().directoryName);

    simPars sp1 = {er->itsEvoPars().directoryName,
        //er->itsEvoPars().skip_steps, 
        60, 50, er->itsEvoPars().StepSize};
    Simulation s1(sp1);


    s1.runSimulation(*w);}

    delete w;
   
    }

    delete er;
    

    return 0;
}