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
#include <TH1D.h>
using namespace fastjet;
using namespace std;

int main(int argc, char *argv[]){

	if(argc < 2){
		cout << " Usage: ./reco-analysis.x [root file]" << endl;
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

	//declare read variables
	TFile* inFile = TFile::Open(file.c_str(),"UPDATE");
	TClonesArray* branchEFCandidate = treeReader->UseBranch("EFlowCandidate");
	TClonesArray* branchEFCandidateTime = treeReader->UseBranch("EFlowCandidateTiming");
	int nEntries = treeReader->GetEntries();	
	GenParticle* EFParticle;
	GenParticle* EFParticleTime;

	JetCluster jc = JetCluster();
	vector<LorentzVector_T> particles;

	TH1D *hist_noTime = new TH1D("EF_noSmear","EF_noSmear",200,0.,1e-7);
	TH1D *hist_wTime = new TH1D("EF_wSmear","EF_wSmear",200,0.,1e-7);
	TH1D *hist_timeDiff = new TH1D("EF_timeDiff","EF_timeDiff",200,-2e-10,2e-10);
	//declare jet clustering variables
	int SKIP = 1;
	int nGam = 0;
	double diff;
	//loop over all objects
	//loop through entries and store GenParticles as pseudojet 4-vectors
	for(int i = 0; i < nEntries; i++){
		treeReader->ReadEntry(i);
		//loop through EFCandidates
		if(branchEFCandidate == nullptr) continue;
		for(int p = 0; p < branchEFCandidate->GetEntriesFast(); p++){
			EFParticle = (GenParticle*)branchEFCandidateTime->At(p);
			LorentzVector_T lv_T = LorentzVector_T(EFParticle->X, EFParticle->Y, EFParticle->Z, EFParticle->T);
			LorentzVector lv = LorentzVector(EFParticle->PT, EFParticle->Eta, EFParticle->Phi, EFParticle->Mass);
			//cout << "time - lv: " << lv.T() << " lv_T: " << lv_T.T() << " EFCandidate: " << EFParticle->T << endl;
			//cout << "Px - lv: " << lv.Px() << " lv_T: " << lv_T.Px() << " EFCandidate: " << EFParticle->Px << endl;
			//cout << "eta - JC: " << jc.Eta(lv_T) << " lv eta: " << lv.Eta() << " EFCandidate: " << EFParticle->Eta << endl;
			//cout << "phi - JC: " << jc.Phi(lv_T) << " lv eta: " << lv.Phi() << " EFCandidate: " << EFParticle->Phi << endl;
			//cout << "\n" << endl;
		}
	}


}

