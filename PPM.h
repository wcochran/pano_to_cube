#ifndef PPM_H
#define PPM_H

#include <string>
#include <cstring>
#include <cassert>
#include <vector>

class BogusPPM : public std::logic_error {
public:
  explicit BogusPPM(const std::string& w) : std::logic_error(w) {}
  explicit BogusPPM(const char *w) : std::logic_error(w) {}
};

struct PPM {
  int W, H;
  std::vector<unsigned char> pixels;
  PPM(int width, int height) : W(width), H(height), pixels(3*W*H) {
    std::fill(pixels.begin(), pixels.end(), 0);
  }
  PPM(const std::string& fname); // throws BogusPPM
  const unsigned char *operator()(int r, int c) const { // read
    return &pixels[3*(r*W + c)];
  }
  unsigned char *operator()(int r, int c) { // write
    return &pixels[3*(r*W + c)];
  }
  void write(const std::string& fname); // throws BogusPPM
  // XXX void bilerp(float r, float c, float pix[3]) const;
};

#endif // PPM_H
