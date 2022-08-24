#include "JetCluster.hh"
#include <map>
#include <TLorentzVector.h>

using std::cout;
using std::endl;
using std::pair;
using std::map;

JetCluster::JetCluster(){ }

JetCluster::~JetCluster(){ }

vector<TLorentzVector> JetCluster::CA(vector<TLorentzVector> particles, double R0){
	int nParticles = particles.size();
	double Rmin = 999.;
	double Rmax = 0.;
	double R;
	//stores pairs of particles based on increasing Rsq
	vector<map<pair<int,int>,double>> idxs;
	pair<int,int> min_idxs;
	TLorentzVector newjet;
//	loop through all pseudojets to find the smallest R2 (Rmin)
		for(int i = 0; i < nParticles; i++){
			for(int j = 0; j < nParticles; j++){
				if(i >= j) continue;
				R = particles[i].DeltaR(particles[j]);
				if(R*R < Rmin){ Rmin = R*R; min_idxs = std::make_pair(j,i);}
				//cout << i << " " << j << " " << R*R << " " << Rmin << endl;	
			}
		}
		//cluster the particles with the smallest R2 (Rmin)
		newjet = particles[min_idxs.first] + particles[min_idxs.second];
	 	particles.erase(particles.begin()+min_idxs.first); particles.erase(particles.begin()+min_idxs.second);
		particles.push_back(newjet);
cout << "min r: " << Rmin << endl;
//cout << "new jet eta, phi: " << newjet.Eta() << " " << newjet.Phi() << endl;
		//if pseudojet Rmin is below R0, keep clustering
		if(Rmin < R0) return CA(particles);
		//otherwise return the pseudoparticles
		else return particles;
}



vector<TLorentzVector> JetCluster::CA_GeometricTime(vector<TLorentzVector> particles){


}

vector<TLorentzVector> JetCluster::CA_WeightedTime(vector<TLorentzVector> particles){


}
