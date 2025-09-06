#include "Worm2Dmods.h"
#include "../neuromlLocal/c302ForW2D.h"


void pfa::swap_all(pfa & pfa_){
    assert(pfa_.size==size);
//size = pfa_.size;
pfa_.phase.swap(phase);
pfa_.freq.swap(freq);
pfa_.amp.swap(amp);
}


void pfa::addParsToJson(json & j)
{
j["size"]["value"] = size;
j["freq"]["value"] = freq;
//j["freq"]["cell_val"] = 1;
j["phase"]["value"] = phase;
//j["phase"]["cell_val"] = 1;
j["amp"]["value"] = amp;
//j["amp"]["cell_val"] = 1;
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

Worm2DPars::Worm2DPars(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<W2Dparameters> w2par_ptr):
Worm2Dm(par1_,n_ptr_),pars1_ptr(w2par_ptr),Worm2D(par1_,0){}


Worm2DoscNML::Worm2DoscNML(int size_):Worm2Dm({size_,24,0.1,1,size_}, new c302ForW2D()),
Worm2DPars({size_,24,0.1,1,size_}, 0, shared_ptr<Worm2Doscpars1>(new Worm2Doscpars1())),
Worm2Dosc1(dynamic_cast<Worm2Doscpars1&>(*pars1_ptr)){}

Worm2DoscNML::Worm2DoscNML(const string & jsonfile_):Worm2DoscNML(48)
{

    ifstream json_in(jsonfile_);
    json j;
    //assert(0 && "Worm2DoscNML(const string & jsonfile_)");
    json_in >> j;
    json_in.close();
    //assert(0 && "Worm2DoscNML(const string & jsonfile_)");
    pars1.setParsFromJson(j["Worm"]);

    setUpMuscleConn(j);
}

Worm2Dosc21NML::Worm2Dosc21NML():
Worm2DPars({2,24,0.1,7,14}, 0, shared_ptr<Worm2Dosc21pars>(new Worm2Dosc21pars())),
Worm2Dosc21base(par1, dynamic_cast<Worm2Dosc21pars&>(*pars1_ptr)),Worm2Dm({2,24,0.1,7,14}, new c302ForW2D())
{
    pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
}

Worm2Dosc21NML::Worm2Dosc21NML(const string & jsonfile_):Worm2Dosc21NML()
{

    ifstream json_in(jsonfile_);
    json j;
    //assert(0 && "Worm2DoscNML(const string & jsonfile_)");
    json_in >> j;
    json_in.close();
    //assert(0 && "Worm2DoscNML(const string & jsonfile_)");
    pars1.setParsFromJson(j["Worm"]);

    /* for (int i=1; i<=par1.N_muscles; i++)
    {
    pars1.NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*pars1.NMJ_Gain_Map)/par1.N_muscles));
    } */

    setUpMuscleConn(j);
}

//Worm2DPars::Worm2DPars(wormIzqParams par1_, NSForW2D * n_ptr_, W2Dparameters * w2par_ptr):
//Worm2Dm(par1_,n_ptr_),pars1_ptr(w2par_ptr),Worm2D(par1_,0){}

Worm2DoscBase::Worm2DoscBase(wormIzqParams par1_, shared_ptr<W2Dparameters> w2par_ptr):
Worm2DPars(par1_, 0, w2par_ptr), Worm2Dm(par1_, new NSosc(par1_.N_size)),
n(dynamic_cast<NSosc&>(*n_ptr)){}


Worm2Dosc::Worm2Dosc(int size_):Worm2Dosc1(dynamic_cast<Worm2Doscpars1&>(*pars1_ptr)),
Worm2DoscBase({size_,24,0.1,1,size_}, shared_ptr<Worm2Doscpars>(new Worm2Doscpars())),
pars1(dynamic_cast<Worm2Doscpars&>(*pars1_ptr)),
Worm2Dm({size_,24,0.1,1,size_},new NSosc(size_))
{

    //TVector<double> pheno;
    //pheno.SetBounds(1,getVectSize());
    //pheno.SetBounds(1,evoVectSize);
    //construct(pheno);
}


Worm2Dosc::Worm2Dosc():Worm2Dosc(48){}


Worm2Dosc::Worm2Dosc(const string & filename_):Worm2Dosc(48)
{
    construct(filename_);
}


Worm2Dosc::Worm2Dosc(TVector<double> & phengen, const bool & isPheno):Worm2Dosc(48)
{
    if (isPheno) construct(phengen);
    else constructFromGeno(phengen);
}

Worm2DoscHalf::Worm2DoscHalf():Worm2Dosc(24),
Worm2Dm({24,24,0.1,1,24},new NSosc(24)){}

Worm2DoscHalf::Worm2DoscHalf(const string & filename_):Worm2Dosc(24),
Worm2Dm({24,24,0.1,1,24},new NSosc(24))
{
    construct(filename_);   
}

Worm2DoscHalf::Worm2DoscHalf(TVector<double> & pheno, const bool & isPheno):Worm2Dosc(24),
Worm2Dm({24,24,0.1,1,24},new NSosc(24))
{
     if (isPheno) construct(pheno);
    else constructFromGeno(pheno);
   
}



Worm2Dosc21::Worm2Dosc21():
Worm2DoscBase({2,24,0.1,7,14}, shared_ptr<Worm2Dosc21pars>(new Worm2Dosc21pars())),
Worm2Dosc21base(par1, dynamic_cast<Worm2Dosc21pars&>(*(pars1_ptr))),
Worm2Dm({2,24,0.1,7,14},new NSosc(14))
{
    pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
}


Worm2Dosc21::Worm2Dosc21(const string & filename_):Worm2Dosc21()
//Worm2DoscBase({2,24,0.1,7,14}, new Worm2Dosc21pars()),
//Worm2Dosc21base(par1, dynamic_cast<Worm2Dosc21pars&>(*(pars1_ptr))),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    construct(filename_);
}


Worm2Dosc21::Worm2Dosc21(TVector<double> & pheno, const bool & isPheno):Worm2Dosc21()
//Worm2DoscBase({2,24,0.1,7,14}, new Worm2Dosc21pars()),
//Worm2Dosc21base(par1, dynamic_cast<Worm2Dosc21pars&>(*(pars1_ptr))),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    if (isPheno) construct(pheno);
    else constructFromGeno(pheno);
}


Worm2Dosc21S::Worm2Dosc21S():Worm2Dosc21(),
Worm2Dm({2,24,0.1,7,14},new NSosc(14)){}


Worm2Dosc21S::Worm2Dosc21S(const string & filename_):Worm2Dosc21S()
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
   construct(filename_);

}

Worm2Dosc21S::Worm2Dosc21S(TVector<double> & pheno, const bool & isPheno):Worm2Dosc21S()
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new NSosc())
{
  //  pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    if (isPheno) construct(pheno);
    else constructFromGeno(pheno);

}

Worm2Dosc21all::Worm2Dosc21all():
Worm2Dosc21(),Worm2Dm({2,24,0.1,7,14},new NSosc(14))
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
}

Worm2Dosc21all::Worm2Dosc21all(const string & filename_):Worm2Dosc21all()
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
   construct(filename_);

}


Worm2Dosc21all::Worm2Dosc21all(TVector<double> & pheno, const bool & isPheno):Worm2Dosc21all()
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new NSosc())
{
  //  pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    if (isPheno) construct(pheno);
    else constructFromGeno(pheno);

}

Worm2Dosc21Coup::Worm2Dosc21Coup():
Worm2Dosc21(),Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec(),14)),
cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
  
   // pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
}

Worm2Dosc21Coup::Worm2Dosc21Coup(const string & filename_):Worm2Dosc21Coup()
//Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec())),
//cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    construct(filename_);

}


Worm2Dosc21Coup::Worm2Dosc21Coup(TVector<double> & pheno, const bool & isPheno):Worm2Dosc21Coup()
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec())),
//cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
   
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    if (isPheno) construct(pheno);
    else constructFromGeno(pheno);

}

Worm2Dosc21CF::Worm2Dosc21CF():Worm2Dosc21Coup(),
Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec(),14))
//cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
  
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
}


Worm2Dosc21CF::Worm2Dosc21CF(const string & filename_):Worm2Dosc21CF()
//Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec()))
//cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    construct(filename_);

}

Worm2Dosc21CF::Worm2Dosc21CF(TVector<double> & pheno, const bool & isPheno):Worm2Dosc21CF()
//Worm2Dosc21(),
//Worm2Dm({2,24,0.1,7,14},new CoupledOsc(getWeightVec()))
//cn(dynamic_cast<CoupledOsc&>(*n_ptr))
{
   
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    if (isPheno) construct(pheno);
    else constructFromGeno(pheno);

}


void Worm2Dosc::setPfaFromPheno(TVector<double> &phen)
{
    
pfa pfa1(n.pfa1.size);
assert(n.pfa1.size==48);
//pfa1.size = 48;
for (int i = 1; i<=24; i++) pfa1.phase.push_back(phen[1]*(i-1));
for (int i = 25; i<=48; i++) pfa1.phase.push_back(phen[1]*(i-25) + phen[2]);
for (int i = 1; i<=48; i++) {pfa1.freq.push_back(phen[3]);pfa1.amp.push_back(1);}

n.pfa1.swap_all(pfa1);

}

void Worm2DoscHalf::setPfaFromPheno(TVector<double> &phen)
{
pfa pfa1(n.pfa1.size);
assert(n.pfa1.size==24);
//pfa1.size = 24;
for (int i = 1; i<=24; i++) {pfa1.phase.push_back(phen[1]*(i-1));
pfa1.freq.push_back(phen[2]);pfa1.amp.push_back(1);}

n.pfa1.swap_all(pfa1);

}

void Worm2Dosc::setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_)
{

    if (evotype_=="Evo21"){
    Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(*(w2par_ptr_));
    Epars1.dbunit = 6;
    Epars1.vbunit = 30;
    }

}

void Worm2Dosc::setParsFromPheno(TVector<double> &phen)
{
    pars1.NMJweight = phen[4];
    //pars1.dbunit = 6;
    //pars1.vbunit = 30;
    cout << "Worm2Dosc::getParsFromPheno" << endl;
}



void Worm2DoscHalf::setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_)
{

    if (evotype_=="Evo21"){
    Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(*(w2par_ptr_));
    Epars1.dbunit = 1;
    Epars1.vbunit = 1;
    }

}

void Worm2DoscHalf::setParsFromPheno(TVector<double> &phen)
{
    pars1.NMJweight = phen[3];
    //pars1.dbunit = 1;
    //pars1.vbunit = 1;
}




vector<toFromWeight> Worm2Dosc1::makeDVMuscleConn(int offset)
{
    vector<toFromWeight> vec1;
    for (int to_musc=1;to_musc<=24;to_musc++){
    int from_neuron = to_musc+offset;
    toFromWeight tv({from_neuron,pars1.NMJweight},to_musc);
    vec1.push_back(tv);}
    return vec1;
}

vector<toFromWeight> Worm2DoscHalf::makeDVMuscleConn(double weig_amp)
{
    vector<toFromWeight> vec1;
    for (int to_musc=1;to_musc<=24;to_musc++){
    int from_neuron = to_musc;
    toFromWeight tv({from_neuron,pars1.NMJweight*weig_amp},to_musc);
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

void Worm2DPars::Step1()
{

    //n.setTime(t);
    // Update Body
    b.StepBody(settedStepSize);
    
    // Update Nervous System
    n_ptr->EulerStep(settedStepSize);
    
    // Interneuron input  //////////////////////
    /* for (int i = 1; i <= par1.N_units; i++){
        n_ptr->SetNeuronExternalInput(nn(DB, i), wAVB_DB * AVB);
        n_ptr->SetNeuronExternalInput(nn(VB, i), wAVB_VB * AVB);
        n_ptr->SetNeuronExternalInput(nn(DA, i), wAVA_DA * AVA);
        n_ptr->SetNeuronExternalInput(nn(VA, i), wAVA_VA * AVA);
    
    } */
    
    setMuscleInput();
    //setMuscleInputOrig(StepSize);   


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

//NSosc::NSosc(const pfa & pfa_):pfa1(pfa_){}

void Worm2Dosc::setPhenoNames(){
    addPhenoName("phase offset", 1);
    addPhenoName("DV phase offset", 2);
    addPhenoName("freq", 3);
    addPhenoName("NMJ weight", 4);
}



void Worm2Dosc::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
{
    
    cout << "GenPhenMapping" << endl;
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
   cout << "GenPhenMapping" << endl;

}

void Worm2DoscHalf::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
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
    vector<toFromWeight> vec1;
    int unit = 1;
    for (int to_musc = 1; to_musc < 5; to_musc++) 
    makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, pars1.NMJ_Gain, par1ref.N_neuronsperunit);
        
    for (int unit = 2; unit <= 5; unit++)
        for (int to_musc = 5 + 3*(unit-2); to_musc < 5 + 3*(unit-1); to_musc++)
            makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, pars1.NMJ_Gain, par1ref.N_neuronsperunit);

    for (int unit = 6; unit <= 7; unit++)
        for (int to_musc = 17 + 4*(unit-6); to_musc < 17 + 4*(unit-5); to_musc++)
            makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, pars1.NMJ_Gain, par1ref.N_neuronsperunit);
    
    cout << "made muscle con" << endl;
            //exit(1);
    return vec1;

}

vector<toFromWeight> Worm2Dosc21base::makeDorsalMuscleConn()
{
const int DN = 1;
vector<int> dorsalNeurons({DN});
vector<double> dorsalNMJ({pars1.NMJ_DN});
return makeMuscleConn(dorsalNeurons, dorsalNMJ);
}

vector<toFromWeight> Worm2Dosc21base::makeVentralMuscleConn()
{
const int VN = 2;
vector<int> ventralNeurons({VN});
vector<double> ventralNMJ({pars1.NMJ_VN});
return makeMuscleConn(ventralNeurons, ventralNMJ);
}

void Worm2DoscBase::addParsToJson(json & j)
{
    Worm2DPars::addParsToJson(j);
    //n.pfa1.addParsToJson(j["Nervous system"]);
    n.addParsToJson(j["Nervous system"]);
}

void Worm2DoscBase::construct(const string & filename_)
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

}

void Worm2DoscBase::construct(TVector<double> &pheno)
{
    setPfaFromPheno(pheno);
    setParsFromPheno(pheno);
    setUpMuscleConn();
    n.setTime(t);

}





void Worm2DoscBase::setPfaFromFile(const string & genofilename_)
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





void Worm2Dosc21::setPfaFromPheno(TVector<double> &phen)
{
assert(n.pfa1.size==14);
pfa pfa1(n.pfa1.size);
//pfa1.size = 14;
vector<double> phase_1(14,0);

for (int unit = 1; unit<=7; unit++){
const int neuron_d = nn(1,unit) - 1;
phase_1[neuron_d] = phen[1]*(unit-1);
const int neuron_v = nn(2,unit) - 1;
phase_1[neuron_v] = phen[1]*(unit-1) + phen[2];
}
pfa1.phase.swap(phase_1);

for (int i = 1; i<=14; i++) {pfa1.freq.push_back(phen[3]);pfa1.amp.push_back(1);}
n.pfa1.swap_all(pfa1);

}


void Worm2Dosc21all::setPfaFromPheno(TVector<double> &phen)
{
assert(n.pfa1.size==14);
pfa pfa1(n.pfa1.size);
//pfa1.size = 14;
vector<double> phase_1(14,0);

for (int unit = 1; unit<=7; unit++){
const int neuron_d = nn(1,unit) - 1;
phase_1[neuron_d] = phen[unit];
const int neuron_v = nn(2,unit) - 1;
phase_1[neuron_v] = phen[unit+7];
}
pfa1.phase.swap(phase_1);

for (int i = 1; i<=14; i++) {pfa1.freq.push_back(phen[15]);pfa1.amp.push_back(1);}
n.pfa1.swap_all(pfa1);

}

void CoupledOsc::setFromPheno(TVector<double> &pheno, int offset)
{
for (int i = 0; i<weights.size(); i++) weights[i].w.weight = pheno[i+1+offset];

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

void Worm2Dosc21Coup::setPfaFromPheno(TVector<double> &phen)
{
assert(n.pfa1.size==14);
//n.pfa1.size = 14;
const int & weisize = cn.weights.size();
vector<double> freq, amp(n.pfa1.size,1), phase(n.pfa1.size,0);
for (int i = 0; i<n.pfa1.size; i++) {freq.push_back(phen[weisize+1]);}
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


void Worm2Dosc21::setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_)
{
    if (evotype_=="Evo21"){
    Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(*(w2par_ptr_));
    Epars1.dbunit = nn(1,3);
    Epars1.vbunit = nn(2,3);
    }

}


void Worm2Dosc21::setParsFromPheno(TVector<double> &phen, int offset)
{
    pars1.NMJ_Gain_Map = phen[offset];
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    for (int i=1; i<=par1.N_muscles; i++)
    {
    pars1.NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*pars1.NMJ_Gain_Map)/par1.N_muscles));
    }
    
    pars1.NMJ_VN = phen[offset+1];
    pars1.NMJ_DN = phen[offset+2];
    //pars1.dbunit = nn(1,3);
    //pars1.vbunit = nn(2,3);
   
}


void Worm2Dosc21::setParsFromPheno(TVector<double> &phen)
{
    setParsFromPheno(phen,4);

}

void Worm2Dosc21S::setParsFromPheno(TVector<double> &phen)
{
    pars1.NMJ_Gain_Map = phen[4];

    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    for (int i=1; i<=par1.N_muscles; i++)
    {
    pars1.NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*pars1.NMJ_Gain_Map)/par1.N_muscles));
    }
    pars1.NMJ_DN = phen[5];
    pars1.NMJ_VN = phen[5];
    //pars1.dbunit = nn(1,3);
    //pars1.vbunit = nn(2,3);

}




void Worm2Dosc21all::setParsFromPheno(TVector<double> &phen)
{
    Worm2Dosc21::setParsFromPheno(phen,16);
   
}

void Worm2Dosc21Coup::setParsFromPheno(TVector<double> &phen)
{
    
    cn.setFromPheno(phen);
    const int & weisize = cn.weights.size();
    Worm2Dosc21::setParsFromPheno(phen,weisize+2);

}

void Worm2Dosc21CF::setParsFromPheno(TVector<double> &phen)
{
    assert(n.pfa1.size==14);
    cn.setFromPheno(phen);
    const int & weisize = cn.weights.size();
    Worm2Dosc21::setParsFromPheno(phen,weisize+n.pfa1.size+1);
    
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

void Worm2Dosc21::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
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


void Worm2Dosc21S::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
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


void Worm2Dosc21all::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
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


void Worm2Dosc21Coup::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
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

void Worm2Dosc21CF::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
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