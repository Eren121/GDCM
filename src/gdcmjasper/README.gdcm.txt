This directory contains a subset of the JasPer project.
The JasPer Project is an open-source initiative to provide a free software-based
reference implementation of the codec specified in the JPEG-2000 Part-1 standard
(i.e., ISO/IEC 15444-1). This project was started as a collaborative effort
between Image Power, Inc. and the University of British Columbia. Presently, the
ongoing maintenance and development of the JasPer software is being coordinated
by its principal author, Michael Adams, who is affiliated with the Digital
Signal Processing Group (DSPG) and Department of Electrical and Computer
Engineering at the University of Victoria.

We only include enough of distribution to build libjasper.  We do not
include the subdirs acaux,data and doc. We do not include either the standard 
executables that come with JasPer (imgcmp, imginfo, jiv). Furthermore, the standard 
libjasper build process is replaced with a CMake build process.

We'd like to thank the JasPer project and Image Power for releasing an open 
source implementation of the JPEG2000 codec.

Modifications
-------------

- modification were made so that compilation with gcc -Wall flags passes without warnings
- remove all explicit tabs and replace by proper amount of spaces
- remove all autotools/configure scripts
