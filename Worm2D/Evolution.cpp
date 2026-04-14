#include "Evolution.h"
//#include <string>
#include <string.h>
#include <sys/stat.h>
//#include <stdio.h>
#include <iostream>



string EvoBase::rename_file(string filename){return evoPars1.directoryName + "/" + 
    evoPars1.fileprefix + filename;}


EvoBase::EvoBase(shared_ptr<const CmdArgs> cmd_, evoPars ep1)
:evoPars1(setPars(cmd_,ep1)),simPars1(setSimPars(cmd_)),
writeBestFlag(true),setFromCPTflag(false)
{

construct(0,0);
phenotype.SetBounds(1, itsVectSize());
phenotype.FillContents(0.0);

    //set phenotype

}

EvoBase::EvoBase(shared_ptr<const CmdArgs> cmd_, evoPars ep1, string prefix_)
:evoPars1(setPars(cmd_,ep1,prefix_)),simPars1(setSimPars(cmd_)),
writeBestFlag(true),setFromCPTflag(false)
{

construct(0,0);
phenotype.SetBounds(1, itsVectSize());
phenotype.FillContents(0.0);
    //set phenotype

}


EvoBase::EvoBase(int argc, const char* argv[], evoPars ep1, int VectSize_)
    :evoPars1(setPars(argc,argv,ep1)),//s(new TSearch(VectSize_)),//VectSize(VectSize_),
    simPars1(setSimPars(argc,argv)),writeBestFlag(true),//phenotype(1, VectSize_),
    //phenprev(1, VectSize_),genprev(1, VectSize_),
    setFromCPTflag(false)
    {
        construct(VectSize_,0);
        phenotype.SetBounds(1, itsVectSize());
        phenotype.FillContents(0.0);
        //setFromCPT();
        //setPopFromBestGenoFile();


    }
  
EvoBase::EvoBase(shared_ptr<const CmdArgs> cmd_, evoPars ep1, int VectSize_)
    :evoPars1(setPars(cmd_,ep1)),//s(new TSearch(VectSize_)),//VectSize(VectSize_),
    simPars1(setSimPars(cmd_)),writeBestFlag(true),//phenotype(1, VectSize_),
    setFromCPTflag(false)
    {  //assert(0);
        //setFromCPT();
        //setPopFromBestGenoFile();

        construct(VectSize_,0);
        phenotype.SetBounds(1, itsVectSize());
        phenotype.FillContents(0.0);
    }

EvoBase::EvoBase(int argc, const char* argv[], evoPars ep1, int VectSize_, string prefix_)
    :evoPars1(setPars(argc,argv,ep1,prefix_)),//s(new TSearch(VectSize_)),//VectSize(VectSize_),
    simPars1(setSimPars(argc,argv)),writeBestFlag(true),//phenotype(1, VectSize_),
    //phenprev(1, VectSize_),genprev(1, VectSize_),
    setFromCPTflag(false)
    {
       

        //setFromCPT();
        //setPopFromBestGenoFile();
        construct(VectSize_,0);
        phenotype.SetBounds(1, itsVectSize());
        phenotype.FillContents(0.0);

    }

EvoBase::EvoBase(shared_ptr<const CmdArgs> cmd_, evoPars ep1, int VectSize_, string prefix_)
    :evoPars1(setPars(cmd_,ep1,prefix_)),//s(new TSearch(VectSize_)),//VectSize(VectSize_),
    simPars1(setSimPars(cmd_)),writeBestFlag(true),//phenotype(1, VectSize_),
    //phenprev(1, VectSize_),genprev(1, VectSize_),
    setFromCPTflag(false)
    {
         cout << "Evo construct " << prefix_ << endl;
        //assert(0);
        //setFromCPT();
        //setPopFromBestGenoFile();
        construct(VectSize_,0);
        phenotype.SetBounds(1, itsVectSize());
        phenotype.FillContents(0.0);

    }


void EvoBase::checkPars()
{
    /* if (s->VectorSize() != VectSize){
         cout << "cpt vectorsize "  << s->VectorSize() << " EvoBase vectorsize " <<  VectSize << endl;
         assert(s->VectorSize() == VectSize && "Vectorsize is not correct");
    } */

    if (s->PopulationSize()!= evoPars1.PopulationSize) 
    {cout << "setting " <<  " population size to cpt population size: " << s->PopulationSize() << endl;
    popsize = s->PopulationSize();}


}


/* void EvoBase::constructAll(int vsize_, int offset_)
{
    bool foundCPTfile = false, foundBGfile = false, foundWRBGfile = false;

    
    struct stat buffer;  
    string bgfilename = rename_file("best.gen.dat");
    if (stat (bgfilename.c_str(), &buffer) == 0) foundBGfile = true;
    string wrbgfilename = rename_file("EvoWJbest.gen.dat");
    if (stat (wrbgfilename.c_str(), &buffer) == 0) foundWRBGfile = true;
    string cptfilename = rename_file("search.cpt");
    if (stat (cptfilename.c_str(), &buffer) == 0) foundCPTfile = true;

    if (foundCPTfile){
    TSearch * s = new TSearch(1);
    s->cptfilename = cptfilename:
    s->ReadCheckpointFile();
        
    }
}
 */


void EvoBase::construct(int vsize_, int offset_)
{


    if (!setFromCPTflag) setFromCPT2(vsize_);
    if (doResume) return;

    string filename;
    bool foundFile = false;
    filename = rename_file("best.gen.dat");
    struct stat buffer;   
    if (stat (filename.c_str(), &buffer) == 0) 
    {
     vector<double> bestgenvec;
     getVecFromFile<double>(filename, bestgenvec);
     if (bestgenvec.size()==vsize_) foundFile = true;
    }

    if (foundFile == false){
    filename = rename_file("EvoWJbest.gen.dat");
    if (stat (filename.c_str(), &buffer) == 0) 
    {
        vector<double> bestgenvec;
        getVecFromFile<double>(filename, bestgenvec);
        assert(bestgenvec.size()==vsize_);
        foundFile = true;
    }
    }

    if (foundFile) {

    cout << "const from best gen " << filename << endl;
    //assert(0 && "setting from best gen");
    vector<double> bestgenvec;
    getVecFromFile<double>(filename, bestgenvec);

    if (vsize_>0)
    {
        assert(vsize_>=bestgenvec.size() && "bestgenvec too large");
        s = new TSearch(vsize_);
    }

    else s = new TSearch(bestgenvec.size());

    
    assert((bestgenvec.size() + offset_) <= s->IndividualT(1).Size());

    configure_p11();
    s->InitializeSearch();
    for (int i = 1; i <= s->PopulationSize(); i++) 
    for (int j = 1; j <= bestgenvec.size(); j++)
    s->IndividualT(i)(j + offset_) = bestgenvec[j-1];
    doResume = false;
    return;
    }

    assert(vsize_>0);
    s = new TSearch(vsize_);
    //configure_p1();
    //s->InitializeSearch();
    doResume = false;
    cout << "const from default with size " << vsize_ << endl;
    //cout << " construct filename " << filename << endl;
    //assert(0);
    return;

}



void EvoBase::setFromCPT2(int vsize_)
{
    doResume = false;
    setFromCPTflag = true;
    popsize = evoPars1.PopulationSize;
    
    const string filename_ = rename_file("search.cpt");
    
    //if (filename_ != "testruns/testCO18Full/CO18Full_search.cpt") assert(0);
   

    cout << "docpt " << doCPT << endl;

    struct stat buffer;   
    if (doCPT && (stat (filename_.c_str(), &buffer) == 0)) {

      //  cout << "set from cpt " << filename_ << endl; 
        {TSearch * stest = new TSearch(1);
        stest->cptfilename = filename_;
        stest->ReadCheckpointFile();
        const int testVsize = stest->VectorSize();
        delete stest;
        if (testVsize!=vsize_) return;
        }

        s = new TSearch(1);
        s->cptfilename = filename_;
        s->ReadCheckpointFile();
        cout << "setFromCPT2 " << s->cptfilename << endl;
        doResume = true;
        //ResultsDisplay(*s);
        //checkPars();
        //configure_p1();
        if (s->PopulationSize()!= evoPars1.PopulationSize) 
        {cout << "setting " <<  " population size to cpt population size: " << s->PopulationSize() << endl;
        popsize = s->PopulationSize();}

        return;
        //assert(0);

    }
    
    
    return;
   
}



void EvoBase::setFromCPT()
{
    setFromCPTflag = true;
    popsize = evoPars1.PopulationSize;
    s->cptfilename = rename_file("search.cpt");
   
    struct stat buffer;   
    if (doCPT && evoPars1.CheckpointInterval>0 && (stat (s->cptfilename.c_str(), &buffer) == 0)) {
    
        s->ReadCheckpointFile();
        cout << "setFromCPT " << s->cptfilename << endl;
        doResume = true;
        //ResultsDisplay(*s);
        checkPars();
    }
    else doResume = false;
   
} 

void EvoBase::setUp()
{   
    s->cptfilename = rename_file("search.cpt");
    //setFromCPT();
    
   
    if  (false) {
        fileDropLines<double>(rename_file("fitness.dat"), s->Generation(), 4);
        fileDropLines<double>(rename_file("genhistory.dat"), s->Generation(), s->VectorSize()*3 + 1);
        //fileDropLines<double>(rename_file("gendiffhistory.dat"), s->Generation(), s->VectorSize()*2 + 1);
    }

    //auto ioflag = std::ios_base::out;
    //if (doCPT) ioflag = std::ios_base::app;
    //auto ioflag = std::ios_base::app;

    string filename_ = rename_file("fitness.dat");
    struct stat buffer; 
    if (stat (filename_.c_str(), &buffer) == 0) 
    {

    vector<double> col1 = fileGetCol<double>(filename_,4,0);
    evolfile.open(filename_, std::ios_base::app);
    initGenNum = col1[col1.size()-1] - s->Generation() + 1;

    }
    else 
    {
        initGenNum = 0;
        evolfile.open(filename_, std::ios_base::out);
    }

    //setFromCPT();

    filename_ = rename_file("genhistory.dat");
    if (stat (filename_.c_str(), &buffer) == 0) genhistfile.open(filename_, std::ios_base::app);
    else genhistfile.open(filename_, std::ios_base::out);
    //genhistfile2.open(rename_file("gendiffhistory.dat"), ioflag);
    //doneFirst = false;
    evolfile << setprecision(10);

    
}






void EvoBase::setPopFromBestGenoFile(int offset)
{
   
    if (!setFromCPTflag) setFromCPT();
    if (doResume) return;

    string filename = rename_file("best.gen.dat");
    struct stat buffer;   
    if (doCPT && stat (filename.c_str(), &buffer) == 0) {
           assert(0 && "setting from best gen");
   

    //ifstream ifs;
    //ifs.open(filename);
    //double val;
    vector<double> bestgenvec;
    getVecFromFile<double>(filename, bestgenvec);

   /*  while (ifs >> val)
    {
        bestgenvec.push_back(val);
    }
    ifs.close(); */

    s->InitializeSearch();


    cout << "popsize " << s->PopulationSize() 
    << " indsize " << s->IndividualT(1).Size() << " bestsize " << bestgenvec.size() << endl;
   //assert(0);

    //s->InitializeSearch();

    for (int i = 1; i <= s->PopulationSize(); i++) 
    for (int j = 1 + offset; j <= s->IndividualT(i).Size(); j++)
    s->IndividualT(i)(j) = bestgenvec[j-1];
    
    doResume = true;
    //s->Gen = 0;
	// Set up the initial population
	//RandomizePopulation();
	// The search is now initialized
	//s->SearchInitialized = 1;

     //assert(0);
    }

    else doResume = false;


}



void EvoBase::setFromEvol(const EvoBase & er, int offset, int vsize_)
{

    

    if (!setFromCPTflag) setFromCPT2(vsize_);
    if (doResume) return;

    

    //configure_p11();
    //s->InitializeSearch();

    //assert(0);
    cout << "setFromEvol original pop size " 
    << s->PopulationSize() << "loaded pop size " 
    << er.s->PopulationSize() << endl;


    //s->InitializeSearch();
    int minsize = s->PopulationSize();
    if (er.s->PopulationSize() < minsize) 
        minsize = er.s->PopulationSize();


    //assert(s->PopulationSize() == er.s->PopulationSize());
    //assert(evoPars1.VectSize==er.evoPars1.VectSize + offset);

    cout << "using " << minsize << endl;
    for (int i = 1; i <= minsize; i++) 
    for (int j = 1; j <= er.s->IndividualT(i).Size(); j++)
    s->IndividualT(i)(j+offset) = er.s->IndividualT(i)(j);

   
    
      //assert(0);

    s->Gen = 0;
	// Set up the initial population
	//RandomizePopulation();
	// The search is now initialized
	s->SearchInitialized = 1;

    doResume = false;


 
//assert(0);
//doResume = true;

}



void EvoBase::writeJson1(Worm2Dbase & w)
{
json j;
writeJson1(w,j);
}

void EvoBase::writeJson1(Worm2Dbase & w, json & j)
{   
    
   

    RandomState rs;
    rs.SetRandomSeed(evoPars1.randomseed);
   
   
    w.setStepSize(evoPars1.StepSize);
    w.setDataskips(evoPars1.skip_steps);
    w.setPrefix();
    w.InitializeData(evoPars1.directoryName);

    w.InitializeState(rs); 
    w.initForSimulation(rs);
    

    ofstream json_out(rename_file("worm_data_evo.json"));
    w.addParsToJson(j);   
    addParsToJson(j);
   
    json_out << setprecision(32);
    json_out << std::setw(4) << j << std::endl;
    json_out.close(); 
  
}

void EvoBase::addParsToJson(json & j)
{  
    
    //doubIntParamsHead par1pars = evoPars1.getParams();
    //appendToJson<double>(j[par1pars.parDoub.head],par1pars.parDoub);
    //appendToJson<long>(j[par1pars.parInt.head],par1pars.parInt);
    evoPars1.addParsToJson(j["Evolutionary Optimization Parameters"]);
    
 
    j["Evolutionary Optimization Parameters"]["VectSize"]["value"] = itsVectSize();

    addExtraParsToJson(j);
}

simPars EvoBase::setSimPars(int argc, const char* argv[])
{

simPars sp1;
sp1.Duration = evoPars1.Duration;
sp1.Transient = evoPars1.Transient;

 if (((argc-1) % 2) != 0)
     {cout << "The arguments are not configured correctly." << endl;exit(1);}

 for (int arg = 1; arg<argc; arg+=2){
    if (strcmp(argv[arg],"-sd")==0) sp1.Duration = stod(argv[arg+1]);
    if (strcmp(argv[arg],"-st")==0) sp1.Transient = stod(argv[arg+1]);
}

return sp1;

}

simPars EvoBase::setSimPars(shared_ptr<const CmdArgs> cmd)
{

simPars sp1;
sp1.Duration = evoPars1.Duration;
sp1.Transient = evoPars1.Transient;

sp1.Duration = cmd->getArgValDoub("-sd", evoPars1.Duration);
sp1.Transient = cmd->getArgValDoub("-st", evoPars1.Transient);

return sp1;

}




evoPars EvoBase::setPars(int argc, const char* argv[], evoPars ep1)
{
return setPars(argc,argv,ep1,"");
}

evoPars EvoBase::setPars(shared_ptr<const CmdArgs> cmd, evoPars ep1)
{
return setPars(cmd,ep1,"");
}

evoPars EvoBase::setPars(shared_ptr<const CmdArgs> cmd, evoPars ep1, string prefix_)
{
ep1.setFromArgs(cmd);
doCPT = (bool) cmd->getArgValInt("-docpt",1);

//cout << "docpt " << doCPT << endl;
//assert(0);

ep1.fileprefix = prefix_;

return ep1;
}


evoPars EvoBase::setPars(int argc, const char* argv[], evoPars ep1, string prefix_){

    ep1.setFromArgs(argc,argv);

    
    doCPT = true;

    for (int arg = 1; arg<argc; arg+=2)
    { 
    
    if (strcmp(argv[arg],"-docpt")==0) doCPT = stoi(argv[arg+1]);

    }

    ep1.fileprefix = prefix_;

    //evoParsNC.filePrefix = "";

    return ep1;

}

void EvolutionaryRunDisplay_try(int Generation, double BestPerf, double AvgPerf, double PerfVar)
{

assert(0);

}


void Evolution::EvolutionaryRunDisplay(int Generation, double BestPerf, double AvgPerf, double PerfVar)
{
    
    assert(s && "s not set ");

    //cout << "EvolutionaryRunDisplay" << endl;
    
    evolfile << Generation  + initGenNum;
    vector<double> evovals{BestPerf,AvgPerf,PerfVar};
    for (int i=0;i<evovals.size();i++) 
        if (isnan(evovals[i])) evolfile << " " << 0.0;
        else evolfile << " " << evovals[i];
    evolfile << endl;

    //evolfile << Generation << " " << BestPerf << " " << AvgPerf << " " << PerfVar << endl;
    if (writeBestFlag) ResultsDisplay(*s);

    //TVector<double> & phencur =  getBestPhenotype();
    const TVector<double> & gencur =  getBestGenotype();
 
  

    TVector<double> phencur(1, itsVectSize());
    phencur.FillContents(0.0);
    GenPhenMapping(gencur, phencur);

    genhistfile << (Generation + initGenNum) << " " << gencur << " " << phencur;

   

    TVector<double> avphen(1, itsVectSize());
    avphen.FillContents(0.0);
    //for (int j = 1; j <= avphen.Size(); j++) avphen(j)=0;

    for (int i = 1; i <= s->PopulationSize(); i++) {
        TVector<double> phenotype(1, itsVectSize());
        phenotype.FillContents(0.0);
        GenPhenMapping(s->IndividualT(i), phenotype);
        for (int j = 1; j <= phenotype.Size(); j++) 
        avphen(j) =  avphen(j) + phenotype(j);    
    }
    for (int j = 1; j <= avphen.Size(); j++) avphen(j)= avphen(j)/s->PopulationSize();
    
    genhistfile << " " << avphen << endl;

    //cout << phencur.Size() << " " << phencur << endl;
    writeJson(phencur);
  
    

}

const TVector<double> & Evolution::getBestPhenotype()
{

//TVector<double> phenotype(1, itsEvoPars().VectSize);   
const TVector<double> & bestVector = s->BestIndividualT();
GenPhenMapping(bestVector, phenotype);
return phenotype;

}
 


const TVector<double> & EvoBase::getBestGenotype()
{
    return s->BestIndividualT();
}



void Evolution::ResultsDisplay(TSearch &s)
{
    //assert(0);
    //TVector<double> bestVector;
    const TVector<double> & bestVector = s.BestIndividualT();

    {ofstream BestIndividualFile;
    //bestVector = s.BestIndividual();
    BestIndividualFile.open(rename_file("best.gen.dat"));
    //BestIndividualFile.open(bestfilename);
    BestIndividualFile << setprecision(32);
    BestIndividualFile << bestVector << endl;
    BestIndividualFile.close();}

    {
    ofstream BestIndividualFile;
    BestIndividualFile.open(rename_file("best.phen.dat"));
    TVector<double> bestPheno(1,bestVector.Size());
    bestPheno.FillContents(0);
    GenPhenMapping(bestVector,bestPheno);
    //BestIndividualFile.open(bestfilename);
    BestIndividualFile << setprecision(32);
    BestIndividualFile << bestPheno << endl;
    BestIndividualFile.close();
    }



}

void EvoBase::configure_p11()
{
    int gentot = s->Generation() + evoPars1.MaxGenerations;

    if (configP1Called) return;
    configP1Called = true;

    s->SetSelectionMode(evoPars1.SelectionMode);             //{FITNESS_PROPORTIONATE,RANK_BASED}
    s->SetReproductionMode(evoPars1.ReproductionMode);	// {HILL_CLIMBING, GENETIC_ALGORITHM}
    s->SetPopulationSize(popsize); //96
    s->SetMaxGenerations(gentot); //1000
    s->SetMutationVariance(evoPars1.MutationVariance);                // For 71 parameters, an estimated avg change of 0.25 for weights (mapped to 15).
    s->SetCrossoverProbability(evoPars1.CrossoverProbability);
    s->SetCrossoverMode(evoPars1.CrossoverMode);              //{UNIFORM, TWO_POINT}
    s->SetMaxExpectedOffspring(evoPars1.MaxExpectedOffspring);
    s->SetElitistFraction(evoPars1.ElitistFraction);
    s->SetSearchConstraint(evoPars1.SearchConstraint);
    s->SetCheckpointInterval(evoPars1.CheckpointInterval);
    s->SetReEvaluationFlag(evoPars1.ReEvaluationFlag);

}


void Evolution::configure_p1()
{
    
    s->SetRandomSeed(evoPars1.randomseed);
    //cout << "tad " << evoPars1.randomseed << endl;
    //assert(0);

    if (true){
    {typedef void (*callback_t)(int, double, double, double);
    Callback<void(int, double, double, double)>::func 
    = std::bind(&Evolution::EvolutionaryRunDisplay, this, 
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    callback_t func = static_cast<callback_t>(Callback<void(int, double, double, double)>::callback); 
    s->SetPopulationStatisticsDisplayFunction(func);
    }
    }
    else s->SetPopulationStatisticsDisplayFunction(EvolutionaryRunDisplay_try);

    {typedef void (*callback_t)(TSearch&);
    Callback<void(TSearch&)>::func = std::bind(&Evolution::ResultsDisplay, this, std::placeholders::_1);
    callback_t func = static_cast<callback_t>(Callback<void(TSearch&)>::callback); 
    s->SetSearchResultsDisplayFunction(func);
    }

    configure_p11();
   

    
}


void Evolution::configure_p2()
{
    
    s->SetSearchTerminationFunction(NULL);

    {typedef double (*callback_t)(TVector<double> &, RandomState &);
    Callback<double(TVector<double> &, RandomState &)>::func = std::bind(&Evolution::EvaluationFunction, this, 
            std::placeholders::_1, std::placeholders::_2);
    callback_t func = static_cast<callback_t>(Callback<double(TVector<double> &, RandomState &)>::callback);
    s->SetEvaluationFunction(func);}
    
    if (doResume) {cout << "Resuming search" << endl; s->DoSearch(1);}
    else s->ExecuteSearch();
  
   
}

void Evolution::configure()
{
    setUp();
    configure_p1();
    configure_p12();
    configure_p2();
    evolfile.close();
    genhistfile.close();
   // genhistfile2.close();
}

   
void Evolution::RunStandardSimulation(Worm2Dm & w, RandomState &rs){

    
    //Worm2D21 & w = dynamic_cast<Worm2D21&>(w1);

    const double & Duration = evoPars1.Duration;
    //const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;
    //const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = evoPars1.Transient;
    const int & skip_steps = evoPars1.skip_steps;

    ofstream paramsfile;//, velfile;

    //bodyfile.open(rename_file("body2.dat"));
    //actfile.open(rename_file("act2.dat"));
    //curvfile.open(rename_file("curv2.dat"));
    paramsfile.open(rename_file("sts_params.dat"));
    //velfile.open(rename_file("sts_velocity.dat"));

    w.setPrefix("sts");
    w.setBasename(itsEvoPars().directoryName);
    w.setDataskips(itsEvoPars().skip_steps);
    //w.dataReset();

    w.DumpParams(paramsfile);
    paramsfile.close();

    w.InitializeState(rs);
    
    for (double t = 0.0; t <= 50; t += StepSize) w.Step(StepSize);
       
        double xt = w.CoMx();
        double yt = w.CoMy();
   
        for (double t = 0.0; t <= 60; t += StepSize){
            

            double xtp = xt; 
            double ytp = yt;
            xt = w.CoMx(); yt = w.CoMy();

            double vel = sqrt(pow(xt-xtp,2)+pow(yt-ytp,2))/StepSize;

            w.Step(StepSize);
            w.writeDataCheck();
            //w.DumpBodyState(bodyfile, skip_steps);
            //w.DumpActState(actfile, skip_steps);
            //w.DumpCurvature(curvfile, skip_steps);
            w.DumpVal("sts_velocity", vel);
        }

        
        //bodyfile.close();
        //actfile.close();
        //curvfile.close();
       // velfile.close();

}


