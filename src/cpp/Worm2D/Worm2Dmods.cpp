#include "Worm2Dmods.h"
#include "../neuromlLocal/c302ForW2D.h"


void pfa::swap_all(pfa & pfa_){
    assert(pfa_.size==size);
//size = pfa_.size;
phase.swap(pfa_.phase);
freq.swap(pfa_.freq);
amp.swap(pfa_.amp);
}


void pfa::addParsToJson(json & j)
{
 
json & j2 = j["Nervous system"];
    
j2["size"]["value"] = size;
j2["freq"]["value"] = freq;
//j["freq"]["cell_val"] = 1;
j2["phase"]["value"] = phase;
//j["phase"]["cell_val"] = 1;
j2["amp"]["value"] = amp;
//j["amp"]["cell_val"] = 1;
vector<string> cell_names(size, "cell");
j2["Cell name"]["value"] = cell_names;

 



{

json & j2 = j["nervous_system"];
vector<string> cell_names_full = makeUnique(cell_names);
j2["cell_names"]["value"] = cell_names_full;


//if (!j2.contains("cells")) j2["cells"] = json::object();
json & j3 = j2["cells"];
for (int i=0;i<cell_names_full.size();i++) 
  {
    const string & name = cell_names_full[i];
    if (!j3.contains(name)) j3[name] = json::object();
    json & j4 = j3[name];
    j4["freq"]["value"] = freq[i];
    j4["phase"]["value"] = phase[i];
    j4["amp"]["value"] = amp[i];
  
  }

}

}



void CoupledOsc::EulerStep(double stepsize)
{  
   
    vector<double> phase_inc = pfa1.freq;
    for (int i=0;i<phase_inc.size();i++) phase_inc[i]=phase_inc[i]*pi2;
    for (int i=0;i<weights.size();i++){
    toFromWeight & w = weights[i];
    phase_inc[w.to-1] += sin(pfa1.phase[w.w.from-1] - pfa1.phase[w.to-1])*w.w.weight;
    }
    for (int i=0;i<phase_inc.size();i++) pfa1.phase[i] += phase_inc[i]*stepsize;

}

//shared_ptr<W2Dparameters> w2par_ptr(new Worm2Doscpars1());

/* Worm2DPars::Worm2DPars(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<W2Dbaseparameters> w2par_ptr):
Worm2Dm(par1_,n_ptr_,w2par_ptr)//,pars1_ptr(w2par_ptr)
,Worm2D(par1_,0){} */


Worm2DoscNML::Worm2DoscNML(int size_, shared_ptr<const CmdArgs> cmd_, const json & j):
Worm2Dm({size_,24,0.1,1,size_}, new c302ForW2D(), cmd_, j),
Worm2D({size_,24,0.1,1,size_}, nullptr)
{
    basePar1 = this;
    const json & j2 = BPitsJson;
    setMuscBodExt(j2);
}


Worm2DoscNML::Worm2DoscNML(const string & jsonfile_, 
    shared_ptr<const CmdArgs> cmd_):
    Worm2DoscNML(48, cmd_, getJsonFromFile(jsonfile_))
{
    //json j = getJsonFromFile(jsonfile_);
    //pars1->setParsFromJson(j["Worm"]);
    //setMuscBodExt(j); 

}

Worm2DoscNMLm::Worm2DoscNMLm(int size_, shared_ptr<const CmdArgs> cmd_, const json & j):
Worm2Dm({size_,24,0.1,1,size_},new c302ForW2D(),0, cmd_, j)
{

    const json & j2 = BPitsJson;
    setBodExt(j2);
}

Worm2DoscNMLm::Worm2DoscNMLm(const string & jsonfile_, 
    shared_ptr<const CmdArgs> cmd_):Worm2DoscNMLm(48,cmd_,getJsonFromFile(jsonfile_))
{
}







//Worm2DPars::Worm2DPars(wormIzqParams par1_, NSForW2D * n_ptr_, W2Dparameters * w2par_ptr):
//Worm2Dm(par1_,n_ptr_),pars1_ptr(w2par_ptr),Worm2D(par1_,0){}

Worm2DoscBase::Worm2DoscBase(wormIzqParams par1_):
Worm2D(par1_, nullptr), Worm2Dm(par1_, new NSosc(par1_.N_size)),
n(dynamic_cast<NSosc&>(*n_ptr)){}


Worm2Dosc::Worm2Dosc(int size_, shared_ptr<const CmdArgs> cmd_):
Worm2DoscBase({size_,24,0.1,1,size_}),
//pars1(dynamic_pointer_cast<Worm2Doscpars>(W2Dbaseparameters1)),
Worm2Dm({size_,24,0.1,1,size_},new NSosc(size_), cmd_)
{

    basePar1 = this;
    //TVector<double> pheno;
    //pheno.SetBounds(1,getVectSize());
    //pheno.SetBounds(1,evoVectSize);
    //construct(pheno);
}


Worm2Dosc::Worm2Dosc(shared_ptr<const CmdArgs> cmd_):Worm2Dosc(48, cmd_){}


Worm2Dosc::Worm2Dosc(const string & filename_, shared_ptr<const CmdArgs> cmd_):Worm2Dosc(48, cmd_)
{
    setParsFromFile(filename_);
    //construct(filename_);
}



Worm2Dosc::Worm2Dosc(TVector<double> & phengen, const bool & isPheno):Worm2Dosc(48)
{
    //if (isPheno) construct(phengen);
    //else constructFromGeno(phengen);
    setParsFromPhenGen(phengen,isPheno);
    //if (isPheno) setParsFromPheno(phengen);
    //else setParsFromGeno(phengen);

}

Worm2DoscHalf::Worm2DoscHalf():Worm2Dosc(24),
Worm2Dm({24,24,0.1,1,24},new NSosc(24)){}

Worm2DoscHalf::Worm2DoscHalf(const string & filename_):Worm2Dosc(24),
Worm2Dm({24,24,0.1,1,24},new NSosc(24))
{
    //construct(filename_);
    setParsFromFile(filename_);
}

Worm2DoscHalf::Worm2DoscHalf(TVector<double> & phengen, const bool & isPheno):Worm2Dosc(24),
Worm2Dm({24,24,0.1,1,24},new NSosc(24))
{
    setParsFromPhenGen(phengen,isPheno);
    //if (isPheno) setParsFromPheno(phengen);
    //else setParsFromGeno(phengen);
   
}



Worm2Dosc21::Worm2Dosc21(shared_ptr<const CmdArgs> cmd_):
Worm2DoscBase({2,24,0.1,7,14}),
Worm2Dosc21base(2),
Worm2Dm({2,24,0.1,7,14},new NSosc(14),  cmd_) 
//shared_ptr<Worm2Dosc21pars>(make_shared<Worm2Dosc21pars>(24)))
{
    basePar1 = this;
    //pars1->NMJ_Gain.SetBounds(1, par1.N_muscles);
}


Worm2Dosc21::Worm2Dosc21(const string & filename_, shared_ptr<const CmdArgs> cmd_):Worm2Dosc21(cmd_)
//Worm2DoscBase({2,24,0.1,7,14}, new Worm2Dosc21pars()),
//Worm2Dosc21base(par1, dynamic_cast<Worm2Dosc21pars&>(*(pars1_ptr))),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    setParsFromFile(filename_);
}


Worm2Dosc21::Worm2Dosc21(TVector<double> & phengen, const bool & isPheno):Worm2Dosc21()
//Worm2DoscBase({2,24,0.1,7,14}, new Worm2Dosc21pars()),
//Worm2Dosc21base(par1, dynamic_cast<Worm2Dosc21pars&>(*(pars1_ptr))),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    //if (isPheno) construct(pheno);
    //else constructFromGeno(pheno);

    setParsFromPhenGen(phengen,isPheno);
    //if (isPheno) setParsFromPheno(phengen);
    //else setParsFromGeno(phengen);
   
}


Worm2Dosc21NML::Worm2Dosc21NML(const json & j, shared_ptr<const CmdArgs> cmd_):
Worm2D({2,24,0.1,7,14}, nullptr),
Worm2Dosc21base(2),
//dynamic_pointer_cast<Worm2Dosc21pars>(W2Dbaseparameters1b)),
//Worm2Dm({2,24,0.1,7,14}, new c302ForW2D(), shared_ptr<Worm2Dosc21pars>(make_shared<Worm2Dosc21pars>(24)))
Worm2Dm({2,24,0.1,7,14}, new c302ForW2D(), cmd_,j)
{
    basePar1 = this;

    const json & j2 = BPitsJson;
    setMuscBodExt(j2);
    //pars1->NMJ_Gain.SetBounds(1, par1.N_muscles);
}

Worm2Dosc21NML::Worm2Dosc21NML(const string & jsonfile_, 
    shared_ptr<const CmdArgs> cmd_):Worm2Dosc21NML(getJsonFromFile(jsonfile_), cmd_)
{
    
}

Worm2Dosc21NMLm::Worm2Dosc21NMLm(const json & j, shared_ptr<const CmdArgs> cmd_):
Worm2Dm({2,24,0.1,7,14}, new c302ForW2D(),0, cmd_,j)
{

    const json & j2 = BPitsJson;
    setBodExt(j2);
}





Worm2Dosc21NMLm::Worm2Dosc21NMLm(const string & jsonfile_, 
    shared_ptr<const CmdArgs> cmd_):Worm2Dosc21NMLm(
        getJsonFromFile(jsonfile_), cmd_)
{
    //W2Dbaseparameters1b->setParsFromJson(j["Worm"]);
    
    
        
}



Worm2Dosc21S::Worm2Dosc21S():Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new NSosc(14),shared_ptr<Worm2Dosc21pars>(make_shared<Worm2Dosc21pars>(24))){}
Worm2Dm({2,24,0.1,7,14},new NSosc(14)){}

Worm2Dosc21S::Worm2Dosc21S(const string & filename_):Worm2Dosc21S()
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
   setParsFromFile(filename_);

}

Worm2Dosc21S::Worm2Dosc21S(TVector<double> & phengen, const bool & isPheno):Worm2Dosc21S()
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new NSosc())
{
  //  pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    //if (isPheno) construct(pheno);
    //else constructFromGeno(pheno);
    setParsFromPhenGen(phengen,isPheno);

    //if (isPheno) setParsFromPheno(phengen);
    //else setParsFromGeno(phengen);

}

Worm2Dosc21all::Worm2Dosc21all(shared_ptr<const CmdArgs> cmd_):
Worm2Dosc21(),Worm2Dm({2,24,0.1,7,14},new NSosc(14),cmd_)
//shared_ptr<Worm2Dosc21pars>(make_shared<Worm2Dosc21pars>(24)))
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
}


Worm2Dosc21allNML::Worm2Dosc21allNML(const json & j, shared_ptr<const CmdArgs> cmd_):
Worm2D({2,24,0.1,7,14}, nullptr), Worm2Dosc21base(2),
//Worm2Dm({2,24,0.1,7,14}, new c302ForW2D(), shared_ptr<Worm2Dosc21pars>(make_shared<Worm2Dosc21pars>(24)))
Worm2Dm({2,24,0.1,7,14}, new c302ForW2D(), cmd_, j)
{  basePar1 = this;
    const json & j2 = BPitsJson;
    setMuscBodExt(j2);
    //pars1->NMJ_Gain.SetBounds(1, par1.N_muscles);
}

Worm2Dosc21allNML::Worm2Dosc21allNML(const string & jsonfile_, 
    shared_ptr<const CmdArgs> cmd_):Worm2Dosc21allNML(getJsonFromFile(jsonfile_), cmd_)
{
}

Worm2Dosc21allNMLm::Worm2Dosc21allNMLm(const json & j, shared_ptr<const CmdArgs> cmd_):
Worm2Dm({2,24,0.1,7,14}, new c302ForW2D(),0, cmd_, j)
{
    const json & j2 = BPitsJson;
    setBodExt(j2);
}

Worm2Dosc21allNMLm::Worm2Dosc21allNMLm(const string & jsonfile_, 
    shared_ptr<const CmdArgs> cmd_):Worm2Dosc21allNMLm(getJsonFromFile(jsonfile_), cmd_)
{
}

Worm2Dosc21all::Worm2Dosc21all(const string & filename_, 
    const bool & isGenJson, shared_ptr<const CmdArgs> cmd_):Worm2Dosc21all(cmd_)
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    if (isGenJson) setParsFromFile(filename_);
    else {
    json j = getJsonFromFile(filename_);
    //pars1->setParsFromJson(j);
    assert(0 && "setPfafromjson not implemented");
    setMuscBodExt(j);
    }
}




Worm2Dosc21all::Worm2Dosc21all(TVector<double> & phengen, const bool & isPheno):Worm2Dosc21all()
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new NSosc())
{
  //  pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
   
    setParsFromPhenGen(phengen,isPheno);
    //if (isPheno) setParsFromPheno(phengen);
    //else setParsFromGeno(phengen);

}





Worm2Dosc21Coup::Worm2Dosc21Coup():
Worm2Dosc21(),Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec(),14)),
//shared_ptr<Worm2Dosc21pars>(make_shared<Worm2Dosc21pars>(24))),
cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
  
   // pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
}

Worm2Dosc21Coup::Worm2Dosc21Coup(const string & filename_):Worm2Dosc21Coup()
//Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec())),
//cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    setParsFromFile(filename_);

}


Worm2Dosc21Coup::Worm2Dosc21Coup(TVector<double> & phengen, const bool & isPheno):Worm2Dosc21Coup()
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec())),
//cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    setParsFromPhenGen(phengen,isPheno);
    //if (isPheno) setParsFromPheno(phengen);
    //else setParsFromGeno(phengen);
}

Worm2Dosc21CF::Worm2Dosc21CF():Worm2Dosc21Coup(),
Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec(),14))
//,shared_ptr<Worm2Dosc21pars>(make_shared<Worm2Dosc21pars>(24)))
//cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
  
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
}


Worm2Dosc21CF::Worm2Dosc21CF(const string & filename_):Worm2Dosc21CF()
//Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec()))
//cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
   setParsFromFile(filename_);

}

Worm2Dosc21CF::Worm2Dosc21CF(TVector<double> & phengen, const bool & isPheno):Worm2Dosc21CF()
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec()))
//cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
   
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
   setParsFromPhenGen(phengen,isPheno);

    //if (isPheno) setParsFromPheno(phengen);
    //else setParsFromGeno(phengen);
}


void Worm2Dosc::setPfaFromPheno(const TVector<double> &phen)
{
    

pfa pfa1(n.pfa1.size);
assert(n.pfa1.size==48);
//pfa1.size = 48;
for (int i = 1; i<=24; i++) pfa1.phase.push_back(phen(1)*(i-1));
for (int i = 25; i<=48; i++) pfa1.phase.push_back(phen(1)*(i-25) + phen(2));
for (int i = 1; i<=48; i++) {pfa1.freq.push_back(phen(3));pfa1.amp.push_back(1);}

n.pfa1.swap_all(pfa1);


}

void Worm2DoscHalf::setPfaFromPheno(const TVector<double> &phen)
{
pfa pfa1(n.pfa1.size);
assert(n.pfa1.size==24);
//pfa1.size = 24;
for (int i = 1; i<=24; i++) {pfa1.phase.push_back(phen(1)*(i-1));
pfa1.freq.push_back(phen(2));pfa1.amp.push_back(1);}

n.pfa1.swap_all(pfa1);

}

void Worm2Dosc::setEvolPars(W2Dparameters & w2par_, string evotype_)
//void Worm2Dosc::setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_)
{
    if (evotype_=="Evo21"){
    Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(w2par_);
    Epars1.dbunit = 6;
    Epars1.vbunit = 30;
    }
}

void Worm2Dosc::setParsFromPheno(const TVector<double> &phen)
{
    
    setValCJWorm<double>("NMJWeight",phen(4));

    //if (pars1 == nullptr) assert(0);
    //pars1->NMJweight = phen(4);
    //pars1.dbunit = 6;
    //pars1.vbunit = 30;
    cout << "Worm2Dosc::setParsFromPheno" << endl;
    construct(phen);
   
}




void Worm2DoscHalf::setEvolPars(W2Dparameters & w2par_, string evotype_)
//void Worm2DoscHalf::setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_)
{

    if (evotype_=="Evo21"){
    Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(w2par_);
    Epars1.dbunit = 1;
    Epars1.vbunit = 1;
    }

}

void Worm2DoscHalf::setParsFromPheno(const TVector<double> &phen)
{
    setValCJWorm<double>("NMJWeight",phen(3));
    //pars1->NMJweight = phen(3);
    //pars1.dbunit = 1;
    //pars1.vbunit = 1;
    construct(phen);
}




vector<toFromWeight> Worm2Dosc1::makeDVMuscleConn(int offset)
{
    //cout << "NMJweight " << pars1->NMJweight << endl;
    //assert(0);

    double NMJweight;
    basePar1->getValCJWorm<double>("nmj_weight",NMJweight);

    vector<toFromWeight> vec1;
    for (int to_musc=1;to_musc<=24;to_musc++){
    int from_neuron = to_musc+offset;
    toFromWeight tv({from_neuron,NMJweight},to_musc);
    vec1.push_back(tv);}
    return vec1;
}

vector<toFromWeight> Worm2DoscHalf::makeDVMuscleConn(double weig_amp)
{
    double NMJweight;
    basePar1->getValCJWorm<double>("nmj_weight",NMJweight);


    vector<toFromWeight> vec1;
    for (int to_musc=1;to_musc<=24;to_musc++){
    int from_neuron = to_musc;
    toFromWeight tv({from_neuron,NMJweight*weig_amp},to_musc);
    vec1.push_back(tv);}
    return vec1;
}

vector<toFromWeight> Worm2DoscHalf::makeDorsalMuscleConn()
{
    return makeDVMuscleConn(1);
}

vector<toFromWeight> Worm2DoscHalf::makeVentralMuscleConn()
{
    return makeDVMuscleConn(-1);
}

vector<toFromWeight> Worm2Dosc1::makeDorsalMuscleConn()
{
    return makeDVMuscleConn(0);
}

vector<toFromWeight> Worm2Dosc1::makeVentralMuscleConn()
{
    return makeDVMuscleConn(24);
}

/* void Worm2DPars::Step1_old()
{

   
    b.StepBody(settedStepSize);
    
    
    n_ptr->EulerStep(settedStepSize);
    

    
    setMuscleInput();
  
    // Set input to Body
    //  First two segments receive special treatment because they are only affected by a single muscle
    b.SetDorsalSegmentActivation(1, m_ptr->DorsalMuscleOutput(1)/2);
    b.SetVentralSegmentActivation(1, m_ptr->VentralMuscleOutput(1)/2);
    b.SetDorsalSegmentActivation(2, m_ptr->DorsalMuscleOutput(1)/2);
    b.SetVentralSegmentActivation(2, m_ptr->VentralMuscleOutput(1)/2);
    
    //  All other segments receive force from two muscles
    for (int i = 3; i <= N_segments-2; i++)
    {
        int mi = (int) ((i-1)/2);
        b.SetDorsalSegmentActivation(i, (m_ptr->DorsalMuscleOutput(mi) + m_ptr->DorsalMuscleOutput(mi+1))/2);
        b.SetVentralSegmentActivation(i, (m_ptr->VentralMuscleOutput(mi) + m_ptr->VentralMuscleOutput(mi+1))/2);
    }
    
    //  Last two segments receive special treatment because they are only affected by a single muscle
    b.SetDorsalSegmentActivation(N_segments-1, m_ptr->DorsalMuscleOutput(par1.N_muscles)/2);
    b.SetVentralSegmentActivation(N_segments-1, m_ptr->VentralMuscleOutput(par1.N_muscles)/2);
    b.SetDorsalSegmentActivation(N_segments, m_ptr->DorsalMuscleOutput(par1.N_muscles)/2);
    b.SetVentralSegmentActivation(N_segments, m_ptr->VentralMuscleOutput(par1.N_muscles)/2);
    
   
}
 */


//NSosc::NSosc(const pfa & pfa_):pfa1(pfa_){}

void Worm2Dosc::setPhenoNames(){
    addPhenoName("phase offset", 1);
    addPhenoName("DV phase offset", 2);
    addPhenoName("freq", 3);
    addPhenoName("NMJ weight", 4);
}



void Worm2Dosc::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{
    
    cout << "w2dosc GenPhenMapping 1" << endl;
    //assert(0);
    const double NMJweight_top = 10;
    const double freq_lo = 0.01;
    const double freq_hi = 4;
    //phases_lag
    int i = 1;
    phen(i) = MapSearchParameter(gen(i), 0, pi2);
    i++;
    phen(i) = MapSearchParameter(gen(i), 0, pi2);
    //dv phase_offset

    //for (i = 1; i <= 48; i++)
    //phen(i) = MapSearchParameter(gen(i), 0, pi2);
    i++;
    phen(i) = MapSearchParameter(gen(i), freq_lo, freq_hi);
    //weight
    i++;
    phen(i) = MapSearchParameter(gen(i), 0, NMJweight_top);
   cout << "w2dosc GenPhenMapping 2" << endl;

}

void Worm2DoscHalf::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{
    
    const double NMJweight_top = 10;
    const double freq_lo = 0.1;
    const double freq_hi = 10;
    //phases_lag
    int i = 1;
    phen(i) = MapSearchParameter(gen(i), 0, pi2);
   
    //for (i = 1; i <= 48; i++)
    //phen(i) = MapSearchParameter(gen(i), 0, pi2);
    i++;
    phen(i) = MapSearchParameter(gen(i), freq_lo, freq_hi);
    //weight
    i++;
    phen(i) = MapSearchParameter(gen(i), 0, NMJweight_top);
   

}


vector<toFromWeight> Worm2Dosc21base::makeMuscleConn(vector<int> neurons, vector<double> NMJ)
{

    const int N_musc = 24;
     double NMJ_Gain_Map;
    basePar1->getValCJWorm<double>("nmj_gain_map",NMJ_Gain_Map);


    vector<double> NMJ_Gain(N_musc,0);
    for (int i=1; i<=N_musc; i++)
    {
    NMJ_Gain[i-1] = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/N_musc));
    }

    vector<toFromWeight> vec1;
    int unit = 1;
    for (int to_musc = 1; to_musc < 5; to_musc++) 
    makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, N_neuronsperunit);
        
    for (int unit = 2; unit <= 5; unit++)
        for (int to_musc = 5 + 3*(unit-2); to_musc < 5 + 3*(unit-1); to_musc++)
            makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, N_neuronsperunit);

    for (int unit = 6; unit <= 7; unit++)
        for (int to_musc = 17 + 4*(unit-6); to_musc < 17 + 4*(unit-5); to_musc++)
            makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, N_neuronsperunit);
    
    cout << "made muscle con" << endl;
    
            //exit(1);
    return vec1;

}

vector<toFromWeight> Worm2Dosc21base::makeDorsalMuscleConn()
{
   //"NMJ_DN"
   //double NMJ_VN = 1, NMJ_DN = 1, NMJ_Gain_Map = 1;
 
double NMJ_DN;
basePar1->getValCJWorm<double>("nmj_dn",NMJ_DN);

const int DN = 1;
vector<int> dorsalNeurons({DN});
vector<double> dorsalNMJ({NMJ_DN});
return makeMuscleConn(dorsalNeurons, dorsalNMJ);
}

vector<toFromWeight> Worm2Dosc21base::makeVentralMuscleConn()
{
    
double NMJ_VN;
basePar1->getValCJWorm<double>("nmj_vn",NMJ_VN);

//cout << "NMJ_VN " << NMJ_VN << endl;

 
    
const int VN = 2;
vector<int> ventralNeurons({VN});
vector<double> ventralNMJ({NMJ_VN});
return makeMuscleConn(ventralNeurons, ventralNMJ);
}

void Worm2DoscBase::addParsToJson(json & j)
{
    //n.addParsToJson(j["Nervous system"]);
    n.addParsToJson(j);

    Worm2D::addParsToJson(j);
    //n.pfa1.addParsToJson(j["Nervous system"]);
    
}

void Worm2Dosc::addParsToJson(json & j)
{
    Worm2DoscBase::addParsToJson(j);
    //pars1->addParsToJson(j["Worm"]);
    //Worm2D::addParsToJson(j);
    //n.pfa1.addParsToJson(j["Nervous system"]);
    //n.addParsToJson(j["Nervous system"]);

}

void Worm2Dosc21::addParsToJson(json & j)
{
    Worm2DoscBase::addParsToJson(j);
    //pars1->addParsToJson(j["Worm"]);
    //Worm2D::addParsToJson(j);
    //n.pfa1.addParsToJson(j["Nervous system"]);
    //n.addParsToJson(j["Nervous system"]);

}

/* void Worm2DoscBase::construct(const string & filename_)
{
    setPfaFromFile(filename_);
    setParsFromFile(filename_);
    setUpMuscleConn();
    n.setTime(t);
}

void Worm2DoscBase::constructFromGeno(TVector<double> &geno)
{
    setPfaFromGeno(geno);
    setParsFromGeno(geno);
    setUpMuscleConn();
    n.setTime(t);

} */

void Worm2DoscBase::construct(const TVector<double> &pheno)
{
    setPfaFromPheno(pheno);
    //setParsFromPheno(pheno);
    setMuscBodExt();
    //setUpMuscleConn();
    //setUpBodyConn();
    //makeExternalInputConn();
    n.setTime(t);

}





/* void Worm2DoscBase::setPfaFromFile(const string & genofilename_)
{
    
    ifstream ifs;
    ifs.open(genofilename_);
    TVector<double> bestVector(1, getVectSize());
    ifs >> bestVector;
    ifs.close();
  
    setPfaFromGeno(bestVector);

}

void Worm2DoscBase::setPfaFromGeno(TVector<double> &v)
{ 
 TVector<double> phenotype(1, v.Size());
 GenPhenMapping(v, phenotype);
 setPfaFromPheno(phenotype);
}
 */




void Worm2Dosc21::setPfaFromPheno(const TVector<double> &phen)
{


assert(n.pfa1.size==14);
pfa pfa1(n.pfa1.size);
//pfa1.size = 14;
vector<double> phase_1(14,0);

for (int unit = 1; unit<=7; unit++){
const int neuron_d = nn(1,unit) - 1;
phase_1[neuron_d] = phen(1)*(unit-1);
const int neuron_v = nn(2,unit) - 1;
phase_1[neuron_v] = phen(1)*(unit-1) + phen(2);
}
pfa1.phase.swap(phase_1);

for (int i = 1; i<=14; i++) {pfa1.freq.push_back(phen(3));pfa1.amp.push_back(1);}
n.pfa1.swap_all(pfa1);

}


void Worm2Dosc21all::setPfaFromPheno(const TVector<double> &phen)
{
assert(n.pfa1.size==14);
pfa pfa1(n.pfa1.size);
//pfa1.size = 14;
vector<double> phase_1(14,0);

for (int unit = 1; unit<=7; unit++){
const int neuron_d = nn(1,unit) - 1;
phase_1[neuron_d] = phen(unit);
const int neuron_v = nn(2,unit) - 1;
phase_1[neuron_v] = phen(unit+7);
}
pfa1.phase.swap(phase_1);

for (int i = 1; i<=14; i++) {pfa1.freq.push_back(phen(15));pfa1.amp.push_back(1);}
n.pfa1.swap_all(pfa1);

}

void CoupledOsc::setFromPheno(const TVector<double> &pheno, int offset)
{
for (int i = 0; i<weights.size(); i++) weights[i].w.weight = pheno(i+1+offset);

}

vector<toFromWeight> Worm2Dosc21Coup::getWeightVec()
{
vector<toFromWeight> vec;
for (int i=1;i<=6;i++){
{toFromWeight w;
w.w.from = i;
w.to = i+1;
w.w.weight = 0;
vec.push_back(w);}
{toFromWeight w;
w.w.from = i+1;
w.to = i;
w.w.weight = 0;
vec.push_back(w);}
{toFromWeight w;
w.w.from = i;
w.to = i+7;
w.w.weight = 0;
vec.push_back(w);}
{toFromWeight w;
w.w.from = i+7;
w.to = i;
w.w.weight = 0;
vec.push_back(w);}
{toFromWeight w;
w.w.from = i+7;
w.to = i+8;
w.w.weight = 0;
vec.push_back(w);}
{toFromWeight w;
w.w.from = i+8;
w.to = i+7;
w.w.weight = 0;
vec.push_back(w);}
}
return vec;
}

void Worm2Dosc21Coup::setPfaFromPheno(const TVector<double> &phen)
{
assert(n.pfa1.size==14);
//n.pfa1.size = 14;
const int & weisize = cn.weights.size();
vector<double> freq, amp(n.pfa1.size,1), phase(n.pfa1.size,0);
for (int i = 0; i<n.pfa1.size; i++) {freq.push_back(phen(weisize+1));}
//freq.swap(n.pfa1.freq);
//amp.swap(n.pfa1.amp);
n.pfa1.freq.swap(freq);
n.pfa1.amp.swap(amp);
n.pfa1.phase.swap(phase);

}

void Worm2Dosc21CF::setPfaFromPheno(TVector<double> &phen)
{
assert(n.pfa1.size==14);
//n.pfa1.size = 14;
const int & weisize = cn.weights.size();
vector<double> freq, amp(n.pfa1.size,1), phase(n.pfa1.size,0);
for (int i = 1; i<=n.pfa1.size; i++) {freq.push_back(phen[weisize+i]);}
//freq.swap(n.pfa1.freq);
//amp.swap(n.pfa1.amp);
n.pfa1.freq.swap(freq);
n.pfa1.amp.swap(amp);
n.pfa1.phase.swap(phase);

}

void Worm2Dosc21::setEvolPars(W2Dparameters & w2par_, string evotype_)
//void Worm2Dosc21::setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_)
{
    if (evotype_=="Evo21"){
    Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(w2par_);
    Epars1.dbunit = nn(1,3);
    Epars1.vbunit = nn(2,3);
    }

}


void Worm2Dosc21::setParsFromPheno(const TVector<double> &phen, int offset)
{


    basePar1->setValCJWorm<double>("NMJ_Gain_Map",phen(offset));

    basePar1->setValCJWorm<double>("NMJ_VN",phen(offset+1));
    basePar1->setValCJWorm<double>("NMJ_DN",phen(offset+2));

    //pars1->NMJ_Gain_Map = phen(offset);
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);

    /* for (int i=1; i<=24; i++)
    {
    pars1->NMJ_Gain[i-1] = 0.7*(1.0 - (((i-1)*pars1->NMJ_Gain_Map)/pars1->N_musc));
    }
    
    pars1->NMJ_VN = phen(offset+1);
    pars1->NMJ_DN = phen(offset+2);

     */
    //pars1.dbunit = nn(1,3);
    //pars1.vbunit = nn(2,3);
   
}


void Worm2Dosc21::setParsFromPheno(const TVector<double> &phen)
{
    setParsFromPheno(phen,4);
    construct(phen);

}

void Worm2Dosc21S::setParsFromPheno(const TVector<double> &phen)
{

    basePar1->setValCJWorm<double>("NMJ_Gain_Map",phen(4));

    basePar1->setValCJWorm<double>("NMJ_VN",phen(5));
    basePar1->setValCJWorm<double>("NMJ_DN",phen(5));


  /*   pars1->NMJ_Gain_Map = phen(4);

    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    for (int i=1; i<=pars1->N_musc; i++)
    {
    pars1->NMJ_Gain[i-1] = 0.7*(1.0 - (((i-1)*pars1->NMJ_Gain_Map)/pars1->N_musc));
    }


    pars1->NMJ_DN = phen(5);
    pars1->NMJ_VN = phen(5); */



    //pars1.dbunit = nn(1,3);
    //pars1.vbunit = nn(2,3);

    construct(phen);
}


void Worm2Dosc21all::setParsFromPheno(const TVector<double> &phen)
{
    Worm2Dosc21::setParsFromPheno(phen,16);
    construct(phen);
   
}

void Worm2Dosc21Coup::setParsFromPheno(const TVector<double> &phen)
{
    
    cn.setFromPheno(phen);
    const int & weisize = cn.weights.size();
    Worm2Dosc21::setParsFromPheno(phen,weisize+2);
    construct(phen);

}

void Worm2Dosc21CF::setParsFromPheno(const TVector<double> &phen)
{
    assert(n.pfa1.size==14);
    cn.setFromPheno(phen);
    const int & weisize = cn.weights.size();
    Worm2Dosc21::setParsFromPheno(phen,weisize+n.pfa1.size+1);
    construct(phen);
    
}


void Worm2Dosc21::setPhenoNames(){
    addPhenoName("phase offset", 1);
    addPhenoName("DV phase offset", 2);
    addPhenoName("freq", 3);
    addPhenoName("NMJ Gain map", 4);
    addPhenoName("D_NMJ weight", 5);
    addPhenoName("V_NMJ weight", 6);
}

void Worm2Dosc21S::setPhenoNames(){
    addPhenoName("phase offset", 1);
    addPhenoName("DV phase offset", 2);
    addPhenoName("freq", 3);
    addPhenoName("NMJ Gain map", 4);
    addPhenoName("NMJ weight", 5);
   
}

void Worm2Dosc21::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{

    cout << "GenPhenMapping" << endl;
    //assert(0);
    const double NMJweight_top = 10;
    const double freq_lo = 0.01;
    const double freq_hi = 4;
    //phases_lag
    
    phen(1) = MapSearchParameter(gen(1), 0, pi2);
    
    phen(2) = MapSearchParameter(gen(2), 0, pi2);
    
    phen(3) = MapSearchParameter(gen(3), freq_lo, freq_hi);
    //weight
    
    phen(4) = MapSearchParameter(gen(4), 0.2, 1.0); //from Net21
    //phen(4) = MapSearchParameter(gen(4), 0.0, 0.1);

    //phen(5) = MapSearchParameter(gen(5), NMJweight_top*-1, NMJweight_top);
    phen(5) = MapSearchParameter(gen(5), 0, NMJweight_top);


    //phen(6) = MapSearchParameter(gen(6), NMJweight_top*-1, NMJweight_top);
    phen(6) = MapSearchParameter(gen(6), 0, NMJweight_top);
    
   cout << "GenPhenMapping" << endl;

}


void Worm2Dosc21S::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{

    cout << "GenPhenMapping" << endl;
    //assert(0);
    const double NMJweight_top = 10;
    const double freq_lo = 0.01;
    const double freq_hi = 4;
    //phases_lag
    
    phen(1) = MapSearchParameter(gen(1), 0, pi2);
    
    phen(2) = MapSearchParameter(gen(2), 0, pi2);
    
    phen(3) = MapSearchParameter(gen(3), freq_lo, freq_hi);
    //weight
    
    phen(4) = MapSearchParameter(gen(4), 0.2, 1.0); //from Net21
    //phen(4) = MapSearchParameter(gen(4), 0.0, 0.1);

    //phen(5) = MapSearchParameter(gen(5), NMJweight_top*-1, NMJweight_top);
    phen(5) = MapSearchParameter(gen(5), 0, NMJweight_top);


    //phen(6) = MapSearchParameter(gen(6), NMJweight_top*-1, NMJweight_top);
    //phen(6) = MapSearchParameter(gen(6), 0, NMJweight_top);
    
   cout << "GenPhenMapping" << endl;

}


void Worm2Dosc21all::setPhenoNames(){
    for (int unit=1; unit<=7; unit++)
    addPhenoName("D phase offset", unit);
    for (int unit=8; unit<=14; unit++)
    addPhenoName("V phase offset", unit);
    
    addPhenoName("freq", 15);
    addPhenoName("NMJ Gain map", 16);
    addPhenoName("D_NMJ weight", 17);
    addPhenoName("V_NMJ weight", 18);
}

void Worm2Dosc21Coup::setPhenoNames(){
    const int & size1 = cn.weights.size(); 
    for (int unit=1;unit<=size1;unit++) 
    addPhenoName("connection weight", unit);
    
    addPhenoName("freq", size1+1);
    addPhenoName("NMJ Gain map", size1+2);
    addPhenoName("D_NMJ weight", size1+3);
    addPhenoName("V_NMJ weight", size1+4);
}

void Worm2Dosc21CF::setPhenoNames(){
    const int & size1 = cn.weights.size(); 
    for (int unit=1;unit<=size1;unit++) addPhenoName("connection weight", unit);
    for (int unit=1;unit<=n.pfa1.size;unit++) addPhenoName("freq", size1+unit);

    addPhenoName("NMJ Gain map", size1+n.pfa1.size+1);
    addPhenoName("D_NMJ weight", size1+n.pfa1.size+2);
    addPhenoName("V_NMJ weight", size1+n.pfa1.size+3);
}


void Worm2Dosc21all::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{
   
    cout << "GenPhenMapping" << endl;
    //assert(0);
    const double NMJweight_top = 10;
    const double freq_lo = 0.01;
    const double freq_hi = 4;
    //phases_lag
    
    for (int unit=1;unit<=14;unit++) 
    phen(unit) = MapSearchParameter(gen(unit), 0, pi2);
    
    phen(15) = MapSearchParameter(gen(15), freq_lo, freq_hi);
    //weight
    

    phen(16) = MapSearchParameter(gen(16), 0.2, 1.0); //from Net21
    phen(17) = MapSearchParameter(gen(17), 0, NMJweight_top);
    phen(18) = MapSearchParameter(gen(18), 0, NMJweight_top);
    
   cout << "GenPhenMapping" << endl;

}


void Worm2Dosc21Coup::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{
   
    cout << "GenPhenMapping Worm2Dosc21Coup " << endl;
    
    const double NMJweight_top = 10, weight_top = 1;
    const double freq_lo = 0.01;
    const double freq_hi = 4;
    //phases_lag
    
    const int & size1 = cn.weights.size();
    cout << "size1 " << size1 << endl;
    //assert(0 && size1);
    for (int unit=1;unit<=size1;unit++) 
    phen(unit) = MapSearchParameter(gen(unit), 0, weight_top);
    

    phen(size1+1) = MapSearchParameter(gen(size1+1), freq_lo, freq_hi);
    //weight
    

    phen(size1+2) = MapSearchParameter(gen(size1+2), 0.2, 1.0); //from Net21
    phen(size1+3) = MapSearchParameter(gen(size1+3), 0, NMJweight_top);
    phen(size1+4) = MapSearchParameter(gen(size1+4), 0, NMJweight_top);
    
   cout << "GenPhenMapping" << endl;

}

void Worm2Dosc21CF::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{
   
    cout << "GenPhenMapping Worm2Dosc21CF " << endl;
    
    const double NMJweight_top = 10, weight_top = 1;
    const double freq_lo = 0.01;
    const double freq_hi = 4;
    //phases_lag
    cout << "n.pfa1.size " << n.pfa1.size << endl;
    assert(n.pfa1.size==14);
    const int pfasize = 14; //n.pfa1.size;
    const int & size1 = cn.weights.size();
    cout << "size1 " << size1 << endl;
    //assert(0 && size1);
    for (int unit=1;unit<=size1;unit++) phen(unit) = MapSearchParameter(gen(unit), 0, weight_top);
    
    for (int unit=1;unit<=pfasize;unit++) phen(size1+unit) = MapSearchParameter(gen(size1+unit), freq_lo, freq_hi);
    //weight
    

    phen(size1+pfasize+1) = MapSearchParameter(gen(size1+pfasize+1), 0.2, 1.0); //from Net21
    phen(size1+pfasize+2) = MapSearchParameter(gen(size1+pfasize+2), 0, NMJweight_top);
    phen(size1+pfasize+3) = MapSearchParameter(gen(size1+pfasize+3), 0, NMJweight_top);
    
   cout << "GenPhenMapping" << endl;

}
