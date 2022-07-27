//#include "include/Delphes.hh"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include "classes/DelphesClasses.h"
#include "fastjet/ClusterSequence.hh"
#include "TClonesArray.h"
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <utility>
#include <vector>
#include <iostream>

using namespace fastjet;
using namespace std;
//forward declarations of helper functions
//particle = pair(eta,phi)
double Rsq(pair<double,double> particle1, pair<double,double> particle2){
	return pow((particle1.first-particle2.first),2) + pow((particle1.second-particle2.second),2);
}

//takes a vector of doubles <eta,phi> for particles in an event/jet
//void AachenCluster(vector<pair<double,double>> particles){
vector<TLorentzVector> AachenCluster(vector<TLorentzVector> particles){
	int nParticles = particles.size();
	double R0 = 1.;
	double Rmin = 999.;
	double Rmax = 0.;
	double R;
	//stores pairs of particles based on increasing Rsq
	vector<map<pair<int,int>,double>> idxs;
	pair<int,int> min_idxs;
	TLorentzVector newjet;
//	while(Rmin > R0){
//	loop through all pseudojets to find the smallest R2 (Rmin)
		for(int i = 0; i < nParticles; i++){
			for(int j = 0; j < nParticles; j++){
				if(i >= j) continue;
				R = particles[i].DeltaR(particles[j]);
	//			idxs[make_pair(i,j)] = R2;
				if(R*R < Rmin){ Rmin = R*R; min_idxs = make_pair(j,i);}
				cout << i << " " << j << " " << R*R << " " << Rmin << endl;	
			}
		}
		//cluster the particles with the smallest R2 (Rmin)
		newjet = particles[min_idxs.first] + particles[min_idxs.second];
	 	particles.erase(particles.begin()+min_idxs.first); particles.erase(particles.begin()+min_idxs.second);
		particles.push_back(newjet);
cout << "min r: " << Rmin << endl;
//cout << "new jet eta, phi: " << newjet.Eta() << " " << newjet.Phi() << endl;
		//if pseudojet Rmin is below R0, keep clustering
		if(Rmin < R0) return AachenCluster(particles);
		//otherwise return the pseudoparticles
		else return particles;
//	}
}



int main(){
	TChain* chain = new TChain("Delphes");
	string file = "/Users/margaretlazarovits/delphes/hepmc41.root";
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
	TClonesArray* branchJet = treeReader->UseBranch("Jet");
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
	int nParticles;
	int nJets = 0;
	int nJets_Delphes = 0;
	int nGenJets_Delphes = 0;
	int SKIP = 1;
	int id;
	PseudoJet jetBaby;
	int nTotParticles = 0;
	int particlenum;
	//loop over all objects
	//loop through entries and store GenParticles as pseudojet 4-vectors
	for(int i = 0; i < nEntries; i++){
		treeReader->ReadEntry(i);
		//cout << "event #" << i << endl;
		particles.clear();
		particlenum = 0;
		for(int p = 0; p < branchParticle->GetEntriesFast(); p++){
			//cluster only final state particles
			particle = (GenParticle*)branchParticle->At(p);
			if(particle->Status != 1) continue;
			id = particle->PID;
			//cluster only visible particles
			if(fabs(id) == 12 || fabs(id) == 14 || fabs(id) == 16 || fabs(id) == 18) continue;
			nTotParticles++;
			jetBaby = PseudoJet(particle->Px,particle->Py,particle->Pz,particle->E);
			particles.push_back(jetBaby);
			particlenum++;
		}
		ClusterSequence cs(particles, jet_def);
		jets.clear();
		jets = sorted_by_pt(cs.inclusive_jets(20.0));
		//cout << "delphes reco jet size: " << branchJet->GetEntriesFast() << endl;
		//cout << "delphes gen jet size: " << branchGenJet->GetEntriesFast() << endl;
		//cout << "FJ clustered jet size: " << jets.size() << endl;	
//		newtree->Fill();
		
	}
	cout << "total particles: " << nTotParticles << endl;
//	f->Close();
//	newfile->cd();
//	newtree->Write();
//	newfile->Close();


}

