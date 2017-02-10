CXX=g++
CXXFLAGS=-Wall
ALL=testppm

all: $(ALL)

JUNK=*.o *~ *.dSYM pano.ppm

clean:
	-rm -rf $(JUNK)

clobber:
	-rm -rf $(JUNK) $(ALL)

testppm: PPM.cpp PPM.h pano.jpg
	$(CXX) $(CXXFLAGS) -DTEST_PPM $< -o $@
	convert pano.jpg pano.ppm
	./testppm
	convert crop.ppm crop.jpg

