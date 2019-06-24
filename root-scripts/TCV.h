#ifndef TCV_H
#define TCV_H

#include "TObject.h"
#include "TDatime.h"
#include "TString.h"

#include "TMath.h"
#include "TTree.h"
#include "TH1.h"
#include "TDirectory.h"
#include "TEntryListArray.h"
#include "TTreeFormula.h"
#include "TEventList.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLegend.h"

#include "TF1.h"
#include "TList.h"
#include "TLatex.h"
#include "TVectorD.h"
#include "TLine.h"

#include "Math/Interpolator.h"
#include "Math/Polynomial.h" 
#include <iostream>
#include "TGraph.h"
#include "TGraphSmooth.h"
#include "TMarker.h"
#include "TGaxis.h"

#include <map> 
#include <iterator>
#include <utility>

//CONSTANTS      ----------------------
#define NXCHAR 512
#define F2PF 1 //Constant to scale picofarads (1 if values already in picofarads)
#define PF2F 1e-12 // Constant to scale from pF to F 
#define BW 15 //Bandwith of smoothing of data for GetVdepCurveRad 15
#define BWD 15 //Bandwith of smoothing of first derivative for GetVdepCurveRad 15 
#define BWDD 10 //Bandwith of smoothing of second derivative for GetVdepCurveRad 10
using namespace std ;       //prevents using std::cin;

class TCV: public TObject {

   public:
     TCV() ;
     ~TCV() ;
     
     map<pair<TString, TString>, Double_t>   SensorList; // Sensor area List
     Int_t   NCV ;	          // Number of voltage steps (CV meas)	
     Int_t   Nann ;               // Number of annealing steps   
     TDatime    utc   ;           //  Time in UTC 
     Double_t   lapse ;           //  Total measuring time lapse		
     Int_t	*entry ;	  //[NCV]  
     Double_t *Vbias     ;	  //[NCV] Voltage provided by K2410 (nominal)
     Double_t *Vsense 	 ; 	  //[NCV] Sensed voltage
     Double_t *Cp        ;        //[NCV] Capacitance in parallel (pF)
     Double_t *Cs        ;        //[NCV] Capacitance in series (pF)
     Double_t *iC2p      ;        //[NCV] 1/C^2 in parallel(pF-2)
     Double_t *iC2s      ;        //[NCV] 1/C^2 in series(pF-2)
     Double_t *Condp     ;        //[NCV] Conductance (pS)
     Double_t *Resistp   ;        //[NCV] Resistance in parallel (Ohm)
     Double_t *Resists   ;        //[NCV] Resistance in series  (Ohm)
     Double_t *Itot      ;        //[NCV] Leakage current (A)
     Double_t *AV 	 ;        //[NCV] Increment in CV measurement (V)      
     Double_t *Ipad      ;        //[NCV] Current power supply (A)
     Double_t *kbd       ;        //[NCV] 
     Double_t *profNeffs ;	  //[NCV] Profile Effective doping concentration in series
     Double_t *profNeffp ;        //[NCV] Profile Effective doping concentration in parallel
     Double_t *diC2p     ;        //[NCV] First derivative of the iC2-Vbias curve 
     Double_t   Compl ;            // Compliance (A)
     Double_t   Cend ;             // End capacitance (pF)   
     Double_t   Fluence ;          // Fluence (neq/cm2)
     TString    irrad ;            // Irradiated with "n", "p", "..."
     Double_t   *timeann ;         //[Nann]  
     Double_t   *Tempann ;         //[Nann] 
     Double_t   tann;
     Double_t   Temp ;             // Temperature
     TString    who ;              // Who did it?
     TString    bulk ;             // Bulk type (p/n)
     TString    process ;          // FZ, CZ, epi...
     TString    DataQuality;        // Message that shows the quality of the data  
     TString    GR  ;	        	   // GND, floating
     Double_t   Vbr ;              // Detection of breakdown start
     Double_t   Vlast ;            // Voltage at which we stopped/reached compliance
     Double_t   Ilast ;            // Current at which we stopped/reached compliance
     Double_t   Freq ;             // Frecuency
     TString    sensor ;           // Sensor name
     TString    fnm ;              // File name
     Double_t   SensorArea ;       // Area of the sensor (mm-2)
     Double_t   rho ;               //Resistivity (ohm.cm)
     // SECTION: FIT
     //Extracted from fits of C(V) or 1/C2(V), etc
     //Examples of intended usage:
     //tree->Draw("Vdep:iCV","iFit==2")
     //tree->Draw("Vdep:name","irrad.Equal("n")==1 && ..."
     //plot2D("name:Fluence:Vdep",...)
     Int_t      NFit ;             //Total number of fits tested
     Int_t      iFit ;             //Type of fit:
				   //1=Estimated from histogram 1/C2  
			           //2=Bilinear fit

     /* For the two linear fits we calculate ranges:
	 [Vstart     , fitPoint1V]
         [fitPoint2V , Vbias[NCV]]
	 The index of fitPoint{1,2}V in the array are: [fitPoint1ii,fitPoint2ii]
	 Then:
	   iC2p[fitPoint1V] = fitPoint1iC2p
	   iC2p[fitPoint2V] = fitPoint2iC2p
     */
     Double_t fitPoint1V    ;  //! 
     Double_t fitPoint2V    ;  //! 
     Double_t fitPoint1iC2p ;  //! 
     Double_t fitPoint2iC2p ;  //! 
     Int_t    fitPoint1ii   ;  //!	
     Int_t    fitPoint2ii   ;  //! 
     Double_t *deriC2p      ;  //[NCV] 
     Double_t *deriC2s      ;  //[NCV] 
     Double_t *wV	    ;  //[NCV]
     Double_t   *Vdeps      ;  //[NFit] Depletion voltage, series model
     Double_t   *Vdepp      ;  //[NFit] Depletion voltage, parallel model
     Double_t	*Neff       ;  //[NFit] Effective doping concentration
     Double_t *thickness    ;  //[NFit]
     Double_t  *IVdepp      ;  //[NFit] Current at depletion voltage in parallel
     Double_t  *IVdeps      ;  //[NFit] Current at depletion voltage in series 
     Int_t     *iIVdepp     ;  //[NFit] 
     Int_t     *iIVdeps	    ;  //[NFit]
     Int_t      event ;  //  Correlative number of measurements
     Int_t      cvent ;  //  Increases only if file is CV
     Int_t      isCV ;             
     TString    comment ; // File comment

     //Methods
     void       SetDimension( Int_t NV ) ;
     void       SetAnnealingDimension( Int_t numann );
     void       ReSetDimension( Int_t New ) ;
     void       GetDataQuality( TCanvas *c1, char SoP) ;
     Int_t      GetisCV( ) { return isCV ; } ;
     TString    GetSensorName( ) { return sensor ; } ;
     TString    GetGR( )         { return GR ;} ;
     Double_t   GetFluence()     { return Fluence ;};
     Double_t   GetAnnealing()   { return tann ;} ;
     Double_t   GetTemperature() { return Temp ;} ;
     TString    GetUser()        { return who ;} ;
     TDatime    GetDate()        { return utc ;} ;
     TString    GetParticle()    { return irrad ;};
     TString    GetMateResistprial()    { return process ;};
     Double_t   GetFrequence()   { return Freq ;} ;
     TString    GetMaterial()    { return bulk ;};
     TString    GetFilename()    { return fnm  ;};
     
     void MyClear( ) ;
     void RotateFit(TCanvas *c, char SoP);
     void GetLinearIntersection( Int_t cvent , TCanvas *c , char SoP);
     Int_t DerivativeFilter(char SoP); 
     Double_t GetDataMode(Int_t length ,vector<Double_t> data );	 
     void GetFitRange( Int_t NCV , Double_t *aVbias , Double_t *aiC2p ) ;
     Double_t GetDetectorArea( TString SensorName );
     void GetVdepCurveRad( TCanvas  *c , char SoP);
     Double_t GetFirstDerivative( Double_t x , TGraph *gy); 
     Double_t GetAsymFirstDerivative(Double_t *x, Double_t *f);    
     ClassDef(TCV,1)  //CV data class
     Double_t GetRotationAngle(vector<Double_t> invC2, vector<Double_t> Voltage);
     Int_t GetMaximumPosition(Double_t *Cp);
     void GetICurrentAboveVdep();
     Double_t SearchICurrentAbove(Double_t Vsearch, Int_t index, char SoP);
     void DrawLeakageCurrent(TCanvas *c);
  
} ;


#endif
