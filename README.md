# MLPID_For_PANDARoot
Utilizing Machine Learning Techniques for Particle Identification in PANDA experiment

To be able to use this code you have to be sure that Python interpreter is installed on your machine. If you do not have Python,
Please navigate to Python website https://www.python.org/. It is preferable to install Python2 since we are using ROOT, and usually 
Python3 with ROOT have a lot of problems.

1. Having Python2 installed we can now install the necessary Python modules. The required packages are:
   * ROOT (Python bindings of ROOT  https://root.cern.ch/building-root).
   * ipython
   * jupyter (optional)
   * scipy
   * numpy
   * root_numpy
   * pandas
   * matplotlib
   * seaborn
   * tensorflow (for deep learning)
   * tensorboard (for deep learning)
   * keras (for deep learning)

to install any of these modules use pip, e.g `pip install ipython`. If you do not know the exact name of the module use `pip search`
and to narrow the search use `grep` with `pip`, e.g `pip search numpy | grep numpy`.

2. The second step is to copy these two files `PndPidMlAssociatorTask.cxx` and `PndPidMlAssociatorTask.h` to:  
`yourPandaRootDirectory/source/pid/PidClassifier/`. Then open `CMakeLists.txt` and add the following lines:   
`PidClassifier/PndPidMlAssociatorTask.cxx`   and   `PidClassifier/PndPidMlAssociatorTask.h`.

You can find easily where to paste these lines. Then open `PidLinkDef.h` and add the following line:  
`#pragma link C++ class  PndPidMlAssociatorTask+;`

Finally compile PANDARoot again from the build directory.

3. WOW!. We are ready, the most important file here is `Classifier.py`. Open ipython from shell prompot, just type `ipython`,
then import the `Classifier` module by typing inside ipython `import Classifier`.

The module contains some useful method, here is a list of the methods:
* `loadmodel()`
* `loadmyfile()`
* `getListofFeatures()`
* `predict()`
* `getRootPrediction()`
* `PlotCorrMatrix()`
* `PlotFeature()`
* `PlotScatter()`

to learn about any of these methods (what it does, what arguments it take, what it's return value) you can use help builtin method
just type in ipython e.g `help(Classifier.PlotFeature)`, then you will get a documentation of the method. 

For the purpose of using machine learning output probability distributions in your analysis, the `getRootPrediction()` method is
in hand, it accepts three parameters 
* input file (pid file, last step of PANDARoot simulation).
* input trained model (at the moment only Random Forest classifier is available `savedRF.pkl` ).
* output file (which will contain Ml probability distributions).
an example call in ipython would be:

`Classifier.getRootPrediction('signal_pid.root', 'BDT_TrainedOnEvt.pkl', 'signal_ml.root')`

please make sure that `signal_pid.root` and `savedRF.pkl` exist in the same directory. This function will create named 
`signal_ml.root` which contain Ml output probability distributions.

4. Finally in your analysis code `tut_ana.C` use `fRun->AddFriend("signal_ml.root");` to be able to access the Ml output probability 
distributions.

5. For example tight selection of muons using Ml would be in the analysis code:

`theAnalysis->FillList(muminus, "MuonTightMinus", "PidAlgoMl");`

Note: the analysis code taken from http://panda-wiki.gsi.de/cgi-bin/view/Computing/PandaRootRhoTutorial

6. You can plot a histogram of any of the input features using `PlotFeature()`, or you can plot a scatter between two 
features using `PlotScatter()`.

7. You can plot a correlation matrix between all features by using `PlotCorrMatrix()`.

Have Fun!
