// =============================================================
// Tests for C++ code
// =============================================================

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <cassert>
#include "TSearch.h"
#include "NervousSystem.h"
#include "Worm2D.h"

extern string output_dir_name;


void testNervousSystem()
{
    NervousSystem n;
    
    output_dir_name = "exampleRun_pop6"; //fix this
    ifstream NS_ifs(rename_file("worm_data.json"));
    setNSFromJsonFile(NS_ifs, n);
    NS_ifs.close();  


    double Duration = 100;
    double StimDel = 10;
    double StimDur = 50;
    double StimAmp = 1;
    const double StepSize = 0.005;

    ofstream state_file("test_output_2/test.state.dat");
    ofstream output_file("test_output_2/test.output.dat");

    for (double t = 0.0; t <= Duration; t += StepSize)
    {

        //std::cout << "Time: " << t << "" << std::endl;
        if (t > StimDel && t < (StimDel + StimDur))
        {
            n.SetNeuronExternalInput(1, StimAmp);
        }
        else
        {
            n.SetNeuronExternalInput(1, 0);
        }
        n.EulerStep(StepSize);
        state_file << t << " ";
        output_file << t << " ";
        for (int i = 1; i <= n.size; i += 1)
        {
            double st = n.NeuronState(i);
            double ou = n.NeuronOutput(i);
            //std::cout << "  Neuron: " << i << ", state: " << st << ", output: " << ou << "" << std::endl;

            state_file << st << " ";
            output_file << ou << " ";
        }
        state_file << "\n";
        output_file << "\n";
    }
    state_file.close();
    output_file.close();
}

int main(int argc, const char *argv[])
{

    // Many more tests should be added!

    std::cout << "Test simulation of optimized nervous system..." << std::endl;
    
    testNervousSystem();

    std::cout << "Done!" << std::endl;

    return 0;
}
