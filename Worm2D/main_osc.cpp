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
    
    InitializeBodyConstants();
    
    string directoryName = cmd->getArgVal("--folder","HJUYGYT");
    if (!directoryExists(directoryName))
    {cout << "Directory doesn't exist." << endl;exit(1);}

    
    const string sup_model_name = cmd->getArgVal("--modelname","");
    double StepSize = 0.005;
    int skip_steps = 10;
    long simrandseed = 42;
    string model_name = sup_model_name;

    string json_filename = rename_file("worm_data_worm.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data_evo.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data.json", directoryName);


   
    json j_orig;
    if (directoryExists(json_filename)) 
    j_orig = getJsonFromFile(json_filename);

   

    if (!j_orig.empty() && (model_name == "" || model_name == "W2DSR")) {
    //if (model_name == ""){
    //if (directoryExists(json_filename)){
        //j_orig = getJsonFromFile(json_filename);
        if (j_orig["Worm"].contains("Main model name"))
        model_name = j_orig["Worm"]["Main model name"]["value"];
        else if (j_orig["Nervous system"].contains("Model name"))
        model_name = j_orig["Nervous system"]["Model name"]["value"];
        
    }
    if (model_name == "") model_name = "W2DSR";

    /* else{
    cout << "Model name is not in json file or the argument list. Exiting." << endl;
    return 0; */
    //}
    


   /*  if (directoryExists(json_filename)){
        json j = getJsonFromFile(json_filename);
        StepSize = j["Evolutionary Optimization Parameters"]["StepSize"]["value"];
        skip_steps = j["Evolutionary Optimization Parameters"]["skip_steps"]["value"];
    }
    else{
    StepSize = 0.005;
    skip_steps = 10;

    } */



    if (model_name == "CE") model_name = "W2DCE";

    json j;
    

    bool do_evol = cmd->getArgValInt("--doevol", 0);
    if (do_evol) 
    {
        Evolution * evo = 0;
    
        if (sup_model_name == "W2DSR" || model_name == "W2DSR") 
        //evo = new EvolutionFullWJ<Worm2DSRE>(j_orig,cmd); 
        evo = new EvolutionFullWJ<WormCO2DSR>(j_orig,cmd);


        else{
        if (model_name == "W2DCE") evo = new EvolutionFullWC<WormCE>(cmd); 
        if (model_name == "W2DCESR") evo = new EvolutionFullWC<WormCESR>(cmd);

        if (model_name == "W2Dosc") evo = new EvolutionFullWC<Worm2Dosc>(cmd);
        if (model_name == "W2DoscH") evo = new EvolutionFullW<Worm2DoscHalf>(cmd);
        if (model_name == "W2Dosc21") evo = new EvolutionFullWC<Worm2Dosc21>(cmd);
        if (model_name == "W2Dosc21S") evo = new EvolutionFullW<Worm2Dosc21S>(cmd);
        if (model_name == "W2Dosc21all") evo = new EvolutionFullWC<Worm2Dosc21all>(cmd);
        if (model_name == "W2Dosc21Coup") evo = new EvolutionFullW<Worm2Dosc21Coup>(cmd);
        if (model_name == "W2Dosc21CF") evo = new EvolutionFullW<Worm2Dosc21CF>(cmd);
        if (model_name == "W2D21") evo = new EvolutionFullWC<Worm21>(cmd); 
        if (model_name == "W2D21R") evo = new EvolutionFullWC<Worm21R>(cmd); 

        if (model_name == "W2D18") evo = new EvolutionFullWC<Worm18>(cmd);
        if (model_name == "W2DCO") evo = new EvolutionFullWC<WormAgent>(cmd);
        }
        //assert(0);
        StepSize = evo->itsEvoPars().StepSize;
        skip_steps = evo->itsEvoPars().skip_steps;

        

        evo->configure();
     
        //evo->addParsToJson(j);

        //this->evopar_ptr->addParsToJson(j["Evolutionary Optimization Parameters"]);
        delete evo;
        //assert(0);
        string json_filename = rename_file("worm_data_evo.json", directoryName);
        json j_evo = getJsonFromFile(json_filename);
        j_evo["Worm"]["Main model name"]["value"] = model_name;
        j_evo["Nervous system"]["Model name"]["value"] = model_name;

        ofstream json_out(json_filename);
        json_out << setprecision(32);
        json_out << std::setw(4) << j_evo << std::endl;
        json_out.close();

    }

    
    json_filename = rename_file("worm_data_evo.json", directoryName);
     if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data_worm.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data.json", directoryName);
   
   //delete w1;
    
    //cout << ep1.rename_file("best.gen.dat") << " " << model_name << endl;

    //bool do_nml =  getParameterInt(argc,argv,"--donml","0");

    
    
    bool do_nml =  cmd->getArgValInt("--donml",0);

    Worm2Dbase * w2;
   

    //const string json_filename = ep1.rename_file("worm_data_evo.json");

    bool do_musclesim = getParameterInt(argc,argv,"--domusc","0");
    bool useGenJson = getParameterInt(argc,argv,"--useGenJson","1");
 
   

    if (sup_model_name == "W2DSR") 
    
    if (do_musclesim) w2 = new Worm2DSRm(json_filename, cmd);
    //else w2 = new Worm2DSR(json_filename, cmd);
    //else w2 = new Worm2DSRE(json_filename, cmd);
    else w2 = new WormCO2DSR(json_filename, cmd);

    else{

    if (!do_nml){

    const string gen_filename =  rename_file("best.gen.dat", directoryName);

    if (model_name == "W2Dosc") w2 = new Worm2Dosc(gen_filename, cmd);
    if (model_name == "W2DoscH") w2 = new Worm2DoscHalf(gen_filename);
    if (model_name == "W2Dosc21") w2 = new Worm2Dosc21(gen_filename, cmd);
    if (model_name == "W2Dosc21S") w2 = new Worm2Dosc21S(gen_filename);
    if (model_name == "W2Dosc21all") w2 = new Worm2Dosc21all(gen_filename, useGenJson, cmd);
    if (model_name == "W2Dosc21Coup") w2 = new Worm2Dosc21Coup(gen_filename);
    if (model_name == "W2Dosc21CF") w2 = new Worm2Dosc21CF(gen_filename);
    if (model_name == "W2D21") w2 = new Worm21(gen_filename, cmd);
    //if (model_name == "W2DCE") w2 = new WormCE(json_filename, gen_filename);
    //if (model_name == "W2DCE") w2 = new WormCE(json_filename, cmd);

    if (model_name == "W2DCE") w2 = new WormCE(cmd, gen_filename);
    //if (model_name == "W2DCE") w2 = new WormCE(gen_filename);
    if (model_name == "W2D21R") w2 = new Worm21R(gen_filename, cmd);
    //if (model_name == "W2DCESR") w2 = new WormCESR(cmd, gen_filename);
    if (model_name == "W2DCESR") w2 = new WormCESR(json_filename, gen_filename, cmd);
    if (model_name == "W2D18") w2 = new Worm18(gen_filename, cmd);
    if (model_name == "W2DCO") w2 = new WormAgent(gen_filename, cmd);

    }else{

   
    if (model_name == "W2Dosc") 
    {
        if (do_musclesim) w2 = new Worm2DoscNMLm(json_filename, cmd);
        else w2 = new Worm2DoscNML(json_filename, cmd);
    }

    if (model_name == "W2Dosc21") 
    {
        if (do_musclesim) w2 = new Worm2Dosc21NMLm(json_filename, cmd);
        else w2 = new Worm2Dosc21NML(json_filename, cmd);
    }

    if (model_name == "W2Dosc21all") 
    {
        if (do_musclesim) w2 = new Worm2Dosc21allNMLm(json_filename, cmd);
        else w2 = new Worm2Dosc21allNML(json_filename, cmd);
    }

    if (model_name == "W2DCE") w2 = new Worm2DCE(json_filename, cmd);

    if (model_name == "W2D21") 
    {

        if (do_musclesim) w2 = new Worm2D21m(cmd);
        else w2 = new Worm2D21(json_filename, cmd);
        

    }

 
   
    }

}

    json_filename = rename_file("worm_data_evo.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data_worm.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data.json", directoryName);

    json j_evo;
    if (directoryExists(json_filename))
    j_evo = getJsonFromFile(json_filename);


  

    if (!j_evo.empty()){
    string jloc;
    if (j_evo.contains("Simulation")) jloc = "Simulation";
    else if (j_evo.contains("Evolutionary Optimization Parameters")) 
    jloc = "Evolutionary Optimization Parameters";

    simrandseed = j_evo[jloc]["randomseed"]["value"];
    StepSize = j_evo[jloc]["StepSize"]["value"];
    skip_steps = j_evo[jloc]["skip_steps"]["value"];
    } 
        
    StepSize = cmd->getArgValDoub("--SimStepSize", StepSize);
    skip_steps = cmd->getArgValDoub("--SimSkipSteps", skip_steps);


//if (do_nml) assert(0);

    const bool prioritizeCmd = cmd->getArgValInt("--prioritizeCmd",0);
    

    if (!(model_name == "W2DCE" || model_name == "W2DCESR") || prioritizeCmd)
    {
    simrandseed =  cmd->getArgValLong("-R",-1);
    if (simrandseed == -1) {cout << "Seed not set properly. Exiting." << endl; return 0;}
    w2->setWormPars(cmd);
    }


    //cout << "ssed " << StepSize << " " << skip_steps << endl;
    //assert(0);

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

  //  if (do_nml) assert(0);

    w2->addParsToJson(j);
    


    const bool dotest = cmd->getArgValInt("--doTestRun",0);
    //const bool dotest = getParameterInt(argc,argv,"--doTestRun","0");

    double simduration = cmd->getArgValDoub("-sd",10);
    double simtransient = cmd->getArgValDoub("-st",10);   
    
    //WormFR* const w = dynamic_cast<WormFR*>(w2);


    if (dotest)
    {

    //if (w!=nullptr) w->setForward();

    //double simduration = getParameterDouble(argc,argv,"-sd","10");
    //double simtransient = getParameterDouble(argc,argv,"-st","10");    
    simPars sp1 = {directoryName, simduration, simtransient, StepSize};
    Simulation s1(sp1);
    s1.runSimulation(*w2);

    //if (do_nml) assert(0);
    j["Simulation"]["transient"]["value"] = simtransient;
    j["Simulation"]["duration"]["value"] = simduration;
    }
    
    else{

    WormFR* const w = nullptr; //dynamic_cast<WormFR*>(w2);
    EvolvableS* const ew = dynamic_cast<EvolvableS*>(w2);

    int zeroGainsType;
    w2->getValCJWorm("SRZeroGainsType", zeroGainsType);
    

    j["Simulation"]["transient"]["value"] = simtransient;
    j["Simulation"]["duration"]["value"] = simduration*2;

    bool forwardfirst = cmd->getArgValInt("--doForwardFirst",1);
    //forwardfirst = getParameterInt(argc,argv,"--doForwardFirst","0");

    simPars sp1 = {directoryName, simduration, simtransient, StepSize};
    Simulation s1(sp1);

    bool doforward = forwardfirst;
    for (int mode=0;mode<2;mode++){
    if (mode==1) {doforward = !forwardfirst; 
        w2->randomizeNS(rs);
        s1.sp.Transient = 0;
        }

    if (w!=nullptr) {if (doforward) w->setForward(); else w->setBackward();}
    else {

        if (ew!=nullptr && zeroGainsType == 1)
        {
        
        json efconds;
        if (doforward) efconds["condval"] = 0; else efconds["condval"] = 1;
        ew->callEfcond(efconds);

        if (false)
    {const NervousSystem & n_ptr1 = dynamic_cast<const NervousSystem&>(w2->itsNS());
    cout << "evo21 states kkds " << n_ptr1.states << endl << endl;
    cout << "evo21 biases kkds " << n_ptr1.biases << endl;
    cout << "evo21 taus kkds " << n_ptr1.taus << endl << endl;
    //assert(0);
    }
        }

        if (doforward) w2->setInputOnce(0); else w2->setInputOnce(1);
    }
 
    s1.runSimulation(*w2);

    }
    }

    j["Simulation"]["StepSize"]["value"] = StepSize;
    j["Simulation"]["skip_steps"]["value"] = skip_steps;
    j["Simulation"]["randomseed"]["value"] = simrandseed;

    //cout << "const 1" << endl;
    j["Worm"]["Main model name"]["value"] = model_name;
    j["Nervous system"]["Model name"]["value"] = model_name;

    if (!j_evo.empty() && j_evo.contains("Evolutionary Optimization Parameters"))
    j["Evolutionary Optimization Parameters"] = j_evo["Evolutionary Optimization Parameters"];

    
    ofstream json_out(rename_file("worm_data_worm.json", directoryName));
    json_out << setprecision(32);
    json_out << std::setw(4) << j << std::endl;
    json_out.close();

    delete w2;
    return 0;
}

