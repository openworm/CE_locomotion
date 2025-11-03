#include "Worm2Dmods.h"
#include "Worm21.h"
#include "WormRS18.h"
#include "Worm2DCE.h"
#include "WormAgent.h"
#include "Evolution.h"


#include <vector>
#include <string>
#include <iostream>




int main (int argc, const char* argv[])
{

    shared_ptr<const CmdArgs> cmd = make_shared<const CmdArgs>(argc, argv);

    
    
   
    string directoryName = cmd->getArgVal("--folder","HJUYGYT");
    if (!directoryExists(directoryName))
    {cout << "Directory doesn't exist." << endl;exit(1);}

    
    string model_name = cmd->getArgVal("--modelname","");
    double StepSize;
    int skip_steps;


    string json_filename = rename_file("worm_data_evo.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data_worm.json", directoryName);

    if (model_name == "") {  
    if (directoryExists(json_filename)){
        json j = getJsonFromFile(json_filename);
        model_name =  j["Nervous system"]["Model name"]["value"];
    }
    else{
    cout << "Model name is not in json file or the argument list. Exiting." << endl;
    return 0;
    }
    }

    if (directoryExists(json_filename)){
        json j = getJsonFromFile(json_filename);
        StepSize = j["Evolutionary Optimization Parameters"]["StepSize"]["value"];
        skip_steps = j["Evolutionary Optimization Parameters"]["skip_steps"]["value"];
    }
    else{
    StepSize = 0.005;
    skip_steps = 10;

    }



    if (model_name == "CE") model_name = "W2DCE";

   

    json j;

    bool do_evol = cmd->getArgValInt("--doevol", 0);
    if (do_evol) 
    {
        Evolution * evo = 0;
    
        if (model_name == "W2DCE") evo = new EvolutionFullW<WormCE>(cmd); 
        if (model_name == "W2DCESR") evo = new EvolutionFullW<WormCESR>(cmd);

        if (model_name == "W2Dosc") evo = new EvolutionFullW<Worm2Dosc>(cmd);
        if (model_name == "W2DoscH") evo = new EvolutionFullW<Worm2DoscHalf>(cmd);
        if (model_name == "W2Dosc21") evo = new EvolutionFullW<Worm2Dosc21>(cmd);
        if (model_name == "W2Dosc21S") evo = new EvolutionFullW<Worm2Dosc21S>(cmd);
        if (model_name == "W2Dosc21all") evo = new EvolutionFullW<Worm2Dosc21all>(cmd);
        if (model_name == "W2Dosc21Coup") evo = new EvolutionFullW<Worm2Dosc21Coup>(cmd);
        if (model_name == "W2Dosc21CF") evo = new EvolutionFullW<Worm2Dosc21CF>(cmd);
        if (model_name == "W2D21") evo = new EvolutionFullW<Worm21>(cmd); 
        if (model_name == "W2D21R") evo = new EvolutionFullW<Worm21R>(cmd); 

        if (model_name == "W2D18") evo = new EvolutionFullW<Worm18>(cmd);
        if (model_name == "W2DCO") evo = new EvolutionFullWC<WormAgent>(cmd);

        //assert(0);
        StepSize = evo->itsEvoPars().StepSize;
        skip_steps = evo->itsEvoPars().skip_steps;
        evo->configure();
       
        evo->addParsToJson(j);
        delete evo;

    }

    
    json_filename = rename_file("worm_data_evo.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data_worm.json", directoryName);

   //delete w1;
    
    //cout << ep1.rename_file("best.gen.dat") << " " << model_name << endl;

    //bool do_nml =  getParameterInt(argc,argv,"--donml","0");

    
    bool do_nml =  cmd->getArgValInt("--donml",0);

    Worm2Dbase * w2;
   

    //const string json_filename = ep1.rename_file("worm_data_evo.json");


    if (model_name == "W2DSR") w2 = new Worm2DSR(json_filename, cmd);

    if (!do_nml){

    const string gen_filename =  rename_file("best.gen.dat", directoryName);

    if (model_name == "W2Dosc") w2 = new Worm2Dosc(gen_filename);
    if (model_name == "W2DoscH") w2 = new Worm2DoscHalf(gen_filename);
    if (model_name == "W2Dosc21") w2 = new Worm2Dosc21(gen_filename);
    if (model_name == "W2Dosc21S") w2 = new Worm2Dosc21S(gen_filename);
    if (model_name == "W2Dosc21all") w2 = new Worm2Dosc21all(gen_filename);
    if (model_name == "W2Dosc21Coup") w2 = new Worm2Dosc21Coup(gen_filename);
    if (model_name == "W2Dosc21CF") w2 = new Worm2Dosc21CF(gen_filename);
    if (model_name == "W2D21") w2 = new Worm21(gen_filename);
    //if (model_name == "W2DCE") w2 = new WormCE(json_filename, gen_filename);
    if (model_name == "W2DCE") w2 = new WormCE(json_filename);
    //if (model_name == "W2DCE") w2 = new WormCE(cmd, gen_filename);
    //if (model_name == "W2DCE") w2 = new WormCE(gen_filename);
    if (model_name == "W2D21R") w2 = new Worm21R(gen_filename);
    //if (model_name == "W2DCESR") w2 = new WormCESR(cmd, gen_filename);
    if (model_name == "W2DCESR") w2 = new WormCESR(json_filename, gen_filename);
    if (model_name == "W2D18") w2 = new Worm18(gen_filename);
    if (model_name == "W2DCO") w2 = new WormAgent(gen_filename,cmd);

    }else{

    if (model_name == "W2Dosc") w2 = new Worm2DoscNML(json_filename);
    if (model_name == "W2Dosc21") w2 = new Worm2Dosc21NML(json_filename);
    if (model_name == "W2DCE") w2 = new Worm2DCE(json_filename);

    }

    json_filename = rename_file("worm_data_evo.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data_worm.json", directoryName);

    const json j_evo = getJsonFromFile(json_filename);
    long simrandseed = j_evo["Evolutionary Optimization Parameters"]["randomseed"]["value"];
    StepSize = j_evo["Evolutionary Optimization Parameters"]["StepSize"]["value"];
    skip_steps = j_evo["Evolutionary Optimization Parameters"]["skip_steps"]["value"];

    const bool prioritizeCmd = cmd->getArgValInt("--prioritizeCmd",0);
    

    if (!(model_name == "W2DCE" || model_name == "W2DCESR") || prioritizeCmd)
    {
    simrandseed =  cmd->getArgValLong("-R",-1);
    if (simrandseed == -1) {cout << "Seed not set properly. Exiting." << endl; return 0;}
    w2->setWormPars(cmd);
    }

    

    RandomState rs;
    rs.SetRandomSeed(simrandseed);
    

    w2->InitializeState(rs);
    //cout << "const 1" << endl;
    w2->initForSimulation(rs);
    w2->setStepSize(StepSize);
    w2->setDataskips(skip_steps);
    //w->setPrefix("sim");
    w2->InitializeData(directoryName);
    //w2->setWormPars(cmd);

    w2->addParsToJson(j);
    


    const bool dotest = cmd->getArgValInt("--doTestRun",0);
    //const bool dotest = getParameterInt(argc,argv,"--doTestRun","0");

    double simduration = cmd->getArgValDoub("-sd",10);
    double simtransient = cmd->getArgValDoub("-st",10);   
    
    WormFR* const w = dynamic_cast<WormFR*>(w2);


    if (dotest || w==nullptr)
    {

    //if (w!=nullptr) w->setForward();

    //double simduration = getParameterDouble(argc,argv,"-sd","10");
    //double simtransient = getParameterDouble(argc,argv,"-st","10");    
    simPars sp1 = {directoryName, simduration, simtransient, StepSize};
    Simulation s1(sp1);
    s1.runSimulation(*w2);

    j["Simulation"]["transient"]["value"] = simtransient;
    j["Simulation"]["duration"]["value"] = simduration;
    }
    
    else{

    j["Simulation"]["transient"]["value"] = simtransient;
    j["Simulation"]["duration"]["value"] = simduration*2;

    bool forwardfirst = cmd->getArgValInt("--doForwardFirst",1);
    //forwardfirst = getParameterInt(argc,argv,"--doForwardFirst","0");


    if (forwardfirst) w->setForward();
    else w->setBackward();


    /*  if (model_name == "W2DCE") 
    {
        //shared_ptr<W2DCEpars> W2DCEpars1(new W2DCEpars(argc,argv));
        WormCE & w = dynamic_cast<WormCE&>(*w2);
        //w.setWormPars(argc,argv);
        //string SRType = getParameter(argc,argv,"--SRType","None");
        //w.setW2DCEpars(W2DCEpars1);
        if (forwardfirst) w.setForward();
        else w.setBackward();
    } */

   

    //double simduration = getParameterDouble(argc,argv,"-sd","10");
    //double simtransient = getParameterDouble(argc,argv,"-st","10");    
    
    simPars sp1 = {directoryName, simduration, simtransient, StepSize};
    Simulation s1(sp1);
    s1.runSimulation(*w2);

    if (forwardfirst) w->setBackward();
        else w->setForward();

    s1.sp.Transient = 0; 
    w->randomizeNS(rs);
    s1.runSimulation(*w2);
    
    }

    /* 
    if (model_name == "W2DCE") 
    {
        s1.sp.Transient = 0;
        WormCE & w = dynamic_cast<WormCE&>(*w2);
        if (forwardfirst) w.setBackward();
        else w.setForward();

      
        s1.runSimulation(*w2);
    }
    */

    //cout << "const 1" << endl;

    ofstream json_out(rename_file("worm_data_worm.json", directoryName));
    json_out << std::setw(4) << j << std::endl;
    json_out.close();

    delete w2;
    return 0;
}

