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
    Evolution * evo = 0;
    
    if (model_name == "W2Dosc") evo = new EvolutionFullW2<Worm2Dosc>(argc,argv);
    if (model_name == "W2DoscH") evo = new EvolutionFullW2<Worm2DoscHalf>(argc,argv);
    if (model_name == "W2Dosc21") evo = new EvolutionFullW2<Worm2Dosc21>(argc,argv);

    //assert(0);

    //EvolutionFullW<Worm2Dosc> evo(argc,argv);

    /* Evolvable * w1;
    if (model_name == "W2Dosc") w1 = new Worm2Dosc();
    if (model_name == "W2DoscH") w1 = new Worm2DoscHalf();

    EvolutionFull evo(argc,argv,w1); */
    bool do_evol = atoi(getParameter(argc,argv,"--doevol","0"));
    if (do_evol) 
    {
        evo->configure();
    }
   //delete w1;
    
    
    cout << evo->rename_file("best.gen.dat") << " " << model_name << endl;

    bool do_nml =  atoi(getParameter(argc,argv,"--donml","0"));

    Worm2Dbase * w2;
    if (!do_nml){
    if (model_name == "W2Dosc") w2 = new Worm2Dosc(evo->rename_file("best.gen.dat"));
    if (model_name == "W2DoscH") w2 = new Worm2DoscHalf(evo->rename_file("best.gen.dat"));
    if (model_name == "W2Dosc21") w2 = new Worm2Dosc21(evo->rename_file("best.gen.dat"));
    }else{

    if (model_name == "W2Dosc") w2 = new Worm2DoscNML(evo->rename_file("worm_data_evo.json"));

    }
    ofstream json_out(evo->rename_file("worm_data_worm.json"));
    //json_out << setprecision(32);
    json j;
    w2->addParsToJson(j);
    if (do_evol) evo->addParsToJson(j);
    json_out << std::setw(4) << j << std::endl;
    json_out.close();

    
    cout << "const 1" << endl;
    //assert(0);

    const int simrandseed =  atoi(getParameter(argc,argv,"-R","-1"));
    if (simrandseed == -1) {cout << "Seed not set properly. Exiting." << endl; return 0;}
    RandomState rs;
    rs.SetRandomSeed(simrandseed);
    w2->InitializeState(rs);
    cout << "const 1" << endl;

    double simduration = atof(getParameter(argc,argv,"-sd","60"));
    double simtransient = atof(getParameter(argc,argv,"-st","50"));

    
    simPars sp1 = {evo->itsEvoPars().directoryName,
        //er->itsEvoPars().skip_steps, 
        simduration, simtransient, evo->itsEvoPars().StepSize};
    Simulation s1(sp1);
    
    cout << "const 1" << endl;

    w2->initForSimulation(rs);
    w2->setStepSize(evo->itsEvoPars().StepSize);
    w2->setDataskips(evo->itsEvoPars().skip_steps);
    //w->setPrefix("sim");
    w2->InitializeData(evo->itsEvoPars().directoryName);

    cout << "const 1" << endl;

    s1.runSimulation(*w2);
    cout << "const 1" << endl;

    delete evo;
    delete w2;
    return 0;
}