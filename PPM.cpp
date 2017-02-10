#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <errno.h>
#include <string>
#include <iostream>
#include <sstream>

#include "PPM.h"

PPM::PPM(const std::string& fname) {
  FILE *f = fopen(fname.c_str(), "rb");
  if (f == nullptr) {
    std::stringstream ss;
    ss << fname << ":" << strerror(errno);
    throw BogusPPM(ss.str());
  }
  static char buf[200];
  if (fgets(buf, sizeof(buf), f) == 0 || strncmp("P6",buf,2) != 0) {
    std::stringstream ss;
    ss << fname << ": not a raw PPM image!";
    fclose(f);
    throw BogusPPM(ss.str());
  }
  while (fgets(buf, sizeof(buf), f) != 0 && buf[0] == '#')
    ;
  if (sscanf(buf, "%d %d", &W, &H) != 2 || W <= 0 || H <= 0) {
    std::stringstream ss;
    ss << fname << ": has a bogus size!";
    fclose(f);
    throw BogusPPM(ss.str());
  }
  int maxval;
  while (fgets(buf, sizeof(buf), f) != 0 && buf[0] == '#')
    ;
  if (sscanf(buf, "%d", &maxval) != 1 || maxval != 255) {
    std::stringstream ss;
    ss << fname << ": max pixel value != 255!";
    fclose(f);
    throw BogusPPM(ss.str());
  }
  const unsigned numPixels = W*H;
  pixels.resize(3*numPixels);
  if (fread(&pixels[0], 3, numPixels, f) != numPixels) {
    std::stringstream ss;
    if (ferror(f))
      ss << fname << ":" << strerror(errno);
    else if (feof(f))
      ss << fname << ": unexpected EOF!";
    else
      ss << fname << ": unable to read pixels";
    fclose(f);
    throw BogusPPM(ss.str());
  }
  fclose(f);
}

void PPM::write(const std::string& fname) { // throws BogusPPM
  FILE *f = fopen(fname.c_str(), "wb");
  if (f == nullptr) {
    std::stringstream ss;
    ss << fname << ":" << strerror(errno);
    throw BogusPPM(ss.str());
  }
  fprintf(f, "P6\n%d %d\n255\n", W, H);
  if (fwrite(&pixels[0], 3, W*H, f) != W*H) {
    std::stringstream ss;
    if (ferror(f))
      ss << fname << ":" << strerror(errno);
    else if (feof(f))
      ss << fname << ": unexpected EOF!";
    else
      ss << fname << ": unable to write pixels";
    fclose(f);
    throw BogusPPM(ss.str());
  }
  fclose(f);
}

void PPM::bilerp(float r, float c, float pix[3]) const {
  if (r < 0 || r >= H || c < 0 || c >= W) { // clamp to black
    pix[0] = pix[1] = pix[2] = 0;
  } else {
    const int r0 = static_cast<int>(floorf(r));
    const int c0 = static_cast<int>(floorf(c));
    const int r1 = (r0 < H-1) ? (r0 + 1) : r0;
    const int c1 = (c0 < W-1) ? (c0 + 1) : c0;
    const unsigned char *pix00 = this->operator()(r0,c0);
    const unsigned char *pix01 = this->operator()(r0,c1);
    const unsigned char *pix10 = this->operator()(r1,c0);
    const unsigned char *pix11 = this->operator()(r1,c1);
    const float s = c - c0;
    const float t = r - r0;
    float pix0[3], pix1[3];
    for (int i = 0; i < 3; i++) {
      pix0[i] = (1-s)*pix00[i] + s*pix01[i];
      pix1[i] = (1-s)*pix10[i] + s*pix11[i];
    }
    for (int i = 0; i < 3; i++)
      pix[i] = (1-t)*pix0[i] + t*pix1[i];
  }
}

#ifdef TEST_PPM

void rotate(float theta, const PPM& source, PPM& target) {
  const float c = cosf(theta);
  const float s = sinf(theta);
  const float cx = 0.5*source.W;
  const float cy = 0.5*source.H;
  const float dx = 0.5*target.W;
  const float dy = 0.5*target.H;
  for (int y = 0; y < target.H; y++) {
    const float v = y - dy;
    for (int x = 0; x < target.W; x++) {
      const float u = x - dx;
      const float xx = c*u - s*v + cx;
      const float yy = s*u + c*v + cy;
      float spix[3];
      source.bilerp(yy, xx, spix);
      unsigned char *tpix = target(y,x);
      for (int i = 0; i < 3; i++)
	tpix[i] = static_cast<unsigned char>(spix[i] + 0.5);
    }
  }
}

int main() {
  try {
    PPM pano("pano.ppm");
    PPM crop(400, 600);
    const int r0 = 1300, c0 = 1200;
    for (int r = 0; r < crop.H; r++)
      for (int c = 0; c < crop.W; c++) {
	const unsigned char *inpix = pano(r0 + r, c0 + c);
	unsigned char* outpix = crop(r,c);
	outpix[0] = inpix[0]; // std::copy?
	outpix[1] = inpix[1];
	outpix[2] = inpix[2];
      }
    crop.write("crop.ppm");
    PPM rot(600, 700);
    rotate(M_PI/4, crop, rot);
    rot.write("rot.ppm");
  } catch(BogusPPM& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}

#endif // TEST_PPM
