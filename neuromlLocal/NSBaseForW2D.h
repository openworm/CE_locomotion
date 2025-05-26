#pragma once


class NSForW2D {
    public:
    
virtual double NeuronOutput(int i) = 0;
virtual double NeuronState(int i) = 0;
virtual void SetNeuronExternalInput(int i, double value) = 0;
virtual void EulerStep(double stepsize) = 0;
virtual ~NSForW2D(){};

};

class muscForW2D
{
    public:
    

//virtual void EulerStep(double stepsize) = 0;

//virtual void SetDorsalMuscleInput(int muscle, double input) = 0;
//virtual void SetVentralMuscleInput(int muscle, double input) = 0;
virtual double DorsalMuscleOutput(int muscle) = 0;
virtual double VentralMuscleOutput(int muscle) = 0;


virtual ~muscForW2D(){};

};

