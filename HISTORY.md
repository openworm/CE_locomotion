# Neuromechanical model of forward and backward locomotion in _C. elegans_

A listing of the various publications and code releases related to this model of C. elegans forward and backward locomotion by Dr. Eduardo J. Izquiredo, Dr. Erick Olivares and Prof. Randall Beer.

## 2013

### Connecting a Connectome to Behavior: An Ensemble of Neuroanatomical Models of C. elegans Klinotaxis

**Eduardo J. Izquierdo, Randall D. Beer**

https://journals.plos.org/ploscompbiol/article?id=10.1371/journal.pcbi.1002890

[SSID](https://www.semanticscholar.org/paper/Connecting-a-Connectome-to-Behavior%3A-An-Ensemble-of-Izquierdo-Beer/658a9534388597f425a898e29e31a11f0d315311)

This paper develops a neuroanatomically-grounded model of salt klinotaxis, a form of chemotaxis in which changes in orientation are directed towards the source through gradual continual adjustments.

<details><summary>Abstract</summary>
Increased efforts in the assembly and analysis of connectome data are providing new insights into the principles underlying the connectivity of neural circuits. However, despite these considerable advances in connectomics, neuroanatomical data must be integrated with neurophysiological and behavioral data in order to obtain a complete picture of neural function. Due to its nearly complete wiring diagram and large behavioral repertoire, the nematode worm Caenorhaditis elegans is an ideal organism in which to explore in detail this link between neural connectivity and behavior. In this paper, we develop a neuroanatomically-grounded model of salt klinotaxis, a form of chemotaxis in which changes in orientation are directed towards the source through gradual continual adjustments. We identify a minimal klinotaxis circuit by systematically searching the C. elegans connectome for pathways linking chemosensory neurons to neck motor neurons, and prune the resulting network based on both experimental considerations and several simplifying assumptions. We then use an evolutionary algorithm to find possible values for the unknown electrophsyiological parameters in the network such that the behavioral performance of the entire model is optimized to match that of the animal. Multiple runs of the evolutionary algorithm produce an ensemble of such models. We analyze in some detail the mechanisms by which one of the best evolved circuits operates and characterize the similarities and differences between this mechanism and other solutions in the ensemble. Finally, we propose a series of experiments to determine which of these alternatives the worm may be using.
</details>

## 2016

### Propagation of rhythmic dorsoventral wave in a neuromechanical model of locomotion in Caernohabditis elegans

**Eduardo J. Izquierdo, Randall D. Beer**


https://direct.mit.edu/isal/proceedings/alif2016/28/544/99414

<details><summary>Abstract</summary>
With 302 neurons and a fully reconstructed connectome, Caernohabditis elegans is an ideal candidate organism to study how behavior is grounded in the interaction between an organism's brain, its body, and its environment. Since nearly its entire behavioral repertoire is expressed through movement, understanding the neuromechanical basis of locomotion is especially critical as a foundation upon which analyses of all other behaviors must build. In this extended abstract, we report on the evolution and analysis of an integrated neuromechanical model of forward locomotion.
</details>




## 2018

### From head to tail: a neuromechanical model of forward locomotion in Caenorhabditis elegans

**Eduardo J. Izquierdo, Randall D. Beer**

https://royalsocietypublishing.org/doi/full/10.1098/rstb.2017.0374


<details><summary>Abstract</summary>
With 302 neurons and a near-complete reconstruction of the neural and muscle anatomy at the cellular level, Caenorhabditis elegans is an ideal candidate organism to study the neuromechanical basis of behaviour. Yet despite the breadth of knowledge about the neurobiology, anatomy and physics of C. elegans, there are still a number of unanswered questions about one of its most basic and fundamental behaviours: forward locomotion. How the rhythmic pattern is generated and propagated along the body is not yet well understood. We report on the development and analysis of a model of forward locomotion that integrates the neuroanatomy, neurophysiology and body mechanics of the worm. Our model is motivated by experimental analysis of the structure of the ventral cord circuitry and the effect of local body curvature on nearby motoneurons. We developed a neuroanatomically grounded model of the head motoneuron circuit and the ventral nerve cord circuit. We integrated the neural model with an existing biomechanical model of the worm's body, with updated musculature and stretch receptors. Unknown parameters were evolved using an evolutionary algorithm to match the speed of the worm on agar. We performed 100 evolutionary runs and consistently found electrophysiological configurations that reproduced realistic control of forward movement. The ensemble of successful solutions reproduced key experimental observations that they were not designed to fit, including the wavelength and frequency of the propagating wave. Analysis of the ensemble revealed that head motoneurons SMD and RMD are sufficient to drive dorsoventral undulations in the head and neck and that short-range posteriorly directed proprioceptive feedback is sufficient to propagate the wave along the rest of the body.
</details>

## 2018

### Potential role of a ventral nerve cord central pattern generator in forward and backward locomotion in Caenorhabditis elegans

**Erick Olivares, Eduardo J. Izquierdo, Randall D. Beer**

https://direct.mit.edu/netn/article/2/3/323/5431

A computational model grounded in the available neuroanatomy and neurophysiology to explore the space of possible configurations of the circuit that matched the neural traces observed during forward and backward locomotion in the worm. 


<details><summary>Abstract</summary>
C. elegans locomotes in an undulatory fashion, generating thrust by propagating dorsoventral bends along its body. Although central pattern generators (CPGs) are typically involved in animal locomotion, their presence in C. elegans has been questioned, mainly because there has been no evident circuit that supports intrinsic network oscillations. With a fully reconstructed connectome, the question of whether it is possible to have a CPG in the ventral nerve cord (VNC) of C. elegans can be answered through computational models. We modeled a repeating neural unit based on segmentation analysis of the connectome. We then used an evolutionary algorithm to determine the unknown physiological parameters of each neuron so as to match the features of the neural traces of the worm during forward and backward locomotion. We performed 1,000 evolutionary runs and consistently found configurations of the neural circuit that produced oscillations matching the main characteristic observed in experimental recordings. In addition to providing an existence proof for the possibility of a CPG in the VNC, we suggest a series of testable hypotheses about its operation. More generally, we show the feasibility and fruitfulness of a methodology to study behavior based on a connectome, in the absence of complete neurophysiological details.
</details>


## 2021

### A Neuromechanical Model of Multiple Network Rhythmic Pattern Generators for Forward Locomotion in C. elegans

**Erick Olivares, Eduardo J. Izquierdo, Randall D. Beer**

https://www.frontiersin.org/journals/computational-neuroscience/articles/10.3389/fncom.2021.572339/full


<details><summary>Abstract</summary>
Multiple mechanisms contribute to the generation, propagation, and coordination of the rhythmic patterns necessary for locomotion in Caenorhabditis elegans. Current experiments have focused on two possibilities: pacemaker neurons and stretch-receptor feedback. Here, we focus on whether it is possible that a chain of multiple network rhythmic pattern generators in the ventral nerve cord also contribute to locomotion. We use a simulation model to search for parameters of the anatomically constrained ventral nerve cord circuit that, when embodied and situated, can drive forward locomotion on agar, in the absence of pacemaker neurons or stretch-receptor feedback. Systematic exploration of the space of possible solutions reveals that there are multiple configurations that result in locomotion that is consistent with certain aspects of the kinematics of worm locomotion on agar. Analysis of the best solutions reveals that gap junctions between different classes of motorneurons in the ventral nerve cord can play key roles in coordinating the multiple rhythmic pattern generators.
</details>

