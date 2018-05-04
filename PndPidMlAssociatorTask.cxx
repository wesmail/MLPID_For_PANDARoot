#include "PndPidMlAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TF1.h"
#include "Riostream.h"
#include <iostream>


//___________________________________________________________
PndPidMlAssociatorTask::~PndPidMlAssociatorTask() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidMlAssociatorTask::PndPidMlAssociatorTask(TFile *inFile) {
  //---
  if (inFile->IsZombie() || inFile==nullptr){
	  std::cout<<" Error opening Probability file. "<<std::endl;
	  exit(-1);
  }
  else{
	  fPidChargedProb = new TClonesArray("PndPidProbability");
	  inTree = (TTree*)inFile->Get("cbmsim");
      inTree->GetBranch("PidAlgoMl")->SetAutoDelete(kFALSE);
      inTree->SetBranchAddress("PidAlgoMl",&fPidAlgoMl);
      i = 0;
  }
}

//___________________________________________________________
PndPidMlAssociatorTask::PndPidMlAssociatorTask(const char *name, const char *title):FairTask(name) 
{
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
  SetTitle(title);
}

//___________________________________________________________
InitStatus PndPidMlAssociatorTask::Init() {
  
  std::cout << "InitStatus PndPidMlAssociatorTask::Init()" << std::endl;
  

  
  FairRootManager *fManager =FairRootManager::Instance();	

  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-I- PndPidMlAssociatorTask::Init: No PndPidCandidate array PidChargedCand there!" << std::endl;
    return kERROR;
  }
     
  Register();
  
  std::cout << "-I- PndPidMlAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidMlAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidMlAssociatorTask::Exec(Option_t *) {
	  if (fPidChargedProb->GetEntriesFast() != 0) fPidChargedProb->Clear();
  if(fVerbose>1) std::cout << "-I- Start PndPidMlAssociatorTask. "<<std::endl;
  
  inTree->GetEntry(i);
  i++;
      
  // Get the Candidates
  for(Int_t j=0; j<fPidChargedCand->GetEntriesFast(); j++)
    {
		PndPidProbability* procand = (PndPidProbability*)fPidAlgoMl->At(j);
        TClonesArray& pidRef = *fPidChargedProb;
        PndPidProbability* prob = new(pidRef[j]) PndPidProbability();// initializes with zeros
        prob->SetIndex(j);
        prob->SetElectronPdf(procand->GetElectronPdf());
        prob->SetMuonPdf(procand->GetMuonPdf());
        prob->SetPionPdf(procand->GetPionPdf());
        prob->SetKaonPdf(procand->GetKaonPdf());
        prob->SetProtonPdf(procand->GetProtonPdf());
    }
}

//_________________________________________________________________
void PndPidMlAssociatorTask::Register() {
  //---
  FairRootManager::Instance()->
    Register("PidAlgoMl","Pid", fPidChargedProb, kTRUE); 
}

//_________________________________________________________________
void PndPidMlAssociatorTask::Finish() {
}

//_________________________________________________________________
void PndPidMlAssociatorTask::Reset() {
  //---
}


ClassImp(PndPidMlAssociatorTask)
