#include "WormAgent.h"
#include "TSearch.h"
#include "CTRNN.h"
#include "random.h"

// ****************************
// Constructors and Destructors
// ****************************

// The constructor
WormAgent::WormAgent(int newsize)
{
	InitialiseCircuit(newsize);
}

WormAgent::WormAgent(int newsize, const char* fnm)
{
	SetWormParametersFromFile(newsize, fnm);  //Call to initialise sensors within
}

// The destructor
WormAgent::~WormAgent()
{
	InitialiseCircuit(0);
}

// *********
// Setting parameters
// *********
void WormAgent::SetWormParametersFromFile(int newsize, const char* fnm)
{
	ifstream BestIndividualFile;

	// parameters from file
	BestIndividualFile.open(fnm);

	// sensory connections
	for (int i = 1; i <= newsize-4; i++){
		BestIndividualFile >> w_ASER[i];
	}
	for (int i = 1; i <= newsize-4; i++){
		BestIndividualFile >> w_ASEL[i];
	}

	// CPG connections
	BestIndividualFile >> w_CPG_SMBV;
	BestIndividualFile >> w_CPG_SMBD;

	BestIndividualFile >> sensorN;
	BestIndividualFile >> sensorM;

	BestIndividualFile >> outputGain;

	BestIndividualFile >> NervousSystem;
	size = NervousSystem.CircuitSize();

	BestIndividualFile.close();
}

void WormAgent::SetParameters(TVector<double> &v)
{
	// sensory connections
	int k = 1;
	for (int i = 1; i <= size-4; i++){
		w_ASER[i] = v(k);
		k++;
	}
	for (int i = 1; i <= size-4; i++){
		w_ASEL[i] = v(k);
		k++;
	}

	// Chemical synapse weights between the N CTRNN nodes
	for (int i = 1; i <= size-4; i++){
		for (int j = 1; j <= size-4; j++){
			NervousSystem.SetConnectionWeight(i, j, v(k));
			k++;
		}
	}

	// Chemical synapses from interneurons to F/B  and between F/B motor neurons
	for (int i = 1; i <= size-2; i++){
		for (int j = size-3; j <= size-2; j++){
			NervousSystem.SetConnectionWeight(i, j, v(k));
			k++;
		}
	}

	// // Chemical synapses from interneurons to D/V with DV symmetry
	for (int i = 1; i <= size-4; i++){
			NervousSystem.SetConnectionWeight(i, size, v(k));
			NervousSystem.SetConnectionWeight(i, size-1, v(k));
			k++;
	}

	// Self-connections for the DV motorneurons
	NervousSystem.SetConnectionWeight(size, size, v(k));
	NervousSystem.SetConnectionWeight(size-1, size-1, v(k));
	k++;

	// Biases
	for (int i = 1; i <= size-2; i++){
		NervousSystem.SetNeuronBias(i, v(k));
		k++;
	}
	NervousSystem.SetNeuronBias(size, v(k));
	NervousSystem.SetNeuronBias(size-1, v(k));
	k++;

	// Time-constants
	for (int i = 1; i <= size-2; i++){
		NervousSystem.SetNeuronTimeConstant(i, v(k));
		k++;
	}
	NervousSystem.SetNeuronTimeConstant(size, v(k));
	NervousSystem.SetNeuronTimeConstant(size-1, v(k));
	k++;

	// conections from CPG to motor neurons
	w_CPG_SMBV = v(k);
	w_CPG_SMBD = v(k);
	k++;

	// Sensors
	sensorN = v(k);
	k++;
	sensorM = v(k);
	k++;

	// output gain between motor neurons and muscles
	outputGain = v(k);
}

// **************
// Initialising
// **************
void WormAgent::InitialiseCircuit(int CircuitSize)
{
	size = CircuitSize;
	NervousSystem.SetCircuitSize(size);
	w_ASER.SetBounds(1, size);
	w_ASER.FillContents(0.0);
	w_ASEL.SetBounds(1, size);
	w_ASEL.FillContents(0.0);
	forward = 1;
}

void WormAgent::InitialiseAgent(double runduration, double stepsize)
{
	iSensorN = (int) (sensorN/stepsize);
	dSensorN = (double) iSensorN;
	iSensorM = (int) (sensorM/stepsize);
	dSensorM = (double) iSensorM;
	int upperbound = ((int) ((runduration + sensorN + sensorM) / stepsize)) + 1;
	chemConHistory.SetBounds(1, upperbound);
	chemConHistory.FillContents(0.0);
	histCurv.SetBounds(1, VelDelta);
	histCurv.FillContents(0.0);
	histTheta.SetBounds(1, VelDelta);
	histTheta.FillContents(0.0);
}

// *******
// Resetting
// *******

void WormAgent::ResetAgentsBody(double neworient, RandomState &rs)
{
	distanceToCentre = -MaxDist;
	double tempangle = 0.0;
	px = cos(tempangle) * distanceToCentre;
	py = sin(tempangle) * distanceToCentre;
	vx = 0.0;
	vy = 0.0;
	theta = 0.0;
	orient = neworient;
	CPGoffset = 0.0;
	forward = 1;
}

void WormAgent::ResetChemCon(double gradSteep)
{
	chemCon = -distanceToCentre * gradSteep;
	pastCon = chemCon;
	timer = iSensorN + iSensorM + 1;
	for (int i = 1; i <= timer; i++)
		chemConHistory(i) = chemCon;
	presentAvgCon = chemCon * iSensorN;
	pastAvgCon = chemCon * iSensorM;
}

void WormAgent::ResetAgentIntState(RandomState &rs)
{
	NervousSystem.RandomizeCircuitState(0.0, 0.0, rs);
}

// *******
// Updating
// *******
void WormAgent::UpdateChemCon(double gradSteep)
{
	distanceToCentre = sqrt(pow(px,2) + pow(py,2));
	pastCon = chemCon;
	chemCon = -distanceToCentre * gradSteep;
	chemConHistory(timer) = chemCon;
	timer += 1;
}

void WormAgent::UpdateSensors()
{
	presentAvgCon += chemConHistory(timer - 1) - chemConHistory(timer - iSensorN - 1);
	pastAvgCon += chemConHistory(timer - iSensorN - 1) - chemConHistory(timer - iSensorN - iSensorM - 1);
	tempDiff = (presentAvgCon/dSensorN) - (pastAvgCon/dSensorM);
	oASEL = tempDiff > 0.0 ? tempDiff: 0.0;
	oASER = tempDiff < 0.0 ? fabs(tempDiff): 0.0;
}

void WormAgent::PrintPath( ofstream &file)
{
	file << px << " ";
	file << py << " ";
	file << endl;
}

void WormAgent::PrintDetail( ofstream &file)
{
	file << px << " ";
	file << py << " ";
	file << theta << " ";
	file << orient << " ";
	file << avgvel << " ";
	file << oASEL << " ";
	file << oASER << " ";
	for (int i = 1; i <= size; i++){
		file << NervousSystem.NeuronOutput(i) << " ";
	}
	file << avgtheta << " ";
	// file << pastAvgCon << " ";
	file << endl;
}

// Step
void WormAgent::Step(double StepSize, RandomState &rs, double timestep, int taxis, int kinesis)
{
	// Input from sensory neurons to interneurons
	for (int i = 1; i <= size-4; i++){
			NervousSystem.SetNeuronExternalInput(i, w_ASEL[i] * oASEL + w_ASER[i] * oASER);
	}

	// Add antiphase oscillatory input to the neck motor neurons
	NervousSystem.SetNeuronExternalInput(size-1, w_CPG_SMBV * sin(CPGoffset + HSP*timestep));
	NervousSystem.SetNeuronExternalInput(size, w_CPG_SMBD * sin(CPGoffset + Pi + HSP*timestep));

	// Update the signaling system
	NervousSystem.EulerStep(StepSize);

	// Update curvature
	if (taxis == 1){
		NMdiff = NervousSystem.NeuronOutput(size-1) - NervousSystem.NeuronOutput(size);
		theta = outputGain * NMdiff;
		orient += StepSize * theta;

		// Check that there is always one part of the curvature that is convex and another that is concave. This way thrust is generated.
		// Maintain a cumulative average of absolute theta and ensure that it is not too low (otherwise it's going straightish)
		DthetaDt = theta - pastTheta;
		pastTheta = theta;
		pushCurv = DthetaDt > 0 ? StepSize : -StepSize;
		avgtheta = histTheta.PushFront(fabs(theta))/VelDelta;
		avgvel = MaxVel * pow(1 - (fabs(histCurv.PushFront(pushCurv))/HST), 2) * pow(1-fabs(0.7-avgtheta),2);
	}

	// Update Forward -> Backward
	if (kinesis == 1){
		if ((forward == 1) && (NervousSystem.NeuronOutput(size-2) > 0.6) && (NervousSystem.NeuronOutput(size-3) < 0.4) )
		{
			forward = 0;
		}
		// Update Backward -> Forward
		if ((forward == 0) && (NervousSystem.NeuronOutput(size-2) < 0.4) && (NervousSystem.NeuronOutput(size-3) > 0.6) )
		{
			forward = 1;
			orient = rs.UniformRandom(0, 2*Pi);
		}
		if (forward == 0)
			{avgvel = -MaxVel;}
		else
			{avgvel = MaxVel;}
 	}

	// Update the velocity
	vx = cos(orient) * avgvel;
	vy = sin(orient) * avgvel;

	// Move the agent
	px += StepSize * vx;
	py += StepSize * vy;
}
