%module gdcm
%{
#include "gdcm.h"
%}
%rename(new_dcmHeader_c) dcmHeader::dcmHeader(char*);

%include gdcm.h
