/*  
    This script plots a 2D colz histogram of an edge_tct root file.
    
    Syntax is: 
        
	plot2D("Xcoord:Ycoord:Zcoord","selection",rootfile);
    
    where Zcoord will be painted in colors as a 2D histogram.
    
    "selection" is a string like for the TTree::Draw command. Example:

       plot2D("time:Vbias:volt","(8.5<z && z<9.0) && (95.<time && time<150.)","~/data/etct/test.ZVscan.root")
    
    Do not use variables to constrain another variable in a loop (like "tleft<time"), cause
    they will change from event to event and then one cannot histogram it.

    _________________________________________________________________________________________

    Some things to remember:
    
    1) Only use arrays in X (and therefore Z) coordinates. Not in Y.
    
    2) Note that it can well happen that the Y coordinate is not evely distributted. This script 
    uses varibale bin size in Yaxis.

    3) By changing the preprocessor EXE variable, we can either:
       EXE 0 = interpret code
       EXE 1 = .L plot2D.C++
       		plot2D(...)
       EXE 2 = compiles (easier to debug using ddd)
      
 Geneve Jun 2012
  
*/

#define EXE 1
#define DEBUG 1

#define STRIDE 1

#if EXE==1 || EXE==2
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TClassTable.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TTreeFormula.h"
#include "TEventList.h"
#include "TEntryListArray.h"
#include "TLegend.h"
#include "TString.h"
#include "TMath.h"

#include "TMeas.h"

//#define def_TCT 1     //ONLY NEEDED FOR LGdiodes measured at CERN and analyzed interactively in root.
                      //AutoPilot_TCT will set variable by itself in the compiler script

#include "TMeasHeader.h"

#include "TWaveform.h"

#endif


#if EXE>0
UInt_t 	SplitNames2D(const TString &varexp, std::vector<TString> &names) ;
Int_t  	GetNerOfBins( TTree *tree , TString varnm ) ;
bool 	tolerance (Double_t x, Double_t y) ;
#endif

#if EXE==2
Int_t 	plot2D( TString varexp , TString selection, TString fnm , TString OutDir ) ;
#endif

//___________________________________________________________________________________
bool tolerance (Double_t x, Double_t y) {
   
   //return( TMath::Abs(x-y)<0.0005 );  //Good for steppers
   return( TMath::Abs(x-y)<1e-6 );  //Good for piezoelectric motors
   
}


//___________________________________________________________________________________

Int_t plot2D( TString varexp , TString selection, TString fnm , TString OutDir ) {
 
#if EXE==2  //Interpreted script
  if (!TClassTable::GetDict("TMeas"))        gSystem->Load("TMeas_cpp.so");
  if (!TClassTable::GetDict("TMeasHeader"))  gSystem->Load("TMeasHeader_cpp.so");
  if (!TClassTable::GetDict("Waveform"))     gSystem->Load("TWaveform_cpp.so");
#endif
 
  //Tree handling

  TFile *f = new TFile( fnm );
  if ( !f->IsOpen() ) return 0 ;
  
  TTree *tree = (TTree*)f->Get("edge");  
  //Long64_t nentries = tree->GetEntries();
  
  //See MainEvent.cxx
  
  TMeas     *em = 0 ;
  TBranch *raw  = tree->GetBranch("raw");
  raw->SetAddress(&em) ;
  raw->GetEntry(0) ;  

  TWaveform *wv = new TWaveform( em ) ;     
  TBranch *proc  = tree->GetBranch("proc");
  proc->SetAddress(&wv) ;
  
  //tree->GetEntry(0) ;

  //tree->AddFriend("s2=edge","/home/mfg/APDs/cern/TCT/data/bdc2/w8c4_allTemp.dat.root");

  //Input variable parsing. From "time:z:volt" --> "time", "z", "volt"
  
  vector<TString> varname ;
  UInt_t nvar  = SplitNames2D(varexp, varname);
  if (nvar!=3) {
    cout << "This script only works with 3 variables" << endl;
    return 0;
  }
  
  //Convert from varname (string) to actual variable name:
  //Note that varname[0] works for tree->Draw(), since it is a string,
  //but not for HFill(time,z,volt)
  
  TTreeFormula **fVar = new TTreeFormula *[nvar] ;
  Double_t     **fVal = new Double_t*[nvar] ;
  for (UInt_t ii=0 ; ii<nvar ; ii++ ) {
    fVar[ii] = new TTreeFormula( Form("Var%i", ii + 1), varname[ii].Data() , tree );
    fVar[ii]->GetNdata() ;
  }
   
  //Canvas 
  TCanvas *c1=new TCanvas() ;
  gStyle->SetOptStat(0);

  //Create a TEventList to account for events passing selection   
  
  cout << "Counting ner of events that pass the cut ... " << flush ; 

#ifdef SIMPLE  
  Int_t  nev  = tree->Draw( ">>myList" , selection ) ;
  TEventList *list=(TEventList*)gDirectory->Get("myList") ;
  tree->SetEventList( list ) ; //All next calls use the event list
                               //Use tree->SetEventList(0) to switch off
#endif  
  tree->Draw( ">>myList" , selection , "entrylistarray") ;
  TEntryListArray *list=(TEntryListArray*)gDirectory->Get("myList") ;
  tree->SetEntryList( list ) ; //All next calls use the event list
                               //Use tree->SetEventList(0) to switch off
  Int_t  nev  = list->GetN() ; 
  
  cout << nev <<" events in total" << endl ;
  
  //2D histogram limits and number of bins
  //Double_t ymin=tree->GetMinimum( varname[1] ) ;			       
  //Double_t ymax=tree->GetMaximum( varname[1] ) ;
  
  Int_t Nx = GetNerOfBins( tree , varname[0] ) ;			      
  Int_t Ny = GetNerOfBins( tree , varname[1] ) ;
  Int_t Nz = GetNerOfBins( tree , varname[2] ) ;
  if (Nx==-1) Nx = em->Nt ;
  if (Ny==-1) Ny = em->Nt ;
  if (Nz==-1) Nz = em->Nt ;
  
   if ( Nx!=Nz ) {
     cout << "Variable "<<varname[0]<<" and "<<varname[2]<<" have different ner of bins:"<<endl ;
     cout <<"(Nx="<<Nx<<" and Nz="<<Nz<<". They should be equal!"<<endl ;
     cout <<"Discard this warning if you are using special functions on variables"<<endl;    
   } 

  
  // The variable we pass to Draw() is text. It can be an array, like "volt" or a single value, like "Vmin"
  
  Int_t Nvx ,Nvy , Nvz ;  //These are the size of the 2nd dimension of fVal[ii]
  Nvx = ( Nx==em->Nt ) ? Nx : 1 ;  
  Nvy = ( Ny==em->Nt ) ? Ny : 1 ;  
  Nvz = ( Nz==em->Nt ) ? Nz : 1 ;  
  

  //Y coordinate is likely not to be uniformly binned if we are measuring ZV scan
  //Besides that, most likely we are also constraining the X or Y coordinates, so the actual number
  //of bins along the coordinates involved is lower.
  
  vector<Double_t> xsorted , ysorted ;
  
  cout << "Dumping X and Y coordinates into vectors..." << flush ; 
  for ( Int_t ii=0 ; ii<nev ; ii++ ) {
  
    //Event that passes "selection"
  
    Int_t iev = list->GetEntry(ii) ; 
    tree->GetEntry( iev );       //FIXME: Read only one leaf of the branch

    //By convention, do not plot arrays in Y coordinate, so there are no sublists in Y
    for (Int_t iy=0 ; iy<Nvy ; iy++ ) ysorted.push_back( fVar[1]->EvalInstance(iy) ) ;

    //If there are arrays in this event, selection may have chosen some elements of the array
    //and discarded others. We then need to look at sublist(=list of elements of an array 
    //that passed) the selection.
  
    TEntryListArray *slist=list->GetSubListForEntry( iev ) ;
    //if (ii<1) {  //IF YOU UNCOMMENT THIS "if", then you accept that the X binning is that of the first slice along X
      if ( slist ) {
	for (Int_t ix=0 ; ix < slist->GetN() ; ix++ ) { 
	  Int_t isev = slist->GetEntry(ix) ;
	  xsorted.push_back( fVar[0]->EvalInstance(isev) ) ;
	}
      } else for (Int_t ix=0 ; ix<Nvx ; ix++ ) xsorted.push_back( fVar[0]->EvalInstance(ix) ) ;
    //}
    
  }
  cout << "done" << endl ;
  
  //To find the real ner of bins along X and Y with conditions, we fill vectors with all values
  //then we sort them and finally we find unique elements

  cout << "Finding unique occurrences..." << flush ; 
  vector<Double_t>::iterator it;
  
  sort( ysorted.begin() , ysorted.end() );
  it=unique( ysorted.begin() , ysorted.end() , tolerance );
  ysorted.resize( it-ysorted.begin() ) ;

  sort( xsorted.begin() , xsorted.end() );
  it=unique( xsorted.begin() , xsorted.end() , tolerance );
  xsorted.resize( it-xsorted.begin() ) ;
  
  if ( xsorted.size() > Nx ) {
    cout << "Found more entries than originally for variable "<<varname[0]<<endl ;
    cout<< xsorted.size()<<" " <<Nx<<endl;
    for (Int_t ix=0 ; ix < xsorted.size() ; ix++ ) std::cout<< xsorted[ix] << " " <<std::endl;
    return 0;
  } else   Nx=xsorted.size() ;
  if ( ysorted.size() > Ny ) {
    cout << "Found more entries than originally for variable "<<varname[0]<<endl ;
    return 0 ;
  } else   Ny=ysorted.size() ;
  cout << "done" << endl ;
    

  // We will have to dump the value of the variables into x-->fVal[0][0..Nx] , y-->fVal[1][0..Ny]
  
  fVal[0] = new Double_t[Nx];
  fVal[1] = new Double_t[Ny];
  fVal[2] = new Double_t[Nx];  //We force X and Z to be of the same dimension!!

  //Calculate Bin width for Y coordinate (it may not be evenly distributed)
  
  Double_t BinWidth=1.e6;
  for ( Int_t ii=0 ; ii<Ny-1 ; ii++ ) {
    Double_t val=ysorted[ii+1]-ysorted[ii] ;
    if (val<BinWidth) BinWidth=val ;
  }
   
  Double_t *ylowedge= new Double_t [Ny+1] ;
  for ( Int_t ii=0 ; ii<Ny ; ii++ ) ylowedge[ii]=ysorted[ii]-0.5*BinWidth;
  ylowedge[Ny]=ysorted[Ny-1]+0.5*BinWidth;

  //Create the 2D histo
 
  Double_t xmin=xsorted[0] ; Double_t xmax=xsorted[Nx-1] ;
  TString htit=varname[2]+"=f("+varname[0]+","+varname[1]+")" ;  
  Double_t wbin = xsorted[1]-xsorted[0] ;
  Double_t txmin = xmin - 0.5*wbin , txmax = xmax + 0.5*wbin ;
  TH2D *h2D = new TH2D( htit , "" , Nx , txmin , txmax , Ny , ylowedge ) ;
  if (DEBUG==1) {
    cout << "Creating 2D histogram: (Nx,Xmin,Xmax)="<<Nx<<","<<txmin<<","<<txmax;
    cout << " (Ny,Ymin,Ymax)=" <<Ny<<","<<ylowedge[0]<<","<<ylowedge[Ny] << endl ;
  }
  
  //Loop
  
  cout << "Filling 2D histogram" << endl ; 
  UInt_t isel =0 , ifill=0 ;
  for ( Int_t ii=0 ; ii<nev ; ii++ ) {
     
    Int_t iev=list->GetEntry(ii) ;
    tree->GetEntry( iev );
    TEntryListArray *sublist = list->GetSubListForEntry( iev );
    
    if (sublist) {

      for (Int_t iy=0 ; iy<Nvy ; iy++ ) {
	fVal[1][iy]=fVar[1]->EvalInstance(iy);      //Normally a single value variable
	for (Int_t ix=0 ; ix<sublist->GetN() ; ix++ ) {
          Int_t jj=sublist->GetEntry(ix) ;
          fVal[0][ix]=fVar[0]->EvalInstance(jj);    //Xaxis&Zaxis either vector or single value variable
	  fVal[2][ix]=fVar[2]->EvalInstance(jj);
          h2D->Fill(fVal[0][ix],fVal[1][iy],fVal[2][ix]) ;
	  ifill++;
	}
      }

    } else {

      for (Int_t iy=0 ; iy<Nvy ; iy++ ) {
	fVal[1][iy]=fVar[1]->EvalInstance(iy);      //Normally a single value variable
	for (Int_t ix=0 ; ix<Nvx ; ix++ ) {
          fVal[0][ix]=fVar[0]->EvalInstance(ix);    //Xaxis&Zaxis either vector or single value variable
	  fVal[2][ix]=fVar[2]->EvalInstance(ix);
          h2D->Fill(fVal[0][ix],fVal[1][iy],fVal[2][ix]) ;
	  ifill++;
	}
      }      

    }

    
    if (DEBUG==1 && isel%100==0) cout << "Event=" <<isel << "/"<<nev<<" processed"<< "\r" <<flush ;
    isel++;
    
    delete sublist ;

  }

  cout << "\n Total: "<<isel<<" events passed the selection" << endl ;
  cout << ifill << " HFILL calls" << endl ;
  //h2D->GetXaxis()->SetTitle( varname[0] ) ; h2D->GetYaxis()->SetTitle( varname[1] ) ;  
  if      ( varname[0].Contains("time") )      h2D->GetXaxis()->SetTitle( "Time [ns]" ) ;
  else if ( varname[0].EqualTo("z") )          h2D->GetXaxis()->SetTitle( "z [mm]" ) ;
  else if ( varname[0].EqualTo("y") )          h2D->GetXaxis()->SetTitle( "y [mm]" ) ;
  else if ( varname[0].EqualTo("x") )          h2D->GetXaxis()->SetTitle( "x [mm]" ) ;
  else if ( varname[0].Contains("Vbias") )     h2D->GetXaxis()->SetTitle( "Bias voltage [V]" ) ;
  else                                         h2D->GetXaxis()->SetTitle( varname[0] ) ; 
  
  if      ( varname[1].Contains("volt") )      h2D->GetYaxis()->SetTitle( "Signal [V]" ) ;
  else if ( varname[1].Contains("Q50") )       h2D->GetYaxis()->SetTitle( "Charge [a.u.]" ) ;
  else if ( varname[1].Contains("RiseTime") )  h2D->GetYaxis()->SetTitle( "Rise Time [ns]" ) ;
  else if ( varname[1].EqualTo("z") )          h2D->GetYaxis()->SetTitle( "z [mm]" ) ;
  else if ( varname[1].EqualTo("y") )          h2D->GetYaxis()->SetTitle( "y [mm]" ) ;
  else if ( varname[1].EqualTo("x") )          h2D->GetYaxis()->SetTitle( "x [mm]" ) ;
  else if ( varname[1].Contains("Vbias") )     h2D->GetYaxis()->SetTitle( "Bias voltage [V]" ) ;
  else                                         h2D->GetYaxis()->SetTitle( varname[1] ) ; 
  h2D->Draw("col,z");
  c1->Update();
  
  //Create a PDF out of it
  char* basec   = strdup( fnm ) ; 
  char *fname   = basename(basec);
  TString sname = TString( fname );
  Int_t idot    = sname.First('.') ;
  TString pdfnm = OutDir + sname(0,idot) + TString(".pdf") ;
  c1->Print( pdfnm ); 
  
  delete [] fVar ;
  delete [] fVal ;
  xsorted.clear();
  ysorted.clear();
  delete tree;
  
  h2D->SetDirectory(0);
  f->Close();

  return 1 ;    
}

//___________________________________________________________________________________


UInt_t SplitNames2D(const TString &varexp, std::vector<TString> &names)
{
   // Build Index array for names in varexp.
   // This will allocated a C style array of TString and Ints

   names.clear();

   Bool_t ternary = kFALSE;
   Int_t prev = 0;
   for (Int_t i = 0; i < varexp.Length(); i++) {
      if (varexp[i] == ':'
          && !((i > 0 && varexp[i-1] == ':') || varexp[i+1] == ':')
         ) {
         if (ternary) {
            ternary = kFALSE;
         } else {
            names.push_back(varexp(prev, i - prev));
            prev = i + 1;
         }
      }
      if (varexp[i] == '?') {
         ternary = kTRUE;
      }
   }
   names.push_back(varexp(prev, varexp.Length() - prev));
   return names.size();
}
//___________________________________________________________________________________

Int_t GetNerOfBins( TTree *tree , TString varnm ) {

   
   TMeasHeader *emh=(TMeasHeader *) tree->GetUserInfo()->At(0) ;

   //In edge-TCT the direction z is priviledged.
   // If not a func of x,y,or time, then it is a function of z.
   Int_t val=-2 ; 
   if ( varnm=="Vbias" )                val = emh->GetNV() ;
   
   if ( emh->GetNx()>0 && varnm.Contains("x") )  val = emh->GetNx() ;
   
   if ( emh->GetNy()>0 && varnm.Contains("y") )  val = emh->GetNy() ;
   
   if ( varnm.Contains("volt") || 
        varnm.Contains("time") )        val = -1 ;
   
   if ( emh->GetNz()>0 && val==-2 )     val = emh->GetNz() ;
   

   return val ;

}

//______________________________________________________________________________

#if EXE==2

int main(int argc, char *argv[]) {
   
  char varexp[]="y:x:Vmin" ;
  char selection[]="";

  plot2D( varexp , selection , "/home/mfg/TPA/data/March2017_HVCMOS/20170322_0945_HVCMOSv3_n7E15_22C_1300nm_150fs_baseline_subtracted_alphacorr.root" , "/home/ssd/tmp") ;
  return (0) ;
  
}

#endif
