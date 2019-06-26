#ifndef TCTSummary_H
#define TCTSummary_H

#include <TObject.h>
#include <TDirectory.h>
#include "TTree.h"
#include "TMeas.h"
#include "TMeasHeader.h"
#include "TWaveform.h"

class TCTSummary : public TObject {   //TScan derives from TObject, so we can use some of its methods
  
  public:
  
     TCTSummary(   ) ;
     Int_t FillTree( TTree *tsum , char *tfnm , Int_t iev  ) ;
     ~TCTSummary()  ;
     
     TString    fnm  ;
     TString    sensor ;
     
     UShort_t   NV   ;	 	  //Number of voltage scans	   
     UShort_t   Nx   ;	 	  //Number of different x positions		   
     UShort_t   Ny   ;	 	  //Number of different y positions			    
     UShort_t   Nz   ;	 	  //Number of different z positions		    
     Int_t      Ntot ;            //Total number Nx*Ny*Nz*NV (when >0)

     
     TString    comment ;         //File comment
     Double_t   Temp    ;	  //Mean Temperature
     Double_t   Lambda  ;
     Double_t   LPower  ;	  //Mean Pulse Power
     Double_t   Freq    ;	  //Frequency
     Int_t      Nav     ;	  //Ner of averages
     
     UShort_t   Setup   ;	  // 0=OldTCT , 1=eTCT (default), 2=TCT+
                                  // 3=Hamburg, 4=IFCA , 5=TRACS, 6=TPA Bilbao
                                  // 7=WeightField2 , 8=TIFCA , 9=TPA CERN	  
     
     Int_t      Illum   ;         //Illumination 1 = top , 0=side, -1 = bottomW     
			  
     Double_t   dt ;              //Step in time
     Double_t   dx ;              //Step in x
     Double_t   dy ;              //Step in y
     Double_t   dz ;              //Step in z
     //vector<Double_t>   *Vbias ;        //

     TDatime    utc   ;           //Start time in UTC 
     Short_t    aPolarity ;	  //Polarity of the FILE! 		    
          
     Int_t event ;
     
  ClassDef(TCTSummary,1) ; //ROOT RTTI

};

#endif


