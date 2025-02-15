#include "owSignalSimulator.h"

class SignalSimulatorForWorm2D: public SignalSimulator{
public:
  SignalSimulatorForWorm2D(const std::string & simFileName, 
  const std::string & simClassName, float timeStep):SignalSimulator(simFileName,simClassName,timeStep){}
  void setNeuronInput(int i, double value);
};