#include <stdio.h>
#include <string.h>

#define BOOL int

typedef struct {
	int HufSz;
	int HufCode;
	int HufVal;
} HuffTable;


typedef struct {
	unsigned char precision;
	int Himg;
	int Wimg;
	unsigned char NbComponent;
	long SofTabPos;
} Sof;


typedef struct {
	unsigned char CompCount;
	unsigned char CompId;
	unsigned char CompDc;
	unsigned char SpectralSelStart;
	unsigned char SpectralSelEnd;
	unsigned char SuccessiveAp;
	unsigned char Sospttrans;
	long SofTabPos;
} Sos;


typedef struct  {
	unsigned long RestartInterval;
	BOOL MarkerFound;
	int MaxHuffVal;
	int MaxHuffSz;
	int* DataImg;
	unsigned char RawDHT[256];
	unsigned char ValCurByte;
	unsigned char PosCurBit;
	FILE* infp;
	unsigned char RawDHTstart[256];
	Sof lSof;
	Sos lSos;
	HuffTable lHuffTable[256];	
} ClbJpeg;


ClbJpeg*_IdDcmJpegRead (FILE *);
void 	_IdDcmJpegFree (ClbJpeg *);


//
	
	



