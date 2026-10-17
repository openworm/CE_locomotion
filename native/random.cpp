
// ======================= random.cpp =======================
#include "random.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

// Global RNG instance
static RandomState GRS;

// Global wrappers
void SetRandomSeed(long seed) { GRS.SetRandomSeed(seed); }
long GetRandomSeed(void) { return GRS.GetRandomSeed(); }
void WriteRandomState(std::ostream& os) { GRS.WriteRandomState(os); }
void BinaryWriteRandomState(std::ofstream& bofs) { GRS.BinaryWriteRandomState(bofs); }
void ReadRandomState(std::istream& is) { GRS.ReadRandomState(is); }
void BinaryReadRandomState(std::ifstream& bifs) { GRS.BinaryReadRandomState(bifs); }
double UniformRandom(double min,double max) { return GRS.UniformRandom(min,max); }
int UniformRandomInteger(int min,int max) { return GRS.UniformRandomInteger(min,max); }
double GaussianRandom(double mean, double variance) { return GRS.GaussianRandom(mean,variance); }
void RandomUnitVector(std::vector<double>& v) { GRS.RandomUnitVector(v); }
int ProbabilisticChoice(double prob) { return GRS.ProbabilisticChoice(prob); }
void RandomUnitVector(TVector<double> &v) {GRS.RandomUnitVector(v);};

RandomState::RandomState(long s) {
    SetRandomSeed(s);
    gaussian_flag = 0;
}

double RandomState::ran1(void)
{
    if (!iy) SetRandomSeed(1);
    long k = idum / IQ;
    idum = IA * (idum - k * IQ) - IR * k;
    if (idum < 0) idum += IM;
    int j = iy / NDIV;
    iy = iv[j];
    iv[j] = idum;
    double temp = AM * iy;
    return (temp > RNMX) ? RNMX : temp;
}

void RandomState::SetRandomSeed(long s)
{
    idum = seed = s;
    if (idum == 0) idum = 1;
    for (int j = NTAB + 7; j >= 0; --j) {
        long k = idum / IQ;
        idum = IA * (idum - k * IQ) - IR * k;
        if (idum < 0) idum += IM;
        if (j < NTAB) iv[j] = idum;
    }
    iy = iv[0];
}

long RandomState::GetRandomSeed(void) { return seed; }

void RandomState::BinaryWriteRandomState(std::ofstream& bofs)
{
    bofs.write((const char*)&seed, sizeof(seed));
    bofs.write((const char*)&idum, sizeof(idum));
    bofs.write((const char*)&iy, sizeof(iy));
    bofs.write((const char*)&gaussian_flag, sizeof(gaussian_flag));
    bofs.write((const char*)&gX1, sizeof(gX1));
    bofs.write((const char*)&gX2, sizeof(gX2));
    for (int i = 0; i < NTAB; ++i)
        bofs.write((const char*)&iv[i], sizeof(iv[i]));
}

void RandomState::WriteRandomState(std::ostream& os)
{
    os << seed << " " << idum << " " << iy << " "
       << gaussian_flag << " " << gX1 << " " << gX2 << " ";
    for (int i = 0; i < NTAB; ++i) os << iv[i] << " ";
    os << '\n';
}

void RandomState::ReadRandomState(std::istream& is)
{
    is >> seed >> idum >> iy >> gaussian_flag >> gX1 >> gX2;
    for (int i = 0; i < NTAB; ++i) is >> iv[i];
}

void RandomState::BinaryReadRandomState(std::ifstream& bifs)
{
    bifs.read((char*)&seed, sizeof(seed));
    bifs.read((char*)&idum, sizeof(idum));
    bifs.read((char*)&iy, sizeof(iy));
    bifs.read((char*)&gaussian_flag, sizeof(gaussian_flag));
    bifs.read((char*)&gX1, sizeof(gX1));
    bifs.read((char*)&gX2, sizeof(gX2));
    for (int i = 0; i < NTAB; ++i)
        bifs.read((char*)&iv[i], sizeof(iv[i]));
}

double RandomState::UniformRandom(double min,double max)
{
    return (max - min) * ran1() + min;
}

int RandomState::UniformRandomInteger(int min,int max)
{
    return static_cast<int>(std::floor(0.5 + UniformRandom(min - 0.5, max + 0.5)));
}

void RandomState::GenerateNormals(void)
{
    double v1, v2, s;
    do {
        v1 = UniformRandom(-1.0, 1.0);
        v2 = UniformRandom(-1.0, 1.0);
        s = v1 * v1 + v2 * v2;
    } while (s >= 1.0 || s == 0.0);

    double d = std::sqrt((-2.0 * std::log(s)) / s);
    gX1 = v1 * d;
    gX2 = v2 * d;
}

double RandomState::GaussianRandom(double mean,double variance)
{
    if (!gaussian_flag) {
        GenerateNormals();
        gaussian_flag = 1;
        return std::sqrt(variance) * gX1 + mean;
    } else {
        gaussian_flag = 0;
        return std::sqrt(variance) * gX2 + mean;
    }
}

void RandomState::RandomUnitVector(std::vector<double>& v)
{
    double r = 0.0;
    for (double& x : v) {
        x = GaussianRandom(0.0, 1.0);
        r += x * x;
    }
    r = std::sqrt(r);
    for (double& x : v) x /= r;
}

int RandomState::ProbabilisticChoice(double prob)
{
    return (UniformRandom(0.0,1.0) <= prob) ? 1 : 0;
}

void RandomState::RandomUnitVector(TVector<double> &v)
{
	double r = 0.0;

	for (int i = v.LowerBound(); i <= v.UpperBound(); i++)
	{
		v[i] = GaussianRandom(0,1);
		r += v[i] * v[i];
	}
	r = sqrt(r);
	for (int i = v.LowerBound(); i <= v.UpperBound(); i++)
		v[i] = v[i] / r;
}