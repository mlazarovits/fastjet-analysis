#include "modules/Delphes.h"
#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "classes/DelphesHepMC3Reader.h"
#include "classes/DelphesModule.h"

#include "ExRootAnalysis/ExRootTreeWriter.h"
#include "ExRootAnalysis/ExRootTreeBranch.h"
#include "ExRootAnalysis/ExRootProgressBar.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"

#include "TROOT.h"
#include "TApplication.h"
#include "TObjArray.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TFile.h"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <signal.h>
#include <utility>
#include <vector>
#include <stdio.h>
using namespace fastjet;
using namespace std;

static bool interrupted = false;


void SignalHandler(int sig){
	interrupted = true;
}

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
	//using Delphes module with HEPMC input
	FILE *inputFile = fopen("/Users/margaretlazarovits/fastjet-analysis/hepmcout41.dat","r");
	Delphes *d = 0;
        d = new Delphes("Delphes");
	DelphesFactory* factory;
	ExRootConfReader *confReader = new ExRootConfReader;
	DelphesHepMC3Reader *reader = new DelphesHepMC3Reader;
	TObjArray *inputArray, *stableParticleOutputArray,*allParticleOutputArray, *partonOutputArray;
        TIterator *inputIterator;
	Candidate *candidate;
	TLorentzVector momentum;	
	//int nEntries = 1;
	int skipEvents = 1;
	int maxEvents = 10;
	Long64_t length, eventCounter;
	//choose jet definition -- TODO: double check this with Delphes FastJet default
	double R = 0.5;
	JetDefinition jet_def(anti_kt, R);
	cout << "Clustered with: " << jet_def.description() << endl;

	signal(SIGINT,SignalHandler);	

	//set .tcl card
	confReader->ReadFile("/Users/margaretlazarovits/delphes/cards/delphes_card_CMS_NoFastJet.tcl");

	maxEvents = confReader->GetInt("::MaxEvents",0);
	skipEvents = confReader->GetInt("::SkipEvents",0);

	d->SetConfReader(confReader);
	factory = d->GetFactory();
	allParticleOutputArray = d->ExportArray("allParticles");
	stableParticleOutputArray = d->ExportArray("stableParticles");
	partonOutputArray = d->ExportArray("partons");

	d->InitTask();

	inputArray = d->ImportArray("EFlowMerger/eflow");
	inputIterator = inputArray->MakeIterator();

	//read HepMC file
	fseek(inputFile, 0L, SEEK_END);
	length = ftello(inputFile);
	fseek(inputFile, 0L, SEEK_SET);
	if(length <= 0)
		fclose(inputFile);



	reader->SetInputFile(inputFile);

	//declare variables in new root inputFile
	
	TFile* oFile = new TFile("hepmc41_FJclustered.root","RECREATE","FastJet clustered jets");
	TTree* newtree = new TTree("FastJet","FastJet jets");
	vector<double> jet_E;
	vector<double> jet_pt;
	vector<double> jet_eta;
	vector<double> jet_phi;
	vector<double> jet_mass;
	vector<double> jet_theta;
	int jet_size;
	newtree->Branch("Jet.E",&jet_E);
	newtree->Branch("Jet.PT",&jet_pt);
	newtree->Branch("Jet.eta",&jet_eta);
	newtree->Branch("Jet.phi",&jet_phi);
	newtree->Branch("Jet.theta",&jet_theta);
	newtree->Branch("Jet.mass",&jet_mass);
	newtree->Branch("Jet_size",&jet_size);
	
	vector<PseudoJet> particles, jets;
	PseudoJet jet;
	int nParticles;
	vector<pair<double,double>> etaPhi;
	vector<TLorentzVector> fourvecs;

	//loop over all objects
	d->Clear();
	reader->Clear();
	eventCounter = 0;
	//maxEvents = 1;
	while((maxEvents <= 0 || eventCounter - skipEvents < maxEvents) &&
			reader->ReadBlock(factory,allParticleOutputArray,
				stableParticleOutputArray,partonOutputArray) && !interrupted){
		//loop over events
		if(reader->EventReady()){
			++eventCounter;
			if(eventCounter > skipEvents){
				//run delphes reco
				d->ProcessTask();
				particles.clear();
				inputIterator->Reset();
				//pass delphes candidates to fastjet for clustering
				while((candidate = static_cast<Candidate*>(inputIterator->Next()))){
					momentum = candidate->Momentum;
					jet = PseudoJet(momentum.Px(),momentum.Py(),momentum.Pz(),momentum.E());
					particles.push_back(jet);
				}
				//run clusering
				ClusterSequence cs(particles,jet_def);
				jets.clear();
				jets = sorted_by_pt(cs.inclusive_jets(0.0));
				//fill new ttree with clustered jets
				jet_size = jets.size();
cout << "event #: " << eventCounter << " # jets: " << jet_size << " # particles: " << particles.size() << endl;
				for(int j = 0; j < jet_size; j++){
					jet_E.push_back(jets[j].E());
					//vector<double> jet_pt;
					//vector<double> jet_eta;
					//vector<double> jet_phi;
					//vector<double> jet_mass;
					//vector<double> jet_theta;

				}
				newtree->Fill();		
			}
			d->Clear();
			reader->Clear();
		}
	}
	fclose(inputFile);
	d->FinishTask();

	oFile->cd();
	oFile->Write();

	delete reader;
	delete d;
	delete confReader;


}

