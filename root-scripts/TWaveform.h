#ifndef TWAVEFORM_H
#define TWAVEFORM_H

#include <TObject.h>
#include <TH1.h>
#include <TProfile.h>
#include <algorithm>

#include <TFile.h>

#define RTIME 0.9



#include "TMeas.h"

using namespace std ;       //Evita usar std::cin;


class TWaveform : public TObject {
		
  public:
	  	  
          TWaveform() ;
          TWaveform( TProfile *hprof ) ;
          TWaveform( TMeas *em , Int_t &pass ) ;
          TWaveform( TMeas *em , TWaveform *wvavg ) ;
	  TWaveform( TMeas *em , Double_t atleft=0. , Int_t aitleft=0 ) ;
	  ~TWaveform() ;
	  
	  /*------------------- DATA MEMBERS ---------------------*/
          Double_t *volt   ;			            //!
          Double_t *volts  ;			            //!  Smoothed volt vector
          Double_t *time   ;			            //!
	  double   Vbias   ;			            //!

	  /* Histograms */
	  TH1D *hvt    ;                        //! Histo of Voltage vs time
	  TH1D *hbl    ;		        //! Only bline
	  TH1D *hpbl   ;			//! Projected bline (vplot of bline)
	  TH1D *hvtbl  ;			//! Voltage vs time, baseline corrected
	  TH1D *hvts   ;                        //! Histo of Voltage vs time (smoothed)
	  TH1D *hder   ;                        //! Histo of derivative of Voltage vs time
	  
	  double LPower ;                       //Laser power calculated as: Ipd [A]/0.77 [A/W], 
	                                        //where Ipd=Photocurrent is simply Vmax [V]/50 [Ohm]
	  double LNph ;                         //Number of photons injected in photodiode: Int(Vpd*dt)/(50*0.77*Ephoton)
	  
	  int      GetNbins()	 ;  //Ner of time points      //!
	  int      GetPolarity() ;			      //!
	  int      GetaPolarity();			      //!
	  Double_t GetAbsVmax()  ;			      //!
	  double   GetVmax()	 ;			      //!
	  double   GetVmin()	 ;			      //!
	  double   GettVmax()	 ;			      //!
	  double   GettVmin()	 ;			      //!
	  Int_t    GetitVmax()	 ;			      //!
	  Int_t    GetitVmin()	 ;			      //!
	  double   BlineGetMean();			      //!
	  double   BlineGetRMS() ;			      //!
	  double   aBlineGetMean();			      //!
	  double   aBlineGetRMS() ;			      //!
	  double   GetTleft( )   ;			      //!
	  double   GetTright( )  ;			      //!
	  Int_t    GetiTleft( )  ;			      //!
	  Int_t    GetiTright( ) ;			      //!
	  double   GetaTleft( )  ;			      //!
	  double   GetT1st( )    ;			      //!
	  Int_t    GetiT1st( )   ;			      //!
	  double   GetaTright( ) ;			      //!
	  Double_t GetQ50()      ;                            //!
	  Double_t Getcct()      ;                            //!
	  Double_t Getacct()     ;                            //!	
	  Double_t GetCalculatedRiseTime() ;                  //!
	  Double_t GetLPower()   ;                            //!  
	  
	  void     SetTleft( Double_t val ) { tleft=val;}	   ;   //!
	  void     SetTright( Double_t val) { tright=val;}	   ;   //!
	  void     SetBlineMean( Double_t val ) { BlineMean=val;}  ;   //!
	  void     SetBlineRMS( Double_t val) { BlineRMS=val;}     ;   //!
	  
	  void     SetAvgTleft( Double_t val )	   { atleft     =val ;}  ;   //!
	  void     SetAvgTright( Double_t val)	   { atright    =val ;}  ;   //!
	  void     SetAvgiTleft( Int_t ival )	   { aitleft    =ival;}  ;   //!
	  void     SetAvgiTright( Int_t ival)	   { aitright   =ival;}  ;   //!
	  void     SetAvgBlineMean( Double_t val ) { aBlineMean =val ;}  ;   //!
	  void     SetAvgBlineRMS( Double_t val)   { aBlineRMS  =val ;}  ;   //!
	  
	  double   GetTrms()	 ;			      //!
	  double   GetRiseTime( double fraction=0.9 ) ;	      //!
	  Double_t GetCharge( double tinf , double tsup ) ;   //!
	  Double_t GetCCT( double tinf , double tsup ) ;      //! 
	  Double_t RGetCharge( double tinf , double tsup ) ;  //!
          void     CalcRunningCharge( TMeas *em ) ;           //!
          void     CalcSignalDerivative( TMeas *em ) ;        //!
          TProfile* GetAvgProfile() ;                         //!
	  
	  

  private:						     
          int    Nbins       ;  			    //!
	  int    Polarity    ;  			    //!
	  
	  Double_t Vmax      ; 
	  Double_t Vmin      ; 
	  int      iVmax     ; 
	  int      iVmin     ; 
	  Double_t tVmax     ; 
	  Double_t tVmin     ; 
	   
	  double tleft       ; 
	  double tright      ; 
	  double trms        ; 

	  Double_t aVmax     ; 
	  Double_t aVmin     ; 
	  int      aiVmax    ; 
	  int      aiVmin    ; 
	  /* 
	    tright-tleft, where tright is calculated at different thresholds
  	    Value tcoll[0] is the time over a threshold of 5%
	    Value tcoll[1] is the ToT for a 10% threshold
	    Value tcoll[2[=ToT(threshold=15%)
	    ...
	    Value tcoll[8]=ToT(45%)
	    tcoll[0] (overall) can be a bit spiky, and this is probably corrected 
	    in tright-atleft. To calculate tright we start by tcoll[0] and try to refine it
          */
	  double tcoll[9]   ;   
	  
	  /*
	     Variables optimized for timing. Method 1:
	     toa: gives the intersection of a threshold with the first slope of a TCT pulse. Granularity of 5%
	          The slope goes from [tleft,t1st]
             tder,itder Correspond to the maximum of the derivative which happens in [tleft,t1st]
             t1st,it1st Correspond to the point where the derivative is 0, that is, the maximum of the ramp
	  */
	  Double_t t1st     ;    //Time for the first raising peak
	  Int_t    it1st    ;    //Index of t1st
	  Double_t tder     ;    //Time for the maximum of 1st derivative
	  			 //Coincides with middle of RT		 
	  Double_t itder    ;    //Index of tder
	  
	  double toa[21]    ;    //TimeOfArrival: threshold varies in steps of 5%

	  /*
	     Variables for CFD method: 0...18
	     I book 19 values though I divide the wvfs in 20 pieces. The
	     index 0 corresponds to 5 and the index 18 correspondes 95%. We
	     cannot calculate the cfd at 100% of the pulse. 
	  */
	  Double_t atcfd[19]   ;      //Time [ns] at which CFD pulse passes by 0 
	  Double_t aitcfd[19]  ;      //Index of tcfd
	  Double_t athcfd[19]  ;      //Absolute threshold (V) at which tcfd was computed
	  Double_t aslewth[19] ;      //Absolute slope at the crossing point of the threshold
	  Double_t aslewth_cfd[19] ;  //Absolute slope at the crossing point of the threshold in the CFD pulse
	  Double_t cfdval[19]  ;      //Constant Fraction Value [Range: 0...1]        
	  Int_t    aNdelay[19] ;      //Number of bins of delay for the average waveform
	  
	  Double_t tcfd[19]    ;      //Time [ns] at which CFD pulse passes by 0 
	  Double_t itcfd[19]   ;      //Index of tcfd
	  Double_t thcfd[19]   ;      //Absolute threshold (V) at which tcfd was computed
	  Double_t slewth[19]  ;      //Slope at the crossing point of the threshold
	  Double_t slewth_cfd[19]  ;  //Slope at the crossing point of the threshold in the CFD pulse
	  
	  int    itleft      ; 
	  int    itright     ; 
	  int    aitleft     ;   //itleft of the average wvf (!=average of the itlefts)
	  int    aitright    ;   //itright of the average wvf over all the file
	  
	  double BlineMean   ; 
	  double BlineRMS    ; 
	  double aBlineMean  ;   //Average over all the file
	  double aBlineRMS   ;   //Average over all the file
	  
	  double BlineMeanTcoll ; //Bline Mean calculated in [tleft-(tright-tleft),tleft]
	  double BlineRMSTcoll  ; //BlineRMS calculated in [tleft-(tright-tleft),tleft]
	  
	  double atleft      ;   //Average over all the file
	  double atright     ;   //Average over all the file
	  Int_t  aPolarity   ;   //!Average over all the file
	  double at1st       ;   //Position of first peak after tleft
	  Int_t  ait1st      ;   //Index for it1st
	  
	  Double_t Q50       ;   //From [tleft,tright+10 ns]
	  Double_t Qtot      ;   //From [time[0],time[Nt]]
	  Double_t cct       ;   //Using Q(tleft,tright)
	  Double_t acct      ;   //Using Q(atleft,atright)
	  
	  double RiseTime    ;
	  //static TProfile *hvtavg ;   //!
	  
	  
	  void   CalcVmaxmin()  ;						       //!
	  void   CalcPolarity() ;						       //!
	  void   CalcBline( Double_t atleft =0.)    ; 						       //!
	  void   CalcBlineTcoll( Int_t il , Int_t ir )    ; 						       //!
	  double CalcRiseTime( double fraction=RTIME , Int_t itleft = 0, Double_t atleft =0. )  ;	       //!
	  void   SmoothVoltTime() ;						       //!		       //!
	  void   CreateHistos() ;
	  
	  //Timing related methods
	  void   CalcSignalDerivative() ;
	  void   FindFirstPeak( TH1D *hspec , Int_t itl , Int_t itr ) ;
	  void   EmulateCFD( TH1D *h1 , Int_t it1st = 0 , Int_t Average=0 ) ; 
	    
          //The following were protected, classes that inherit from TWaveform can access these methods
    
	  void   CalcSignalTimeL(  double fraction , double &TimeL  , int &iTimeL  )   ;   //!
	  void   CalcLeftmostIntersection( double fraction , TH1D *h1 , double &TimeL , int &iTimeL , Int_t it1st =0) ; //!
	  void   FitSignalTimeL(   double &TimeL   , int    &iTimeL 		  )    ;   //!
	  void   FitSignalTimeR(   double &TimeR   , int    &iTimeR 		  )    ;   //!	
	  void   CalcToT_TimeR(    double atleft   , double &TimeR  , int    &iTimeR ) ;   //!	
	  void   CalcToA( ) ;                                                              //!	

	  void   CalcSignalTimeR(  double fraction , double &TimeR , int &iTimeR )     ;   //!
          void   CalcQTimeR(       double fraction , double &TimeR , int &iTimeR )     ;   //!
	  void   CalcSignalTimeLR( Int_t aitleft = 0 , Double_t atleft = 0.)           ;   //!
	  
	  TH1D*  TProf2TH1D( TProfile *hprof ); //!
	  
  ClassDef(TWaveform,4) ; //ROOT RTTI
	
};

#endif
