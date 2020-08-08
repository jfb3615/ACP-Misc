#include "InterpolationMainWindow.h"
#include <QPixmap>
#include <QImage>
#include <iostream>
#include "Interpolator.h"

class InterpolationMainWindow::Clockwork {

 public:
  QPixmap *pixmap   = NULL;
  QImage  *gImage   = NULL;
  QImage  *oImage   = NULL;

};


InterpolationMainWindow::InterpolationMainWindow(QWidget *parent,
						 const std::string &filename):QMainWindow(parent),c(new Clockwork)
{
  // Set up  the user interface:
  ui.setupUi(this);

  // connect the quit button to the quit method and give it keyboard shortcut "q"
  connect(ui.quitButton,SIGNAL(pressed()), this, SLOT(close()));
  ui.quitButton->setShortcut(QKeySequence("q"));

  // connect the spin boxes to the refresh method:
  connect(ui.xSamplesSpinBox, SIGNAL(editingFinished()), this, SLOT(refresh()));
  connect(ui.ySamplesSpinBox, SIGNAL(editingFinished()), this, SLOT(refresh()));
  
  // display the input file:
  c->pixmap=new QPixmap(filename.c_str());
  ui.label1->setPixmap(*c->pixmap);

  // allocate the sampling window:
  c->gImage=new QImage(c->pixmap->toImage());
  
  // allocate the output window:
  c->oImage = new QImage(c->pixmap->toImage());

  // refresh:
  refresh();
  

}

InterpolationMainWindow::~InterpolationMainWindow() {
  delete c->gImage;
  delete c->oImage;
  delete c->pixmap;
  delete c;
}


void InterpolationMainWindow::refresh() {
  using namespace std;
  
  // Get the number of samples from the spin box:
  double NSAMPLEX=ui.xSamplesSpinBox->value();
  double NSAMPLEY=ui.ySamplesSpinBox->value();

  // White out the sampling window:
  for (int i=0;i<c->gImage->width();i++) {
    for (int j=0;j<c->gImage->height();j++) {
      c->gImage->setPixel(i,j,0XFFFFFFFF);
    }
  }
  ui.label2->setPixmap(QPixmap::fromImage(*c->gImage));

  double dX= NSAMPLEX==1 ? ui.label2->pixmap()->width()/2 : ui.label2->pixmap()->width()/(NSAMPLEX-1);
  double dY= NSAMPLEY==1 ? ui.label2->pixmap()->height()/2 : ui.label2->pixmap()->height()/(NSAMPLEY-1);

  // This could slow down the application, consider allocating it once and for all.  
  QImage  iImage   = c->pixmap->toImage();

  // Make a list of "hot spots"; these are the ones we will use for interpolation:
  InterpolationData hotSpot;

  hotSpot.gridPointsX.resize(NSAMPLEX);
  hotSpot.gridPointsY.resize(NSAMPLEY);
  hotSpot.rgbAlphaValues.resize(NSAMPLEX);
  for (int i=0;i<NSAMPLEX;i++){
    int    iX = NSAMPLEX==1 ? (i+1)*dX: i*dX;
    double x= double(iX)/iImage.width();
    hotSpot.gridPointsX[i]=x;
    hotSpot.rgbAlphaValues[i].resize(NSAMPLEY);
    for (int j=0;j<NSAMPLEY;j++){

      int jY=NSAMPLEY==1 ? (j+1)*dY : j*dY;
      double y= double(jY)/iImage.width();
      hotSpot.gridPointsY[j]=y;

      QRgb zRGB = iImage.pixel(min(max(iX,0),299), min(max(jY,0),299));
     
      RGBAlphaData data;
      data.red=qRed(zRGB)/255.0;
      data.green=qGreen(zRGB)/255.0;
      data.blue=qBlue(zRGB)/255.0;
      data.alpha=qAlpha(zRGB)/255.0;
      hotSpot.rgbAlphaValues[i][j]=data;
      
      static const int WINDOW=10;
      for (int i=-WINDOW;i<=WINDOW;i++){
	int iXX=iX-i;
	if (iXX>0 && iXX<ui.label2->pixmap()->width()) {
	  for (int j=-WINDOW;j<=WINDOW;j++){
	    int jYY=jY-j;
	    if (jYY>0 && jYY<ui.label2->pixmap()->height()) {
	      c->gImage->setPixel(iXX,jYY,zRGB);
	    }
	  }
	}
      }
    }
  }
  ui.label2->setPixmap(QPixmap::fromImage(*c->gImage));
  
  Interpolator interpolator(hotSpot);
  for (int i=0;i<c->oImage->width();i++) {
    double x = double(i)/c->oImage->width();
    for (int j=0;j<c->oImage->height();j++) {
      double y = double(j)/c->oImage->height();
      RGBAlphaData data = interpolator.interpolate(x,y);
      QRgb rgb=QColor(min(max(data.red*255,0.0),255.),
		      min(max(data.green*255,0.0),255.),
		      min(max(data.blue*255,0.0),255.),
		      min(max(data.alpha*255,0.0),255.)).rgba();
      c->oImage->setPixel(i,j,rgb);
    }
  }
  ui.label3->setPixmap(QPixmap::fromImage(*c->oImage));

}
