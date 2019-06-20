#ifndef TIV_H
#define TIV_H

#include <TObject.h>
#include <TDatime.h>
#include <TString.h>

#include <map> 
#include <iterator>
#include <utility>


using namespace std ;       //Evita usar std::cin;

class TIV: public TObject {

   public: 
     TIV() ;
     ~TIV() ;
     
     Int_t      NIV ;	          // (DNS) Number of steps (IV meas)
     Int_t      Nann ;            // Number of annealing steps	   
     
     TDatime    utc   ;           //  Time in UTC 
     Double_t   lapse ;           //  Total measuring time lapse
     
     Double_t   *Ipad ;           //[NIV]  
     Double_t   *Igr  ;           //[NIV]  
     Double_t   *Itot ;           //[NIV]  
     Double_t   *Vbias ;          //[NIV]  
     Double_t   *AV ;	          //[NIV]  
     //DOI 10.1088/1748-0221/11/09/P09006   
     Double_t   *kbd ;	          //[NIV]  
     Int_t      *entry ;          //[NIV]  
     
     map<pair<TString, TString>, Double_t > SensorList;  // Sensor Area List (see DETECTOR_AREA.txt)
 
     
     Double_t   Compl ;           //  Compliance [A]
   
     Double_t   tann;             //Annealing time [min]
     Double_t   *timeann ;        //[Nann]  
     Double_t   *Tempann ;        //[Nann]  
     Double_t   Fluence ;	  //  Fluence (neq/cm2)

     Double_t   Temp ;            //  Temperature
     TString	who ;  	          //  Who did it?
     TString	sensor ;          //  Sensor name
     TString    bnm ;             //  Basename 
     TString    fnm ;             //  File name 
     TString	bulk ;  	  //  Bulk type (p/n)
     TString	process ;	  //  FZ, CZ, epi...
     TString    irrad ;           //  neutrons, protons ...
     TString	GR  ;		  //  GND, floating
     Double_t   Vbr ;             //  Detection of breakdown start
     Double_t   Vlast ;           //  Voltage at which we stopped/reached compliance
     Double_t   Ilast ;           //  Current at which we stopped/reached compliance

     Int_t      event ;           //  Number of measurement, irrespective of IV or CV
     Int_t      ivent ;           //  Increases only if file is IV
     Int_t      isIV  ;           //  1 if IV, 0 otherwise
     Double_t   SensorArea;       // Sensor area 
     Double_t rho     ;           //  Resistivity
     TString    comment ;        // File comment 
     
     //Methods
     void       SetDimension( Int_t NV ) ; //Declares dimension of a single measurement
     void       SetAnnealingDimension( Int_t numann ); 
     Double_t   GetDetectorArea( TString SensorName   );
     Int_t      GetisIV( )       { return isIV ; } ;
     TString    GetSensorName( ) { return sensor ; } ;
     TString    GetGR( )         { return GR ;} ;
     Double_t   GetFluence()     { return Fluence ;} ;
     Double_t   GetAnnealing()   { return tann ;} ;
     Double_t   GetTemperature() { return Temp ;} ;
     TString    GetUser()        { return who ;} ;
     TDatime    GetDate()        { return utc ;} ;
     TString    GetParticle()    { return irrad;};
     TString    GetMaterial()    { return bulk;};
     TString    GetFilename()    { return fnm;};
     
          
     void       MyClear( ) ;     // Resets (not destroys) the object
     
     
     
     ClassDef(TIV,1)  //IV data class
     
} ;


#endif
