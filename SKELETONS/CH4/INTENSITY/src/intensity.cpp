#include "InterpolationMainWindow.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <cstdlib>
#include <iostream>
#include <string>
#include <libgen.h>
int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0] + "[-f filename]"; 
  char *wd=dirname(argv[0]);
  std::string filename=std::string(wd)+"/../../src/inputBlueOnRed.png";
  for (int i=1; i<argc;i++) {
    if (std::string(argv[i])=="-f") {
      i++;
      filename=argv[i];
      std::copy (argv+i+1, argv+argc, argv+i-1);
      argc -=2;
      i    -=2;
    }
  }

  if (argc!=1) {
    std::cout << usage << std::endl;
  }




  QApplication     app(argc,argv);
  
  InterpolationMainWindow window(NULL, filename);
  app.connect(&app,SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
  
  window.show();
  app.exec();
  return 1;
}

