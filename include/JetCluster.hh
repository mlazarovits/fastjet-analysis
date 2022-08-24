#ifndef JetCluster_HH
#define JetCluster_HH

#include <iostream>
#include <vector>
#include <TLorentzVector.h>

using std::vector;
class JetCluster {
	public:
		JetCluster();
		virtual ~JetCluster();
		vector<TLorentzVector> CA(vector<TLorentzVector> particles, double R0 = 0.4);
		vector<TLorentzVector> CA_GeometricTime(vector<TLorentzVector> particles);
		vector<TLorentzVector> CA_WeightedTime(vector<TLorentzVector> particles);
		//SpectralCluster();
};
#endif
