#ifndef PID_MLASSOCIATOR_H
#define PID_MLASSOCIATOR_H

// **************************************************************************
//  Author: Waleed Esmail e-mail: w.esmail@fz-juelich.de
//   
//  Output Probability Distributions from Ml Algorithms
//
// Created: 17-03-18
//
// **************************************************************************

#include "TClonesArray.h"
#include "FairTask.h"

class PndPidCandidate;
class PndPidProbability;

class PndPidMlAssociatorTask : public FairTask {
  
 protected:
  
  TClonesArray* fPidChargedCand;                                        //! PndPidCandidate TCA for charged particles
  TClonesArray *fPidAlgoMl = new TClonesArray("PndPidProbability");     //! PndPidProbability TCA for charged particles
  TClonesArray* fPidChargedProb;                                        //! PndPidProbability TCA for charged particles

  TTree *inTree;                                                        //! Input Tree for Ml outputs
  Int_t i;                                                             //! Tree Index
  
 public:
  
  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  
  void Register();
  void Reset();
  
  PndPidMlAssociatorTask(const char *name, const char *title="Pnd Task");
  PndPidMlAssociatorTask(TFile *inFile=nullptr);
  virtual ~PndPidMlAssociatorTask();
  
  void SetVerbose(Bool_t verb)            { fVerbose = verb;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();

  ClassDef(PndPidMlAssociatorTask,0)   // PndPidMlTask
  
    };

#endif
