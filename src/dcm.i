%module dcm
%{
#include "dcmlib.h"
%}
%rename(new_dcmHeader_c) dcmHeader::dcmHeader(char*);

%include dcmlib.h
