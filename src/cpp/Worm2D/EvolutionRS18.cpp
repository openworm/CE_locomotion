#include "EvolutionRS18.h"
#include <math.h>
#include "WormRS18.h"


void EvolutionRS18::writeJson(TVector<double> &v) {Worm18 w(v); writeJson1(w);}

void EvolutionRS18::addExtraParsToJson(json & j)
{
  
    doubIntParamsHead var1;
    var1.parDoub.head = "Evolutionary Optimization Parameters";
       var1.parInt.head = "Evolutionary Optimization Parameters";
       var1.parDoub.names = {"fps", "BiasRange", "SCRange", "CSRange", "TauMin",
        "TauMax", "ESRange", "SRmax", "NMJmax", "HCSRange", "AvgSpeed", "BBCfit", 
    };
       var1.parDoub.vals = {fps, BiasRange, SCRange, CSRange, TauMin,
        TauMax, ESRange, SRmax, NMJmax, HCSRange, AvgSpeed, BBCfit, 
    };

       var1.parInt.names = {"skip", "speedoutput", "evo_seed"};
       var1.parInt.vals = {skip, speedoutput, evo_seed};

    appendToJson<double>(j[var1.parDoub.head],var1.parDoub);
    appendToJson<long>(j[var1.parInt.head],var1.parInt);
}

void EvolutionRS18::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{
    // --------------------------------
    // Parameters for the Ventral Nerve Cord Unit
    // --------------------------------
    // Bias
    phen(1) = MapSearchParameter(gen(1), -BiasRange, BiasRange);        // DB, VBa, VBp
    phen(2) = MapSearchParameter(gen(2), -BiasRange, BiasRange);        // DD, VDa, VDp

    // Time Constant
    phen(3) = MapSearchParameter(gen(3), TauMin, TauMax);               // DB, VBa, VBp
    phen(4) = MapSearchParameter(gen(4), TauMin, TauMax);               // DD, VDa, VDp

    // Self connections
    phen(5) = MapSearchParameter(gen(5), -SCRange, SCRange);            // DB, VBa, VBp
    phen(6) = MapSearchParameter(gen(6), -SCRange, SCRange);            // DD, VDa, VDp

    // Chemical synapses
    phen(7) = MapSearchParameter(gen(7), -CSRange, CSRange);            // DB -> DD, VBa -> VDa, VBp -> VDp

    phen(8) = MapSearchParameter(gen(8), -CSRange, CSRange);          // DB -> VDa, DB -> VDp, VBa -> DD /2, VBp -> DD /2

    phen(9) = MapSearchParameter(gen(9), -CSRange, CSRange);          // DD -> VDa

    // Gap junctions across class within unit
    phen(10) = MapSearchParameter(gen(10), 0.0, ESRange);      // DD - VDa, DD - VDp

    // Gap junctions per class
    phen(11) = MapSearchParameter(gen(11), 0.0, ESRange);      // VD - VD, DD - DD
    phen(12) = MapSearchParameter(gen(12), 0.0, ESRange);      // VB - VB, DB - DB

    // Gap junctions across class, across neural unit
    phen(13) = MapSearchParameter(gen(13), 0.0, ESRange);      // VB -> DB+1

    // Stretch receptor
    phen(14) = MapSearchParameter(gen(14), -SRmax, 0.0);        // B- class SR weight

    // NMJ Weight
    phen(15) = MapSearchParameter(gen(15), 0.0, NMJmax);       // DB, VBa, VBp
    phen(16) = MapSearchParameter(gen(16), -NMJmax, 0.0);      // DD, VDa, VDp

    // --------------------------------
    // Parameters for the Head circuit
    // --------------------------------
    // Bias
    phen(17) = MapSearchParameter(gen(17), -BiasRange, BiasRange);    // SMDD, SMDV
    phen(18) = MapSearchParameter(gen(18), -BiasRange, BiasRange);    // RMDD, RMDV

    // Time Constant
    phen(19) = MapSearchParameter(gen(19), TauMin, TauMax);           // SMDD, SMDV
    phen(20) = MapSearchParameter(gen(20), TauMin, TauMax);           // RMDD, RMDV

    // Self connections
    phen(21) = MapSearchParameter(gen(21), -SCRange, SCRange);      // SMDD, SMDV
    phen(22) = MapSearchParameter(gen(22), 4.0, SCRange);           // RMDD, RMDV

    // Chemical synapses
    phen(23) = MapSearchParameter(gen(23), -HCSRange, HCSRange);      // SMDD -> SMDV, SMDV -> SMDD
    phen(24) = MapSearchParameter(gen(24), -HCSRange, HCSRange);      // SMDD -> RMDV, SMDV -> RMDD
    phen(25) = MapSearchParameter(gen(25), -HCSRange, HCSRange);      // RMDD -> RMDV, RMDV -> RMDD

    // Gap junctions across class within unit
    phen(26) = MapSearchParameter(gen(26), 0.0, ESRange);      // SMDD - RMDD, SMDV - RMDV
    phen(27) = MapSearchParameter(gen(27), 0.0, ESRange);      // RMDV - RMDD

    // SMD Stretch Receptor
    phen(28) = MapSearchParameter(gen(28), -SRmax, 0.0);        // SMD- class SR weight

    // NMJ Weight
    phen(29) = MapSearchParameter(gen(29), 0.0, NMJmax);    // SMDD, SMDV
    phen(30) = MapSearchParameter(gen(30), 0.0, NMJmax);    // RMDD, RMDV
}

double EvolutionRS18::EvaluationFunction(TVector<double> &v, RandomState &rs)
{return EvaluationFunctionNoOut(v,rs);}


void EvolutionRS18::RunSimulation(TVector<double> &v, RandomState &rs)
{
    EvaluationFunctionOrig(v,rs);
    return;
}


double EvolutionRS18::EvaluationFunctionNoOut(TVector<double> &v, RandomState &rs)
{
    double fitness;
    //ofstream fitfile;
  
    const double & Duration = evoPars1.Duration;
    //const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;
    const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = evoPars1.Transient;
    //const int & skip_steps = evoPars1.skip_steps;
   

    // Fitness
    fitness = 0.0;
    double bodyorientation, anglediff;
    double movementorientation, distancetravelled = 0, temp;
    TVector<double> curvature(1, N_curvs);
    TVector<double> antpostcurv(1, 2);
    antpostcurv.FillContents(0.0);

    // Genotype-Phenotype Mapping
    TVector<double> phenotype(1,  itsVectSize());
    GenPhenMapping(v, phenotype);
   

    Worm18 w(phenotype, 0);
  

    w.InitializeState(rs);
   
   
    w.setRs18output(1);

    // Transient
    for (double t = 0.0; t <= Transient; t += StepSize)
    {
        w.Step(StepSize);

    }

    double xt = w.CoMx(), xtp;
    double yt = w.CoMy(), ytp;

    // Time loop
    for (double t = 0.0; t <= Duration; t += StepSize) {

        w.Step(StepSize);

        // Current and past centroid position
        xtp = xt; ytp = yt;
        xt = w.CoMx(); yt = w.CoMy();

        // Integration error check
        if (isnan(xt) || isnan(yt) || sqrt(pow(xt-xtp,2)+pow(yt-ytp,2)) > 100*AvgSpeed*StepSize)
        {
            return 0.0;
        }

        // Fitness
        bodyorientation = w.Orientation();                  // Orientation of the body position
        movementorientation = atan2(yt-ytp,xt-xtp);         // Orientation of the movement
        anglediff = movementorientation - bodyorientation;  // Check how orientations align
        temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
        distancetravelled += temp * sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));



    }
    fitness = 1 - (fabs(BBCfit-distancetravelled)/BBCfit);


    return fitness;
}

double EvolutionRS18::EvaluationFunctionOrig(TVector<double> &v, RandomState &rs)
{
    double fitness;
    ofstream fitfile;
  
    const double & Duration = evoPars1.Duration;
    //const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;
    const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = evoPars1.Transient;


    if (speedoutput)
    {

    fitfile.open(rename_file("speed.dat"));

    }


    //ofstream bodyfile, actfile, curvfile, voltagefile, paramsfile;
    ofstream paramsfile;

    //bodyfile.open(rename_file("body.dat"));
    //actfile.open(rename_file("act.dat"));
    //curvfile.open(rename_file("curv.dat"));
    paramsfile.open(rename_file("params.dat"));



    // Fitness
    fitness = 0.0;
    double bodyorientation, anglediff;
    double movementorientation, distancetravelled = 0, temp;
    TVector<double> curvature(1, N_curvs);
    TVector<double> antpostcurv(1, 2);
    antpostcurv.FillContents(0.0);

    // Genotype-Phenotype Mapping
    TVector<double> phenotype(1, itsVectSize());
    GenPhenMapping(v, phenotype);

    Worm18 w(phenotype, 0);
    w.setRs18output(1);
    //w.setBasename(itsEvoPars().directoryName);
    w.setDataskips(itsEvoPars().skip_steps);
    //w.dataReset();



    w.DumpParams(paramsfile);
    //writeParsToJson(w);



    w.InitializeState(rs);
    w.InitializeData(itsEvoPars().directoryName);


    // Transient
    for (double t = 0.0; t <= Transient; t += StepSize)
    {
        w.Step(StepSize);
        w.writeDataCheck();


        //w.Curvature(curvature);
        //curvfile << curvature << endl;
        //w.DumpBodyState(bodyfile, skip);
        //w.DumpActState(actfile, skip);
    

    }

    double xt = w.CoMx(), xtp;
    double yt = w.CoMy(), ytp;

    // Time loop
    for (double t = 0.0; t <= Duration; t += StepSize) {

        w.Step(StepSize);

        // Current and past centroid position
        xtp = xt; ytp = yt;
        xt = w.CoMx(); yt = w.CoMy();

        // Integration error check
        if (isnan(xt) || isnan(yt) || sqrt(pow(xt-xtp,2)+pow(yt-ytp,2)) > 100*AvgSpeed*StepSize)
        {
            return 0.0;
        }

        // Fitness
        bodyorientation = w.Orientation();                  // Orientation of the body position
        movementorientation = atan2(yt-ytp,xt-xtp);         // Orientation of the movement
        anglediff = movementorientation - bodyorientation;  // Check how orientations align
        temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
        distancetravelled += temp * sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));

        w.writeDataCheck();

        //w.Curvature(curvature);
        //curvfile << curvature << endl;
        //w.DumpBodyState(bodyfile, skip);
        //w.DumpActState(actfile, skip);


    }
    fitness = 1 - (fabs(BBCfit-distancetravelled)/BBCfit);



    cout << fitness << " " << BBCfit << " " << distancetravelled << " " << distancetravelled/Duration << endl;
    //bodyfile.close();
    //actfile.close();
    //curvfile.close();


if (speedoutput){
    fitfile << fitness << " "<< BBCfit << " " << distancetravelled << " " << distancetravelled/Duration << " " << endl;
    fitfile.close();
}

    return fitness;
}

void EvolutionRS18::RunSimulation(Worm2Dbase &w1, RandomState &rs)
{
    Worm18 & w = dynamic_cast<Worm18&>(w1);
    double fitness;
    ofstream fitfile;
    
    w.setRs18output(1);

    const double & Duration = simPars1.Duration;
  
    const double & StepSize = evoPars1.StepSize;
    const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = simPars1.Transient;


    if (speedoutput)
    {

    fitfile.open(rename_file("speed.dat"));

    }


//ofstream bodyfile, actfile, curvfile, voltagefile, paramsfile;
ofstream paramsfile;

    //bodyfile.open(rename_file("body.dat"));
    //actfile.open(rename_file("act.dat"));
    //curvfile.open(rename_file("curv.dat"));
    paramsfile.open(rename_file("params.dat"));



    // Fitness
    fitness = 0.0;
    double bodyorientation, anglediff;
    double movementorientation, distancetravelled = 0, temp;
    TVector<double> curvature(1, N_curvs);
    TVector<double> antpostcurv(1, 2);
    antpostcurv.FillContents(0.0);

    // Genotype-Phenotype Mapping
    
    



    w.DumpParams(paramsfile);
    //writeParsToJson(w);
   


    w.InitializeState(rs);



    // Transient
    for (double t = 0.0; t <= Transient; t += StepSize)
    {
        w.Step(StepSize);
        w.writeDataCheck();


        //w.Curvature(curvature);
        //curvfile << curvature << endl;
        //w.DumpBodyState(bodyfile, skip);
        //w.DumpActState(actfile, skip);
    

    }

    double xt = w.CoMx(), xtp;
    double yt = w.CoMy(), ytp;

    // Time loop
    for (double t = 0.0; t <= Duration; t += StepSize) {

        w.Step(StepSize);

        // Current and past centroid position
        xtp = xt; ytp = yt;
        xt = w.CoMx(); yt = w.CoMy();

       
        // Fitness
        bodyorientation = w.Orientation();                  // Orientation of the body position
        movementorientation = atan2(yt-ytp,xt-xtp);         // Orientation of the movement
        anglediff = movementorientation - bodyorientation;  // Check how orientations align
        temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
        distancetravelled += temp * sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));


        w.writeDataCheck();
        //w.Curvature(curvature);
        //curvfile << curvature << endl;
        //w.DumpBodyState(bodyfile, skip);
        //w.DumpActState(actfile, skip);


    }
    fitness = 1 - (fabs(BBCfit-distancetravelled)/BBCfit);



    cout << fitness << " " << BBCfit << " " << distancetravelled << " " << distancetravelled/Duration << endl;
    //bodyfile.close();
    //actfile.close();
    //curvfile.close();


if (speedoutput){
    fitfile << fitness << " "<< BBCfit << " " << distancetravelled << " " << distancetravelled/Duration << " " << endl;
    fitfile.close();
}

    
}



void EvolutionRS18::configure_p12()
{
   // configure_p1();
    if (evo_seed)
    {
        ifstream BestIndividualFile;
        TVector<double> bestVector(1, itsVectSize());
        BestIndividualFile.open(rename_file("best.gen.dat"));
        BestIndividualFile >> bestVector;
        s->InitializeSearch();
        for (int i = 1; i <= s->PopulationSize(); i++){
            for (int j = 1; j <= itsVectSize(); j++)
            {
                s->Individual(i)[j] = bestVector[j];
            }
        }
    }
   // configure_p2();

   
}