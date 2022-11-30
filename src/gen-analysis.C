//#include "include/Delphes.hh"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include "classes/DelphesClasses.h"
#include "fastjet/ClusterSequence.hh"
#include "JetCluster.hh"
#include "TClonesArray.h"
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <utility>
#include <vector>
#include <iostream>

using namespace fastjet;
using namespace std;

int main(int argc, char *argv[]){

	if(argc < 2){
		cout << " Usage: ./delphes-analysis.x [root file]" << endl;
		cout << " root file - delphes output" << endl;
		return 1;
	}

	TChain* chain = new TChain("Delphes");
	string file = string(argv[1]);
	chain->Add(file.c_str());
	int idx1 = file.find_last_of("/");
	int idx2 = file.find(".root");
	string ofile = file.substr(idx1+1,idx2-idx1-1)+"_delphesAnalysisOut.root"; 
	
	ExRootTreeReader* treeReader = new ExRootTreeReader(chain);


	//declare variables in new root file
	/*
	TFile* newfile = new TFile(ofile.c_str(),"RECREATE","FastJet clustered jets");
	TTree* newtree = new TTree("FastJet","FastJet jets");
	vector<double> jet_E;
	vector<double> jet_pt;
	vector<double> jet_eta;
	vector<double> jet_phi;
	vector<double> jet_mass;
	vector<double> jet_theta;
	int jet_size;
	//double jet_t; unsure if i can fill this branch?
	newtree->Branch("Jet.E",&jet_E);
	newtree->Branch("Jet.PT",&jet_pt);
	newtree->Branch("Jet.eta",&jet_eta);
	newtree->Branch("Jet.phi",&jet_phi);
	newtree->Branch("Jet.theta",&jet_theta);
	newtree->Branch("Jet.mass",&jet_mass);
	newtree->Branch("Jet_size",&jet_size);
	*/
	//declare read variables
	TClonesArray* branchGenJet = treeReader->UseBranch("GenJet");
	TClonesArray* branchParticle = treeReader->UseBranch("Particle");
	int nEntries = treeReader->GetEntries();	
	Jet* delphesJet;
	GenParticle* particle;
	TLorentzVector momentum;


	//declare jet clustering variables
	vector<PseudoJet> particles, jets;
	PseudoJet jet;
	double R = 0.5;
	JetDefinition jet_def(antikt_algorithm,R);
	int SKIP = 1;
	int id;
	PseudoJet jetBaby
	//loop over all objects
	//loop through entries and store GenParticles as pseudojet 4-vectors
	for(int i = 0; i < nEntries; i++){
		if(i > 0) break;
		treeReader->ReadEntry(i);
		particles.clear();
		for(int p = 0; p < branchParticle->GetEntriesFast(); p++){
			//cluster only final state particles
			particle = (GenParticle*)branchParticle->At(p);
			if(particle->Status != 1) continue;
			id = particle->PID;
			//cluster only visible particles
			if(fabs(id) == 12 || fabs(id) == 14 || fabs(id) == 16) continue;
			jetBaby = PseudoJet(particle->Px,particle->Py,particle->Pz,particle->E);
			particles.push_back(jetBaby);
		}

		ClusterSequence cs(particles, jet_def);
		jets.clear();
		jets = sorted_by_pt(cs.inclusive_jets(20.0));
		
		cout << "Njets gen particles: " << jets.size() << endl;
		for(int j = 0; j < jets.size(); j++) cout << "pt: " << jets[j].pt() << " eta: " << jets[j].eta() << " phi: " << jets[j].phi() << endl;
		cout << "Njets gen delphes: " << branchGenJet->GetEntriesFast() << endl;
		for(int j = 0; j < branchGenJet->GetEntriesFast(); j++){ 
			delphesJet = (Jet*)branchGenJet->At(j);
			cout << "pt: " << delphesJet->PT << " eta: " << delphesJet->Eta << " phi: " << delphesJet->Phi << endl;}
		//		newtree->Fill();
		
	}
//	f->Close();
//	newfile->cd();
//	newtree->Write();
//	newfile->Close();


}

