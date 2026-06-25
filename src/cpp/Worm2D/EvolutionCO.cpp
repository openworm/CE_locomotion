#include "EvolutionCO.h"
#include <math.h>

//#include "Segment21.h"

//using namespace TSCO;



void EvolutionCO::writeJson(TVector<double> & v) 
{
	WormAgent w(v,CircuitSize); 
	w.setSimPars(0, 0.5,100,evoPars1.StepSize, 1, 0);	
	writeJson1(w);
}

int EvolutionCO::getVectSize(int CircuitSize_)
{
//cout << "EvolutionCO::getVectSize" << endl;

return 2*(CircuitSize_-4) + (CircuitSize_-4)*(CircuitSize_-4) 
+ (CircuitSize_-2)*2 + (CircuitSize_-4) + 1 + 2*((CircuitSize_-2) + 1) + 4;
}


void EvolutionCO::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{

//	cout << "EvolutionCO::GenPhenMapping " << endl;
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
	return EvaluationFunction<WormAgent>(v,rs);
}




double EvolutionCO::EvaluationFunction(TVector<double> &v, RandomState &rs, WormAgent * Worm)
{

//	cout << "EvolutionCO::EvaluationFunction(TVector<double> &v, RandomState &rs, WormAgent * Worm)" << endl;

	TVector<double> phenotype;
	phenotype.SetBounds(1, itsVectSize());
	GenPhenMapping(v, phenotype);
	
	//WormAgent Worm(CircuitSize);

	//RandomState rs2 = rs;
	Worm->InitializeState(rs);
	Worm->SetParameters(phenotype);
	Worm->setStepSize(evoPars1.StepSize);

    double rundur = evoPars1.Transient + evoPars1.Duration;

	Worm->setValCJWorm<double>("RunDuration",rundur);
	Worm->setStepSize(evoPars1.StepSize);

	double f, accdist, totaldist;
	int k = 0;
	double fitness = 0.0;
	int taxis,kinesis;
	for (int mode = 1; mode <= 1; mode++)
	{
		if (mode==0){taxis = 0;kinesis = 1;}
		else {taxis = 1;kinesis = 0;}

		Worm->setValCJWorm<int>("taxis",taxis);
		Worm->setValCJWorm<int>("kinesis",kinesis);

		for (double gradSteep = 0.5; gradSteep <= 0.5; gradSteep += 0.2)
		{

		Worm->setValCJWorm<double>("gradSteep",gradSteep);
		
			for (double orient = 0.0; orient < 2*Pi; orient += Pi/2)
			{

				Worm->setValCJWorm<double>("orient",orient);
		
				//cout << Worm->itsNewSetVals() << endl;
/* 
				if (orient>Pi){
				cout << Worm->itsNewSetVals() << endl;
				assert(0);
				}  */

				/* Worm->setSimPars(orient,
					gradSteep,
					evoPars1.Transient + evoPars1.Duration,
					evoPars1.StepSize, taxis, kinesis);
 */
				//Worm->InitializeSimulation(rs);

				Worm->initForSimulation(rs);

				/* Worm->InitialiseAgent(2*RunDuration, evoPars1.StepSize);
				Worm->ResetAgentsBody(orient, rs);
				Worm->ResetChemCon(gradSteep);
				Worm->ResetAgentIntState(rs);
				Worm->UpdateChemCon(gradSteep); */

				for (int repeats = 1; repeats <= 2; repeats++)
				{
					//Worm->InitializeSensors(rs);
					Worm->ResetAgentsBody();
					Worm->setTime(0);
					for (double t = evoPars1.StepSize; t <= evoPars1.Transient; t += evoPars1.StepSize)
					{
						//Worm->setStepPars(gradSteep,rs,t,taxis,kinesis);
						//Worm->Step(evoPars1.StepSize);
						Worm->Step();

						//Worm->UpdateSensors();
						//Worm->Step(evoPars1.StepSize,rs,t,taxis,kinesis);
						//Worm->UpdateChemCon(gradSteep);
					}
					accdist = 0.0;
					Worm->setTime(0);
					for (double t = evoPars1.StepSize; t <= evoPars1.Duration; t += evoPars1.StepSize)
					{
						//Worm->setStepPars(gradSteep,rs,t,taxis,kinesis);
						//Worm->Step(evoPars1.StepSize);
						Worm->Step();
						
						//Worm->UpdateSensors();
						//Worm->Step(evoPars1.StepSize,rs,t,taxis,kinesis);
						//Worm->UpdateChemCon(gradSteep);
 
						//accdist += Worm->distanceToCenter();

						accdist += Worm->DistanceToCentre();
						//cout << "D " << Worm->DistanceToCentre() << endl;
					}
					totaldist = (accdist/(evoPars1.Duration/evoPars1.StepSize));
					f = (MaxDist - totaldist)/MaxDist;
					f = f < 0 ? 0.0 : f;
					fitness += f;
					k++;
					//cout << k << " " << totaldist << endl;
				}
			}
		}
	}

//	cout << " evaluation CO " << fitness << endl;
	
//assert(0);
	return fitness/k;
}




double EvolutionCO::Behavior(TVector<double> &v)
{

	return Behavior<WormAgent>(v);

}


double EvolutionCO::Behavior(TVector<double> &v, WormAgent * Worm)
{

	//int VectSize = evoPars1.VectSize;
	TVector<double> phenotype;
	phenotype.SetBounds(1, itsVectSize());
	GenPhenMapping(v, phenotype);
	//WormAgent Worm(CircuitSize);
	//Worm.setBasename(itsEvoPars().directoryName);
    Worm->setDataskips(itsEvoPars().skip_steps);
	Worm->InitializeData(itsEvoPars().directoryName);
	RandomState rs2;
	Worm->InitializeState(rs2);
    //Worm.dataReset();
	Worm->SetParameters(phenotype);
	Worm->setStepSize(itsEvoPars().StepSize);
	return Behavior(*Worm);
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
				
				Worm.setSimPars(orient,gradSteep,
					simPars1.Transient + simPars1.Duration,
					evoPars1.StepSize, taxis, kinesis);
				//Worm.InitializeSimulation(rs);
				Worm.initForSimulation(rs);

				for (int repeats = 1; repeats <= 1; repeats++)
				{
					Worm.ResetAgentsBody();
					Worm.setTime(0);
					for (double t = StepSize; t <= simPars1.Transient; t += StepSize)
					{
					
						Worm.Step();

			

						if (mode==0){filek << t << " ";
							Worm.PrintDetail(filek);}
						else {filet << t << " ";
							Worm.PrintDetail(filet);}
						Worm.writeData();
						//Worm.DumpActState(actfile, skip_steps);	
					}
					accdist = 0.0;
					Worm.setTime(simPars1.Transient);
					for (double t = simPars1.Transient + StepSize; 
						t <= simPars1.Transient + simPars1.Duration; 
						t += StepSize)
					{
				
						Worm.Step();
				
						accdist += Worm.distanceToCenter();
						//accdist += Worm.DistanceToCentre();
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

       var1.parDoub.vals = {evoPars1.Transient, evoPars1.Transient + evoPars1.Duration,
	evoPars1.Duration, HST, BiasRange, SensorWeightRange, 
	InterneuronWeightRange, StretchReceptorRange, MinDifSensor, 
	MaxDifSensor, TauMin, TauMax, MinNeckTurnGain, MaxNeckTurnGain, MaxDist

    };

     

    appendToJson<double>(j[var1.parDoub.head],var1.parDoub);
    //appendToJson<long>(j[var1.parInt.head],var1.parInt);
}
