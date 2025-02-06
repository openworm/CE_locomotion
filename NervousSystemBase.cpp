#include "NervousSystemBase.h"

template<class T> 
void NervousSystemInt<T>::EulerStep(double stepsize)
{
    T& underlying = static_cast<T&>(*this);
    underlying.EulerStep(stepsize);
}

