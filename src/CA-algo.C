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
	TClonesArray* branchEFCandidate = treeReader->UseBranch("EFlowCandidateTiming");
	
	
	
	int nEntries = treeReader->GetEntries();
	GenParticle* particle;
	GenParticle* EFCandidate;
	vector<LorentzVector> jets;
	vector<LorentzVector> gen_babies;
	vector<LorentzVector> reco_babies;
	LorentzVector pseudoJet;
	JetCluster jc;
	int id;
	//loop over events
	for(int i = 0; i < nEntries; i++){
		if(i < 100) continue;
		treeReader->ReadEntry(i);
		gen_babies.clear();
		reco_babies.clear();
		//gen clustering
		/*
		for(int p = 0; p < branchParticle->GetEntriesFast(); p++){
			particle = (GenParticle*)branchParticle->At(p);
			if(particle->Status != 1) continue;
			id = particle->PID;
			if(fabs(id) == 12 || fabs(id) == 14 || fabs(id) == 16 || fabs(id) == 18) continue;
			pseudoJet.SetCoordinates(particle->PT,particle->Eta,particle->Phi,particle->Mass);
			gen_babies.push_back(pseudoJet);
		}
		*/
		//reco clustering
	if(branchEFCandidate == nullptr) continue;
		for(int p = 0; p < branchEFCandidate->GetEntriesFast(); p++){
			EFCandidate = (GenParticle*)branchEFCandidate->At(p);
			id = EFCandidate->PID;
			if(fabs(id) == 12 || fabs(id) == 14 || fabs(id) == 16 || fabs(id) == 18) continue;
			pseudoJet.SetCoordinates(EFCandidate->PT,EFCandidate->Eta,EFCandidate->Phi,EFCandidate->Mass);
		//	pseudoJet.SetCoordinates(particle->X,particle->Y,particle->Z,particle->T);
			if(i < 1 && p < 10) cout << "event #" << i << " cand #" << p << " time: " << EFCandidate->T << " mass: " << EFCandidate->Mass << " pseudojet time:" << pseudoJet.T() << endl;
			reco_babies.push_back(pseudoJet);
		}
		jets.clear();
		jets = jc.CA(reco_babies);
		jc.ptSort(jets);
		jc.ptCut(jets,20.);
		cout << "Njets: " << jets.size() << endl;
	}








}
