# Neuromechanical model of forward and backward locomotion in _C. elegans_

This repository contains all the classes necessary to evolve a neuromechanical model of _C. elegans_ to produce forward and backward locomotion. 

We are using this to better understand how the same neural circuit can produce multiple behaviors (i.e., forward and backward) and modulate between them through command neurons.

Work in collaboration with Dr. Erick Olivares and Prof. Randall Beer.

## Instructions for use

1. Compile using the Makefile: 
```
$ make
```
2. Perform an evolutionary run (takes aprox. 2 minutes): 
```
$ ./main
```
3. Visualize the evolutionary progress: 
```
$ python viz.py
```
4. Visualize the movement of the body using Mathematica: 
```
$ Mathematica viz.nb
```

![Screenshot of the animation of the simulated worm's body in Mathematica](https://github.com/openworm/CE_locomotion/blob/main/viz.png?raw=true)



5. Visualize the dynamic neural and muscle activity:
```
$ python load_data.py
```
![Activity in neurons, muscles and stretch receptors](https://github.com/openworm/CE_locomotion/blob/main/ExampleActivity.png?raw=true)



