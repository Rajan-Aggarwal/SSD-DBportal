/*
    ./GetPlot_TCT <filename>
*/

#include <iostream>   
#include <string> 
#include <unistd.h>

#include "TWaveform.h"
#include "TMeas.h"
#include "TMeasHeader.h"
#include "TString.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TDatime.h"
#include "TCanvas.h"
#include "TError.h"

#define OUTDIR "/home/ubuntu/ssd-dbportal/tmp/pdfs/"

using namespace std;
extern Int_t  plot1D( TString varexp , TString selection, TString fnm , TString OutDir ) ;
extern Int_t  plot2D( TString varexp , TString selection, TString fnm , TString OutDir ) ;

void GetPlot_TCT( TString fnm ) {

   fnm = "/home/ubuntu/mounts/TCT_DB/" + fnm + ".txt.root" ;
   
   TString OutDir = OUTDIR ;

   //Do not show warnings, just errors
   gErrorIgnoreLevel = kError;

   //Convert input to TStrings
   TString fname    = TString( fnm );
   
   //Open input file
   TFile *f = new TFile( fnm );
   if (f->GetNkeys()==0)  return  ;
   if ( !f->IsOpen() ) return  ;
   TTree *tree = (TTree*) f->Get("edge");  
   tree->SetDirectory(f);

   //Get objects
   TMeas   *em   = 0 ;
   TBranch *raw  = tree->GetBranch("raw") ;
   raw->SetAddress(&em) ;
   raw->GetEntry(0) ;  

   TWaveform *wv = new TWaveform( ) ;     
   TBranch *proc = tree->GetBranch("proc") ;
   proc->SetAddress(&wv) ;

   TMeasHeader *emh = (TMeasHeader *) tree->GetUserInfo()->At(0) ;
   tree->GetEntry(0) ; 

   Int_t NV = emh->NV ;
   Int_t Nx = emh->Nx ;
   Int_t Ny = emh->Ny ;
   Int_t Nz = emh->Nz ;
   
   TString what , sel=TString("notset") ;
   if ( (Nx==1 && Ny==1 && Nz==1) && NV>1 ) {
     Long_t nentries = tree->GetEntries() ; 
     if (nentries<40) {
       what="volt-aBlineMean:time-atleft" ; sel="" ;
       plot1D( what , sel, fnm , OutDir );
     } else {
       TCanvas *c1=new TCanvas();
       tree->Draw("Q50:Vbias","","l");
       char* basec   = strdup( fnm.Data() ) ; 
       char *fname   = basename(basec);
       TString sname = TString( fname );
       Int_t idot    = sname.First('.') ;
       TString pdfnm = TString(OutDir) + sname(0,idot) + TString(".pdf") ;
       c1->Print( pdfnm ); 
     }
     return ;
   }

   if (Nx>1 && Ny>1 ) what = TString("x:y:Q50") ;
   if (Nx>1 && Nz>1 ) what = TString("x:z:Q50") ;
   if (Ny>1 && Nz>1 ) what = TString("y:z:Q50") ;
   if ( NV == 1 ) plot2D( what , "" , fnm , OutDir ) ;
   if ( NV > 1 ) {
      for ( Int_t i= 0 ; i<NV ; i++ ) {
        sel = Form("Vbias==%f",emh->vVbias[i]);
	plot2D( what , sel , fnm , OutDir) ;
      }
   }
   
   
}

int main( int argc , char *argv[] ) {

   GetPlot_TCT( TString(argv[1]) ) ;
   return 0; 
}
