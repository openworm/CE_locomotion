#include "c302NervousSystem.h"

using namespace std;

int main (int argc, const char* argv[])
{

c302NervousSystem n("main_sim");

/* fout = open('Worm2D.outputs-test.dat', 'w')
    for i in range(0,10000):
        out_str = str(i) + listToStr(w.run())
        #time_vals.append(i)
        #out_vals.append(w.run())
        #f_outputs_file_f2.write(i)
        #f_outputs_file_f2.write(w.run())
        fout.write(out_str)
        fout.write('\n')
    #import numpy as np
    #out_arr = np.array(out_vals)

    fout.close()    
    w.save_results() */

//n.EulerStep(1);
//return 0;

//n.setSimulator("main_sim", "Worm2DNRNSimulation", 0.005);
for (int i=0;i<10;i++){
n.EulerStep(1);
cout << n.NeuronOutput(1) << endl;
cout << n.NeuronOutput(2) << endl;
cout << n.NeuronOutput(3) << endl;
}

return 0;
}