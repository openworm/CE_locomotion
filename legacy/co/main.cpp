#include "CTRNN.h"
#include "WormAgent.h"
#include "TSearch.h"
#include <sstream>
#include "../argUtils.h"

#define EVOLVE
#define PRINTTOFILE

// Global constants
const int	CircuitSize = 10;
const double TransientDuration = 50.0;
const double RunDuration = 100.0;
const double EvalDuration = RunDuration - TransientDuration;
const double SimTransientDuration = 10.0;
const double SimEvalDuration = 10.0;

const double BiasRange = 15.0;
const double SensorWeightRange = 1500.0;
const double InterneuronWeightRange = 15.0;
const double StretchReceptorRange = 15.0;
const double MinDifSensor = 10*StepSize;
const double MaxDifSensor = HST;
const double TauMin = 10*StepSize;
const double TauMax = HST;

const double MinNeckTurnGain = 1.0;
const double MaxNeckTurnGain = 2.0;

SuppliedArgsCO supArgs1;


// Global variables
int	VectSize = 2*(CircuitSize-4) + (CircuitSize-4)*(CircuitSize-4) + (CircuitSize-2)*2 + (CircuitSize-4) + 1 + 2*((CircuitSize-2) + 1) + 4;

// ------------------------------------
// Genotype-Phenotype Mapping Functions
// ------------------------------------
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
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

// ------------------------------------
// Fitness function
// ------------------------------------
double EvaluationFunction(TVector<double> &v, RandomState &rs)
{
	TVector<double> phenotype;
	phenotype.SetBounds(1, VectSize);
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
				Worm.InitialiseAgent(2*RunDuration, StepSize);
				Worm.ResetAgentsBody(orient, rs);
				Worm.ResetChemCon(gradSteep);
				Worm.ResetAgentIntState(rs);
				Worm.UpdateChemCon(gradSteep);
				for (int repeats = 1; repeats <= 2; repeats++)
				{
					Worm.ResetAgentsBody(orient, rs);
					for (double t = StepSize; t <= TransientDuration; t += StepSize)
					{
						Worm.UpdateSensors();
						Worm.Step(StepSize,rs,t,taxis,kinesis);
						Worm.UpdateChemCon(gradSteep);
					}
					accdist = 0.0;
					for (double t = StepSize; t <= EvalDuration; t += StepSize)
					{
						Worm.UpdateSensors();
						Worm.Step(StepSize,rs,t,taxis,kinesis);
						Worm.UpdateChemCon(gradSteep);
						accdist += Worm.DistanceToCentre();
					}
					totaldist = (accdist/(EvalDuration/StepSize));
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

// ------------------------------------
// Fitness function
// ------------------------------------
void Generalization(TVector<double> &v)
{
	RandomState rs;
	ofstream filet(supArgs1.rename_file("gen_taxis.dat"));
	ofstream filek(supArgs1.rename_file("gen_kinesis.dat"));
	ofstream file(supArgs1.rename_file("gen.dat"));
	TVector<double> phenotype;
	phenotype.SetBounds(1, VectSize);
	GenPhenMapping(v, phenotype);
	WormAgent Worm(CircuitSize);
	Worm.SetParameters(phenotype);

	double f, accdist, totaldist;
	int k = 0;
	double fitness = 0.0;
	int taxis,kinesis;
	for (int mode = 0; mode <= 1; mode++)
	{
		if (mode==0){taxis = 0;kinesis = 1;}
		else {taxis = 1;kinesis = 0;}
		for (double gradSteep = 0.1; gradSteep <= 1.0; gradSteep += 0.01)
		{
			for (double orient = 0.0; orient < 2*Pi; orient += Pi/24)
			{
				Worm.InitialiseAgent(2*RunDuration, StepSize);
				Worm.ResetAgentsBody(orient, rs);
				Worm.ResetChemCon(gradSteep);
				Worm.ResetAgentIntState(rs);
				Worm.UpdateChemCon(gradSteep);
				double avgfit = 0.0;
				for (int repeats = 1; repeats <= 2; repeats++)
				{
					Worm.ResetAgentsBody(orient, rs);
					for (double t = StepSize; t <= TransientDuration; t += StepSize)
					{
						Worm.UpdateSensors();
						Worm.Step(StepSize,rs,t,taxis,kinesis);
						Worm.UpdateChemCon(gradSteep);
					}
					accdist = 0.0;
					for (double t = StepSize; t <= EvalDuration; t += StepSize)
					{
						Worm.UpdateSensors();
						Worm.Step(StepSize,rs,t,taxis,kinesis);
						Worm.UpdateChemCon(gradSteep);
						accdist += Worm.DistanceToCentre();
					}
					totaldist = (accdist/(EvalDuration/StepSize));
					f = (MaxDist - totaldist)/MaxDist;
					f = f < 0 ? 0.0 : f;
					avgfit += f;
					fitness += f;
					k++;
				}
				if (mode==0){filek << gradSteep << " " << orient << " " << avgfit/2 << endl;}
				else {filet << gradSteep << " " << orient << " " << avgfit/2 << endl;}
			}
		}
	}
	filet.close();
	filek.close();
	file << fitness/k << endl;
	file.close();
}
// ------------------------------------
// Fitness function
// ------------------------------------
double Behavior(TVector<double> &v)
{
	RandomState rs;
	ofstream filet(supArgs1.rename_file("behavior_taxis.dat"));
	ofstream filek(supArgs1.rename_file("behavior_kinesis.dat"));
	TVector<double> phenotype;
	phenotype.SetBounds(1, VectSize);
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
			for (double orient = 0.0; orient <= 0.0; orient += Pi/2)
			{
				Worm.InitialiseAgent(2*RunDuration, StepSize);
				Worm.ResetAgentsBody(orient, rs);
				Worm.ResetChemCon(gradSteep);
				Worm.ResetAgentIntState(rs);
				Worm.UpdateChemCon(gradSteep);
				for (int repeats = 1; repeats <= 1; repeats++)
				{
					Worm.ResetAgentsBody(orient, rs);
					for (double t = StepSize; t <= SimTransientDuration; t += StepSize)
					{
						Worm.UpdateSensors();
						Worm.Step(StepSize,rs,t,taxis,kinesis);
						Worm.UpdateChemCon(gradSteep);
						if (mode==0){filek << t << " ";
							Worm.PrintDetail(filek);}
						else {filet << t << " ";
							Worm.PrintDetail(filet);}
					}
					accdist = 0.0;
					for (double t = SimTransientDuration + StepSize; 
						t <= SimTransientDuration + SimEvalDuration; t += StepSize)
					{
						Worm.UpdateSensors();
						Worm.Step(StepSize,rs,t,taxis,kinesis);
						Worm.UpdateChemCon(gradSteep);
						accdist += Worm.DistanceToCentre();
						if (mode==0){filek << t << " ";
							Worm.PrintDetail(filek);}
						else {filet << t << " ";
							Worm.PrintDetail(filet);}
					}
					totaldist = (accdist/(SimEvalDuration/StepSize));
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
	return fitness/k;
}

// ------------------------------------
// Display functions
// ------------------------------------
void EvolutionaryRunDisplay(int Generation, double BestPerf, double AvgPerf, double PerfVar)
{
	cout << BestPerf << " " << AvgPerf << " " << PerfVar << endl;
}

void ResultsDisplay(TSearch &s)
{
	TVector<double> bestVector;
	ofstream BestIndividualFile;
	TVector<double> phenotype;
	phenotype.SetBounds(1, VectSize);

	// Save the genotype of the best individual
	bestVector = s.BestIndividual();
	Generalization(bestVector);
	Behavior(bestVector);
	BestIndividualFile.open(supArgs1.rename_file("best.gen.dat"));
	BestIndividualFile << bestVector << endl;
	BestIndividualFile.close();

	// Also show the best individual in the Circuit Model form
	BestIndividualFile.open(supArgs1.rename_file("best.ns.dat"));
	GenPhenMapping(bestVector, phenotype);
	WormAgent Worm(CircuitSize);
	Worm.SetParameters(phenotype);
	Worm.InitialiseAgent(RunDuration, StepSize);

	// sensory connections
	for (int i = 1; i <= CircuitSize-4; i++){
		BestIndividualFile << Worm.w_ASER[i] << endl;
	}
	for (int i = 1; i <= CircuitSize-4; i++){
		BestIndividualFile << Worm.w_ASEL[i] << endl;
	}

	BestIndividualFile << Worm.w_CPG_SMBV << endl;
	BestIndividualFile << Worm.w_CPG_SMBD << endl;

	BestIndividualFile << Worm.sensorN << endl;
	BestIndividualFile << Worm.sensorM << endl;

	BestIndividualFile << Worm.outputGain << endl;
	// the rest of the network
	BestIndividualFile << Worm.NervousSystem;
	BestIndividualFile.close();
}


// ------------------------------------
// The main program
// ------------------------------------
#ifdef EVOLVE
int main (int argc, const char* argv[])
{
	long randomseed = 1749493257; //static_cast<long>(time(NULL));
	//if (argc == 2)
	//	randomseed += atoi(argv[1]);
	

	if (argc>2) if (!supArgs1.setArgs(argc,argv,randomseed)) return 0;

	TSearch s(VectSize);
	TVector<double> phenotype;
	phenotype.SetBounds(1, VectSize);

	// save the seed to a file
	ofstream seedfile;
	seedfile.open (supArgs1.rename_file("seed.dat"));
	seedfile << randomseed << endl;
	seedfile.close();

	// configure the search
	s.SetRandomSeed(randomseed);
	s.SetPopulationStatisticsDisplayFunction(EvolutionaryRunDisplay);
	s.SetSearchResultsDisplayFunction(ResultsDisplay);
	s.SetSelectionMode(RANK_BASED);			//{FITNESS_PROPORTIONATE,RANK_BASED}
	s.SetReproductionMode(GENETIC_ALGORITHM);	// {HILL_CLIMBING, GENETIC_ALGORITHM}
	//s.SetPopulationSize(26); //96
	s.SetPopulationSize(supArgs1.pop_size); //96
	//s.SetMaxGenerations(40); //100
	s.SetMaxGenerations(supArgs1.max_gens);
	s.SetMutationVariance(0.05);
	s.SetCrossoverProbability(0.5);
	s.SetCrossoverMode(UNIFORM);			//{UNIFORM, TWO_POINT}
	s.SetMaxExpectedOffspring(1.1);
	s.SetElitistFraction(0.1);				//Default is 0.0.
	s.SetSearchConstraint(1);
	s.SetReEvaluationFlag(1);

	std::streambuf *coutbuf = std::cout.rdbuf();
	// redirect standard output to a file
	#ifdef PRINTTOFILE
	ofstream evolfile;
	evolfile.open (supArgs1.rename_file("evol.dat"));
	cout.rdbuf(evolfile.rdbuf());
	#endif

	// start evolution
	s.SetEvaluationFunction(EvaluationFunction);
	s.ExecuteSearch();

	#ifdef PRINTTOFILE
	std::cout.rdbuf(coutbuf); //reset to standard output again
	evolfile.close();
	#endif

	ifstream genefile(supArgs1.rename_file("best.gen.dat"));
	TVector<double> genotype(1, VectSize);
	genefile >> genotype;
	
	cout << Behavior(genotype) << endl;
	//cout << Generalization(genotype) << endl;
	genefile.close();
	return 0;

	
}
#else
int main (int argc, const char* argv[])
{
	ifstream genefile("best.gen.dat");
	TVector<double> genotype(1, VectSize);
	genefile >> genotype;
	//cout << Behavior(genotype) << endl;
	cout << Generalization(genotype) << endl;
	return 0;
}
#endif
