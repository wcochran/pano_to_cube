## PANORAMA TO CUBE

Simple application that takes a 360 degree panorama image
and converts it to an equirectangular image suitable for
some VR application.

## BUILD AND RUN

The input and output images need to by in PPM (Portable Pixel Map) format.
Assuming you have ImageMagick convert's utility you can build the C++ application
and construct a JPEG image as follows:

``` bash 
$ make
$ convert pano.jpg pano.ppm 
$ ./pano2cube pano.ppm cube.ppm
$ convert cube.ppm cube.jpg
```

## MORE INFO

Read the included PDF for the mildly gory details.

## WHAT IS INCLUDED

```
README.md ............. This File
LICENSE.md ............ MIT License
Makefile .............. Build c++ app
PPM.h ................. PPM image I/O header
PPM.cpp ............... PPM image I/O source
pano.jpg .............. Example panorama image
pano2cube.cpp ......... Main application source
pano_to_cube.pdf ...... Documentation
pano_to_cube.tex ...... Documentation LaTeX source
cube_cylinder.graffle . Source for figure in docs.
cube_cylinder.pdf ..... Figure included in docs.
```

## LICENSE

The MIT License (MIT). See included License.
