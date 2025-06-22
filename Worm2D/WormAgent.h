#pragma once
//#include "CTRNN.h"
#include "Worm2D.h"

//using namespace CTRNNspace;

// Global constants
//const double	StepSize		=	0.01;			// Fastest time-constant is now 0.1 (10ms)
const double	Pi				=	3.1415926;
const double	MaxDist			=	4.5;			// Half the radius of the big petri dish (in cm)
const double	MaxVel			=	0.022;			// Forward velocity (in cm/s) CHECK WHAT THE REAL VELOCITY SHOULD BE
const double	MaxGauGradHeight =	1.0;			// Because the MaxDist is also the maximum height of the cone shaped gradient
const double	ChemDiffConst	=	2*pow(1.61,2);	// Simulated chemical environment according to Ward, 1973 as described in Ferree and Lockery 1999 equation 14.
const double	HST				=	4.2;			// Head Sweep Time, T=4.2sec, According to Ferree, Marcotte, Lockery, 1997.
const double	HSP				=	(2*Pi)/HST;		// Head-sweep period 2*Pi/T, According to Ferree, Marcotte, Lockery, 1997.
//int		VelDelta;  //		=	(int) (HST/StepSize);
	

// The WormAgent class declaration
class WormAgent : public Worm2Dbase {
public:
	// The constructor
	WormAgent(TVector<double> &v, int newsize); // Construct from phenotype
	WormAgent(int newsize = 0);	// Construct from evolutionary algorithm
	WormAgent(int newsize, const char* fnm);	// Construct from file
	// The destructor
	~WormAgent();

	// Accessors
	int CircuitSize(void) {return size;};
	void SetCircuitSize(int news) {size = news;};
	void InitialiseCircuit(int newcs);
	void SetWormParametersFromFile(int newsize, const char* fnm);
	double PositionX(void) {return px;};
	void SetPositionX(double newx) {px = newx;};
	double PositionY(void) {return py;};
	void SetPositionY(double newy) {py = newy;};
	double VelocityX(void) {return vx;};
	void SetVelocityX(double newx) {vx = newx;};
	double VelocityY(void) {return vy;};
	void SetVelocityY(double newy) {vy = newy;};
	double Orientation(void) {return orient;};
	void SetOrientation(double newo) {orient = newo;};
	double OffsetCPG(void) {return CPGoffset;};
	void SetOffsetCPG(double newo) {CPGoffset = newo;};
	double ChemCon(void) {return chemCon;};
	void SetChemCon(double newc) {chemCon = newc;};
	double OutputGain(void) {return outputGain;};
	void SetOutputGain(double newdc) {outputGain = newdc;};
	double DistanceToCentre(void) {return distanceToCentre;};

	// Control
	void ResetChemCon();
	void UpdateChemCon();

	void UpdateSensors();
	void ResetAgentsBody();
	void ResetAgentIntState(RandomState &rs);
	void SetParameters(TVector<double> &v);
	//void InitialiseAgent(double runduration, double stepsize);
	void InitialiseAgent();
	void PrintDetail(ofstream &file);
	void PrintPath(ofstream &file);
	void StepOrig(double StepSize);

	VMCO::TVector<double> chemConHistory;
	double sensorN, sensorM;
	double dSensorN, dSensorM;
	int iSensorN, iSensorM;
	double tempDiff;

	VMCO::TVector<double> w_ASER, w_ASEL;
	double oASEL, oASER;
	double w_CPG_SMBV, w_CPG_SMBD;
	double avgvel,avgtheta;
	double pastTheta,DthetaDt;
	int timer;
	double NMdiff;
	double pushCurv;
	VMCO::TVector<double> histCurv,histTheta;
	double px, py, vx, vy, orient, distanceToCentre, theta;
	double CPGoffset, chemCon, pastCon, presentAvgCon, pastAvgCon, outputGain;
	int size;
	int forward;


	int		VelDelta;  //		=	(int) (HST/StepSize);

	using Worm2Dbase::Step;
	RandomState rs;
	double gradSteep, orient_orig, RunDuration, HSStepSize;
	int taxis, kinesis;
	//double sjadd;	
	//int sdqqq;

	//void writeData(){Worm2Dm::writeAct();Worm2Dm::writeState();}
	void addParsToJson(json & j);
	void initForSimulation(){return;}


	//const vector<string> getCellNames() {return {"A","B"};}
	const string getModelName() {return {"CO"};}
	vector<doubIntParamsHead> getWormParams();
	void Step1(double Stepsize);
	void preNStep(double StepSize);
	void postNStep(double StepSize);
	void moveAgent(double StepSize);

	void InitializeState(RandomState &rs);

	//void setStepPars(double gradSteep_, RandomState &rs_, double t_, int taxis_, int kinesis_);
	
	void setSimPars(double orient_orig_,
	double gradSteep_, double RunDuration_, double HSStepSize_, int taxis_, int kinesis_);
	void DumpParams(ofstream &ofs){return;}
	double getVelocity(){return avgvel;}
	void writeBodyPos();
	void writeData();	
  	

};
