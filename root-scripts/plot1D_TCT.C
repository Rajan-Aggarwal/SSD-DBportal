/* 

EXE = 1 ROOT lib (or when we need this function to be called from a compiled script)
.L plot1D.C++
plot1D("Vmin:z","",3,"~/data/etct/20120517113317_Micron_FZ2328-11_NonP.ZVscan.root","~/data/etct/20120517190507_Micron_FZ2328-11_NonP.ZVscan.root","/home/mfg/data/etct/20120517193253_Micron_FZ2328-11_NonP.ZVscan.root")

or in the shell:
EXE = 2
plot1D "Vmin:z" "" ~/data/etct/201205*_Micron_FZ2328-11_NonP.ZVscan.root
plot1D 'Sum$((volt-BlineMean)*(time>100.0 && time<125.0)):z' "Vbias==-170" ~/data/etct/20120815133509_Micron_FZ2328-11_NonP_3.ZVscan.root
plot1D 'Sum$((volt-BlineMean)*(time>10.12 && time<15.42)):z' "" "$DATA/20121101165527_FZ320N_03_Badd_1.ZVscan.root"

   This scripts draws a variable vs time or in general, against any variable
   that is pitched, like x,y or Z. 
   
   Conditions, as in Draw, are allowed i the 2nd argument
   
   IMPORTANT NOTE {
      
      In 1D (ColorDraw*), the syntax is: "Yvar:Xvar"
      In 2D (plot2D*), the syntax is: "Xvar:Yvar:Zvar"
   }
   
   For each time series, we use a different color/line style
   
      ColorDraw("volt:time","z*1000%150==0 && Vbias==300","")
      ColorDraw("Sum$(volt-BlineMean):z")

   We should use TGraphs if we want to use cuts in the X variable that are
   variables, therefore changing from event to event. In this case, the resulting
   histograms are of different ner of bins.
   Example: 
   ColorDraw("volt:time","z>8.45 && z<8.85 && time>tleft && time<tright && event%60==0","~/data/etct/20120607100742_HPK_FZ320Y_05_BADD_1.ZVscan.root")
   
   TODO:
     
     1) We are using sublists to select those events that pass time cuts, but X arrays are binned
     For binned data the cut does not need to review the contents of the whole vector. It would be
     enough to do cutvalue/step and we would know from which to which index we could use. 
     If we want to implement this, we need to parse the selection argument. For the moment we only 
     parsed varexp argument. 

   TESTS:
   
   OK
   ColorDraw("volt:time","8.45<z && z<8.85 && Vbias==120 && event%100==0","~/data/etct/20120607100742_HPK_FZ320Y_05_BADD_1.ZVscan.root")
   ColorDraw("volt:time","8.45<z && z<8.85 && Vbias==120 && tleft<time && time<200. && event%100==0","~/data/etct/20120607100742_HPK_FZ320Y_05_BADD_1.ZVscan.root")
   ColorDraw("volt:time","8.45<z && z<8.85 && Vbias==120 && tleft<time && time<tright && event%100==0","~/data/etct/20120607100742_HPK_FZ320Y_05_BADD_1.ZVscan.root")
   ColorDraw("Vmin:z","","~/data/etct/20120607100742_HPK_FZ320Y_05_BADD_1.ZVscan.root")
   ColorDraw("Vmin:z","8.45<z && z<8.85","~/data/etct/20120607100742_HPK_FZ320Y_05_BADD_1.ZVscan.root")  

   OK?
   ColorDraw("Sum$(volt-BlineMean):z","140.<time && time<180.","~/data/etct/20120607100742_HPK_FZ320Y_05_BADD_1.ZVscan.root")  
   There is a sublist but we do not plot vs time
   (atually sublist is in volt). Do we need to implement it or it comes for free from ROOT interpreter?

   Notes:
   
   Once the histograms are painted and the file is closed, we can get the stack and change
   coordinates, like
   THStack *hs = (THStack *) gPad->GetPrimitive("hs");
   hs->GetYaxis()->SetTitle("Drift velocity [a.u.]");

   
   Geneve Jun 2012
  
*/

#define EXE 1      //1 for shared lib inside root. Note that still a ".o" can be produced in this mode
                   //by compiling the source, as needed for plotvd.C
		   
		   //2 for executable outside root
		   
#define DEBUG 0     

#define PSOUTPUT 0

#define AT 0.05
#define STRIDE 1

#if EXE==1 || EXE==2
#include <cstdarg>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <libgen.h>

#include "TFile.h"
#include "TTree.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TClassTable.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TH1.h"
#include "TAttLine.h"
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
UInt_t  SplitNames(const TString &varexp, std::vector<TString> &names) ;
TString CalculateName( vector<TString> varname , TMeas *em ) ;
Int_t   plot1D( TString varexp , TString selection, TString fnm , TString OutDir  ) ;
void    ParseChars( TString &tstring  ) ;
#endif


//___________________________________________________________________________________
Int_t plot1D( TString varexp , TString selection, TString fnm , TString OutDir) {
 
#if EXE<2  //Interpreted script
  if (!TClassTable::GetDict("TMeas"))            gSystem->Load("TMeas_cpp.so");
  if (!TClassTable::GetDict("TMeasHeader"))      gSystem->Load("TMeasHeader_cpp.so");
  if (!TClassTable::GetDict("TWaveform"))        gSystem->Load("TWaveform_cpp.so");
#endif
 
  TH1::AddDirectory(kFALSE);
  
  gStyle->SetLineWidth(1.);      //thickness of null box
  gStyle->SetHistLineWidth(1.);  //thickness of histograms
  
  //Tree handling

  TFile *f = new TFile( fnm );
  if ( !f->IsOpen() ) exit(-1) ;

  TTree *tree = (TTree*)f->Get("edge");  
    
  //See MainEvent.cxx
  
  TMeas     *em = 0 ;
  TBranch *raw  = tree->GetBranch("raw");
  raw->SetAddress(&em) ;
  raw->GetEntry(0) ;  

  TWaveform *wv = new TWaveform( em ) ;     
  TBranch *proc  = tree->GetBranch("proc");
  proc->SetAddress(&wv) ;
  
  //tree->GetEntry(0) ;  //Commented 19Sept2012
  
  //Trick: we pass in "selection" the name of the friend.
  TString Frnm=TString( selection );
  TFile *frnm = new TFile( Frnm );
  
  //If selection is a tree friend's filename
  //File to store histograms 
  TFile *fout ;
  if ( frnm->IsOpen() ) { 
    frnm->Close();
    tree->AddFriend("s2=edge", selection );  
    TString Fout = TString("1D_")+selection;
    fout = new TFile( Fout.Data() ,"recreate" );
    selection="";
  } else fout = new TFile( "1Dhistos.root","recreate" );
  
  /* Input variable parsing. From "time:z:volt" --> "time", "z", "volt" */

  vector<TString> varname ;
  UInt_t nvar  = SplitNames(varexp, varname);
  
  /*//////////////////////////////////////////////
  
    Convert from varname (string) to actual variable name:
    Note that varname[0] works for tree->Draw(), since it is a string,
    but not for HFill(time,z,volt)
    Note: For the case "volt:Vbias" we have:
      fVar[0]->EvalInstance(0)   = volt[0]
      fVar[1]->EvalInstance(0)   = time[0]
      fVar[1]->EvalInstance(10)  = time[10]

    And afterwards, we will have:
    
      fVal[0][0..Nx] = volt[0..Nx]
      fVal[1][0..Nx] = time[0..Nx]

  //////////////////////////////////////////////*/
  
  TTreeFormula **fVar = new TTreeFormula *[nvar] ;
  Double_t     **fVal = new Double_t*[nvar] ;
  for (UInt_t ii=0 ; ii<nvar ; ii++ ) {
    fVar[ii] = new TTreeFormula( Form("Var%i", ii + 1), varname[ii].Data() , tree );
    fVar[ii]->GetNdata() ;
  }  
   
  //Canvas + stack + legend box
  
  //TCanvas *c1=new TCanvas( ) ;
  
  //THStack allows histograms of different bins, xmin, xmax to be overlapped
  THStack *hs=new THStack( "hs", selection ) ;
 

  //Create a TEventList to account for events passing selection   
  
  cout << "Counting ner of events that pass the cut ... " << flush ; 

  tree->Draw( ">>myList" , selection , "entrylistarray") ;
  TEntryListArray *list=(TEntryListArray*)gDirectory->Get("myList") ;
  tree->SetEntryList( list ) ; //All next calls use the event list
                               //Use tree->SetEventList(0) to switch off
  Int_t  nev  = list->GetN() ; 
  
  cout << nev <<" events in total" << endl ;
  if (nev==0) return 0;

  //If we are plotting a variable vs something else than 
  //a time array, then the Xaxis does not change from event to event,
  //so we can calculate Nx once for all. Note that everytime we pass Nx events,
  //we will have a new histogram.

  Int_t Nx ;
  vector<Double_t> xsorted ;
  if ( varname[1](0,4)!="time" ) {

    cout << "Dumping X variable into a vector..." << flush ; 
    for ( Int_t ii=0 ; ii < nev ; ii++ ) {
      Int_t iev = list->GetEntry(ii) ; 
      tree->GetEntry( iev );       
      xsorted.push_back( fVar[1]->EvalInstance(0) ) ;
    }  
    //To find the real ner of bins along X with conditions, we have filled a vector
    //with all values. Now we sort it and finally we find unique elements
    vector<Double_t>::iterator it;
    sort( xsorted.begin() , xsorted.end() );
    it=unique( xsorted.begin() , xsorted.end() );
    xsorted.resize( it-xsorted.begin() ) ;
    Nx=xsorted.size();
    cout << "done" << endl ;
  } 
  
  if ( varname[1](0,4)=="time" ) 
    cout << "We are filling " << nev <<" histograms" << endl ;    
  else
    cout << "We are filling " << nev/Nx <<" histograms" << endl ;    
  

  UInt_t icol=1 , isel =0 ;
  Int_t  lsty = 1 ;
  TH1D *h1 ;
  for (Int_t ii=0 ; ii<nev ; ii++ ) {

    //Event that passes "selection"
  
    Int_t iev = list->GetEntry(ii) ; 
    tree->GetEntry( iev );       //FIXME: Read only one leaf of the branch
    
    //Search for X array limits within this event. We suppose the X coordinate
    //grows monotically. In case we plot vs time, we can sometimes put
    //constraints in that variable time. In this case, for each even, there
    //will be a sublist of events passing the cuts. Otherwise, "slist" 
    //will be null
    
    TEntryListArray *slist=list->GetSubListForEntry( iev ) ;
    Double_t tmin , tmax ;
    
    //Case: "volt:time","tleft<time && time<150."
    if ( slist && varname[1](0,4)=="time" ) {  //We plot vs time && there are constraints in time

      Int_t il=slist->GetEntry(0) ;
      Int_t ir=slist->GetEntry( slist->GetN()-1 ) ;
      Nx=ir-il+1;    
      if ( Nx!=slist->GetN() ) {
        cout<<"Help! Non consistent error!!"<<endl;
	exit(-1);
      }
      
      tmin = fVar[1]->EvalInstance(il) ;
      tmax = fVar[1]->EvalInstance(ir) ;
      
    } 
    
    //Case: "volt:time","Vbias==120"
    if ( !slist && varname[1](0,4)=="time" ) { //Plot vs time && there are NO constraints in time
      
      Nx = em->Nt;
      tmin = fVar[1]->EvalInstance(0);
      tmax = fVar[1]->EvalInstance(Nx-1);
    }
    
    //Two different cases here:
    // Case 1: "Vmin:z","Vbias==120"                          Not plotting vs time and no sublist
    //Case: 2 "Sum$(volt):z","tleft<time && time<tleft+50."   Not plotting vs time, sublist in Y!!!
    if ( varname[1](0,4)!="time" && !slist ) { //Not plotting vs time 
      tmin = xsorted[0] ;
      tmax = xsorted[Nx-1] ;
      if (slist) {
        cout << "There is a sublist but we do not plot vs time!!" << endl ;
	exit(-1);
      }
    }


    //HISTOGRAM BOOKING:
    //If we plot vs time, each "ii" event (out of nev) corresponds to 1 histogram. Nx will
    //be the ner of bins and therefore the dimension of fVar[1]
    //
    //If we do NOT plot vs time, then Nx will be the number of bins
    //but the variable fVar[1] will not be anymore and array. Note that everytime we hit 
    //xsorted[0] we are starting a new histo
    
    if ( varname[1](0,4)=="time" ) {   

      //TString htit=Form("(%7.4f,%7.4f,%7.4f) mm, V=%4d V",em->x,em->y,em->z,(Int_t)em->Vbias) ;  //Needed by AutoPilot_eTCT
      //TString htit=Form("V=%4d V",(Int_t)em->Vbias) ;
      TString htit=Form("ix=%d",(Int_t)em->ix) ;
      h1 = new TH1D (htit,htit,Nx,tmin,tmax) ;
      if (DEBUG) cout<<"Booking new time histogram h1("<<Nx<<","<<tmin<<","<<tmax<<")"<<endl;

    } else {

      if ( xsorted[0]== fVar[1]->EvalInstance() ) {
	TString htit=CalculateName( varname , em ) ;
	//h1 = new TH1D (htit,htit,Nx,tmin,tmax) ; 
	Double_t wbin = xsorted[1]-xsorted[0] ;
	Double_t ttmin = tmin - 0.5*wbin , ttmax = tmax + 0.5*wbin ;
	h1 = new TH1D (htit,htit,Nx,ttmin,ttmax) ; 
	cout << "Dumping event "<< ii/Nx+1 << "/"<< nev/Nx << "\r" << flush ;
      }
      
    }

    //Dynamic allocation for the vectors where we will store variables
    
    if ( varname[1](0,4)=="time" ) {
      fVal[0] = new Double_t[Nx];
      fVal[1] = new Double_t[Nx];  //Obviously X and Y need to have same dimension!!
    } else {
      fVal[0] = new Double_t[1];
      fVal[1] = new Double_t[1];   //There are no arrays in X     
    }
    
    // Filling histograms
    
    if ( slist && varname[1](0,4)=="time" ) {
    
      for (Int_t ix=0 ; ix < slist->GetN() ; ix++ ) {
	Int_t jj = slist->GetEntry(ix) ;
	fVal[0][ix]=fVar[0]->EvalInstance(jj);
	fVal[1][ix]=fVar[1]->EvalInstance(jj);
      }
    
      h1->FillN( Nx , fVal[1] , fVal[0] , STRIDE ) ;
      cout << "Event "<<ii+1<<"/"<<nev<<" dumped" << "\r" << flush   ;
    }
     
    if ( !slist && varname[1](0,4)=="time" ) {
    
      for (Int_t jj=0 ; jj<em->Nt ; jj++ ) {
        fVal[0][jj]=fVar[0]->EvalInstance(jj);
        fVal[1][jj]=fVar[1]->EvalInstance(jj);
      }

      h1->FillN( Nx , fVal[1] , fVal[0] , STRIDE ) ;
      cout << "Event "<<ii+1<<"/"<<nev<<" dumped" << "\r" << flush   ;
  
    }
    
    if ( varname[1](0,4)!="time" ) {
      Double_t    xc  = fVar[1]->EvalInstance() ;
      Double_t weight = fVar[0]->EvalInstance() ;      
      h1->Fill( xc , weight  ) ;
      //cout<<h1->GetTitle()<<" "<<xc<<" " <<weight <<endl;
      //cout<<h1->GetXaxis()->GetXmin()<<" "<<h1->GetXaxis()->GetXmax()<<" "<<xc<<" " <<weight <<endl;
   }
    

    //Add full histogram to the stack
    if ( varname[1](0,4)=="time" ) {
      f->cd();
      hs->Add(h1);
      if ( icol==9 ) {
	icol=1 ; 
	lsty++ ;
      }
      h1->SetLineColor( icol ) ;
      if (icol==5) h1->SetLineColor( kOrange ) ;
      h1->SetLineStyle( lsty ) ;

      fout->cd();
      h1->Write();
      f->cd();

      icol++ ; 
    } else{ 
      if ( ii>0 && ((ii+1)%Nx == 0) ) { 
	if ( icol==9 ) icol=1 ; 
        f->cd();
        hs->Add(h1);
        h1->SetLineColor( icol ) ;
        if (icol==5) h1->SetLineColor( kOrange ) ;
	h1->SetLineStyle( lsty ) ;
        if      (lsty==1) lsty=3 ;   
        else if (lsty==3) lsty=7 ;   
        else if (lsty==7) lsty=-1 ;   
        
	
	fout->cd();
	//cout <<"Writing for event "<<ii<<" which is "<<ii/Nx<<" and "<<ii%(Nx-1)<<endl ;
	h1->Write();
	f->cd();

	if (lsty==-1) {
	  lsty = 1 ;
          icol++ ; 
	}
      }
    }
    isel++;
    
    delete slist ; 
    delete [] fVal[0] ;   
    delete [] fVal[1] ;   


  }
  
  cout << endl ; 
  
  TCanvas *c1=new TCanvas() ;
  hs->Draw("nostack hist");    
  if      ( varname[1].Contains("time") )      hs->GetXaxis()->SetTitle( "Time [ns]" ) ;
  else if ( varname[1].EqualTo("z") )          hs->GetXaxis()->SetTitle( "z [mm]" ) ;
  else if ( varname[1].EqualTo("y") )          hs->GetXaxis()->SetTitle( "y [mm]" ) ;
  else if ( varname[1].EqualTo("x") )          hs->GetXaxis()->SetTitle( "x [mm]" ) ;
  else if ( varname[1].Contains("Vbias") )     hs->GetXaxis()->SetTitle( "Bias voltage [V]" ) ;
  else                                         hs->GetXaxis()->SetTitle( varname[1] ) ; 
  
  if ( varname[0].Contains("volt") )      hs->GetYaxis()->SetTitle( "Signal [V]" ) ;
  else if ( varname[0].Contains("Q50") )       hs->GetYaxis()->SetTitle( "Charge [a.u.]" ) ;
  else if ( varname[0].Contains("RiseTime") )  hs->GetYaxis()->SetTitle( "Rise Time [ns]" ) ;
  else                                         hs->GetYaxis()->SetTitle( varname[0] ) ; 
  //hs->GetXaxis()->SetTitle( varname[1] ) ; 
  //hs->GetYaxis()->SetTitle( varname[0] ) ; 
  
  //TLegend* legend = c1->BuildLegend(0.7,0.65,0.93,0.85);
  TLegend* legend = c1->BuildLegend(0.7,0.65,0.93,0.85);
  legend->SetFillColor(kWhite);
  legend->Draw();
  //legend->SetFillStyle(0);
  c1->SetTitle( selection ) ;
  c1->Update();

  //Create a PDF out of it
  char* basec   = strdup( fnm.Data() ) ; 
  char *fname   = basename(basec);
  TString sname = TString( fname );
  Int_t idot    = sname.First('.') ;
  TString pdfnm = TString(OutDir) + sname(0,idot) + TString(".pdf") ;
  c1->Print( pdfnm ); 

  
  fout->Close() ;  
  delete [] fVar ;
  delete [] fVal ;
  xsorted.clear();
  delete tree ;   //THIS WAS CAUSING MEMORY LEAK. IMPORTANT TO CLEAN!!!!!!
  
  return 1 ;
    
}

//___________________________________________________________________________________


UInt_t SplitNames(const TString &varexp, std::vector<TString> &names)
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

//______________________________________________________________________________

TString CalculateName( vector<TString> varname , TMeas *em ) {
 
  TString tit ;
  if (varname[1](0,1)=="z") {
    //tit=Form("V=%4d V, (x,y)=(%7.4f,%7.4f) mm",(Int_t)em->Vbias,em->x,em->y) ;
    tit=Form("V=%4d V",(Int_t)em->Vbias) ;
  }else if (varname[1](0,1)=="y") {
    tit=Form("V=%4d V, (x,z)=(%7.4f,%7.4f) mm",(Int_t)em->Vbias,em->x,em->z) ;
  }else if (varname[1](0,1)=="x") {
    tit=Form("V=%4d V, (y,z)=(%7.4f,%7.4f) mm",(Int_t)em->Vbias,em->y,em->z) ;
  }else if (varname[1]=="Vbias") {
    tit=Form("(x,y,z)=(%7.4f,%7.4f,%7.4f) mm",em->x,em->y,em->z) ;
  }
  
  return(tit) ;
}

//___________________________________________________________________________________

void ParseChars( TString &tstring  ) 
{
  tstring.ReplaceAll( "==", "EQ" );
  tstring.ReplaceAll( "&&", "AND");
  tstring.ReplaceAll( "||", "OR");
  tstring.ReplaceAll( " " , "_"  );
  tstring.ReplaceAll( "<" , "LT" );
  tstring.ReplaceAll( ">" , "GT" );
  tstring.ReplaceAll( "*" , "x" );
  tstring.ReplaceAll( "/" , "DIV" );
  tstring.ReplaceAll( "%" , "mod" );
  tstring.ReplaceAll( "*" , "x" );
  tstring.ReplaceAll( "(" , "" );
  tstring.ReplaceAll( ")" , "" );
  tstring.ReplaceAll( "Sum$" , "Sum" );
  tstring.ReplaceAll( ":" , "" );
  tstring.ReplaceAll( "TMath" , "" );
  tstring.ReplaceAll( "[" , "" );
  tstring.ReplaceAll( "]" , "" );
  tstring.ReplaceAll( "Iteration$" , "" );
 
}
//___________________________________________________________________________________
#if EXE==2

int main(int argc, char *argv[]) {
   
  //char varexp[]="volt:time" ;
  //char selection[]="z*1000%60==0 && time>90.";
  //char varexp[]="Vmin:z" ;
  //char selection[]="";

  plot1D(argv[1],argv[2],argv[3],argv[4]);
  return (0) ;
  
}

#endif
