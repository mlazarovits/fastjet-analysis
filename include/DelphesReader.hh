#ifndef DelphesReader_HH
#define DelphesReader_HH

#include <iostream>
#include <TChain.h>

using std::string;
class DelphesReader{
	public:
		DelphesReader();
		DelphesReader(string file);
		virtual ~DelphesReader();

		TChain* GetChain();
		string GetOutFileName();
		void AddFile(string file);

	private:
		TChain* m_chain;
		string m_outFileName;
};
#endif
