/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestTS.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/11 00:37:41 $
  Version:   $Revision: 1.4 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmTS.h"

int TestTS(int , char *[])
{
   gdcm::TS ts;
   // There should be 150 entries
   ts.Print( std::cout );

   // Implicit VR Little Endian
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2" ) << std::endl;
   // Implicit VR Big Endian DLX (G.E Private)
   std::cout << ts.IsTransferSyntax( "1.2.840.113619.5.2" ) << std::endl;
   // Explicit VR Little Endian
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.1" ) << std::endl;
   // Deflated Explicit VR Little Endian
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.1.99" ) << std::endl;
   // Explicit VR Big Endian
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.2" ) << std::endl;
   // JPEG Baseline (Process 1)
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.4.50" ) << std::endl;
   // JPEG Extended (Process 2 & 4)
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.4.51" ) << std::endl;
   // JPEG Extended (Process 3 & 5)
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.4.52" ) << std::endl;
   // JPEG Spectral Selection, Non-Hierarchical (Process 6 & 8)
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.4.53" ) << std::endl;
   // JPEG Full Progression, Non-Hierarchical (Process 10 & 12)
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.4.55" ) << std::endl;
   // JPEG Lossless, Non-Hierarchical (Process 14)
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.4.57" ) << std::endl;
   // JPEG Lossless, Hierarchical, First-Order Prediction (Process 14, [Selection Value 1])
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.4.70" ) << std::endl;
   // JPEG 2000 Lossless
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.4.90" ) << std::endl;
   // JPEG 2000
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.4.91" ) << std::endl;
   // RLE Lossless
   std::cout << ts.IsTransferSyntax( "1.2.840.10008.1.2.5" ) << std::endl;
   // Unknown
   std::cout << ts.IsTransferSyntax( "Unknown Transfer Syntax" ) << std::endl;

   return ts.GetValue( "" ) != gdcm::GDCM_UNFOUND;
}
