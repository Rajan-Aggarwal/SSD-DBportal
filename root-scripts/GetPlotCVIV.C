/*
    ./GetPlotCVIV "3D-7781-DET-5-Pad_u_25x50" "cv" "15-5-2017::17:43:4"
    
    Or:
    root -l -q -b "GetPlotCVIV.C(\"3D-7781-DET-5-Pad_u_25x50\" ,\"cv\",\"15-5-2017::17:43:4\")"

*/

#include <iostream>   
#include <string> 
#include <unistd.h>

#include "TIV.h"
#include "TCV.h"
#include "TString.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TDatime.h"
#include "TCanvas.h"
#include "TError.h"

using namespace std;

void GetPlotCVIV( string detid , string meastype , string date ) {

   //Do not show warnings, just errors
   gErrorIgnoreLevel = kError;

   //gSystem->Load("TIV_cpp.so");   
   //gSystem->Load("TCV_cpp.so");   

   //Convert input to TStrings
   TString DetID    = TString( detid );
   TString MeasType = TString( meastype );
   TString Date     = TString( date );
   
   //Parse the date with the format 05-11-2011::14:22:43
   TObjArray *td = Date.Tokenize("-");
   TString tDay=TString(((TObjString*) td->At(0))->String());
   TString tMonth=TString(((TObjString*) td->At(1))->String());
   
   TString reminder=TString(((TObjString*) td->At(2))->String()); //2011::13:14:43
   td = reminder.Tokenize("::");
   TString tYear=TString(((TObjString*) td->At(0))->String());  
   TString tHour=TString(((TObjString*) td->At(1))->String());
   TString tMin = TString(((TObjString*) td->At(2))->String());
   TString tSec = TString(((TObjString*) td->At(3))->String());
   
   //Build the date we request
   TDatime InDate;
   InDate.Set(tYear.Atoi(), tMonth.Atoi() , tDay.Atoi(), tHour.Atoi(), tMin.Atoi(), tSec.Atoi());
      
   //Open existing ROOT file and Get the tree
   TFile *file0 = TFile::Open("CVIV.root") ;
   TTree *tree = (TTree *) file0->Get("cviv") ;
   
   //Build the right selection string
   TString whatt, whatp , sel , Xtit , Ytit  ;
   if ( MeasType.Contains("iv")) {
     whatt =  TString("iv->Itot:iv->Vbias") ;
     whatp =  TString("iv->Ipad:iv->Vbias") ;
     sel  = TString("Type==1 && iv->sensor.Contains(\"") + DetID.Data()+ TString("\") && ") + Form("iv->utc.Convert()==%d",InDate.Convert()) ;
     Xtit = TString( "Bias voltage [V]" ) ;
   }
   if ( MeasType.Contains("cv")) {
     whatt =  TString("cv->iC2p:cv->Vsense") ;  Xtit = TString("Sensed voltage [V]") ;
     sel  = TString("Type==2 && cv->sensor.Contains(\"") + DetID.Data()+ TString("\") && ") + Form("cv->utc.Convert()==%d",InDate.Convert()) ;
     Ytit = TString("1/Cp^{2} [pF-2]");
   }
   
   TCanvas *c1 = new TCanvas();
   tree->Draw( whatt , sel , "l" )   ;
   if ( MeasType.Contains("iv")) {
    tree->Draw( whatp , sel , "lsame" )   ;
   }
   //htemp->GetXaxis()->SetTitle( Xtit ) ;   htemp->GetYaxis()->SetTitle( Ytit ) ;
   std::string name;
   name = getlogin();
   TString pdfnm;
   if (name.compare("root") == 0) {
      pdfnm = TString("/home/ubuntu/ssd-dbportal/tmp/pdfs/") + DetID + MeasType + Date + ".pdf" ;
   } else {
      pdfnm = TString("/home/") + TString(name) + TString("/ssd-dbportal/tmp/pdfs/") + DetID + MeasType + Date + ".pdf" ;
   }
   c1->Print( pdfnm );
}

int main( int argc , char *argv[] ) {

   GetPlotCVIV( string(argv[1]) , string(argv[2]) , string(argv[3]) ) ;
   return 0; 
}
