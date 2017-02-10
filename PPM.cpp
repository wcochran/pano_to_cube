#include <cstdio>
#include <cstdlib>
#include <cstring>
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

#ifdef TEST_PPM

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
  } catch(BogusPPM& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}

#endif // TEST_PPM
