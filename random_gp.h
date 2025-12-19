// ======================= random.h =======================
#pragma once

#include <vector>
#include <fstream>
#include <ostream>
#include <istream>
#include "VectorMatrix.h"

// Numerical Recipes ran1 constants
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

// Backward-compatible global RNG API
void SetRandomSeed(long seed);
long GetRandomSeed(void);
void WriteRandomState(std::ostream& os);
void BinaryWriteRandomState(std::ofstream& bofs);
void ReadRandomState(std::istream& is);
void BinaryReadRandomState(std::ifstream& bifs);
double UniformRandom(double min,double max);
int UniformRandomInteger(int min,int max);
double GaussianRandom(double mean, double variance);
void RandomUnitVector(std::vector<double>& v);
int ProbabilisticChoice(double prob);
void RandomUnitVector(TVector<double> &v);

class RandomState {
public:
    explicit RandomState(long seed = 0);
    ~RandomState() = default;

    void SetRandomSeed(long seed);
    long GetRandomSeed(void);

    double UniformRandom(double min,double max);
    int UniformRandomInteger(int min,int max);
    double GaussianRandom(double mean, double variance);
    void RandomUnitVector(std::vector<double>& v);
    int ProbabilisticChoice(double prob);
    void RandomUnitVector(TVector<double> &v);

    void WriteRandomState(std::ostream& os);
    void BinaryWriteRandomState(std::ofstream& bofs);
    void ReadRandomState(std::istream& is);
    void BinaryReadRandomState(std::ifstream& bifs);

private:
    double ran1(void);
    void GenerateNormals(void);

    long seed = 0;
    long idum = 0;
    long iy = 0;
    long iv[NTAB]{};

    int gaussian_flag = 0;
    double gX1 = 0.0, gX2 = 0.0;
};

