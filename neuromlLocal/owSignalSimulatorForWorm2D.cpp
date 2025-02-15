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