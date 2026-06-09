#include "WormAgent.h"
#include "TSearchCO.h"
//#include "CTRNN.h"
//#include "random.h"

// ****************************
// Constructors and Destructors
// ****************************

//using namespace CTRNNspace;



/* WormAgent::WormAgent(int newsize, const char* fnm):
Worm2Dbase({newsize,0,1,1,newsize}, new NervousSystem(), 0, make_shared<gradParameters>()),
gradPars(dynamic_pointer_cast<gradParameters>(W2Dbaseparameters1)),size(newsize)
{
	//setSimParsDefault();
	InitialiseCircuit();
	SetWormParametersFromFile(newsize, fnm);  //Call to initialise sensors within
} */


// The destructor
WormAgent::~WormAgent()
{
	wa_rs = nullptr;
	zeroCircuit();
	//InitialiseCircuit(0);
}



// *********
// Setting parameters
// *********

int WormAgent::getVectSize()
{

return 2*(size-4) + (size-4)*(size-4) 
+ (size-2)*2 + (size-4) + 1 + 2*((size-2) + 1) + 4;

}

void WormAgent::SetWormParametersFromFile(const char* fnm)
{
	ifstream BestIndividualFile;
	NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);

	// parameters from file
	BestIndividualFile.open(fnm);

	// sensory connections
	for (int i = 1; i <= size-4; i++){
		BestIndividualFile >> w_ASER[i];
	}
	for (int i = 1; i <= size-4; i++){
		BestIndividualFile >> w_ASEL[i];
	}

	// CPG connections
	BestIndividualFile >> w_CPG_SMBV;
	BestIndividualFile >> w_CPG_SMBD;

	BestIndividualFile >> sensorN;
	BestIndividualFile >> sensorM;

	BestIndividualFile >> outputGain;

	BestIndividualFile >> n;
	//size = n.CircuitSize();

	BestIndividualFile.close();
}



void WormAgent::setWormPars(shared_ptr<const CmdArgs> cmd_)
{
	(void)cmd_;
}



void WormAgent::SetParameters(const TVector<double> &v)
{
setParsFromPheno(v);
}

void WormAgent::setParsFromPheno(const TVector<double> &v)
{


	//cout << "WormAgent::SetParameters" << endl;

	NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
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
			n.SetChemicalSynapseWeight(i, j, v(k));
			k++;
		}
	}

	// Chemical synapses from interneurons to F/B  and between F/B motor neurons
	for (int i = 1; i <= size-2; i++){
		for (int j = size-3; j <= size-2; j++){
			n.SetChemicalSynapseWeight(i, j, v(k));
			k++;
		}
	}

	// // Chemical synapses from interneurons to D/V with DV symmetry
	for (int i = 1; i <= size-4; i++){
			n.SetChemicalSynapseWeight(i, size, v(k));
			n.SetChemicalSynapseWeight(i, size-1, v(k));
			k++;
	}

	// Self-connections for the DV motorneurons
	n.SetChemicalSynapseWeight(size, size, v(k));
	n.SetChemicalSynapseWeight(size-1, size-1, v(k));
	k++;

	// Biases
	for (int i = 1; i <= size-2; i++){
		n.SetNeuronBias(i, v(k));
		k++;
	}
	n.SetNeuronBias(size, v(k));
	n.SetNeuronBias(size-1, v(k));
	k++;

	// Time-constants
	for (int i = 1; i <= size-2; i++){
		n.SetNeuronTimeConstant(i, v(k));
		k++;
	}
	n.SetNeuronTimeConstant(size, v(k));
	n.SetNeuronTimeConstant(size-1, v(k));
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



void WormAgent::InitialiseCircuit()
{

	NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
	//size = CircuitSize_;
	n.SetCircuitSize(size,300,300);
	w_ASER.SetBounds(1, size);
	w_ASER.FillContents(0.0);
	w_ASEL.SetBounds(1, size);
	w_ASEL.FillContents(0.0);
	forward = 1;
}

void WormAgent::zeroCircuit()
{

	NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
	//size = CircuitSize_;
	n.SetCircuitSize(0,300,300);
	w_ASER.SetBounds(1, 0);
	w_ASER.FillContents(0.0);
	w_ASEL.SetBounds(1, 0);
	w_ASEL.FillContents(0.0);
	forward = 1;
}



void WormAgent::setSimParsDefault()
{
}


void WormAgent::setSimPars(double orient_orig_,
	double gradSteep_, double RunDuration_, double HSStepSize_, int taxis_, int kinesis_)
{

	setValCJWorm<double>("orient",orient_orig_);
	setValCJWorm<double>("gradSteep",gradSteep_);
	setValCJWorm<double>("RunDuration",RunDuration_);
	setValCJWorm<int>("taxis",taxis_);
	setValCJWorm<int>("kinesis",kinesis_);


	setStepSize(HSStepSize_);

}



void WormAgent::InitializeSensors(RandomState &rs_)
{
	//rs = &rs_;
	InitialiseAgent();
	ResetChemCon();
	ResetAgentIntState(rs_);

	UpdateChemCon();
}


void WormAgent::initForSimulation(RandomState &rs_)
//void WormAgent::InitializeSimulation(RandomState &rs_)
{
	
	//InitializeSensors(rs_);
	//ResetAgentsBody();

	//return;

	ResetAgentsBody();
	InitializeSensors(rs_);
	
	/* rs = &rs_;
	InitialiseAgent();
	
	ResetChemCon();
	ResetAgentIntState(*rs);
	UpdateChemCon(); */

	//RandomState rs2 = *rs;	
	//Worm2Dbase::InitializeState(rs2);
}


void WormAgent::InitializeState(RandomState &rs_)
{
	setInternalRandomState(rs_);
	Worm2Dbase::InitializeState(rs_);
}

void WormAgent::InitialiseAgent()
{
	const double HSStepSize = itsStepSize();
	double RunDuration;
	getValCJWorm<double>("run_duration",RunDuration);


	VelDelta = (int) (HST/HSStepSize);
	iSensorN = (int) (sensorN/HSStepSize);
	dSensorN = (double) iSensorN;
	iSensorM = (int) (sensorM/HSStepSize);
	dSensorM = (double) iSensorM;
	int upperbound = ((int) (((2*RunDuration) + sensorN + sensorM) / HSStepSize)) + 1;

	//cout << "uppervel " << upperbound << " " << VelDelta << endl;
 	
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

void WormAgent::ResetAgentsBody()
{
	
    getValCJWorm<int>("taxis",taxis);
	getValCJWorm<int>("kinesis",kinesis);

	double MaxDist1;//, orient1;
    getValCJWorm<double>("max_dist",MaxDist1);
	getValCJWorm<double>("orient",orient);
	distanceToCentre = -MaxDist1;

	double tempangle = 0.0;

	//SetPositionX(cos(tempangle) * distanceToCentre);
	//SetPositionY(sin(tempangle) * distanceToCentre);
	px = cos(tempangle) * MaxDist1*-1; //DistanceToCentre();
	py = sin(tempangle) * MaxDist1*-1; //DistanceToCentre();
	vx = 0.0;
	vy = 0.0;
	theta = 0.0;
	//orient = orient1;
	CPGoffset = 0.0;
	forward = 1;
}

void WormAgent::ResetChemCon()
{

	//double gradSteep; 
    getValCJWorm<double>("grad_steep",gradSteep);
	chemCon = -DistanceToCentre() * gradSteep;

	//double dist = distanceToCenter();
	//chemCon = -dist * gradPars->gradSteep;

	pastCon = chemCon;
	timer = iSensorN + iSensorM + 1;
	for (int i = 1; i <= timer; i++)
		chemConHistory(i) = chemCon;
	presentAvgCon = chemCon * iSensorN;
	pastAvgCon = chemCon * iSensorM;
}

void WormAgent::ResetAgentIntState(RandomState &rs)
{
	NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
	n.RandomizeCircuitState(0.0, 0.0, rs);
	//n.RandomizeCircuitState(0.0, 0.5, rs);
}


void WormAgent::setDistanceToCentre()
{
	distanceToCentre = sqrt(pow(PositionX(),2) + pow(PositionY(),2));

}

double WormAgent::DistanceToCentre(void) {return distanceToCentre;}

double WormAgent::distanceToCenter() const
{
	return distanceToCentre;
	//cout << "WormAgent::setDistanceToCentre()" << endl;
	return sqrt(pow(PositionX(),2) + pow(PositionY(),2));
}

void WormAgent::UpdateChemCon()
{
	//double gradSteep; 
    //getValCJWorm<double>("grad_steep",gradSteep);

	//double dist = distanceToCenter();
	setDistanceToCentre();
	//distanceToCentre = sqrt(pow(px_,2) + pow(py_,2));
	//distanceToCentre = sqrt(pow(px,2) + pow(py,2));
	pastCon = chemCon;
	chemCon = -DistanceToCentre() * gradSteep;

	//chemCon = -dist * gradPars->gradSteep;
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


void WormAgent::writeData()
{
	//cout << "WormAgent write data" << endl;
	Worm2Dbase::writeData();
	writeBodyPos();
}

void WormAgent::writeAct()
{
  
    size_t pos = getPos("act.dat");
    ofstream & ofs = ofsvec[pos];  
    int & tt = tts[pos];

    if (++tt >= dataskips) {
        tt = 0;
        //time
        ofs << datatime;

		for (int i = 1; i <= size; i++) ofs << " " << n_ptr->NeuronOutput(i);

		ofs << " " << oASEL << " " << oASER ;
      

        ofs << endl;
    }
}


void WormAgent::writeBodyPos()
{
    /* static bool firstcall = true;
    static size_t pos;
    static int tt;
    
    if (resetStats(firstcall,pos,tt,"bodypos.dat")) return; */

	size_t pos = getPos("bodypos.dat");
    ofstream & ofs = ofsvec[pos];  
	int & tt = tts[pos];

    if (++tt >= dataskips) {
        tt = 0;

	//ofs << datatime << " " << px << " " << py << " " << theta << endl;
    ofs << datatime << " " << PositionX() << " " << PositionY() << " " << theta << endl;
        // Body
        
    }
    return;
}


void WormAgent::PrintPath( ofstream &file)
{
	file << PositionX() << " ";
	//file << px << " ";
	file << PositionY() << " ";
	//file << py << " ";
	file << endl;
}

void WormAgent::PrintDetail( ofstream &file)
{
	file << PositionX() << " ";
	file << PositionY() << " ";
	//file << px << " ";
	//file << py << " ";
	file << theta << " ";
	file << orient << " ";
	file << avgvel << " ";
	file << oASEL << " ";
	file << oASER << " ";
	for (int i = 1; i <= size; i++){
		file << n_ptr->NeuronOutput(i) << " ";
	}
	file << avgtheta << " ";
	// file << pastAvgCon << " ";
	file << endl;
}




vector<doubIntParamsHead> WormAgent::getWormParams()
{

    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;

    var1.parDoub.head = "worm";
    var1.parDoub.names = {"MaxDist", "MaxVel", "MaxGauGradHeight", 
		"ChemDiffConst", "HST", "HSP",
	"w_CPG_SMBV", "w_CPG_SMBD", "sensorN", "sensorM", "outputGain"
	};
    var1.parDoub.vals = {MaxDist, MaxVel, MaxGauGradHeight, ChemDiffConst, HST, HSP,
	w_CPG_SMBV, w_CPG_SMBD, sensorN, sensorM, outputGain
	};

	var1.parDoub.messages = {
	"Half the radius of the big petri dish (in cm)", 
	"Forward velocity (in cm/s)",
	"Because the MaxDist is also the maximum height of the cone shaped gradient",
	"Simulated chemical environment according to Ward, 1973 as described in Ferree and Lockery 1999 equation 14.",
	"Head Sweep Time, T=4.2sec, According to Ferree, Marcotte, Lockery, 1997",
	"Head-sweep period 2*Pi/T, According to Ferree, Marcotte, Lockery, 1997."};


    parvec.push_back(var1);


    return parvec;

}


void WormAgent::addParsToJson(json & j)
{
   
	string nsHead = "Nervous system";
	

	NervousSystem * n_ptr1 = dynamic_cast<NervousSystem*>(n_ptr);
    if (n_ptr1){
    appendAllNSJson(j[nsHead], *n_ptr1);
    }


 	vector<string> names;
    if (j.contains("nervous_system")
    && j.at("nervous_system").contains("cell_names")
    && j.at("nervous_system").at("cell_names").contains("value")
    && j.at("nervous_system").at("cell_names").at("value").is_array())
    names = j.at("nervous_system").at("cell_names").at("value").template get< vector<string> >();
    else names = getDistinctCellNames();

    if ((names.empty() || names[0]=="not implemented") && j.contains("Nervous system")
    && j.at("Nervous system").is_object()
    && j.at("Nervous system").contains("Cell name") 
    && j.at("Nervous system").at("Cell name").contains("value") 
    && j.at("Nervous system").at("Cell name").at("value").is_array())
    {
    names = makeUnique(j.at("Nervous system").
    at("Cell name").at("value").template get< vector<string> >());
    }
    if ((names.empty() || names[0]=="not implemented") && j.contains("Nervous system")
    && j.at("Nervous system").is_object()
    && j.at("Nervous system").contains("size")
    && j.at("Nervous system").at("size").contains("value"))
    {
        int size = j.at("Nervous system").at("size").at("value").get<int>();
        names.clear();
        for (int i=1; i<=size; i++) names.push_back("cell_"+to_string(i-1));
    }
    if (names.empty() || names[0]=="not implemented")
    {
        names.clear();
        for (int i=0; i<par1.N_size; i++) names.push_back("cell_"+to_string(i));
    }
    assert(!names.empty() && names[0]!="not implemented");

	 
    if (n_ptr1){
    appendNSToJsonByCell(j, *n_ptr1, names, getSectionNames());
    }






	j[nsHead]["section sizes"]["interneurons"]["value"] = size;
	j[nsHead]["section sizes"]["interneurons"]["plot order"] = 0;

	Params< vector<double> > par;
	par.names =  {"w_ASER", "w_ASEL"};
	par.vals = {getVector<double>(w_ASER), getVector<double>(w_ASEL)};
	appendToJson<vector<double> >(j["Sensory"],par);

	Worm2Dbase::addParsToJson(j);
	 
}


void WormAgent::Step1()
{
	UpdateSensors();
	StepOrig();
	UpdateChemCon();
}



// Step
void WormAgent::StepOrig()
{
	preNStep();
	// Update the signaling system
	n_ptr->EulerStep(settedStepSize);
	postNStep();
	moveAgent();
}


void WormAgent::preNStep1()
{
// Input from sensory neurons to interneurons
	for (int i = 1; i <= size-4; i++){
			n_ptr->SetNeuronExternalInput(i, w_ASEL[i] * oASEL + w_ASER[i] * oASER);
	}

}


void WormAgent::preNStep()
{
// 
	preNStep1();

	const double timestep1 =  t + settedStepSize;
	// Add antiphase oscillatory input to the neck motor neurons
	n_ptr->SetNeuronExternalInput(size-1, w_CPG_SMBV * sin(CPGoffset + HSP*timestep1));
	n_ptr->SetNeuronExternalInput(size, w_CPG_SMBD * sin(CPGoffset + Pi + HSP*timestep1));

}




void WormAgent::postNStep()
{
	

// Update curvature
	if (taxis == 1){
		NMdiff = n_ptr->NeuronOutput(size-1) - n_ptr->NeuronOutput(size);
		theta = outputGain * NMdiff;
		orient += settedStepSize * theta;

		// Check that there is always one part of the curvature that is convex and another that is concave. This way thrust is generated.
		// Maintain a cumulative average of absolute theta and ensure that it is not too low (otherwise it's going straightish)
		DthetaDt = theta - pastTheta;
		pastTheta = theta;
		pushCurv = DthetaDt > 0 ? settedStepSize : -settedStepSize;
		avgtheta = histTheta.PushFront(fabs(theta))/VelDelta;
		avgvel = MaxVel * pow(1 - (fabs(histCurv.PushFront(pushCurv))/HST), 2) * pow(1-fabs(0.7-avgtheta),2);
	}

	// Update Forward -> Backward
	if (kinesis == 1){

		assert(0);
		if ((forward == 1) && (n_ptr->NeuronOutput(size-2) > 0.6) && (n_ptr->NeuronOutput(size-3) < 0.4) )
		{
			forward = 0;
		}
		// Update Backward -> Forward
		if ((forward == 0) && (n_ptr->NeuronOutput(size-2) < 0.4) && (n_ptr->NeuronOutput(size-3) > 0.6) )
		{
			forward = 1;
			orient = wa_rs->UniformRandom(0, 2*Pi);
		}
		if (forward == 0)
			{avgvel = -MaxVel;}
		else
			{avgvel = MaxVel;}
 	}

}

void WormAgent::moveAgent()
{
// Update the velocity
	vx = cos(orient) * avgvel;
	vy = sin(orient) * avgvel;

	// Move the agent
	//SetPositionX(PositionX() + (settedStepSize * vx));
	//SetPositionY(PositionY() + (settedStepSize * vx));

	//PositionX() += settedStepSize * vx;
	//PositionY() += settedStepSize * vy;
	px += settedStepSize * vx;
	py += settedStepSize * vy;
	//t += StepSize;


}

	
void WormAgent::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{
	const double BiasRange = 15.0;
const double SensorWeightRange = 1500.0;
const double InterneuronWeightRange = 15.0;

const double HST =	4.2;			// Head Sweep Time, T=4.2sec, According to Ferree, Marcotte, Lockery, 1997.

const double StretchReceptorRange = 15.0;

const double MaxDifSensor = HST;

const double TauMax = HST;

const double HSStepSize = itsStepSize();

const double MinNeckTurnGain = 1.0;
const double MaxNeckTurnGain = 2.0;
const double TauMin = HSStepSize*10; // = 10*evoPars1.StepSize;
const double MinDifSensor = HSStepSize*10; // 



//const double TauMin = itsStepSize()*10; // = 10*evoPars1.StepSize;
//const double MinDifSensor = itsStepSize()*10; // = 10*evoPars1.StepSize;


//	cout << "EvolutionCO::GenPhenMapping " << endl;
	int k = 1;

	// Sensor to interneurons
	for (int i = 1; i <= 2*(size-4); i++){
		phen(k) = MapSearchParameter(gen(k), -SensorWeightRange, SensorWeightRange);
		k++;
	}

	// Weights between interneurons (fully recurrent, non-symm)
	for (int i = 1; i <= (size-4)*(size-4) + (size-2)*2 + (size-4) + 1; i++){
		phen(k) =  MapSearchParameter(gen(k), -InterneuronWeightRange, InterneuronWeightRange);
		k++;
	}

	// Biases interneurons
	for (int i = 1; i <= (size-2) + 1; i++){
		phen(k) =  MapSearchParameter(gen(k), -BiasRange, BiasRange);
		k++;
	}

	//  Time-constants
	for (int i = 1; i <= (size-2) + 1; i++){
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
