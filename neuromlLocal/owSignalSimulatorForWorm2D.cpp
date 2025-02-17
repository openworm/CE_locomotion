#include "owSignalSimulatorForWorm2D.h"

SignalSimulatorForWorm2D::SignalSimulatorForWorm2D(const std::string & simFileName, 
  const std::string & simClassName, float timeStep):
  SignalSimulator(simFileName,simClassName,timeStep)
  //pFuncNameSetNeuronInput(Py_BuildValue("s", "set_neuron_input")),
  //pFuncNameSetSynapticWeight(Py_BuildValue("s", "set_synaptic_weight"))
  {}


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




void SignalSimulatorForWorm2D::setNeuronInput(int i, double value)
{
    //set_neuron_input(self, i, weight):
    return oneValFunc("set_neuron_input",i,value);
    
}


void SignalSimulatorForWorm2D::SetChemicalSynapseWeight(int from, int to, double value) 
{
    return twoValFunc("set_synaptic_weight",from,to,value);
}

SignalSimulatorForWorm2D::~SignalSimulatorForWorm2D()
{
  //Py_DECREF(pFuncNameSetNeuronInput);
  //Py_DECREF(pFuncNameSetSynapticWeight);
}