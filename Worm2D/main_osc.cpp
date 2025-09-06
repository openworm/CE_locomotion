#include "Worm2Dmods.h"
#include "Worm21.h"
#include "WormCE.h"
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

    evoPars ep1;
    ep1.setFromArgs(argc,argv);
    ep1.StepSize = 0.005;
    ep1.skip_steps = 10;

   
    //assert(0);

    //EvolutionFullW<Worm2Dosc> evo(argc,argv);

    /* Evolvable * w1;
    if (model_name == "W2Dosc") w1 = new Worm2Dosc();
    if (model_name == "W2DoscH") w1 = new Worm2DoscHalf();

    EvolutionFull evo(argc,argv,w1); */
    
    //json_out << setprecision(32);
    json j;

    string evoType = getParameter(argc,argv,"--evoType","0");
    bool do_evol = atoi(getParameter(argc,argv,"--doevol","0"));
    if (do_evol) 
    {
        Evolution * evo = 0;
    
        if (model_name == "W2Dosc") evo = new EvolutionFullW<Worm2Dosc>(argc,argv,evoType);
         if (model_name == "W2DoscH") evo = new EvolutionFullW<Worm2DoscHalf>(argc,argv,evoType);
        if (model_name == "W2Dosc21") evo = new EvolutionFullW<Worm2Dosc21>(argc,argv,evoType);
        if (model_name == "W2Dosc21S") evo = new EvolutionFullW<Worm2Dosc21S>(argc,argv,evoType);
        if (model_name == "W2Dosc21all") evo = new EvolutionFullW<Worm2Dosc21all>(argc,argv,evoType);
        if (model_name == "W2Dosc21Coup") evo = new EvolutionFullW<Worm2Dosc21Coup>(argc,argv,evoType);
        if (model_name == "W2Dosc21CF") evo = new EvolutionFullW<Worm2Dosc21CF>(argc,argv,evoType);
        if (model_name == "W2D21") evo = new EvolutionFullW<Worm21>(argc,argv,evoType); 
        if (model_name == "W2DCE") evo = new EvolutionFullW<WormCE>(argc,argv,evoType); 

        //assert(0);
        ep1.StepSize = evo->itsEvoPars().StepSize;
        ep1.skip_steps = evo->itsEvoPars().skip_steps;
        evo->configure();
        evo->addParsToJson(j);
        delete evo;

    }

    

   //delete w1;
    
    
    cout << ep1.rename_file("best.gen.dat") << " " << model_name << endl;

    bool do_nml =  atoi(getParameter(argc,argv,"--donml","0"));

    Worm2Dbase * w2;
    if (!do_nml){

    if (model_name == "W2Dosc") w2 = new Worm2Dosc(ep1.rename_file("best.gen.dat"));
    if (model_name == "W2DoscH") w2 = new Worm2DoscHalf(ep1.rename_file("best.gen.dat"));
    if (model_name == "W2Dosc21") w2 = new Worm2Dosc21(ep1.rename_file("best.gen.dat"));
    if (model_name == "W2Dosc21S") w2 = new Worm2Dosc21S(ep1.rename_file("best.gen.dat"));
    if (model_name == "W2Dosc21all") w2 = new Worm2Dosc21all(ep1.rename_file("best.gen.dat"));
    if (model_name == "W2Dosc21Coup") w2 = new Worm2Dosc21Coup(ep1.rename_file("best.gen.dat"));
    if (model_name == "W2Dosc21CF") w2 = new Worm2Dosc21CF(ep1.rename_file("best.gen.dat"));
    if (model_name == "W2D21") w2 = new Worm21(ep1.rename_file("best.gen.dat"));
    if (model_name == "W2DCE") w2 = new WormCE(ep1.rename_file("best.gen.dat"));


    }else{

    if (model_name == "W2Dosc") w2 = new Worm2DoscNML(ep1.rename_file("worm_data_evo.json"));
    if (model_name == "W2Dosc21") w2 = new Worm2Dosc21NML(ep1.rename_file("worm_data_evo.json"));

    }



    w2->addParsToJson(j);
    ofstream json_out(ep1.rename_file("worm_data_worm.json"));
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

    
    simPars sp1 = {ep1.directoryName, simduration, simtransient, ep1.StepSize};
    Simulation s1(sp1);
    
    cout << "const 1" << endl;

    w2->initForSimulation(rs);
    w2->setStepSize(ep1.StepSize);
    w2->setDataskips(ep1.skip_steps);
    //w->setPrefix("sim");
    w2->InitializeData(ep1.directoryName);

    cout << "const 1" << endl;

    s1.runSimulation(*w2);
    cout << "const 1" << endl;

    
    delete w2;
    return 0;
}