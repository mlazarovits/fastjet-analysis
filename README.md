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

