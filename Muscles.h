//
// LocomotionCircuit.h
//
//  Copyright (c) 2015 Eduardo Izquierdo. All rights reserved.
//

#include "VectorMatrix.h"
#include "random.h"
#include <cmath>
#include "neuromlLocal/NSBaseForW2D.h"
using namespace std;

class Muscles : public muscForW2D{
public:

    Muscles(int nmuscles = 24, double t_muscle = 0.1);

    void SetMuscleParams(int nmuscles, double t_muscle);

    void InitializeMuscleState();

    void EulerStep(double StepSize);

    void SetDorsalMuscleInput(int muscle, double input){V_input[muscle][1] = input;};
    void SetVentralMuscleInput(int muscle, double input){V_input[muscle][2] = input;};

    void SetDorsalMuscleActivation(int muscle, double activation){V_muscle[muscle][1] = activation;};
    void SetVentralMuscleActivation(int muscle, double activation){V_muscle[muscle][2] = activation;};

    double DorsalMuscleOutput(int muscle){return V_muscle[muscle][1];};
    double VentralMuscleOutput(int muscle){return V_muscle[muscle][2];};


    void ventralMuscInputOut(){
        cout << "vmusc"; 
    for (int i=1;i<=Nmuscles;i++) cout << " " << V_input[i][2];
    cout << endl;
    
    }

    void dorsalMuscInputOut(){
        cout << "dmusc"; 
    for (int i=1;i<=Nmuscles;i++) cout << " " << V_input[i][1];
    cout << endl;
    
    }

    TMatrix<double> V_muscle;
    TMatrix<double> V_input;

    double T_muscle;
    int Nmuscles;
};
