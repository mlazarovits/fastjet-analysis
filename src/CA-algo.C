#include "JetCluster.hh"
#include "DelphesReader.hh"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include "classes/DelphesClasses.h"
#include <TClonesArray.h>
#include "Math/LorentzVector.h"



using namespace std;


int main(int argc, char *argv[]){
		if(argc < 2){
		cout << " Usage: ./CA-algo.x [root file]" << endl;
		cout << " root file - delphes output" << endl;
		return 1;
		}


	DelphesReader dr = DelphesReader(string(argv[1]));
	TChain* chain = dr.GetChain();
	string ofile = dr.GetOutFileName();
cout << ofile << endl;

	ExRootTreeReader* treeReader = new ExRootTreeReader(chain);
	TClonesArray* branchParticle = treeReader->UseBranch("Particle");
	TClonesArray* branchJet = treeReader->UseBranch("Jet");
	int nEntries = treeReader->GetEntries();
	GenParticle* particle;
	vector<LorentzVector> jets;
	vector<LorentzVector> particles;
	LorentzVector pseudoJet;
	JetCluster jc;
	int id;
	//loop over events
	for(int i = 0; i < nEntries; i++){
		if(i > 0) break;
		treeReader->ReadEntry(i);
		particles.clear();
		for(int p = 0; p < branchParticle->GetEntriesFast(); p++){
			particle = (GenParticle*)branchParticle->At(p);
			if(particle->Status != 1) continue;
			id = particle->PID;
			if(fabs(id) == 12 || fabs(id) == 14 || fabs(id) == 16 || fabs(id) == 18) continue;
			pseudoJet.SetCoordinates(particle->PT,particle->Eta,particle->Phi,particle->Mass);
			particles.push_back(pseudoJet);
		}
		jets.clear();
		jets = jc.CA(particles);
		jc.ptSort(jets);
		jc.ptCut(jets,20.);
	}








}
