/*
  This program reads in any number of TCT files and produces a summary with the
  most important information
  
  Missing: sensor name, date

*/

#include "TCTSummary.h"
#include <iostream>

ClassImp(TCTSummary)

TCTSummary::TCTSummary(  ) {
  
  fnm    = TString("empty");
  sensor = TString("empty");
  NV  = 1 ;
  Nx  = 0 ; 
  Ny  = 0 ; 
  Nz  = 0 ;
  Ntot= 1 ;     

  comment = TString("empty") ;  
  Temp   = 20.0 ;   
  Lambda = 1064.0 ;
  LPower = 1. ;   
  Freq   = 200. ;   
  Nav    = 16 ;   

  Setup  = 1 ;
  Illum  = 0 ;

  dt     = 0. ;	   
  dx     = 0. ;	   
  dy     = 0. ;	   
  dz     = 0. ;	   
  
  utc    =  TDatime(2000,11,5,3,0,0) ;  
  aPolarity  = 0 ;   
  event    = 0 ;
  
}

Int_t TCTSummary::FillTree( TTree *tsum , char *tfnm , Int_t iev ) {

     
     //Open input file
     TFile *f = new TFile( tfnm );
     if (f->GetNkeys()==0)  return(-1) ;
     if ( !f->IsOpen() ) return(-1) ;
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
    

     //Grab information
     fnm = TString( tfnm ) ;     
     
     NV = emh->NV ;
     Nx = emh->Nx ; 
     Ny = emh->Ny ; 
     Nz = emh->Nz ;
     Ntot = tree->GetEntries(); 
     
     comment = emh->comment;
     
     tree->Draw("Temp>>hist");
     TH1D *hist = (TH1D *) gDirectory->Get("hist");
     Temp       = hist->GetMean();
     delete hist ;
     
     Lambda  = emh->Lambda ;

     tree->Draw("LPower>>hpow");
     TH1D *hpow = (TH1D *) gDirectory->Get("hpow");
     LPower   = hpow->GetMean();
     delete hpow ;

     Freq    = emh->Freq ;
     Nav     = emh->Nav ;
     
     Setup   = em->Setup ;
     Illum   = emh->Illum ;

     dt = emh->At ;
     dx = emh->Ax ; 
     dy = emh->Ay ; 
     dz = emh->Az ;
     
     aPolarity  = emh->Polarity  ; 

     //tree->GetEntry(0) ;
     utc        = em->utc ;
          
     //Parse sensor name from filename. Name delimited between 2nd underscore and final dot
     if (Setup!=3 && Setup!=4 && Setup!=7 && Setup!=8) { 
       char* basec   = strdup(tfnm) ; 
       char *fname   = basename(basec);
       TString sname = TString( fname );
       Int_t iund = sname.Index('_'); iund = sname.Index('_',iund+1);
       Int_t idot = sname.Index('.') ;
       sensor = sname( iund+1,idot-iund-1 );
     }

     event = iev ; 
     tsum->Fill();
     if (event%10==0) cout << "Read " << event << flush <<"\r" ;
     
     delete wv ;
     delete em ;
     delete tree ;
     f->Close();  
     return(1) ;
     
}

TCTSummary::~TCTSummary(  ) {

     //delete [] Vbias     ;
     
}
