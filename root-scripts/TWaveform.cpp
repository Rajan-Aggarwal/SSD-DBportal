/* 

  Known problems:
  File: 20121108094858_Micron_FZ2852-23_PonN.ZVscan.root
  tleft evaluation using a 5%-25% fit of risetime does not work
    tree->Draw("volt:time","Vbias==10 && event==473","l")
  tleft ends up being:
     1) too big->BlineMean catches signal which does not go to Q50->Q50 drops
     2) or too small (maybe 0) and Q50 has no meaning 

  STRANGE::::: WHY DO WE CHANGE PARAMETER iTimeL in the loop of CalcSignalTimeL ????
*/
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream> //ostringstream

#include "TWaveform.h"
#include <TMath.h>
#include <TVectorD.h>
#include <TGraph.h>
#include <TF1.h>
#include <TGraphSmooth.h>
#include <TText.h>
#include <THStack.h>

#define VMAX -999999.9
#define CHARSZ 256

#define T4BL 8.0  //Time for baseline (5.0 is the standard)

#define SMOOTH 0   //0 No smoothing used
		   //1 Use TH1::Smooth
		   //Use TGraphSmooth class
		   
#define SMVAL 1e9   //Smooth when amplitude<=SMVAL*BlineRMS. If you use a big number (e.g 1e9) it
                    //will smooth all waveforms

#define ToT_PLOTFIT 0  //Show fit of (threshold,ToT) where threshold 5%,10%,15%...
                       //Therefore it is not directly the graph of volt:time
                       //0 do not plot ToT fit. 
                       //1 Plot fits. Open the TBrowser and check when X axis goes to 0

#define TIMING     0  //Do timing calculations
	              //0 no timing. This variable should go into wv->Timing and filled from file_parsers

#define iLGAD      1  //Only needed if TIMING is 1. Possible values:
                      //1 Timing calculated using t1st as peaking time
                      //2 Timing calculated using tVmax as peaking time

#define SAVECFDHISTOS 0 //It should be used for files with a single event to crossckeck

#define DERMETHOD 2  //2 for V(t+h)-V(t)
                     //5 for 5 point stencil
		     
		      
ClassImp(TWaveform)

const int LOW  = -1;
const int HIGH = 15;

//This is a static data member. It persists even if the object is deleted?
//TProfile* TWaveform::hvtavg = 0;
TProfile* hvtavg = 0;

/* Default constructor, see ROOT manual page 270 */

TWaveform::TWaveform(  ) {
  Nbins = 0 ;
  time  = 0 ;
  volt  = 0 ;
  volts = 0 ;
  Vbias = 0.;
  itleft= Nbins ;
  hvt  = 0 ;
  hbl  = 0 ; 
  hpbl = 0 ; 
  hvtbl= 0 ;
  hder = 0 ;
  LPower=0.;
  LNph  =0.;
  aBlineMean = aBlineRMS = 0. ;
  atleft =  atright = 0. ;
  aitleft  = Nbins ;
  aitright = Nbins ;
  aPolarity = 0 ;
  at1st     = 0. ;
  hvts   = 0 ;
  cct = acct = 0. ; 
  for (Int_t i=0 ; i<9 ; i++)  tcoll[i]=0.;
  for (Int_t i=0 ; i<21 ; i++) toa[i]=0.;
}

  
/* 
  First phase: 
  Constructor that fills the TProfile. 
  It DOES NOT CALCULATE ANY FIGURE OF MERIT of the waveform (tleft, tright, etc...)
  It increases variable pass 
*/
TWaveform::TWaveform( TMeas *em , Int_t &pass ) {

   time = new Double_t [em->Nt] ;
   volt = new Double_t [em->Nt] ;
   volts= new Double_t [em->Nt] ;

   for ( int i=0 ; i < em->Nt ; i++ ) {
     time[i]=em->time[i];
     volt[i]=volts[i]=em->volt[i];
   }

   Nbins=em->Nt ;

   Vbias=em->Vbias ;
   
   //Declarar el TProfile como static (no constant) y volcar datos en el?  
   double tmin = TMath::MinElement( Nbins , time ) ;
   double tmax = TMath::MaxElement( Nbins , time ) ;
   double At   = (tmax-tmin)/(Nbins-1.0) ;
   tmin = -0.5*At ;
   tmax = tmin + Nbins*At ;
   
   //Creates this static histogram the first time we enter
   if (pass == 0) {
     hvtavg = new TProfile( "hvtavg","average wvf",Nbins,tmin,tmax ) ;
     hvtavg->SetDirectory(0);
   }
   for (Int_t i=0 ; i<Nbins ; i++) hvtavg->Fill( time[i] , volt[i] ) ; 
   
   pass++ ;

   hvt   = 0 ;
   hbl   = 0 ; 
   hpbl  = 0 ; 
   hvtbl = 0 ;
   hvts  = 0 ;
   hder  = 0 ;
}


/* 
   Second phase:
   Constructor used once TProfile is filled up. 
   It CALCULATES THE FIGURES OF MERIT of the AVERAGE waveform:
      polarity, maxmin, Bline*, tleft, tright,...
 */
TWaveform::TWaveform( TProfile *hprof ) {

   //hprof->Write("Profile.root");
   Nbins = hprof->GetNbinsX();
   
   time = new double [Nbins] ;
   volt = new double [Nbins] ;
   volts= new double [Nbins] ;
   for ( Int_t i=0 ; i < Nbins ; i++ ) {
     time[i] = hprof->GetBinCenter(i+1) ;
     volt[i] = volts[i] = hprof->GetBinContent(i+1)  ;
   }
   
   CalcPolarity () ;

   #if SMOOTH==1 
     SmoothVoltTime( ) ;
   #endif

   CalcVmaxmin( ) ;

   /* 
   Note that we need a rough estimation of itleft to calculate BlineMean. 
   After knowing BlineMean we can properly calculate the left and right extremes of the
   signal, and actually the BlineMean
   
   Taking  itleft=iVmax/2 does not work for very wide pulses (like in N bulk, low Vbias)
   Take 5 ns worth of data
   
   */
   
   if ( Polarity == 1 )   //NEW: Sept 2012
          itleft=( 0<iVmax && iVmax<=Nbins )? TMath::Nint( T4BL/(time[1]-time[0]) ) : Nbins ;
          //itleft = TMath::Nint( T4BL/(time[1]-time[0]) ) ; //I assume that T4BL is better itleft estimation than Nbins!!
   else 
          itleft=( 0<iVmin && iVmin<=Nbins )? TMath::Nint( T4BL/(time[1]-time[0]) ) : Nbins ;
          //itleft = TMath::Nint( T4BL/(time[1]-time[0]) );    
   
   CalcBline( ) ;
   
   CalcSignalTimeLR( ) ;

   //Now that we know where the signal is, calculate the Bline properly
   CalcBline(tleft) ;
         
   atleft     = tleft     ; 
   atright    = tright    ;  
   aitleft    = itleft	  ; 
   aitright   = itright   ;  
   aBlineMean = BlineMean ; 
   aBlineRMS  = BlineRMS  ; 
   aPolarity  = Polarity  ;
   aVmax      = Vmax      ; 
   aVmin      = Vmin      ;
   aiVmax     = iVmax     ; 
   aiVmin     = iVmin     ;
   
   #if TIMING == 1

     //Calculate the position of the 1st peak after tleft
     //First, calculate the derivative of the average profile
     Double_t At   = time[1]-time[0];
     Double_t tmin = time[0]-0.5*At ;
     Double_t tmax = time[Nbins-1]+ 0.5*At ;
     TH1D *hader=new TH1D( "hader" , "Average derivative" , Nbins,tmin,tmax );
     #if DERMETHOD==5
       for (Int_t i = 2 ; i<Nbins-2 ; i++) hader->SetBinContent(i,(-volt[i+2]+8*volt[i+1]-8*volt[i-1]+volt[i-2])/(12.*At));
       hader->SetBinContent(    1    , hader->GetBinContent(2) );
       hader->SetBinContent( Nbins-2 , hader->GetBinContent(2) );
       hader->SetBinContent( Nbins-1 , hader->GetBinContent(2) );
       hader->SetBinContent( Nbins   , hader->GetBinContent(2) );
     #elif DERMETHOD==2
       for (Int_t i = 0 ; i<Nbins-2 ; i++) hader->SetBinContent(i+1,(volt[i+1]-volt[i])/At);
       hader->SetBinContent( Nbins   , hader->GetBinContent(Nbins-1) );
     #endif

     FindFirstPeak( hader , aitleft , itright ) ;
     at1st      = t1st      ;
     ait1st     = it1st      ;

     ifstream f("KCFD.txt");
     if ( f.good() ) {

       /* If file KCFD, it means the delays have been calculated */

	//Read existing KCFD delays
       string line ;
       stringstream myStream(line);
       for (Int_t iloop = 0 ; iloop< 19 ; iloop ++ ) {
	 getline(f, line); myStream.str(""); myStream.clear() ; myStream << line ;
	 myStream >> aNdelay[iloop];
       }
       f.close();
       cout << "" << endl ;
       cout << "-----------> READING KCFD.txt <-----------" << endl ;
       cout << "" << endl ;

     } else {

       /* 
         Calculate the delays which correspond to eack attenuation factor "K" 
         We do that in the *baseline corrected* average profile
       */
              
       TH1D *h1 ;
       h1 = TProf2TH1D( hprof );
       
       //REMOVING THE BASELINE IN THE PROFILE!!!
       Int_t nx = h1->GetNbinsX() ; 
       for (Int_t i=1 ; i<nx ; i++) h1->AddBinContent(i,-BlineMean);  
       #if SAVECFDHISTOS==1
         TFile *fpout=new TFile("profileBL.root","RECREATE"); 
	 h1->Write();
	 fpout->Close();
       #endif
       
       #if iLGAD==2
         EmulateCFD( h1 , aiVmax , 1 ) ; //Threshold moves from 5% to 95% of absolute maximum
       #elif iLGAD==1
         EmulateCFD( h1 , ait1st , 1 ) ;   //Threshold reaches only until 1st iLGAD maxima
       #endif
       
       //Dump delays to disk
       ofstream myfile;
       myfile.open ("KCFD.txt",ios::out);
       for (Int_t i=0 ; i<19 ; i++ ) myfile <<  aNdelay[i] << endl ;
       myfile.close();
       cout << "" << endl ;
       cout << "-----------> CREATED KCFD.txt <-----------" << endl ;
       cout << "" << endl ;

     }
     
     delete hader ;
     
   #endif //TIMING         

   hvt   = 0 ;
   hbl   = 0 ; 
   hpbl  = 0 ; 
   hvtbl = 0 ;
   hvts  = 0 ;
   hder  = 0 ;
   
   
} 


/*
  Third phase: 
  Analyse each event but having information of the average as well

    We calculate PER EVENT:      Polarity, Vmax, Vmin, 
    We profit from averages for: Bline, (i)tleft, (i)tright, tcoll, RiseTime, charge
    (CAUTION: this does not mean that tleft=atleft)

  Note that the tleft and tright are still calculated per event BUT 
  if calculations fail, then we use averages
*/
TWaveform::TWaveform( TMeas *em , TWaveform *wvavg ) {
   
   //Get first some average figures of the waveforms
   atleft     =   tleft   = wvavg->GetTleft()	  ;
   atright    =   tright  = wvavg->GetTright()    ;
   aitleft    =   itleft  = wvavg->GetiTleft()    ;
   aitright   =   itright = wvavg->GetiTright()   ; 
   aBlineMean = BlineMean = wvavg->BlineGetMean() ; 
   aBlineRMS  = BlineRMS  = wvavg->BlineGetRMS()  ; 
   aPolarity  = wvavg->GetPolarity() ;
   aiVmax     = wvavg->GetitVmax() ;
   #if TIMING == 1
     at1st      = wvavg->GetT1st() ;
     ait1st     = wvavg->GetiT1st() ;
     for ( Int_t i=0 ; i<19 ; i++ ) {
       aNdelay[i]     = wvavg->aNdelay[i] ;
       aslewth[i]     = wvavg->aslewth[i] ;
       aslewth_cfd[i] = wvavg->aslewth_cfd[i] ;
       aitcfd[i]      = wvavg->aitcfd[i] ;
     }
   #endif
   
   time = new Double_t [em->Nt] ;
   volt = new Double_t [em->Nt] ;
   volts= new Double_t [em->Nt] ;

   for ( int i=0 ; i < em->Nt ; i++ ) {
     time[i]=em->time[i];
     volt[i]=volts[i]=em->volt[i];
   }

   Nbins=em->Nt ;

   Vbias=em->Vbias ;
 
   CalcPolarity () ;

   #if SMOOTH==1 
     SmoothVoltTime( ) ;
   #endif
   
   CalcVmaxmin( ) ;

   itleft = aitleft ;
      
   CalcBline( atleft  ) ;
   CalcBlineTcoll( itleft , itright ) ;
   
   CalcSignalTimeLR( aitleft , atleft ) ;
   
   //Protection if something goes wrong in tcoll
   for (Int_t i=0;i<9;i++) { 
     if ( tcoll[i]<0. || tcoll[i]>time[Nbins-1]) tcoll[i]=atright-atleft ; //Protection
   }

   CreateHistos() ;

   #if TIMING == 1

     //Calculating first peak after tleft
     CalcSignalDerivative( em ) ;
     FindFirstPeak( hder , itleft , itright ) ;  

     /* UNCOMMENT THIS IF YOU WANT TOA method

	//Calculate the Time Of Arrivale (time at which 
	//leftmost ramp crosses selected threshold)
	CalcToA();   
     */

     //Timing calculation with CFD (please change the EmulateCFD call for the average as well)
     #if iLGAD==2
       EmulateCFD( hvtbl , 0 , 0 ) ;     //kval going up to second maxima of the iLGAD
     #elif iLGAD==1
       EmulateCFD( hvtbl , it1st , 0 ) ; //kval going only up to 1st maxima of the iLGAD
     #endif
     //EmulateCFD( hvtbl , aiVmax , 0 ) ;

   #endif

   CalcRiseTime( RTIME , aitleft , atleft) ;
   
   Q50 = GetCharge( atleft   , atleft+25.  ) ;
   Qtot= GetCharge( time[0] , time[Nbins-1] ) ;
   CalcRunningCharge( em ) ;
   
   //Charge Collection Time [2*At*Q(tinf,tsup)/(Vmaxmin-BlineMean)
   cct  = GetCCT( tleft  , tright )  ;
   acct = GetCCT( atleft , atright ) ;
      
}


//Destructor
TWaveform::~TWaveform(){
  //if (volt != (Double_t *) 0) delete [] volt ; //19Sept2012
  //if (time != (Double_t *) 0) delete [] time ;
  delete [] volt  ; 
  delete [] time  ;
  delete [] volts ; 
  if (hvt) {
    delete hvt   ;  
    delete hbl   ; 
    delete hpbl  ; 
    delete hvtbl ;
    delete hder  ;
    #if SMOOTH>0
      delete hvts  ;  
    #endif
    //delete hvtavg;
  }
}

/*------------------- PUBLIC METHODS -----------------------------*/ 
/*------------------- GETTERS         -----------------------------*/ 
int    TWaveform::GetNbins(){
  return Nbins ;
}

Double_t TWaveform::GetVmax(){
  return Vmax ;
}

Double_t TWaveform::GetVmin(){
  return Vmin ;
}

Double_t TWaveform::GettVmax(){
  return tVmax ;
}

Int_t TWaveform::GetitVmin(){
  return iVmin ;
}

Int_t TWaveform::GetitVmax(){
  return iVmax ;
}

Double_t TWaveform::GettVmin(){
  return tVmin ;
}

Double_t   TWaveform::GetAbsVmax(){
  Double_t val = (fabs(Vmax)>fabs(Vmin)) ? Vmax : Vmin  ;
  return val ;
}

int    TWaveform::GetPolarity(){
  return Polarity ;
}

int    TWaveform::GetaPolarity(){
  return aPolarity ;
}

double TWaveform::GetTleft() { 
  return tleft ; 
}

double TWaveform::GetTright(){ 
  return tright; 
}

double TWaveform::GetaTleft() { 
  return atleft ; 
}

double TWaveform::GetaTright(){ 
  return atright; 
}

Int_t TWaveform::GetiTleft() { 
  return itleft ; 
}

Int_t TWaveform::GetiTright(){ 
  return itright; 
}

double TWaveform::GetTrms()  { 
  return trms  ; 
}

double TWaveform::GetT1st()  { 
  return t1st  ; 
}

Int_t TWaveform::GetiT1st() { 
  return it1st  ; 
}

double TWaveform::BlineGetMean() {
  return BlineMean ;
}

double TWaveform::BlineGetRMS() {
  return BlineRMS ;
}

double TWaveform::aBlineGetMean() {
  return aBlineMean ;
}

double TWaveform::aBlineGetRMS() {
  return aBlineRMS ;
}

Double_t TWaveform::GetQ50()      { return Q50      ; }                            
Double_t TWaveform::Getcct()      { return cct 	    ; }			      
Double_t TWaveform::Getacct()     { return acct     ; }			      
Double_t TWaveform::GetCalculatedRiseTime() { return RiseTime ; }			      
Double_t TWaveform::GetLPower()   { return LPower   ; }			       

double TWaveform::GetRiseTime( double fraction )  { 
  double rt=CalcRiseTime( fraction );
  return rt  ; 
}

Double_t TWaveform::GetCharge(double tinf , double tsup ) {
  if (hvtbl==0) { 
    cout << "No histogram"<<endl;
    exit(0) ;
  }
  int imin=hvtbl->FindBin(tinf);
  int imax=hvtbl->FindBin(tsup);
  //cout << "imin="<<imin<<" imax" << imax << endl ;
  return hvtbl->Integral(imin,imax,"width") ;
}

Double_t TWaveform::GetCCT(double tinf , double tsup ) {

  Double_t Vbl   = ( aPolarity== 1 ) ? Vmax - BlineMean :  Vmin - BlineMean ;
  Double_t val   = 2. * GetCharge(tinf,tsup) / Vbl ;  //Q already multiplied by At

  return val ;
}

Double_t TWaveform::RGetCharge(double tinf , double tsup ) {
//Estudiar como acceder a los datos llamando a este metodo desde root:
//    tree->Draw("RGetCharge(100.,200.)")

  cout << "Nt="<< Nbins <<endl;
//   CreateHistos();
//   cout << "Histos Created" << endl ;
//   int imin=hvtbl->FindBin(tinf);
//   int imax=hvtbl->FindBin(tsup);
//   cout << "imin="<<imin<<" imax" << imax << endl ;
//   return hvtbl->Integral(imin,imax,"width") ;
  return 0.;
}

TProfile* TWaveform::GetAvgProfile() {
   return hvtavg ;
}

/*------------------- PRIVATE METHODS -----------------------------*/

void   TWaveform::CalcVmaxmin() {

  #if SMOOTH ==0
    iVmax=TMath::LocMax(Nbins,volt) ;
    iVmin=TMath::LocMin(Nbins,volt) ;
  #else
    iVmax=TMath::LocMax(Nbins,volts) ;
    iVmin=TMath::LocMin(Nbins,volts) ;
  #endif
  Vmax=volt[iVmax];
  Vmin=volt[iVmin];
  tVmax=time[iVmax];
  tVmin=time[iVmin];

}

#ifdef OLD
void   TWaveform::CalcPolarity(){

  //Gives +1 if signal is positive, -1 otherwise
  if ( Vmax==VMAX ) CalcVmaxmin() ;
  double avx=fabs(Vmax) ;
  double avn=fabs(Vmin) ;
  Polarity = ( avx > avn ) ? 1 : -1 ;
}
#endif

#ifdef OLD1 //Failed for 2014-05-20_09-00-10_Liv-2935-7-1-15-L
void   TWaveform::CalcPolarity(){

  /*Gives +1 if signal is positive, -1 otherwise */
  
  //Estimate baseline
  Int_t nbl=TMath::Nint(  T4BL/(time[1]-time[0]) );
  TVectorD myblv(nbl,volt) ;
  Double_t mybl = myblv.Sum()/nbl ;
  
  TVectorD voltbl(Nbins,volt) ;
  voltbl.AddSomeConstant(-1.0*mybl,voltbl) ;
  
  double sum = voltbl.Sum() ;
  Polarity = ( sum > 0. ) ? 1 : -1 ;
  //cout  << "Polarity" << Polarity << endl;

}
#endif

void   TWaveform::CalcPolarity(){

  /*Gives +1 if signal is positive, -1 otherwise */
    
  //Estimate baseline
  Int_t nbl=TMath::Nint(  T4BL/(time[1]-time[0]) ) ;
  TVectorD myblv(nbl,volt) ;
  Double_t mybl = myblv.Sum()/nbl ;
  
  TVectorD voltbl(Nbins,volt) ;
  voltbl.AddSomeConstant(-1.0*mybl,voltbl) ;
  
  double avx = voltbl.Max( ) ;
  double avn = voltbl.Min( ) ;
  Polarity = ( fabs(avx) > fabs(avn) ) ? 1 : -1 ;
  //cout  << "Polarity" << Polarity << endl;

}


void TWaveform::CalcSignalTimeL( double fraction , double &TimeL , int &iTimeL ) {

  /* Identify entries that are above a fraction of the maximum. Discriminate
     those voltages (bline corrected) above a threshold given as fraction*Vmax.
     If abovethold=0, we have the left extreme of the figure
  */

  Double_t thold , abovethold ;
  thold         = (Polarity==1)? BlineMean+fraction*(Vmax-BlineMean) : BlineMean+fraction*(Vmin-BlineMean) ;
  int iPos      = (Polarity==1)? iVmax : iVmin ;
  Double_t Vamp = (Polarity==1)? Vmax-BlineMean : TMath::Abs(Vmin-BlineMean) ;
  
  #if SMOOTH == 1
    if (Vamp <=SMVAL*BlineRMS) {
      Double_t sVmin = hvts->GetMinimum() ;
      Double_t sVmax = hvts->GetMaximum() ;           
      thold = (Polarity==1)? BlineMean+fraction*(sVmax-BlineMean) : BlineMean+fraction*(sVmin-BlineMean) ;
      iPos  = (Polarity==1)? iVmax : iVmin ;
      Vamp  = (Polarity==1)? sVmax-BlineMean : TMath::Abs(sVmin-BlineMean) ;
    }
  #endif


  for (int i=iPos-1 ; i>iTimeL ; i--) {     //Very few iterations needed

    Double_t volti  = volt[i];
    Double_t voltim = volt[i+1];
    #if SMOOTH==1 
      if ( Vamp <=SMVAL*BlineRMS ) {
	volti  = volts[i];
	voltim = volts[i+1];	  
      }
    #endif

    //Double_t diff=Polarity*(volt[i]-BlineMean-thold) ;
    Double_t diff=Polarity*(volti-thold) ;
    abovethold=( diff>0.)? 1:0 ;
    
    Double_t diffm=Polarity*(voltim-thold) ;
    Double_t abovetholdm=( diffm>0.)? 1:0 ;
    
    if ( abovethold==0 && abovetholdm==1 ) { 
      TimeL = ( i+1 < Nbins && (voltim-volti)!=0. )?
              time[i] + (time[i+1]-time[i])/(voltim-volti)*(thold-volti):
              time[i] ; 
      iTimeL=i;
      //break ;   //Commented 16Oct2017: get the lefmost time that fulfills this condition
    }
  }
  
  //Protection: for instance Pol=-1 but noise in current event sets Pol=1 for this event
  if ( iPos < iTimeL) {
     TimeL  = GetaTleft() ;
     iTimeL = TimeL/(time[1]-time[0]) ;
  } 
    
}

void TWaveform::FitSignalTimeL( double &TimeL , int &iTimeL ) {

  /* Identify entries that are above a fraction of the maximum. Discriminate
     those voltages (bline corrected) above a threshold given as fraction*Vmax.
     If abovethold=0, we have the left extreme of the figure
  */

  Double_t thold , abovethold , abovetholdm ;
  int iPos      = (Polarity==1)? iVmax : iVmin ;
  Double_t Vamp = (Polarity==1)? Vmax-BlineMean : TMath::Abs(Vmin-BlineMean) ;
  thold         = (Polarity==1)? BlineMean+0.25*(Vmax-BlineMean) : BlineMean+0.25*(Vmin-BlineMean) ;
  
  #if SMOOTH == 1
    //Smooth those waveforms where amplitude<=5*N
    if (Vamp <=SMVAL*BlineRMS) {      
      Double_t sVmin = hvts->GetMinimum() ;
      Double_t sVmax = hvts->GetMaximum() ;           
      thold = (Polarity==1)? BlineMean+0.25*(sVmax-BlineMean) : BlineMean+0.25*(sVmin-BlineMean) ;
      iPos  = (Polarity==1)? iVmax : iVmin ;
      Vamp  = (Polarity==1)? sVmax-BlineMean : TMath::Abs(sVmin-BlineMean) ;
    }
  #endif
  
  //Look for the moment we cross the Baseline+25%
  //thold = (Polarity==1)? BlineMean+0.85*(Vmax-BlineMean) : BlineMean+0.85*(Vmin-BlineMean) ;
  for (int i=iPos-1 ; i>0 ; i--) {     //Very few iterations needed

    Double_t volti  = volt[i];
    Double_t voltim = volt[i+1];
    #if SMOOTH==1 
      if ( Vamp <=SMVAL*BlineRMS ) {
	volti  = volts[i];
	voltim = volts[i+1];
      }
    #endif
    Double_t diff=Polarity*(volti-thold) ;  //NEW: 9 Nov 2012
    abovethold=( diff>0.)? 1:0 ;

    Double_t diffm=Polarity*(voltim-thold) ;  //NEW: 9 Nov 2012
    abovetholdm=( diffm>0.)? 1:0 ;
    if ( abovethold==0 && abovetholdm==1 ) { 
      TimeL=time[i] ; 
      iPos=i;
      break ;    //Feb 2019: We enter into this method after knowing the 50% of the signal. We should be close to 25%
    }
  }
  
  //Look for the moment we cross Baseline +5%
  //thold = (Polarity==1)? BlineMean+0.05*(Vmax-BlineMean) : BlineMean+0.05*(Vmin-BlineMean) ;
  thold = (Polarity==1)? BlineMean+0.1*(Vmax-BlineMean) : BlineMean+0.1*(Vmin-BlineMean) ; //2019: Raised value

  //Protection against cases where thold falls below the BlineRMS. DOES IT WORK???
  if (Polarity*thold < BlineRMS) thold = (Polarity==1)? BlineMean+0.15*(Vmax-BlineMean) : BlineMean+0.15*(Vmin-BlineMean);

  #if SMOOTH==1 
    Double_t sVmin = hvts->GetMinimum() ;
    Double_t sVmax = hvts->GetMaximum() ;           
    thold = (Polarity==1)? BlineMean+0.05*(sVmax-BlineMean) : BlineMean+0.05*(sVmin-BlineMean) ;
    //Protection against cases where thold falls below the BlineRMS
    if (Polarity*thold<BlineRMS) thold = (Polarity==1)? BlineMean+0.15*(sVmax-BlineMean) : BlineMean+0.15*(sVmin-BlineMean);
  #endif
  
  
  for (int i=iPos-1 ; i>0 ; i--) {     //Very few iterations needed

    Double_t volti  = volt[i];
    Double_t voltim = volt[i+1];
    #if SMOOTH==1 
      if ( Vamp <=SMVAL*BlineRMS ) {
	volti  = volts[i];
	voltim = volts[i+1];
      }
    #endif
    Double_t diff=Polarity*(volti-thold) ;  //NEW: 9 Nov 2012
    abovethold=( diff>0.)? 1:0 ;

    Double_t diffm=Polarity*(voltim-thold) ;  //NEW: 9 Nov 2012
    abovetholdm=( diffm>0.)? 1:0 ;
    if ( abovethold==0 && abovetholdm==1 ) { 
      TimeL=time[i] ; 
      iTimeL=i;
      break ;
    }
  }
  
  //These should be "no signal" cases. I leave the "naive" estimation as iPos/2 as the BlineMean
  if ( (iTimeL>iPos) || ( iPos-iTimeL+1<3 ) ) {
    //TimeL  = 0.0  ; iTimeL = 0.0  ;
    return ;
  }
  
  //Fit the slope of volt:time from the Baseline to the middle of the Vmax
  TF1 *fpol1 = new TF1("fpol1","pol1",time[iTimeL],time[iPos]);
  TGraph *gr = new TGraph( iPos-iTimeL+1, &time[iTimeL] , &volt[iTimeL] );
  #define LPLOTFIT 0
  #if LPLOTFIT==1
    gr->Fit("fpol1","RQ");
  #else
    gr->Fit("fpol1","RQ0");
  #endif
  Double_t offset=fpol1->GetParameter(0);
  Double_t  slope=fpol1->GetParameter(1);
 
  TimeL  = (slope!=0.)? (BlineMean-offset)/slope : 0.0 ;
  iTimeL = TMath::Nint( (TimeL-time[0])/(time[1]-time[0]) ) ;
  
  #if LPLOTFIT==1
    TGraph *grt = new TGraph( Nbins, &time[0] , &volt[0] );
    grt->Draw("awlp") ; fpol1->Draw("lsame") ;
    grt->Write(); fpol1->Write();
    delete grt;
  #endif

  delete fpol1;
  delete gr;
  
  //cout << "tleft="<<TimeL<<endl;
    
}

void TWaveform::FitSignalTimeR( double &TimeR , int &iTimeR ) {

  /* Identify entries that are above a fraction of the maximum. Discriminate
     those voltages (bline corrected) above a threshold given as fraction*Vmax.
     If abovethold=0, we have the left extreme of the figure
  */

  Double_t thold , abovethold ;
  int iPos = (Polarity==1)? iVmax : iVmin ;
  
  //Look for the moment we cross the Baseline+25% and Baseline +5%
  //thold = (Polarity==1)? BlineMean+0.45*(Vmax-BlineMean) : BlineMean+0.45*(Vmin-BlineMean) ;
  thold = (Polarity==1)? BlineMean+0.5*(Vmax-BlineMean) : BlineMean+0.5*(Vmin-BlineMean) ;
  for (int i=iPos+1 ; i<Nbins ; i++) {     //Very few iterations needed
    //Double_t diff=Polarity*(volt[i]-BlineMean-thold) ;
    Double_t diff=Polarity*(volt[i]-thold) ;  //NEW: 9 Nov 2012
    abovethold=( diff>0.)? 1:0 ;
    if ( abovethold==0 ) { 
      TimeR = ( i-1 >= 0 && (volt[i-1]-volt[i])!=0. )?
              time[i] + (time[i-1]-time[i])/(volt[i-1]-volt[i])*(thold-volt[i]):
              time[i] ; 
      iPos=i;
      break ;
    }
  }
  thold = (Polarity==1)? BlineMean+0.05*(Vmax-BlineMean) : BlineMean+0.05*(Vmin-BlineMean) ;

  //Protection against cases where thold falls below the BlineRMS
  if (Polarity*thold<BlineRMS) thold = (Polarity==1)? BlineMean+0.15*(Vmax-BlineMean) : BlineMean+0.15*(Vmin-BlineMean);

  for (int i=iPos-1 ; i<Nbins ; i++) {     //Very few iterations needed
    //Double_t diff=Polarity*(volt[i]-BlineMean-thold) ;
    Double_t diff=Polarity*(volt[i]-thold) ;  //NEW: 9 Nov 2012
    abovethold=( diff>0.)? 1:0 ;
    if ( abovethold==0 ) { 
      TimeR = ( i-1 >= 0 && (volt[i-1]-volt[i])!=0. )?
              time[i] + (time[i-1]-time[i])/(volt[i-1]-volt[i])*(thold-volt[i]):
              time[i] ; 
      iTimeR=i;
      break ;
    }
  }
  
  //These should be "no signal" cases. I leave the "naive" estimation as iPos/2 as the BlineMean
  if ( (iTimeR<iPos) || ( iTimeR-iPos+1<3 ) || (iTimeR>Nbins-1) ) {
    return ;
  }
  
  //Fit the slope of volt:time from the Baseline to the middle of the Vmax
  TVectorD voltbl(Nbins,volt) ;
  TVectorD timed(Nbins,time) ;
  voltbl.AddSomeConstant(-1.0*BlineMean,voltbl) ;
  TGraph *gr = new TGraph( timed , voltbl );
  TF1 *fpol1 = new TF1("fpol1","pol1",time[iPos],time[iTimeR]);
  
  #define RPLOTFIT 0
  #if RPLOTFIT==1
    gr->Fit("fpol1","RQ");
  #else
    gr->Fit("fpol1","RQ0");
  #endif
  Double_t offset=fpol1->GetParameter(0);
  Double_t  slope=fpol1->GetParameter(1);
 
  TimeR  = (slope!=0.)? (BlineMean-offset)/slope : 0.0 ;
  iTimeR = TMath::Nint( (TimeR-time[0])/(time[1]-time[0]) ) ;
  
  //cout << "tleft="<<TimeL<<endl;

  #if RPLOTFIT==1
    gr->Draw("awlp") ; fpol1->Draw("lsame") ;
    gr->Write(); fpol1->Write();
  #endif
  
  delete fpol1;
  delete gr;
  timed.Clear();
  voltbl.Clear();
    
}

void TWaveform::CalcSignalTimeR( double fraction , double &TimeR , int &iTimeR) {

  /* Identify entries that are above a fraction of the maximum. Discriminate
     those voltages (bline corrected) above a threshold given as fraction*Vmax.
     From iVmax to the right, once abovethold=0, we have the right extreme of the figure
  */

  Double_t thold , abovethold ;
  thold         = (Polarity==1)? BlineMean+fraction*(Vmax-BlineMean) : BlineMean+fraction*(Vmin-BlineMean) ;
  int iPos      = (Polarity==1)? iVmax : iVmin ;
  Double_t Vamp = (Polarity==1)? Vmax-BlineMean : TMath::Abs(Vmin-BlineMean) ;
  
  #if SMOOTH == 1
    //Smooth those waveforms where amplitude<=3*N
    if (Vamp <=SMVAL*BlineRMS) {
      Double_t sVmin = hvts->GetMinimum() ;
      Double_t sVmax = hvts->GetMaximum() ;           
      thold = (Polarity==1)? BlineMean+fraction*(sVmax-BlineMean) : BlineMean+fraction*(sVmin-BlineMean) ;
      iPos  = (Polarity==1)? iVmax : iVmin ;
      Vamp  = (Polarity==1)? sVmax-BlineMean : TMath::Abs(sVmin-BlineMean) ;
    }
  #endif
  
  for (int i=iPos+1 ; i<Nbins ; i++) { //Very few iterations here
    Double_t volti  = volt[i];
    Double_t voltim = volt[i-1];
    #if SMOOTH==1 
      if ( Vamp <=SMVAL*BlineRMS ) {
	volti  =  volts[i];
	voltim =  volts[i-1];
      }
    #endif
   
    //Double_t diff=Polarity*(volt[i]-BlineMean-thold) ;
    Double_t diff=Polarity*(volti-thold) ;
    abovethold=( diff>0.)? 1:0 ;
    if ( abovethold==0 ) { 
      TimeR = ( i-1 >= 0 && (voltim-volti)!=0. )?
              time[i] + (time[i-1]-time[i])/(voltim-volti)*(thold-volti):
              time[i] ; 
      iTimeR=i;
      break ;
    }
    
  }
       
}

void TWaveform::CalcQTimeR(  double fraction , double &TimeR , int &iTimeR ) {

  /* Calculate the total charge of the pulse, then look for the time at which 
     Q=fraction*Qtot. Example: fraction is 98%
     NEW: sometime [for instance simulated cases], the pulses are longer than 25 ns
     Therefore use an estimation of tright of the signal, coming from CalcSignalTimeR, 
     for instance.
  */

 
  //Voltage baseline corrected
  TVectorD voltbl(Nbins,volt) ;
  voltbl.AddSomeConstant(-1*BlineMean,voltbl) ;
    
   //Use only X ns (for instance) to define what Qtotal has to be)
  //Int_t Nb25   = TMath::Nint( 5. / (time[1] - time[0]) ) ;  
  Int_t Nb25   = (iTimeR-itleft+10>0) ? iTimeR-itleft+10 : 2 ;  
  

  Int_t iqlast = ( itleft + Nb25 > Nbins-1 ) ? Nbins-1 : itleft +  Nb25 ;
  Nb25 = iqlast - itleft ;
  TVectorD voltblr( Nb25 ) ; 
  voltblr = voltbl.GetSub( itleft ,  iqlast-1 ) ;

  Double_t Qt = voltblr.Sum() ;
  Double_t Qfrac = fraction * Qt ;  
    
  Double_t Qsum = Qt ;
  Int_t i = Nb25 ;  
  //cout << Nb25 << " " << iqlast << " " << itleft << endl ;
  while ( TMath::Abs(Qsum) > TMath::Abs(Qfrac) ) {
     
    i-- ;
    if ( (Polarity==1 && voltblr[i]>0.) || (Polarity==-1 && voltblr[i]<0.) ) Qsum-=voltblr[i] ;
    //Qsum-=voltblr[i] ;
    if (i==0) break ;
  }
  TimeR=time[itleft + i] ; 
  iTimeR=itleft + i;
  
  voltblr.Clear() ; voltbl.Clear() ;
       
}

void TWaveform::CalcToT_TimeR( double atleft , double &TimeR , int &iTimeR ){
   
   /*
   This method does 2 things:
     1) Calculates the ToT by simply calculating the time over a certain threshold
        ToT[0] is the time the signal lives above 5% of the maximum amplitude
	ToT[1] is the time the signal lives above 10% of the maximum amplitude
        ...
	
     2) These values tcoll[0..8] are used then to fit a function to all of them
        and then tright is estimated as the extrapolation to the baseline
   */
   static Double_t p0 , p1, p2 ;
   static Int_t itime =0;
   Int_t FitStrat ;
   
   /* 
    Vary the threshold and calculate ToT (actually, we do not use tleft, so we actually
    calculate the rightmost limit)
   */
   
   Double_t Signal = (Polarity==1)?(Vmax-BlineMean): TMath::Abs(Vmin-BlineMean);
   //Double_t fStart = 0.25 , fStop= 2.*BlineRMS*BlineRMS*100./Signal ; //This follows tright until it crosses the baseline!
   Double_t fStart = 0.05 , fStop= 0.45 , Delta = 0.05 ; 
   Int_t nToT = 9 , ndf ;
   Double_t *ToT  = new Double_t [nToT] ;
   Double_t *Thr  = new Double_t [nToT] ;
   Int_t itval ;
   Double_t tval ; 
   //cout<<atleft<<" " ;
   for (Int_t i= 0 ;i<nToT;i++) {
     Thr[i]   = fStart + i*Delta ;
     CalcSignalTimeR( Thr[i] , tval , itval ) ;
     ToT[i]   = tval ;
     tcoll[i] = tval ;
     //cout<<ToT[i]<<" " ;
   }
   //cout<<endl;
   
   /* 
   The following contains different fits of the above calculated times
   Goal is to find a good extrapolation to threshold 0.
   
   Fitting strategy:
      F1) Try first a fit to an exponential                             (FitStrat=1,TF1 *fall)
      F2) if not good, exponential with parameters of former iteration  (FitStrat=2, TF1 *fall)
      F3) it not good fit a straight line                               (FitStrat=3, TF1 *fall2)
      F4) if not good take ToT[0] <- Fails if 1st one is bad. 
         Better: filter out ToT's by RMS and fit to pol1
    */

   //F1) FIT TO AN EXPONENTIAL
   
   //TF1 *fall  = new TF1("fall","expo",Thr[0],Thr[nToT-1]); ndf=nToT-2;
   //TF1 *fall  = new TF1("fall","pol1",Thr[0],Thr[nToT-1]); ndf=nToT-2;
   FitStrat=1;
   TF1 *fall , *fall2 ;
   fall  = new TF1("fall","[0]-[1]*(1.-TMath::Exp(-x/[2]))",Thr[0],Thr[nToT-1]) ; ndf=nToT-3;
   fall->SetParameters(ToT[0],ToT[0]-ToT[nToT-1],0.1);
   fall->SetParLimits(0,tleft,time[GetNbins()-1]);
   TGraph *gr = new TGraph( nToT, Thr , ToT );
   #if ToT_PLOTFIT==1
     gr->Fit("fall","RQ");
   #else
     gr->Fit("fall","RQ0");
   #endif
   TimeR = fall->Eval(0.) ; 
  
   //F2) If bad fit, EXPONENTIAL WITH PARAMETERS OF FORMER ITERATION
   Double_t chi2 = fall->GetChisquare() * ndf ;
   if ( chi2 > 1. ) {
     FitStrat=2;
     if (p0!=0) fall->SetParameters(p0,p1,p2);
     else fall->SetParameters(ToT[0],ToT[0]-ToT[nToT-1],0.1);
     fall->SetParLimits(1,0.,1e6);
     #if ToT_PLOTFIT==1
       gr->Fit("fall","BRQ");
     #else
       gr->Fit("fall","BRQ0");
     #endif
     chi2 = fall->GetChisquare() * ndf ;
     TimeR = fall->Eval(0.) ; 
   }

   //Only store if they are good parameters
   if ( chi2 < 1. ) {
     p0=fall->GetParameter(0);
     p1=fall->GetParameter(1);
     p2=fall->GetParameter(2);
   }

   //F3) If bad fit #2, CHANGE THE FITTING FUNCTION
   fall2  = new TF1("fall2","pol1",Thr[0],Thr[nToT-1]); ndf=nToT-2;
   if ( chi2 > 1. ) {

     FitStrat=3;
     #if ToT_PLOTFIT==1
       gr->Fit("fall2","RQ");
     #else
       gr->Fit("fall2","RQ0");
     #endif
     TimeR = fall2->Eval(0.) ;
     chi2 = fall2->GetChisquare() * ndf ;
   }

   //F4) If bad fit #3, filter out points that are far from fall2 fitting function
   TGraph *gr4;
   if ( chi2 > 1. ) {
   
     FitStrat=4;
     
     //Calculate residual of the fit
     Double_t *res =new Double_t [nToT] ;
     for (Int_t i=0;i<nToT;i++) res[i] = ToT[i] - fall2->Eval( Thr[i] ) ;
     Double_t resRMS = TMath::RMS( nToT, res  ) ;

     //Filter out if distance > RMS of residuals
     vector<Double_t> fThr , fToT ;
     for (Int_t i=0;i<nToT;i++) {
       if ( TMath::Abs(res[i]) < resRMS ) {
	  fThr.push_back( Thr[i] );
	  fToT.push_back( ToT[i] );
       }
     }

     //Fit again
     Int_t nfToT = fThr.size() ; 
     gr4 = new TGraph( nfToT , &fThr[0] , &fToT[0] );
     fall->SetParameters( fToT[0] , fToT[0]-fToT[nfToT-1] , 0.1 );
     #if ToT_PLOTFIT==1
       gr4->Fit("fall","RQ");
     #else
       gr4->Fit("fall","RQ0");
     #endif
     TimeR = fall->Eval(0.) ; 
     delete [] res;

   }
    
   #if ToT_PLOTFIT==1
     if   (FitStrat<4) {
        gr->SetNameTitle(Form("event=%d", itime),Form("event=%d", itime)) ;
	gr->Draw("awlp") ;
     } 
     if   (FitStrat==4){
        gr4->SetNameTitle(Form("event=%d", itime),Form("event=%d", itime)) ;
        gr4->Draw("awlp") ; 
     
     }
     itime++;
     if   (FitStrat==3)     fall2->Draw("lsame") ; 
     else		    fall->Draw("lsame") ;

     //TText *t = new TText();
     //TString ttr = Form("tright=%f",TimeR) ;
     //t->DrawTextNDC(0.3,0.21,ttr.Data());
     
     if   (FitStrat<4)  gr->Write(); 
     if   (FitStrat==4) gr4->Draw("awlp") ; 
     //fall->Write();
   #endif

   /*
      PROTECTION AGAINST BAD FITS 
   */
   if ( TimeR<time[0] || TimeR>time[GetNbins()-1] ) TimeR = atright ;   
   Double_t tstep = time[1]-time[0];
   iTimeR = TMath::Nint( TimeR/tstep ) ;
   
   /* 
      Finally calculate ToT by substracting atleft 
   */
   for (Int_t i=0;i<nToT;i++) tcoll[i]-= atleft ;
   
           
   delete gr;
   delete [] ToT ;
   delete [] Thr ;
   delete fall ;delete fall2 ;
   if (FitStrat==4) delete gr4;
   
}

void TWaveform::CalcSignalTimeLR( Int_t aitleft , Double_t atleft  ) {

  /* Identify entries that are above a fraction (normally half) of the maximum. 
     Coming from small times, the 1st time we reach this value, we will be
     in the rising of the pulse. The second time will be the falling edge. In PAW
     I also fitted a P0+G and then extracted the RMS.
  */
  
    //For the RMS of the signal, it is better to study half of the width of the signal
    //This is not calculating tleft and tright, only estimating at FWHM
    CalcSignalTimeL(0.5 , tleft  , itleft  ) ;
    CalcSignalTimeR(0.5 , tright , itright ) ;
    
    //Protection
    if (tright<tleft) {
      tright  = atright;
      itright = aitright;
    }
    trms   = (tright-tleft)/2.35 ;
        
    /* 
    tleft 
    */
    FitSignalTimeL( tleft  , itleft  ) ;
    Double_t tstep = time[1]-time[0];
    itleft  = TMath::Nint(  tleft/tstep ) ;
    
    //Protection against no signal cases
    if (itleft<0 || itleft>Nbins) {
      itleft=aitleft ; tleft=atleft ;
    }
    
    /* 
    tright 
    */
    
    //CalcSignalTimeR(0.1 , tright , itright ) ;
    //CalcQTimeR(0.98 , tright , itright ) ;
    if (atleft>0.) CalcToT_TimeR( atleft , tright , itright ) ;
    else           CalcToT_TimeR( tleft , tright , itright ) ;

    //FitSignalTimeR( tright  , itright  ) ;  //not used
    itright = TMath::Nint( tright/tstep ) ;

    if (itright<0 || itright>Nbins) {
      itright=aitright ; tright=atright ;
    }

  
    //Protection
    if (tright<tleft) {
      tleft   = atleft ; itleft  = aitleft ;
      tright  = atright; itright = aitright;
    }
    
}

void TWaveform::CalcBline( Double_t atleft ) {
  
  int N5ns ;
  if   ( atleft ==0 ) N5ns = TMath::Nint( T4BL/(time[1]-time[0]) );
  else                N5ns = TMath::Nint( 0.95*atleft/(time[1]-time[0]) );
  
  #if SMOOTH == 0
    if ( itleft>0 ) {
      //If the pulses are not monotonous (increasing or decreasing), itleft can be wrong. 
      //If itleft falls in the signal, then BlineMean is wrong. We use BlineMean to correct 
      //many magnitudes.
      BlineMean = ( N5ns <= itleft ) ? TMath::Mean( N5ns , volt ) : TMath::Mean( itleft , volt );
      BlineRMS  = ( N5ns <= itleft ) ? TMath::RMS( N5ns , volt )  : TMath::RMS(  itleft , volt );
    } else {
      BlineMean = TMath::Mean( (int) Nbins , volt );
      BlineRMS  = TMath::RMS(  (int) Nbins , volt );    
    }
  #else
    if ( itleft>0 ) {
      BlineMean = ( N5ns <= itleft ) ? TMath::Mean( N5ns , volts ) : TMath::Mean( itleft , volts );
      BlineRMS  = ( N5ns <= itleft ) ? TMath::RMS( N5ns , volts )  : TMath::RMS(  itleft , volts );
    } else {
      BlineMean = TMath::Mean( (int) Nbins , volts );
      BlineRMS  = TMath::RMS(  (int) Nbins , volts );    
    }
  
  #endif
  //cout << "BlineMean["<<itleft<<"]=" << BlineMean ;
  //cout << " BlineRMS[" <<itleft<<"]=" << BlineRMS << endl ;
  
}

void TWaveform::CalcBlineTcoll( Int_t il , Int_t ir ) {
  
  Int_t nel = ir-il+1 ;
  Double_t *vtemp = new Double_t [nel];
  Int_t ill = il-(ir-il) ;
  for (Int_t i = ill ; i<=il ; i++ ) vtemp[i-ill] = volt[i] ;
  BlineMeanTcoll = TMath::Mean( (int) nel , vtemp ) ;
  BlineRMSTcoll  = TMath::RMS( (int) nel , vtemp ) ;
  delete [] vtemp ;
  
}

double TWaveform::CalcRiseTime( double fraction , Int_t aitleft , Double_t atleft ) {
  double rtl  , rtr  ;
  int    irtl , irtr ;
  irtl = irtr = aitleft ;
  rtl = rtr = atleft ;
  CalcSignalTimeL( 1.0-fraction  , rtl , irtl );
  CalcSignalTimeL( fraction      , rtr , irtr );
  RiseTime = rtr - rtl ;
  return RiseTime ;
}

void TWaveform::SmoothVoltTime( ) {

  static Int_t IsSmoothed ;
  
  //Smoothing is heavy. Skip as much smoothing as possible
  //Only if SNR<SMVAL (see below)
  iVmax=TMath::LocMax(Nbins,volt) ;
  iVmin=TMath::LocMin(Nbins,volt) ;
  Vmax=volt[iVmax];
  Vmin=volt[iVmin];
  Double_t Vamp = (Polarity==1)? Vmax : TMath::Abs(Vmin) ;
  

  //Useful for very noisy/diffusion waveforms
  double tmin = TMath::MinElement( Nbins , time ) ;
  double tmax = TMath::MaxElement( Nbins , time ) ;
  double At   = (tmax-tmin)/(Nbins-1.0) ;
  tmin = -0.5*At ;
  tmax = tmin + Nbins*At ;
  
  //double xpos  = xyz.X() , ypos=xyz.Y(), zpos=xyz.Z() ;
  double xpos  = 0 , ypos=0, zpos=0 ;
  
  char hid[CHARSZ] , htxt[CHARSZ] ;
  
  //Voltage vs time, smoothed version
  sprintf( hid , "Smoothed (%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  sprintf( htxt , "V(t),(%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  hvts=new TH1D( hid , htxt , Nbins , tmin , tmax ) ;
  hvts->FillN(Nbins, time , volt ) ;
  
  if ( Vamp > SMVAL*BlineRMS ) return ;

  
  hvts->Smooth(50) ;
  
  //Dump contents to class member volts
  for (Int_t i=1 ; i<=Nbins ; i++) volts[i-1] = hvts->GetBinContent( i ) ;
  IsSmoothed++;
  cout << "Smoothing waveform " << IsSmoothed << flush <<"\r" ;
  
}

void TWaveform::CreateHistos( ) {

  double tmin = TMath::MinElement( Nbins , time ) ;
  double tmax = TMath::MaxElement( Nbins , time ) ;
  double At   = (tmax-tmin)/(Nbins-1.0) ;
  tmin = -0.5*At ;
  tmax = tmin + Nbins*At ;
  
  //double xpos  = xyz.X() , ypos=xyz.Y(), zpos=xyz.Z() ;
  double xpos  = 0 , ypos=0, zpos=0 ;
  
  char hid[CHARSZ] , htxt[CHARSZ] ;
  
  //Voltage vs time, as seen in the scope
  sprintf( hid , "(%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  sprintf( htxt , "V(t),(%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  hvt=new TH1D( hid , htxt , Nbins , tmin , tmax ) ;
  hvt->FillN(Nbins, time , volt ) ;
  

  //volt[0...itleft] to store the baseline only
  sprintf( hid , "BL (%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  sprintf( htxt , "Baseline,(%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  hbl=new TH1D( hid , htxt , itleft , tmin , tmin + itleft*At ) ; //As long as it starts in bin 1
  hbl->FillN (itleft, time , volt ) ;
  
  //Projected baseline (v/hfill equivalent)
  //http://www.yolinux.com/TUTORIALS/LinuxTutorialC++STL.html
  vector<double> weight(itleft);
  weight.assign(itleft,1.0);
  double bmin=TMath::MinElement( itleft , volt ) ;
  double bmax=TMath::MaxElement( itleft , volt ) ;
  double Ab=0.1*(bmax-bmin) ;
  sprintf( hid , "PBL (%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  sprintf( htxt , "1D Baseline,(%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  hpbl=new TH1D(hid , htxt , 100, bmin-Ab , bmin+Ab );
  hpbl->FillN (itleft, volt , &weight[0] ) ;
  weight.clear() ;

  //Voltage vs time, baseline corrected
  TVectorD voltbl(Nbins,volt) ;
  voltbl.AddSomeConstant(-1*BlineMean,voltbl) ;
  sprintf( hid , "V(t)-bl (%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  sprintf( htxt , "BaselineCorr,(%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  hvtbl=new TH1D( hid , htxt , Nbins , tmin , tmax ) ;
  //hvtbl=(TH1D*)hbl->Clone() ;
  //hvtbl->Reset();
  hvtbl->FillN(Nbins, time , &voltbl[0] );
  
  //Derivative (five point stencil)
  sprintf( hid , "dV/dt,(%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  sprintf( htxt , "Signal derivative,(%f,%f,%f),Vb=%f" ,xpos,ypos,zpos,Vbias ) ;
  hder=new TH1D( hid , htxt , Nbins , tmin , tmax ) ;
  TVectorD dVdt(Nbins) ;
  #if DERMETHOD==5
    for (Int_t i=2;i<Nbins-2;i++)       dVdt[i]=(-volt[i+2]+8*volt[i+1]-8*volt[i-1]+volt[i-2])/(12.*At);
  #elif DERMETHOD==2
    for (Int_t i = 0 ; i<Nbins-1 ; i++) dVdt[i]=(volt[i+1]-volt[i])/At;
  #endif
  hder->FillN(Nbins, time , &dVdt[0] );
    
  //hvt->Write() ; hbl->Write() ; hpbl->Write() ; hvtbl->Write() ;
  
}


void TWaveform::CalcRunningCharge( TMeas *em ) {
  
  //Increasing running charge
   Int_t ir = 1 ;
   em->Qt[0] = volt[0]- BlineMean ;
   while ( ir < Nbins ) { 
     em->Qt[ir]= em->Qt[ir-1] + volt[ir] - BlineMean ;
     ir++;
   }
  
  //Decreasing running charge
  //Int_t ir = Nbins-1 ;
  //em->Qt[Nbins-1] = volt[Nbins-1]- BlineMean ;
  //while ( ir >= 0 ) { 
  //  em->Qt[ir]= em->Qt[ir+1] + volt[ir] - BlineMean ;
  //  ir--;
  //}
  
    
}

void TWaveform::CalcSignalDerivative( TMeas *em ) {
  
   //Using the five points stencil
   em->dVdt[0]       = em->dVdt[1]       = 0. ;
   em->dVdt[Nbins-2] = em->dVdt[Nbins-1] = 0. ;
   Double_t dt = time[1]-time[0];
   #if DERMETHOD==5
     for (Int_t i=2;i<Nbins-2;i++) em->dVdt[i]=(-volt[i+2]+8*volt[i+1]-8*volt[i-1]+volt[i-2])/(12.*dt);
   #elif DERMETHOD==2
     for (Int_t i = 0 ; i<Nbins-2 ; i++) em->dVdt[i]=(volt[i+1]-volt[i])/dt;   
   #endif
    
}

void TWaveform::FindFirstPeak( TH1D *der , Int_t itl , Int_t itr ) {

   /* This method is optimized for iLGADs or, in general, for TCT pulses where the 
      first raising part of the pulse does not lead to the signal maximum. 
      To find the position of the peak, find the position of the maximum of dV/dT
      inmediately after tleft, searching for 0 of the first derivative after tleft
   */
  
   //iMax = position of pulse max
   Double_t dt = time[1]-time[0] ;
   Int_t iMax=itr ;
   
   //Find the maximum of the derivative, This is halfway the maximum
   TH1D *habs =(TH1D *) der->Clone();
   Int_t Ndx = der->GetNbinsX();
   if (Polarity<0) for (Int_t i=1; i < Ndx ; i++ ) habs->SetBinContent( i , TMath::Abs(der->GetBinContent(i)) ) ;
   //TFile *f=new TFile("habs.root","RECREATE") ; habs->Write(); f->Close();
   
   //Maximum of the derivative gives the middle point of the RT
   itder = habs->GetMaximumBin() ;
   tder  = habs->GetBinCenter(itder) ;

/* 
   //Algo1: Time of the maximum: derivative maximum + distance between tleft and maximum
   iMax = habs->GetMaximumBin() + ( habs->GetMaximumBin() - itl );
   t1st  = time[iMax] ;
   it1st = iMax ;
*/   
   //Algo2: Find crossing point by 0
   Int_t ival=itder;
   while ( der->GetBinContent(ival)>0. ) {
     ival++;
     if (ival>=Ndx) break;
   }
   it1st=ival-- ;
   t1st=time[it1st] ;
   
   if ( t1st<time[0] || t1st>time[Nbins-1] ) { 
     t1st  = time[iMax] + 0.5*(time[iMax]-atleft);
     it1st = t1st/dt;
   } 
   delete habs;
   //cout << "t1st="<<t1st<<endl;
   
}

void TWaveform::CalcToA( ){
   
   //Range between [tleft,t1st]
   Double_t dt = time[1]-time[0] ;
   Int_t it1st = TMath::Nint( (t1st-time[0])/dt ) ;
   
   //Protection
   if ( (aitleft==it1st) || (aitleft>it1st) ) {
     for ( Int_t tloop=0;tloop<21;tloop++) toa[tloop]=time[aitleft];
     return ;
   }
   
   //Map the ramp into (vt1,vV1)
   vector<Double_t> vt1 , vV1;
   for (Int_t i = aitleft-1 ; i< it1st+1 ; i++ ) { 
     vt1.push_back( time[i] ) ;
     vV1.push_back( volt[i]-BlineMean ) ;
   }
   Int_t N1st = vV1.size(); 
      
   
   /*  
     Due to noise, the values of vV1 are not monotonically increasing
     Fit slope to line, then find crossing point. Fit from slightly above of tleft to slightly below it
   */
   Double_t tlow = atleft + 0.1*(t1st-atleft) , thigh=tlow + 0.4*(t1st-atleft) ;
   TF1 *f1 = new TF1("MyPol1","[0]+[1]*x", tlow , thigh );
   Double_t ordi = BlineMean ;
   Double_t slope = (hvtbl->GetBinContent( hvtbl->FindBin(thigh) ) - hvtbl->GetBinContent( hvtbl->FindBin(tlow) ))/(thigh-tlow) ;
   f1->SetParameters( ordi , slope ) ;
   hvtbl->Fit( "MyPol1" , "WWRQ0" ) ;
   Double_t p0 = f1->GetParameter(0);
   Double_t p1 = f1->GetParameter(1);
   
   Double_t AV1 = volt[it1st]-volt[aitleft] ;
   for ( Int_t tloop=0;tloop<21;tloop++) {
     Double_t vval = volt[aitleft]-BlineMean + tloop*AV1/20. ;
     toa[tloop] = (vval - p0)/p1 ;
   }
   
   vt1.clear() ; vV1.clear();


}


void TWaveform::EmulateCFD( TH1D *h1 , Int_t it1st , Int_t Average ) {

    /* 
       This method calculates the CFD couple (K,d) [attenuation, delay] 
       Arguments:
         h1: Input histogram
	 it1st : Gives the bin position of the maimum amplitude. Most common values are
	 	 itVmax or otherwise it1st
		 Fill in arrays: acfd, dcfd, thcfd,...
	Average: 1= Incoming pulse is the average pulse. For this one we calculate (K,d)
		 Fill tcfd, thcfd, ...
		
		 0= Use the already calculated (K,d)


           ---> TO BE UPDATED:: INCLUDE NEGATIVE POLARITY PULSES !!!
    */
    
    Int_t    Nbins       = h1->GetNbinsX()     ;
    Double_t OrigSignalMax    ;    
    Int_t    OrigSignalBinMax ; 
    if ( it1st==0 ) {
      OrigSignalMax    = h1->GetMaximum()	;    
      OrigSignalBinMax = h1->GetMaximumBin() ; 
    } else {
      OrigSignalBinMax = it1st ;   
      OrigSignalMax    = h1->GetBinContent(it1st)	;    
    }
    
    #if SAVECFDHISTOS==1
      TFile *fout = new TFile("profs.root","RECREATE");       
    #endif
    Double_t dt = h1->GetBinCenter(2) - h1->GetBinCenter(1) ;
    for ( Int_t kloop=0;kloop<=18 ; kloop++) { //0=5%, 18=95%
    
       //The absolute threhold has to be referred to the BlineMean 
       Double_t frac   = (kloop+1)/20.;
       Double_t kval   = frac*OrigSignalMax ;
       
       //Create inverted and scaled copy of original histo       
       TH1D *hinv = (TH1D*) h1->Clone("hinv") ;
       hinv->Scale( -frac ) ;  
       
       /*
         DELAY:
         If we enter with the average waveform, then calculate interception of ORIGINAL histo 
	 with threshold. 
	 
	 Otherwise use the already calculated average values
       */
       Int_t Ndelay ;
       if ( Average == 1 ) {
	 
	 Int_t itkOrig   = itleft ;
	 Double_t tkOrig = tleft  ;
	 CalcLeftmostIntersection( frac , h1 , tkOrig , itkOrig , it1st ) ; 
         Ndelay = OrigSignalBinMax-itkOrig ;
	 aNdelay[kloop] = Ndelay ;
       } else {
         Ndelay = aNdelay[kloop] ;
       }
       
       //Create the delayed histogram. Delay=tVmax-tkOrig
       TH1D *hdel = (TH1D*) h1->Clone("hdel") ; hdel->Reset();
       for ( Int_t i=1 ; i<=Nbins-Ndelay; i++) 
              hdel->SetBinContent( i+Ndelay , h1->GetBinContent(i) );
       
       //Add inverted and delayed histograms
       TH1D *hcfd = (TH1D *) hinv->Clone("hcfd");
       hcfd->Add( hdel ) ; 
       
       #if SAVECFDHISTOS==1
	 //Save for crosscheck
	 TString ths=Form("hs%d",kloop) , htht=Form("Th%d",kloop);
	 THStack *hs=new THStack(ths.Data(),ths.Data());
	 h1->SetLineColor(1) ; hinv->SetLineColor(2) ; hdel->SetLineColor(6) ;hcfd->SetLineColor(4);
	 hs->Add(h1) ; hs->Add(hinv) ; hs->Add(hdel) ; hs->Add(hcfd) ;
	 TH1D *hth=(TH1D*)h1->Clone(htht); for (Int_t ii=1;ii<=Nbins;ii++) hth->SetBinContent(ii,kval);
	 hs->Add(hth) ;
	 hs->Write();
       #endif
       
       //Find zero crossing of the DELAYED (do we need to stay few more iterations?)
       //Use maximum +/-1.0 ns window range
       Int_t dist , ilow, iup ;
       dist = TMath::Nint(0.5*Ndelay) ;
       ilow = OrigSignalBinMax - dist , iup = OrigSignalBinMax + dist ;
       if (dist<2)       iup  = iup + 5 ;
       if (ilow<=itleft) ilow = itleft + 2 ;
       
       Double_t cval1 = hcfd->GetBinContent( ilow );
       Int_t icrossDel =0 ;
       for ( Int_t i=ilow ; i<=iup; i++ ) {
          Double_t cval2 = hcfd->GetBinContent( i );
          if ( TMath::Sign(1.0,cval1) != TMath::Sign(1.0,cval2) ) {
	    icrossDel = i ;
	    break ;
	  }
	  cval1=cval2;
       }
       
       //If unsuccesful, increase the search range
       if ( icrossDel == 0 ) {
         //dist = TMath::Abs(0.5*(aiVmax-aitleft)) ;
	 ilow = aitleft+2 , iup = aiVmax+(aiVmax-aitleft) ;
	 if (dist<2)       iup  = iup + 5 ;
	 if (ilow<=itleft) ilow = itleft + 2 ;

	 cval1 = hcfd->GetBinContent( ilow );
	 icrossDel =0 ;
	 for ( Int_t i=ilow ; i<=iup; i++ ) {
            Double_t cval2 = hcfd->GetBinContent( i );
            if ( TMath::Sign(1.0,cval1) != TMath::Sign(1.0,cval2) ) {
	      icrossDel = i ;
	      break ;
	    }
	    cval1=cval2;
	 }
         
       }
       
       /*
       Note now:
         1) Bin found refers to histogram. So in the vector it would correspond to 1 bin less !
	 
	    ibin:      time at which the threshold crossed the original histogram
	    icrossDel: time at which the threshold crossed the delayed histogram 
       		       or time at which CFD passes by 0
		       
            Note that ibin = icrossDel - Ndelay.

         2) The crossing happens between bin [icrossDel-1,icrossDel]=[cval1,cval2]
	    Criteria is to take icrossDel-1 as the interception point


       */
       
       /* Protection 1: If icrossDel=0 it means the crossing was not found
          Protection 2: as well arrays or histograms from queries in non existing elements 
	  
	  Note that THE CRITERIA IS TO TAKE THE LOWER BIN FOR THE CROSSING !!! 
       */
       icrossDel  = (icrossDel>3)? icrossDel-1 : 3 ;
       if (icrossDel>Nbins-2) icrossDel = Nbins-2 ;
       
       Int_t ibin = icrossDel - Ndelay ;
       
       if (  ibin<=3  )  ibin = 3 ;  //Protection for histograms and array time[]
       if (ibin>Nbins-2) ibin = Nbins-2 ;
       
       if ( Average==1) {
	 //atcfd[kloop]  = tcrossing ;
	 atcfd[kloop]  = time[ibin-1] ; //-1 here because of histo bin to array index. Note icrossDel&ibin already substracted -1 
	 aitcfd[kloop] = ibin-1 ;       //
	 athcfd[kloop] = kval      ; 
	 #if DERMETHOD==5
	   aslewth[kloop]     = (-h1->GetBinContent(ibin+2)+8.*h1->GetBinContent(ibin+1)-8*h1->GetBinContent(ibin-1)+h1->GetBinContent(ibin-2))/(12.*dt) ;
	   aslewth_cfd[kloop] = (-hcfd->GetBinContent(icrossDel+2)+8.*hcfd->GetBinContent(icrossDel+1)-8*hcfd->GetBinContent(icrossDel-1)+hcfd->GetBinContent(icrossDel-2))/(12.*dt) ;
         #elif DERMETHOD==2
           aslewth[kloop]     = ( h1->GetBinContent(ibin+1)  - h1->GetBinContent(ibin)   )/dt;	 
	   aslewth_cfd[kloop] = ( hcfd->GetBinContent(icrossDel)- hcfd->GetBinContent(icrossDel-1) )/dt;	 
	 #endif

	 //aNdelay was filled above
       } else {
	 tcfd[kloop]   = time[ibin-1] ;  //-1 because of histogram start in bin 1 (icrossDel refers to histogram)
	 itcfd[kloop]  = ibin - 1 ;        //-1 because of histogram start in bin 1
	 thcfd[kloop]  = kval	  ; 
	 #if DERMETHOD==5
	   slewth[kloop]     = (-h1->GetBinContent(ibin+2)+8.*h1->GetBinContent(ibin+1)-8*h1->GetBinContent(ibin-1)+h1->GetBinContent(ibin-2))/(12.*dt) ;
	   slewth_cfd[kloop] = (-hcfd->GetBinContent(icrossDel+2)+8.*hcfd->GetBinContent(icrossDel+1)-8*hcfd->GetBinContent(icrossDel-1)+hcfd->GetBinContent(icrossDel-2))/(12.*dt) ;
         #elif DERMETHOD==2
           slewth[kloop]     = ( h1->GetBinContent(ibin+1)  - h1->GetBinContent(ibin)   )/dt;	
	   slewth_cfd[kloop] = ( hcfd->GetBinContent(icrossDel)- hcfd->GetBinContent(icrossDel-1) )/dt;	
	 #endif
	 cfdval[kloop] = frac      ; 
       }
       
       delete hinv ; delete hdel ; delete hcfd ;
       #if SAVECFDHISTOS==1
         if (kloop ==18) fout->Close();
       #endif
    }
    
    
}

void TWaveform::CalcLeftmostIntersection( double fraction , TH1D *h1 , double &TimeL , int &iTimeL , Int_t it1st ) {

  /* Identify time (to the left of the histogram maximum) for which threshold is crossed.
     ROI is defined by [iTimeL,iVmax]
     If it1st!=0 then the maximum has been tagged because probably we don't aim the full histo maximum

     CHECK POSSIBLE REPLACEMENT BY TH1::FINDFIRSTBINABOVE
  */

  Double_t thold , abovethold , HistoMax , HistoMin  ;
  if ( it1st == 0 ) {
    HistoMax = h1->GetMaximum() ; HistoMin = h1->GetMinimum() ;
  } else {
    HistoMax = HistoMin = h1->GetBinContent( it1st ) ; 
  }  
    
  thold = (Polarity==1)? fraction*HistoMax : fraction*HistoMin ;
  Int_t iLeftLimit = iTimeL ;
  Int_t iPos ;   
  if ( it1st == 0) {
    iPos = (Polarity==1)? h1->GetMaximumBin() : h1->GetMinimumBin() ;
  } else 
    iPos  = it1st ;
  for (int i=iPos-1 ; i>iLeftLimit ; i--) {     //Very few iterations needed

    Double_t volti  = h1->GetBinContent(i), voltim  = h1->GetBinContent(i+1);
    Double_t diff   = volti-thold , diffm = voltim-thold;

    if ( TMath::Sign(1.,diff) != TMath::Sign(1.,diffm) ) {    
      TimeL = time[i] ; 
      iTimeL=i;
      //break;  //Do not escape after the first change of sign
    }
  }
      
}

TH1D* TWaveform::TProf2TH1D( TProfile *hprof ) {

  Int_t Nbins = hprof->GetNbinsX() ;
  TH1D *h1 = new TH1D( "h1" , "" , Nbins , hprof->GetXaxis()->GetXmin() , hprof->GetXaxis()->GetXmax() );
  for ( Int_t i=1;i<=Nbins ; i++) h1->SetBinContent( i , hprof->GetBinContent(i) ) ;
  return h1 ;
}

//Backwards compatibility with plot1D and plot2D
//Do we really need to have access to classes definitions to do 1D and 2D plots.
//Fix this!!!
TWaveform::TWaveform( TMeas *em , Double_t atleft , Int_t aitleft ) {

   time = new Double_t [em->Nt] ;
   volt = new Double_t [em->Nt] ;
   volts= new Double_t [em->Nt] ;

   for ( int i=0 ; i < em->Nt ; i++ ) {
     time[i]=em->time[i];
     volt[i]=volts[i]=em->volt[i];
   }

   Nbins=em->Nt ;

   Vbias=em->Vbias ;

   CalcPolarity () ;

   #if SMOOTH==1 
     SmoothVoltTime( ) ;
   #endif

   CalcVmaxmin( ) ;

 
   /* 
   Note that we need a rough estimation of itleft to calculate BlineMean. 
   After knowing BlineMean we can properly calculate the left and right extremes of the
   signal, and actually the BlineMean

   Taking  itleft=iVmax/2 does not work for very wide pulses (like in N bulk, low Vbias)
   Take 5 ns worth of data

   */

   if ( Polarity == 1 )   //NEW: Sept 2012
          itleft=( 0<iVmax && iVmax<=Nbins )? TMath::Nint( T4BL/(time[1]-time[0]) ) : Nbins ;
          //itleft = TMath::Nint( T4BL/(time[1]-time[0]) ) ; //I assume that T4BL is better itleft estimation than Nbins!!
   else 
          itleft=( 0<iVmin && iVmin<=Nbins )? TMath::Nint( T4BL/(time[1]-time[0]) ) : Nbins ;
          //itleft = TMath::Nint( T4BL/(time[1]-time[0]) );

   //Override estimation if average itleft exists
   if (atleft>0.) itleft = aitleft ; 

   CalcBline( atleft  ) ;

   CalcSignalTimeLR( aitleft , atleft ) ;
   
   CreateHistos() ;

   CalcRiseTime( RTIME , aitleft , atleft) ;

   Q50 = GetCharge( atleft   , atleft+25.  ) ;
   Qtot= GetCharge( time[0] , time[Nbins-1] ) ;

   CalcRunningCharge( em ) ;

}
