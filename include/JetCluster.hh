#ifndef JetCluster_HH
#define JetCluster_HH

#include <iostream>
#include <vector>
#include "Math/LorentzVector.h"
#include "Math/PtEtaPhiM4D.h"


using std::vector;

typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double>> LorentzVector;

class JetCluster {
	public:
		JetCluster();
		virtual ~JetCluster();
		vector<LorentzVector> CA(vector<LorentzVector> particles, double R0 = 0.4);
		vector<LorentzVector> CA_GeometricTime(vector<LorentzVector> particles);
		vector<LorentzVector> CA_WeightedTime(vector<LorentzVector> particles);
		void ptSort(vector<LorentzVector>& jets);
		void ptCut(vector<LorentzVector>& jets, double cut = 20.);
		//SpectralCluster();
	private:
		double DeltaR(LorentzVector particle1, LorentzVector particle2);
};
#endif
