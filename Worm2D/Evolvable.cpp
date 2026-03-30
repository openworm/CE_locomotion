#include "Evolvable.h"


void EvolvableS::setParsFromPhenoNZ(const TVector<double> &pheno)
{

    setCurrentPheno(pheno);
    setParsFromPheno(pheno);
}

void EvolvableS::callEfcond()
//void EvolvableS::callEfcond(const json & j1_)
{
//vector<double> pheno = getCurrentPheno();


/* for (auto& el : j1_.items())
{
itsEf.itsJson[el.key()] = el.value();
}
 */

 
//Worm2DSRE* w2 = dynamic_cast<Worm2DSRE*>(this);

//cout << itsEf.itsJson << endl;

const vector<double> & phen1 = getCurrentPheno();
//setCurrentPheno(pheno);
//current_pheno.swap(pheno);

assert(phen1.size()>0);

TVector<double> pheno1(1,phen1.size());
for (int i=0;i<phen1.size();i++) pheno1(i+1) = phen1[i];

//cout << pheno1 << endl;
//assert(0);

return setParsFromPheno(pheno1);
//setParsFromPheno(pheno);

}

void EvolvableS::setCurrentPheno(const TVector<double> &pheno)
{
    current_pheno.clear();
    for (int i=1;i<=pheno.Size(); i++) current_pheno.push_back(pheno(i));

}


/* void EvolvableS::setParsFromPhenoAndSet(const TVector<double> &pheno)
{
    current_pheno.clear();
    for (int i=1;i<=pheno.Size(); i++) current_pheno.push_back(pheno(i));
    setParsFromPheno(pheno);
}


 */
/* void EvolvableS::setParsFromPheno(const vector<double> &pheno)
{
TVector<double> pheno1(1,pheno.size());
for (int i=0;i<pheno.size();i++) pheno1(i+1) = pheno[i];
return setParsFromPheno(pheno1);
} */


W2Dbaseparameters::W2Dbaseparameters(int argc, const char* argv[])
{
    assert(0 && "This is depreciated");
    randomInitialState = getParameterInt(argc,argv,"--randomInitialState","0");;
    //cout << "ran " << randomInitialState << endl;
    //assert(0);
}



void W2DbaseparametersNML::setPars(shared_ptr<const CmdArgs> cmd)
{
    randomInitialState = cmd->getArgValInt("--randomInitialState", randomInitialState);
    //cout << "randomInitialState " << randomInitialState << endl;
    //assert(0);

}

void W2Dbaseparameters::setPars(shared_ptr<const CmdArgs> cmd)
{
    
    
    doOrigSRInput = cmd->getArgValInt("--doOrigSRInput", doOrigSRInput);
    doOrigMuscInput = cmd->getArgValInt("--doOrigMuscInput", doOrigMuscInput);
    W2DbaseparametersNML::setPars(cmd);
    //cout << "osm " << doOrigSRInput << " " << doOrigMuscInput << endl;
    //assert(0);

}

void gradParameters::setPars(shared_ptr<const CmdArgs> cmd)
{
    worm_rotation= cmd->getArgValDoub("--rotation", worm_rotation);
    orient_orig = cmd->getArgValDoub("--orient", orient_orig);
    gradSteep = cmd->getArgValDoub("--gradSteep", gradSteep);
    RunDuration = cmd->getArgValDoub("--RunDuration", RunDuration);
    HSStepSize = cmd->getArgValDoub("--HSStepSize", HSStepSize);
    taxis = cmd->getArgValInt("--taxis", taxis);
    kinesis = cmd->getArgValInt("--kinesis", kinesis);
    resetAgentBody = cmd->getArgValInt("--resetAgentBody", resetAgentBody);
    W2Dbaseparameters::setPars(cmd);

}



void W2DCEparsA::setPars(shared_ptr<const CmdArgs> cmd)
{

    AB_output_level =  cmd->getArgValDoub("--ABLevel",AB_output_level );
    AVA_output = cmd->getArgValDoub("--AVAOutputLevel", AVA_output);
    AVB_output = cmd->getArgValDoub("--AVBOutputLevel", AVB_output);
    W2Dbaseparameters::setPars(cmd);
}

W2DCEparsA::W2DCEparsA(int argc, const char* argv[]):W2Dbaseparameters(argc,argv)
{

  AB_output_level = getParameterDouble(argc,argv,"--ABLevel","1");
}

SRCEpars::SRCEpars()
{
    sr_type = "None";
    SRForm = 0;
    nsegperstr = 6;

}

SRCEpars::SRCEpars(shared_ptr<const CmdArgs> cmd){
setPars(cmd);
}

SRRegpars::SRRegpars()
{
    offset = 0;
    nsegperstr = 5;
}

SRRegpars::SRRegpars(shared_ptr<const CmdArgs> cmd):SRCEpars(cmd){}



void SRCEpars::setPars(shared_ptr<const CmdArgs> cmd)
{

sr_type = cmd->getArgVal("--SRType",sr_type);
SRForm = cmd->getArgValInt("--SRForm",SRForm);
//nsegperstr = cmd->getArgValInt("--SRSegPerSR",nsegperstr);
zeroGainsType = cmd->getArgValInt("--SRZeroGainsType",zeroGainsType);


assert(sr_type == "SR_TRANS_STRETCH" ||  sr_type ==  "SR_TRANS_CONTRACT" 
    || sr_type == "SR_TRANS_ABS" 
    || sr_type == "SR_TRANS_NEG" || sr_type == "None");

}


void SRRegpars::setPars(shared_ptr<const CmdArgs> cmd)
{

SRCEpars::setPars(cmd);
nsegperstr = cmd->getArgValInt("--SRSegPerSR",nsegperstr);
//nsegperstr = cmd->getArgValInt("--SRSegPerSR",5);
offset = cmd->getArgValInt("--SROffset",offset);

}




void W2DCEpars::setPars(shared_ptr<const CmdArgs> cmd)
{

  SREvoBot = cmd->getArgValDoub("--SREvoBot",SREvoBot);
  SREvoTop = cmd->getArgValDoub("--SREvoTop",SREvoTop);
  SREvoBotA = cmd->getArgValDoub("--SREvoBotA",SREvoBotA);
  SREvoTopA = cmd->getArgValDoub("--SREvoTopA",SREvoTopA);
  W2DCEparsA::setPars(cmd);
}



W2DCEpars::W2DCEpars(int argc, const char* argv[]):W2DCEparsA(argc,argv)
{
  //sr_type = getParameterString(argc,argv,"--SRType","None");
  //SRForm = getParameterInt(argc,argv,"--SRForm","0");

  assert(0 && "This is depreciated.");
  SREvoBot = getParameterDouble(argc,argv,"--SREvoBot","0");
  SREvoTop = getParameterDouble(argc,argv,"--SREvoTop","200");
  
}

gradEvoPars::gradEvoPars(shared_ptr<const CmdArgs> cmd)
{
setPars(cmd);
}



void gradEvoPars::setPars(shared_ptr<const CmdArgs> cmd)
{
    
    HSStepSize = cmd->getArgValDoub("--HSStepSize", HSStepSize);
    
}


AgarPars::AgarPars(shared_ptr<const CmdArgs> cmd)
{
setPars(cmd);
}

void AgarPars::setPars(shared_ptr<const CmdArgs> cmd)
{
    OSCTbase = cmd->getArgValDoub("--OSCTbase",OSCTbase);
    agarfreq = cmd->getArgValDoub("--agarfreq",agarfreq);
    AvgSpeed = cmd->getArgValDoub("--AvgSpeed",AvgSpeed);

}



AgarPars::AgarPars(int argc, const char* argv[])
{
     assert(0 && "This is depreciated.");
    OSCTbase = getParameterDouble(argc,argv,"--OSCTbase","0.25");
    agarfreq = getParameterDouble(argc,argv,"--agarfreq","0.44");
    AvgSpeed = getParameterDouble(argc,argv,"--AvgSpeed","0.00022");

}

EvolparametersCE::EvolparametersCE(int argc, const char* argv[]):AgarPars(argc,argv)
{
    //doAlternateEvo = atoi(getParameter(argc,argv,"--doAlternateEvo","0"));
    
    assert(0 && "This is depreciated.");

    doReverse = getParameterInt(argc,argv,"--doReverse","0");
    fitType = getParameterInt(argc,argv,"--fitType","0");

    //sr_type = getParameter(argc,argv,"--SRType","None");
}

EvolparametersCE::EvolparametersCE(shared_ptr<const CmdArgs> cmd):AgarPars(cmd)
{

    doReverse = cmd->getArgValInt("--doReverse",doReverse);
    fitType = cmd->getArgValInt("--fitType",fitType);
    zeroGainsType = cmd->getArgValInt("--SRZeroGainsTypeEvo",zeroGainsType);
    doAngleDiff= cmd->getArgValInt("--doAngleDiff",doAngleDiff);
}

void EvolparametersCE::setPars(shared_ptr<const CmdArgs> cmd)
{
    AgarPars::setPars(cmd);
    //doAlternateEvo = atoi(getParameter(argc,argv,"--doAlternateEvo","0"));

    doReverse = cmd->getArgValInt("--doReverse",doReverse);
    fitType = cmd->getArgValInt("--fitType",fitType);
    zeroGainsType = cmd->getArgValInt("--SRZeroGainsTypeEvo",zeroGainsType);
    doAngleDiff= cmd->getArgValInt("--doAngleDiff",doAngleDiff);

    //sr_type = getParameter(argc,argv,"--SRType","None");
}



//EvolvableS::EvolvableS(shared_ptr<W2Dparameters> w2par_ptr_):evolvable_w2par_ptr(w2par_ptr_)
//, Epars1(dynamic_cast<Evolparameters&>(*w2par_ptr))
//{}

//EvolvableS::EvolvableS(){}

Evolparameters::Evolparameters(int argc, const char* argv[], 
    shared_ptr<EvolvableS> & evol1_, string evotype_):AgarPars(argc,argv)
{evol1_->setEvolPars(*this, evotype_);}

Evolparameters::Evolparameters(shared_ptr<const CmdArgs> cmd, 
    shared_ptr<EvolvableS> & evol1_, string evotype_):AgarPars(cmd)
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

void EvolvableS::setParsFromPhenGen(const TVector<double> &phengen, const bool & isPheno)
{

     if (isPheno) setParsFromPheno(phengen);
    else setParsFromGeno(phengen);

}


void EvolvableS::setParsFromGeno(const TVector<double> &geno)
{
    
    //cout << v << endl;
    TVector<double> phenotype(1, geno.Size());
    //cout << phenotype.Size() << endl;
    GenPhenMapping(geno, phenotype);
 
    setParsFromPheno(phenotype);
 
}