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



int main(int argc, char *argv[]){
	//using Delphes module with HEPMC input
	FILE *inputFile = 0;//fopen("/Users/margaretlazarovits/jet-timing-analysis/hepmcout41.dat","r");
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
	//Delphes CMS default definition
	double R = 0.5;
	int i;
	stringstream message;
	JetDefinition jet_def(antikt_algorithm, R);

	TFile* oFile = new TFile("hepmc41_FJclustered.root","RECREATE","FastJet clustered jets");
	TTree* newtree = new TTree("FastJet","FastJet jets");
	vector<double> jet_E;
	vector<double> jet_pt;
	vector<double> jet_eta;
	vector<double> jet_phi;
	vector<double> jet_mass;
	vector<int> jet_nConstituents;
	int jet_size;

	vector<PseudoJet> babies;
	vector<vector<float>> baby_pt;
	vector<vector<int>> baby_id;

	newtree->Branch("Jet.E",&jet_E);
	newtree->Branch("Jet.PT",&jet_pt);
	newtree->Branch("Jet.eta",&jet_eta);
	newtree->Branch("Jet.phi",&jet_phi);
	newtree->Branch("Jet.mass",&jet_mass);
	newtree->Branch("Jet.Babies_size",&jet_nConstituents);
	newtree->Branch("JetBaby.pt",&baby_pt);
	newtree->Branch("JetBaby.id",&baby_id);
	newtree->Branch("Jet_size",&jet_size);

	vector<PseudoJet> particles, jets;
	PseudoJet particle;
	
	int nTotalJets = 0;
	int id = -999;
	int nBabies = -999;
	int nTotalBabies = 0;
	int nTotParticles = 0;
	if(argc < 2)
	  {
	    cout << " Usage: ./fastjet-cluster.x" << " config_file" << " [input_file(s)]" << endl;
	    cout << " config_file - configuration file in Tcl format," << endl;
	    cout << " input_file(s) - input file(s) in HepMC format," << endl;
	    cout << " with no input_file, or when input_file is -, read standard input." << endl;
	    return 1;
	  }
		
	signal(SIGINT,SignalHandler);	
	cout << "Clustered with: " << jet_def.description() << endl;
	//set .tcl card
	confReader->ReadFile(argv[1]);
	maxEvents = confReader->GetInt("::MaxEvents",0);
	skipEvents = confReader->GetInt("::SkipEvents",0);

	if(maxEvents < 0)
	    {
	      throw runtime_error("MaxEvents must be zero or positive");
	    }
	
	    if(skipEvents < 0)
	    {
	      throw runtime_error("SkipEvents must be zero or positive");
	    }

	d->SetConfReader(confReader);
	factory = d->GetFactory();
	//only include stable, final state particles
	allParticleOutputArray = d->ExportArray("allParticles");
	stableParticleOutputArray = d->ExportArray("stableParticles");
	partonOutputArray = d->ExportArray("partons");

	d->InitTask();

	inputArray = d->ImportArray("EFlowMerger/eflow");
	//inputArray = d->ImportArray("Calorimeter/towers");
	inputIterator = inputArray->MakeIterator();

	//read HepMC file
	i = 2;
	do
	{
	if(interrupted) break;

      if(i == argc || strncmp(argv[i], "-", 2) == 0)
      {
        cout << "** Reading standard input" << endl;
        inputFile = stdin;
        length = -1;
      }
      else
      {
        cout << "** Reading " << argv[i] << endl;
        inputFile = fopen(argv[i], "r");

        if(inputFile == NULL)
        {
          message << "can't open " << argv[i];
          throw runtime_error(message.str());
        }

        fseek(inputFile, 0L, SEEK_END);
        length = ftello(inputFile);
        fseek(inputFile, 0L, SEEK_SET);

        if(length <= 0)
        {
          fclose(inputFile);
          ++i;
          continue;
        }
      }



	reader->SetInputFile(inputFile);

	//declare variables in new root inputFile
cout << "maxEvents: " << maxEvents << " skipEvents: " << skipEvents << endl;	
	//loop over all objects
	d->Clear();
	reader->Clear();
	eventCounter = 0;
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
				int particlenum = 0;
				//pass delphes candidates to fastjet for clustering
				while((candidate = static_cast<Candidate*>(inputIterator->Next()))){
					momentum = candidate->Momentum;
					id = candidate->PID;
					//if(fabs(id) == 12 || fabs(id) == 14 || fabs(id) == 16 || fabs(id) == 18) continue;
			if(eventCounter == 1){ cout << "particle #" << particlenum << ": " << momentum.Pt() << ", " << momentum.Eta() << ", " << momentum.Phi() << ", " << momentum.E()  << endl;}
					nTotParticles++;
					particle = PseudoJet(momentum.Px(),momentum.Py(),momentum.Pz(),momentum.E());
					particle.set_user_index(id);
					particles.push_back(particle);
					particlenum++;
				}
				//run clusering
				ClusterSequence cs(particles,jet_def);
				jets.clear();
				jets = sorted_by_pt(cs.inclusive_jets(20.0));
				
				//cleaning cuts
				//min pT = 20 (delphes default)
			//	Selector ptSelector = SelectorPtMin(20.);
			//	jets = ptSelector(jets);
				//eta cut
				//Selector etaSelector = SelectorAbsEtaMax(5.0);
				
				//fill new ttree with clustered jets
				jet_size = jets.size();
				for(int j = 0; j < jet_size; j++){
					++nTotalJets;
					nBabies = jets[j].constituents().size();
					jet_nConstituents.push_back(nBabies);
					babies = jets[j].constituents();
					vector<float> tmp_baby_pt;
					vector<int> tmp_baby_id;
					nTotalBabies = nTotalBabies + nBabies;

					//loop throught jet constituents (babies)
					for(int c = 0; c < nBabies; c++){
						tmp_baby_pt.push_back(babies[c].pt());
						tmp_baby_id.push_back(babies[c].user_index());
					}
	//				cout << "jet # " << nTotalJets << " PT: " << jets[j].pt() << endl;
					jet_E.push_back(jets[j].E());
					jet_pt.push_back(jets[j].pt());
					jet_eta.push_back(jets[j].eta());
					jet_phi.push_back(jets[j].phi());
					jet_mass.push_back(jets[j].m());
					baby_pt.push_back(tmp_baby_pt);
					baby_id.push_back(tmp_baby_id);

					babies.clear();
				}
				newtree->Fill();		
				jet_nConstituents.clear();
				jet_E.clear();
				jet_pt.clear();
				jet_eta.clear();
				jet_phi.clear();
				jet_mass.clear();
				baby_pt.clear();
				baby_id.clear();
			}
			d->Clear();
			reader->Clear();
		}
	}
	if(inputFile != stdin) fclose(inputFile);

      ++i;
	}
	while(i < argc);
		cout << "total jets: " << nTotalJets << endl;
		cout << "total babies: " << nTotalBabies << endl;
		cout << "total particles: " << nTotParticles << endl;
	d->FinishTask();

	oFile->cd();
	oFile->Write();

	delete reader;
	delete d;
	delete confReader;


}

