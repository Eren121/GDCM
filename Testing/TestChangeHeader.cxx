#include <iostream>
//#include "gdcm.h"
#include "gdcmHeader.h"
#include "gdcmFile.h"
#include <string>


	// ecriture d'un fichier DICOM à partir d'un dcmHeader correct.
	// et des pixels d'une autre image
	
	
int testChangeEntete(int argc, char* argv[]) {  
	std::string premier, deuxieme;
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
	
	std::string nbFrames = f2->GetHeader()->GetEntryByNumber(0x0028, 0x0008);
	if(nbFrames != "gdcm::Unfound") {
           f1->GetHeader()->ReplaceOrCreateByNumber( nbFrames, 0x0028, 0x0008);
        }
         
	f1->GetHeader()->ReplaceOrCreateByNumber(f2->GetHeader()->GetEntryByNumber(0x0028, 0x0010),
	  0x0028, 0x0010);// nbLig
	f1->GetHeader()->ReplaceOrCreateByNumber( f2->GetHeader()->GetEntryByNumber(0x0028, 0x0011),
          0x0028, 0x0011);// nbCol
	
	
	// sans doute d'autres à mettre à jour...
	
	// TODO : rajouter une valeur par defaut.	
	// TODO : une routine qui recoit une liste de couples (gr,el), 
	//                        et qui fasse le boulot.

				
	dataSize = f2->GetImageDataSize();
	printf ("dataSize %d\n",dataSize);
	imageData= f2->GetImageData();
			
	// TODO : ne devrait-on pas fusionner ces 2 fonctions ?
	f1->SetImageData(imageData,dataSize);
	f1->GetHeader()->SetImageDataSize(dataSize);
	
	f1->GetHeader()->PrintEntry();
	
	std::string s0 =f2->GetHeader()->GetEntryByNumber(0x7fe0, 0x0000);
	std::string s10=f2->GetHeader()->GetEntryByNumber(0x7fe0, 0x0010);
	printf("lgr 7fe0, 0000 %s\n",s0.c_str());
	printf("lgr 7fe0, 0010 %s\n",s10.c_str());	

	sprintf(resultat, "%s.vol", deuxieme.c_str());
	printf ("WriteDCM\n");
	//f1->WriteDcmImplVR(resultat);	
	f1->WriteAcr(resultat);	

  return 0;
}



