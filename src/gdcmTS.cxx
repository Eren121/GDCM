// gdcmTS.cxx

#include <stdio.h>

#include "gdcmTS.h"

gdcmTS::gdcmTS(void) {
   ts["1.2.840.10008.1.2"]      = "Implicit VR - Little Endian";  
   ts["1.2.840.10008.1.2.1"]    = "Explicit VR - Little Endian";         
   ts["1.2.840.10008.1.2.1.99"] = "Deflated Explicit VR - Little Endian";          
   ts["1.2.840.10008.1.2.2"]    = "Explicit VR - Big Endian";                 
   ts["1.2.840.10008.1.2.4.50"] = "JPEG Baseline (Process 1)";       
   ts["1.2.840.10008.1.2.4.51"] = "JPEG Extended (Process 2 & 4)";            
   ts["1.2.840.10008.1.2.4.52"] = "JPEG Extended (Process 3 & 5)"; 
   ts["1.2.840.10008.1.2.4.53"] = "JPEG Spectral Selection, Non-Hierarchical (Process 6 & 8)"; 
   ts["1.2.840.10008.1.2.4.54"] = "JPEG Spectral Selection, Non-Hierarchical (Process 7 & 9)";        
   ts["1.2.840.10008.1.2.4.55"] = "JPEG Spectral Selection, Non-Hierarchical (Process 10 & 12)";           
   ts["1.2.840.10008.1.2.4.56"] = "JPEG Spectral Selection, Non-Hierarchical (Process 11 & 13)";             
   ts["1.2.840.10008.1.2.4.57"] = "JPEG Baseline (Process 14)";     
   ts["1.2.840.10008.1.2.4.58"] = "JPEG Baseline (Process 15)";     
   ts["1.2.840.10008.1.2.4.59"] = "JPEG Spectral Selection, Non-Hierarchical (Process 16 & 18)";           
   ts["1.2.840.10008.1.2.4.60"] = "JPEG Spectral Selection, Non-Hierarchical (Process 17 & 19)";          
   ts["1.2.840.10008.1.2.4.61"] = "JPEG Spectral Selection, Non-Hierarchical (Process 20 & 22)";           
   ts["1.2.840.10008.1.2.4.62"] = "JPEG Spectral Selection, Non-Hierarchical (Process 21 & 23)";          
   ts["1.2.840.10008.1.2.4.63"] = "JPEG Spectral Selection, Non-Hierarchical (Process 24 & 26)";   
   ts["1.2.840.10008.1.2.4.64"] = "JPEG Spectral Selection, Non-Hierarchical (Process 25 & 27)";   
   ts["1.2.840.10008.1.2.4.65"] = "JPEG Lossless, Hierarchical (Process 28)";   
   ts["1.2.840.10008.1.2.4.66"] = "JPEG Lossless, Hierarchical (Process 29)";   
   ts["1.2.840.10008.1.2.4.70"] = "Non-Hierarchical, First-Order Prediction (Process 14 [Selection Value 1])"; 
   ts["1.2.840.10008.1.2.5"]    = "RLE Lossless";              
}

gdcmTS::~gdcmTS() {
   ts.clear();
}

int gdcmTS::Count(TSKey key) {
   return ts.count(key);
}

std::string gdcmTS::GetValue(TSKey key) {
   return ts[key];
}
