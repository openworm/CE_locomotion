#include "Evolution21.h"
#include <math.h>
#include "Worm21.h"
#include "Segment21.h"

void Evolution21::writeJson(TVector<double> &v) {Worm21 w(v); writeJson1(w);}

void Evolution21::addExtraParsToJson(json & j)
{   
    
    doubIntParamsHead var1;
    var1.parDoub.head = "Evolutionary Optimization Parameters";
       //var1.parInt.head = "Evolutionary Optimization Parameters";
       var1.parDoub.names = {"OSCT", "agarfreq", "BiasRange", "SCRange", "CSRange",
        "TauMin", "TauMax", "ESRange", "NMJmax", "IIRange", "AvgSpeed", "BBCfit"
    };
       var1.parDoub.vals = {OSCT, agarfreq, BiasRange, SCRange, CSRange,
        TauMin, TauMax, ESRange, NMJmax, IIRange, AvgSpeed, BBCfit
    };

       //var1.parInt.names = {};
       //var1.parInt.vals = {};

    appendToJson<double>(j[var1.parDoub.head],var1.parDoub);
    //appendToJson<long>(j[var1.parInt.head],var1.parInt);
}

void Evolution21::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{
  // Bias
  for (int i = 1; i <= 7; i++){
    phen(i) = MapSearchParameter(gen(i), -BiasRange, BiasRange);
}
// Time Constant
for (int i = 8; i <= 14; i++){
    phen(i) = MapSearchParameter(gen(i), TauMin, TauMax);
}
// Self connections
for (int i = 15; i <= 21; i++){
    phen(i) = MapSearchParameter(gen(i), -SCRange, SCRange);
}
// Chemical synapses
for (int i = 22; i <=30; i++){
    phen(i) = MapSearchParameter(gen(i), -CSRange, CSRange);
}

// Gap junctions
phen(31) = MapSearchParameter(gen(31), 0.0, ESRange);

// Intersegment synapse tested
phen(40) = MapSearchParameter(gen(40), -CSRange, CSRange);  // DB to DDnext
phen(41) = MapSearchParameter(gen(41), -CSRange, CSRange);  // VAnext to DD
phen(42) = MapSearchParameter(gen(42), 0.0, ESRange);       // AS -- VAnext
phen(43) = MapSearchParameter(gen(43), 0.0, ESRange);       // DA -- ASnext
phen(44) = MapSearchParameter(gen(44), 0.0, ESRange);       // VB -- DBnext

// NMJ Weight
phen(32) = MapSearchParameter(gen(32), 0.0, NMJmax);       // AS
phen(33) = MapSearchParameter(gen(33), 0.0, NMJmax);       // DA
phen(34) = MapSearchParameter(gen(34), NMJmax, NMJmax);       // DB
phen(35) = MapSearchParameter(gen(35), -NMJmax, 0.0);      // DD
phen(36) = MapSearchParameter(gen(36), -NMJmax, 0.0);      // VD
phen(37) = MapSearchParameter(gen(37), NMJmax, NMJmax);      // VB
phen(38) = MapSearchParameter(gen(38), 0.0, NMJmax);      // VA

phen(39) = MapSearchParameter(gen(39), 0.2, 1.0);       // Used to be 0.4/0.6 XXX NMJ_Gain Mapping

}

void Evolution21::RunSimulation(TVector<double> &v, RandomState &rs)
{
    EvaluationFunction2Output(v,rs);
    return;
}




double Evolution21::EvaluationFunction(TVector<double> &v, RandomState &rs)
{return EvaluationFunction2(v,rs);}


int Evolution21::finish_Bosc(int Generation,double BestPerf,double AvgPerf,double PerfVar){
    if (BestPerf > 0.99) return 1;
    else return 0;
}

//////// Stage 1 ////////////
/////////////////////////////
double Evolution21::EvaluationFunction1(TVector<double> &v, RandomState &rs){

    const double & Duration = evoPars1.Duration;
    //const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;

    // Fitness variables
    double DBp, VBp, dDB, dVB;
    double oscDB = 0, oscVB = 0;
    double FoDB, FoVB, FfDB, FfVB;

    double freqDB=0, freqVB=0;
    int pDB = 0, pVB = 0, signtagDB, signtagVB, signDB, signVB;
    TVector<double> peaksDB(1, 2*Duration);
    TVector<double> peaksVB(1, 2*Duration);// longer vector if you want frequencies higer than 2 Hz.
    
    // Genotype-Phenotype Mapping
    TVector<double> phenotype(1, itsVectSize());
    GenPhenMapping(v, phenotype);
    
    Segment s(phenotype);
    s.InitializeState(rs);

    for (double t = 0.0; t <= 10; t += StepSize){
        s.Step(StepSize);
    }
    DBp = s.n.NeuronOutput(3);
    VBp = s.n.NeuronOutput(6);

    s.Step(StepSize); // determine sign of derivative

    dDB = s.n.NeuronOutput(3) - DBp;
    dVB = s.n.NeuronOutput(6) - VBp;
    signtagDB = (dDB  > 0) ? 1 : -1;
    signtagVB = (dVB  > 0) ? 1 : -1;
    DBp = s.n.NeuronOutput(3);
    VBp = s.n.NeuronOutput(6);

    // Time loop
    for (double t = 0.0; t <= Duration; t += StepSize) {
        // Step simulation
        s.Step(StepSize);
        // check changes in sign of derivative
        dDB = s.n.NeuronOutput(3) - DBp;
        dVB = s.n.NeuronOutput(6) - VBp;
        signDB = (dDB  > 0) ? 1 : ((dDB  < 0) ? -1 : 0);
        signVB = (dVB  > 0) ? 1 : ((dVB  < 0) ? -1 : 0);

        oscDB += abs(DBp - s.n.NeuronOutput(3));
        oscVB += abs(VBp - s.n.NeuronOutput(6));

        if ((signDB == -1) and (signtagDB >= 0)){
            pDB +=1;
            peaksDB[pDB] = t;
            if (pDB >= 2*Duration){return 0;};
        }
        if ((signVB == -1) and (signtagVB >= 0)){
            pVB +=1;
            peaksVB[pVB] = t;
            if (pVB >= 2*Duration){return 0;};
        }

        signtagDB = signDB;
        signtagVB = signVB;
        DBp = s.n.NeuronOutput(3);
        VBp = s.n.NeuronOutput(6);
    }
    if ((pDB < 2) or (pVB < 2)){return 0;};
    for (int i = 1; i<pDB; i+=1){freqDB += (1./(pDB-1))*(1./(peaksDB[i+1]- peaksDB[i]));} 
    for (int i = 1; i<pVB; i+=1){freqVB += (1./(pVB-1))*(1./(peaksVB[i+1]- peaksVB[i]));} 

    FfDB = fabs(freqDB - agarfreq)/agarfreq < 1 ? fabs(freqDB - agarfreq)/agarfreq : 1;
    FfVB = fabs(freqVB - agarfreq)/agarfreq < 1 ? fabs(freqVB - agarfreq)/agarfreq : 1;

    FoDB = oscDB > OSCT ? 1 : oscDB / OSCT;
    FoVB = oscVB > OSCT ? 1 : oscVB / OSCT;

    return FoDB * FoVB * (1 - FfDB) * (1 - FfVB);
}


//////// Stage 2 ////////////
/////////////////////////////
double Evolution21::EvaluationFunction2Output(TVector<double> &v, RandomState &rs){

    const double & Duration = evoPars1.Duration;
    //const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;
    const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = evoPars1.Transient;
    const int & skip_steps = evoPars1.skip_steps;

    //ofstream bodyfile, actfile, curvfile;
    ofstream paramsfile;

    //bodyfile.open(rename_file("body.dat"));
    //actfile.open(rename_file("act.dat"));
    //curvfile.open(rename_file("curv.dat"));
    paramsfile.open(rename_file("params.dat"));

    // Fitness
    double fitness_tr = 0.0;
    double bodyorientation, anglediff;
    double movementorientation, distancetravelled = 0, displacement, temp;
    TVector<double> curvature(1, N_curvs);
    TVector<double> antpostcurv(1, 2);
    antpostcurv.FillContents(0.0);

    // Evaluation of B-class neuron oscillation,and frequency in segment 2.
    // The index of B class in this segment correspond to DBs2 = 10; VBs2 = 13
    double DBp, VBp, dDB, dVB;
    double oscDB = 0, oscVB = 0;
    double FoDB, FoVB, FfDB, FfVB;

    double freqDB=0, freqVB=0;
    int pDB = 0, pVB = 0, signtagDB, signtagVB, signDB, signVB;
    TVector<double> peaksDB(1, 2*Duration);
    TVector<double> peaksVB(1, 2*Duration);// longer vector if you want frequencies higer than 2 Hz.

    
    // Genotype-Phenotype Mapping
    TVector<double> phenotype(1, itsVectSize());
    GenPhenMapping(v, phenotype);
    
    Worm21 w(phenotype);
    w.setBasename(itsEvoPars().directoryName);
    w.setDataskips(itsEvoPars().skip_steps);
    w.dataReset();
    
    w.DumpParams(paramsfile);
    

    
    w.InitializeState(rs);
    
    // Transient XXX
    w.SetAVB(0.0);
    w.SetAVA(0.0);
    
    for (double t = 0.0; t <= Transient; t += StepSize){
        w.Step(StepSize);
    }    
    
    DBp = w.n.NeuronOutput(10);
    VBp = w.n.NeuronOutput(13);

    w.Step(StepSize); // determine sign of derivative

    dDB = w.n.NeuronOutput(10) - DBp;
    dVB = w.n.NeuronOutput(13) - VBp;
    signtagDB = (dDB  > 0) ? 1 : -1;
    signtagVB = (dVB  > 0) ? 1 : -1;
    DBp = w.n.NeuronOutput(10);
    VBp = w.n.NeuronOutput(13);
    
    double xt = w.CoMx(), xtp;
    double yt = w.CoMy(), ytp;

    // Time loop
    for (double t = 0.0; t <= Duration; t += StepSize) {
        // Step simulation
        w.Step(StepSize);
        
        ///// Oscilation
        // check changes in sign of derivative
        dDB = w.n.NeuronOutput(10) - DBp;
        dVB = w.n.NeuronOutput(13) - VBp;
        signDB = (dDB  > 0) ? 1 : ((dDB  < 0) ? -1 : 0);
        signVB = (dVB  > 0) ? 1 : ((dVB  < 0) ? -1 : 0);

        oscDB += abs(DBp - w.n.NeuronOutput(10));
        oscVB += abs(VBp - w.n.NeuronOutput(13));

        if ((signDB == -1) and (signtagDB >= 0)){
            pDB +=1;
            peaksDB[pDB] = t;
            if (pDB >= 2*Duration){return 0;};
        }
        if ((signVB == -1) and (signtagVB >= 0)){
            pVB +=1;
            peaksVB[pVB] = t;
            if (pVB >= 2*Duration){return 0;};
        }

        signtagDB = signDB;
        signtagVB = signVB;
        DBp = w.n.NeuronOutput(10);
        VBp = w.n.NeuronOutput(13);
        
        //// Locomotion
        // Current and past centroid position
        xtp = xt; ytp = yt;
        xt = w.CoMx(); yt = w.CoMy();
        
        // Integration error check
        if (isnan(xt) || isnan(yt) || sqrt(pow(xt-xtp,2)+pow(yt-ytp,2)) > 100*AvgSpeed*StepSize){
            return 0.0;
        }
        
        // Fitness
        bodyorientation = w.Orientation();                  // Orientation of the body position
        movementorientation = atan2(yt-ytp,xt-xtp);         // Orientation of the movement
        anglediff = movementorientation - bodyorientation;  // Check how orientations align
        temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
        distancetravelled += temp * sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));

    }
    // B Oscillation evaluation
    if ((pDB < 2) or (pVB < 2)){return 0;};
    for (int i = 1; i<pDB; i+=1){freqDB += (1./(pDB-1))*(1./(peaksDB[i+1]- peaksDB[i]));} 
    for (int i = 1; i<pVB; i+=1){freqVB += (1./(pVB-1))*(1./(peaksVB[i+1]- peaksVB[i]));} 

    FfDB = fabs(freqDB - agarfreq)/agarfreq < 1 ? fabs(freqDB - agarfreq)/agarfreq : 1;
    FfVB = fabs(freqVB - agarfreq)/agarfreq < 1 ? fabs(freqVB - agarfreq)/agarfreq : 1;

    FoDB = oscDB > OSCT ? 1 : oscDB / OSCT;
    FoVB = oscVB > OSCT ? 1 : oscVB / OSCT;

    // Locomotion evaluation
    fitness_tr = (1 - (fabs(BBCfit-distancetravelled)/BBCfit));

//#ifdef OUTPUT

        for (double t = 0.0; t <= 60; t += StepSize){
            w.Step(StepSize);
            w.writeData();
            //w.DumpBodyState(bodyfile, skip_steps);
            //w.DumpActState(actfile, skip_steps);
            //w.DumpCurvature(curvfile, skip_steps);
            
        }

        cout << fitness_tr << endl;
        //cout << fitness_tr << " " << fitness_ds << endl; //removed fitness_ds since not defined
        //bodyfile.close();
        //actfile.close();
        //curvfile.close();
           
//#endif
    return fitness_tr * FoDB * FoVB * (1 - FfDB) * (1 - FfVB);
}

void Evolution21::RunSimulation(Worm2Dbase & w1, RandomState &rs){

    cout << "running evo21 sim" << endl;
  
    Worm2D21m & w = dynamic_cast<Worm2D21m&>(w1);

    const double & Duration = evoPars1.Duration;
    //const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;
    //const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = evoPars1.Transient;
    //const int & skip_steps = evoPars1.skip_steps;

    //ofstream bodyfile, actfile, curvfile, paramsfile, velfile, statefile;
    ofstream paramsfile;

    //bodyfile.open(rename_file("body.dat"));
    //actfile.open(rename_file("act.dat"));
    //curvfile.open(rename_file("curv.dat"));
    paramsfile.open(rename_file("params.dat"));
    //statefile.open(rename_file("state.dat"));
    //velfile.open(rename_file("velocity.dat"));

    w.DumpParams(paramsfile);
    paramsfile.close();

    w.InitializeState(rs);
    
    // Transient XXX
    w.SetAVB(0.0);
    w.SetAVA(0.0);
    
    if (Transient!=0){
    for (double t = 0.0; t <= Transient; t += StepSize){
        w.Step(StepSize);
    }    

    w.Step(StepSize); // determine sign of derivative
    }
        
    if (Duration!=0){
    // Time loop
    for (double t = 0.0; t <= Duration; t += StepSize) {
        // Step simulation
        w.Step(StepSize); 

    }
}
        //double xt = w.CoMx();
        //double yt = w.CoMy();
   
        
     /*    if (Transient == 0)
            {
            w.DumpBodyState(bodyfile, skip_steps);
            w.DumpActState(actfile, skip_steps);
            w.DumpActStateState(statefile, skip_steps);
            w.DumpCurvature(curvfile, skip_steps);

            } */

        

        for (double t = 0.0; t <= 60; t += StepSize){
            
           /*  double xtp = xt; 
            double ytp = yt;
            xt = w.CoMx(); yt = w.CoMy();

            double vel = sqrt(pow(xt-xtp,2)+pow(yt-ytp,2))/StepSize; */

            w.Step(StepSize);
            w.writeDataCheck();
            //w.DumpBodyState(bodyfile, skip_steps);
            //w.DumpActState(actfile, skip_steps);
            //w.DumpActStateState(statefile, skip_steps);
            //w.DumpCurvature(curvfile, skip_steps);
          
            //w.DumpVal(velfile, skip_steps, vel);
        }

        //statefile.close();
        //bodyfile.close();
        //actfile.close();
        //curvfile.close();
        //velfile.close();

}



double Evolution21::EvaluationFunction2(TVector<double> &v, RandomState &rs){

    const double & Duration = evoPars1.Duration;
    //const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;
    const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = evoPars1.Transient;
    const int & skip_steps = evoPars1.skip_steps;

    
        // Fitness
        double fitness_tr = 0.0;
        double bodyorientation, anglediff;
        double movementorientation, distancetravelled = 0, displacement, temp;
        TVector<double> curvature(1, N_curvs);
        TVector<double> antpostcurv(1, 2);
        antpostcurv.FillContents(0.0);
    
        // Evaluation of B-class neuron oscillation,and frequency in segment 2.
        // The index of B class in this segment correspond to DBs2 = 10; VBs2 = 13
        double DBp, VBp, dDB, dVB;
        double oscDB = 0, oscVB = 0;
        double FoDB, FoVB, FfDB, FfVB;
    
        double freqDB=0, freqVB=0;
        int pDB = 0, pVB = 0, signtagDB, signtagVB, signDB, signVB;
        TVector<double> peaksDB(1, 2*Duration);
        TVector<double> peaksVB(1, 2*Duration);// longer vector if you want frequencies higer than 2 Hz.
    
        
        // Genotype-Phenotype Mapping
        TVector<double> phenotype(1, itsVectSize());
        GenPhenMapping(v, phenotype);
        
        Worm21 w(phenotype);
        
        w.InitializeState(rs);
        
        // Transient XXX
        w.SetAVB(0.0);
        w.SetAVA(0.0);
        
        for (double t = 0.0; t <= Transient; t += StepSize){
            w.Step(StepSize);
        }    
        
        DBp = w.n.NeuronOutput(10);
        VBp = w.n.NeuronOutput(13);
    
        w.Step(StepSize); // determine sign of derivative
    
        dDB = w.n.NeuronOutput(10) - DBp;
        dVB = w.n.NeuronOutput(13) - VBp;
        signtagDB = (dDB  > 0) ? 1 : -1;
        signtagVB = (dVB  > 0) ? 1 : -1;
        DBp = w.n.NeuronOutput(10);
        VBp = w.n.NeuronOutput(13);
        
        double xt = w.CoMx(), xtp;
        double yt = w.CoMy(), ytp;
    
        // Time loop
        for (double t = 0.0; t <= Duration; t += StepSize) {
            // Step simulation
            w.Step(StepSize);
            
            ///// Oscilation
            // check changes in sign of derivative
            dDB = w.n.NeuronOutput(10) - DBp;
            dVB = w.n.NeuronOutput(13) - VBp;
            signDB = (dDB  > 0) ? 1 : ((dDB  < 0) ? -1 : 0);
            signVB = (dVB  > 0) ? 1 : ((dVB  < 0) ? -1 : 0);
    
            oscDB += abs(DBp - w.n.NeuronOutput(10));
            oscVB += abs(VBp - w.n.NeuronOutput(13));
    
            if ((signDB == -1) and (signtagDB >= 0)){
                pDB +=1;
                peaksDB[pDB] = t;
                if (pDB >= 2*Duration){return 0;};
            }
            if ((signVB == -1) and (signtagVB >= 0)){
                pVB +=1;
                peaksVB[pVB] = t;
                if (pVB >= 2*Duration){return 0;};
            }
    
            signtagDB = signDB;
            signtagVB = signVB;
            DBp = w.n.NeuronOutput(10);
            VBp = w.n.NeuronOutput(13);
            
            //// Locomotion
            // Current and past centroid position
            xtp = xt; ytp = yt;
            xt = w.CoMx(); yt = w.CoMy();
            
            // Integration error check
            if (isnan(xt) || isnan(yt) || sqrt(pow(xt-xtp,2)+pow(yt-ytp,2)) > 100*AvgSpeed*StepSize){
                return 0.0;
            }
            
            // Fitness
            bodyorientation = w.Orientation();                  // Orientation of the body position
            movementorientation = atan2(yt-ytp,xt-xtp);         // Orientation of the movement
            anglediff = movementorientation - bodyorientation;  // Check how orientations align
            temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
            distancetravelled += temp * sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));
    
        }
        // B Oscillation evaluation
        if ((pDB < 2) or (pVB < 2)){return 0;};
        for (int i = 1; i<pDB; i+=1){freqDB += (1./(pDB-1))*(1./(peaksDB[i+1]- peaksDB[i]));} 
        for (int i = 1; i<pVB; i+=1){freqVB += (1./(pVB-1))*(1./(peaksVB[i+1]- peaksVB[i]));} 
    
        FfDB = fabs(freqDB - agarfreq)/agarfreq < 1 ? fabs(freqDB - agarfreq)/agarfreq : 1;
        FfVB = fabs(freqVB - agarfreq)/agarfreq < 1 ? fabs(freqVB - agarfreq)/agarfreq : 1;
    
        FoDB = oscDB > OSCT ? 1 : oscDB / OSCT;
        FoVB = oscVB > OSCT ? 1 : oscVB / OSCT;
    
        // Locomotion evaluation
        fitness_tr = (1 - (fabs(BBCfit-distancetravelled)/BBCfit));
    
   
        return fitness_tr * FoDB * FoVB * (1 - FfDB) * (1 - FfVB);
    }



    
void Evolution21::configure_p2()
{
    
    // Stage 1 //
    {typedef int (*callback_t)(int, double, double, double);
        Callback<int(int, double, double, double)>::func 
        = std::bind(&Evolution21::finish_Bosc, this, 
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        callback_t func = static_cast<callback_t>(Callback<int(int, double, double, double)>::callback); 
        s->SetSearchTerminationFunction(func);}

    //s->SetSearchTerminationFunction(finish_Bosc);

    {typedef double (*callback_t)(TVector<double> &, RandomState &);
        Callback<double(TVector<double> &, RandomState &)>::func = std::bind(&Evolution21::EvaluationFunction1, this, 
                std::placeholders::_1, std::placeholders::_2);
        callback_t func = static_cast<callback_t>(Callback<double(TVector<double> &, RandomState &)>::callback);
    s->SetEvaluationFunction(func);}

    //s->SetEvaluationFunction(EvaluationFunction1);
  

    s->ExecuteSearch();

    // Stage 2 //
    s->SetSearchTerminationFunction(NULL);

    {typedef double (*callback_t)(TVector<double> &, RandomState &);
        Callback<double(TVector<double> &, RandomState &)>::func = std::bind(&Evolution21::EvaluationFunction2, this, 
                std::placeholders::_1, std::placeholders::_2);
        callback_t func = static_cast<callback_t>(Callback<double(TVector<double> &, RandomState &)>::callback);
    s->SetEvaluationFunction(func);}

    //s->SetEvaluationFunction(EvaluationFunction2);
    InitializeBodyConstants();
    s->ExecuteSearch();
}
