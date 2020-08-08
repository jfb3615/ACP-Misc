#ifndef _INTERPOLATOR_H_
#define _INTERPOLATOR_H_
#include <vector>
#include <memory>

// Here is a structure to define the color value of a pixel:

struct RGBAlphaData {
  double red;   // red value,    from 0 to 1;
  double green; // green value,  from 0 to 1;
  double blue;  // blue value,   from 0 to 1;
  double alpha; // alpha value,  from 0 to 1
};



// This structure contains all the information which is interpolated.
struct InterpolationData {
  
  std::vector<double> gridPointsX;
  std::vector<double> gridPointsY;
  std::vector<std::vector<RGBAlphaData>> rgbAlphaValues;

};


class Interpolator {

 public:

  // The constructor is implemented
  Interpolator (const InterpolationData & data):data(data){};

  // The interpolate method is UNIMPLEMENTED:
  RGBAlphaData interpolate(double x, double y);

 private:

  // The Interpolator holds the interpolation data.
  InterpolationData data;

};
#endif

