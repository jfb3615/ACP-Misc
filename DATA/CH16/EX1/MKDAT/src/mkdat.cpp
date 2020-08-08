#include "QatDataAnalysis/OptParse.h"
#include "QatDataAnalysis/Table.h"
#include "QatDataAnalysis/HistogramManager.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotting/PlotStream.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatGenericFunctions/CubicSplinePolynomial.h"
#include "QatGenericFunctions/RombergIntegrator.h"
#include "QatGenericFunctions/VoigtDistribution.h"
#include "QatGenericFunctions/Variable.h"
#include "QatDataModeling/HistChi2Functional.h"
#include "QatDataModeling/TableLikelihoodFunctional.h"
#include "QatDataModeling/MinuitMinimizer.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random>


int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + " -i Input -o Output [-v] [NPOINTS=val/10000] [NOUT=val/1000]"; 
  if (argc==1) {
    std::cout << usage << std::endl;
    exit(0);
  }

  unsigned int NPOINTS=10000;
  unsigned int NOUT   = 1000;
  NumericInput input;
  input.declare("NPOINTS", "Number of points to generate",NPOINTS);
  input.declare("NOUT",    "Number of points to output  ",NOUT);
  try {
    input.optParse(argc, argv);
  }
  catch (const std::exception & e) {
    std::cerr << "Error " << e.what()  << std::endl;
    std::cerr << input.usage() << std::endl;
    std::cerr << usage    << std::endl << std::endl;
    exit(1);
  }
  
  HIOOneToOne hio;
  try {
    hio.optParse(argc,argv);
  }
  catch (std::exception & e) {
    std::cerr << "Error " << e.what()  << std::endl;
    std::cerr << usage    << std::endl << std::endl;
    exit(2);
  }
  if (argc!=1) {      
    std::cerr << "Unrecognized option: " << argv[1] <<std::endl;
    exit(3);
  }

  NPOINTS = (unsigned int) (0.5+input.getByName("NPOINTS"));
  NOUT    = (unsigned int) (0.5+input.getByName("NOUT"));
  
  QApplication     app(argc,argv);
  
  MultipleViewWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *nextAction=toolBar->addAction("Next");
  
  nextAction->setShortcut(QKeySequence("n"));
  
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));
  
  PRectF rect;
  rect.setXmin(5860.0);
  rect.setXmax(5930.0);
  rect.setYmin(0.0);
  rect.setYmax(200.0);

  PlotFunction1D::Properties prop;
  prop.pen.setColor("darkRed");
  prop.pen.setWidth(3);


  PlotView view0(rect);
  window.add(&view0, "Stage 1");

  PlotView view1(rect);
  window.add(&view1, "Stage 2");

  PlotView view2(rect);
  window.add(&view2, "Stage 3");

  Genfun::CubicSplinePolynomial csPoly;
  const Table *csPoints=hio.input->findTable("CubicSplineControlPoints");
  if (!csPoints) {
    throw std::runtime_error("CubicSplineControlPoints table not found on input file " + hio.input->name());
  }

  for (size_t t=0;t<csPoints->numTuples();t++) {
    double x=0,y=0;
    csPoints->read(t,"X", x);
    csPoints->read(t,"Y", y);
    csPoly.addPoint(x,y);
  }

  Genfun::RombergIntegrator I(rect.xmin(), rect.xmax());
  

  std::random_device dev;
  std::mt19937       engine(dev());
  std::uniform_real_distribution<double> flatX(rect.xmin(), rect.xmax());
  std::uniform_real_distribution<double> flatY(rect.ymin(), rect.ymax());


  Hist1D *h0=hio.output->newHist1D("Pass0", 100, rect.xmin(), rect.xmax());
  Hist1D *h1=hio.output->newHist1D("Pass1", 100, rect.xmin(), rect.xmax());
  Hist1D *h2=hio.output->newHist1D("Pass2", 100, rect.xmin(), rect.xmax());


  PlotFunction1D pcsPoly=csPoly/I(csPoly)*NPOINTS*h0->binWidth();
  {
    pcsPoly.setProperties(prop);
  }

  view0.add(&pcsPoly);

  for (unsigned int e=0;e<NPOINTS;e++) {
    while (1) {
      double x=flatX(engine), y=flatY(engine);
      if (y<csPoly(x)) {
	h0->accumulate(x);
	break;
      }
    }
  }
  
  PlotHist1D pH=*h0;
  PRectF crect=pH.rectHint(); crect.setYmax(1.1*crect.ymax());
  view0.setRect(crect);
  view0.add(&pH);

  PlotView *view[3]={&view0,&view1,&view2};
  std::string title[3]={"Use Cubic spline",
			"Use Breit-Wigner and Gaussian",
			"Fit final datase"};
			
  for (int i=0;i<3;i++) {
    PlotStream titleStream(view[i]->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< title[i]
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(view[i]->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "Eᵧ [eV]"
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(view[i]->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "Counts/bin"
		 << PlotStream::EndP();
    
    
  }
  // Now fit:
  
  Genfun::Parameter delta1("delta1", 1.5, 0.5, 2.5);
  Genfun::Parameter sigma1("sigma1", 1.5, 0.5, 2.5);
  Genfun::Parameter delta2("delta2", 1.5, 0.5, 2.5);
  Genfun::Parameter sigma2("sigma2", 1.5, 0.5, 2.5);
  Genfun::Parameter f("F", 0.3, 0.0, 1.0);
  Genfun::Parameter x1("x1", 5888, 5860, 5930);
  Genfun::Parameter x2("x2", 5900, 5860, 5930);
  Genfun::Parameter alpha("alpha", 1.0, 0.9, 1.1);
  Genfun::Variable X;
  Genfun::VoigtDistribution voigt1,voigt2;
  
  voigt1.sigma().connectFrom(&sigma1);
  voigt1.delta().connectFrom(&delta1);
  voigt2.sigma().connectFrom(&sigma2);
  voigt2.delta().connectFrom(&delta2);
    

  
  Genfun::GENFUNCTION ff=f*voigt1(X-x1)+(1-f)*voigt2(X-x2);
  
  {
    MinuitMinimizer minimizer;
    HistChi2Functional chi2(h0);
    Genfun::GENFUNCTION cff=alpha*NPOINTS*ff;
    
    minimizer.addParameter(&delta1);
    minimizer.addParameter(&delta2);
    minimizer.addParameter(&sigma1);
    minimizer.addParameter(&sigma2);
    minimizer.addParameter(&x1);
    minimizer.addParameter(&x2);
    minimizer.addParameter(&f);
    minimizer.addParameter(&alpha);
    

    minimizer.addStatistic(&chi2, &cff);
    minimizer.minimize();
    record(hio.output->newDir("Results-0"), minimizer);
  }
  
  PlotFunction1D pv=NPOINTS*h1->binWidth()*ff;
  {
    pv.setProperties(prop);
  }
  view1.add(&pv);

  {
    std::bernoulli_distribution f12(f.getValue());
    std::cauchy_distribution<double> cauchy1(x1.getValue(),voigt1.delta().getValue());
    std::cauchy_distribution<double> cauchy2(x2.getValue(),voigt2.delta().getValue());
    std::normal_distribution<double> normal1(0,voigt1.sigma().getValue());
    std::normal_distribution<double> normal2(0,voigt2.sigma().getValue());
    
    for (unsigned int i=0;i<NPOINTS;i++) {
      bool c12=f12(engine);
      double x=c12? normal1(engine)+cauchy1(engine):normal2(engine)+cauchy2(engine);
      h1->accumulate(x);
    }
  }
  
  PlotHist1D pHNew=*h1;
  view1.setRect(crect);
  view1.add(&pHNew);

  {
    MinuitMinimizer minimizer;
    HistChi2Functional chi2(h1);
    Genfun::GENFUNCTION cff=alpha*NPOINTS*ff;
    
    minimizer.addParameter(&delta1);
    minimizer.addParameter(&delta2);
    minimizer.addParameter(&sigma1);
    minimizer.addParameter(&sigma2);
    minimizer.addParameter(&x1);
    minimizer.addParameter(&x2);
    minimizer.addParameter(&f);
    minimizer.addParameter(&alpha);
    

    minimizer.addStatistic(&chi2, &cff);
    minimizer.minimize();
    record(hio.output->newDir("Results-1"), minimizer);
  }

  Table *outputTable=hio.output->newTable("Fe55 Decay");
  
  {
    std::bernoulli_distribution f12(f.getValue());
    std::cauchy_distribution<double> cauchy1(x1.getValue(),voigt1.delta().getValue());
    std::cauchy_distribution<double> cauchy2(x2.getValue(),voigt2.delta().getValue());
    std::normal_distribution<double> normal1(0,voigt1.sigma().getValue());
    std::normal_distribution<double> normal2(0,voigt2.sigma().getValue());
    
    for (unsigned int i=0;i<NOUT;i++) {
      bool c12=f12(engine);
      double x=c12? normal1(engine)+cauchy1(engine):normal2(engine)+cauchy2(engine);
      h2->accumulate(x);
      outputTable->add("EnergyEV", x);
      outputTable->capture();
    }
  }

  {
    MinuitMinimizer minimizer;
    TableLikelihoodFunctional chi2(*outputTable);
    Genfun::GENFUNCTION energyEV=outputTable->symbol("EnergyEV");
    Genfun::GENFUNCTION cff=ff(energyEV);
    
    minimizer.addParameter(&delta1);
    minimizer.addParameter(&delta2);
    minimizer.addParameter(&sigma1);
    minimizer.addParameter(&sigma2);
    minimizer.addParameter(&x1);
    minimizer.addParameter(&x2);
    minimizer.addParameter(&f);
    //    minimizer.addParameter(&alpha);
    alpha.setValue(1.0);

    minimizer.addStatistic(&chi2, &cff);
    minimizer.minimize();
    record(hio.output->newDir("Results-2"), minimizer);
  }

  PlotFunction1D pf=NOUT*h2->binWidth()*ff;
  {
    pf.setProperties(prop);
  }
  view2.add(&pf);


  PlotHist1D pHUlt=*h2;
  view2.setRect(pHUlt.rectHint());
  view2.add(&pHUlt);
  
  window.show();
  app.exec();
  outputTable->print();
  return 1;
}

