#pragma once

#define THREADED_SEARCH
#define THREAD_COUNT 16

#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include "VectorMatrix.h"
#include "random.h"

#ifdef THREADED_SEARCH
  #include <pthread.h>
#endif

// =============================
// Utility functions
// =============================

inline double clip(double x, double min, double max)
{
    double temp = (x > min) ? x : min;
    return (temp < max) ? temp : max;
}

// The minimum and maximum allowable parameter values

constexpr double MinSearchValue = -1.0;
constexpr double MaxSearchValue =  1.0;

// Map from search parameters to model parameters

inline double MapSearchParameter(double x, double min, double max,
                                 double clipmin = -1.0e99,
                                 double clipmax =  1.0e99)
{
    double m = (max - min) / (MaxSearchValue - MinSearchValue);
    double b = min - m * MinSearchValue;
    return clip(m * x + b, clipmin, clipmax);
}

// Map from model parameters to search parameters

inline double InverseMapSearchParameter(double x, double min, double max)
{
    double m = (MaxSearchValue - MinSearchValue) / (max - min);
    double b = MinSearchValue - m * min;
    return m * x + b;
}

// =============================
// TSearch class declaration
// =============================

enum TSelectionMode { FITNESS_PROPORTIONATE, RANK_BASED };
enum TReproductionMode { HILL_CLIMBING, GENETIC_ALGORITHM };
enum TCrossoverMode { UNIFORM, TWO_POINT };

class TSearch {
public:
    // Constructor / Destructor
    TSearch(int vectorSize = 0,
            double (*EvalFn)(TVector<double>&, RandomState&) = nullptr);
    ~TSearch();

    // Basic accessors
    int VectorSize() const { return vectorSize; }
    void SetVectorSize(int NewSize);
    void SetRandomSeed(long seed) { rs.SetRandomSeed(seed); }

    // Search mode accessors
    TSelectionMode SelectionMode() const { return SelectMode; }
    void SetSelectionMode(TSelectionMode newmode) { SelectMode = newmode; }

    TReproductionMode ReproductionMode() const { return RepMode; }
    void SetReproductionMode(TReproductionMode newmode) { RepMode = newmode; }

    TCrossoverMode CrossoverMode() const { return CrossMode; }
    void SetCrossoverMode(TCrossoverMode newmode) { CrossMode = newmode; }

    // Search parameter accessors
    int PopulationSize() const { return static_cast<int>(Population.size()); }
    void SetPopulationSize(int NewSize);

    int MaxGenerations() const { return MaxGens; }
    void SetMaxGenerations(int NewMax);

    double ElitistFraction() const { return EFraction; }
    void SetElitistFraction(double NewFraction);

    double MaxExpectedOffspring() const { return MaxExpOffspring; }
    void SetMaxExpectedOffspring(double NewVal);

    double MutationVariance() const { return MutationVar; }
    void SetMutationVariance(double NewVariance);

    double CrossoverProbability() const { return CrossProb; }
    void SetCrossoverProbability(double NewProb);

   
    void SetSearchConstraint(int Flag);

    int ReEvaluationFlag() const { return ReEvalFlag; }
    void SetReEvaluationFlag(int flag) { ReEvalFlag = flag; }

    int CheckpointInterval() const { return CheckpointInt; }
    void SetCheckpointInterval(int NewFreq);

    // Function pointer accessors
    //void SetEvaluationFunction(
    //    double (*EvalFn)(std::vector<double>&, RandomState&))
    //{ EvaluationFunction = EvalFn; }

    void SetEvaluationFunction(
        double (*EvalFn)(TVector<double>&, RandomState&))
    { EvaluationFunction = EvalFn; }

    void SetBestActionFunction(
        void (*BestFn)(int, std::vector<double>&))
    { BestActionFunction = BestFn; }

    void SetPopulationStatisticsDisplayFunction(
        void (*DisplayFn)(int, double, double, double))
    { PopulationStatisticsDisplayFunction = DisplayFn; }

    void SetSearchTerminationFunction(
        int (*TerminationFn)(int, double, double, double))
    { SearchTerminationFunction = TerminationFn; }

    void SetSearchResultsDisplayFunction(
        void (*DisplayFn)(TSearch&))
    { SearchResultsDisplayFunction = DisplayFn; }

    // Status accessors
    int Generation() const { return Gen; }

   
    TVector<double>& IndividualT(int i)
    {
        individualT.SetBounds(1,vectorSize);
        for (int j=1;j<=vectorSize;j++) individualT(j)=Individual(i-1)[j-1];
        return individualT;
    }

   
    double FitnessT(int i) const { return fitness.at(i-1); }
    double PerformanceT(int i) const { return Perf.at(i-1); }

    double BestPerformance() const { return BestPerf; }
   
    TVector<double>& BestIndividualT()
    {
        bestVectorT.SetBounds(1,vectorSize);
        for (int i=1;i<=vectorSize;i++) bestVectorT(i)=BestIndividual()[i-1];
        return bestVectorT;
    }

    // Control
    void InitializeSearch();
    void ExecuteSearch();
    void ResumeSearch();

    // Input / Output
    void WriteCheckpointFile();
    void ReadCheckpointFile();

    std::string cptfilename;

    friend class Evolution;
    friend class EvoBase;

    void DoSearch(int ResumeFlag);

private:
     std::vector<int>& CrossoverTemplate() { return crossTemplate; }
    //void SetCrossoverTemplate(const std::vector<int>& NewTemplate);

    std::vector<int>& CrossoverPoints() { return crossPoints; }
    //void SetCrossoverPoints(const std::vector<int>& NewPoints);

    std::vector<int>& SearchConstraint() { return ConstraintVector; }
    //void SetSearchConstraint(const std::vector<int>& Constraint);

    std::vector<double>& BestIndividual(){ return bestVector; }
    double Fitness(int i) const { return fitness.at(i); }
    double Performance(int i) const { return Perf.at(i); }
    std::vector<double>& Individual(int i) { return Population.at(i); }

    // Helper methods
    int EqualVector(const std::vector<double>& v1,
                    const std::vector<double>& v2) const
    {
        return v1 == v2;
    }

    void RandomizeVector(std::vector<double>& Vector);
    void RandomizePopulation();

    double EvaluateVector(std::vector<double>& Vector, RandomState& rs);
    friend void* EvaluatePopulationRange(void* arg);
    void EvaluatePopulation(size_t start = 0);

    void SortPopulation();
    void UpdatePopulationFitness();
    void ReproducePopulationHillClimbing();
    void ReproducePopulationGeneticAlgorithm();

    void MutateVector(std::vector<double>& Vector);
    void UniformCrossover(std::vector<double>& v1,
                          std::vector<double>& v2);
    void TwoPointCrossover(std::vector<double>& v1,
                           std::vector<double>& v2);

    //void PrintPopulationStatistics();
    void ReproducePopulation();
    void UpdatePopulationStatistics();
    void DisplayPopulationStatistics();
    int SearchTerminated();
    void DisplaySearchResults();

    // Internal state
    RandomState rs;
    std::vector<RandomState> RandomStates;

    int Gen = 0;
    int SearchInitialized = 0;

    std::vector<std::vector<double>> Population;
    std::vector<double> Perf;
    std::vector<double> fitness;

    int UpdateBestFlag = 0;
    std::vector<double> bestVector;
    TVector<double> bestVectorT, individualT;

    double BestPerf = 0.0;
    double MinPerf  = 0.0;
    double MaxPerf  = 0.0;
    double AvgPerf  = 0.0;
    double PerfVar  = 0.0;

    // Search modes
    TSelectionMode SelectMode = FITNESS_PROPORTIONATE;
    TReproductionMode RepMode = HILL_CLIMBING;
    TCrossoverMode CrossMode = UNIFORM;

    // Search parameters
    int vectorSize = 0;
    int MaxGens = 0;
    double EFraction = 0.0;
    double MaxExpOffspring = 0.0;
    double MutationVar = 0.0;
    double CrossProb = 0.0;

    std::vector<int> crossTemplate;
    std::vector<int> crossPoints;
    std::vector<int> ConstraintVector;

    int ReEvalFlag = 0;
    int CheckpointInt = 0;

    // Function pointers
    //double (*EvaluationFunction)(std::vector<double>&, RandomState&) = nullptr;
    double (*EvaluationFunction)(TVector<double>&, RandomState&) = nullptr;
    void (*BestActionFunction)(int, std::vector<double>&) = nullptr;
    void (*PopulationStatisticsDisplayFunction)(int, double, double, double) = nullptr;
    int (*SearchTerminationFunction)(int, double, double, double) = nullptr;
    void (*SearchResultsDisplayFunction)(TSearch&) = nullptr;
};

// Range specification for threaded evaluation

struct PopRangeSpec {
    TSearch* search;
    size_t start;
    size_t end;
};