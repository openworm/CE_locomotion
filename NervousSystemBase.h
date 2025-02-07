#pragma once
#include <functional>

using std::function;




template<class T>
class NervousSystemInt
{
    public:
    NervousSystemInt():derived(static_cast<T&>(*this)){}

    void EulerStep(double stepsize)
    {
    //T& underlying = static_cast<T&>(*this);
    derived.EulerStep(stepsize);
    //func(stepsize, EulerStep);
    }
    
    double NeuronOutput(int i)
    {
    //T& underlying = static_cast<T&>(*this);
    derived.NeuronOutput(i);
    }
    
    private:
   
    T& derived; // = static_cast<T&>(*this);

    void func(double a, function<void(double)> calc) {
    T& underlying = static_cast<T&>(*this);
    underlying.calc(a);
    }

    double func(double a, function<double(int,int)> calc) {
    T& underlying = static_cast<T&>(*this);
    underlying.calc(a);
    }
   

};

class NervousSystemBase {
    public:
        //virtual int debugTest(void)  = 0;
        virtual int CircuitSize(void)  = 0;
        virtual void SetCircuitSize(int newsize, int maxchemconns, int maxelecconns) = 0;
        virtual double NeuronState(int i) = 0;
        virtual void SetNeuronState(int i, double value) = 0;
        virtual double NeuronOutput(int i) = 0;
        virtual void SetNeuronOutput(int i, double value) = 0;
        virtual double NeuronBias(int i) = 0;
        virtual void SetNeuronBias(int i, double value) = 0;
        virtual double NeuronGain(int i) = 0;
        virtual void SetNeuronGain(int i, double value) = 0;
        virtual double NeuronTimeConstant(int i) = 0;
        virtual void SetNeuronTimeConstant(int i, double value) = 0;
        virtual double NeuronExternalInput(int i) = 0;
        virtual void SetNeuronExternalInput(int i, double value) = 0;
        virtual double ChemicalSynapseWeight(int from, int to) = 0;
        virtual void SetChemicalSynapseWeight(int from, int to, double value) = 0;
        virtual double ElectricalSynapseWeight(int from, int to) = 0;
        virtual void InternalSetElectricalSynapseWeight(int from, int to, double value) = 0;
        virtual void SetElectricalSynapseWeight(int n1, int n2, double value) = 0;
        virtual void EulerStep(double stepsize) = 0;
 
        virtual ~NervousSystemBase(){};

};
