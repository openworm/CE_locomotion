#include "Worm2Dmods.h"
#include "Worm21.h"
#include "WormRS18.h"
#include "Worm2DCE.h"
#include "WormAgent.h"
#include "Evolution.h"


#include <vector>
#include <string>
#include <iostream>



//cpbjvbk8yxl
int main (int argc, const char* argv[])
{
    cout << "Starting Worm2D " << W2D_VERSION << endl;

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
        if (j_orig.contains("worm") && j_orig["worm"].contains("main_model_name"))
        model_name = j_orig["worm"]["main_model_name"]["value"];
        else if (j_orig.contains("worm") && j_orig["worm"].contains("Main model name"))
        model_name = j_orig["worm"]["Main model name"]["value"];
        else if (j_orig.contains("Worm") && j_orig["Worm"].contains("Main model name"))
        model_name = j_orig["Worm"]["Main model name"]["value"];
        else if (j_orig.contains("nervous_system") && j_orig["nervous_system"].contains("model_name"))
        model_name = j_orig["nervous_system"]["model_name"]["value"];
        else if (j_orig.contains("Nervous system") && j_orig["Nervous system"].contains("Model name"))
        model_name = j_orig["Nervous system"]["Model name"]["value"];
        
    }
    if (model_name == "") model_name = "W2DSR";

    if (!j_orig.is_null()) j_orig.erase("Simulation");

    
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
        j_evo["worm"]["main_model_name"]["value"] = model_name;
        j_evo.erase("Worm");
        j_evo.erase("Nervous system");
        j_evo.erase("Dorsal NMJ");
        j_evo.erase("Ventral NMJ");
        j_evo.erase("Dorsal body");
        j_evo.erase("Ventral body");
        j_evo.erase("Stretch receptor");
        j_evo.erase("VNC NMJ");
        j_evo.erase("VNC 18");
        j_evo.erase("Driving input");

        ofstream json_out(json_filename);
        json_out << setprecision(32);
        json_out << std::setw(4) << j_evo << std::endl;
        json_out.close();

    }

    if (do_evol)
    json_filename = rename_file("worm_data_evo.json", directoryName);
    
    if (false){
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data_worm.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data.json", directoryName);
    }

   //delete w1;
    
    //cout << ep1.rename_file("best.gen.dat") << " " << model_name << endl;

    //bool do_nml =  getParameterInt(argc,argv,"--donml","0");

   //cout << "shhd " << endl; 
    
    bool do_nml =  cmd->getArgValInt("--donml",0);

    Worm2Dbase * w2;
   

    //const string json_filename = ep1.rename_file("worm_data_evo.json");

    bool do_musclesim = getParameterInt(argc,argv,"--domusc","0");
    bool useGenJson = getParameterInt(argc,argv,"--useGenJson","1");
 
   
    if (sup_model_name == "W2DSR") {
    
    if (do_musclesim){w2 = new Worm2DSRm(json_filename, cmd);}
    //else w2 = new Worm2DSR(json_filename, cmd);
    //else w2 = new Worm2DSRE(json_filename, cmd);
    else w2 = new WormCO2DSR(json_filename, cmd);

   

    }
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

    if (false){ 
    json_filename = rename_file("worm_data_evo.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data_worm.json", directoryName);
    if (!directoryExists(json_filename))
    json_filename = rename_file("worm_data.json", directoryName);
    }

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
    
   // cout << "shds " << simrandseed << " " << StepSize << " " << skip_steps << endl;
    StepSize = cmd->getArgValDoub("--SimStepSize", StepSize);
    skip_steps = cmd->getArgValDoub("--SimSkipSteps", skip_steps);


//if (do_nml) assert(0);

    const bool prioritizeCmd = cmd->getArgValInt("--prioritizeCmd",0);
    

    if (!(model_name == "W2DCE" || model_name == "W2DCESR") || prioritizeCmd)
    {
    simrandseed =  cmd->getArgValLong("-R", simrandseed);
    w2->setWormPars(cmd);
    
    }


    //cout << "ssed " << StepSize << " " << skip_steps << endl;
    //assert(0);

    RandomState rs;
    rs.SetRandomSeed(simrandseed);
    //cout << "simrandseed " << simrandseed << endl;

    w2->setStepSize(StepSize);
    w2->InitializeState(rs);

    if (false)
    {const NervousSystem & n_ptr1 = dynamic_cast<const NervousSystem&>(w2->itsNS());
    cout << "mo states kkds " << n_ptr1.states << endl << endl;
    cout << "mo biases kkds " << n_ptr1.biases << endl;
    cout << "mo taus kkds " << n_ptr1.taus << endl << endl;
    //assert(0);
    }
    

    //cout << "const 1" << endl;
    w2->initForSimulation(rs);
    w2->setDataskips(skip_steps);
    //w->setPrefix("sim");
    w2->InitializeData(directoryName);
    //w2->setWormPars(cmd);

  //  if (do_nml) assert(0);

   

    //w2->addParsToJson(j);
    
    bool dotest;
    w2->getValCJWorm("do_test_run", dotest);
    //const bool dotest = cmd->getArgValInt("--doTestRun",0);
    //const bool dotest = getParameterInt(argc,argv,"--doTestRun","0");

    double simduration = 10;
    double simtransient = 10;
    if (!j_evo.empty()){
        if (j_evo.contains("Simulation")){
            const json& j_sim = j_evo["Simulation"];
            if (j_sim.contains("duration")) simduration = j_sim["duration"]["value"];
            else if (j_sim.contains("Duration")) simduration = j_sim["Duration"]["value"];
            if (j_sim.contains("transient")) simtransient = j_sim["transient"]["value"];
            else if (j_sim.contains("Transient")) simtransient = j_sim["Transient"]["value"];
        }
        else if (j_evo.contains("Evolutionary Optimization Parameters")){
            const json& j_sim = j_evo["Evolutionary Optimization Parameters"];
            if (j_sim.contains("Duration")) simduration = j_sim["Duration"]["value"];
            if (j_sim.contains("Transient")) simtransient = j_sim["Transient"]["value"];
        }
    }
    simduration = cmd->getArgValDoub("-sd", simduration);
    simtransient = cmd->getArgValDoub("-st", simtransient);   
    
    //WormFR* const w = dynamic_cast<WormFR*>(w2);
    int inputInd;
    w2->getValCJ("input_ind", inputInd, "input_switcher");
    WormCO2DSR* const w2dsre = dynamic_cast<WormCO2DSR*>(w2);
    if (dotest)
    {
    
    if (true){
    if (w2dsre) w2dsre->applyFuncablesExt();
    if (inputInd>=0) w2->setInputOnce(inputInd);



    }


    simPars sp1 = {directoryName, simduration, simtransient, StepSize};
    Simulation s1(sp1);

    if (false){
    cout << "gdgs hs s " << w2->itsBPjson().at("nervous_system") << endl;
    
    }

   
    //assert(!do_musclesim); 
    w2->addParsToJson(j);
   

    s1.runSimulation(*w2);

    

    //if (do_nml) assert(0);
    j["Simulation"]["transient"]["value"] = simtransient;
    j["Simulation"]["duration"]["value"] = simduration;
    }
    
    else{

    WormFR* const w = nullptr; //dynamic_cast<WormFR*>(w2);
    EvolvableS* const ew = dynamic_cast<EvolvableS*>(w2);
   
    int zeroGainsType;
    w2->getValCJ(
        "sr_zero_gains_type", zeroGainsType, "stretch_receptor"
    );
    int doReverse;
    w2->getValCJWorm("do_reverse", doReverse);

    if (doReverse == 0 || doReverse == 1)
    {

    j["Simulation"]["transient"]["value"] = simtransient;
    j["Simulation"]["duration"]["value"] = simduration;
    simPars sp1 = {directoryName, simduration, simtransient, StepSize};
    Simulation s1(sp1);

    if (ew!=nullptr && zeroGainsType == 1)
        {
        
        json efconds = json::object();
        efconds["f_ind"] = 2;
        if (doReverse == 0) efconds["condval"] = 0;
        else efconds["condval"] = 1;
        w2->itsEf.itsJson = efconds;
        if (w2dsre) w2dsre->applyFuncablesExt();
        else ew->callEfcond();

        }
    if (inputInd<0){
    if (doReverse == 0) w2->setInputOnce(0); else w2->setInputOnce(1);
    }
    else w2->setInputOnce(inputInd);
    
    w2->addParsToJson(j);

    s1.runSimulation(*w2);
    }

    
    else if (doReverse == 2 || doReverse == 3){

    j["Simulation"]["transient"]["value"] = simtransient;
    j["Simulation"]["duration"]["value"] = simduration*2;

    bool forwardfirst = cmd->getArgValInt("--doForwardFirst",1);
    //forwardfirst = getParameterInt(argc,argv,"--doForwardFirst","0");

    simPars sp1 = {directoryName, simduration, simtransient, StepSize};
    Simulation s1(sp1);
    
    w2->addParsToJson(j);

    bool doforward = forwardfirst;
    for (int mode=0;mode<2;mode++){
    if (mode==1) {doforward = !forwardfirst; 
        //w2->randomizeNS(rs);
        s1.sp.Transient = 0;
        }

    if (w!=nullptr) {if (doforward) w->setForward(); else w->setBackward();}
    else {

        if (ew!=nullptr && zeroGainsType == 1)
        {
        
        json efconds = json::object();
        efconds["f_ind"] = 2;
        if (doforward) efconds["condval"] = 0;
        else efconds["condval"] = 1;
        w2->itsEf.itsJson = efconds;
        if (w2dsre) w2dsre->applyFuncablesExt();
        else ew->callEfcond();

  
        }

        if (doforward) w2->setInputOnce(0); else w2->setInputOnce(1);
    }
 
   


    s1.runSimulation(*w2);

    }
    }

    }


    //w2->addParsToJson(j);

    j["Simulation"]["StepSize"]["value"] = StepSize;
    j["Simulation"]["skip_steps"]["value"] = skip_steps;
    j["Simulation"]["randomseed"]["value"] = simrandseed;

    //cout << "const 1" << endl;
    j["worm"]["main_model_name"]["value"] = model_name;

    if (!j_evo.empty() && j_evo.contains("Evolutionary Optimization Parameters")){
    j["Evolutionary Optimization Parameters"] = j_evo["Evolutionary Optimization Parameters"];
    json& evo_json = j["Evolutionary Optimization Parameters"];
    if (evo_json.contains("evoType")){
        if (!evo_json.contains("evo_type")) evo_json["evo_type"] = evo_json["evoType"];
        evo_json.erase("evoType");
    }
    if (evo_json.contains("EvolutionType")){
        if (!evo_json.contains("evo_type")) evo_json["evo_type"] = evo_json["EvolutionType"];
        evo_json.erase("EvolutionType");
    }
    }

    appendNSCellClassesToJson(j, w2->getSectionNames());
    w2->cleanLegacyParameterKeys(j);
    if (j.contains("worm")) j["worm"].erase("hs_step_size");
    if (j.contains("Evolutionary Optimization Parameters"))
    {
        j["Evolutionary Optimization Parameters"].erase("hs_step_size");
        j["Evolutionary Optimization Parameters"].erase("HSStepSize");
    }
    j.erase("Worm");
    j.erase("Nervous system");
    j.erase("Dorsal NMJ");
    j.erase("Ventral NMJ");
    j.erase("Dorsal body");
    j.erase("Ventral body");
    j.erase("Stretch receptor");
    j.erase("VNC NMJ");
    j.erase("VNC 18");
    j.erase("Driving input");
    
    ofstream json_out(rename_file("worm_data_worm.json", directoryName));
    json_out << setprecision(32);
    json_out << std::setw(4) << j << std::endl;
    json_out.close();

    delete w2;
    return 0;
}
