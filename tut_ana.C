class RhoCandList;
class RhoCandidate;
class PndAnaPidSelector;
class PndAnaPidCombiner;
class PndAnalysis;

// *** routine to only keep PID matched candidates in list
int SelectTruePid(PndAnalysis *ana, RhoCandList &l)
{
	int removed = 0;
	
	for (int ii=l.GetLength()-1;ii>=0;--ii)
	{
		if ( !(ana->McTruthMatch(l[ii])) )
		{
			l.Remove(l[ii]);
			removed++;
		}
	}
	
	return removed;
}


void tut_ana_ml(int nevts = 0, TString prefix = "signal")
{
 	// *** some variables
	int i=0,j=0, k=0, l=0;
	gStyle->SetOptFit(1011);
	
	// *** the output file for FairRunAna
	TString OutFile="out_dummy.root";  
					
	// *** the files coming from the simulation
	TString inPidFile  = prefix+"_pid.root";    // this file contains the PndPidCandidates and McTruth
	TString inParFile  = prefix+"_par.root";
	
	// *** PID table with selection thresholds; can be modified by the user
	TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";	
	
	// *** initialization
	//FairLogger::GetLogger()->SetLogToFile(kFALSE);
	FairRunAna* fRun = new FairRunAna();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	fRun->SetInputFile(inPidFile);
	fRun->AddFriend("signal_ml.root");
	
	// *** setup parameter database 	
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	FairParAsciiFileIo* parIOPid = new FairParAsciiFileIo();
	parIOPid->open(pidParFile.Data(),"in");
	
	rtdb->setFirstInput(parIO);
	rtdb->setSecondInput(parIOPid);
	rtdb->setOutput(parIO);  
	
	fRun->SetOutputFile(OutFile);
	fRun->Init(); 
	
	// *** create an output file for all histograms
	TFile *out = TFile::Open(prefix+"_ml_ana.root","RECREATE");
	
	// *** create some histograms
	TH1F *hjpsim_all = new TH1F("hjpsim_all","J/#psi mass (all)",200,0,4.5);
	TH1F *hpsim_all  = new TH1F("hpsim_all","#psi(2S) mass (all)",200,0,5);
	
	TH1F *hjpsim_lpid = new TH1F("hjpsim_lpid","J/#psi mass (loose pid)",200,0,4.5);
	TH1F *hpsim_lpid  = new TH1F("hpsim_lpid","#psi(2S) mass (loose pid)",200,0,5);
	
	TH1F *hjpsim_tpid = new TH1F("hjpsim_tpid","J/#psi mass (tight pid)",200,0,4.5);
	TH1F *hpsim_tpid  = new TH1F("hpsim_tpid","#psi(2S) mass (tight pid)",200,0,5);
	
	TH1F *hjpsim_trpid = new TH1F("hjpsim_trpid","J/#psi mass (true pid)",200,0,4.5);
	TH1F *hpsim_trpid  = new TH1F("hpsim_trpid","#psi(2S) mass (true pid)",200,0,5);
	
	
	TH1F *hjpsim_ftm = new TH1F("hjpsim_ftm","J/#psi mass (full truth match)",200,0,4.5);
	TH1F *hpsim_ftm  = new TH1F("hpsim_ftm","#psi(2S) mass (full truth match)",200,0,5);
	
	TH1F *hjpsim_nm = new TH1F("hjpsim_nm","J/#psi mass (no truth match)",200,0,4.5);
	TH1F *hpsim_nm  = new TH1F("hpsim_nm","#psi(2S) mass (no truth match)",200,0,5);
	
	TH1F *hjpsim_diff = new TH1F("hjpsim_diff","J/#psi mass diff to truth",100,-2,2);
	TH1F *hpsim_diff  = new TH1F("hpsim_diff","#psi(2S) mass diff to truth",100,-2,2);
	
	
	TH1F *hjpsim_vf   = new TH1F("hjpsim_vf","J/#psi mass (vertex fit)",200,0,4.5);
	TH1F *hjpsim_4cf  = new TH1F("hjpsim_4cf","J/#psi mass (4C fit)",200,0,4.5);
	TH1F *hjpsim_mcf  = new TH1F("hjpsim_mcf","J/#psi mass (mass constraint fit)",200,0,4.5);
	
	TH1F *hjpsi_chi2_vf  = new TH1F("hjpsi_chi2_vf", "J/#psi: #chi^{2} vertex fit",100,0,10);
	TH1F *hpsi_chi2_4c   = new TH1F("hpsi_chi2_4c",  "#psi(2S): #chi^{2} 4C fit",100,0,250);
	TH1F *hjpsi_chi2_mf  = new TH1F("hjpsi_chi2_mf", "J/#psi: #chi^{2} mass fit",100,0,10);
	
	TH1F *hjpsi_prob_vf  = new TH1F("hjpsi_prob_vf", "J/#psi: Prob vertex fit",100,0,1);
	TH1F *hpsi_prob_4c   = new TH1F("hpsi_prob_4c",  "#psi(2S): Prob 4C fit",100,0,1);
	TH1F *hjpsi_prob_mf  = new TH1F("hjpsi_prob_mf", "J/#psi: Prob mass fit",100,0,1);
	
	TH2F *hvpos = new TH2F("hvpos","(x,y) projection of fitted decay vertex",100,-2,2,100,-2,2);
	
	//
	// Now the analysis stuff comes...
	//
	
	
	// *** the data reader object
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();
	
	// *** RhoCandLists for the analysis
	RhoCandList muplus, muminus, piplus, piminus, jpsi, psi2s;
	
	// *** Mass selector for the jpsi cands
	double m0_jpsi = TDatabasePDG::Instance()->GetParticle("J/psi")->Mass();   // Get nominal PDG mass of the J/psi
	RhoMassParticleSelector *jpsiMassSel=new RhoMassParticleSelector("jpsi",m0_jpsi,1.0);
	
	// *** the lorentz vector of the initial psi(2S)
	TLorentzVector ini(0, 0, 6.231552, 7.240065);
	
	// ***
	// the event loop
	// ***
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;
				
		// *** Select with no PID info ('All'); type and mass are set  (modified)
		theAnalysis->FillList(muplus,  "MuonAllPlus");
		theAnalysis->FillList(muminus, "MuonAllMinus");
		theAnalysis->FillList(piplus,  "PionAllPlus");
		theAnalysis->FillList(piminus, "PionAllMinus");
		
		// *** combinatorics for J/psi -> mu+ mu-
		jpsi.Combine(muplus, muminus);
		
		
		// ***
		// *** do the TRUTH MATCH for jpsi
		// ***
		jpsi.SetType(443);
				
		for (j=0;j<jpsi.GetLength();++j) 
		{
			hjpsim_all->Fill( jpsi[j]->M() );
			
			if (theAnalysis->McTruthMatch(jpsi[j]))
			{ 
				hjpsim_ftm->Fill( jpsi[j]->M() );
			 	hjpsim_diff->Fill( jpsi[j]->GetMcTruth()->M() - jpsi[j]->M() );
			}
			else 
				hjpsim_nm->Fill( jpsi[j]->M() );
		}
		
		// ***
		// *** do VERTEX FIT (J/psi)
		// ***
		for (j=0;j<jpsi.GetLength();++j) 
		{
			PndKinVtxFitter vtxfitter(jpsi[j]);	// instantiate a vertex fitter
			vtxfitter.Fit();
			
			double chi2_vtx = vtxfitter.GetChi2();	// access chi2 of fit
			double prob_vtx = vtxfitter.GetProb();	// access probability of fit
			hjpsi_chi2_vf->Fill(chi2_vtx);
			hjpsi_prob_vf->Fill(prob_vtx);			
			
			if ( prob_vtx > 0.01 )				// when good enough, fill some histos
			{
				RhoCandidate *jfit = jpsi[j]->GetFit();	// access the fitted cand
				TVector3 jVtx=jfit->Pos();		// and the decay vertex position
				
				hjpsim_vf->Fill(jfit->M());            
				hvpos->Fill(jVtx.X(),jVtx.Y());
			}
		}
		
		// *** some rough mass selection
		jpsi.Select(jpsiMassSel);
		
		// *** combinatorics for psi(2S) -> J/psi pi+ pi-
		psi2s.Combine(jpsi, piplus, piminus);
		
		
		// ***
		// *** do the TRUTH MATCH for psi(2S)
		// ***
		psi2s.SetType(88888);

		for (j=0;j<psi2s.GetLength();++j) 
		{
			hpsim_all->Fill( psi2s[j]->M() );
			
			if (theAnalysis->McTruthMatch(psi2s[j])) 
			{
			 	hpsim_ftm->Fill( psi2s[j]->M() );
			 	hpsim_diff->Fill( psi2s[j]->GetMcTruth()->M() - psi2s[j]->M() );
			}
			else 
				hpsim_nm->Fill( psi2s[j]->M() );
		}			

		
		// ***
		// *** do 4C FIT (initial psi(2S) system)
		// ***
		for (j=0;j<psi2s.GetLength();++j) 
		{
			PndKinFitter fitter(psi2s[j]);	// instantiate the kin fitter in psi(2S)
			fitter.Add4MomConstraint(ini);	// set 4 constraint
			fitter.Fit();		            // do fit
			
			double chi2_4c = fitter.GetChi2();	// get chi2 of fit
			double prob_4c = fitter.GetProb();	// access probability of fit
			hpsi_chi2_4c->Fill(chi2_4c);
			hpsi_prob_4c->Fill(prob_4c);			
			
			if ( prob_4c > 0.01 )			// when good enough, fill some histo
			{
				RhoCandidate *jfit = psi2s[j]->Daughter(0)->GetFit();	// get fitted J/psi
				
				hjpsim_4cf->Fill(jfit->M());
			}
		}		
		
		
		// ***
		// *** do MASS CONSTRAINT FIT (J/psi)
		// ***
		for (j=0;j<jpsi.GetLength();++j) 
		{
			PndKinFitter mfitter(jpsi[j]);		// instantiate the PndKinFitter in psi(2S)
			mfitter.AddMassConstraint(m0_jpsi);	// add the mass constraint
			mfitter.Fit();						// do fit
			
			double chi2_m = mfitter.GetChi2();	// get chi2 of fit
			double prob_m = mfitter.GetProb();	// access probability of fit
			hjpsi_chi2_mf->Fill(chi2_m);
			hjpsi_prob_mf->Fill(prob_m);			
			
			if ( prob_m > 0.01 )				// when good enough, fill some histo
			{
				RhoCandidate *jfit = jpsi[j]->GetFit();	// access the fitted cand
				hjpsim_mcf->Fill(jfit->M());
			}
		}		
		
		
		// ***
		// *** TRUE PID combinatorics
		// ***
		
		// *** do MC truth match for PID type
		SelectTruePid(theAnalysis, muplus);
		SelectTruePid(theAnalysis, muminus);
		SelectTruePid(theAnalysis, piplus);
		SelectTruePid(theAnalysis, piminus);
				
		// *** all combinatorics again with true PID
		jpsi.Combine(muplus, muminus);
		for (j=0;j<jpsi.GetLength();++j) hjpsim_trpid->Fill( jpsi[j]->M() );
		jpsi.Select(jpsiMassSel);
		
		psi2s.Combine(jpsi, piplus, piminus);
		for (j=0;j<psi2s.GetLength();++j) hpsim_trpid->Fill( psi2s[j]->M() );
		
		
		// ***
		// *** LOOSE PID combinatorics
		// ***
		
		// *** and again with PidAlgoMl and loose selection
		theAnalysis->FillList(muplus,  "MuonLoosePlus",  "PidAlgoMl");
		theAnalysis->FillList(muminus, "MuonLooseMinus", "PidAlgoMl");
		theAnalysis->FillList(piplus,  "PionLoosePlus",  "PidAlgoMl");
		theAnalysis->FillList(piminus, "PionLooseMinus", "PidAlgoMl");
		
		jpsi.Combine(muplus, muminus);
		for (j=0;j<jpsi.GetLength();++j) hjpsim_lpid->Fill( jpsi[j]->M() );
		jpsi.Select(jpsiMassSel);
		
		psi2s.Combine(jpsi, piplus, piminus);
		for (j=0;j<psi2s.GetLength();++j) hpsim_lpid->Fill( psi2s[j]->M() );
		
		
		// ***
		// *** TIGHT PID combinatorics
		// ***
		
		// *** and again with PidAlgoMl and tight selection
		theAnalysis->FillList(muplus,  "MuonTightPlus",  "PidAlgoMl");
		theAnalysis->FillList(muminus, "MuonTightMinus", "PidAlgoMl");
		theAnalysis->FillList(piplus,  "PionLoosePlus",  "PidAlgoMl");
		theAnalysis->FillList(piminus, "PionLooseMinus", "PidAlgoMl");
		
		jpsi.Combine(muplus, muminus);
		for (j=0;j<jpsi.GetLength();++j) hjpsim_tpid->Fill( jpsi[j]->M() );
		jpsi.Select(jpsiMassSel);
		
		psi2s.Combine(jpsi, piplus, piminus);
		for (j=0;j<psi2s.GetLength();++j) hpsim_tpid->Fill( psi2s[j]->M() );
		
	}
	
	// *** write out all the histos
	out->cd();
	
	hjpsim_all->Write();
	hpsim_all->Write();
	hjpsim_lpid->Write();
	hpsim_lpid->Write();
	hjpsim_tpid->Write();
	hpsim_tpid->Write();
	hjpsim_trpid->Write();
	hpsim_trpid->Write();
	
	hjpsim_ftm->Write();
	hpsim_ftm->Write();
	hjpsim_nm->Write();
	hpsim_nm->Write();
	
	hpsim_diff->Write();
	hjpsim_diff->Write();
	
	hjpsim_vf->Write();
	hjpsim_4cf->Write();
	hjpsim_mcf->Write();
	
	hjpsi_chi2_vf->Write();
	hpsi_chi2_4c->Write();
	hjpsi_chi2_mf->Write();
			
	hjpsi_prob_vf->Write();
	hpsi_prob_4c->Write();
	hjpsi_prob_mf->Write();
			
	hvpos->Write();
		
	out->Save();
	
}
