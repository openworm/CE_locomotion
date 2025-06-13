# Neural model of spatial orientation in C. elegans

This repository contains all the classes necessary to evolve a neural model of C. elegans to orient to a source in a petri dish. The code includes options to evolve klinotaxis and klinokinesis.

We are using this to better understand how the same neural circuit can produce two different behavioral strategies (i.e., klinotaxis and klinokinesis) simultaneously.

Earlier parts of this work were generated in collaboration with Prof. Shawn Lockery and Prof. Randall Beer. Related work has been published:

Izquierdo, E.J., and Lockery, S.R.  (2010) <a href="https://www.jneurosci.org/content/30/39/12908.abstract">Evolution and analysis of minimal neural circuits for klinotaxis in C. elegans</a>. Journal of Neuroscience 30:12908-12817. doi:10.1523/JNEUROSCI.2606-10.2010

Izquierdo, E.J., and Beer, R.D.  (2013) <a href="http://www.ploscompbiol.org/article/info%3Adoi%2F10.1371%2Fjournal.pcbi.1002890">Connecting a connectome to behavior: An ensemble of neuroanatomical models of C. elegans klinotaxis</a>. PLoS Computational Biology 9(2): e1002890. doi:10.1371/journal.pcbi.1002890.

## Instructions for use

1. Compile using the Makefile:
```
$ make
```
2. Perform an evolutionary run (takes aprox. 10 minutes):
```
$ ./main
```
3. Visualize the evolutionary progress:
```
$ python viz.py
```

Optional. You can also visualize the rest of the dynamics of the simulation. 
