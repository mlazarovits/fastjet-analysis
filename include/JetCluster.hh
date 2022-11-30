#ifndef JetCluster_HH
#define JetCluster_HH

#include <iostream>
#include <vector>
#include "Math/LorentzVector.h"
#include "Math/PtEtaPhiM4D.h"
#include "Math/PxPyPzE4D.h"
#include "Math/Vector4D.h"


using std::vector;

typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double>> LorentzVector;
typedef ROOT::Math::LorentzVector<ROOT::Math::XYZTVector> LorentzVector_T;

class JetCluster {
	public:
		JetCluster();
		virtual ~JetCluster();
		vector<LorentzVector> CA(vector<LorentzVector> particles, double R0 = 0.4);
		vector<LorentzVector> CA_GeometricTime(vector<LorentzVector_T> particles, double R0);
		vector<LorentzVector> CA_WeightedTime(vector<LorentzVector> particles);
		void ptSort(vector<LorentzVector>& jets);
		void ptCut(vector<LorentzVector>& jets, double cut = 20.);
		
		double Phi(LorentzVector_T particle);
		double Eta(LorentzVector_T particle);
		//SpectralCluster();
	private:
		double DeltaR(LorentzVector particle1, LorentzVector particle2);
		double DeltaR_T(LorentzVector_T particle1, LorentzVector_T particle2);
  		const double c_light = 2.99792458E8;
};
#endif
