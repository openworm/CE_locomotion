#include "Evolution.h"
//#include <string>
#include <string.h>
#include <sys/stat.h>
//#include <stdio.h>
#include <iostream>


const char* getParameter(int argc, const char* argv[], string parName, const char* defaultval)
{    
   const char* retval = defaultval;
   if (((argc-1) % 2) != 0)
   {cout << "The arguments are not configured correctly." << endl;exit(1);}
   for (int arg = 1; arg<argc; arg+=2) 
   if (strcmp(argv[arg],parName.c_str())==0) {retval = argv[arg+1];break;}
   return retval;
}


string Evolution::rename_file(string filename){return evoPars1.directoryName + "/" + 
    evoPars1.fileprefix + filename;}


Evolution::Evolution(int argc, const char* argv[], evoPars ep1, int VectSize_)
    :evoPars1(setPars(argc,argv,ep1)),s(new TSearch(VectSize_)),
    simPars1(setSimPars(argc,argv)),writeBestFlag(true),phenotype(1, VectSize_),phenprev(1, VectSize_),
    genprev(1, VectSize_)
    {setFromCPT();}
  
Evolution::Evolution(int argc, const char* argv[], evoPars ep1, int VectSize_, string prefix_)
    :evoPars1(setPars(argc,argv,ep1,prefix_)),s(new TSearch(VectSize_)),
    simPars1(setSimPars(argc,argv)),writeBestFlag(true),phenotype(1, VectSize_),phenprev(1, VectSize_),
    genprev(1, VectSize_)
    {setFromCPT();}



void Evolution::checkPars()
{
    if (s->VectorSize() != evoPars1.VectSize){
         cout << "cpt vectorsize "  << s->VectorSize() << " evolution vectorsize " <<  evoPars1.VectSize << endl;
         assert(s->VectorSize() == evoPars1.VectSize && "Vectorsize is not correct");
    }

    if (s->PopulationSize()!= evoPars1.PopulationSize) 
    {cout << "setting " <<  " population size to cpt population size: " << s->PopulationSize() << endl;
    popsize = s->PopulationSize();}


}

void Evolution::setFromCPT()
{
    popsize = evoPars1.PopulationSize;
    s->cptfilename = rename_file("search.cpt");
    struct stat buffer;   
    if (evoPars1.CheckpointInterval>0 && (stat (s->cptfilename.c_str(), &buffer) == 0)) {
        s->ReadCheckpointFile();
        cout << "setFromCPT " << s->cptfilename << endl;
        doResume = true;
        //ResultsDisplay(*s);
        checkPars();


    }
    else doResume = false;
   
}


void Evolution::setUp()
{   
    if  (doResume) {
        fileDropLines<double>(rename_file("fitness.dat"), s->Generation(), 4);
        fileDropLines<double>(rename_file("genhistory.dat"), s->Generation(), s->VectorSize()*3 + 1);
        fileDropLines<double>(rename_file("gendiffhistory.dat"), s->Generation(), s->VectorSize()*2 + 1);
    }
    evolfile.open(rename_file("fitness.dat"), std::ios_base::app);
    evolfile << setprecision(10);
    //setFromCPT();
    genhistfile.open(rename_file("genhistory.dat"), std::ios_base::app);
    genhistfile2.open(rename_file("gendiffhistory.dat"), std::ios_base::app);
    doneFirst = false;
}

void Evolution::setFromEvol(const Evolution & er, int offset)
{
    if (doResume) return;

    cout << "setFromEvol original pop size " 
    << s->PopulationSize() << "loaded pop size " 
    << er.s->PopulationSize();

    s->InitializeSearch();
    int minsize = s->PopulationSize();
    if (er.s->PopulationSize() < minsize) 
        minsize = er.s->PopulationSize();


    //assert(s->PopulationSize() == er.s->PopulationSize());
    //assert(evoPars1.VectSize==er.evoPars1.VectSize + offset);

    cout << "using " << minsize;
    for (int i = 1; i <= minsize; i++) {
    for (int j = 1; j <= er.s->Individual(i).Size(); j++){
    s->Individual(i)(j+offset) = er.s->Individual(i)(j);
}

}
    
//doResume = true;

}



void Evolution::writeJson1(Worm2Dbase & w)
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
    json j;
    w.addParsToJson(j);   
    addParsToJson(j);
   
    json_out << std::setw(4) << j << std::endl;
    json_out.close(); 
  
}

void Evolution::addParsToJson(json & j)
{  
    
    doubIntParamsHead par1pars = evoPars1.getParams();
    appendToJson<double>(j[par1pars.parDoub.head],par1pars.parDoub);
    appendToJson<long>(j[par1pars.parInt.head],par1pars.parInt);

    addExtraParsToJson(j);
}

simPars Evolution::setSimPars(int argc, const char* argv[])
{

simPars sp1;
sp1.Duration = evoPars1.Duration;
sp1.Transient = evoPars1.Transient;

 if (((argc-1) % 2) != 0)
     {cout << "The arguments are not configured correctly." << endl;exit(1);}

 for (int arg = 1; arg<argc; arg+=2){
    if (strcmp(argv[arg],"-sd")==0) sp1.Duration = atoi(argv[arg+1]);
    if (strcmp(argv[arg],"-st")==0) sp1.Transient = atoi(argv[arg+1]);
}

return sp1;

}

evoPars Evolution::setPars(int argc, const char* argv[], evoPars ep1)
{
return setPars(argc,argv,ep1,"");
}
evoPars Evolution::setPars(int argc, const char* argv[], evoPars ep1, string prefix_){


    if (((argc-1) % 2) != 0)
     {cout << "The arguments are not configured correctly." << endl;exit(1);}
    
    bool seed_flag = 1;
    
    for (int arg = 1; arg<argc; arg+=2)
    { 
    if (strcmp(argv[arg],"--maxgens")==0) ep1.MaxGenerations = atoi(argv[arg+1]);
    //if (strcmp(argv[arg],"--doevol")==0) do_evol = atoi(argv[arg+1]);
    //if (strcmp(argv[arg],"--dorandinit")==0) simRandomInit = atoi(argv[arg+1]);
    //if (strcmp(argv[arg],"--skipOrigSim")==0) skipOrigSim = atoi(argv[arg+1]);
    //if (strcmp(argv[arg],"--donml")==0) do_nml = atoi(argv[arg+1]);


    if (strcmp(argv[arg],"--folder")==0) {
      ep1.directoryName= argv[arg+1];
      struct stat sb;
      if (stat(ep1.directoryName.c_str(), &sb) != 0) 
      {cout << "Directory doesn't exist." << endl;exit(1);}
    }

    if (seed_flag){ 
    if (strcmp(argv[arg],"-R")==0) ep1.randomseed = atoi(argv[arg+1]);
    if (strcmp(argv[arg],"-r")==0) 
    {long randomseed1 = static_cast<long>(time(NULL));
           ep1.randomseed = randomseed1 + atoi(argv[arg+1]);
    }
    seed_flag = 0;
    }
    //if (strcmp(argv[arg], "--modelname")==0) model_name = argv[arg+1];
    if (strcmp(argv[arg],"-p")==0) ep1.PopulationSize = atoi(argv[arg+1]);
    if (strcmp(argv[arg],"-d")==0) ep1.Duration = atoi(argv[arg+1]);
    if (strcmp(argv[arg],"-t")==0) ep1.Transient = atoi(argv[arg+1]);
    if (strcmp(argv[arg],"-cpt")==0) ep1.CheckpointInterval = atoi(argv[arg+1]);

    //if (strcmp(argv[arg],"--nervous")==0) nervousSystemNameForSim = argv[arg+1];
    
    }

    ep1.fileprefix = prefix_;

    //evoParsNC.filePrefix = "";

    return ep1;

}

void Evolution::EvolutionaryRunDisplay(int Generation, double BestPerf, double AvgPerf, double PerfVar)
{
    cout << "EvolutionaryRunDisplay" << endl;
    evolfile << Generation << " " << BestPerf << " " << AvgPerf << " " << PerfVar << endl;
    if (writeBestFlag) ResultsDisplay(*s);

    TVector<double> & phencur =  getBestPhenotype();
    TVector<double> & gencur =  getBestGenotype();

    genhistfile << Generation << " " << gencur << " " << phencur;

    if (doneFirst){
    genhistfile2 << Generation;

    {vector<double> val = TVectorRatio<double>(gencur, genprev);
    //const TVector<double> genrat = TVectorRatio<double>(gencur, genprev);
    for (int i=0;i<val.size();i++) genhistfile2 << " " << val[i];}
    {vector<double> val = TVectorRatio<double>(phencur, phenprev);
    //const TVector<double> genrat = TVectorRatio<double>(gencur, genprev);
    for (int i=0;i<val.size();i++) genhistfile2 << " " << val[i];}

    genhistfile2 << endl;
    }

    doneFirst = true;
    phenprev = phencur;
    genprev = gencur;

    TVector<double> avphen(1, evoPars1.VectSize);
    for (int j = 1; j <= avphen.Size(); j++) avphen(j)=0;

    for (int i = 1; i <= s->PopulationSize(); i++) {
        TVector<double> phenotype(1, evoPars1.VectSize);
        GenPhenMapping(s->Individual(i), phenotype);
        for (int j = 1; j <= phenotype.Size(); j++) 
        avphen(j) +=  phenotype(j);    
    }
    for (int j = 1; j <= avphen.Size(); j++) avphen(j)= avphen(j)/s->PopulationSize();
    
    genhistfile << " " << avphen << endl;

    cout << phencur.Size() << " " << phencur << endl;
    writeJson(phencur);
  

}

TVector<double> & Evolution::getBestPhenotype()
{

//TVector<double> phenotype(1, itsEvoPars().VectSize);   
TVector<double> bestVector = s->BestIndividual();
GenPhenMapping(bestVector, phenotype);
return phenotype;

}

TVector<double> & Evolution::getBestGenotype()
{
    return s->BestIndividual();
}



void Evolution::ResultsDisplay(TSearch &s)
{
    TVector<double> bestVector;
    ofstream BestIndividualFile;

    bestVector = s.BestIndividual();
    BestIndividualFile.open(rename_file("best.gen.dat"));
    //BestIndividualFile.open(bestfilename);
    BestIndividualFile << setprecision(32);
    BestIndividualFile << bestVector << endl;
    BestIndividualFile.close();
}

void Evolution::configure_p1()
{
    
    s->SetRandomSeed(evoPars1.randomseed);

    {typedef void (*callback_t)(int, double, double, double);
    Callback<void(int, double, double, double)>::func 
    = std::bind(&Evolution::EvolutionaryRunDisplay, this, 
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    callback_t func = static_cast<callback_t>(Callback<void(int, double, double, double)>::callback); 
    s->SetPopulationStatisticsDisplayFunction(func);
    }

    {typedef void (*callback_t)(TSearch&);
    Callback<void(TSearch&)>::func = std::bind(&Evolution::ResultsDisplay, this, std::placeholders::_1);
    callback_t func = static_cast<callback_t>(Callback<void(TSearch&)>::callback); 
    s->SetSearchResultsDisplayFunction(func);
    }


    s->SetSelectionMode(evoPars1.SelectionMode);             //{FITNESS_PROPORTIONATE,RANK_BASED}
    s->SetReproductionMode(evoPars1.ReproductionMode);	// {HILL_CLIMBING, GENETIC_ALGORITHM}
    s->SetPopulationSize(popsize); //96
    s->SetMaxGenerations(evoPars1.MaxGenerations); //1000
    s->SetMutationVariance(evoPars1.MutationVariance);                // For 71 parameters, an estimated avg change of 0.25 for weights (mapped to 15).
    s->SetCrossoverProbability(evoPars1.CrossoverProbability);
    s->SetCrossoverMode(evoPars1.CrossoverMode);              //{UNIFORM, TWO_POINT}
    s->SetMaxExpectedOffspring(evoPars1.MaxExpectedOffspring);
    s->SetElitistFraction(evoPars1.ElitistFraction);
    s->SetSearchConstraint(evoPars1.SearchConstraint);
    s->SetCheckpointInterval(evoPars1.CheckpointInterval);
    s->SetReEvaluationFlag(evoPars1.ReEvaluationFlag);

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
    genhistfile2.close();
}

   
void Evolution::RunStandardSimulation(Worm2Dm & w, RandomState &rs){

    
    //Worm2D21 & w = dynamic_cast<Worm2D21&>(w1);

    const double & Duration = evoPars1.Duration;
    const int & VectSize = evoPars1.VectSize;
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


