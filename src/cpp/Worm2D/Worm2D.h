#pragma once
//#include "../TSearch.h"
//#include "../VectorMatrix.h"
//#include "../Muscles.h"
//#include "../WormBody.h"
//#include "../NervousSystem.h"
//#include <nlohmann/json.hpp>
//#include "jsonUtils.h"
#include "../neuromlLocal/NSBaseForW2D.h"
#include "Evolvable.h"
//#include "StretchReceptorCE.h"

//datawriter->worm2dbase (nervous system and muscle pointers)
//datawriter->worm2dbody (just body plus functions)
//worm2dbase + worm2dbody -> worm2dm (nn ptr + musc ptr + body)
//worm2dm -> worm2d (cc musc + nn ptr plus nn to musc connections )
//worm2dm -> worm2d21m (nn+musc ptrs + body + net21 specifics)
//Worm2D21m + worm2d -> worm2d21 (cc musc + nn ptr + musc connections)
//worm2d21 -> worm21 (cc musc + cc nn + musc connections)

#ifdef W2D_VERSION_FROM_CMAKE
const string W2D_VERSION = W2D_VERSION_FROM_CMAKE;
#else
const string W2D_VERSION = "-Unknown-";
#endif

//class SRCE;

void setEvoStr(vector<string> & vecval, const vector<string> & evoName);
void getEvoNames1(json::const_iterator it2, vector<vector<string> > & evoNames, 
  vector<string> & path);
void getEvoNames(const json& j, vector<vector<string> > & evoNames, vector<string> & path);
void addEvoNames(json & j);


extern string main_directoryname, main_modelname;
int nn1(int neuronNumber, int unitNumber, int N_neuronsperunit);

void makeMuscleConnHelp1(vector<toFromWeight> & vec1, 
    const vector<int> & neurons, const vector<double> & NMJs, const int & mi, const int & to, 
    const TVector<double> & NMJ_Gain, const int &);
//string main_directoryname;
//string main_modelname;

void makeMuscleConnHelp1(vector<toFromWeight> & vec1, 
    const vector<int> & neurons, const vector<double> & NMJs, const int & unit, const int & to_muscle, 
    const vector<double> & NMJ_Gain, const int & N_neuronsperunit);


    

class baseParameters
{

    public:
    baseParameters(const json & itsJson_, shared_ptr<const CmdArgs> itsCmdArgs_)
    :BPitsJson(itsJson_),BPitsCmdArgs(itsCmdArgs_),defaultVals(setDefaultVals()){}

    baseParameters(shared_ptr<const CmdArgs> itsCmdArgs_)
    :BPitsCmdArgs(itsCmdArgs_),defaultVals(setDefaultVals()){}

    //baseParameters():defaultVals(setDefaultVals()){}


    template<class T>
    void setValCJWorm(const string & name_str, const T & val)
    {
        return setValCJ<T>(name_str,val,"Worm");
    }

    template<class T>
    void setValCJ(const string & name_str, const T & val, const string & bstr)
    {
        const string key_str = snakeCaseKey(name_str);
        const string section_str = canonicalSectionKey(bstr);
        if (!newSetVals.contains(section_str)) newSetVals[section_str] = json::object();
        if (!newSetVals.at(section_str).contains(key_str)) newSetVals[section_str][key_str] =  json::object();
        newSetVals[section_str][key_str]["value"] = val;

        addValToJson(name_str,val,bstr);
    }


    template<class T>
    bool getValCJ(const string & name_str, T & val, const string & bstr) 
    {
        const string key_str = snakeCaseKey(name_str);
        const string section_str = canonicalSectionKey(bstr);
        const string legacy_section_str = legacySectionKey(section_str);
       

        if (newSetVals.contains(section_str) && newSetVals.at(section_str).contains(key_str))
        {
             //cout << "hdjs ns " << name_str << " " << val << endl;
            val = newSetVals[section_str][key_str].at("value").get<T>();
            return true;
        }

        if (newSetVals.contains(section_str) && newSetVals.at(section_str).contains(name_str))
        {
            val = newSetVals[section_str][name_str].at("value").get<T>();
            return true;
        }

        const string legacy_key_str = legacyKeyForSnake(key_str);
        if (!legacy_key_str.empty() &&
            newSetVals.contains(section_str) && newSetVals.at(section_str).contains(legacy_key_str))
        {
            val = newSetVals[section_str][legacy_key_str].at("value").get<T>();
            return true;
        }

        if (legacy_section_str != section_str &&
            newSetVals.contains(legacy_section_str) && newSetVals.at(legacy_section_str).contains(key_str))
        {
            val = newSetVals[legacy_section_str][key_str].at("value").get<T>();
            return true;
        }

        if (legacy_section_str != section_str &&
            newSetVals.contains(legacy_section_str) && newSetVals.at(legacy_section_str).contains(name_str))
        {
            val = newSetVals[legacy_section_str][name_str].at("value").get<T>();
            return true;
        }

        if (!legacy_key_str.empty() && legacy_section_str != section_str &&
            newSetVals.contains(legacy_section_str) && newSetVals.at(legacy_section_str).contains(legacy_key_str))
        {
            val = newSetVals[legacy_section_str][legacy_key_str].at("value").get<T>();
            return true;
        }

    
        if (BPitsCmdArgs!=nullptr) {
            //cout << "hdjs cmd " << name_str << " " << val << endl;

         if( BPitsCmdArgs->getArgValT<T>("--" + key_str, val)
             || BPitsCmdArgs->getArgValT<T>("--" + name_str, val)
             || (!legacy_key_str.empty() && BPitsCmdArgs->getArgValT<T>("--" + legacy_key_str, val))) 
        {
              //cout << "hdjs cmd " << name_str << " " << val << endl;
            addValToJson(name_str,val,bstr);
            return true;
        }
    }

        if (!BPitsJson.empty() && BPitsJson.contains(section_str)) 
        {
            if (getJsonValTF<T>(BPitsJson.at(section_str), key_str, val, true)) return true;
            if (getJsonValTF<T>(BPitsJson.at(section_str), name_str, val, true)) return true;
            if (!legacy_key_str.empty() &&
                getJsonValTF<T>(BPitsJson.at(section_str), legacy_key_str, val, true)) return true;
        }

        if (!BPitsJson.empty() && legacy_section_str != section_str && BPitsJson.contains(legacy_section_str)) 
        {
            if (getJsonValTF<T>(BPitsJson.at(legacy_section_str), key_str, val, true)) return true;
            if (getJsonValTF<T>(BPitsJson.at(legacy_section_str), name_str, val, true)) return true;
            if (!legacy_key_str.empty() &&
                getJsonValTF<T>(BPitsJson.at(legacy_section_str), legacy_key_str, val, true)) return true;
        }

       
       
        if (defaultVals.contains(key_str)) {

         
            val = defaultVals.at(key_str).get<T>();
            addValToJson(name_str,val,bstr);
               //cout << "hdjs ds " << name_str << " " << val << endl;
            return true;
         }


       /*  if (itsCmdArgs!=nullptr && itsCmdArgs->getArgValT<T>("--" + name_str, val))
        {
            if (!newPars.contains(bstr)) newPars[bstr] = json::object();
            newPars[bstr][name_str]["value"] = val; 
            return true;
        }
        if (itsJson!=nullptr && itsJson->contains(bstr)) 
        if (getJsonValTF<T>(itsJson->at(bstr), name_str, val, true)) return true;
        if (defaultVals.contains(name_str)) {
        val = defaultVals.at(name_str).get<T>();
        if (!newPars.contains(bstr)) newPars[bstr] = json::object();
        newPars[bstr][name_str]["value"] = val; 
        return true;
        } */

        cout << "dffd " << defaultVals << endl;
        cout << "getValCJ " << name_str << " " << bstr << endl;
        assert(0);
        return false;
    }

    template<class T>
    bool getValCJWorm(const string & name_str, T & val)
    {

        return getValCJ<T>(name_str,val,"Worm");

    }
 
    template<class T>
    T getValCJWorm(const string & name_str)
    {
       
        T val;
        getValCJ<T>(name_str,val,"Worm");
        //cout <<  name_str << " " << val << endl;

        return val;
    }
     

    template<class T>
    bool getValCJEvo(const string & name_str, T & val)
    {

        return getValCJ<T>(name_str,val,"Evolutionary Optimization Parameters");

    }

    template<class T>
    bool getCmdVal(const string & name_str, T & val) const
    {
        return BPitsCmdArgs != nullptr
            && BPitsCmdArgs->getArgValT<T>("--" + name_str, val);
    }



    json setDefaultVals()
    {
        json defaultVals_;
        defaultVals_["random_initial_state"] = false;
        defaultVals_["do_orig_musc_input"] = true;
        defaultVals_["do_orig_sr_input"] = true;

        defaultVals_["reset_agent_body"] = false;
        defaultVals_["rotation"] = 0.0;
        defaultVals_["orient"] = 0.0;
        defaultVals_["grad_steep"] = 0.5;
        defaultVals_["run_duration"] = 1000;
        defaultVals_["max_dist"] = 4.5;
        defaultVals_["taxis"] = 1;
        defaultVals_["kinesis"] = 0;
        defaultVals_["sr_evo_bot"]=0;
        defaultVals_["sr_evo_top"]=200;
        defaultVals_["sr_evo_bot_a"]=0;
        defaultVals_["sr_evo_top_a"]=200;
        defaultVals_["ab_output_level"] = 1.0;
        defaultVals_["sr_type"] = "None";
        defaultVals_["sr_form"] = 0;
        defaultVals_["sr_seg_per_sr"] = 6;
        defaultVals_["sr_zero_gains_type"] = 1;
        defaultVals_["sr_offset"] = 0;
        defaultVals_["nmj_weight"] = 1;
        defaultVals_["do_reverse"] = 0;

        defaultVals_["do_test_run"] = true;

        defaultVals_["osc_tbase"] = 0.25; // Cap for oscillation evaluation
        defaultVals_["agarfreq"] = 0.44;
        defaultVals_["avg_speed"] = 0.00022; 

        defaultVals_["nmj_vn"] = 1; 
        defaultVals_["nmj_dn"] = 1; 
        defaultVals_["nmj_gain_map"] = 1;
        defaultVals_["fit_type"] = 0;
        defaultVals_["do_angle_diff"] = 0;
        defaultVals_["do_legacy"] = true;
        defaultVals_["init_ns_from_json"] = true;
        defaultVals_["input_ind"] = -1;
        defaultVals_["debug"] = false;
        defaultVals_["evo_type"] = "Evo21";

       return defaultVals_;
    }

    template<class T>
    void addValToJson(const string & name_str, const T & val, const string & bstr)
    {
        const string key_str = snakeCaseKey(name_str);
        const string legacy_key_str = legacyKeyForSnake(key_str);
        const string section_str = canonicalSectionKey(bstr);
        const string legacy_section_str = legacySectionKey(section_str);
        if (!BPitsJson.contains(section_str)) BPitsJson[section_str] = json::object();
        if (!BPitsJson.at(section_str).contains(key_str)) BPitsJson[section_str][key_str] = json::object();
        BPitsJson[section_str][key_str]["value"] = val;
        if (name_str != key_str) BPitsJson[section_str].erase(name_str);
        if (!legacy_key_str.empty() && legacy_key_str != key_str) BPitsJson[section_str].erase(legacy_key_str);
        if (legacy_section_str != section_str && BPitsJson.contains(legacy_section_str))
        {
            if (BPitsJson.at(legacy_section_str).is_object())
            {
                json merged = BPitsJson.at(legacy_section_str);
                merged.update(BPitsJson.at(section_str));
                BPitsJson[section_str] = merged;
                BPitsJson[section_str][key_str]["value"] = val;
                BPitsJson[section_str].erase(name_str);
                if (!legacy_key_str.empty() && legacy_key_str != key_str) BPitsJson[section_str].erase(legacy_key_str);
            }
            BPitsJson.erase(legacy_section_str);
        }
    }

   /*  void addParsToJson(json & j)
    {

    for(auto it = newPars.begin(); it != newPars.end(); ++it)
    {
        if (!j.contains(it.key())) j[it.key()] = it.value(); 
        if (j.contains(it.key()))  j[it.key()].push_back(it.value());

    }        
    }
 */
    //shared_ptr<const json itsJsonPtr()const {return &itsJson;} 

    const json & itsNewSetVals() const {return newSetVals;}
    const json & itsBPjson() const {return BPitsJson;}
    void cleanLegacyParameterKeys(json & j) const {removeLegacyParameterKeys(j);}
    
    friend class Efunctor;
    protected:
    void removeLegacyParameterKeys(json & j) const
    {
        if (!j.is_object()) return;
        mergeLegacySection(j, "worm", "Worm");
        for (auto section = j.begin(); section != j.end(); ++section)
        {
            if (!section.value().is_object()) continue;
            for (auto it = defaultVals.begin(); it != defaultVals.end(); ++it)
            {
                const string key_str = it.key();
                const string legacy_key_str = legacyKeyForSnake(key_str);
                if (!legacy_key_str.empty() && legacy_key_str != key_str &&
                    section.value().contains(legacy_key_str))
                {
                    if (!section.value().contains(key_str))
                        section.value()[key_str] = section.value()[legacy_key_str];
                    section.value().erase(legacy_key_str);
                }
            }
        }
        if (j.contains("worm")) j["worm"].erase("hs_step_size");
        if (j.contains("Evolutionary Optimization Parameters"))
        {
            j["Evolutionary Optimization Parameters"].erase("hs_step_size");
            j["Evolutionary Optimization Parameters"].erase("HSStepSize");
        }
    }

    static string canonicalSectionKey(const string & section)
    {
        if (section == "Worm") return "worm";
        return section;
    }

    static string legacySectionKey(const string & section)
    {
        if (section == "worm") return "Worm";
        return section;
    }

    static const json & getSectionWithLegacy(const json & j, const string & section)
    {
        const string section_str = canonicalSectionKey(section);
        if (j.contains(section_str)) return j.at(section_str);
        const string legacy_section_str = legacySectionKey(section_str);
        return j.at(legacy_section_str);
    }

    static json getSectionCopyWithLegacy(const json & j, const string & section)
    {
        const string section_str = canonicalSectionKey(section);
        const string legacy_section_str = legacySectionKey(section_str);
        json out = json::object();
        if (legacy_section_str != section_str && j.contains(legacy_section_str)
            && j.at(legacy_section_str).is_object())
            out.update(j.at(legacy_section_str));
        if (j.contains(section_str) && j.at(section_str).is_object())
            out.update(j.at(section_str));
        return out;
    }

    static void mergeLegacySection(json & j, const string & section, const string & legacy_section)
    {
        if (!j.is_object() || !j.contains(legacy_section)) return;
        if (!j.contains(section)) j[section] = json::object();
        if (j.at(section).is_object() && j.at(legacy_section).is_object())
        {
            json merged = j.at(legacy_section);
            merged.update(j.at(section));
            j[section] = merged;
        }
        j.erase(legacy_section);
    }

    static string snakeCaseKey(const string & name)
    {
        string key;
        for (string::size_type i = 0; i < name.size(); i++)
        {
            const char c = name[i];
            const bool is_upper = (c >= 'A' && c <= 'Z');
            const bool is_lower = (c >= 'a' && c <= 'z');
            const bool is_digit = (c >= '0' && c <= '9');
            const bool is_alnum = is_upper || is_lower || is_digit;

            if (!is_alnum)
            {
                if (!key.empty() && key.back() != '_') key.push_back('_');
                continue;
            }

            if (is_upper && !key.empty() && key.back() != '_')
            {
                const char prev = name[i-1];
                const bool prev_is_upper = (prev >= 'A' && prev <= 'Z');
                const bool prev_is_lower = (prev >= 'a' && prev <= 'z');
                const bool prev_is_digit = (prev >= '0' && prev <= '9');
                bool next_is_lower = false;
                if (i + 1 < name.size())
                {
                    const char next = name[i+1];
                    next_is_lower = (next >= 'a' && next <= 'z');
                }
                if (prev_is_lower || prev_is_digit || (prev_is_upper && next_is_lower))
                    key.push_back('_');
            }

            key.push_back(is_upper ? c - 'A' + 'a' : c);
        }

        if (!key.empty() && key.back() == '_') key.pop_back();
        return key;
    }

    static string legacyKeyForSnake(const string & key)
    {
        if (key == "random_initial_state") return "randomInitialState";
        if (key == "do_orig_musc_input") return "doOrigMuscInput";
        if (key == "do_orig_sr_input") return "doOrigSRInput";
        if (key == "reset_agent_body") return "resetAgentBody";
        if (key == "grad_steep") return "gradSteep";
        if (key == "run_duration") return "RunDuration";
        if (key == "hs_step_size") return "HSStepSize";
        if (key == "max_dist") return "MaxDist";
        if (key == "sr_evo_bot") return "SREvoBot";
        if (key == "sr_evo_top") return "SREvoTop";
        if (key == "sr_evo_bot_a") return "SREvoBotA";
        if (key == "sr_evo_top_a") return "SREvoTopA";
        if (key == "ab_output_level") return "AB_output_level";
        if (key == "sr_type") return "SRType";
        if (key == "sr_form") return "SRForm";
        if (key == "sr_seg_per_sr") return "SRSegPerSR";
        if (key == "sr_zero_gains_type") return "SRZeroGainsType";
        if (key == "sr_offset") return "SROffset";
        if (key == "nmj_weight") return "NMJWeight";
        if (key == "do_reverse") return "doReverse";
        if (key == "do_test_run") return "doTestRun";
        if (key == "osc_tbase") return "OSCTbase";
        if (key == "avg_speed") return "AvgSpeed";
        if (key == "nmj_vn") return "NMJ_VN";
        if (key == "nmj_dn") return "NMJ_DN";
        if (key == "nmj_gain_map") return "NMJ_Gain_Map";
        if (key == "fit_type") return "fitType";
        if (key == "do_angle_diff") return "doAngleDiff";
        if (key == "do_legacy") return "doLegacy";
        if (key == "init_ns_from_json") return "initNSFromJson";
        if (key == "input_ind") return "inputInd";
        if (key == "evo_type") return "evoType";
        return "";
    }

    json BPitsJson;
    //shared_ptr<const json> itsJson = nullptr;
    //shared_ptr<json> itsJson = nullptr;
    const shared_ptr<const CmdArgs> BPitsCmdArgs = nullptr;
    const json defaultVals;
    json newSetVals;

    
};

class Efunctor
{
public:

Efunctor(baseParameters & bp_):bp(bp_),condf(bp_.BPitsJson.contains("Funcable")){}

double eFunc(const double & val, const json & j, bool setItsJson = false);
//double eFunc(const double & val, const json & j);
//double eFunc1(const double & val, const json & j);

void reset(){itsJson = {};}
void setFunctionCondition(
    const int function_index, const bool has_condval, const int condval);

baseParameters & bp;
const bool condf;
json itsJson;
};



//using json = nlohmann::json;

#define PI 3.14159265

struct wormIzqParams
{
    int N_neuronsperunit;
    int N_muscles;
    double T_muscle;
    int N_units;
    int N_size;

    const doubIntParamsHead getParams() const
    {
        doubIntParamsHead var1;
        var1.parDoub.head = "worm";
        var1.parInt.head = "worm";
        var1.parDoub.names = {"T_muscle"};
        var1.parDoub.vals = {T_muscle};
        var1.parInt.names = {"N_neuronsperunit", "N_muscles", "N_units", "N_size"};
        var1.parInt.vals = {N_neuronsperunit, N_muscles, N_units, N_size};
        return var1;
    }

};

vector<toFromWeight> dummyVec();



class DataWriter{

    public:
   
    
    void writeDataCheck(){ 
        if (basename==".") {cout << "basename not set" << endl; throw std::exception();}
        writeData();
    }
    
    DataWriter()//:doFirstCall(true)
    {datatime=0;
    prefix="";
    basename=".";
    isOpen.clear();
    ofsvec.clear();
    ofnames.clear();
    tts.clear();
    }

    virtual ~DataWriter(){closeAll();}
    

    void incDatatime(double Stepsize_){datatime+=Stepsize_;}
    void setDataskips(double dataskips_){dataskips = dataskips_;}
    void setBasename(string basename_){basename=basename_;}
    void setPrefix(string prefix_){prefix=prefix_;}
    void setPrefix(){prefix=getModelName();}

    void dataReset();
    
    //void dataReset(){closeAll();}
    void closeAll();
    void InitializeData(string basename_);

    

    protected:
    virtual const string getModelName() = 0;
    size_t getPos(string name_);
    virtual void writeData() = 0; //{cout << "write data not implemented!" << endl;}

    //bool resetStats(bool & firstcall, size_t & pos, int & tt, string name_);
   
   
    string getName(string name_);

    //const bool doFirstCall;
    vector<bool> isOpen;
    vector<ofstream> ofsvec;
    vector<string> ofnames;
    vector<int> tts;

    int dataskips;
    double datatime;
    string basename;
    string prefix;
    
};




class InputSwitcher
{

  public:
  InputSwitcher(const json & j){construct(j);}
  InputSwitcher(){}

  protected:

  void setInputOnce(const json & j, const int & ind, vector<double> & externalInputs);

  void setInputOnce(const int & ind, vector<double> & externalInputs);
  void updateScheduledInput(
      const double & current_time, vector<double> & externalInputs);
  void resetScheduledInput();
  void activateScheduleForSimulation();
  void construct(const json & j);
 
  void setParsFromJson(const json & j){construct(j);}
  void addParsToJson(json & j) const;

  void swapVecsIS(vector<vector<int> > & inds_,
  vector<vector<double> > & vals_){inds.swap(inds_);
      vals.swap(vals_);}

  private:
  vector<int> scheduled_input_indices;
  vector<double> timeperiods;
  double time_offset = 0, total_period = 0;
  bool doEvolution = false;
  bool scheduleActive = false;
  int current_schedule_entry = -1;
  double previous_schedule_time = -1;
  vector<vector<int> > inds;
  vector<vector<double> > vals;
  //int inputInd = -1;
};

class Worm2Dbody : virtual public DataWriter
{

    public:

    //Worm2Dbody():DataWriter(){}
    double CoMx();
    double CoMy();
    void Curvature(TVector<double> &c);
    double Orientation();
    void AngleCurvature(TVector<double> &c);
    //void DumpBodyState(ofstream &ofs, int skips);
    virtual void InitializeState(RandomState &rs) = 0;
    double PositionX() const {return b.X(Head)*100.0;} //change to cm
    double PositionY() const {return b.Y(Head)*100.0;}
    void shiftX(double shiftdist_);
    void shiftY(double shiftdist_);
    void zeroX();
    void zeroY();
    //void ResetAgentsBody(shared_ptr<gradParameters> CO2DSRpars);
    void ResetAgentsBody(baseParameters & basePar_);

    double headDistanceToCenter() const;
    double headDistanceToLocation(const double & x, const double & y) const;
    void rotateBody(double theta);

    virtual void addParsToJson(json & j);
    double getVelocity();
    
    //virtual ~Worm2Dbody(){}
    virtual void writeBody();
    virtual void writeCurvature();
    WormBody b;

    protected:
    void writeData();
    

    bool first_call = true;
    double xtp = 0, ytp = 0;

};



struct baseConsts
{
bool debug;

};

class Worm2Dbase : public baseParameters, virtual public DataWriter, public InputSwitcher
{

public:

virtual void InitializeState(RandomState &rs) = 0;
virtual void initForSimulation(RandomState &) {setTime(0);return;}


void Step(double StepSize_);
void Step();
virtual void setStepSize(double val_){settedStepSize=val_;}

virtual void randomizeNS(RandomState &rs);
vector<double> readPhenotype();
virtual void writeAct();
void writeExtInp(ofstream & ofs);
void writeVNC(ofstream & ofs);
void writeMusc(ofstream & ofs);
void writeState();
virtual void addParsToJson(json & j);
void writeJsonFile(ofstream & json_out);
virtual void addEvolvableToJson(json & j) {return;}
virtual void addFuncableToJson(json & j) {return;}
virtual void applyScheduledFuncable(
    const int function_index, const bool has_condval, const int condval);
void addParsToJson();

const NSForW2D & itsNS() const {return *n_ptr;}
NSForW2D & itsNS(){return *n_ptr;}
virtual void DumpParams(ofstream &ofs) {return;}
void DumpNSOrdered();
void DumpVal(string filename_, double val);
virtual double getVelocity() = 0;
const wormIzqParams par1;
int nn(int neuronNumber, int unitNumber) const;

virtual const vector<string> getSectionNames() {return vector<string>(par1.N_size, "vnc");}

virtual ~Worm2Dbase(){
        if (m_ptr) delete m_ptr; 
        if (n_ptr) delete n_ptr;
}

virtual void setTime(double t_){
    t=t_;
    datatime=t_;
    InputSwitcher::resetScheduledInput();
    resetFuncableSchedules();
}
const double & itsStepSize() const {return settedStepSize;}
void incSimTimes();

//virtual shared_ptr<const W2Dparameters> setWormPars(int argc, const char* argv[]) {return nullptr;}
//virtual void setWormPars(const W2Dparameters * w2par_) {assert(0);}

//virtual shared_ptr<const W2Dparameters> setWormPars(shared_ptr<const CmdArgs> cmd) {return nullptr;}

virtual void setWormPars(shared_ptr<const CmdArgs> cmd_) 
{
    //BPitsCmdArgs = cmd_;
    //W2Dbaseparameters1b->setPars(cmd);

}

// not this, shared_ptr<W2Dbaseparameters> W2Dbaseparameters1;

//shared_ptr<baseParameters> basePar1 = nullptr;

//baseParameters basePar1;

//void setBasePar(shared_ptr<baseParameters> basePar1_){basePar1=basePar1_;}

//shared_ptr<W2Dparameters> W2Dbaseparameters1b = nullptr;

void zeroAllInputs(){
    for (int i=0;i<par1.N_size;i++)
    n_ptr->SetNeuronExternalInput(i+1, 0);
}

template<class T> friend class Evolvable_ptrB;

void setInputOnce(const int & ind) {InputSwitcher::setInputOnce(ind,externalInputs);}
void activateInputScheduleForSimulation() {
    InputSwitcher::activateScheduleForSimulation();
}
void activateFuncableSchedulesForSimulation();
const vector<double> & itsExternalInputs() const {return externalInputs;}
const vector<toFromWeight> & itsExternalInputConn() const {
    return externalInputConn;
}
virtual const vector<string> getDistinctCellNames() {return {"not implemented"};}

Efunctor itsEf;

protected:
struct FuncableSchedule
{
    int function_index = -1;
    vector<double> time_intervals;
    vector<int> condvals;
    double time_offset = 0, total_period = 0;
    bool doEvolution = false, active = false;
    int current_entry = -1;
    double previous_time = -1;
};

vector<FuncableSchedule> funcableSchedules;
void constructFuncableSchedules(const json & j);
void resetFuncableSchedules();
void updateScheduledFuncables(const double current_time);

//Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc);

Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_,
    shared_ptr<const CmdArgs> cmd_);

Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_,
    shared_ptr<const CmdArgs> cmd_, const json & j);

//Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_);
//Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, shared_ptr<W2Dparameters>);

//Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc, 
//    shared_ptr<W2Dbaseparameters> w2dpar_);

static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd, const json & j);

void writeData();
virtual void setPhenoNames() {return;}

virtual vector<doubIntParamsHead> getWormParams() {
    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;
    parvec.push_back(var1);
    return parvec;}


virtual void Step1() = 0;
NSForW2D * const n_ptr = nullptr;
muscForW2D * m_ptr = nullptr;
    
vector<string> phenoNames;
vector<int> phenoNamesNums;

//vector<double> sjdkdsdjddssdsloe;


void addPhenoName(string name, int k);

double t = 0; // Time


double settedStepSize = 0.01;

void makeExternalInputConnFromJson(const json & j);
virtual void makeExternalInputConn(){return;}
vector<toFromWeight> externalInputConn;
vector<double> externalInputs;
//vector<double> sjdkdsdjddssdsloe;
//double sjdkdsdjddssdsloe;
void setExternalInput();
//virtual void assignExternalInput(){fill(externalInputs.begin(), externalInputs.end(), 0);}
virtual void assignExternalInput(){return;}

void assignExternalInputOnce(const int & ind, const double & val){externalInputs[ind]=val;}

vector<toFromWeight> NSInputConn, NSOutputConn;
void incInputFromNS(NSForW2D & ns_);
void incOutputToNS(Worm2Dbase & ns_);
virtual void makeNSInputConn(){return;}
virtual void makeNSOutputConn(){return;}
//namedValVec<double> doubVars;
json namedVars;
static wormIzqParams getIzqPars(const json & j);

baseConsts makeBaseConsts();
const baseConsts baseconsts;


};



class Worm2Dm : public Worm2Dbody, public Worm2Dbase //Worm2Dm has body
{
    public:

    //virtual void Step(double StepSize, double output) = 0;

    virtual void InitializeState(RandomState &rs);
    //virtual vector<doubIntParamsHead> getWormParams() = 0;
    
    //virtual void initForSimulation() =  0;

    virtual const vector<string> getCellNames() {return {"not implemented"};}
    virtual const vector<string> getCellNamesUnit() {return {"not implemented"};}
    

    virtual void setMuscleInput() {return;}
    double getVelocity(){return Worm2Dbody::getVelocity();}
    virtual void addParsToJson(json & j);
    virtual ~Worm2Dm(){}
    const vector<toFromWeight> &  itsvBodyConnvec() const {return vBodyConnvec;}

    void writeData();

    protected:
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, 
    //bool mfwc, shared_ptr<W2Dbaseparameters> w2dpar_);

    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<const CmdArgs> cmd_ = nullptr);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<const CmdArgs> cmd_, const json & j);

    //Worm2Dm(wormIzqParams par1_, shared_ptr<W2Dbaseparameters>);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, 
    //    shared_ptr<W2Dparameters> w2dpar_);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, 
    //muscForW2D * m_ptr_, shared_ptr<W2Dbaseparameters> w2dpar_);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, bool);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, bool, shared_ptr<const CmdArgs> cmd_ = nullptr);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, bool, shared_ptr<const CmdArgs> cmd_, const json & j);

    //const bool muscForWDconst;
    void setBodyInput(); //takes muscle outputs to drive body segments
    virtual vector<toFromWeight> makeBodyConn();
    virtual vector<toFromWeight> makeVentralBodyConn(); //from muscles to body
    virtual vector<toFromWeight> makeDorsalBodyConn();
    void setUpBodyConn();
    void setUpBodyConn(const json & j);
    void setBodExt(const json & j);
    void setBodExt();

    bool W2Dmparscalled, W2Dminitcalled;
    //shared_ptr<W2Dbaseparameters> W2Dbaseparameters1;
    vector<toFromWeight> vBodyConnvec, dBodyConnvec;
    
     void Step1();

};




class Worm2D : virtual public Worm2Dm //Worm2Dm has muscles
{
    
    public:
    //virtual void InitializeState(RandomState &rs) = 0;
    //virtual void DumpBodyState(ofstream &ofs, int skips) = 0;
    //virtual void DumpCurvature(ofstream &ofs, int skips) = 0;

    
    void addParsToJson(json & j);

    //virtual ~Worm2D(){if (n_ptr) delete n_ptr;}
    //NSForW2D & itsNS(){return *n_ptr;}

    void InitializeState(RandomState &rs);
    //void writeData(){Worm2Dm::writeData();}
    
    virtual void preNStep(){assert(0);}
    virtual void postNStep(){assert(0);}

    const vector<toFromWeight> & itsvMuscConnvec() const {return vMuscConnvec;}
 
    const vector<toFromWeight> & itsdMuscConnvec() const {return dMuscConnvec;}

    protected:

    
    virtual const vector<string> getVMuscNames() {return {"not implemented"};}
    virtual const vector<string> getDMuscNames() {return {"not implemented"};}

    //virtual void addExtraParsToJson(json & j) = 0;
    virtual vector<toFromWeight> makeVentralMuscleConn() {assert(0);} //from neurons to muscles
    virtual vector<toFromWeight> makeDorsalMuscleConn() {assert(0);}  //from neurons to muscles


    //void addMuscleParsToJson(json & j);
    void setUpMuscleConn(); //calls make dorsal and ventral musccon to set up connections. 
    void setUpMuscleConn(const json & j);

    void makeMuscleConnHelp(vector<toFromWeight> & vec1, 
    const vector<int> & neurons, const vector<double> & NMJs, 
    const int & unit, const int & to_muscle, const TVector<double> & NMJ_Gain);

    //void makeMuscleConnHelp(vector<toFromWeight> & vec1, 
    //vector<int> neurons, vector<double> NMJs, int mi, int to, TVector<double> & NMJ_Gain);

    //vector<toFromWeight> makeMuscleConn(vector<int> dorsalNeurons, vector<double> dorsalNMJ);
    //vector<toFromWeight> makeMuscleConnW2D(vector<int> neurons, vector<double> NMJ,
    //TVector<double> & NMJ_Gain, vector<intPair> & unitToMusc);


    vector<toFromWeight> makeMuscleConnW2D(const vector<int> & neurons, const vector<double> & NMJ,
    const TVector<double> & NMJ_Gain, const vector<intPair> & unitToMusc);


    virtual void setMuscleInputOrig(){assert(0 && "setMuscleInputOrig needs overriding");}
    void setMuscleInput(); //calls setMuscleInputVec()
    void setMuscleInputVec(); //takes neuron output, inputs it to muscles using connection vector

    void setMuscBodExt();
    void setMuscBodExt(const json & j);

    Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_);
    Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_, bool forceNoOrigInputs);
    //Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_, json & j);
    //void setMuscleInputVent();
    //void setMuscleInputDors();
    //Worm2D();
    void Step1();
    void setUp();
    Muscles & m;
    
    //NSToMuscles vMuscConn, dMuscConn;
    vector<toFromWeight> vMuscConnvec, dMuscConnvec;
        
    //shared_ptr<W2Dbaseparameters> W2Dbaseparameters1;  //change this back

    vector<weightentry> ventinds, dorsinds;
    vector<intPair> unitToMuscV, unitToMuscD;
    //double NMJ_gain_map_V, NMJ_gain_map_D, NMJ_gain_fact = 0.7;
    
    const bool doOrigMuscInput, doOrigSRInput;
    bool hasVNCNMJ = false, hasVNC18 = false;
    vector<toFromWeight> makeMuscleConnVNCV();
    vector<toFromWeight> makeMuscleConnVNCD();
    vector<toFromWeight> makeVentralMuscleConn18();
    vector<toFromWeight> makeDorsalMuscleConn18();

};






class WormFR 
{
public:
virtual void setForward() = 0;
virtual void setBackward() = 0;
//virtual void randomizeNS(RandomState &rs)  = 0;

};

class WormGrad
{
public:
virtual void ResetAgentsBody()  = 0;
virtual double distanceToCenter() const = 0;
virtual void InitializeSensors(RandomState& rs) = 0;
virtual void setGradientSteepness(const double &) {}

};
