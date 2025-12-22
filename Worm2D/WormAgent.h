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
class WormAgent : public Worm2Dbase, public WormGrad, public EvolvableS {
public:
	// The constructor
	//WormAgent();
	
	WormAgent(int newsize):
	Worm2Dbase({newsize,0,1,1,newsize}, new NervousSystem(), nullptr, make_shared<gradParameters>()),
	gradPars(dynamic_pointer_cast<gradParameters>(W2Dbaseparameters1b)),size(newsize)
	{InitialiseCircuit();}
	WormAgent(TVector<double> & v, int newsize):WormAgent(newsize){SetParameters(v);}
	WormAgent(int newsize, const char* fnm):WormAgent(newsize){SetWormParametersFromFile(fnm);}


	WormAgent(shared_ptr<const CmdArgs> cmd_):WormAgent(cmd_->getArgValInt("--network_size", 10))
	{setWormPars(cmd_);}
	WormAgent(const string & filename_, shared_ptr<const CmdArgs> cmd_):WormAgent(cmd_)
	{setParsFromFile(filename_);}




	// The destructor
	~WormAgent();

	// Accessors
	int CircuitSize(void) {return size;};
	//void SetCircuitSize(int news) {size = news;};
	void InitialiseCircuit();
	void zeroCircuit();
	virtual void SetWormParametersFromFile(const char* fnm);
	virtual double PositionX(void) const {return px;};
	void SetPositionX(double newx) {px = newx;};
	virtual double PositionY(void) const {return py;};
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
	double distanceToCenter(void) const;

	//double DistanceToCentre(void){return sqrt(pow(PositionX(),2) + pow(PositionY(),2));}	

	// Control
	void ResetChemCon();
	void UpdateChemCon();
	//void UpdateChemCon(double px_, double py_);
	void UpdateSensors();
	virtual void ResetAgentsBody();
	void ResetAgentIntState(RandomState &rs);
	virtual void SetParameters(const TVector<double> &v);
	//void InitialiseAgent(double runduration, double stepsize);
	void InitialiseAgent();
	void PrintDetail(ofstream &file);
	void PrintPath(ofstream &file);
	void StepOrig();

	//VMCO::TVector<double> chemConHistory;
	TVector<double> chemConHistory;
	double sensorN, sensorM;
	double dSensorN, dSensorM;
	int iSensorN, iSensorM;
	double tempDiff;

	TVector<double> w_ASER, w_ASEL;
	//VMCO::TVector<double> w_ASER, w_ASEL;
	double oASEL, oASER;
	double w_CPG_SMBV, w_CPG_SMBD;
	double avgvel,avgtheta;
	double pastTheta,DthetaDt;
	int timer;
	double NMdiff;
	double pushCurv;
	//VMCO::TVector<double> histCurv,histTheta;
	TVector<double> histCurv,histTheta;
	double px, py, vx, vy, orient, theta;

	double distanceToCentre;
	
	double CPGoffset, chemCon, pastCon, presentAvgCon, pastAvgCon, outputGain;
	const int size;
	int forward;

	shared_ptr<gradParameters> gradPars;

	int		VelDelta;  //		=	(int) (HST/StepSize);

	//using Worm2Dbase::Step;

	RandomState * rs = nullptr;

	//double gradSteep, orient_orig, RunDuration, HSStepSize;
	//int taxis, kinesis;


	//double sjadd;	
	//int sdqqq;

	//void writeData(){Worm2Dm::writeAct();Worm2Dm::writeState();}
	void addParsToJson(json & j);
	void initForSimulation(RandomState &rs_);//{InitializeSimulation(rs_);}


	//const vector<string> getCellNames() {return {"A","B"};}
	const string getModelName() {return {"CO"};}
	vector<doubIntParamsHead> getWormParams();
	void Step1();
	void preNStep();
	void postNStep();
	void moveAgent();
	void preNStep1();

	
	//void InitializeSimulation(RandomState &rs_);
	void InitializeState(RandomState &rs);

	//void setStepPars(double gradSteep_, RandomState &rs_, double t_, int taxis_, int kinesis_);
	void setSimParsDefault();
	void setSimPars(double orient_orig_,
	double gradSteep_, double RunDuration_, double HSStepSize_, int taxis_, int kinesis_);
	void DumpParams(ofstream &ofs){return;}
	double getVelocity(){return avgvel;}
	void writeBodyPos();
	void writeData();	
  	void setDistanceToCentre();
	void writeAct();


	void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
	void setEvolPars(W2Dparameters & w2par_, string evotype_){}
	void setParsFromPheno(const TVector<double> &pheno);
	void setWormPars(shared_ptr<const CmdArgs> cmd);
	int getVectSize();
};

