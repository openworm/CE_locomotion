#include "owSignalSimulator.h"

class SignalSimulatorForWorm2D: public SignalSimulator{
public:
  SignalSimulatorForWorm2D(const std::string & simFileName, 
  const std::string & simClassName, float timeStep);
  SignalSimulatorForWorm2D(const std::string & simFileName, 
  const std::string & simClassName, const std::string & simDirName, float timeStep);
  const std::string & setUpSignalSimulator(const std::string & simDirName,
  const std::string & simFileName);

  void strValFunc(const std::string & funcName, const std::string & parName,
  const int & i, const double & value);

  ~SignalSimulatorForWorm2D();
  double getTwoValFunc(const std::string & funcName, const int & i, const int & j);
  double getValFunc(const std::string & funcName, const std::string & parName, const int & i);
  void oneValFunc(const std::string & simFileName, const int & i, const double & value);
  void twoValFunc(const std::string & simFileName, const int & i, const int & j, const double & value);
  //PyObject *pFuncNameSetNeuronInput, *pFuncNameSetSynapticWeight;
};