//   ./GetNerOfMeasurement "3D-7781-DET-5-Pad_u_25x50" "tct"
#include "TString.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TError.h"

#include <iostream>

using namespace std;

void GetNerOfMeasurement_TCT( string detid , string meastype ) {
/*
  tct = gives total number of TCT measurements
  red_{top,bottom}_tct	
  ir_{top,bottom}_tct	
  edge_tct		
  tpa_tct			

*/   
   //gSystem->Load("~/apps/TRICS/TCTSummary_cpp.so");   

   gErrorIgnoreLevel = kError;
   
   //Convert input to TStrings
   TString MeasType = TString( meastype );
   TString DetID    = TString( detid );

   //Open existing ROOT file and Get the tree
   TFile *file0 = TFile::Open("TCTDB.root") ;
   TTree *tree = (TTree *) file0->Get("summary") ;
   
   //Build the right selection string
   TString what, sel="event==-1" ;
   what = TString("event") ;
   if ( MeasType.EqualTo("tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\")") ;
   }
   if ( MeasType.EqualTo("red_top_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && Illum==1 && Lambda==660") ;
   }
   if ( MeasType.EqualTo("red_bottom_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && Illum==-1 && Lambda==660") ;
   }
   if ( MeasType.EqualTo("ir_top_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && Illum==1 && Lambda==1064") ;
   }
   if ( MeasType.EqualTo("ir_bottom_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && Illum==-1 && Lambda==1064") ;
   }
   if ( MeasType.EqualTo("edge_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && Illum==0 && Lambda==1064") ;
   }
   if ( MeasType.EqualTo("tpa_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && (Setup==6 ||Setup==9)") ;
   }
   
   //Get Ner of Measurements of type meastype
   int Ner = tree->Draw( what , sel ,"goff");
   
   cout << Ner << endl;
   //return Ner ;
   
}

int main (int argc , char *argv[] ) {

   GetNerOfMeasurement_TCT( string(argv[1]) , string(argv[2]) ) ;
   
}
