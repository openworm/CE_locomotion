#include "Worm2Dmods.h"
#include "Evolution.h"


int main (int argc, const char* argv[])
{

    std::cout << std::setprecision(10);
    string model_name =  getParameter(argc,argv,"--modelname","");
    if (model_name == "")
    {
    cout << "Model name is required." << endl;
    return 0;
    }

    Worm2Dosc * w1;
    if (model_name == "W2Dosc") w1 = new Worm2Dosc();
    EvolutionFull evo(argc,argv,w1);
    bool do_evol = atoi(getParameter(argc,argv,"--doevol","0"));
    if (do_evol) 
    {
        evo.configure();
    }
    delete w1;

    Worm2Dbase * w2;
    if (model_name == "W2Dosc") w2 = new Worm2Dosc(evo.rename_file("best.gen.dat"));

    
    const int simrandseed =  atoi(getParameter(argc,argv,"-R","-1"));
    if (simrandseed == -1) {cout << "Seed not set properly. Exiting." << endl; return 0;}
    RandomState rs;
    rs.SetRandomSeed(simrandseed);
    w2->InitializeState(rs);
    
    double simduration = atof(getParameter(argc,argv,"-sd","60"));
    double simtransient = atof(getParameter(argc,argv,"-st","50"));

    
    simPars sp1 = {evo.itsEvoPars().directoryName,
        //er->itsEvoPars().skip_steps, 
        simduration, simtransient, evo.itsEvoPars().StepSize};
    Simulation s1(sp1);
    
    
    w2->initForSimulation(rs);
    w2->setStepSize(evo.itsEvoPars().StepSize);
    w2->setDataskips(evo.itsEvoPars().skip_steps);
    //w->setPrefix("sim");
    w2->InitializeData(evo.itsEvoPars().directoryName);

    
    s1.runSimulation(*w2);

    delete w2;
    return 0;
}