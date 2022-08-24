#include "DelphesReader.hh"

using std::cout;
using std::endl;

DelphesReader::DelphesReader(){ 
	m_chain = new TChain("Delphes");
}


DelphesReader::DelphesReader(string file){
	m_chain = new TChain("Delphes");
	m_chain->Add(file.c_str());
	int idx1 = file.find_last_of("/");
	int idx2 = file.find(".root");
	m_outFileName = file.substr(idx1+1,idx2-idx1-1)+"_CAalgoOut.root";
}


DelphesReader::~DelphesReader(){ 
	delete m_chain;
	m_outFileName.erase();
}


TChain* DelphesReader::GetChain(){
	if(m_chain != nullptr)
		return m_chain;
	else{
		cout << "chain empty" << endl;
		return nullptr;
	}
}


string DelphesReader::GetOutFileName(){
	if(!m_outFileName.empty())
		return m_outFileName;
	else
		return " ";
}


void DelphesReader::AddFile(string file){
	if(m_chain != nullptr)
		m_chain->Add(file.c_str());
	else{
		m_chain = new TChain("Delphes");
		m_chain->Add(file.c_str());
	}
}



