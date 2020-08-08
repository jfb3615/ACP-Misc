#include "Interpolator.h"
#include <iostream>
RGBAlphaData Interpolator::interpolate(double x, double y) {
  RGBAlphaData rgb;
  rgb.red=0;
  rgb.green=0;
  rgb.blue=0;
  rgb.alpha=0;
  static bool init=0;
  if (!init) {
    for (size_t i=0;i<data.gridPointsX.size();i++) {
      for (size_t j=0;j<data.gridPointsY.size();j++) {
	std::cout
	  << "x=" << data.gridPointsX[i]
	  << "y=" << data.gridPointsY[i]
	  << "R,G,B,alpha="
	  << data.rgbAlphaValues[i][j].red << ","
	  << data.rgbAlphaValues[i][j].green << ","
	  << data.rgbAlphaValues[i][j].blue << ","
	  << data.rgbAlphaValues[i][j].alpha << std::endl;
      }
    }
    init=true;
  }
  rgb.red=x;
  rgb.blue=y;
  return rgb;
}

