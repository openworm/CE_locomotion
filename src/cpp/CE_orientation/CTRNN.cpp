// ************************************************************
// A class for continuous-time recurrent neural networks
//
// RDB 
//  8/94 Created
//  12/98 Optimized integration
//  1/08 Added table-based fast sigmoid w/ linear interpolation
//
// To Do
//   1. Could optimize a bit further by making StepSize
//      a CTRNN parameter and prescaling RTaus and
//      by providing versions of EulerStep and RK4Step
//      that ignore gains, but it's not clear that the
//      few percent speed increase is really worth it.
// ************************************************************

#include "CTRNN.h"
#include "random.h"
#include <stdlib.h>

// A fast sigmoid implementation using a table w/ linear interpolation
#ifdef FAST_SIGMOID
int SigTableInitFlag = 0;
double SigTab[SigTabSize];

void InitSigmoidTable(void)
{
	if (!SigTableInitFlag) {
		double DeltaX = SigTabRange/(SigTabSize-1);
		for (int i = 0; i <= SigTabSize-1; i++)
			SigTab[i] = sigma(i * DeltaX);
		SigTableInitFlag = 1;
	}
}

double fastsigmoid(double x)
{
	if (x >= SigTabRange) return 1.0;
	if (x < 0) return 1.0 - fastsigmoid(-x);
	double id;
	double frac = modf(x*(SigTabSize-1)/SigTabRange, &id);
	int i = (int)id;
	double y1 = SigTab[i], y2 = SigTab[i+1];
	
	return y1 + (y2 - y1) * frac;
}
#endif

// **************************** 
// Constructors and Destructors
// ****************************

// The constructor

CTRNN::CTRNN(int newsize)
{
	SetCircuitSize(newsize);
#ifdef FAST_SIGMOID
	InitSigmoidTable();
#endif
}


// The destructor

CTRNN::~CTRNN()
{
	SetCircuitSize(0);
}


// *********
// Utilities
// *********

// Resize a circuit.

void CTRNN::SetCircuitSize(int newsize)
{
	size = newsize;
	states.SetBounds(1,size);
	states.FillContents(0.0);
	outputs.SetBounds(1,size);
	outputs.FillContents(0.0);
	biases.SetBounds(1,size);
	biases.FillContents(0.0);
	gains.SetBounds(1,size);
	gains.FillContents(1.0);
	taus.SetBounds(1,size);
	taus.FillContents(1.0);
	Rtaus.SetBounds(1,size);
	Rtaus.FillContents(1.0);
	externalinputs.SetBounds(1,size);
	externalinputs.FillContents(0.0);
	weights.SetBounds(1,size,1,size);
	weights.FillContents(0.0);
	junctions.SetBounds(1,size,1,size);
	junctions.FillContents(0.0);
	TempStates.SetBounds(1,size);
	TempOutputs.SetBounds(1,size);
	k1.SetBounds(1,size);
	k2.SetBounds(1,size);
	k3.SetBounds(1,size);
	k4.SetBounds(1,size);
}


// *******
// Control
// *******

// Randomize the states or outputs of a circuit.

void CTRNN::RandomizeCircuitState(double lb, double ub)
{
	for (int i = 1; i <= size; i++)
        SetNeuronState(i, UniformRandom(lb, ub));
}

void CTRNN::RandomizeCircuitState(double lb, double ub, RandomState &rs)
{
	for (int i = 1; i <= size; i++)
		SetNeuronState(i, rs.UniformRandom(lb, ub));
}

void CTRNN::RandomizeCircuitOutput(double lb, double ub)
{
	for (int i = 1; i <= size; i++)
        SetNeuronOutput(i, UniformRandom(lb, ub));
}

void CTRNN::RandomizeCircuitOutput(double lb, double ub, RandomState &rs)
{
	for (int i = 1; i <= size; i++)
		SetNeuronOutput(i, rs.UniformRandom(lb, ub));
}



// Integrate a circuit one step using Euler integration.

void CTRNN::EulerStep(double stepsize)
{
	// Update the state of all neurons.
//#pragma omp parallel private(input) shared(stepsize, externalinputs, Rtaus, weights, outputs, states) // XXX OpenMPI 
//	{
//#pragma omp for schedule(dynamic) nowait
		for (int i = 1; i <= size; i++) {
			double input = externalinputs[i];
			for (int j = 1; j <= size; j++) 
				input += weights[j][i] * outputs[j];
			
			for (int j = 1; j <= size; j++) {
				input += junctions[j][i] * (states[j] - states[i]);
			}
			states[i] += stepsize * Rtaus[i] * (input - states[i]);

            
		}
		// Update the outputs of all neurons.
		for (int i = 1; i <= size; i++)
			outputs[i] = sigmoid(gains[i] * (states[i] + biases[i]));
//	}
}


// Integrate a circuit one step using 4th-order Runge-Kutta.
/* ELECTRICAL JUNCTIONS NOT IMPLEMENTED XXX
void CTRNN::RK4Step(double stepsize)
{
	int i,j;
	double input;
	
	// The first step.
	for (i = 1; i <= size; i++) {
		input = externalinputs[i];
		for (j = 1; j <= size; j++)
			input += weights[j][i] * outputs[j];
		k1[i] = stepsize * Rtaus[i] * (input - states[i]); 
		TempStates[i] = states[i] + 0.5*k1[i];
		TempOutputs[i] = sigmoid(gains[i]*(TempStates[i]+biases[i]));
	}
	
	// The second step.
	for (i = 1; i <= size; i++) {
		input = externalinputs[i];
		for (j = 1; j <= size; j++)
			input += weights[j][i] * TempOutputs[j];
		k2[i] = stepsize * Rtaus[i] * (input - TempStates[i]);
		TempStates[i] = states[i] + 0.5*k2[i];
	}
	for (i = 1; i <= size; i++)
		TempOutputs[i] = sigmoid(gains[i]*(TempStates[i]+biases[i]));
	
	// The third step.
	for (i = 1; i <= size; i++) {
		input = externalinputs[i];
		for (j = 1; j <= size; j++)
			input += weights[j][i] * TempOutputs[j];
		k3[i] = stepsize * Rtaus[i] * (input - TempStates[i]);
		TempStates[i] = states[i] + k3[i];
	}
	for (i = 1; i <= size; i++)
		TempOutputs[i] = sigmoid(gains[i]*(TempStates[i]+biases[i]));
	
	// The fourth step.
	for (i = 1; i <= size; i++) {
		input = externalinputs[i];
		for (j = 1; j <= size; j++)
			input += weights[j][i] * TempOutputs[j];
		k4[i] = stepsize * Rtaus[i] * (input - TempStates[i]);
		states[i] += (1.0/6.0)*k1[i] + (1.0/3.0)*k2[i] + (1.0/3.0)*k3[i] + (1.0/6.0)*k4[i];
		outputs[i] = sigmoid(gains[i]*(states[i]+biases[i]));
	}
}
*/

// Set the biases of the CTRNN to their center-crossing values

void CTRNN::SetCenterCrossing(void)
{
    double InputWeights, ThetaStar;
    
    for (int i = 1; i <= CircuitSize(); i++) {
        // Sum the input weights to this neuron
        InputWeights = 0;
        for (int j = 1; j <= CircuitSize(); j++)
            InputWeights += ConnectionWeight(j, i);
        // Compute the corresponding ThetaStar
        ThetaStar = -InputWeights/2;
        SetNeuronBias(i, ThetaStar);
    }
}


// ****************
// Input and Output
// ****************

#include <iomanip>

ostream& operator<<(ostream& os, CTRNN& c)
{
	// Set the precision
	os << setprecision(32);
	// Write the size
	os << c.size << endl << endl;
	// Write the time constants
	for (int i = 1; i <= c.size; i++)
		os << c.taus[i] << " ";
	os << endl << endl;
	// Write the biases
	for (int i = 1; i <= c.size; i++)
		os << c.biases[i] << " ";
	os << endl << endl;
	// Write the gains
	for (int i = 1; i <= c.size; i++)
		os << c.gains[i] << " ";
	os << endl << endl;
	// Write the chemical synaptic weights
	for (int i = 1; i <= c.size; i++) {
		for (int j = 1; j <= c.size; j++)
			os << c.weights[i][j] << " ";
		os << endl;
	}
	// Write the electrical junctions
	for (int i = 1; i <= c.size; i++) {
		for (int j = 1; j <= c.size; j++)
			os << c.junctions[i][j] << " ";
		os << endl;
	}
	// Return the ostream
	return os;
}

istream& operator>>(istream& is, CTRNN& c)
{
	// Read the size
	int size;
	is >> size;
	c.SetCircuitSize(size);
	// Read the time constants
	for (int i = 1; i <= size; i++) {
		is >> c.taus[i];
		c.Rtaus[i] = 1/c.taus[i];
	}
	// Read the biases
	for (int i = 1; i <= size; i++)
		is >> c.biases[i];
	// Read the gains
	for (int i = 1; i <= size; i++)
		is >> c.gains[i];
	// Read the chemical synaptic weights
	for (int i = 1; i <= size; i++)
		for (int j = 1; j <= size; j++)
			is >> c.weights[i][j];
	// Read the electrical junctions
	for (int i = 1; i <= size; i++)
		for (int j = 1; j <= size; j++)
			is >> c.junctions[i][j];
	// Return the istream		
	return is;
}		

