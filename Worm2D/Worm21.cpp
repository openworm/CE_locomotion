//
//  Worm21.cpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "Worm21.h"
//#include "../argUtils.h"


//extern SuppliedArgs2021 supArgs1;



Worm21::Worm21(shared_ptr<const CmdArgs> cmd_):
Worm2Dm({7,24,0.1,7,49}, new NervousSystem(), cmd_),
//Worm2Dm({7,24,0.1,7,49}, new NervousSystem(), new Muscles),
Worm2D21(cmd_), n(dynamic_cast<NervousSystem&>(*n_ptr)){

    if (true) n.SetCircuitSize(par1.N_units*par1.N_neuronsperunit, 9, 6);
}

Worm21::Worm21(TVector<double> &pheno, shared_ptr<const CmdArgs> cmd_):Worm21(pheno, true, cmd_){}


Worm21::Worm21(const string & filename_, shared_ptr<const CmdArgs> cmd_):Worm21(cmd_)
{
    setParsFromFile(filename_);
}

// The constructor
Worm21::Worm21(TVector<double> &phengen, bool isPheno, shared_ptr<const CmdArgs> cmd_):Worm21(cmd_)
{

    if (isPheno) setParsFromPheno(phengen);
    else setParsFromGeno(phengen);

}

Worm21R::Worm21R(shared_ptr<const CmdArgs> cmd_):Worm21(cmd_),
Worm2Dm({7,24,0.1,7,49}, new NervousSystem(), cmd_){}

Worm21R::Worm21R(TVector<double> &phengen, bool isPheno, shared_ptr<const CmdArgs> cmd_):Worm21R(cmd_)
{

    if (isPheno) setParsFromPheno(phengen);
    else setParsFromGeno(phengen);
}


Worm21R::Worm21R(TVector<double> &pheno, shared_ptr<const CmdArgs> cmd_):Worm21R(pheno, true, cmd_){}


Worm21R::Worm21R(const string & filename_, shared_ptr<const CmdArgs> cmd_):Worm21R(cmd_)
{
    setParsFromFile(filename_);
}


void Worm21::setEvolPars(W2Dparameters & w2par_, string evotype_)
//void Worm21::setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_)
{
    if (evotype_=="Evo21" || evotype_=="Evo21R"){
    Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(w2par_);

    Epars1.dbunit = 10;
    Epars1.vbunit = 13;
    }

}

/* void Worm21::setWormPars(shared_ptr<const CmdArgs> cmd_)
{
  
    //Worm2D21::setWormPars(cmd_);

  //W2DCEparsA w1(cmd_);
  //W2DCEpars1->setPars(cmd_); 
  
  //return W2Dbaseparameters1;

} */



void Worm21::setParsFromPheno(const TVector<double> &pheno)
{
   
    setCurrentPheno(pheno);

// Muscles
   // m.SetMuscleParams(par1.N_muscles, par1.T_muscle);
    
    // Nervous system // Ventral cord
    if (false)
    n.SetCircuitSize(par1.N_units*par1.N_neuronsperunit, 9, 6);
    
    int as, da, db, dd, vd, vb, va;
    int asNext, dbNext, ddNext, vdNext, vbNext, vaNext ;
    
    for (int u = 1; u <= par1.N_units; u++){
        as = nn(AS, u);
        da = nn(DA, u);
        db = nn(DB, u);
        dd = nn(DD, u);
        vd = nn(VD, u);
        vb = nn(VB, u);
        va = nn(VA, u);

        asNext = nn(AS, u+1);
        dbNext = nn(DB, u+1);
        ddNext = nn(DD, u+1);
        vdNext = nn(VD, u+1);
        vbNext = nn(VB, u+1);
        vaNext = nn(VA, u+1);
        
        // Bias, Time Constant and Self Connections
        n.SetNeuronBias(as, pheno(1));
        n.SetNeuronBias(da, pheno(2));
        n.SetNeuronBias(db, pheno(3));
        n.SetNeuronBias(dd, pheno(4));
        n.SetNeuronBias(vd, pheno(5));
        n.SetNeuronBias(vb, pheno(6));
        n.SetNeuronBias(va, pheno(7));

        n.SetNeuronTimeConstant(as, pheno(8));
        n.SetNeuronTimeConstant(da, pheno(9));
        n.SetNeuronTimeConstant(db, pheno(10));
        n.SetNeuronTimeConstant(dd, pheno(11));
        n.SetNeuronTimeConstant(vd, pheno(12));
        n.SetNeuronTimeConstant(vb, pheno(13));
        n.SetNeuronTimeConstant(va, pheno(14));
        
        n.SetChemicalSynapseWeight(as, as, pheno(15));
        n.SetChemicalSynapseWeight(da, da, pheno(16));
        n.SetChemicalSynapseWeight(db, db, pheno(17));
        n.SetChemicalSynapseWeight(dd, dd, pheno(18));
        n.SetChemicalSynapseWeight(vd, vd, pheno(19));
        n.SetChemicalSynapseWeight(vb, vb, pheno(20));
        n.SetChemicalSynapseWeight(va, va, pheno(21));
        
        // --------
        // Chemical Synapses minimal network
        n.SetChemicalSynapseWeight(as, da, pheno(22));
        n.SetChemicalSynapseWeight(as, vd, pheno(23));
        n.SetChemicalSynapseWeight(da, db, pheno(24));
        n.SetChemicalSynapseWeight(db, as, pheno(25));
        n.SetChemicalSynapseWeight(vd, va, pheno(26));
        n.SetChemicalSynapseWeight(vd, vb, pheno(27));

        n.SetChemicalSynapseWeight(da, dd, pheno(28));
        n.SetChemicalSynapseWeight(vb, dd, pheno(29));
        n.SetChemicalSynapseWeight(va, dd, pheno(30));

        // Electrical Synapse minimal network
        n.SetElectricalSynapseWeight(vd, dd, pheno(31));



//        // Intersegment connections
//        // Chemicals
        if (u < par1.N_units){
            n.SetChemicalSynapseWeight(db, ddNext, pheno(40));
            n.SetChemicalSynapseWeight(vaNext, dd, pheno(41));
        }
//        // Electricals
        if (u < par1.N_units){
//        // Interclasses
            n.SetElectricalSynapseWeight(as, vaNext, pheno(42));
            n.SetElectricalSynapseWeight(da, asNext, pheno(43));
            n.SetElectricalSynapseWeight(vb, dbNext, pheno(44));
        // Intraclasses
//            n.SetElectricalSynapseWeight(db, dbNext, pheno(32));
//            n.SetElectricalSynapseWeight(vb, vbNext, pheno(32));
//            n.SetElectricalSynapseWeight(vd, vdNext, pheno(32));
//            n.SetElectricalSynapseWeight(dd, ddNext, pheno(32));
        }
    }
   
    NMJ_AS = pheno(32);
   NMJ_DA = pheno(33);
   NMJ_DB = pheno(34);
   NMJ_DD = pheno(35);
   NMJ_VD = pheno(36);
   NMJ_VB = pheno(37);
   NMJ_VA = pheno(38);
   
   // NMJ Gain XXX
   NMJ_Gain_Map = pheno(39);

   /* if (false){
   NMJ_Gain.SetBounds(1, par1.N_muscles);
   for (int i=1; i<=par1.N_muscles; i++)
   {
       NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/par1.N_muscles));
   }
   } */

   //setUpMuscleConn();
    setMuscBodExt();


}


void Worm21::InitializeState(RandomState &rs)
{    
    Worm2D21::InitializeState(rs);
    //shared_ptr<W2Dbaseparameters> w1parss = dynamic_pointer_cast<W2Dbaseparameters>(W2Dbaseparameters1b);
    //assert(w1parss!=nullptr);
    
    bool doLegacy;
    getValCJWorm<bool>("do_legacy",doLegacy);

    if (doLegacy){
    bool randomInitialState;
    getValCJWorm<bool>("random_initial_state",randomInitialState);

    if (randomInitialState)
    {
        n.RandomizeCircuitState(-1, 1, rs);
        n.RandomizeCircuitOutput(0.2, 0.8, rs);
    }
    //else if (true) n.RandomizeCircuitOutput(0.5, 0.5, rs);
    else n.RandomizeCircuitOutput(0.5, 0.5, rs); //fix this error?? adam (should be -0.5?)
    //    assert(0);
    }

    return;
}

void Worm21::DumpParams(ofstream &ofs)
{   Worm2D21::DumpParams(ofs);
    ofs << "Biases: \n DB: " << n.NeuronBias(DB) << "\n VB/P: " << n.NeuronBias(VB) << " / " << n.NeuronBias(VB)  << "\n VDA/P: " << n.NeuronBias(VD) <<  " / " << n.NeuronBias(VD) << endl;
}


void Worm21::addParsToJson(json & j)
{
    

        //string nsHead = "Nervous system";
        //appendAllNSJson(j[nsHead], n);
    Worm2D21::addParsToJson(j);   
 
        
}

void Worm21::addEvolvableToJson(json & j)
{


    const vector<string>  cell_names_full = getDistinctCellNames();

  {vector<doubDoub> vec;

const double	BiasRange				= 15.0;
     const double    SCRange                 = 15.0;
     const double    CSRange                 = 15.0;
     const double    TauMin                 = 0.1;
     const double    TauMax                 = 2.5;
     const double    ESRange                 = 2.0;
     const double    NMJmax                  = 1.2;


for (int i = 1; i <= 7; i++) vec.push_back({-BiasRange, BiasRange});
// Time Constant
for (int i = 8; i <= 14; i++) vec.push_back({TauMin, TauMax});
// Self connections
for (int i = 15; i <= 21; i++) vec.push_back({-SCRange, SCRange});
// Chemical synapses
for (int i = 22; i <=30; i++) vec.push_back({-CSRange, CSRange});

vec.push_back({0.0, ESRange});

vec.push_back({0.0, NMJmax});
vec.push_back({0.0, NMJmax});
vec.push_back({NMJmax, NMJmax});
vec.push_back({-NMJmax, 0.0});
vec.push_back({-NMJmax, 0.0});
vec.push_back({NMJmax, NMJmax});
vec.push_back({0.0, NMJmax});
vec.push_back({0.2, 1.0});

vec.push_back({-CSRange, CSRange});
vec.push_back({-CSRange, CSRange});
vec.push_back({0.0, ESRange});
vec.push_back({0.0, ESRange});
vec.push_back({0.0, ESRange});


  j["evolvable_ranges"] = toEvolvableRangesJson(vec);
  j["evolved_used"]["value"] = toEvolvedUsedJson(vec);
  }

vector<intPair> biasvec, tauvec;
vector<fromToInt> chemvec, elecvec;

int as, da, db, dd, vd, vb, va;
    int asNext, dbNext, ddNext, vdNext, vbNext, vaNext ;
    
    for (int u = 1; u <= par1.N_units; u++){
        as = nn(AS, u);
        da = nn(DA, u);
        db = nn(DB, u);
        dd = nn(DD, u);
        vd = nn(VD, u);
        vb = nn(VB, u);
        va = nn(VA, u);

        asNext = nn(AS, u+1);
        dbNext = nn(DB, u+1);
        ddNext = nn(DD, u+1);
        vdNext = nn(VD, u+1);
        vbNext = nn(VB, u+1);
        vaNext = nn(VA, u+1);
      
      
        {vector<intPair> & vec = biasvec;
            vec.push_back({as,1});
            vec.push_back({da,2});
            vec.push_back({db,3});
            vec.push_back({dd,4});
            vec.push_back({vd,5});
            vec.push_back({vb,6});
            vec.push_back({va,7});
       
        }

        {vector<intPair> & vec = tauvec;
            vec.push_back({as,8});
            vec.push_back({da,9});
            vec.push_back({db,10});
            vec.push_back({dd,11});
            vec.push_back({vd,12});
            vec.push_back({vb,13});
            vec.push_back({va,14});
        //j["Nervous system"]["taus"]["evolvable"] = vec;
        }

        {
            vector<fromToInt> & vec = chemvec;
            vec.push_back({as,as,15});
            vec.push_back({da,da,16});
            vec.push_back({db,db,17});
            vec.push_back({dd,dd,18});
            vec.push_back({vd,vd,19});
            vec.push_back({vb,vb,20});
            vec.push_back({va,va,21});

            vec.push_back({as,da,22});
            vec.push_back({as,vd,23});
            vec.push_back({da,db,24});
            vec.push_back({db, as,25});
            vec.push_back({vd, va,26});
            vec.push_back({vd, vb,27});
            vec.push_back({da, dd,28});
            vec.push_back({vb, dd,29});
            vec.push_back({va, dd,30});

            if (u < par1.N_units){
                vec.push_back({db, ddNext, 40});
                vec.push_back({vaNext, dd, 41});
        }

        //j["Nervous system"]["Chemical weights"]["evolvable"] = vec;        
        }
{ //evolution of electric connections must be bidirectional
  vector<fromToInt> & vec = elecvec;
  push_back_double({vd, dd, 31}, vec);
  push_back_double({dd, vd, 31}, vec);
         



    if (u < par1.N_units){
        push_back_double({as, vaNext, 42}, vec);
        push_back_double({vaNext, as, 42}, vec);
        push_back_double({da, asNext, 43}, vec);
        push_back_double({asNext, da, 43}, vec);
        push_back_double({vb, dbNext, 44}, vec);
        push_back_double({dbNext, vb, 44}, vec);
}

 //j["Nervous system"]["Electrical weights"]["evolvable"] = vec;
}


}



j["Nervous system"]["biases"]["evolvable"] = to_evo_json(biasvec);
j["Nervous system"]["taus"]["evolvable"] = to_evo_json(tauvec);
j["Nervous system"]["Chemical weights"]["evolvable"] = chemvec;
j["Nervous system"]["Electrical weights"]["evolvable"] = elecvec;

addEvolvableTFI(j["nervous_system"]["chemical_conns"]["value"], chemvec, cell_names_full);
    addEvolvableTFI(j["nervous_system"]["electrical_conns"]["value"], elecvec, cell_names_full, true);
    addEvolvableIP(j["nervous_system"]["cells"], biasvec, "bias", cell_names_full);
    addEvolvableIP(j["nervous_system"]["cells"], tauvec, "tau", cell_names_full);


vector<intPair> nmjvecd;
nmjvecd.push_back({AS,32});
nmjvecd.push_back({DA,33});
nmjvecd.push_back({DB,34});
nmjvecd.push_back({DD,35});

vector<intPair> nmjvecv;
nmjvecv.push_back({VD,36});
nmjvecv.push_back({VB,37});
nmjvecv.push_back({VA,38});


addEvolvableIP(j["vnc_nmj"]["dorsal_conns"], nmjvecd , "weight", getCellNames());
addEvolvableIP(j["vnc_nmj"]["ventral_conns"], nmjvecv , "weight", getCellNames());

j["vnc_nmj"]["gain_map_d"]["evotag"] = "evotag_39";
j["vnc_nmj"]["gain_map_v"]["evotag"] = "evotag_39";
 

j["VNC NMJ"]["V inds"]["evolvable"] = to_evo_json(nmjvecv);
j["VNC NMJ"]["D inds"]["evolvable"] = to_evo_json(nmjvecd);

j["VNC NMJ"]["NMJ gain map D"]["evolvable"] = 39;
j["VNC NMJ"]["NMJ gain map V"]["evolvable"] = 39;



addEvoNames(j);
}

void Worm21::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{

   
    const double	BiasRange				= 15.0;
     const double    SCRange                 = 15.0;
     const double    CSRange                 = 15.0;
     const double    TauMin                 = 0.1;
     const double    TauMax                 = 2.5;
     const double    ESRange                 = 2.0;
     const double    NMJmax                  = 1.2;
     //const double    IIRange                 = 15.0;    


  // Bias
  for (int i = 1; i <= 7; i++){
    phen(i) = MapSearchParameter(gen(i), -BiasRange, BiasRange);
}
// Time Constant
for (int i = 8; i <= 14; i++){
    phen(i) = MapSearchParameter(gen(i), TauMin, TauMax);
}
// Self connections
for (int i = 15; i <= 21; i++){
    phen(i) = MapSearchParameter(gen(i), -SCRange, SCRange);
}
// Chemical synapses
for (int i = 22; i <=30; i++){
    phen(i) = MapSearchParameter(gen(i), -CSRange, CSRange);
}

// Gap junctions
phen(31) = MapSearchParameter(gen(31), 0.0, ESRange);


// NMJ Weight
phen(32) = MapSearchParameter(gen(32), 0.0, NMJmax);       // AS
phen(33) = MapSearchParameter(gen(33), 0.0, NMJmax);       // DA
phen(34) = MapSearchParameter(gen(34), NMJmax, NMJmax);       // DB
phen(35) = MapSearchParameter(gen(35), -NMJmax, 0.0);      // DD
phen(36) = MapSearchParameter(gen(36), -NMJmax, 0.0);      // VD
phen(37) = MapSearchParameter(gen(37), NMJmax, NMJmax);      // VB
phen(38) = MapSearchParameter(gen(38), 0.0, NMJmax);      // VA

phen(39) = MapSearchParameter(gen(39), 0.2, 1.0);       // Used to be 0.4/0.6 XXX NMJ_Gain Mapping

// Intersegment synapse tested
phen(40) = MapSearchParameter(gen(40), -CSRange, CSRange);  // DB to DDnext
phen(41) = MapSearchParameter(gen(41), -CSRange, CSRange);  // VAnext to DD
phen(42) = MapSearchParameter(gen(42), 0.0, ESRange);       // AS -- VAnext
phen(43) = MapSearchParameter(gen(43), 0.0, ESRange);       // DA -- ASnext
phen(44) = MapSearchParameter(gen(44), 0.0, ESRange);       // VB -- DBnext


}    

void Worm21::setPhenoNames()
{
 
    Worm2D21::setPhenoNames();

	for (int i = 1; i <= 7; i++){	
		phenoNamesNums.push_back(i);
		phenoNames.push_back("bias");
	}
        
    for (int i = 8; i <= 14; i++){	
		phenoNamesNums.push_back(i);
		phenoNames.push_back("tau");
	}

    for (int i = 15; i <= 30; i++){	
		phenoNamesNums.push_back(i);
		phenoNames.push_back("chemsyn");
	}
       
    phenoNamesNums.push_back(31);
	phenoNames.push_back("electsyn");
    
    for (int i = 40; i <= 41; i++){	
		phenoNamesNums.push_back(i);
		phenoNames.push_back("chemsyn");
	}
    
    for (int i = 42; i <= 44; i++){	
		phenoNamesNums.push_back(i);
		phenoNames.push_back("electsyn");
	}

}


void Worm21R::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{

const double wAV_top = 10;

Worm21::GenPhenMapping(gen,phen);

phen(45) = MapSearchParameter(gen(45), 0.0, wAV_top);
phen(46) = MapSearchParameter(gen(46), 0.0, wAV_top);
phen(47) = MapSearchParameter(gen(47), 0.0, wAV_top);
phen(48) = MapSearchParameter(gen(48), 0.0, wAV_top);


}

void Worm21R::setParsFromPheno(TVector<double> &pheno)
{

      wAVA_DA = pheno(45); 
      wAVA_VA = pheno(46);
      wAVB_DB = pheno(47);
      wAVB_VB = pheno(48);

    Worm21::setParsFromPheno(pheno);
}

void Worm21R::setPhenoNames()
{
 
    Worm21::setPhenoNames();

    phenoNamesNums.push_back(45);
    phenoNames.push_back("wAVA_DA");
    phenoNamesNums.push_back(46);
    phenoNames.push_back("wAVA_VA");
    phenoNamesNums.push_back(47);
    phenoNames.push_back("wAVB_DB");
    phenoNamesNums.push_back(48);
    phenoNames.push_back("wAVB_VB");


}
