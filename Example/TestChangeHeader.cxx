#include "gdcmHeader.h"
#include "gdcmFile.h"

// This examples read two images (could be the same). Try to modify
// Acquisition Matrix and then write the image again

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
       std::cerr << "usage :" << std::endl <<
         argv[0] << " nomFichierPourEntete nomFichierPourDonnées" << 
std::endl;
       return 1;
    }

    gdcmHeader *h1 = new gdcmHeader( argv[1] );
    gdcmFile  *f1 = new gdcmFile( h1 );
    gdcmFile  *f2 = new gdcmFile( argv[2] );

    // 0018 1310 US ACQ Acquisition Matrix
    gdcmDictEntry *dictEntry =
         f2->GetHeader()->GetPubDict()->GetDictEntryByName( "Acquisition Matrix" );
    std::cerr << std::hex << dictEntry->GetGroup() << "," << dictEntry->GetElement() << std::endl;

   // std::string matrix = f2->GetHeader()->GetEntryByNumber(0x0018, 0x1310);
   // Or, strictly equivalent (a little bit longer at run-time !):
    std::string matrix = f2->GetHeader()->GetEntryByName("Acquisition Matrix");  
    if(matrix != "gdcm::Unfound")
    {
       std::cerr << "Aquisition Matrix:" << matrix << std::endl;
      f1->GetHeader()->ReplaceOrCreateByNumber( matrix, 0x0018, 0x1310);
      
       //f1->GetHeader()->ReplaceOrCreateByNumber( matrix, dictEntry->GetGroup(),
       //  dictEntry->GetElement());
    }

    f1->GetImageData();
    
    h1->Print();
    
    f1->WriteDcmExplVR("output-matrix.dcm");

    return 0;
}


