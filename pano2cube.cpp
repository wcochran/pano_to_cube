#include <iostream>
#include <cmath>
#include "PPM.h"

void cylindricalPanoramaToCube(const PPM& inImage, PPM& outImage) {
  for (int row = 0; row < outImage.H; row++) {
    for (int col = 0; col < outImage.W; col++) {
      const double s = 4.0*col/outImage.W;
      const int face = static_cast<int>(floorf(s));
      const double f = s - face;
      float u,v,w;
      switch(face) {
      case 0:
	u = 1;
	v = 2*f - 1;
	break;
      case 1:
	u = 1 - 2*f;
	v = 1;
	break;
      case 2:
	u = -1;
	v = 1 - 2*f;
	break;
      default:
	u = 2*f - 1;
	v = -1;
      }
      w = 2.0*row/outImage.H - 1;
      const float a = 1/sqrtf(u*u + v*v);
      const float x = a*u;
      const float y = a*v;
      const float z = a*w;
      float theta = atan2(y,x) + M_PI/4;
      if (theta < 0) theta += 2*M_PI;
      const float r = inImage.H*(z + 1)/2.0;
      const float c = inImage.W*theta/(2*M_PI);
      float inpix[3];
      inImage.bilerp(r,c,inpix);
      unsigned char *outpix = outImage(row,col);
      for (int i = 0; i < 3; i++)
	outpix[i] = static_cast<unsigned char>(inpix[i] + 0.5);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << " pano.ppm cube.ppm" << std::endl;
    exit(1);
  }

  try {
    PPM inImage(argv[1]);
    PPM outImage(inImage.W, inImage.H);
    cylindricalPanoramaToCube(inImage, outImage);
    outImage.write(argv[2]);
  } catch (BogusPPM &e) {
    std::cerr << e.what() << std::endl;
    exit(2);
  }

  return 0;
}
