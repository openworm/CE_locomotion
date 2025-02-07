#pragma once
//#include <functional>


/* #include <concepts>

using std::function;

template <typename T>
concept Clock = requires(T c) { 
    c.start();  
    c.stop();
    c.getTime();
  }; */


template<class T>
class NervousSystemInt
{
    public:
    NervousSystemInt():derived(static_cast<T *>(this)){}
      
    //int CircuitSize(void) {return static_cast<T *>(this)->CircuitSize();};
    void SetCircuitSize(int newsize, int maxchemconns, int maxelecconns)
    {return derived->SetCircuitSize(newsize,maxchemconns,maxelecconns);}
    //{return static_cast<T *>(this)->SetCircuitSize(newsize,maxchemconns,maxelecconns);}
    //double NeuronState(int i) {return static_cast<T *>(this)->NeuronState(i);}
    //void SetNeuronState(int i, double value) {return static_cast<T *>(this)->SetNeuronState(i,value);}
    double NeuronOutput(int i) {return derived->NeuronOutput(i);}
    void SetNeuronOutput(int i, double value) {return derived->SetNeuronOutput(i,value);}
    double NeuronBias(int i) {return derived->NeuronBias(i);}
    void SetNeuronBias(int i, double value) {return derived->SetNeuronBias(i,value);}
    //double NeuronGain(int i) {return derived->NeuronGain(i);}
    void SetNeuronGain(int i, double value) {return derived->SetNeuronGain(i,value);}
    double NeuronTimeConstant(int i) {return derived->NeuronTimeConstant(i);}
    void SetNeuronTimeConstant(int i, double value) {return derived->SetNeuronTimeConstant(i,value);}
    //double NeuronExternalInput(int i) {return derived->NeuronExternalInput(i);}
    void SetNeuronExternalInput(int i, double value) {return derived->SetNeuronExternalInput(i,value);}
    double ChemicalSynapseWeight(int from, int to) {return derived->ChemicalSynapseWeight(from,to);}
    void SetChemicalSynapseWeight(int from, int to, double value) {return derived->SetChemicalSynapseWeight(from,to,value);}
    double ElectricalSynapseWeight(int from, int to) {return derived->ElectricalSynapseWeight(from,to);}
    //void InternalSetElectricalSynapseWeight(int from, int to, double value) {return derived->InternalSetElectricalSynapseWeight(from,to,value);} 
    void SetElectricalSynapseWeight(int n1, int n2, double value) {return derived->SetElectricalSynapseWeight(n1,n2,value);} 
    void EulerStep(double stepsize) {return derived->EulerStep(stepsize);}
    
    operator T&(){return static_cast<T&>(*this);}
    //operator T(){return static_cast<T>(*this);}
    //operator T*(){return static_cast<T *>(this);}

    private:
    T * const derived; 

    //T& derived; // = static_cast<T&>(*this);

/*     void func(double a, function<void(double)> calc) {
    T& underlying = static_cast<T&>(*this);
    underlying.calc(a);
    }

    double func(double a, function<double(int,int)> calc) {
    T& underlying = static_cast<T&>(*this);
    underlying.calc(a);
    } */
   

};

