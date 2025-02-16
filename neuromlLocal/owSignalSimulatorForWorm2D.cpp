#include "owSignalSimulatorForWorm2D.h"

void SignalSimulatorForWorm2D::setNeuronInput(int i, double value)
{
    //set_neuron_input(self, i, weight):
    PyObject *neuron_number = Py_BuildValue("i", i); // Create tuple of arguments for initialization
    PyObject *pFuncName = Py_BuildValue("s", "set_neuron_input");
    PyObject *neuron_weight = Py_BuildValue("f", value);

    //pInstance = PyObject_CallMethod(pInstance, "set_timestep", "(f)", timeStep);

    try {
      PyObject_CallMethodObjArgs(pInstance, pFuncName, neuron_number, neuron_weight, nullptr);
      //PyObject_CallMethod(pInstance, const_cast<char *>("run"), nullptr);

    } catch (const std::exception& ex)
    {
      std::cout << "Error: " << ex.what() << std::endl;
    }

    if (PyErr_Occurred()) PyErr_Print();

    Py_DECREF(neuron_number);
    Py_DECREF(neuron_weight);
    Py_DECREF(pFuncName);

}

void SignalSimulatorForWorm2D::SetChemicalSynapseWeight(int from, int to, double value) 
{

    PyObject *pre_neuron_number = Py_BuildValue("i", from); // Create tuple of arguments for initialization
    PyObject *post_neuron_number = Py_BuildValue("i", to); 
    PyObject *pFuncName = Py_BuildValue("s", "set_synaptic_weight");
    PyObject *neuron_weight = Py_BuildValue("f", value);

    //set_synaptic_weight(self, pre, post, weight):
    //pInstance = PyObject_CallMethod(pInstance, "set_timestep", "(f)", timeStep);

    try {
      PyObject_CallMethodObjArgs(pInstance, pFuncName, pre_neuron_number, post_neuron_number, neuron_weight, nullptr);
      //PyObject_CallMethod(pInstance, const_cast<char *>("run"), nullptr);

    } catch (const std::exception& ex)
    {
      std::cout << "Error: " << ex.what() << std::endl;
    }

    if (PyErr_Occurred()) PyErr_Print();

    Py_DECREF(pre_neuron_number);
    Py_DECREF(post_neuron_number);
    Py_DECREF(neuron_weight);
    Py_DECREF(pFuncName);


}