/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestInline.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/24 16:01:39 $
  Version:   $Revision: 1.6 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
// This test is expected to 'show' the actual effect on an 'inline' function.
// We exchange 2 numbers
// - with a macro : this is the quicker (any doubt ?)
// - with a function, passing the params by pointer
// - with a function, passing the params by reference (exactly same time)
// - with an inline function described in the main()
//                                                   absolutely NO effect ?!?
// - with a function, described in the .h
//                                                   absolutely NO effect ?!?
// - with an inline function, described in the .h
//                                                   absolutely NO effect ?!?
//
// Which CXX_FLAGS, LINKER_FLAGS, ...,  must we set to see the difference?

#include "gdcmUtil.h"
#include "gdcmDebug.h"
#include <iostream>

#include <time.h>
#ifdef CMAKE_HAVE_SYS_TIMES_H
#include <sys/times.h>
#else
#include <sys/timeb.h>
#endif

void        frswap (double &a, double &b);
void        fpswap (double *a, double *b);
inline void ifrswap(double &a, double &b);
inline void ifpswap(double *a, double *b);

uint16_t     passDirect(uint16_t a,  uint16_t b);
uint16_t     passRef(uint16_t &a, uint16_t &b);
uint16_t     passPtr(uint16_t *a, uint16_t *b);

uint32_t     passDirect32(uint32_t a,  uint32_t b);
uint32_t     passRef32(uint32_t &a, uint32_t &b);
uint32_t     passPtr32(uint32_t *a, uint32_t *b);

double     passDirect(double a,  double b);
double     passRef(double &a, double &b);
double     passPtr(double *a, double *b);

#define       \
mswap(a, b)   \
{             \
   tmp = a;   \
   a   = b;   \
   b   = tmp; \
}

void frswap(double &a, double &b)
{
   double tmp;
   tmp = a;
   a   = b;
   b   = tmp;

}

void fpswap(double *a, double *b)
{
   double tmp;
   tmp = *a;
   *a  = *b;
   *b  = tmp;

}

inline void ifpswap(double *a, double *b)
{
   double tmp;
   tmp = *a;
   *a  = *b;
   *b  = tmp;
}

inline void ifrswap(double &a, double &b)
{
   double tmp;
   tmp = a;
   a   = b;
   b   = tmp;
}


uint32_t passRef32(uint32_t &a, uint32_t &b)
{
   return a + b;
} 
uint32_t passPtr32(uint32_t *a, uint32_t *b)
{
   return *a + *b;
} 
uint32_t passDirect32(uint32_t a, uint32_t b)
{
   return a + b;
} 


uint16_t passRef(uint16_t &a, uint16_t &b)
{
   return a + b;
} 
uint16_t passPtr(uint16_t *a, uint16_t *b)
{
   return *a + *b;
} 
uint16_t passDirect(uint16_t a, uint16_t b)
{
   return a + b;
} 

double passRefDouble(double &a, double &b)
{
   return a + b;
} 
double passPtrDouble(double *a, double *b)
{
   return *a + *b;
} 
double passDirectDouble(double a, double b)
{
   return a + b;
} 

int TestInline(int argc, char *argv[])
{

   unsigned int nbLoop; 
   unsigned int i;
      
   if (argc > 1)
      nbLoop = atoi(argv[1]);
   else
      nbLoop = 100000000;      

   clock_t r1, r2;
   struct tms tms1, tms2;
   
   double a = 1, b = 2;
   double tmp;
   
   uint16_t x=1, y=2;    
 // ----------------------------------------
 
   std::cout << "Use a macro "<< std::endl;
   r1 = times(&tms1);   
   for(i = 0 ; i< nbLoop ; i++)
   {
      mswap (a,b);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl;
   
 // ----------------------------------------
 
   std::cout << "Use reference function" << std::endl;
   r1 = times(&tms1);         
   for(i = 0 ; i< nbLoop ; i++)
   {
      frswap (a,b);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl; 
   
 // ----------------------------------------
  
   std::cout << "Use pointer function" << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      fpswap (&a, &b);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl;  
   
 // ----------------------------------------
 
   std::cout << "Use inline, main-defined reference function" << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      ifrswap (a, b);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl;    
   
 // ----------------------------------------
 
   std::cout << "Use inline, main-defined pointer function" << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      ifpswap (&a, &b);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl;

 // ----------------------------------------
  
//To check the 2 following cases, we just put the two 'static' functions
//hifpswap and  hNoifpswap in gdcmUtil.h
    
   std::cout << "Use inline, .h defined, WITH inline keyword pointer function"
             << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      gdcm::Util::hifpswap (&a, &b);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl;  

   
 // ----------------------------------------

   std::cout << "Use inline, .h defined, NO inline keyword pointer function"
             << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      gdcm::Util::hNoifpswap (&a, &b);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl; 


 // ----------------------------------------
 
   std::cout << "Pass uint_16 param directly"
             << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      passDirect (x, y);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl; 

 // ----------------------------------------
 
   std::cout << "Pass uint_16 param as ref"
             << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      passRef (x, y);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl; 

 // ----------------------------------------
 
   std::cout << "Pass uint_16 param as ptr"
             << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      passPtr (&x, &y);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl; 



 // ----------------------------------------

   uint32_t m =1, n=2; 
   std::cout << "Pass uint_32 param directly"
             << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      passDirect32 (m, n);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl; 

 // ----------------------------------------
 
   std::cout << "Pass uint32_t param as ref"
             << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      passRef32 (m, n);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl; 

 // ----------------------------------------
 
   std::cout << "Pass uint_32 param as ptr"
             << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      passPtr32 (&m, &n);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl; 

 // ----------------------------------------
 
 
   double dx=1.0, dy=2.0;
   
   std::cout << "Pass double param directly"
             << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      passDirectDouble (dx, dy);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl; 

 // ----------------------------------------
 
   std::cout << "Pass double param as ref"
             << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      passRefDouble (dx, dy);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl; 

 // ----------------------------------------
 
   std::cout << "Pass double param as ptr"
             << std::endl;
   r1 = times(&tms1);     
   for(i = 0 ; i< nbLoop ; i++)
   {
      passPtrDouble (&dx, &dy);  
   }
   r2 = times(&tms2);
   std::cout 
        << (long) ((tms2.tms_utime)  - (tms1.tms_utime)) 
        << std::endl; 

   return 0;
}
