/*
 This function takes a sensor name and a type of measurement and gives back an array of strings
 containing the dates these measurements were taken.
 
 Usage example:
   
   root -l
   .L GetListOfDates.C
   GetListOfDates("3D-7781-DET-5-Pad_u_25x50","cv")
   
*/   

#include "TIV.h"
#include "TCV.h"

void GetListOfDates( string detid , string meastype ) {

   gSystem->Load("TIV_cpp.so");   
   gSystem->Load("TCV_cpp.so");   

   //Convert input to TStrings
   TString DetID    = TString( detid );
   TString MeasType = TString( meastype );

   //Open existing ROOT file and Get the tree
   TFile *file0 = TFile::Open("CVIV.root") ;
   TTree *tree = (TTree *) file0->Get("cviv") ;
   
   //Build the right selection string
   TString what, sel ;
   if ( MeasType.Contains("iv")) {
     sel  = TString("iv->sensor.Contains(\"") + DetID.Data()+ TString("\") && Type==1") ;
   }
   if ( MeasType.Contains("cv")) {
     sel  = TString("cv->sensor.Contains(\"") + DetID.Data()+ TString("\") && Type==2") ;
   }
   
   //Get the objects of type IV and CV
   TIV *oiv=0;
   TBranch *iv = (TBranch*) tree->GetBranch("iv");   
   iv->SetAddress(&oiv) ;
   
   TCV *ocv=0;
   TBranch *cv = (TBranch*) tree->GetBranch("cv");   
   cv->SetAddress(&ocv) ;

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
     
     out = ( MeasType.Contains("iv")) ?  Form("%d-%d-%d::%d:%d:%d",oiv->utc.GetDay(),oiv->utc.GetMonth(),oiv->utc.GetYear(),oiv->utc.GetHour(),oiv->utc.GetMinute(),oiv->utc.GetSecond()) :
     					 Form("%d-%d-%d::%d:%d:%d",ocv->utc.GetDay(),ocv->utc.GetMonth(),ocv->utc.GetYear(),ocv->utc.GetHour(),ocv->utc.GetMinute(),ocv->utc.GetSecond()) ;
     cout<<  out <<endl;
   }
   
   tree->SetEventList(0);
   
   
}
