#include <iostream>
//#include "gdcm.h"
#include "gdcmHeader.h"
#include "gdcmFile.h"


	// ecriture d'un fichier DICOM à partir d'un dcmHeader correct.
	// et des pixels d'une autre image
	
	
int main(int argc, char* argv[])
{  
	string premier, deuxieme;
	char resultat[200];
	
	gdcmFile  *f1, *f2;
	//gdcmHeader *e1, *e2;
	void* imageData;
	int dataSize;
		
	if (argc < 3) {
		printf ("usage :\n testChangeEntete nomFichierPourEntete nomFichierPourDonnées \n");
		exit(0);	
	}

	premier = argv[1];
	f1 = new gdcmFile(premier);
	
	deuxieme = argv[2];
	f2 = new gdcmFile(deuxieme);
	
	//f1->PrintPubElVal();
	
	// On suppose que les champs DICOM du 2ieme fichier existent *effectivement*
	
        f1->ReplaceOrCreateByNumber( f2->GetPubElValByNumber(0x0028, 0x0008),
          0x0028, 0x0008);// nb Frames
	f1->ReplaceOrCreateByNumber( f2->GetPubElValByNumber(0x0028, 0x0010),
	  0x0028, 0x0010);// nbLig
	f1->ReplaceOrCreateByNumber( f2->GetPubElValByNumber(0x0028, 0x0011),
          0x0028, 0x0011);// nbCol
	
	// sans doute d'autres à mettre à jour...
	
	// TODO : une routine qui recoit une liste de couples (gr,el), et qui fasse le boulot.

				
	dataSize = f2->GetImageDataSize();
	printf ("dataSize %d\n",dataSize);
	imageData= f2->GetImageData();
	
	f1->SetImageData(imageData,dataSize);
	
	// ou, plus joli:
	//f1->SetImageData(f2->GetImageData(),f2->GetImageDataSize());	

	sprintf(resultat, "%s.vol", deuxieme.c_str());
	printf ("WriteDCM\n");
	f1->WriteDcmImplVR(resultat);		
}



