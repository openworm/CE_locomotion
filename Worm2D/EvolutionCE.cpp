#include "EvolutionCE.h"
#include <math.h>
#include "Worm2DCE.h"

void EvolutionCE::writeJson(TVector<double> &v) {WormCE w(v); writeJson1(w);}

void EvolutionCE::addExtraParsToJson(json & j)
{

    doubIntParamsHead var1;
    var1.parDoub.head = "Evolutionary Optimization Parameters";
       var1.parInt.head = "Evolutionary Optimization Parameters";
       var1.parDoub.names = {"AvgSpeed", "BBCfit", "BiasRange", "SCRange",
        "CSRange", "ESRange", "SRmax", "NMJmax", "NMJmin"
      };
       var1.parDoub.vals = {AvgSpeed, BBCfit, BiasRange, SCRange,
        CSRange, ESRange, SRmax, NMJmax, NMJmin
      };

       var1.parInt.names = {"SR_A", "SR_B"};
       var1.parInt.vals = {SR_A, SR_B};

    appendToJson<double>(j[var1.parDoub.head],var1.parDoub);
    appendToJson<long>(j[var1.parInt.head],var1.parInt);
}

void EvolutionCE::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{
     // Parameters for the Stretch Receptors
  phen(SR_A) = MapSearchParameter(gen(SR_A), 0.0, SRmax);
  phen(SR_B) = MapSearchParameter(gen(SR_B), 0.0, SRmax);

  // Bias
  int k=3;
  for (int i = 1; i <= 3; i++){
    phen(k) = MapSearchParameter(gen(k), -BiasRange, BiasRange);k++;
  }
  // Self connections
  for (int i = 1; i <= 3; i++){
    phen(k) = MapSearchParameter(gen(k), -SCRange, SCRange);k++;
  }
  // DA, DB, VA, VB Chemical synapses (excitatory)
  for (int i = 1; i <= 2; i++){
    phen(k) = MapSearchParameter(gen(k), 0.0, CSRange);k++;
  }
  // VD Chemical synapses (Inhibitory)
  for (int i = 1; i <= 2; i++){
    phen(k) = MapSearchParameter(gen(k), -CSRange, 0.0);k++;
  }
  // Interunits Gap junctions
  for (int i = 1; i <= 2; i++){
    phen(k) = MapSearchParameter(gen(k), 0.0, ESRange);k++;
  }
  // Excitatory NMJ Weight
  for (int i = 1; i <= 2; i++){
    phen(k) = MapSearchParameter(gen(k), NMJmin, NMJmax);k++;
  }
  // Inhibitory NMJ Weight
  for (int i = 1; i <= 1; i++){
    phen(k) = MapSearchParameter(gen(k), -NMJmax, -NMJmin);k++;
  }


}

double EvolutionCE::EvaluationFunction(TVector<double> &v, RandomState &rs)
{
    double sra = v(SR_A);
    double srb = v(SR_B);
    double fitnessForward, fitnessBackward;
    v(SR_A)= -1.0;
    v(SR_B)= srb;
    fitnessForward = Evaluation(v, rs, 1);
    //  v(SR_A)= sra;
    //  v(SR_B)= -1.0;
    //  fitnessBackward = Evaluation(v, rs, -1);
    //  return (fitnessForward + fitnessBackward)/2;

    v(SR_A)= sra;
    v(SR_B)= srb;
    return fitnessForward;
    // return fitnessBackward;
}

double EvolutionCE::Evaluation(TVector<double> &v, RandomState &rs, int direction){

  const double & Duration = evoPars1.Duration;
  //const int & VectSize = evoPars1.VectSize;
  const double & StepSize = evoPars1.StepSize;
  const double & Transient = evoPars1.Transient;


    double fitA,fitB;
    double bodyorientation, anglediff;
    double movementorientation, distancetravelled = 0, temp;
    double distance;
    double xt, xtp, oxt, fxt;
    double yt, ytp, oyt, fyt;

    // Genotype-Phenotype Mapping
    TVector<double> phenotype(1,  itsVectSize());
    GenPhenMapping(v, phenotype);
    WormCE w(phenotype);
    w.InitializeState(rs);

    if (direction == 1){
      w.setInputOnce(0);
        //w.W2DCEpars1->AVA_output =  0.0;
        //w.W2DCEpars1->AVB_output =  1.0;
    }
    else{
      w.setInputOnce(1);
        //w.W2DCEpars1->AVA_output =  1.0;
        //w.W2DCEpars1->AVB_output =  0.0; // Command Interneuron Activation Backward
    }

    // Transient
    for (double t = 0.0; t <= Transient; t += StepSize){
        w.Step(StepSize);
    }
    xt = w.CoMx(); yt = w.CoMy();
    oxt = w.CoMx(); oyt = w.CoMy();
    // Run
    for (double t = 0.0; t <= Duration; t += StepSize) {
        w.Step(StepSize);
        // Current and past centroid position
        xtp = xt; ytp = yt;
        xt = w.CoMx(); yt = w.CoMy();
        // Integration error check
        if (isnan(xt) || isnan(yt) || sqrt(pow(xt-xtp,2)+pow(yt-ytp,2)) > 10*AvgSpeed*StepSize) {return 0.0;}
        // Velocity Fitness
        bodyorientation = w.Orientation();                  // Orientation of the body position
        movementorientation = atan2(yt-ytp,xt-xtp);         // Orientation of the movement
        anglediff = movementorientation - bodyorientation;  // Check how orientations align
        if (direction == 1){
            temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
        }
        else{
            temp = cos(anglediff) > 0.0 ? -1.0 : 1.0;           // Add to fitness only movement backward
        }
        distancetravelled += temp * sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));
    }
    fxt = w.CoMx(); fyt = w.CoMy();
    distance = sqrt(pow(oxt-fxt,2)+pow(oyt-fyt,2));
    fitA = 1 - (fabs(BBCfit - distance)/BBCfit);
    fitA = (fitA > 0)? fitA : 0.0;

    fitB = 1 - (fabs(BBCfit-distancetravelled)/BBCfit);
    fitB = (fitB > 0)? fitB : 0.0;
    return fitB;
}


void EvolutionCE::RunSimulation(TVector<double> &v, RandomState &rs)
{
  save_traces(v,rs);
  return;
}


double EvolutionCE::save_traces(TVector<double> &v, RandomState &rs){


  const double & Duration = evoPars1.Duration;
  //const int & VectSize = evoPars1.VectSize;
  const double & StepSize = evoPars1.StepSize;
  const double & Transient = evoPars1.Transient;
  //const int & skip_steps = evoPars1.skip_steps;


  //ofstream curvfile(rename_file("curv.dat"));
  //ofstream bodyfile(rename_file("body.dat"));
  //ofstream actfile(rename_file("act.dat"));
  // Genotype-Phenotype Mapping
  TVector<double> phenotype(1, itsVectSize());
  GenPhenMapping(v, phenotype);
  double sra = phenotype(SR_A);
  double srb = phenotype(SR_B);
  
  
  WormCE w(phenotype);
  w.setBasename(itsEvoPars().directoryName);
  w.setDataskips(itsEvoPars().skip_steps);
  w.dataReset();

  {
  ofstream phenfile(rename_file("phenotype.dat"));
  w.DumpParams(phenfile);
  phenfile.close();
  } 
  
  w.InitializeState(rs);
  w.sr_ptr->SR_A_gain = 0.0;
  w.sr_ptr->SR_B_gain = srb;
  w.AVA_output =  w.AVA_inact;
  w.AVB_output =  w.AVB_act;
  w.sr_ptr->setWeights();


  for (double t = 0.0; t <= Transient + Duration; t += StepSize){
      w.Step(StepSize);
      w.writeDataCheck();
      //w.DumpBodyState(bodyfile, skip_steps);
      //w.DumpCurvature(curvfile, skip_steps);
      //w.DumpActState(actfile, skip_steps);
  }

   w.sr_ptr->SR_A_gain = 0.0;
   w.sr_ptr->SR_B_gain = 0.0;
    w.sr_ptr->setWeights();

   for (double t = 0.0; t <= (12); t += StepSize){
       w.Step(StepSize);
       w.writeDataCheck();
       //w.DumpBodyState(bodyfile, skip_steps);
       //w.DumpCurvature(curvfile, skip_steps);
       //w.DumpActState(actfile, skip_steps);
   }

   w.sr_ptr->SR_A_gain = sra;
   w.sr_ptr->SR_B_gain = 0.0;
   w.AVA_output =  w.AVA_act;
   w.AVB_output =  w.AVB_inact;
   w.sr_ptr->setWeights();

   for (double t = 0.0; t <= (20); t += StepSize){
      w.Step(StepSize);
      w.writeDataCheck();
       //w.DumpBodyState(bodyfile, skip_steps);
       //w.DumpCurvature(curvfile, skip_steps);
       //w.DumpActState(actfile, skip_steps);
   }

   w.sr_ptr->SR_A_gain = 0.0;
   w.sr_ptr->SR_B_gain = 0.0;
   w.sr_ptr->setWeights();

   for (double t = 0.0; t <= (12); t += StepSize){
       w.Step(StepSize);
       w.writeDataCheck();
       //w.DumpBodyState(bodyfile, skip_steps);
       //w.DumpCurvature(curvfile, skip_steps);
       //w.DumpActState(actfile, skip_steps);
   }

  
  //bodyfile.close();
  //curvfile.close();
  return 0;
}


void EvolutionCE::RunSimulation(Worm2Dbase & w1, RandomState &rs){

  cout << "running sim" << endl;
  
  Worm2DCE & w = dynamic_cast<Worm2DCE&>(w1);

  const double & Duration = evoPars1.Duration;
  
  const double & StepSize = evoPars1.StepSize;
  const double & Transient = evoPars1.Transient;
  //const int & skip_steps = evoPars1.skip_steps;


  //ofstream curvfile(rename_file("curv.dat"));
  //ofstream bodyfile(rename_file("body.dat"));
  //ofstream actfile(rename_file("act.dat"));
  

  double sra = w.sr_ptr->SR_A_gain;
  double srb = w.sr_ptr->SR_B_gain;
  double wao = w.AVA_output;
  double wbo = w.AVB_output;

  cout << "ce evo" << sra << " " << srb << " " << wao << " " << wbo << endl;

  w.InitializeState(rs);
  w.sr_ptr->SR_A_gain = 0.0;
  w.sr_ptr->SR_B_gain = srb;
  w.AVA_output =  w.AVA_inact;
  w.AVB_output =  w.AVB_act;
  w.sr_ptr->setWeights();


  for (double t = 0.0; t <= Transient + Duration; t += StepSize){
      w.Step(StepSize);
      w.writeDataCheck();
      //w.DumpBodyState(bodyfile, skip_steps);
      //w.DumpCurvature(curvfile, skip_steps);
      //w.DumpActState(actfile, skip_steps);
  }

   w.sr_ptr->SR_A_gain = 0.0;
   w.sr_ptr->SR_B_gain = 0.0;
    w.sr_ptr->setWeights();

   for (double t = 0.0; t <= (12); t += StepSize){
       w.Step(StepSize);
       w.writeDataCheck();
       //w.DumpBodyState(bodyfile, skip_steps);
       //w.DumpCurvature(curvfile, skip_steps);
       //w.DumpActState(actfile, skip_steps);
   }

   w.sr_ptr->SR_A_gain = sra;
   w.sr_ptr->SR_B_gain = 0.0;
   w.AVA_output =  w.AVA_act;
   w.AVB_output =  w.AVB_inact;

   w.sr_ptr->setWeights();

   for (double t = 0.0; t <= (20); t += StepSize){
       w.Step(StepSize);
       w.writeDataCheck();
       //w.DumpBodyState(bodyfile, skip_steps);
       //w.DumpCurvature(curvfile, skip_steps);
       //w.DumpActState(actfile, skip_steps);
   }

   w.sr_ptr->SR_A_gain = 0.0;
   w.sr_ptr->SR_B_gain = 0.0;
   w.sr_ptr->setWeights();

   for (double t = 0.0; t <= (12); t += StepSize){
       w.Step(StepSize);
       w.writeDataCheck();
       //w.DumpBodyState(bodyfile, skip_steps);
       //w.DumpCurvature(curvfile, skip_steps);
       //w.DumpActState(actfile, skip_steps);
   }

  // reset worm parameters to presimulation values

  w.sr_ptr->SR_A_gain = sra;
  w.sr_ptr->SR_B_gain = srb;
  w.AVA_output =  wao;
  w.AVB_output =  wbo;
   w.sr_ptr->setWeights();
  //bodyfile.close();
  //curvfile.close();
  //actfile.close();
}


