Utilizing Machine Learning Techniques for Particle Identification in PANDA experiment

To be able to use this code you have to be sure that Python interpreter is installed on your machine. If you do not have Python,
Please navigate to Python website https://www.python.org/. It is preferable to install Python2 since we are using ROOT, and usually 
Python3 with ROOT have a lot of problems.

- [x] Having Python2 installed we can now install the necessary Python modules. The required packages are:
   * ROOT (Python bindings of ROOT  https://root.cern.ch/building-root).
   * ipython
   * jupyter (optional)
   * scipy
   * numpy
   * root_numpy
   * pandas
   * matplotlib
   * seaborn
   * xgboost
   * tensorflow (for deep learning)
   * tensorboard (for deep learning)
   * keras (for deep learning)

to install any of these modules use pip, e.g `pip install ipython`. If you do not know the exact name of the module use `pip search`
and to narrow the search use `grep` with `pip`, e.g `pip search numpy | grep numpy`.

- [x] We are ready, the most important file here is `Classifier.py`. Open ipython from shell prompot, just type `ipython`,
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
* input trained model (BDT or ANN).
* output file (which will contain Ml probability distributions).
an example call in ipython would be:

`Classifier.getRootPrediction('signal_pid.root', 'BDT_TrainedOnEvt.pkl', 'signal_ml.root')`

please make sure that `signal_pid.root` and `BDT_TrainedOnEvt.pkl` exist in the same directory. This function will create an output ROOT file named
`signal_ml.root` which contain Ml output probability distributions.

- [x] Finally in your analysis code `tut_ana_ml.C` use `fRun->AddFriend("signal_ml.root");` to be able to access the Ml output probability 
distributions.

- [x] For example tight selection of muons using Ml would be in the analysis code:

`theAnalysis->FillList(muminus, "MuonTightMinus", "PidAlgoMl");`

Note: the analysis code taken from http://panda-wiki.gsi.de/cgi-bin/view/Computing/PandaRootRhoTutorial

- [x] You can plot a histogram of any of the input features using `PlotFeature()`, or you can plot a scatter between two 
features using `PlotScatter()`.

- [x] You can plot a correlation matrix between all features by using `PlotCorrMatrix()`.



- [x] **In addition to pure Python, the Classifier module is also avaliable in C++. Type at terminal `root -l -q 'Classifier.C(inPidFile,weightFile,outFile)'`,
  where inPidFile is the input file (id file, last step of PANDARoot simulation), weightFile is the trained model in .xml format, and outFile is the outputfile 
  (which will contain Ml probability distributions). In this case you will need at least Root v6.08**

- [x] Installation via miniconda 
     1. Go to miniconda web page https://docs.conda.io/en/latest/miniconda.html and download miniconda for Python2.7.
     2. Change the access permission `chmod +x Miniconda2-version.sh`.
     3. Excute via `Miniconda2-version.sh`
     4. Follow the instructions.
     5. Create new conda enviroment (e.g PandaMlEnv) via `conda create -n PandaMlEnv python=2.7`, if doesn't work restart the terminal.
     6. Activate the enviroment via `conda activate PandaMlEnv`
     7. Install the required packages via `conda install -c nlesc root_numpy scipy pandas matplotlib seaborn xgboost keras ipython jupyter`
     8. At `pandaroot` build directory and run `. ./config`
     9. Now you everything installed type `ipython` at the terminal and then import the Classifier module `import Classifier`.

Have Fun!
