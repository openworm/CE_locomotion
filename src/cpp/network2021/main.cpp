// =============================================================
// Evolution of Integrated Neuromechanical Forward Locomotion
// Eduardo Izquierdo
// Indiana University
// February, 2015
// =============================================================
#include <iostream>
#include <iomanip>  // cout precision
#include <math.h>
#include "TSearch.h"
#include "VectorMatrix.h"
#include "Worm.h"
#include "Segment.h"
#include "../argUtils.h"


#define EVOLVE
#define PRINTTOFILE
//#define OUTPUT

SuppliedArgs2021 supArgs1;

int skip_steps = 10;

using namespace std;

// Integration parameters
const double Duration = 40.0;       //
const double Transient = 10.0;       //
const double StepSize = 0.005;
const int N_curvs = 23;             // Number of cuvature points

double OSCT = 0.25 * Duration; // Cap for oscillation evaluation
const double agarfreq = 0.44;

// Genotype -> Phenotype Mapping (Ventral cord)
const double	BiasRange				= 15.0;
const double    SCRange                 = 15.0;
const double    CSRange                 = 15.0;
const double    TauMin                 = 0.1;
const double    TauMax                 = 2.5;
const double    ESRange                 = 2.0;
const double    NMJmax                  = 1.2;
const double    IIRange                 = 15.0;

// Fitness
const double    AvgSpeed = 0.00022;             // Average speed of the worm in meters per seconds
const double    BBCfit = AvgSpeed*Duration;

// Size of genotype (VC)
int	VectSize = 44;

// ------------------------------------
// Genotype-Phenotype Mapping
// ------------------------------------
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
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

void curvRatio(TVector<double> &v, TVector<double> &antposcurv){
    for (int i = 1; i <= N_curvs; i++){
        if (i <= 11)
            antposcurv(1) += fabs(v(i));
        else
            antposcurv(2) += fabs(v(i));
    }
}
//////// Stage 1 ////////////
/////////////////////////////
double EvaluationFunction1(TVector<double> &v, RandomState &rs){
    // Fitness variables
    double DBp, VBp, dDB, dVB;
    double oscDB = 0, oscVB = 0;
    double FoDB, FoVB, FfDB, FfVB;

    double freqDB=0, freqVB=0;
    int pDB = 0, pVB = 0, signtagDB, signtagVB, signDB, signVB;
    TVector<double> peaksDB(1, 2*Duration);
    TVector<double> peaksVB(1, 2*Duration);// longer vector if you want frequencies higer than 2 Hz.
    
    // Genotype-Phenotype Mapping
    TVector<double> phenotype(1, VectSize);
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
double EvaluationFunction2(TVector<double> &v, RandomState &rs){

/* #ifdef OUTPUT
    ofstream bodyfile, actfile, curvfile, paramsfile;
    bodyfile.open("body.dat");
    actfile.open("act.dat");
    curvfile.open("curv.dat");
    paramsfile.open("params.dat");
#endif */

ofstream bodyfile, actfile, curvfile, paramsfile;
if (supArgs1.output){
    bodyfile.open(supArgs1.rename_file("body.dat"));
    actfile.open(supArgs1.rename_file("act.dat"));
    curvfile.open(supArgs1.rename_file("curv.dat"));
    paramsfile.open(supArgs1.rename_file("params.dat"));
}
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
    TVector<double> phenotype(1, VectSize);
    GenPhenMapping(v, phenotype);
    
    Worm w(phenotype);
    
    if (supArgs1.output){
    w.DumpParams(paramsfile);

    }

/* #ifdef OUTPUT
    w.DumpParams(paramsfile);
#endif */
    
    w.InitializeState(rs);
    
    ofstream nsdump(supArgs1.rename_file("NSdump.dat"));
    nsdump << w.n;
    nsdump.close();

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
if (supArgs1.output){
    cout << "doout" << endl;
        for (double t = 0.0; t <= 60; t += StepSize){
            w.Step(StepSize);
            w.DumpBodyState(bodyfile, skip_steps);
            w.DumpActState(actfile, skip_steps);
            w.DumpCurvature(curvfile, skip_steps);
        }

        cout << fitness_tr << endl;
        //cout << fitness_tr << " " << fitness_ds << endl; //removed fitness_ds since not defined
        bodyfile.close();
        actfile.close();
        curvfile.close();
    }        
//#endif
    return fitness_tr * FoDB * FoVB * (1 - FfDB) * (1 - FfVB);
}

// ------------------------------------
// Display functions
// ------------------------------------
void EvolutionaryRunDisplay(int Generation, double BestPerf, double AvgPerf, double PerfVar){
    cout << Generation << " " << BestPerf << " " << AvgPerf << " " << PerfVar << endl;
}

void ResultsDisplay(TSearch &s){
    TVector<double> bestVector;
    ofstream BestIndividualFile;
    
    bestVector = s.BestIndividual();
    BestIndividualFile.open(supArgs1.rename_file("best.gen.dat"));
    BestIndividualFile << setprecision(32);
    BestIndividualFile << bestVector << endl;
    BestIndividualFile.close();
}

int finish_Bosc(int Generation,double BestPerf,double AvgPerf,double PerfVar){
    if (BestPerf > 0.99) return 1;
    else return 0;
}

// ------------------------------------
// The main program
// ------------------------------------
//#ifdef EVOLVE
int main (int argc, const char* argv[]){

    supArgs1.do_evol = 0;
    #ifdef EVOLVE
    supArgs1.do_evol = 1;
    #endif
    supArgs1.printToFile = 0;
    #ifdef PRINTTOFILE
    supArgs1.printToFile = 1;
    #endif
    supArgs1.output = 0;
    #ifdef OUTPUT
    supArgs1.output = 1;
    #endif

    std::cout << std::setprecision(10);
    
    long randomseed = static_cast<long>(time(NULL));
    
    if (argc == 2)
        randomseed += atoi(argv[1]);

    supArgs1.randomseed = randomseed;
    if (argc>2) if (!supArgs1.setArgs(argc,argv,randomseed)) return 0;

    if (supArgs1.do_evol){

    TSearch s(VectSize);
    TVector<double> phenotype(1, VectSize);
    
    // save the seed to a file
    ofstream seedfile;
    seedfile.open(supArgs1.rename_file("seed.dat"));
    seedfile << supArgs1.randomseed << endl;
    seedfile.close();
   
    // configure the search
    s.SetRandomSeed(supArgs1.randomseed);
    s.SetPopulationStatisticsDisplayFunction(EvolutionaryRunDisplay);
    s.SetSearchResultsDisplayFunction(ResultsDisplay);
    s.SetSelectionMode(RANK_BASED);             //{FITNESS_PROPORTIONATE,RANK_BASED}
    s.SetReproductionMode(GENETIC_ALGORITHM);	// {HILL_CLIMBING, GENETIC_ALGORITHM}
    s.SetPopulationSize(supArgs1.pop_size);
    s.SetMaxGenerations(supArgs1.max_gens);
    s.SetMutationVariance(0.1);
    s.SetCrossoverProbability(0.5);
    s.SetCrossoverMode(UNIFORM);              //{UNIFORM, TWO_POINT}
    s.SetMaxExpectedOffspring(1.1);
    s.SetElitistFraction(0.04);
    s.SetSearchConstraint(1);
    s.SetCheckpointInterval(0);
    s.SetReEvaluationFlag(0);

    // Stage 1 //
    s.SetSearchTerminationFunction(finish_Bosc);
    s.SetEvaluationFunction(EvaluationFunction1);
    // redirect standard output to a file
    ofstream evolfile;
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    if (supArgs1.printToFile)
    {
        evolfile.open (supArgs1.rename_file("fitness.dat"));
        cout.rdbuf(evolfile.rdbuf());
    }

/* #ifdef PRINTTOFILE
    ofstream evolfile;
    evolfile.open ("fitness.dat");
    cout.rdbuf(evolfile.rdbuf());
#endif */

    s.ExecuteSearch();

    // Stage 2 //
    s.SetSearchTerminationFunction(NULL);
    s.SetEvaluationFunction(EvaluationFunction2);
    InitializeBodyConstants();
    s.ExecuteSearch();

    if (supArgs1.printToFile)
    {
        std::cout.rdbuf(coutbuf); //reset to standard output again
        evolfile.close();
    
    }

    }

/* #ifdef PRINTTOFILE
    evolfile.close();
#endif */

    RandomState rs;
    //long seed = static_cast<long>(time(NULL));
    rs.SetRandomSeed(supArgs1.randomseed);
    
    std::cout << std::setprecision(10);
    
    // Code to run simulation:
    InitializeBodyConstants();

    supArgs1.output = 1;
    ifstream BestIndividualFile;
    TVector<double> bestVector(1, VectSize);
    BestIndividualFile.open(supArgs1.rename_file("best.gen.dat"));
    BestIndividualFile >> bestVector;
    EvaluationFunction2(bestVector, rs);


    return 0;
}

//#endif
