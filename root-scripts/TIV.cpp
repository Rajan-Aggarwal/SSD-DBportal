#include "TIV.h"

ClassImp(TIV)

TIV::TIV( ) {
  NIV     = 1 ;
  Nann    = 1 ;
  utc     = TDatime("2000-01-01 00:00:01") ;
  lapse   = 0.; 

  Ipad    = 0 ;
  Igr     = 0 ; 
  Itot    = 0 ; 
  Vbias   = 0 ; 
  AV      = 0 ; 
  kbd     = 0 ; 
  entry   = 0 ; 
  Compl   = 0.;     		

  timeann = 0 ;     		
  Tempann = 0 ;
  
  SensorArea = 0;
  Temp    = -100. ;   		
  who     = TString("unknown") ;       
  sensor  = TString("undefined") ;	      
  bnm     = TString("undefined") ;	      
  fnm     = TString("undefined") ;	      
  bulk    = TString("undefined") ;   
  process = TString("undefined") ; 
  GR	  = TString("undefined") ;	
  Vbr     = 0.;      
  Vlast   = 0.; 
  Ilast   = 0.; 
  event   = 0 ;
  ivent   = 0 ;
  isIV    = 0 ;  
  rho     = 0    ;   
  irrad=TString("") ; 
  comment=TString("") ; 
  
}


TIV::~TIV( ) {

  delete [] Ipad   ;
  delete [] Igr    ;
  delete [] Itot   ;
  delete [] Vbias  ;
  delete [] AV     ;
  delete [] kbd    ;
  delete [] entry  ;
  
  delete [] timeann ;
  delete [] Tempann ;
  
}

void TIV::SetDimension( Int_t NV ) { 

  NIV=NV;
  Ipad  = new Double_t [NIV] ;
  Igr	= new Double_t [NIV] ;
  Itot  = new Double_t [NIV] ;
  Vbias = new Double_t [NIV] ;
  AV	= new Double_t [NIV] ;
  kbd   = new Double_t [NIV] ;
  entry = new Int_t    [NIV] ; 
  
}

void TIV::SetAnnealingDimension( Int_t numan ) {

  Nann = numan;
  timeann = new Double_t [Nann] ; memset( timeann, 0 , Nann*sizeof(*timeann ) );
  Tempann = new Double_t [Nann] ; memset( Tempann, 0 , Nann*sizeof(*Tempann ) );

}

void TIV::MyClear() {

  //Set all array contents to zero
  memset( Ipad  , 0 , NIV*sizeof(*Ipad ) );
  memset( Igr   , 0 , NIV*sizeof(*Igr  ) );
  memset( Itot  , 0 , NIV*sizeof(*Itot ) );
  memset( Vbias , 0 , NIV*sizeof(*Vbias) );
  memset( AV    , 0 , NIV*sizeof(*AV   ) );
  memset( kbd   , 0 , NIV*sizeof(*kbd ) );
  memset( entry , 0 , NIV*sizeof(*entry) ); 
  
  //Null variables
  NIV     = 1  ;  
  utc     = TDatime("2000-01-01 00:00:01") ;
  lapse   = 0. ; 

  Compl   = 0. ;     		
  tann    = 0. ;      		

  Temp    = -100. ;   		
  who     = TString("unknown") ;       
  sensor  = TString("unk") ;	      
  bnm     = TString("undefined") ;	      
  fnm     = TString("undefined") ;	      
  bulk    = TString("undefined") ;   
  process = TString("undefined") ; 
  GR	  = TString("undefined") ;	
  Vbr     = 0. ;       
  Vlast   = 0. ;     
  Ilast   = 0. ;     

  event   = 0  ;  
  ivent   = 0  ;  
  isIV    = 0  ;    

}
/**
 This function returns the area of the detector with name SensorName
 */
Double_t TIV::GetDetectorArea(TString SensorName){
  Double_t Area=0;
  map<pair<TString, TString>, Double_t >::iterator it;
  
  for(it = SensorList.begin(); it!= SensorList.end(); it++){ // Loop for each elements in the list SensorList
    if(SensorName.Contains(it->first.first)){ // Check if the name containd the first key 
      if(it->first.second.Contains("#")){ // Special character # see (DETECTOR_AREA.txt or #define DETECTORFILE)
        Area= it->second;
	break;
      }else if(SensorName.Contains(it->first.second)){ // Check if the name contains the second key
        Area= it->second;
	break;
      }
    }
  }
  
  return Area;
}

