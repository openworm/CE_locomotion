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

