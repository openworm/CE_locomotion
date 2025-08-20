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

void Worm2DoscBase::addParsToJson(json & j)
{
    Worm2DPars::addParsToJson(j);
    n.pfa1.addParsToJson(j["Nervous system"]);

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

Worm2DoscBase::Worm2DoscBase(wormIzqParams par1_, W2Dparameters * w2par_ptr):
Worm2DPars(par1_, 0, w2par_ptr), Worm2Dm(par1_, new NSosc()),
Epars1(dynamic_cast<Evolparameters&>(*pars1_ptr)),
n(dynamic_cast<NSosc&>(*n_ptr)){}


Worm2Dosc::Worm2Dosc():Worm2Dosc(48){}


Worm2Dosc::Worm2Dosc(int size_):Worm2Dosc1(dynamic_cast<Worm2Doscpars1&>(*pars1_ptr)),
Worm2DoscBase({size_,24,0.1,1,size_}, new Worm2Doscpars()),
pars1(dynamic_cast<Worm2Doscpars&>(*pars1_ptr)),Worm2Dm({size_,24,0.1,1,size_},new NSosc())
{

    TVector<double> pheno;
    pheno.SetBounds(1,getVectSize());
    construct(pheno);
}

Worm2Dosc21::Worm2Dosc21():
Worm2DoscBase({2,24,0.1,7,14}, new Worm2Dosc21pars()),
pars1(dynamic_cast<Worm2Dosc21pars&>(*pars1_ptr)),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
}

Worm2Dosc21::Worm2Dosc21(const string & filename_):
Worm2DoscBase({2,24,0.1,7,14}, new Worm2Dosc21pars()),
pars1(dynamic_cast<Worm2Dosc21pars&>(*pars1_ptr)),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    construct(filename_);
}

Worm2Dosc21::Worm2Dosc21(TVector<double> & pheno, const bool & isPheno):
Worm2DoscBase({2,24,0.1,7,14}, new Worm2Dosc21pars()),
pars1(dynamic_cast<Worm2Dosc21pars&>(*pars1_ptr)),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    if (isPheno) construct(pheno);
    else constructFromGeno(pheno);
}



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




void Worm2DoscBase::setParsFromFile(const string & filename_)
{
    ifstream ifs;
    ifs.open(filename_);
    TVector<double> bestVector(1, getVectSize());
    //assert(0);
    ifs >> bestVector;
    ifs.close();
    setParsFromGeno(bestVector);
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





void Worm2DoscBase::setParsFromGeno(TVector<double> &v)
{
    
    //cout << v << endl;
    TVector<double> phenotype(1, v.Size());
    //cout << phenotype.Size() << endl;
    GenPhenMapping(v, phenotype);
 
    setParsFromPheno(phenotype);
 
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

//void Worm2DoscBase::writeJson(TVector<double> &){}

evoPars Worm2DoscBase::getDefaultEvoPars(){ return {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        100, 2000, 0.1, 0.5, UNIFORM, 
        1.1, 0.04, 1, 0, 0, 10, 40.0, 10.0, 0.005, 23, getVectSize()};}



/* double Worm2DoscBase::EvaluationFunction(TVector<double> &geno, RandomState &rs){

  
    cout << "Worm2Dosc::EvaluationFunction" << endl;

    const double OSCT = 0.25 * ep_ptr->Duration; // Cap for oscillation evaluation
    const double agarfreq = 0.44;
    const double    AvgSpeed = 0.00022;             // Average speed of the worm in meters per seconds
    const double    BBCfit = AvgSpeed*ep_ptr->Duration;

    const double & Duration = ep_ptr->Duration;
    assert(ep_ptr->VectSize == getVectSize());
    const int & VectSize = ep_ptr->VectSize ;
    const double & StepSize = ep_ptr->StepSize;
    const int & N_curvs = ep_ptr->N_curvs;
    const double & Transient = ep_ptr->Transient;
    //const int & skip_steps = ep_ptr->skip_steps;

   
    TVector<double> phenotype(1, VectSize);
    GenPhenMapping(geno, phenotype);
    //setPfaFromPheno(phenotype);
    //setParsFromPheno(phenotype);
    construct(phenotype);
    //setUpMuscleConn();
    InitializeState(rs);
    initForSimulation(rs);
    setStepSize(StepSize);

        // Fitness
        double fitness_tr = 0.0;
        double bodyorientation, anglediff;
        double movementorientation, distancetravelled = 0, displacement, temp;
        TVector<double> curvature(1, N_curvs);
        TVector<double> antpostcurv(1, 2);
        antpostcurv.FillContents(0.0);
    
        // Evaluation of B-class neuron oscillation,and frequency in segment 2.
        // The index of B class in this segment correspond to DBs2 = 10; VBs2 = 13
        double DBp, VBp, dDB, dVB;
        double oscDB = 0, oscVB = 0;
        double FoDB, FoVB, FfDB, FfVB;
    
        double freqDB=0, freqVB=0;
        int pDB = 0, pVB = 0, signtagDB, signtagVB, signDB, signVB;
        TVector<double> peaksDB(1, 2*Duration);
        TVector<double> peaksVB(1, 2*Duration);// longer vector if you want frequencies higer than 2 Hz.
    
        
        // Genotype-Phenotype Mapping
        //Worm21 w(phenotype);
        
        

        // Transient XXX
        //SetAVB(0.0);
        //SetAVA(0.0);
        
        for (double t = 0.0; t <= Transient; t += StepSize){
            Step();
        }    
        
        DBp = n.NeuronOutput(Epars1.dbunit);
        VBp = n.NeuronOutput(Epars1.vbunit);
    
        Step(); // determine sign of derivative
    
        dDB = n.NeuronOutput(Epars1.dbunit) - DBp;
        dVB = n.NeuronOutput(Epars1.vbunit) - VBp;
        signtagDB = (dDB  > 0) ? 1 : -1;
        signtagVB = (dVB  > 0) ? 1 : -1;
        DBp = n.NeuronOutput(Epars1.dbunit);
        VBp = n.NeuronOutput(Epars1.vbunit);
        
        double xt = CoMx(), xtp;
        double yt = CoMy(), ytp;
    
        // Time loop
        for (double t = 0.0; t <= Duration; t += StepSize) {
            // Step simulation
            Step();
            
            ///// Oscilation
            // check changes in sign of derivative
            dDB = n.NeuronOutput(Epars1.dbunit) - DBp;
            dVB = n.NeuronOutput(Epars1.vbunit) - VBp;
            signDB = (dDB  > 0) ? 1 : ((dDB  < 0) ? -1 : 0);
            signVB = (dVB  > 0) ? 1 : ((dVB  < 0) ? -1 : 0);
    
            oscDB += abs(DBp - n.NeuronOutput(Epars1.dbunit));
            oscVB += abs(VBp - n.NeuronOutput(Epars1.vbunit));
    
            if ((signDB == -1) and (signtagDB >= 0)){
                pDB +=1;
                peaksDB[pDB] = t;
                if (pDB >= 2*Duration){return 0;};
            }
            if ((signVB == -1) and (signtagVB >= 0)){
                pVB +=1;
                peaksVB[pVB] = t;
                if (pVB >= 2*Duration){return 0;};
            }
    
            signtagDB = signDB;
            signtagVB = signVB;
            DBp = n.NeuronOutput(Epars1.dbunit);
            VBp = n.NeuronOutput(Epars1.vbunit);
            
            //// Locomotion
            // Current and past centroid position
            xtp = xt; ytp = yt;
            xt = CoMx(); yt = CoMy();
            
            // Integration error check
            if (isnan(xt) || isnan(yt) || sqrt(pow(xt-xtp,2)+pow(yt-ytp,2)) > 100*AvgSpeed*StepSize){
                return 0;
            }
            
            // Fitness
            bodyorientation = Orientation();                  // Orientation of the body position
            movementorientation = atan2(yt-ytp,xt-xtp);         // Orientation of the movement
            anglediff = movementorientation - bodyorientation;  // Check how orientations align
            temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
            distancetravelled += temp * sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));
    
        }
        // B Oscillation evaluation
        if ((pDB < 2) or (pVB < 2)){return 0;};
        for (int i = 1; i<pDB; i+=1){freqDB += (1./(pDB-1))*(1./(peaksDB[i+1]- peaksDB[i]));} 
        for (int i = 1; i<pVB; i+=1){freqVB += (1./(pVB-1))*(1./(peaksVB[i+1]- peaksVB[i]));} 
    
        FfDB = fabs(freqDB - agarfreq)/agarfreq < 1 ? fabs(freqDB - agarfreq)/agarfreq : 1;
        FfVB = fabs(freqVB - agarfreq)/agarfreq < 1 ? fabs(freqVB - agarfreq)/agarfreq : 1;
    
        FoDB = oscDB > OSCT ? 1 : oscDB / OSCT;
        FoVB = oscVB > OSCT ? 1 : oscVB / OSCT;
    
        // Locomotion evaluation
        fitness_tr = (1 - (fabs(BBCfit-distancetravelled)/BBCfit));
    
   
        return fitness_tr * FoDB * FoVB * (1 - FfDB) * (1 - FfVB);
    } */






vector<toFromWeight> Worm2Dosc21::makeMuscleConn(vector<int> neurons, vector<double> NMJ)
{
    vector<toFromWeight> vec1;
    int unit = 1;
    for (int to_musc = 1; to_musc < 5; to_musc++) 
    makeMuscleConnHelp(vec1, neurons, NMJ, unit, to_musc, pars1.NMJ_Gain);
        
    for (int unit = 2; unit <= 5; unit++)
        for (int to_musc = 5 + 3*(unit-2); to_musc < 5 + 3*(unit-1); to_musc++)
            makeMuscleConnHelp(vec1, neurons, NMJ, unit, to_musc, pars1.NMJ_Gain);

    for (int unit = 6; unit <= 7; unit++)
        for (int to_musc = 17 + 4*(unit-6); to_musc < 17 + 4*(unit-5); to_musc++)
            makeMuscleConnHelp(vec1, neurons, NMJ, unit, to_musc, pars1.NMJ_Gain);
    
    cout << "made muscle con" << endl;
            //exit(1);
    return vec1;

}

vector<toFromWeight> Worm2Dosc21::makeDorsalMuscleConn()
{
const int DN = 1;
vector<int> dorsalNeurons({DN});
vector<double> dorsalNMJ({pars1.NMJ_DN});
return makeMuscleConn(dorsalNeurons, dorsalNMJ);
}

vector<toFromWeight> Worm2Dosc21::makeVentralMuscleConn()
{
const int VN = 2;
vector<int> ventralNeurons({VN});
vector<double> ventralNMJ({pars1.NMJ_VN});
return makeMuscleConn(ventralNeurons, ventralNMJ);
}


void Worm2Dosc21::setPfaFromPheno(TVector<double> &phen)
{

pfa pfa1;
pfa1.size = 14;
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

void Worm2Dosc21::setParsFromPheno(TVector<double> &phen)
{
    double NMJ_Gain_Map = phen[4];
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    for (int i=1; i<=par1.N_muscles; i++)
    {
    pars1.NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/par1.N_muscles));
    }
    
    pars1.NMJ_VN = phen[5];
    pars1.NMJ_DN = phen[6];
    pars1.dbunit = nn(1,3);
    pars1.vbunit = nn(2,3);
   
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


