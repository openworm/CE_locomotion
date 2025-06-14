#include "EvolutionCO.h"
#include <math.h>
#include "WormAgent.h"
//#include "Segment21.h"

//using namespace TSCO;


int EvolutionCO::getVectSize(int CircuitSize)
{
return 2*(CircuitSize-4) + (CircuitSize-4)*(CircuitSize-4) 
+ (CircuitSize-2)*2 + (CircuitSize-4) + 1 + 2*((CircuitSize-2) + 1) + 4;
}


void EvolutionCO::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
{
	int k = 1;

	// Sensor to interneurons
	for (int i = 1; i <= 2*(CircuitSize-4); i++){
		phen(k) = MapSearchParameter(gen(k), -SensorWeightRange, SensorWeightRange);
		k++;
	}

	// Weights between interneurons (fully recurrent, non-symm)
	for (int i = 1; i <= (CircuitSize-4)*(CircuitSize-4) + (CircuitSize-2)*2 + (CircuitSize-4) + 1; i++){
		phen(k) =  MapSearchParameter(gen(k), -InterneuronWeightRange, InterneuronWeightRange);
		k++;
	}

	// Biases interneurons
	for (int i = 1; i <= (CircuitSize-2) + 1; i++){
		phen(k) =  MapSearchParameter(gen(k), -BiasRange, BiasRange);
		k++;
	}

	//  Time-constants
	for (int i = 1; i <= (CircuitSize-2) + 1; i++){
		phen(k) =  MapSearchParameter(gen(k), TauMin, TauMax);
		k++;
	}

	//		CPG to motorneurons
	phen(k) = MapSearchParameter( gen(k), 0.0, StretchReceptorRange); // w_CPG_SMB
	k++;

	//      Difference sensor parameters
	phen(k) = MapSearchParameter( gen(k), MinDifSensor, MaxDifSensor);  // N
	k++;
	phen(k) = MapSearchParameter( gen(k), MinDifSensor, MaxDifSensor);  // M
	k++;

	//      Weight of the connection between the motorneurons and the muscles
	phen(k) = MapSearchParameter( gen(k), MinNeckTurnGain, MaxNeckTurnGain);
}


double EvolutionCO::EvaluationFunction(TVector<double> &v, RandomState &rs)
{


	TVector<double> phenotype;
	phenotype.SetBounds(1, evoPars1.VectSize);
	GenPhenMapping(v, phenotype);
	WormAgent Worm(CircuitSize);
	Worm.SetParameters(phenotype);

	double f, accdist, totaldist;
	int k = 0;
	double fitness = 0.0;
	int taxis,kinesis;
	for (int mode = 1; mode <= 1; mode++)
	{
		if (mode==0){taxis = 0;kinesis = 1;}
		else {taxis = 1;kinesis = 0;}
		for (double gradSteep = 0.5; gradSteep <= 0.5; gradSteep += 0.2)
		{
			for (double orient = 0.0; orient < 2*Pi; orient += Pi/2)
			{
				Worm.setSimPars(orient,gradSteep,evoPars1.Transient + evoPars1.Duration,evoPars1.StepSize);
				Worm.InitializeState(rs);

				/* Worm.InitialiseAgent(2*RunDuration, evoPars1.StepSize);
				Worm.ResetAgentsBody(orient, rs);
				Worm.ResetChemCon(gradSteep);
				Worm.ResetAgentIntState(rs);
				Worm.UpdateChemCon(gradSteep); */

				for (int repeats = 1; repeats <= 2; repeats++)
				{
					Worm.ResetAgentsBody(orient, rs);
					for (double t = evoPars1.StepSize; t <= evoPars1.Transient; t += evoPars1.StepSize)
					{
						Worm.setStepPars(gradSteep,rs,t,taxis,kinesis);
						Worm.Step(evoPars1.StepSize);

						//Worm.UpdateSensors();
						//Worm.Step(evoPars1.StepSize,rs,t,taxis,kinesis);
						//Worm.UpdateChemCon(gradSteep);
					}
					accdist = 0.0;
					for (double t = evoPars1.StepSize; t <= evoPars1.Duration; t += evoPars1.StepSize)
					{
						Worm.setStepPars(gradSteep,rs,t,taxis,kinesis);
						Worm.Step(evoPars1.StepSize);

						//Worm.UpdateSensors();
						//Worm.Step(evoPars1.StepSize,rs,t,taxis,kinesis);
						//Worm.UpdateChemCon(gradSteep);

						accdist += Worm.DistanceToCentre();
					}
					totaldist = (accdist/(evoPars1.Duration/evoPars1.StepSize));
					f = (MaxDist - totaldist)/MaxDist;
					f = f < 0 ? 0.0 : f;
					fitness += f;
					k++;
				}
			}
		}
	}
	return fitness/k;
}

double EvolutionCO::Behavior(TVector<double> &v)
{

	int VectSize = evoPars1.VectSize;
	TVector<double> phenotype;
	phenotype.SetBounds(1, VectSize);
	GenPhenMapping(v, phenotype);
	WormAgent Worm(CircuitSize);
	Worm.SetParameters(phenotype);
	return Behavior(Worm);
}


double EvolutionCO::Behavior(Worm2Dbase & w1)
{

	//const int & skip_steps = evoPars1.skip_steps;
	double StepSize = evoPars1.StepSize;

	WormAgent & Worm = dynamic_cast<WormAgent&>(w1);

	RandomState rs;
	ofstream filet(rename_file("behavior_taxis.dat"));
	ofstream filek(rename_file("behavior_kinesis.dat"));
	//ofstream actfile(rename_file("act.dat"));
	

	double f, accdist, totaldist;
	int k = 0;
	double fitness = 0.0;
	int taxis,kinesis;
	for (int mode = 1; mode <= 1; mode++)
	{
		if (mode==0){taxis = 0;kinesis = 1;}
		else {taxis = 1;kinesis = 0;}
		for (double gradSteep = 0.5; gradSteep <= 0.5; gradSteep += 0.2)
		{
			for (double orient = 0.0; orient <= 0.0; orient += Pi/2)
			{
				/* Worm.InitialiseAgent(2*(simPars1.Transient + simPars1.Duration), StepSize);
				Worm.ResetAgentsBody(orient, rs);
				Worm.ResetChemCon(gradSteep);
				Worm.ResetAgentIntState(rs);
				Worm.UpdateChemCon(gradSteep); */

				Worm.setSimPars(orient,gradSteep,simPars1.Transient + simPars1.Duration,evoPars1.StepSize);
				Worm.InitializeState(rs);

				for (int repeats = 1; repeats <= 1; repeats++)
				{
					Worm.ResetAgentsBody(orient, rs);
					for (double t = StepSize; t <= simPars1.Transient; t += StepSize)
					{
						Worm.setStepPars(gradSteep,rs,t,taxis,kinesis);
						Worm.Step(StepSize);

						//Worm.UpdateSensors();
						//Worm.Step(StepSize,rs,t,taxis,kinesis);
						//Worm.UpdateChemCon(gradSteep);

						if (mode==0){filek << t << " ";
							Worm.PrintDetail(filek);}
						else {filet << t << " ";
							Worm.PrintDetail(filet);}
						Worm.writeData();
						//Worm.DumpActState(actfile, skip_steps);	
					}
					accdist = 0.0;
					for (double t = simPars1.Transient + StepSize; t <= simPars1.Transient + simPars1.Duration; t += StepSize)
					{
						Worm.setStepPars(gradSteep,rs,t,taxis,kinesis);
						Worm.Step(StepSize);

						//Worm.UpdateSensors();
						//Worm.Step(StepSize,rs,t,taxis,kinesis);
						//Worm.UpdateChemCon(gradSteep);

						accdist += Worm.DistanceToCentre();
						if (mode==0){filek << t << " ";
							Worm.PrintDetail(filek);}
						else {filet << t << " ";
							Worm.PrintDetail(filet);}
						Worm.writeData();	
						//Worm.DumpActState(actfile, skip_steps);
					}
					totaldist = (accdist/(simPars1.Duration/StepSize));
					f = (MaxDist - totaldist)/MaxDist;
					f = f < 0 ? 0.0 : f;
					fitness += f;
					k++;
				}
			}
		}
	}
	filet.close();
	filek.close();
	//actfile.close();
	return fitness/k;
}

void EvolutionCO::addExtraParsToJson(json & j)
{   
    

    doubIntParamsHead var1;
    var1.parDoub.head = "Evolutionary Optimization Parameters";
       //var1.parInt.head = "Evolutionary Optimization Parameters";
       var1.parDoub.names = {"TransientDuration", "RunDuration", "EvalDuration", "HST", "BiasRange", "SensorWeightRange", 
	"InterneuronWeightRange", "StretchReceptorRange", "MinDifSensor", 
	"MaxDifSensor", "TauMin", "TauMax", "MinNeckTurnGain", "MaxNeckTurnGain", "MaxDist"
	   };

       var1.parDoub.vals = {
TransientDuration, RunDuration, EvalDuration, HST, BiasRange, SensorWeightRange, 
	InterneuronWeightRange, StretchReceptorRange, MinDifSensor, 
	MaxDifSensor, TauMin, TauMax, MinNeckTurnGain, MaxNeckTurnGain, MaxDist

    };

       //var1.parInt.names = {};
       //var1.parInt.vals = {};

    appendToJson<double>(j[var1.parDoub.head],var1.parDoub);
    //appendToJson<long>(j[var1.parInt.head],var1.parInt);
}