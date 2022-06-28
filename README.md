# jet-timing-analysis
Repository for jet clustering with timing and analysis.

## Dependencies
- ROOT
- [FastJet](http://fastjet.fr/quickstart.html)
- [Delphes](https://github.com/delphes/delphes)

## To build
Make sure all dependencies are installed. Update Makefile to change ```DELPHES_DIR``` to the path to your own Delphes installation.

Then
```
make
```
to make all executables. 
To make just ```fastjet-cluster.x```,
```
make fastjet-cluster
```
To make just ```delphes-analysis.x```,
```
make delphes-analysis
```

## Executables
```fastjet-cluster.x``` takes as input (in the script) a hepMC file. Then, it runs a detector simulation on the gen particles from the hepMC file with a modular version of Delphes. After the reconstruction, the external FastJet library clusters the Delphes candidates. Be sure to update the card input (confReader->ReadFile(...)) to a Delphes card in your build.

```delphes-analysis.x``` analyses a ROOT file from Delphes. This script stores the jet pT, eta, phi, and mass and the number of jets in each event in a new root file.

