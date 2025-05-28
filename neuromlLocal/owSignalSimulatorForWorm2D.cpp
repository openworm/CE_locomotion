#include "owSignalSimulatorForWorm2D.h"

SignalSimulatorForWorm2D::SignalSimulatorForWorm2D(const std::string & simFileName, 
  const std::string & simClassName, float timeStep):
  SignalSimulator(simFileName,simClassName,timeStep){}

SignalSimulatorForWorm2D::SignalSimulatorForWorm2D(const std::string & simFileName, 
  const std::string & simClassName, const std::string & simDirName, float timeStep):
  SignalSimulator(setUpSignalSimulator(simDirName, simFileName),simClassName,timeStep){}

const std::string & SignalSimulatorForWorm2D::setUpSignalSimulator(const std::string & simDirName,
const std::string & simFileName)
  {
    Py_Initialize();
    if (strcmp(simDirName.c_str(), "parent")==0){
    PyRun_SimpleString(
      "import os, sys \n"
      "sys.path.append('..') \n");
    }
    else
    {
     PyRun_SimpleString(("import os, sys\nsys.path.append('" +  simDirName + "')\n").c_str());
    } 
  return simFileName;
  }

double SignalSimulatorForWorm2D::getValFunc(const std::string & funcName, const std::string & parName,
const int & i)
{
PyObject *py_par = Py_BuildValue("s", parName.c_str());
PyObject *py_i = Py_BuildValue("i", i); 
PyObject *pFuncName = Py_BuildValue("s", funcName.c_str());

//PyObject *pValue = PyObject_CallMethod(pInstance, const_cast<char *>(funcName), py_i, nullptr);
PyObject *pValue = PyObject_CallMethodObjArgs(pInstance, pFuncName, py_par, py_i, nullptr);

  if (PyErr_Occurred()) {
      PyErr_Print();
      throw std::runtime_error("Exception in simulator getValFunc (printed above)");
  }
  
  double pValue_doub =  (double) PyFloat_AsDouble(pValue);

  Py_DECREF(pFuncName);
  Py_DECREF(py_i);
  Py_DECREF(py_par);
  Py_DECREF(pValue);
  return pValue_doub;
}

double SignalSimulatorForWorm2D::getTwoValFunc(const std::string & funcName, const int & i, const int & j)
{
PyObject *py_i = Py_BuildValue("i", i); 
PyObject *pFuncName = Py_BuildValue("s", funcName.c_str());
PyObject *py_j = Py_BuildValue("i", j); 

//PyObject *pValue = PyObject_CallMethod(pInstance, const_cast<char *>(funcName), py_i, nullptr);
PyObject *pValue = PyObject_CallMethodObjArgs(pInstance, pFuncName, py_i, py_j, nullptr);

  if (PyErr_Occurred()) {
      PyErr_Print();
      throw std::runtime_error("Exception in simulator getValFunc (printed above)");
  }
  
  double pValue_doub =  (double) PyFloat_AsDouble(pValue);

  Py_DECREF(pFuncName);
  Py_DECREF(py_i);
  Py_DECREF(py_j);
  Py_DECREF(pValue);
  return pValue_doub;
}

void SignalSimulatorForWorm2D::oneValFunc(const std::string & funcName, const int & i, const double & value)
{
    PyObject *py_i = Py_BuildValue("i", i); // Create tuple of arguments for initialization
    PyObject *py_value = Py_BuildValue("f", value);
    PyObject *pFuncName = Py_BuildValue("s", funcName.c_str());

    //pInstance = PyObject_CallMethod(pInstance, "set_timestep", "(f)", timeStep);

    try {
      PyObject_CallMethodObjArgs(pInstance, pFuncName, py_i, py_value, nullptr);
      //PyObject_CallMethod(pInstance, const_cast<char *>("run"), nullptr);

    } catch (const std::exception& ex)
    {
      std::cout << "Error: " << ex.what() << std::endl;
    }

    if (PyErr_Occurred()) PyErr_Print();
    
    Py_DECREF(pFuncName);
    Py_DECREF(py_i);
    Py_DECREF(py_value);
    return;
}

//set_neuron_parameter(self, parameter, i, val):


void SignalSimulatorForWorm2D::strValFunc(const std::string & funcName, const std::string & parName,
const int & i, const double & value)
{
    PyObject *py_i = Py_BuildValue("i", i); // Create tuple of arguments for initialization
    PyObject *py_value = Py_BuildValue("f", value); 
    PyObject *pFuncName = Py_BuildValue("s", funcName.c_str());
    PyObject *py_par = Py_BuildValue("s", parName.c_str());

    //pInstance = PyObject_CallMethod(pInstance, "set_timestep", "(f)", timeStep);

    try {
      PyObject_CallMethodObjArgs(pInstance, pFuncName, py_par, py_i, py_value, nullptr);
      //PyObject_CallMethod(pInstance, const_cast<char *>("run"), nullptr);

    } catch (const std::exception& ex)
    {
      std::cout << "Error: " << ex.what() << std::endl;
    }

    if (PyErr_Occurred()) PyErr_Print();


    Py_DECREF(pFuncName);
    Py_DECREF(py_i);
    Py_DECREF(py_par);
    Py_DECREF(py_value);
    return;
}


void SignalSimulatorForWorm2D::twoValFunc(const std::string & funcName, 
const int & i, const int & j, const double & value)
{
    PyObject *py_i = Py_BuildValue("i", i); // Create tuple of arguments for initialization
    PyObject *py_value = Py_BuildValue("f", value);
    PyObject *py_j = Py_BuildValue("i", j); 
    PyObject *pFuncName = Py_BuildValue("s", funcName.c_str());

    //pInstance = PyObject_CallMethod(pInstance, "set_timestep", "(f)", timeStep);

    try {
      PyObject_CallMethodObjArgs(pInstance, pFuncName, py_i, py_j, py_value, nullptr);
      //PyObject_CallMethod(pInstance, const_cast<char *>("run"), nullptr);

    } catch (const std::exception& ex)
    {
      std::cout << "Error: " << ex.what() << std::endl;
    }

    if (PyErr_Occurred()) PyErr_Print();


    Py_DECREF(pFuncName);
    Py_DECREF(py_i);
    Py_DECREF(py_j);
    Py_DECREF(py_value);
    return;
}



/* 
void SignalSimulatorForWorm2D::setNeuronInput(int i, double value)
{
    //set_neuron_input(self, i, weight):
    return oneValFunc("set_neuron_input",i,value);
    
}


void SignalSimulatorForWorm2D::SetChemicalSynapseWeight(int from, int to, double value) 
{
    return twoValFunc("set_synaptic_weight",from,to,value);
} */

SignalSimulatorForWorm2D::~SignalSimulatorForWorm2D()
{
  //Py_DECREF(pFuncNameSetNeuronInput);
  //Py_DECREF(pFuncNameSetSynapticWeight);
}