#include "Worm2Dmods.h"
#include "../neuromlLocal/c302ForW2D.h"


void pfa::swap_all(pfa & pfa_){
size = pfa_.size;
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


Worm2DPars::Worm2DPars(wormIzqParams par1_, NSForW2D * n_ptr_, W2Dparameters * w2par_ptr):
Worm2Dm(par1_,n_ptr_),pars1_ptr(w2par_ptr),Worm2D(par1_,0){}


Worm2DoscNML::Worm2DoscNML(int size_):Worm2Dm({size_,24,0.1,1,size_}, new c302ForW2D()),
Worm2DPars({size_,24,0.1,1,size_}, 0, new Worm2Doscpars1()),
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



Worm2Dosc::Worm2Dosc():Worm2Dosc(48){}


Worm2Dosc::Worm2Dosc(int size_):Worm2Dosc1(dynamic_cast<Worm2Doscpars1&>(*pars1_ptr)),
Worm2DoscBase({size_,24,0.1,1,size_}, new Worm2Doscpars()),
pars1(dynamic_cast<Worm2Doscpars&>(*pars1_ptr)),Worm2Dm({size_,24,0.1,1,size_},new NSosc())
{

    TVector<double> pheno;
    pheno.SetBounds(1,getVectSize());
    //pheno.SetBounds(1,evoVectSize);
    construct(pheno);
}


Worm2Dosc21NML::Worm2Dosc21NML():Worm2DPars({2,24,0.1,7,14}, 0, new Worm2Dosc21pars()),
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


Worm2Dosc21D::Worm2Dosc21D(const string & filename_):
Worm2Dosc21<Worm2Dosc21D>::Worm2Dosc21(filename_),
Worm2DoscBase<Worm2Dosc21D>({2,24,0.1,7,14}, new Worm2Dosc21pars()),
Worm2Dm({2,24,0.1,7,14},new NSosc())
{}

Worm2Dosc21D::Worm2Dosc21D(TVector<double> & pheno, const bool & isPheno):
Worm2Dosc21<Worm2Dosc21D>::Worm2Dosc21(pheno,isPheno),
Worm2DoscBase<Worm2Dosc21D>({2,24,0.1,7,14}, new Worm2Dosc21pars()),
Worm2Dm({2,24,0.1,7,14},new NSosc())
{}




Worm2Dosc21allD::Worm2Dosc21allD(const string & filename_):
Worm2Dosc21all<Worm2Dosc21allD>::Worm2Dosc21all(filename_),
Worm2DoscBase<Worm2Dosc21allD>({2,24,0.1,7,14}, new Worm2Dosc21pars()),
Worm2Dm({2,24,0.1,7,14},new NSosc())
{}
Worm2Dosc21allD::Worm2Dosc21allD(TVector<double> & pheno, const bool & isPheno):
Worm2Dosc21all<Worm2Dosc21allD>::Worm2Dosc21all(pheno,isPheno),
Worm2DoscBase<Worm2Dosc21allD>({2,24,0.1,7,14}, new Worm2Dosc21pars()),
Worm2Dm({2,24,0.1,7,14},new NSosc())
{}

Worm2DoscHalf::Worm2DoscHalf():Worm2Dosc(24),Worm2Dm({24,24,0.1,1,24},new NSosc()){}


/* Worm2Dosc::Worm2Dosc(const Worm2Dosc& w):
Worm2DoscBase(w.pars1, new Worm2Doscpars()),
Worm2DoscBase(new Worm2Doscpars(w.pars1),w.n.pfa1.size),
Worm2Dm({w.n.pfa1.size,24,0.1,1,w.n.pfa1.size}, new NSosc(w.n.pfa1), new Muscles),
pars1(dynamic_cast<Worm2Doscpars&>(*pars1_ptr))
{
    cout << "Worm2Dosc copy" << endl;
    setUpMuscleConn();
    n.setTime(t);
} */

/* 
Worm2Dosc::Worm2Dosc(const pfa & pfa_, const Worm2Doscpars & pars1_):
Worm2DoscBase(new Worm2Doscpars(pars1_),pfa_.size),
Worm2Dm({pfa_.size,24,0.1,1,pfa_.size}, new NSosc(pfa_), new Muscles),
pars1(dynamic_cast<Worm2Doscpars&>(*pars1_ptr))
{
    cout << "Worm2Dosc" << endl;
    setUpMuscleConn();
    n.setTime(t);
}
 */

Worm2Dosc::Worm2Dosc(const string & filename_):Worm2Dosc(48)
{
    construct(filename_);
}



Worm2Dosc::Worm2Dosc(TVector<double> & phengen, const bool & isPheno):Worm2Dosc(48)
{
    if (isPheno) construct(phengen);
    else constructFromGeno(phengen);
}



Worm2DoscHalf::Worm2DoscHalf(const string & filename_):Worm2Dosc(24),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    construct(filename_);   
}

Worm2DoscHalf::Worm2DoscHalf(TVector<double> & pheno, const bool & isPheno):Worm2Dosc(24),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
     if (isPheno) construct(pheno);
    else constructFromGeno(pheno);
   
}

/* Worm2DoscHalf::Worm2DoscHalf(const pfa & pfa_, const Worm2Doscpars & pars1_):
Worm2Dosc(pfa_,pars1_),Worm2Dm({pfa_.size,24,0.1,1,pfa_.size}, new NSosc(pfa_), new Muscles){} */


//Worm2DoscHalf::Worm2DoscHalf(const string & filename_):
//Worm2DoscHalf(getPfaFromFile(filename_,3), getParsFromFile(filename_,3)){}

//Worm2Dosc::Worm2Dosc(const string & filename_):
//Worm2Dosc(getPfaFromFile(filename_,4), getParsFromFile(filename_,4)){}

//Worm2Dosc::Worm2Dosc(TVector<double> & geno_):
//Worm2Dosc(getPfaFromGeno(geno_), getParsFromGeno(geno_)){}



/* void Worm2DoscBase::InitializeState(RandomState &rs)
{    
    cout << "Worm2Dosc init state" << endl;
    Worm2D::InitializeState(rs);
    return;    
} */



void Worm2Dosc::setPfaFromPheno(TVector<double> &phen)
{
    
pfa pfa1;
pfa1.size = 48;
for (int i = 1; i<=24; i++) pfa1.phase.push_back(phen[1]*(i-1));
for (int i = 25; i<=48; i++) pfa1.phase.push_back(phen[1]*(i-25) + phen[2]);
for (int i = 1; i<=48; i++) {pfa1.freq.push_back(phen[3]);pfa1.amp.push_back(1);}

n.pfa1.swap_all(pfa1);

}

void Worm2DoscHalf::setPfaFromPheno(TVector<double> &phen)
{
pfa pfa1;
pfa1.size = 24;
for (int i = 1; i<=24; i++) {pfa1.phase.push_back(phen[1]*(i-1));
pfa1.freq.push_back(phen[2]);pfa1.amp.push_back(1);}

n.pfa1.swap_all(pfa1);

}



void Worm2Dosc::setParsFromPheno(TVector<double> &phen)
{
    pars1.NMJweight = phen[4];
    pars1.dbunit = 6;
    pars1.vbunit = 30;
    cout << "Worm2Dosc::getParsFromPheno" << endl;
}


void Worm2DoscHalf::setParsFromPheno(TVector<double> &phen)
{
    pars1.NMJweight = phen[3];
    pars1.dbunit = 1;
    pars1.vbunit = 1;
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

NSosc::NSosc(const pfa & pfa_):pfa1(pfa_){}

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

