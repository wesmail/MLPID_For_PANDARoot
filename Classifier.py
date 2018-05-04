from array import array
import math
import ROOT
import pandas as pd
import numpy as np
import collections
import keras
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.externals import joblib
from sys import exit
from ROOT import (PndPidProbability, TFile, TClonesArray, TTree, AddressOf, TObject, gROOT, FairRootManager, TList, TNamed, FairRunAna)
from ROOT import (PndPidMlAssociatorTask, FairRunAna, FairGeane, TFile, kFALSE, kTRUE, TStopwatch)

def loadmodel(x):
    '''
    loadmodel(string) -> sklearn.classifier
    
    Used to load the trained model.
    '''
    from xgboost import XGBClassifier
    x = str(x)
    #clf = keras.models.load_model(x)
    clf = joblib.load(x)
    return clf


def loadmyfile(x):
    '''
    loadmyfile(string) -> pandas DataFrame
    
    Accepts Root file of FairRoot Folder structure, extracts the features used in the training step, and return pandas
    DataFrame of the features.
    '''
    x = str(x)
    try:
        infile = ROOT.TFile.Open(x, 'read')
        if (infile.IsZombie()):
            print ('Error Opening Input File.')
            exit(-1)
        intree = infile.Get('cbmsim')
        PidChargedCand = ROOT.TClonesArray('PndPidCandidate')
        intree.GetBranch('PidChargedCand').SetAutoDelete(ROOT.kFALSE)
        intree.SetBranchAddress('PidChargedCand', ROOT.AddressOf(PidChargedCand))
        
        energy = array('d')
        charge = array('d')
        momentumx = array('d')
        momentumy = array('d')
        momentumz = array('d')
        momentum = array('d')
        position = array('d')
        MvdDEDX = array('d')
        MvdHits = array('i')
        SttMeanDEDX = array('d')
        SttHits = array('i')
        GemHits = array('i')
        TofStopTime = array('d')
        TofM2 = array('d')
        TofTrackLength = array('d')
        TofQuality = array('d')
        TofBeta = array('d')
        DrcThetaC = array('d')
        DrcQuality = array('d')
        DiscThetaC = array('d')
        DiscQuality = array('d')
        EmcRawEnergy = array('d')
        EmcCalEnergy = array('d')
        EmcQuality = array('d')
        EmcNumberOfCrystals = array('i')
        EmcNumberOfBumps = array('i')
        EmcModule = array('i')
        EmcZ20 = array('d')
        EmcZ53 = array('d')
        EmcLat = array('d')
        EmcE1 = array('d')
        EmcE9 = array('d')
        EmcE25 = array('d')
        MuoQuality = array('d')
        MuoIron = array('d')
        MuoMomentumIn = array('d')
        MuoNumberOfLayers = array('d')
        MuoModule = array('i')
        MuoHits = array('i')
        DegreesOfFreedom = array('d')
        ChiSquared = array('d')
        events = array('i')
        ii = 0
        
        for event in intree:
            ii = ii + 1
            for b in range(PidChargedCand.GetEntries()):
                events.append(ii)
                cand = PidChargedCand.At(b)
                energy.append(cand.GetEnergy())
                charge.append(cand.GetCharge())
                momentumx.append(cand.GetMomentum().X())
                momentumy.append(cand.GetMomentum().Y())
                momentumz.append(cand.GetMomentum().Z())
                mom = math.sqrt(math.pow(cand.GetMomentum().X(),2) + math.pow(cand.GetMomentum().Y(),2) + math.pow(cand.GetMomentum().Z(),2))
                momentum.append(mom)
                pos = math.sqrt(math.pow(cand.GetPosition().X(),2) + math.pow(cand.GetPosition().Y(),2) + math.pow(cand.GetPosition().Z(),2))
                position.append(pos)
                MvdDEDX.append(cand.GetMvdDEDX())
                MvdHits.append(cand.GetMvdHits())
                SttMeanDEDX.append(cand.GetSttMeanDEDX())
                SttHits.append(cand.GetSttHits())
                GemHits.append(cand.GetGemHits())
                TofStopTime.append(cand.GetTofStopTime())
                TofM2.append(cand.GetTofM2())
                TofTrackLength.append(cand.GetTofTrackLength())
                TofQuality.append(cand.GetTofQuality())
                TofBeta.append(cand.GetTofBeta())
                DrcThetaC.append(cand.GetDrcThetaC())
                DrcQuality.append(cand.GetDrcQuality())
                DiscThetaC.append(cand.GetDiscThetaC())
                DiscQuality.append(cand.GetDiscQuality())
                EmcRawEnergy.append(cand.GetEmcRawEnergy())
                EmcCalEnergy.append(cand.GetEmcCalEnergy())
                EmcQuality.append(cand.GetEmcQuality())
                EmcNumberOfCrystals.append(cand.GetEmcNumberOfCrystals())
                EmcNumberOfBumps.append(cand.GetEmcNumberOfBumps())
                EmcModule.append(cand.GetEmcModule())
                EmcZ20.append(cand.GetEmcClusterZ20())
                EmcZ53.append(cand.GetEmcClusterZ53())
                EmcLat.append(cand.GetEmcClusterLat())
                EmcE1.append(cand.GetEmcClusterE1())
                EmcE9.append(cand.GetEmcClusterE9())
                EmcE25.append(cand.GetEmcClusterE25())
                MuoQuality.append(cand.GetMuoQuality())
                MuoIron.append(cand.GetMuoIron())
                MuoMomentumIn.append(cand.GetMuoMomentumIn())
                MuoNumberOfLayers.append(cand.GetMuoNumberOfLayers())
                MuoModule.append(cand.GetMuoModule())
                MuoHits.append(cand.GetMuoHits())
                DegreesOfFreedom.append(cand.GetDegreesOfFreedom())
                ChiSquared.append(cand.GetChiSquared())

        features = collections.OrderedDict()
        #features['momentumx'] = np.array(momentumx)
        #features['momentumy'] = np.array(momentumy)
        #features['momentumz'] = np.array(momentumz)
        features['momentum'] = np.array(momentum)
        features['energy']  = np.array(energy)
        #features['charge']  = np.array(charge)
        #features['position'] = np.array(position) 
        features['MvdDEDX'] = np.array(MvdDEDX)
        features['MvdHits'] = np.array(MvdHits) 
        features['SttMeanDEDX'] = np.array(SttMeanDEDX)
        features['SttHits'] = np.array(SttHits)
        features['GemHits'] = np.array(GemHits) 
        features['TofStopTime'] = np.array(TofStopTime)
        features['TofM2'] = np.array(TofM2)
        features['TofTrackLength'] = np.array(TofTrackLength)
        features['TofQuality'] = np.array(TofQuality)
        features['TofBeta'] = np.array(TofBeta)
        features['DrcThetaC'] = np.array(DrcThetaC)
        features['DrcQuality'] = np.array(DrcQuality)
        features['DiscThetaC'] = np.array(DiscThetaC)
        features['DiscQuality'] = np.array(DiscQuality)
        features['EmcRawEnergy'] = np.array(EmcRawEnergy)
        features['EmcCalEnergy'] = np.array(EmcCalEnergy)
        features['EmcQuality'] = np.array(EmcQuality)
        features['EmcNumberOfCrystals'] = np.array(EmcNumberOfCrystals)
        features['EmcNumberOfBumps'] = np.array(EmcNumberOfBumps)
        features['EmcModule'] = np.array(EmcModule)
        features['EmcZ20'] = np.array(EmcZ20)
        features['EmcZ53'] = np.array(EmcZ53)
        features['EmcLat'] = np.array(EmcLat)
        features['EmcE1'] = np.array(EmcE1)
        features['EmcE9'] = np.array(EmcE9)
        features['EmcE25'] = np.array(EmcE25)
        features['MuoQuality'] = np.array(MuoQuality)
        features['MuoIron'] = np.array(MuoIron)
        features['MuoMomentumIn'] = np.array(MuoMomentumIn)
        features['MuoNumberOfLayers'] = np.array(MuoNumberOfLayers)
        features['MuoModule'] = np.array(MuoModule)
        features['MuoHits'] = np.array(MuoHits)
        features['DegreesOfFreedom'] = np.array(DegreesOfFreedom)
        features['ChiSquared'] = np.array(ChiSquared)
        features['temp'] = np.array(events)
        
        Features = {#'momentumx': np.array(momentumx), \
                    #'momentumy': np.array(momentumy), \
                    #'momentumz': np.array(momentumz), \
                    'momentum': np.array(momentum), \
                    'energy'  : np.array(energy), \
                    #'charge'  : np.array(charge), \
                    #'position' : np.array(position), \
                    'MvdDEDX' : np.array(MvdDEDX), \
                    'MvdHits' : np.array(MvdHits), \
                    'SttMeanDEDX' : np.array(SttMeanDEDX), \
                    'SttHits' : np.array(SttHits), \
                    'GemHits' : np.array(GemHits), \
                    'TofStopTime' : np.array(TofStopTime), \
                    'TofM2' : np.array(TofM2), \
                    'TofTrackLength' : np.array(TofTrackLength), \
                    'TofQuality' : np.array(TofQuality), \
                    'TofBeta': np.array(TofBeta), \
                    'DrcThetaC': np.array(DrcThetaC), \
                    'DrcQuality' : np.array(DrcQuality),
                    'EmcRawEnergy' : np.array(EmcRawEnergy), \
                    'EmcCalEnergy' : np.array(EmcCalEnergy), \
                    'EmcQuality' : np.array(EmcQuality), \
                    'EmcNumberOfCrystals' : np.array(EmcNumberOfCrystals), \
                    'EmcNumberOfBumps' : np.array(EmcNumberOfBumps), \
                    'EmcModule' : np.array(EmcModule), \
                    'EmcZ20' : np.array(EmcZ20), \
                    'EmcZ53' : np.array(EmcZ53), \
                    'EmcLat' : np.array(EmcLat), \
                    'EmcE1' : np.array(EmcE1), \
                    'EmcE9' : np.array(EmcE9), \
                    'EmcE25' : np.array(EmcE25), \
                    'MuoQuality' : np.array(MuoQuality), \
                    'MuoIron' : np.array(MuoIron), \
                    'MuoMomentumIn' : np.array(MuoMomentumIn), \
                    'MuoNumberOfLayers' : np.array(MuoNumberOfLayers), \
                    'MuoModule' : np.array(MuoModule), \
                    'MuoHits' : np.array(MuoHits), \
                    'DegreesOfFreedom' : np.array(DegreesOfFreedom), \
                    'ChiSquared' : np.array(ChiSquared), \
                    'temp' : np.array(events)}
        
        my_df = pd.DataFrame(features)
        my_df['E/p'] = my_df.loc[:,'EmcCalEnergy']/my_df.loc[:,'momentum']
        #my_df['Pt'] = np.sqrt( my_df.loc[:,'momentumx']**2  + my_df.loc[:,'momentumy']**2 )
        #my_df = my_df.drop(['momentumx'], axis=1)
        #my_df = my_df.drop(['momentumy'], axis=1)
        my_df['events'] = my_df.loc[:,'temp']
        my_df = my_df.drop(['temp'], axis=1)
        print ('File loaded correctly, and the DataFrame is created.')
        return my_df
        
    except AttributeError:
        print ('Corrupt File or Tree.')
        
        
def predict(myfile, mymodel, flag=False):
    '''
    predict(string, string, boolean) -> pandas DataFrame
    
    Accepts three arguments, the first one is a Root file of FairRoot Folder structure that contains your simulated 
    events,the second argument is the saved model, the third argument (optional) if true will print the entries of 
    the DataFrame. Return pandas DataFrame of probabilities of being different particle species.
    '''
    from xgboost import XGBClassifier
    myfile  = str(myfile)
    mymodel = str(mymodel)
    df = loadmyfile(myfile)
    entry_to_predict_on = df.iloc[:,0:-1]
    
    #from sklearn.preprocessing import StandardScaler
    #scaler = StandardScaler()
    #scaler.fit(entry_to_predict_on)
    #entry_to_predict_on = scaler.transform(entry_to_predict_on)  
    
    clf = loadmodel(mymodel)
    particles = clf.predict_proba(entry_to_predict_on)
    List = ['e','pi','mu','k','p']
    particles = pd.DataFrame(particles, columns=List)
    particles['event'] = df.loc[:,'events']
    if flag==True:
        print(particles.head(5))
    return particles
    
def getRootPrediction(myfile, mymodel, outfile):
    '''
    getRootPrediction(string, string, string) -> None
    
    Accepts input ROOT file of FairRoot Folder structure that contains your simulated events, the saved model, and 
    outputs another ROOT file that contains probability distributions of different particles in a branch called 
    (PidAlgoMl).
    '''
    myfile  = str(myfile)
    mymodel = str(mymodel)
    outfile = str(outfile)
    try:
        infile = ROOT.TFile.Open(myfile, 'update')
        if (infile.IsZombie()):
            print ('Error Opening Input File.')
            exit(-1)
        intree = infile.Get('cbmsim')
        
        Array = ROOT.TClonesArray('PndPidProbability')
        newBr = intree.Branch('PidAlgoMl', 'TClonesArray', ROOT.AddressOf(Array))
        List = ['e','pi','mu','k','p']
        particles = predict(myfile, mymodel)
        
        for i in range(1,intree.GetEntries()+1):
            if ((i%100)==0):
                print ('Event ')
                print (i)
            new_df = particles.loc[particles.loc[:,'event']==i, List]
            index = len(new_df.index)
            for j in range(index):
                x_0 = new_df.iloc[j,0]    # Electron
                x_1 = new_df.iloc[j,1]    # Pion
                x_2 = new_df.iloc[j,2]    # Muon
                x_3 = new_df.iloc[j,3]    # Kaon
                x_4 = new_df.iloc[j,4]    # Proton
                ProbabilityObject = PndPidProbability(x_0, x_2, x_1, x_3, x_4, j)
                Array[j] = ProbabilityObject
            newBr.Fill()
            Array.Clear()
        
        infile.cd()
        intree.Write('',TObject.kOverwrite)
        
        
        nEvents = 0
        fRun = FairRunAna()
        fRun.SetInputFile(myfile)
        fRun.SetOutputFile(outfile)
        fRun.SetGenerateRunInfo(kFALSE)

        myFile = ROOT.TFile(myfile, 'update')
        assMl = PndPidMlAssociatorTask(myFile)
        fRun.AddTask(assMl)
        print ('fRun.Init()')
        fRun.Init()
        fRun.Run(0,nEvents)
        infile.Close()
    except AttributeError:
        print ('Corrupt File or Tree.')
    except ReferenceError:
        print ('File does not exist.')       
