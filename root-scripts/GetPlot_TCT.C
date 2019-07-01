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
#include "TMath.h"

//#define OUTDIR "/home/ubuntu/ssd-dbportal/tmp/pdfs/"
//#define INDIR "/home/ubuntu/mounts/TCT_DB/"

#define OUTDIR "/home/ssd/tmp/"
#define INDIR  "/home/ssd/mounts/TCT_DB/"


using namespace std;
extern Int_t  plot1D( TString varexp , TString selection, TString fnm , TString OutDir ) ;
extern Int_t  plot2D( TString varexp , TString selection, TString fnm , TString OutDir ) ;

void GetPlot_TCT( TString fnm ) {

   fnm = TString(INDIR) + fnm + ".txt.root" ;
   
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
   
      //Get filename without extension
      char* basec   = strdup( fnm.Data() ) ; 
      char *fname   = basename(basec);
      TString sname = TString( fname );
      Int_t idot    = sname.First('.') ;
      
      //Clean the disk from existing files (we will concatenate files)
      TString delnm = TString(OutDir) + sname(0,idot) + TString("*.pdf") ;
      TString cmd = Form("rm -f ") + delnm  ;
      gSystem->Exec( cmd ) ;
      
      //Run plot2D for each voltage
      Int_t ANV = ( NV>4)? TMath::Nint(NV/4) : 1 ;
      for ( Int_t i= 0 ; i<NV ; i=i+ANV ) {
      
        //Check there are entries for this voltage (some files may be incomplete)
        sel = Form("Vbias==%f",emh->vVbias[i]);
	Int_t Nent = tree->Draw("x",sel,"goff");
	if (Nent>0) {
	  plot2D( what , sel , fnm , OutDir) ;
        
	 //Save file with _iteration
	 TString fromnm = TString(OutDir) + sname(0,idot) + TString(".pdf") ;
	 TString tonm   = TString(OutDir) + sname(0,idot) + Form("_%d.pdf",i) ;
         cmd = Form("mv ") + fromnm +" "+ tonm ;
	 gSystem->Exec( cmd ) ;
	}
	
      }
      
      //Concatenate PDFs
      TString fromnm = TString(OutDir) + sname(0,idot) + TString("_[0-9].pdf ");
      if ( NV > 9 )  fromnm+=TString(OutDir) + sname(0,idot) + TString("_[1-9][0-9].pdf ") ;
      TString tonm   = TString(OutDir) + sname(0,idot) + TString(".pdf") ;
      cmd = Form("pdftk ")+ fromnm + TString(" cat output ") + tonm;
      gSystem->Exec( cmd ) ;
      
   }
   
   
}

int main( int argc , char *argv[] ) {

   GetPlot_TCT( TString(argv[1]) ) ;
   return 0; 
}
