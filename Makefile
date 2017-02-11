CXX=g++
CXXFLAGS=-Wall
ALL=pano2cube

all: $(ALL)

JUNK=*.o *~ *.dSYM pano.ppm crop.* rot.* test.ppm cube.*

clean:
	-rm -rf $(JUNK)

clobber:
	-rm -rf $(JUNK) $(ALL)

testppm: PPM.cpp PPM.h pano.jpg
	$(CXX) $(CXXFLAGS) -DTEST_PPM $< -o $@
	convert pano.jpg pano.ppm
	./testppm
	convert crop.ppm crop.jpg
	convert rot.ppm rot.jpg

.c.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@

PPM.o: PPM.cpp PPM.h
pano2cube.o: pano2cube.cpp PPM.h

pano2cube: pano2cube.o PPM.o
	$(CXX) $(CXXFLAGS) $^ -o $@
