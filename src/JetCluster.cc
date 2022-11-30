#include "JetCluster.hh"
#include <map>
#include "Math/LorentzVector.h"
#include "Math/PtEtaPhiM4D.h"


using std::cout;
using std::endl;
using std::pair;
using std::map;


JetCluster::JetCluster(){ }

JetCluster::~JetCluster(){ }

vector<LorentzVector> JetCluster::CA(vector<LorentzVector> particles, double R0){
	if(particles.size() < 1) return particles;
	int nParticles = particles.size();
	double Rmin = 999.;
	double Rmax = 0.;
	double R;
	vector<map<pair<int,int>,double>> idxs;
	pair<int,int> min_idxs;
	LorentzVector newjet;
//	loop through all pseudojets to find the smallest R2 (Rmin)
		for(int i = 0; i < nParticles; i++){
			for(int j = 0; j < nParticles; j++){
				if(i >= j) continue;
				R = DeltaR(particles[i],particles[j]);
				if(R*R < Rmin){ Rmin = R*R; min_idxs = std::make_pair(j,i);}
			}
		}
	//cluster the particles with the smallest R2 (Rmin)
		newjet = particles[min_idxs.first] + particles[min_idxs.second];
	 	particles.erase(particles.begin()+min_idxs.first); particles.erase(particles.begin()+min_idxs.second);
		particles.push_back(newjet);
		//if pseudojet Rmin is below R0, keep clustering
		if(Rmin < R0) return CA(particles,R0);
		//otherwise return the pseudoparticles
		else return particles;
}



vector<LorentzVector> JetCluster::CA_GeometricTime(vector<LorentzVector_T> particles, double R0){
	if(particles.size() < 1) return particles;
	int nParticles = particles.size();
	double Rmin = 999.;
	double Rmax = 0.;
	double R;
	vector<map<pair<int,int>,double>> idxs;
	pair<int,int> min_idxs;
	LorentzVector newjet;
//	loop through all pseudojets to find the smallest R2 (Rmin)
		for(int i = 0; i < nParticles; i++){
			for(int j = 0; j < nParticles; j++){
				if(i >= j) continue;
				R = DeltaR_T(particles[i],particles[j]);
				if(R*R < Rmin){ Rmin = R*R; min_idxs = std::make_pair(j,i);}
			}
		}
	//cluster the particles with the smallest R2 (Rmin)
		newjet = particles[min_idxs.first] + particles[min_idxs.second];
	 	particles.erase(particles.begin()+min_idxs.first); particles.erase(particles.begin()+min_idxs.second);
		particles.push_back(newjet);
		//if pseudojet Rmin is below R0, keep clustering
		if(Rmin < R0) return CA(particles,R0);
		//otherwise return the pseudoparticles
		else return particles;


}

vector<LorentzVector> JetCluster::CA_WeightedTime(vector<LorentzVector> particles){


}

double JetCluster::DeltaR(LorentzVector particle1, LorentzVector particle2){
	double phi1 = particle1.phi();
	double phi2 = particle2.phi();
	double eta1 = particle1.eta();
	double eta2 = particle2.eta();

	return pow(((phi1-phi2)*(phi1-phi2) + (eta1-eta2)*(eta1-eta2)),0.5);
}

double JetCluster::DeltaR_T(LorentzVector_T particle1, LorentzVector_T particle2){
	double phi1 = Phi(particle1);
	double phi2 = Phi(particle2);
	double eta1 = Eta(particle1);
	double eta2 = Eta(particle2);
	double t1 = particle1.t();
	double t2 = particle2.t();

	return pow(((phi1-phi2)*(phi1-phi2) + (eta1-eta2)*(eta1-eta2) + c_light*(t2-t1)*(t2-t1)),0.5);
}



double JetCluster::Eta(LorentzVector_T particle){
	double ret = -999;
	double x = particle.X();
	double y = particle.Y();
	double z = particle.Z();
	
	double r = pow((x*x + y*y + z*z),0.5);

	double theta = acos(z/r);

	ret = -log(tan(theta/2.));

	return ret;


}
double JetCluster::Phi(LorentzVector_T particle){
	double ret = -999.;
	double x = particle.X();
	double y = particle.Y();
	double pi = acos(-1);

	if(x > 0){
		ret = atan(y/x);
	}
	else if(x < 0){
		if(y >= 0){
			ret = atan(y/x) + pi;
		}
		//y < 0
		else{
			ret = atan(y/x) - pi;
		}
	}
	//x = 0
	else{
		if(y > 0){
			ret = pi;
		}
		else if(y < 0){
			ret = -pi;
		}
		//x == 0 && y == 0
		else{
			cout << "Phi undefined for x: " << x << " and y: " << y << endl;
		}
	}
	return ret;
}

void JetCluster::ptSort(vector<LorentzVector>& jets){
	for(int j = 0; j < jets.size(); j++){
		LorentzVector jet = jets[j];
		int k = j-1;
		while(k >= 0 && jets[k].pt() < jet.pt()){
			jets[k+1] = jets[k];
			k -= 1;
		}
		jets[k+1] = jet;
	}
}

void JetCluster::ptCut(vector<LorentzVector>& jets, double cut){
	ptSort(jets);
	for(int j = 0; j < jets.size(); j++){
		if(jets[j].pt() < cut){
			jets.erase(jets.begin()+j,jets.end());
			break;
		}
	}
}


