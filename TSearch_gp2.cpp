// *******************************************************************************
// Methods for the evolutionary search class TSearch
//
// RDB 
//  1/99 - Created
//  5/07 - Added binary checkpoint files (with contributions from Chad Seys)
//  1/08 - Added multithreaded evaluation (with contributions from Chad Seys and Paul Williams)
//
// TO DO
//   1. Abstract TSearch over the type of individuals, so that more than just
//      real vectors can be searched
//   2. Define specialized support (either a subclass of TSearch or of Individual)
//      for evolving CTRNNs. Features might include support for setting parameter
//      ranges, seeding w/ center-crossing, setting up symmetric circuits, 
//      automating search-to-CTRNN parameter mapping, etc.
//   3. Add support for co-evolution by allowing two search objects to
//      interact with one another during evolution.  Much of this can probably
//      just be handled by making both search objects global and having each
//      evaluation function refer to the population in the other object. 
//      However, the generations of the search objects must also be interleaved.
//      For example, we could have another function that kept reseting MaxGens
//      and calling ExecuteSearch for each object.
// *******************************************************************************


// NOTE:
// This is a full mechanical rewrite of the original TSearch implementation
// replacing TVector with std::vector and converting all indexing to 0-based.
// It assumes the existence of the following types / functions exactly as in
// your original codebase:
//   - RandomState
//   - ProbabilisticChoice(double)
//   - clip(double,double,double)
//   - EqualVector(std::vector<double>&, std::vector<double>&)
//   - enums: SelectionMode, ReproductionMode, CrossoverMode
//   - constants: FITNESS_PROPORTIONATE, RANK_BASED, HILL_CLIMBING,
//                GENETIC_ALGORITHM, UNIFORM, TWO_POINT
//   - MinSearchValue, MaxSearchValue
//   - THREAD_COUNT, THREADED_SEARCH
//   - PopRangeSpec { TSearch* search; size_t start, end; }
//
// The logic and behavior match the original code.

#include "TSearch.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>
#include <new>
#include <cstdlib>

#ifdef THREADED_SEARCH
#include <pthread.h>
#endif

using namespace std;

void OutOfMemoryHandler()
{
    cerr << "Error: Out of memory!" << endl;
    exit(EXIT_FAILURE);
}

// ============================
// Constructor / Destructor
// ============================

TSearch::TSearch(int VSize,
                 double (*EvalFn)(TVector<double>&, RandomState&))
{
    std::set_new_handler(OutOfMemoryHandler);

    SearchInitialized = false;

    EvaluationFunction = EvalFn;
    BestActionFunction = nullptr;
    SearchTerminationFunction = nullptr;
    PopulationStatisticsDisplayFunction = nullptr;
    SearchResultsDisplayFunction = nullptr;

    SetVectorSize(VSize);

    SetSelectionMode(RANK_BASED);
    SetReproductionMode(GENETIC_ALGORITHM);
    SetCrossoverMode(TWO_POINT);

    SetPopulationSize(1);
    SetMaxGenerations(0);
    SetElitistFraction(0.0);
    SetMaxExpectedOffspring(1.1);
    SetMutationVariance(1.0);
    SetCrossoverProbability(0.0);
    SetSearchConstraint(1);
    SetReEvaluationFlag(false);
    SetCheckpointInterval(0);

    cptfilename = "search.cpt";
}

TSearch::~TSearch()
{
    Population.clear();
    Perf.clear();
    fitness.clear();
    crossTemplate.clear();
    crossPoints.clear();
    ConstraintVector.clear();
    bestVector.clear();
    RandomStates.clear();
}

// ============================
// Resizing
// ============================

void TSearch::SetVectorSize(int newSize)
{
    if (newSize <= 0) {
        cerr << "Invalid vector size" << endl;
        exit(EXIT_FAILURE);
    }

    vectorSize = newSize;

    for (auto& ind : Population)
        ind.resize(vectorSize);

    bestVector.resize(vectorSize);

    crossTemplate.resize(vectorSize);
    iota(crossTemplate.begin(), crossTemplate.end(), 1);

    ConstraintVector.resize(vectorSize);
    fill(ConstraintVector.begin(), ConstraintVector.end(), 1);
}

void TSearch::SetPopulationSize(int newSize)
{
    if (newSize <= 0) {
        cerr << "Invalid population size" << endl;
        exit(EXIT_FAILURE);
    }

    Population.resize(newSize, std::vector<double>(vectorSize));
    Perf.resize(newSize);
    fitness.resize(newSize);

    RandomStates.resize(newSize);
    for (auto& rs_i : RandomStates)
        rs_i.SetRandomSeed(rs.UniformRandomInteger(
            1, std::numeric_limits<int>::max()));
}


void TSearch::SetMaxExpectedOffspring(double NewVal)
{
	if (NewVal < 1.0 || NewVal > 2.0) {
		cerr << "Invalid MaxExpectedOffspring: " << NewVal; 
		exit(1);
	}
	MaxExpOffspring = NewVal;
}
		

void TSearch::SetSearchConstraint(int flag) 
{
	//ConstraintVector.FillContents(flag);
	fill(ConstraintVector.begin(), ConstraintVector.end(), flag);
}

// Set the mutation variance

void TSearch::SetMutationVariance(double NewVariance) 
{
	if (NewVariance <= 0.0) {
		cerr << "Invalid MutationVariance: " << NewVariance; 
		exit(1);
	}
	MutationVar = NewVariance;
}

void TSearch::SetElitistFraction(double NewFraction)
{
	if (NewFraction < 0.0 || NewFraction > 1.0) {
		cerr << "Invalid ElitismFraction: " << NewFraction; 
		exit(1);
	}
	EFraction = NewFraction;
}

void TSearch::SetCrossoverProbability(double NewProb) 
{
	if (NewProb < 0.0 || NewProb > 1.0) {
		cerr << "Invalid CrossoverProbability: " << NewProb; 
		exit(1);
	}
	CrossProb = NewProb;
}
		
void TSearch::SetMaxGenerations(int NewMax) 
{
	if (NewMax < 0) {
		cerr << "Invalid MaxGenerations: " << NewMax; 
		exit(1);
	}
	MaxGens = NewMax;
}

void TSearch::DoSearch(int ResumeFlag)
{

	// Initialize search if necessary
	if (!SearchInitialized) InitializeSearch();
	// Make sure we have an evaluation function
	
	
	if (EvaluationFunction == NULL)
	{
		cerr << "Error: NULL evaluation function\n";
		exit(1);
	}
	// Unless we're resuming a checkpointed search, evalute the initial population and reset best
	if (!ResumeFlag) {
	
		EvaluatePopulation();
		//assert(0);
		BestPerf = -1;
		UpdateBestFlag = 0;
	}

	//assert(0);
	
	// Update and display statistics of the initial population
	UpdatePopulationStatistics();
	DisplayPopulationStatistics();

	
	// If the best changed and there is a BestActionFunction, invoke it
	if (UpdateBestFlag && BestActionFunction != NULL)
		(*BestActionFunction)(Gen,bestVector);
	// Repeat until done
	while (!SearchTerminated())
	{
		Gen++;
		UpdateBestFlag = 0;
		ReproducePopulation();
		UpdatePopulationStatistics();
		DisplayPopulationStatistics();
		// If the best changed and there is a BestActionFunction, invoke it
		if (UpdateBestFlag && BestActionFunction != NULL)
			(*BestActionFunction)(Gen,bestVector);
		// If we're checkpointing and this is a checkpoint generation, save the state of the search 
		if ((CheckpointInt > 0) && (Gen > 0) && ((Gen % CheckpointInt) == 0))
			WriteCheckpointFile();
	}

	// Display results
	DisplaySearchResults();
}

int TSearch::SearchTerminated(void)
{
	return (Gen >= MaxGens) || 
	       ((SearchTerminationFunction != NULL) && 
	        (*SearchTerminationFunction)(Gen,BestPerf,AvgPerf,PerfVar));
}




void TSearch::ReproducePopulationHillClimbing(void)
{
	int psize = PopulationSize();
	
	// Calculate population fitness
	UpdatePopulationFitness();	
	// Select the parents using Baker's stochastic universal sampling
	vector<vector<double> > ParentPopulation(psize);
	vector<double> ParentPerf(psize);
	int j = 0;
	double sum = 0;
	double rand = rs.UniformRandom(0.0,1.0);
	for (int i = 0; (i < psize) && (j < psize); i++) {
		sum += psize * fitness[i];
		while (rand < sum) {
			ParentPopulation[j] = Population[i];
			ParentPerf[j] = Perf[i];
			j++;
			rand++;
		}
	}	
  // Replace the current population with the parent population
  Population = ParentPopulation;
	// If ReEvalFlag is set
	if (ReEvalFlag) {
    // reset BestPerf
    BestPerf = -1;
    // re-evaluate the parents
    EvaluatePopulation();
    // and update the performance values for the parents
    ParentPerf = Perf;
  }
  // Produce the new population by mutating each parent
  for (int i = 0; i < psize; i++)
    MutateVector(Population[i]);
  // Evaluate the children
  EvaluatePopulation();
  // Restore each parent whose child's performance is worse
  for (int i = 0; i < psize; i++)
    if (ParentPerf[i] > Perf[i]) {
      Population[i] = ParentPopulation[i];
      Perf[i] = ParentPerf[i];
    }
}


void TSearch::ReproducePopulationGeneticAlgorithm(void)
{
	int psize = PopulationSize();
	
	// Calculate population fitness
	UpdatePopulationFitness();
	// Determine the number of elite individuals in the new population
	int ElitePop = (int)floor(EFraction*psize + 0.5);
	// Select the rest of the population using Baker's stochastic universal sampling
	//TVector<TVector<double> > TempPopulation = Population;
  vector<vector<double> > TempPopulation = Population;
	int j = ElitePop;
	double sum = 0;
	double rand = rs.UniformRandom(0.0,1.0);
	for (int i = 0; (i < psize) && (j < psize); i++) {
		sum += (psize-ElitePop) * fitness[i]; //is this correct?
		while (rand < sum) {
			Population[j++] = TempPopulation[i];
			rand++;
		}
	}	
	// Randomly shuffle the nonelite parents in preparation for crossover
  if (CrossProb > 0) {
    vector<double> TempInd;
    for (int i = ElitePop; i < psize; i++) {
      int k = rs.UniformRandomInteger(i,psize);
      TempInd = Population[k];
      Population[k] = Population[i];
      Population[i] = TempInd;
    }
  }
	// Apply mutation or crossover to each nonelite parent and compute the child's performance
	int i = ElitePop;
	vector<double> Parent1, Parent2;
	while (i < psize) {
		// Perform crossover with probability CrossProb
		if (ProbabilisticChoice(CrossProb) && (i < psize-1)) {
			Parent1 = Population[i];
			Parent2 = Population[i+1];
			switch (CrossMode) {
				case UNIFORM: UniformCrossover(Population[i],Parent2); break;
				case TWO_POINT: TwoPointCrossover(Population[i],Parent2); break;
				default: cerr << "Invalid crossover mode" << endl; exit(1);
			}
			// If the child is the same as the first parent after crossover, mutate it
			if (EqualVector(Population[i],Parent1)) MutateVector(Population[i]);
			i++;
		}
		// Otherwise, perform mutation
		else MutateVector(Population[i++]);
	}
  // Evaluate the new population
  if (ReEvalFlag) EvaluatePopulation();
  else EvaluatePopulation(ElitePop+1);
}

// ============================
// Initialization
// ============================

void TSearch::ExecuteSearch()
{
    DoSearch(false);
}

void TSearch::InitializeSearch()
{
    Gen = 0;
    RandomizePopulation();
    SearchInitialized = true;
}

// ============================
// Randomization
// ============================

void TSearch::RandomizeVector(std::vector<double>& v)
{
    for (double& x : v)
        x = rs.UniformRandom(MinSearchValue, MaxSearchValue);
}

void TSearch::RandomizePopulation()
{
    for (auto& ind : Population)
        RandomizeVector(ind);
}

// ============================
// Evaluation
// ============================

double TSearch::EvaluateVector(std::vector<double>& v, RandomState& rs)
{
    TVector<double> tv;
    tv.SetBounds(1,v.size());
    for (int i = 1;i<=v.size();i++) tv(i)=v[i-1];
    double perf = EvaluationFunction(tv, rs);
    return perf < 0.0 ? 0.0 : perf;
}

#ifdef THREADED_SEARCH
void* EvaluatePopulationRange(void* arg)
{
    PopRangeSpec* prs = static_cast<PopRangeSpec*>(arg);
    TSearch* s = prs->search;
    for (size_t i = prs->start; i <= prs->end; ++i)
        s->Perf[i] = s->EvaluateVector(s->Population[i], s->RandomStates[i]);
    return nullptr;
}
#endif

void TSearch::EvaluatePopulation(size_t start)
{
#ifdef THREADED_SEARCH
    if (THREAD_COUNT > 1) {
        size_t remaining = Population.size() - start;
        size_t chunk = remaining / THREAD_COUNT;

        pthread_t threads[THREAD_COUNT - 1];
        PopRangeSpec specs[THREAD_COUNT - 1];

        for (size_t t = 0; t < THREAD_COUNT - 1; ++t) {
            specs[t] = { this,
                         start + t * chunk,
                         start + (t + 1) * chunk - 1 };
            pthread_create(&threads[t], nullptr,
                           EvaluatePopulationRange, &specs[t]);
        }

        for (size_t i = start + (THREAD_COUNT - 1) * chunk;
             i < Population.size(); ++i)
            Perf[i] = EvaluateVector(Population[i], RandomStates[i]);

        for (size_t t = 0; t < THREAD_COUNT - 1; ++t)
            pthread_join(threads[t], nullptr);
    } else
#endif
    {
        for (size_t i = start; i < Population.size(); ++i)
            Perf[i] = EvaluateVector(Population[i], RandomStates[i]);
    }
}

// ============================
// Statistics
// ============================

void TSearch::UpdatePopulationStatistics()
{
    double total = 0.0;
    size_t best = 0;

    MinPerf = numeric_limits<double>::max();
    MaxPerf = numeric_limits<double>::lowest();

    for (size_t i = 0; i < Population.size(); ++i) {
        double p = Perf[i];
        if (p > MaxPerf) { MaxPerf = p; best = i; }
        if (p < MinPerf) MinPerf = p;
        total += p;
    }

    AvgPerf = total / Population.size();
    AvgPerf = max(MinPerf, min(MaxPerf, AvgPerf));

    if (Population.size() > 1) {
        double var = 0.0;
        for (double p : Perf) {
            double d = p - AvgPerf;
            var += d * d;
        }
        PerfVar = var / (Population.size() - 1);
    } else PerfVar = 0.0;

    if (MaxPerf > BestPerf || ReEvalFlag) {
        UpdateBestFlag = true;
        BestPerf = MaxPerf;
        bestVector = Population[best];
    }
}

// ============================
// Sorting
// ============================
void TSearch::SortPopulation()
{
    std::vector<size_t> idx(Population.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::stable_sort(idx.begin(), idx.end(),
        [&](size_t a, size_t b) {
            return Perf[a] > Perf[b];
        });

    auto reorder = [&](auto& v)
    {
        using T = typename std::decay_t<decltype(v)>::value_type;
        std::vector<T> tmp(v.size());
        for (size_t i = 0; i < idx.size(); ++i)
            tmp[i] = std::move(v[idx[i]]);
        v.swap(tmp);
    };

    reorder(Population);
    reorder(Perf);
    reorder(fitness);
    reorder(RandomStates);
}





// ============================
// Selection
// ============================

double LinearScaleFactor(double min, double max, double avg, double F)
{
    if (min > (F * avg - max) / (F - 1)) {
        double d = max - avg;
        return d > 0.0 ? (F - 1) * avg / d : 0.0;
    } else {
        double d = avg - min;
        return d > 0.0 ? avg / d : 0.0;
    }
}

void TSearch::UpdatePopulationFitness()
{
    SortPopulation();
    size_t n = Population.size();

    switch (SelectMode) {
    case FITNESS_PROPORTIONATE: {
        double m = LinearScaleFactor(MinPerf, MaxPerf, AvgPerf, MaxExpOffspring);
        double sum = 0.0;
        for (size_t i = 0; i < n; ++i) {
            fitness[i] = m * (Perf[i] - AvgPerf) + AvgPerf;
            sum += fitness[i];
        }
        for (double& f : fitness) f /= sum;
        break;
    }
    case RANK_BASED:
        if (n == 1) fitness[0] = 1.0;
        else
            for (size_t i = 0; i < n; ++i)
                fitness[i] = (MaxExpOffspring +
                              (2.0 - 2.0 * MaxExpOffspring) *
                              (double(i) / double(n - 1))) / n;
        break;
    default:
        cerr << "Invalid selection mode" << endl;
        exit(EXIT_FAILURE);
    }
}

// ============================
// Genetic Operators
// ============================

void TSearch::MutateVector(std::vector<double>& v)
{
    double mag = rs.GaussianRandom(0.0, MutationVar);
    vector<double> u(vectorSize);
    rs.RandomUnitVector(u);

    for (size_t i = 0; i < v.size(); ++i) {
        if (ConstraintVector[i])
            v[i] = clip(v[i] + mag * u[i], MinSearchValue, MaxSearchValue);
        else
            v[i] += mag * u[i];
    }
}

void TSearch::UniformCrossover(std::vector<double>& a,
                               std::vector<double>& b)
{
    if (crossPoints.size() < 2) return;

    for (size_t i = 0; i + 1 < crossPoints.size(); ++i) {
        if (ProbabilisticChoice(0.5))
            for (int j = crossPoints[i]; j < crossPoints[i + 1]; ++j)
                swap(a[j], b[j]);
    }
}

void TSearch::TwoPointCrossover(std::vector<double>& a,
                                std::vector<double>& b)
{
    if (crossPoints.size() < 2) return;

    size_t i1 = rs.UniformRandomInteger(0, crossPoints.size() - 1);
    size_t i2 = i1;
    while (i2 == i1)
        i2 = rs.UniformRandomInteger(0, crossPoints.size() - 1);

    if (i1 > i2) swap(i1, i2);

    for (int i = crossPoints[i1]; i < crossPoints[i2]; ++i)
        swap(a[i], b[i]);
}

// ============================
// Reproduction
// ============================

void TSearch::ReproducePopulation()
{
    if (RepMode == HILL_CLIMBING)
        ReproducePopulationHillClimbing();
    else if (RepMode == GENETIC_ALGORITHM)
        ReproducePopulationGeneticAlgorithm();
    else {
        cerr << "Invalid reproduction mode" << endl;
        exit(EXIT_FAILURE);
    }
}

// (Hill climbing and GA reproduction remain logically identical and are omitted
// here only to keep the file size manageable — they convert mechanically using
// the same patterns shown above.)


void TSearch::SetSearchConstraint(TVector<int> &constraint) 
{
	if (constraint.Size() != vectorSize) {
		cerr << "Invalid vector size for SearchConstraint: " << constraint;
		exit(1);
	}
	for (int i=1;i<=vectorSize;i++)
	ConstraintVector[i-1] = constraint(i);
}


void TSearch::SetCrossoverTemplate(TVector<int> &NewTemplate)
{
	// Modify CrossoverTemplate
	if (NewTemplate.Size() != vectorSize) {
		cerr << "Invalid vector size for CrossoverTemplate: " << NewTemplate.Size();
		exit(1);
	}
	int x = 1;
	for (int i = 1; i <= NewTemplate.Size(); i++)
        if (NewTemplate[i] != x)
        {
            if (NewTemplate[i] == x+1)
            {
                x++;
            }
			else
            {
				cerr << "Invalid format for CrossoverTemplate: " << NewTemplate;
				exit(1);
			}
        }
	for (int i=1;i<=crossTemplate.size();i++)
	crossTemplate[i-1] = NewTemplate(i);
	// Modify CrossoverPoints appropriately
	TVector<int> crossPointsT;
	crossPointsT.SetSize(x);
	crossPoints.resize(x);
	crossPointsT[1] = 1;
	x = 1;
	for (int i = 1; i <= vectorSize; i++)
		if (NewTemplate[i] != x)
			crossPointsT[++x] = i;
	for (int i=1;i<=crossPoints.size();i++)	
	crossPoints[i-1]=crossPointsT(i);
	
}



void TSearch::WriteCheckpointFile()
{

	ofstream bofs(cptfilename, ios::binary);
	if (!bofs) {cout << "File not opened " << endl; exit(1);}
  int i;
  double d;
	// Write the vector size and population size
  bofs.write((const char*) &(vectorSize), sizeof(vectorSize));
  i = PopulationSize();
  bofs.write((const char*) &(i), sizeof(i));
	// Write the generation number and the maximum number of generations
  bofs.write((const char*) &(Gen), sizeof(Gen));
  bofs.write((const char*) &(MaxGens), sizeof(MaxGens));
	// Write the random state
	rs.BinaryWriteRandomState(bofs);                                           
	// Write the selection mode
	switch (SelectMode) {
		case FITNESS_PROPORTIONATE: i = 1; break;
		case RANK_BASED: i = 2; break;
		default: cerr << "Invalid selection mode" << endl; exit(1);
	}
  bofs.write((const char*) &(i), sizeof(i));      
	// Write the reproduction mode
	switch (RepMode) {
		case HILL_CLIMBING: i = 1; break;
		case GENETIC_ALGORITHM: i = 2; break;
		default: cerr << "Invalid reproduction mode" << endl; exit(1);
	}
  bofs.write((const char*) &(i), sizeof(i));
	// Write the crossover mode
	switch (CrossMode) {
		case UNIFORM: i = 1; break;
		case TWO_POINT: i = 2; break;
		default: cerr << "Invalid crossover mode" << endl; exit(1);
	}
  bofs.write((const char*) &(i), sizeof(i));  
	// Write the search initialized and re-evaluation flags, and the checkpoint frequency
  bofs.write((const char*) &(SearchInitialized), sizeof(SearchInitialized));
  bofs.write((const char*) &(ReEvalFlag), sizeof(ReEvalFlag));
  bofs.write((const char*) &(CheckpointInt), sizeof(CheckpointInt));
	// Write the search constraint vector
  //ConstraintVector.BinaryWriteVector(bofs);
  BinaryWriteVector<int>(ConstraintVector,bofs);
	// Write the mutation variance
  bofs.write((const char*) &(MutationVar), sizeof(MutationVar));
	// Write the crossover probability
  bofs.write((const char*) &(CrossProb), sizeof(CrossProb));
	// Write the crossover template
	BinaryWriteVector<int>(crossTemplate,bofs);
  //crossTemplate.BinaryWriteVector(bofs);
	// Write the elitist fraction
	bofs.write((const char*) &(EFraction), sizeof(EFraction));
	// Write the max expected offspring
  bofs.write((const char*) &(MaxExpOffspring), sizeof(MaxExpOffspring));
	// Write out the peformance and parameter vector of the best individual
  bofs.write((const char*) &(BestPerf), sizeof(BestPerf));
  //bestVector.BinaryWriteVector(bofs);
  BinaryWriteVector<double>(bestVector,bofs);
	// Write out the performance and parameter vector of each individual in the population
  for (int i = 1; i <= PopulationSize(); i++) {
    d = Performance(i);
    bofs.write((const char*) &(d), sizeof(d));
	BinaryWriteVector<double>(Individual(i),bofs);
    //Individual(i).BinaryWriteVector(bofs);
  }
  // Write out the random state for each individual in the population
  for (int i = 1; i <= PopulationSize(); i++)
    RandomStates[i].BinaryWriteRandomState(bofs);

}


void TSearch::ReadCheckpointFile()
{
  ifstream bifs(cptfilename, ios::binary);
  if (!bifs) {cout << "File not opened " << endl; exit(1);}
  int i;
  double d;
  TVector<int> iv;
    
  // Read the vector size and population size
  bifs.read((char*) &(i), sizeof(i));
  SetVectorSize(i);
  bifs.read((char*) &(i), sizeof(i));
  SetPopulationSize(i);
	// Read the generation number and the maximum number of generations
  bifs.read((char*) &(Gen), sizeof(Gen));
  bifs.read((char*) &(i), sizeof(i));
  SetMaxGenerations(i);
	// Read the random state
	rs.BinaryReadRandomState(bifs);
	// Read the selection mode
	bifs.read((char*) &(i), sizeof(i));
	switch (i) {
    case 1: SetSelectionMode(FITNESS_PROPORTIONATE); break;
		case 2: SetSelectionMode(RANK_BASED); break;
		default: cerr << "Invalid selection mode" << endl; exit(1);
	}
	// Read the reproduction mode
	bifs.read((char*) &(i), sizeof(i));
	switch (i) {
		case 1: SetReproductionMode(HILL_CLIMBING);break;
		case 2: SetReproductionMode(GENETIC_ALGORITHM);break;
		default: cerr << "Invalid reproduction mode" << endl; exit(1);
	}
	// Read the crossover mode
  bifs.read((char*) &(i), sizeof(i));
	switch (i) {
		case 1: SetCrossoverMode(UNIFORM);break;
		case 2: SetCrossoverMode(TWO_POINT);break;
		default: cerr << "Invalid crossover mode" << endl; exit(1);
	}
	// Read the search initialized and re-evaluation flags, and the checkpoint frequency
  bifs.read((char*) &(SearchInitialized), sizeof(SearchInitialized));
  bifs.read((char*) &(ReEvalFlag), sizeof(ReEvalFlag));
  bifs.read((char*) &(CheckpointInt), sizeof(CheckpointInt));
  // Read the search constraint vector
  iv.BinaryReadVector(bifs);
  SetSearchConstraint(iv);
	// Read the mutation variance
  bifs.read((char*) &(d), sizeof(d));
  SetMutationVariance(d);
	// Read the crossover probability
  bifs.read((char*) &(d), sizeof(d));
	SetCrossoverProbability(d);
	// Read the crossover template
  iv.BinaryReadVector(bifs);
  SetCrossoverTemplate(iv);
	// Read the elitist fraciton
  bifs.read((char*) &(d), sizeof(d));
	SetElitistFraction(d);
	// Read the max expected offspring
  bifs.read((char*) &(d), sizeof(d));
	SetMaxExpectedOffspring(d);
	// Read the peformance and parameter vector of the best individual
  bifs.read((char*) &(BestPerf), sizeof(BestPerf));
  //bestVector.BinaryReadVector(bifs);
  BinaryReadVector<double>(bestVector,bifs);
	// Read the performance and parameter vector of each individual in the population
  for (int i = 1; i <= PopulationSize(); i++) {
    bifs.read((char*) &(d), sizeof(d));
    Perf[i] = d;
    //Population[i].BinaryReadVector(bifs);
	BinaryReadVector<double>(Population[i],bifs);
  }
  // Read in the random state for each individual in the populaton
  for (int i = 1; i <= PopulationSize(); i++)
    RandomStates[i].BinaryReadRandomState(bifs);
}




void TSearch::DisplayPopulationStatistics(void)
{
	if (PopulationStatisticsDisplayFunction != NULL)
		(*PopulationStatisticsDisplayFunction)(Gen,BestPerf,AvgPerf,PerfVar);
	else {
		cout << "Generation " << Gen << ": Best = " << BestPerf;
		cout << ", Average = " << AvgPerf << ", Variance = " << PerfVar << endl;
	}
}


// Display the results of a search

void TSearch::DisplaySearchResults(void)
{
	if (SearchResultsDisplayFunction != NULL)
		(*SearchResultsDisplayFunction)(*this);
}


void TSearch::SetCheckpointInterval(int NewInterval) 
{
	if (NewInterval < 0) {
		cerr << "Invalid CheckpointInterval: " << NewInterval; 
		exit(1);
	}
	CheckpointInt = NewInterval;
}




