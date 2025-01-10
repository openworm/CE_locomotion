// =============================================================
// Tests for C++ code
// =============================================================

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <cassert>
#include "TSearch.h"
#include "NervousSystem.cpp"
#include "Worm2D.cpp"

extern string output_dir_name;

void setUpNS(NervousSystem & n)
{

    std::cout << "Created simple Nervous System..." << std::endl;
    int N_units = 2;
    int N_neuronsperunit = 2;
    n.SetCircuitSize(N_units * N_neuronsperunit, 3, 2);

    for (int i = 1; i <= n.size; i += 1)
    {
        n.SetNeuronBias(i, 0);
        n.SetNeuronGain(i, 0);
        n.SetNeuronTimeConstant(i, 1);
    }

    n.SetChemicalSynapseWeight(1, 2, 1);
    //n.SetChemicalSynapseWeight(1, 3, .75);

}


void testNervousSystem()
{
    NervousSystem n;
    //setUpNS(n);
    

    output_dir_name = "exampleRun"; //fix this
    ifstream NS_ifs(rename_file("worm_data.json"));
    setNSFromJsonFile(NS_ifs, n);
    NS_ifs.close();  


    double Duration = 10;
    double StimDel = 2;
    double StimDur = 6;
    double StimAmp = 1;
    const double StepSize = 0.005;

    ofstream state_file("test_output/test.state.dat");
    ofstream output_file("test_output/test.output.dat");

    for (double t = 0.0; t <= Duration; t += StepSize)
    {

        std::cout << "Time: " << t << "" << std::endl;
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
            std::cout << "  Neuron: " << i << ", state: " << st << ", output: " << ou << "" << std::endl;

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

    std::cout << "Running a number of tests..." << std::endl;

    std::cout << clip(10, -10, 20) << std::endl;

    assert(clip(10, -10, 20) == 10.0);
    assert(clip(1, -1, 1) == 1);
    assert(clip(-1.1, -1, 1) == -1);
    assert(clip(0, -1, 1) == 0);
    assert(clip(0, 1, 1) == 1);

    std::cout << MapSearchParameter(-1, 0, 10, -1.0e99, 1.0e99) << std::endl;

    assert(MapSearchParameter(-1, 0, 10) == 0);
    assert(MapSearchParameter(1, 0, 10) == 10);
    assert(MapSearchParameter(1, 0, 10, -5, 5) == 5);

    testNervousSystem();

    std::cout << "Done!" << std::endl;

    return 0;
}
