/*
 This function takes a sensor name and a type of measurement and gives back an array of strings
 containing the dates these measurements were taken.
 
 Usage example:
   
   ./GetListOfFiles "3D-7781-DET-5-Pad_u_25x50" "red_top_tct"
   
*/   

#include <iostream>   
#include <string> 

#include "TString.h"
#include "TSystem.h"
#include "TTree.h"
#include "TBranch.h"
#include "TEntryList.h"
#include "TEntryListArray.h"
#include "TError.h"
#include "TCTSummary.h"


using namespace std;

void GetListOfFiles( string detid , string meastype ) {


   //Do not show warnings, just errors
   gErrorIgnoreLevel = kError;

   //Convert input to TStrings
   TString DetID    = TString( detid );
   TString MeasType = TString( meastype );

   //Open existing ROOT file and Get the tree
   TFile *file0 = TFile::Open("TCTDB.root") ;
   TTree *tree = (TTree *) file0->Get("summary") ;
   
   //Build the right selection string
   TString what="fnm" , sel ;
   if ( MeasType.EqualTo("tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\")") ;
   }
   if ( MeasType.EqualTo("red_top_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && Illum==1 && Lambda==660 && Nav>1") ;
   }
   if ( MeasType.EqualTo("red_bottom_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && Illum==-1 && Lambda==660 && Nav>1") ;
   }
   if ( MeasType.EqualTo("ir_top_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && Illum==1 && Lambda==1064 && Nav>1") ;
   }
   if ( MeasType.EqualTo("ir_bottom_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && Illum==-1 && Lambda==1064 && Nav>1") ;
   }
   if ( MeasType.EqualTo("edge_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && Illum==0 && Lambda==1064 && Nav>1") ;
   }
   if ( MeasType.EqualTo("tpa_tct")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && (Setup==6 ||Setup==9) && Nav>1") ;
   }
   if ( MeasType.EqualTo("timing")) {
     sel  = TString("sensor.Contains(\"") + DetID.Data()+ TString("\") && (Setup==8 || Nav==1)") ;
   }
   
   //Get the objects of type IV and CV
   TCTSummary *osumm=0;
   TBranch *bsumm = (TBranch*) tree->GetBranch("summ");   
   bsumm->SetAddress(&osumm) ;
   

   //Create a list with the events that satisfied the cut
   tree->Draw( ">>myList",sel,"entrylistarray");
   TEntryListArray *mylist=(TEntryListArray*)gDirectory->Get("myList") ;
   tree->SetEntryList( mylist ) ; //Use tree->SetEventList(0) to switch off
   
   Int_t  nev  = mylist->GetN() ; 
   if (nev==0) exit(-1) ;
   
   TString out;  
   for ( Int_t ii=0 ; ii < nev ; ii++ ) {

     Int_t iev = mylist->GetEntry(ii) ; 
     tree->GetEntry( iev );    
     
     out = osumm->fnm ;
     char* basec   = strdup(out.Data()) ; 
     char *fname   = basename(basec);
     TString sname = TString( fname );
     Int_t idot    = sname.First('.') ;
     cout<<  sname(0,idot) <<endl;
   }
   
   tree->SetEventList(0);
   
   
}

int main( int argc , char *argv[] ) {
   
   GetListOfFiles( string(argv[1]) , string(argv[2]) ) ;
}
