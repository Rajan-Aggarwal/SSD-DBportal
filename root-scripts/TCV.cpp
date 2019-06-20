#include "TCV.h"
  

#define CERR 0.2e-12
#define KBREAK 4.0
#define FIRSTFIT 3
#define EPS0 8.854187817e-14  // F/cm
#define EPSREL 11.6
#define Q0 1.60217657e-19
#define MMtoCM 0.1
#define MM2toCM2 0.01
#define SHOWLines 0

#define GOOD       1 
#define BAD_IV    -1
#define BAD_IC2V  -2
#define BAD_DATA  -3
#define VSCALE     1.1

ClassImp(TCV)

TCV::TCV( ) {

  NCV       = 1 ;
  
  Nann      = 1 ;
  utc       = TDatime("2000-01-01 00:00:01") ;
  
  Vbias     = 0 ;
  Vsense    = 0 ;  
  Cp        = 0 ;
  Cs        = 0 ;  
  iC2p      = 0 ; 
  iC2s      = 0 ;
  Condp     = 0 ;  
  Itot      = 0 ; 
  AV        = 0 ;
  Ipad      = 0 ;
  kbd       = 0 ;
  profNeffs = 0 ;
  profNeffp = 0 ;
  diC2p     = 0 ;
  Resistp   = 0 ;
  Resists   = 0 ;
  
  Compl   = 0 ;
  Cend    = 0 ; 
  Fluence = 0 ;
  irrad   = TString(" ");
  timeann = 0 ;
  Tempann = 0 ;      		

  entry   = 0 ;
  
	
  Temp    = -100. ;   		
  who     = TString("unknown") ;       
  sensor  = TString("undefined") ;	      
  fnm     = TString("undefined") ;	      
  bulk    = TString("undefined") ;   
  process = TString("undefined") ; 
  GR	  = TString("undefined") ;	
  Vbr	  = 0.;	 
  Vlast	  = 0.;     
  Ilast	  = 0.; 
  Freq    = 0.;
  rho = 0;
  SensorArea = 0;    
  NFit    = 3 ;
  iFit    = 0 ;
  IVdepp  =0;
  IVdeps  =0;
  iIVdeps =0;
  iIVdepp =0;
  fitPoint1V    = 0. ;
  fitPoint2V    = 0. ;
  fitPoint1iC2p = 0. ;
  fitPoint2iC2p = 0. ;
  fitPoint1ii	  = 0  ;
  fitPoint2ii	  = 0  ;
  deriC2p	  = 0  ;
  deriC2s	  = 0  ;
  
  Vdeps     = 0 ; 
  Vdepp     = 0 ;
  Neff      = 0 ;
  wV	    = 0 ;
  thickness = 0 ;
  
  event	  = 0 ;  
  cvent	  = 0 ;  
  isCV    = 0 ;   
     
  comment = TString("") ; 
  DataQuality = TString("");
}


TCV::~TCV( ) {
  delete [] Vbias     ;
  delete [] Vsense    ;
  delete [] Cp        ;
  delete [] Cs        ;
  delete [] iC2p      ;
  delete [] iC2s      ;
  delete [] Condp     ;
  delete [] Itot      ;
  delete [] AV        ;
  delete [] Ipad      ;
  delete [] kbd       ;
  delete [] profNeffs ;
  delete [] profNeffp ;
  delete [] deriC2p   ;
  delete [] deriC2s   ;
  delete [] entry     ;
  delete [] wV        ;
  delete [] Vdeps     ;
  delete [] Vdepp     ;
  delete [] IVdepp     ;
  delete [] IVdeps     ;
  delete [] iIVdeps    ;
  delete [] iIVdepp    ;
  delete [] Neff      ;
  delete [] thickness ;
  delete [] diC2p     ;
  delete [] timeann   ;
  delete [] Tempann   ;
  delete [] Resistp   ; 
  delete [] Resists   ;

}


void TCV::SetDimension( Int_t NV ) {

  NCV=NV;
  
  Vbias     = new Double_t [NCV]  ;  memset( Vbias	, 0 , NCV *sizeof( *Vbias     ) ) ; 
  Vsense    = new Double_t [NCV]  ;  memset( Vsense	, 0 , NCV *sizeof( *Vsense    ) ) ; 
  Cp        = new Double_t [NCV]  ;  memset( Cp 	, 0 , NCV *sizeof( *Cp        ) ) ;
  Cs        = new Double_t [NCV]  ;  memset( Cs 	, 0 , NCV *sizeof( *Cs        ) ) ;
  iC2p      = new Double_t [NCV]  ;  memset( iC2p	, 0 , NCV *sizeof( *iC2p      ) ) ;
  iC2s      = new Double_t [NCV]  ;  memset( iC2s	, 0 , NCV *sizeof( *iC2s      ) ) ;
  Condp     = new Double_t [NCV]  ;  memset( Condp	, 0 , NCV *sizeof( *Condp     ) ) ;
  Itot      = new Double_t [NCV]  ;  memset( Itot	, 0 , NCV *sizeof( *Itot      ) ) ;
  AV        = new Double_t [NCV]  ;  memset( AV 	, 0 , NCV *sizeof( *AV        ) ) ;
  Ipad      = new Double_t [NCV]  ;  memset( Ipad	, 0 , NCV *sizeof( *Ipad      ) ) ;
  kbd       = new Double_t [NCV]  ;  memset( kbd	, 0 , NCV *sizeof( *kbd       ) ) ;
  profNeffs = new Double_t [NCV]  ;  memset( profNeffs  , 0 , NCV *sizeof( *profNeffs ) ) ;
  profNeffp = new Double_t [NCV]  ;  memset( profNeffp  , 0 , NCV *sizeof( *profNeffp ) ) ;
  deriC2p   = new Double_t [NCV]  ;  memset( deriC2p    , 0 , NCV *sizeof( *deriC2p   ) ) ;
  deriC2s   = new Double_t [NCV]  ;  memset( deriC2s	, 0 , NCV *sizeof( *deriC2s   ) ) ; 
  entry     = new Int_t    [NCV]  ;  memset( entry	, 0 , NCV *sizeof( *entry     ) ) ; 
  wV	    = new Double_t [NCV]  ;  memset( wV 	, 0 , NCV *sizeof( *wV        ) ) ;
  diC2p     = new Double_t [NCV]  ;  memset( diC2p      , 0 , NCV *sizeof( *diC2p     ) ) ;
  Vdeps     = new Double_t [NFit] ;  memset( Vdeps      , 0 , NFit*sizeof( *Vdeps     ) ) ;  
  Vdepp     = new Double_t [NFit] ;  memset( Vdepp      , 0 , NFit*sizeof( *Vdepp     ) ) ;
  IVdepp    = new Double_t [NFit] ;  memset( IVdepp	, 0 , NFit*sizeof( *IVdepp    ) ) ; 
  IVdeps    = new Double_t [NFit] ;  memset( IVdeps	, 0 , NFit*sizeof( *IVdeps    ) ) ;
  iIVdeps   = new Int_t    [NFit] ;  memset( iIVdeps	, 0 , NFit*sizeof( *iIVdeps   ) ) ; 
  iIVdepp   = new Int_t    [NFit] ;  memset( iIVdepp	, 0 , NFit*sizeof( *iIVdepp   ) ) ; 
  Neff 	    = new Double_t [NFit] ;  memset( Neff       , 0 , NFit*sizeof( *Neff      ) ) ;
  thickness = new Double_t [NFit] ;  memset( thickness  , 0 , NFit*sizeof( *thickness ) ) ;
  Resistp   = new Double_t [NCV]  ;  memset( Resistp    , 0 , NCV *sizeof( *Resistp   ) ) ;
  Resists   = new Double_t [NCV]  ;  memset( Resists    , 0 , NCV *sizeof( *Resists   ) ) ;
	
}


void TCV::SetAnnealingDimension( Int_t numann ) {
  Nann = numann ;
  timeann    = new Double_t [Nann] ; memset( timeann   , 0, Nann*sizeof(*timeann ));
  Tempann    = new Double_t [Nann] ; memset( Tempann   , 0, Nann*sizeof(*Tempann ));  

}

void TCV::ReSetDimension( Int_t New ) {
 
  //Preserves contents
  Vbias      = (Double_t *) realloc ( Vbias     , New*sizeof(Double_t) ) ;
  Vsense     = (Double_t *) realloc ( Vsense    , New*sizeof(Double_t) ) ;
  Cp	     = (Double_t *) realloc ( Cp        , New*sizeof(Double_t) ) ;
  Cs	     = (Double_t *) realloc ( Cs        , New*sizeof(Double_t) ) ;
  iC2p       = (Double_t *) realloc ( iC2p      , New*sizeof(Double_t) ) ;
  iC2s       = (Double_t *) realloc ( iC2s      , New*sizeof(Double_t) ) ;
  Condp      = (Double_t *) realloc ( Condp     , New*sizeof(Double_t) ) ;
  Itot       = (Double_t *) realloc ( Itot      , New*sizeof(Double_t) ) ;
  AV	     = (Double_t *) realloc ( AV        , New*sizeof(Double_t) ) ;
  Ipad       = (Double_t *) realloc ( Ipad      , New*sizeof(Double_t) ) ;
  kbd	     = (Double_t *) realloc ( kbd       , New*sizeof(Double_t) ) ;
  profNeffs  = (Double_t *) realloc ( profNeffs , New*sizeof(Double_t) ) ;
  profNeffp  = (Double_t *) realloc ( profNeffp , New*sizeof(Double_t) ) ;
  deriC2p    = (Double_t *) realloc ( deriC2p   , New*sizeof(Double_t) ) ;
  deriC2s    = (Double_t *) realloc ( deriC2s   , New*sizeof(Double_t) ) ;
  entry      = (Int_t *)    realloc ( entry     , New*sizeof(Int_t) ) ;
  wV	     = (Double_t *) realloc ( wV        , New*sizeof(Double_t) ) ;
  diC2p      = (Double_t *) realloc ( diC2p     , New*sizeof(Double_t) ) ;
  Resistp    = (Double_t *) realloc ( Resistp   , New*sizeof(Double_t) ) ;
  Resists    = (Double_t *) realloc ( Resists   , New*sizeof(Double_t) ) ;
  NCV=New;
  
	
}

void TCV::MyClear( ) {
  memset( Vbias      , 0 , NCV*sizeof(*Vbias)	) ;
  memset( Vsense     , 0 , NCV*sizeof(*Vsense)  ) ;
  memset( Cp         , 0 , NCV*sizeof(*Cp  )    ) ;
  memset( Cs         , 0 , NCV*sizeof(*Cs  )    ) ;
  memset( iC2p       , 0 , NCV*sizeof(*iC2p )	) ;
  memset( iC2s	     , 0 , NCV*sizeof(*iC2p )	) ;
  memset( Condp      , 0 , NCV*sizeof(*Condp)	) ;
  memset( Itot       , 0 , NCV*sizeof(*Itot )	) ;
  memset( AV         , 0 , NCV*sizeof(*AV   )	) ;
  memset( Ipad       , 0 , NCV*sizeof(*Ipad )	) ;
  memset( kbd        , 0 , NCV*sizeof(*kbd  )	) ;
  memset( profNeffs  , 0 , NCV*sizeof(*kbd  )   ) ;
  memset( profNeffp  , 0 , NCV*sizeof(*kbd  )   ) ;
  memset( entry      , 0 , NCV*sizeof(*entry)	) ;
  memset( deriC2p    , 0 , NCV*sizeof(*deriC2p) ) ; 
  memset( deriC2s    , 0 , NCV*sizeof(*deriC2s) ) ; 
  memset( wV	     , 0 , NCV*sizeof(*wV)	) ;
  memset( diC2p      , 0 , NCV*sizeof(*diC2p)   ) ;
  memset( timeann    , 0 , Nann*sizeof(*timeann)) ;
  memset( Tempann    , 0 , Nann*sizeof(*Tempann)) ;
  memset( diC2p      , 0 , NCV*sizeof(*diC2p)   ) ;
  memset( Resistp    , 0 , NCV*sizeof(*Resistp) ) ;
  memset( Resists    , 0 , NCV*sizeof(*Resists) ) ;
  NCV=1 ;	           
  utc           = TDatime( ) ;  	
  Compl         = 0. ;     
  Cend          = 0. ;  	 
  Fluence       = 0. ;   
  irrad         = TString("") ;     
  tann          = 0.;	 
  Temp          = 0. ;  	 
  who           = TString("unknown") ;	 
  bulk          = TString("undefined") ; 
  process       = TString("undefined") ;      
  GR            = TString("undefined") ; 
  Vbr           = 0. ;  	 
  Vlast         = 0. ;     
  Ilast         = 0. ;     
  Freq          = 0. ;  	 
  sensor        = TString("unk" ) ;
  SensorArea    = 0. ;   
  fnm           = TString("undefined" ) ;
  NFit          = 3 ;	      
  iFit          = 0 ;
  fitPoint1V    = 0. ;
  fitPoint2V    = 0. ;
  fitPoint1iC2p = 0. ;
  fitPoint2iC2p = 0. ;
  fitPoint1ii   = 0  ;
  fitPoint2ii   = 0  ;
  memset( Vdeps	    , 0 , NFit*sizeof(*Vdeps)   );
  memset( Vdepp	    , 0 , NFit*sizeof(*Vdepp)   );
  memset( IVdepp    , 0 , NFit*sizeof(*IVdepp)  );
  memset( IVdeps    , 0 , NFit*sizeof(*IVdeps)  );
  memset( iIVdeps   , 0 , NFit*sizeof(*iIVdeps) ); 
  memset( iIVdepp   , 0 , NFit*sizeof(*iIVdepp) ); 
  memset( Neff 	    , 0 , NFit*sizeof(*Neff)    ); 
  memset( thickness , 0 , NFit*sizeof(*thickness) ); 
  event = 0 ;           //  Correlative number of measurements
  cvent = 0 ;           //  Correlative number of measurements
  isCV  = 0 ;            
  comment=TString("") ; // File comment
}

/*--------------------------------------------------------------------------------*/
//Make the iC2p plot versus Vbias, calculate two linear fits before and
//After the kink, which is then calculated from the intersection of both fits.
void TCV::GetLinearIntersection( Int_t cvent , TCanvas *c, char SoP ) {
	vector<Double_t> erriC2p;
	vector<Double_t> errV(NCV);
	Double_t *iC2 , *C; 
	Double_t Volt = 0.;
	iC2 = (SoP=='S')? iC2s : iC2p ;
	C   = (SoP=='S')? Cs  : Cp  ;
	
        /* We want to fit inside ranges:
	   Fit1: [0,fitPoint1V]
	   Fit2: [fitPoint2V,Vbias[NCV-1]] 
	*/
        GetFitRange( NCV , Vsense , iC2 ) ;	
	
	//Skip first few volts if the 1/C2 is not increasing with bias
	Int_t iVstart   = DerivativeFilter(SoP);
	Double_t Vstart = Vsense[iVstart];
	
	if (TMath::Abs(Vstart)>TMath::Abs(fitPoint1V)) Vstart= Vsense[0];


	// Recalculate the mean in case a minimum in iC2p is found. Points with lower voltage than the
	// minimum are neglected so that new ranges for the fit can be calculated.
	//If 1/C2 decreased, recalculate the new mean and RMS
	vector<Double_t>  iC2pstripped , Vstripped ;
	if (Vstart != Vsense[0]) {
		for (Int_t ii=iVstart; ii<NCV; ii++) {     
		  iC2pstripped.push_back(iC2[ii]);
		  Vstripped.push_back(Vsense[ii]);
		}
		GetFitRange( NCV-iVstart+1, &Vstripped[0] , &iC2pstripped[0] ) ;
	}

	//Fit to y=a+bx
	c->cd(1); 

	TF1 *line1=new TF1("line1","[0]+[1]*x",Vstart,fitPoint1V);
	line1->SetParameters(iC2[0], 0.5*(iC2[0]+fitPoint1iC2p));

	TF1 *line2=new TF1("line2","[0]+[1]*x",fitPoint2V,Vsense[NCV-1]);
	line2->SetParameters(fitPoint2iC2p,0.5*(fitPoint2iC2p+Vsense[NCV-1]));

	//First slice fit
	for (Int_t ii=0; ii<NCV; ii++) erriC2p.push_back(2.*CERR/(C[ii]*C[ii]*C[ii]));
	
	vector<Double_t>  erroriC2pnoOutlyers, errorVnoOutlyers, VbiasnoOutlyers, iC2pnoOutlyers;

	//Fills all the points up to the start of the second fit.
	for (Int_t ii= 0; ii<NCV; ii++) {
		if (ii<(fitPoint2ii+2)){
			iC2pnoOutlyers.push_back(iC2[ii]);
			VbiasnoOutlyers.push_back(Vsense[ii]);
			erroriC2pnoOutlyers.push_back(erriC2p[ii]);
			errorVnoOutlyers.push_back(errV[ii]);
		}
		// Get rid of outlyers and points with an abrupt change.
		if (ii>(fitPoint2ii+2)) {
			if (iC2[ii-1]<1.5*iC2[ii] && iC2[ii]<1.5*iC2[ii-2])
				if( 0.7*fitPoint2iC2p<iC2[ii] && kbd[ii]<KBREAK) {
					iC2pnoOutlyers.push_back(iC2[ii]);
					VbiasnoOutlyers.push_back(Vsense[ii]);
					erroriC2pnoOutlyers.push_back(erriC2p[ii]);
					errorVnoOutlyers.push_back(errV[ii]);
			}
		}
	
	}
	
	//Two TGraphErrors, one for calculating the fit with the filterred data 
	//and the other one with the original data and the fit to be plot
	TGraphErrors *giC2p = new TGraphErrors( NCV , Vsense , iC2, &errV[0], &erriC2p[0]) ;
	TGraphErrors *fitiC2p = new TGraphErrors( iC2pnoOutlyers.size(),&VbiasnoOutlyers[0],&iC2pnoOutlyers[0], &errorVnoOutlyers[0], &erroriC2pnoOutlyers[0] ) ;
	giC2p->SetTitle(sensor);
	giC2p->GetXaxis()->SetTitle("Voltage [V]");
	giC2p->GetYaxis()->SetTitle("1/C^{2} [F^{-2}]");
        giC2p->GetYaxis()->SetTitleOffset( 0.78 );
 	giC2p->SetMarkerStyle(20);
	giC2p->SetMarkerSize(0.5);
	giC2p->Draw("alp");
	
	//First slice fit, and calculate coefficients for the line y=a1+b1*x 
	fitiC2p->Fit("line1", "WNRF", "same");
	Double_t a1=line1->GetParameter(0);				
	Double_t b1=line1->GetParameter(1);
	Double_t chisq1=(line1->GetChisquare());

	//Second slice fit, and calculate coefficients for the line y=a2+b2*x 
	fitiC2p->Fit("line2","NRF", "same");
	Double_t a2=line2->GetParameter(0);				
	Double_t b2=line2->GetParameter(1);
	Double_t erra1=line1->GetParError(0);
	Double_t errb1=line1->GetParError(1); 	
 	Double_t erra2=line1->GetParError(0);
	Double_t errb2=line1->GetParError(1);
	if (b2!=b1)	Volt=((a1-a2)/(b2-b1));
	if (b1==b2)	Volt=1234;
	if(SoP=='S') Vdeps[0] = Volt;
	else Vdepp[0] = Volt;

	Double_t chisq2=(line2->GetChisquare());
	Double_t errVdep=TMath::Abs(Volt*TMath::Sqrt(TMath::Power(TMath::Sqrt((erra1*erra1+erra2*erra2))/(a1-a2),2)+TMath::Power(TMath::Sqrt((errb1*errb1+errb2*errb2))/(b1-b2),2)));
	Int_t Year = utc.GetYear();
	Int_t Month = utc.GetMonth();
	Int_t Day = utc.GetDay();
	
	//Paint two lines with the fit coefficients of the filter data so that they can be overimposed to the iC2p-Vbias data.
	TLine *line1paint= new TLine( 0 , a1 , Volt, a1+b1*Volt);
	TLine *line2paint= new TLine( Volt, a2+b2*Volt, Vsense[NCV-1], a2+ b2*Vsense[NCV-1]);
	line1paint->SetLineColor(2);
	line2paint->SetLineColor(2);	
	line1paint->Draw();
	line2paint->Draw();
	
	//Depletion voltage line
	Double_t minY = TMath::MinElement(NCV,iC2);
	TLine *Vdepline= new TLine(Volt,minY,Volt, a1+b1*Volt);
	TLine *minuserrVdep= new TLine(Volt-errVdep,minY,Volt-errVdep, a1+b1*Volt);
	TLine *pluserrVdep= new TLine(Volt+errVdep,minY,Volt+errVdep, a1+b1*Volt);

	Vdepline->SetLineColor(4);
	Vdepline->SetLineStyle(1);	
	Vdepline->Draw("");
	minuserrVdep->SetLineColor(4);
	minuserrVdep->SetLineStyle(3);	
	minuserrVdep->Draw("");
	pluserrVdep->SetLineColor(4);
	pluserrVdep->SetLineStyle(3);	
	pluserrVdep->Draw("");
	
	
	/*Calculate the derivative in all the points where the function is defined without any interpolation needed.
	 For the first and the last element derivative is calculated by the normal Newtonian definition of derivative.
	 For the second and penultimate element, derivative is calculated averaging the normal derivative calculated using
	 the previous point (ii-1) and the one calculated with the following (ii+1).
	 In general, for all points where the interval is symmetric (same voltage step between measurements for all the points used),
	 the Five Point Stencil formula is used for calculating the derivative https://en.wikipedia.org/wiki/Five-point_stencil#See_also 
	 For those points from where Five Point Stencil cannot be used but the previous and the following points are evenly separated,
	 the derivative is calculated as for the second/penultimate element.
	 For those points where the voltage difference to the left than to the right of the point is different, derivative is calculated
	 by applying the usual definition both to the left and the right, and calculating a weighted mean of those values.
	 Note: This last procedure could be improved if uncertainty in the measurements was known.	 
	 */
	TVectorD deriC2;
	for (Int_t ii=0; ii<NCV ; ii++) {
		if (ii==0) deriC2p[ii]=iC2[ii+1]-iC2[ii-1]/(Vsense[ii+1]-Vsense[ii]);
		if (ii==1) deriC2p[ii]=(iC2[ii+1]-iC2[ii-1])/(Vsense[ii+1]-Vsense[ii-1]);
		if (ii>2 &&ii<=NCV-2){
			if (((Vsense[ii+2]-Vsense[ii])==(Vsense[ii]-Vsense[ii-2]) )&&((Vsense[ii+1]-Vsense[ii])==(Vsense[ii]-Vsense[ii-1]))) {
				deriC2p[ii]=(-iC2[ii+2]+8.*iC2[ii+1]-8.*iC2[ii-1]+iC2[ii-2])/12*(Vsense[ii]-Vsense[ii-1]); 
			}if ((Vsense[ii+1]-Vsense[ii]==Vsense[ii]-Vsense[ii-1]) && (Vsense[ii+2]-Vsense[ii] != Vsense[ii]-Vsense[ii-2])) {
				deriC2p[ii]=(iC2[ii+1]-iC2[ii-1])/(Vsense[ii+1]-Vsense[ii-1]);
			}
			if ((Vsense[ii+1]-Vsense[ii]) != (Vsense[ii]-Vsense[ii-1])) {
				Double_t DeltaVbiasLeft  = Vsense[ii]-Vsense[ii-1];
				Double_t DeltaVbiasRight = Vsense[ii+1]-Vsense[ii];
				Double_t deriC2Left  		 = (iC2[ii]-iC2[ii-1])/DeltaVbiasLeft ;
				Double_t deriC2Right 	 	 = (iC2[ii+1]-iC2[ii])/DeltaVbiasRight ;
				deriC2p[ii]= (DeltaVbiasLeft*deriC2Right+DeltaVbiasRight*deriC2Left)/(DeltaVbiasLeft+DeltaVbiasRight);
			}
		}
		if (ii==NCV-1) deriC2p[ii]=(iC2[ii+1]-iC2[ii-1])/(Vsense[ii+1]-Vsense[ii-1]);
		if (ii==NCV) deriC2p[ii]= (iC2[ii]-iC2[ii-1])/(Vsense[ii]-Vsense[ii-1]);
	}
	for (Int_t ii=0; ii<NCV ; ii++) {
		if (Vsense[ii-1]<1.2*Volt && Vsense[ii]>1.2*Volt) Cend=C[ii];
	} 
	Cend=C[fitPoint2ii];
	SensorArea=GetDetectorArea(sensor);


        vector<Double_t> minusVbias;
	ROOT::Math::Interpolator inter(NCV, ROOT::Math::Interpolation::kCSPLINE);
	if (Vsense[1]<Vsense[0]) {
		for (Int_t ii=0; ii<NCV; ii++) minusVbias.push_back(-Vsense[ii]);
		inter.SetData(NCV,&minusVbias[0],iC2);
		Double_t Vstep= minusVbias[1]-minusVbias[0];
		for (Int_t ii=2; ii< NCV; ii++) {
			if (ii==1 || ii==NCV-2) deriC2s[ii]=(inter.Eval(minusVbias[ii]+Vstep)-inter.Eval(minusVbias[ii]-Vstep))/2*Vstep;
			if (ii==0) deriC2s[ii]=(inter.Eval(minusVbias[ii]+Vstep)-inter.Eval(minusVbias[ii]))/Vstep;
			if (ii==NCV-1) deriC2s[ii]=(inter.Eval(minusVbias[ii])-inter.Eval(minusVbias[ii])-Vstep)/Vstep;
			else {
				deriC2s[ii]=(-inter.Eval(minusVbias[ii]+2*Vstep)+8*inter.Eval(minusVbias[ii]+Vstep)
				-8*inter.Eval(minusVbias[ii]-Vstep)+inter.Eval(minusVbias[ii]-2*Vstep))/(12*Vstep);
			}
		}
	}
	if (Vsense[1]>Vsense[0]) {
		inter.SetData(NCV,Vsense,iC2);
		Double_t Vstep= Vsense[1]-Vsense[0];
		for (Int_t ii=0; ii< NCV; ii++) {
			if (ii==1 || ii==NCV-2) deriC2s[ii]=(inter.Eval(Vsense[ii]+Vstep)-inter.Eval(Vsense[ii]-Vstep))/2*Vstep;
			if (ii==0) deriC2s[ii]=(inter.Eval(Vsense[ii]+Vstep)-inter.Eval(Vsense[ii]))/Vstep;
			if (ii==NCV-1) deriC2s[ii]=(inter.Eval(Vsense[ii])-inter.Eval(Vsense[ii])-Vstep)/Vstep;
			else {
				deriC2s[ii]=(-inter.Eval(Vsense[ii]+2*Vstep)+8*inter.Eval(Vsense[ii]+Vstep)
				-8*inter.Eval(Vsense[ii]-Vstep)+inter.Eval(Vsense[ii]-2*Vstep))/(12*Vstep);
			}
		}
	}
	
	
	if (SensorArea==0)  SensorArea=1;
	Neff[0] = (2*Volt*Cend*Cend*PF2F*PF2F)/(Q0*EPS0*EPSREL*TMath::Power(SensorArea,2));
	Neff[1] = 2*PF2F*PF2F/(EPSREL*EPS0*Q0*b1*TMath::Power(SensorArea,2)); //formula of neff with correct units
	
	for (Int_t ii=0; ii<NCV; ii++) { 		
		profNeffp[ii] = 2/(EPS0*EPSREL*Q0*deriC2p[ii]*TMath::Power(SensorArea,2));
		profNeffs[ii] = 2/(EPS0*EPSREL*Q0*deriC2s[ii]*TMath::Power(SensorArea,2));
		wV[ii]	=	(EPSREL*EPS0*SensorArea)/(C[ii]);
	}
	thickness[0] = (EPSREL*EPS0*SensorArea)/(Cend);
	thickness[1] = (EPSREL*EPS0*SensorArea)/(Cend);
	
	Double_t positive= 1.0, posx = 0.3;
	if(Vsense[GetMaximumPosition(iC2)]<0) positive = -1.0;
	if(positive>0) posx = 0.7;  // Display the legend on the right if the voltage is positive and on left if it is negative
	
	//Legend
	TLatex latex;
	latex.SetNDC();
	latex.SetTextSize(0.025);
        latex.SetTextAlign(12);
	latex.DrawLatex(posx,0.45,Form("Vdep=%4.1f#pm%4.1f V",Volt,errVdep));
	latex.DrawLatex(posx,0.4, Form("Cend=%4.3e pF",Cend));

	latex.DrawLatex(posx,0.35,Form("T=%4.1f C",Temp));
	latex.DrawLatex(posx,0.3 ,Form("%d/%d/%d",Day,Month,Year));
	latex.DrawLatex(posx,0.25,Form("#chi^{2}_{1}= %4.3e",chisq1));
	latex.DrawLatex(posx,0.2 ,Form("#chi^{2}_{2}= %4.3e",chisq2));
	latex.DrawLatex(posx,0.15 ,Form("N_{eff}= %4.3e",Neff[0]));
	latex.DrawLatex(0.15,0.85 ,fnm);


	cout<<"Fit1: ["<<Vstart<<","<<fitPoint1V<<"] fp2v="<<fitPoint2V<<" startFit = "<<iVstart<<endl;
	cout<<"fp1="<<fitPoint1iC2p<<" fp2="<<fitPoint2iC2p<<endl;
	cout<<" Volt="<<Volt<<"+-"<<errVdep<<endl;
	cout<<" Neff[0]="<<Neff[0]<<" cm-3, Neff[1]= "<<Neff[1]<<" cm-3, for area(mm2)="<<SensorArea<<endl;
	cout<<" Thickness[0]="<<thickness[0]<<" cm-3, Thickness= "<< thickness[1]<<" cm-3"<<endl;
	if (SensorArea==1) {
		cout<<" area of the sensor is 1. Update information"<<endl;
	}
	cout<<"------------------------------------------------------------------------"<<endl;
}				

/*---------------------------------------------------------------------------------------------*/

//Filters the data by taking the 5 point derivative of iC2p, checks if a minimum is found
//If the minimum is found, it is returned the position where it was found.
Int_t TCV::DerivativeFilter(char SoP) {
	TVectorD deriC2; Double_t *iC2;
	iC2 = (SoP=='S')? iC2s : iC2p;
	
	deriC2.ResizeTo(NCV);
	deriC2[0]=0. ; deriC2[1]=iC2[1]-iC2[0]; 
	Int_t startFit=0;
	for (Int_t ii=2; ii<fitPoint2ii; ii++) {
    //Five Point Stencil https://en.wikipedia.org/wiki/Five-point_stencil#See_also
		deriC2[ii]=(-iC2[ii+2]+8.*iC2[ii+1]-8.*iC2[ii-1]+iC2[ii-2])/12.; 
		if (ii>2 && deriC2[ii-1]<0. && deriC2[ii]>0.) startFit=ii;

	}

  	deriC2.Clear(); 
	return startFit;
}

/*-------------------------------------------------------------------------*/

void TCV::GetFitRange( Int_t size , Double_t *Vsense , Double_t *iC2p ) {

  /*  First fit range extends up to Vbias=fitPoint1V. Indeed, iC2p(fitPoint1V)=fitPoint1iC2p
	    The second fit extends from [fitPoint2V,...]
	*/
     if(size>1){	
	vector<Double_t> iC2p_noBD , Vbias_noBD ;
	for (Int_t ii=0 ; ii< size ; ii++) {
	//  if (kbd[ii]<KBREAK) { 
		  iC2p_noBD.push_back( iC2p[ii] );
		  Vbias_noBD.push_back( Vsense[ii] );
	 // }
	}
	Int_t NCV_noBD = iC2p_noBD.size();
        Double_t meaniC2p , rmsiC2p, medianiC2p , modeiC2p, modeV;
        meaniC2p   = TMath::Mean(    NCV_noBD , &iC2p_noBD[0] ) ;
        rmsiC2p    = TMath::RMS(     NCV_noBD , &iC2p_noBD[0] ) ;
        medianiC2p = TMath::Median(  NCV_noBD , &iC2p_noBD[0] ) ;
        modeiC2p = GetDataMode(NCV_noBD,iC2p_noBD);
  

	fitPoint1iC2p = modeiC2p-rmsiC2p ;
	fitPoint2iC2p = modeiC2p-rmsiC2p*.15;
	

	//Loops over the elements of iC2p and looks for the ii element where the fitPointiC2p is found.
	for (Int_t ii=1; ii<size; ii++) {
		if ( (iC2p[ii-1] < fitPoint1iC2p) && (fitPoint1iC2p < iC2p[ii]) ) {
			if (ii>2){  
			  fitPoint1V = Vsense[ii];
			  fitPoint1ii = ii;
			  break;
			}
			//if (ii==2) fitPoint1V = Vbias[ii+1];
			//else fitPoint1V=Vbias[ii+2];
		}
	}
	
	for (Int_t ii=fitPoint1ii; ii<size; ii++) {
	  if ( (iC2p[ii-1]  < fitPoint2iC2p) && (fitPoint2iC2p < iC2p[ii]) ) {
	    if (ii>2) { 
	    fitPoint2V=Vsense[ii];
	    fitPoint2ii=ii+2;
	    break;
	  }
		   
	    if (ii==2) {
	      fitPoint2V=Vsense[ii+1];
	      fitPoint2ii=ii+1;
	    }
	  }
	//Crosscheck if selection makes sense
	  if ( (iC2p[ii-1] < modeiC2p) && (modeiC2p < iC2p[ii])) modeV=Vsense[ii];
	}

	cout<<" mode="<<modeV<<" meaniC2p="<<meaniC2p<<" median="<<medianiC2p<<" rms="<<rmsiC2p/meaniC2p<<NCV<<endl;
    }
}

/*----------------------------------------------------------------------------------------------*/				
void TCV::GetDataQuality(TCanvas *c, char SoP){
   Int_t quality=GOOD, oscilation = 0, Thereshold = 10;
   Double_t p1, p2;
   Double_t *iC2 = (SoP=='S')? iC2s: iC2p;
   for(Int_t i=0; i<NCV; i++){
     if(1) {quality=BAD_DATA; break;}
   }
   
   if(quality==BAD_DATA) DataQuality=DataQuality+"Bad data set\n";
   
   for(Int_t i=0; i<NCV-1; i++){
     p1= (iC2[i]-iC2[i-1])/(Vsense[i]-Vsense[i-1]);
     p2= (iC2[i+1]-iC2[i])/(Vsense[i+1]-Vsense[i]);
     if(p2>0 && p1<0) oscilation++;
     if(oscilation>Thereshold) {
       quality=BAD_IC2V;
       oscilation = 0;
       break;
     }
   }
   if(quality==BAD_IC2V) DataQuality=DataQuality+"Bad CV data\n";
   
   for(Int_t i=1; i<NCV-1; i++){
     p1=(Itot[i]-Itot[i-1])/(Vsense[i]-Vsense[i-1]);
     p2=(Itot[i+1]-Itot[i])/(Vsense[i+1]-Vsense[i]);
     if(p2>0 && p1<0) oscilation++;
     if(oscilation>Thereshold) {
       quality=BAD_IV; 
       oscilation = 0;
       break;
     }  
   }
   
   if(quality==BAD_IV) DataQuality=DataQuality+"Bad IV data\n";
   if(quality==GOOD) DataQuality=DataQuality+"Good Data";

   
  // Plot the Data quality message 
  c->cd(1);
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.025);
  latex.SetTextAlign(12);
  latex.DrawLatex(0.15,0.35,DataQuality);
}

/*---------------------------------------------------------------------------------------------------------------*/
/**
 This method returns the most common value (mode) of a given distribution of numbers  
 */
Double_t TCV::GetDataMode( Int_t length , vector<Double_t> data ){
 Double_t mode, max, min, thereshold;
 Int_t    Nbins ;
 
 // Calculate the parameters to create a histogram
 max=*max_element(data.begin(),data.end()); 
 min=*min_element(data.begin(),data.end());
 thereshold= min+0.2*(max-min); // Thereshold to avoid the closest values to 0 capacitance.
 Nbins=40; // Number of bins in the histogram
 
 
 TH1* h = new TH1D("h","h",Nbins,min,max); // Create a histogram
 
 for (Int_t i =0; i<length ; i++) if(data[i]>thereshold) h->Fill(data[i],1.) ; // Fill the histogram with the data 

 Int_t ibin=h->GetMaximumBin(); // Get the bin numer with more entries 
 mode = h->GetBinCenter(ibin); // Get the value inside the bin with more entries 
 
 h->Delete();
 
 return mode;
}		

/*-------------------------------------------------------------------------*/
Double_t TCV::GetFirstDerivative( Double_t x , TGraph *gy ) {       
   //f'(x)=[-f(x+2h)+8f(x+h)-8f(x-h)+f(x-2h)]/12h 
   
   Int_t    Nx   = gy->GetN() ;
   Double_t xmax = gy->GetXaxis()->GetXmax() , xmin = gy->GetXaxis()->GetXmin();
   Double_t h    = (xmax-xmin)/(2.*Nx) ;
   Double_t yp   = (-gy->Eval(x+2.*h,0,"S")+8.*gy->Eval(x+h,0,"S")-8.*gy->Eval(x-h,0,"S")+gy->Eval(x-2.*h,0,"S"))/(12.*h);
   
   return yp ; 
}

Double_t TCV::GetAsymFirstDerivative(Double_t *x, Double_t *f){
  Double_t dfx, h1, h2;
  h1=x[2]-x[1]; h2=x[1]-x[0];
  dfx=f[2]*h2/h1-f[0]*h1/h2+f[1]*(h1-h2)/h1/h2;
  
  return dfx;
}

/*-----------------------------------------------------------------------*/
Double_t TCV::GetDetectorArea(TString SensorName){
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

/*-------------------------------------------------------------------------------------------------------*/
/**
  This method calculates the depleteion voltage rotating the iC2-Vbias data, then fit the date to a smoot curve and finds 
  the maximum  or the minimum of the rotated curve. 
 */
void TCV::RotateFit(TCanvas *c, char SoP){
   Double_t theta, cosine, sine, Volt1, Volt2;
   vector<Double_t> C2temp, Vtemp, x, y;
   bool Thereshold = (NCV<=30);
   Double_t *iC2;
   iC2 = (SoP=='S')? iC2s : iC2p;
   
   Double_t positive = 1.0;
   if(Vsense[GetMaximumPosition(iC2)]<0) positive=-1.0; // Check if the voltage is positive or negative 
   TGraph *graph = new TGraph( NCV ,Vsense, iC2); // Fit the graph to a smooth curve
   TGraphSmooth *smooth = new TGraphSmooth("normal");
   TGraph *curve = smooth->SmoothLowess(graph,"",0.1);//smooth->SmoothLowess(graph,"",0.1);
   
   Int_t start= 0;//DerivativeFilter();  // Filter if there is a minimum at the curve
   for (Int_t ii=1; ii<NCV; ii++) {
     if (ii>2 && diC2p[ii-1]<0. && diC2p[ii]>0.){ start=ii; break;}
     if(Vsense[ii]>(Vsense[NCV-1]/6.) || iC2[ii]>(iC2[NCV-1]/6.)) break; 
   }
   
   for(Int_t k=start; k<NCV; k++){ // Fill the temporary vectors 
     Vtemp.push_back(positive*Vsense[k]);
     if(Thereshold)C2temp.push_back(iC2[k]);
     else C2temp.push_back(curve->Eval(Vsense[k]));   
   }
   
  theta = GetRotationAngle(C2temp, Vtemp);   
  cosine = TMath::Cos(theta); // components of the rotation matrix cos(theta) and sin(theta)
  sine = TMath::Sin(theta);
  
  Int_t length = Vtemp.size();
 
  // rotate the whole data set by the angle theta
  for (Int_t i=0; i<length ; i++){ 
    x.push_back(cosine*Vtemp[i]-sine*C2temp[i]); 
    y.push_back(sine*Vtemp[i]+cosine*C2temp[i]);
  }
  
  // Find the minimum of the curve and when it is found rotate again the point to obtain the depletion voltage and break the loop
  for(Int_t j=2 ; j<length-4; j++){    
    if((y[j]-y[j-1])/(x[j]-x[j-1])>0 && (y[j+1]-y[j])/(x[j+1]-x[j])<0){ 
       Volt1=cosine*x[j]+sine*y[j]; 
       break;
    }
  }
  
  // Find the minimum of the curve but starts backwards and obtains the depletion voltage 
  for(Int_t j=length-1 ; j>2; j--){ 
    if((y[j-1]-y[j-2])/(x[j-1]-x[j-2])>0 && (y[j]-y[j-1])/(x[j]-x[j-1])<0){
      Volt2=cosine*x[j-1]+sine*y[j-1];
      break;
    }
  }
  
  if(TMath::Abs(Volt1)>=1111.0) Volt1=0; //Check If the Voltage make sense 
  if(TMath::Abs(Volt2)>=1111.0) Volt2=0;
  if(SoP=='P')Vdepp[2]=Volt2;
  else Vdeps[2]= Volt2; 
  c->cd(1); Double_t posx=0.3 , posy= 0.55; // Plot the results in the legend
  if(positive>0) posx = 0.7 ;
  TLine *lineV1 = new TLine(positive*Volt1,*min_element(C2temp.begin(),C2temp.end()) , positive*Volt1, *max_element(C2temp.begin(),C2temp.end()));
  TLine *lineV2 = new TLine(positive*Volt2,*min_element(C2temp.begin(),C2temp.end()) , positive*Volt2, *max_element(C2temp.begin(),C2temp.end()));
  lineV1->SetLineColor(6); lineV2->SetLineColor(7); lineV1->Draw(); lineV2->Draw(); 
  TLatex latex; 
  latex.SetNDC(); latex.SetTextSize(0.025); latex.SetTextAlign(12); 
  latex.DrawLatex(posx,posy,Form("Vdeprot=%4.3f / %4.3f",positive*Volt1,positive*Volt2));// curve->Draw("lSAME"); curve->SetLineColor(kBlue);
}

/**
 This method returns the index of the maximum value of the array 
 */
Int_t TCV::GetMaximumPosition(Double_t *Cp){ 
  Int_t max =0;
  for(Int_t i=0;i<NCV; i++) if(Cp[i]>Cp[max]) max = i;
  return max;
}
  
Double_t TCV::GetRotationAngle(vector<Double_t> invC2,vector<Double_t> Voltage){
  Double_t Theta =0., deltay =0. , deltax =0.;
  deltay= *max_element(invC2.begin(),invC2.end())-*min_element(invC2.begin(),invC2.end());// diference between the maximum and minimum value in iC2
  deltax= Voltage[GetMaximumPosition(&invC2[0])]; // difference between the biger value and the smaller value  
  Theta=-1.*TMath::ATan(deltay/deltax); // Rotation angle
  
  return Theta;
}
/*-----------------------------------------------------------------------*/ 

//Gets Vbias with finding a minimum of radius of curvature
void TCV::GetVdepCurveRad(TCanvas *c, char SoP){
  Double_t *iC2Temp = new Double_t [NCV]; // Temporary array of iC2 parallel
  Double_t *yd  = new Double_t [NCV] ; //First derivative of ic2, parallel iC2
  Double_t *ydd = new Double_t [NCV] ; //Second derivative of ic2, parallel iC2
  Double_t *rc = new Double_t [NCV] ; //Curvature radius, parallel
  Int_t imax = 0; //Position of the maximum of the first derivative, parallel iC2
  Int_t imin = 0; //Position of the minimum of the curvature radius, parallel iC2
  Double_t Volt=0.;
  Double_t *iC2; 
  iC2 = (SoP=='S')? iC2s : iC2p; 
  
  //Copy data into temporary arrays and scale it
  for(Int_t i = 0; i < NCV; i++) iC2Temp[i] = F2PF*F2PF*iC2[i];
  
  TGraph *giC2p = new TGraph( NCV ,Vsense, iC2Temp);

  //Smooth the TGraph of iC2 and dump it to a new TGraph
  TGraphSmooth *gs = new TGraphSmooth("normal");
  TGraph *gsp  = gs->SmoothKern(giC2p,"normal", BW);
 
  //Calculate first derivative
  for (Int_t i=1 ; i<NCV-1 ; i++) yd[i] = GetFirstDerivative( Vsense[i], gsp );
  
  //Pad the first and the last entry to avoid random numbers
  yd[0] = yd[1]; yd[NCV-1]=yd[NCV-2];
  
  TGraph *gdiC2p  = new TGraph( NCV ,Vsense, yd);

  //First smoothen the first derivative
  TGraphSmooth *gds = new TGraphSmooth("normal");
  TGraph *gdsp  = gds->SmoothKern(gdiC2p,"normal",BWD);
 
  //Then calculate the second derivative
  for (Int_t i=1 ; i<NCV-1 ; i++)  ydd[i]  = GetFirstDerivative( Vsense[i], gdsp );
  
  //Padding to avoid random entries
  ydd[0] = ydd[1]; ydd[NCV-1]=ydd[NCV-2];
  
  TGraph *gddiC2p = new TGraph( NCV ,Vsense, ydd);

  //Smoothen the second derivative
  TGraphSmooth *gdds = new TGraphSmooth("normal");
  TGraph *gddsp = gdds->SmoothKern(gddiC2p,"normal",BWDD);

  //Calculate radious of curvature, use smoothed derivatives
  for (Int_t i=1 ; i<NCV-1 ; i++)  rc[i] = TMath::Abs( TMath::Power( 1. + TMath::Power(gdsp->Eval(Vsense[i]),2) , 1.5 )/ gddsp->Eval(Vsense[i]) );
  
  //Pad non-filled entries
  rc[0] = rc[1]; rc[NCV-1] = rc[NCV-2];
  
  //Find the maximum of the absolute value of first derivative

  if(Vsense[NCV-1] <0) imax = TMath::LocMin(NCV,yd); 
  else imax = TMath::LocMax(NCV,yd);
  
  Double_t ydmax = yd[imax];

  //Find the global maximum where the first derivative decreases and is not too large.
  //Additionally, the position of this minimum should not be lower than the initial point
  Volt = -1111 ; 
  for(Int_t i=2; i< NCV-2;i++){
    if(rc[i] < rc[imin] || imin == 0){
      if(TMath::Abs(gdsp->Eval(Vsense[i+1])) < TMath::Abs(gdsp->Eval(Vsense[i-1])) && TMath::Abs(ydmax)/2 > TMath::Abs(yd[i]) && TMath::Abs(gsp->Eval(0.)) < TMath::Abs(gsp->Eval(Vsense[i]))){
	Volt = Vsense[i];
	imin = i;
      }
    }
  }
  
  if(SoP=='S') Vdeps[1] = Volt;
  else Vdepp[1] = Volt;

  //Plot on the canvas the smoothened graph
  c->cd(1); Double_t positive=-1.0, posx =0.3 , posy = 0.5 ; // The position of the legend changes if the voltage is positive or negative.
  if(Vsense[GetMaximumPosition(iC2)]>0) positive = 1.0;
  if(positive>0) posx=0.7 ; 
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.025);
  latex.SetTextAlign(12);
  latex.DrawLatex(posx,posy,Form("Vdepcurv=%4.1f",Volt));
  
  gsp->SetLineColor(kGreen);
  for (Int_t i=0;i<gsp->GetN();i++) gsp->GetY()[i] /= F2PF*F2PF; //Rescale the graph
  gsp->Draw("lSAME");

  //Red marker for parallel and green marker for series
  TMarker *m = new TMarker(Volt,giC2p->Eval(Volt)/(F2PF*F2PF) , 22);
  m->SetMarkerSize(2);
  m->SetMarkerColor(kRed);
  m->Draw("SAME"); 
  
  delete [] rc ;     
  delete [] yd ;
  delete [] ydd ;
  delete [] iC2Temp ;
}

Double_t TCV::SearchICurrentAbove(Double_t Vsearch, Int_t index, char SoP){
  Double_t Isearch=0.;

  for(Int_t i=0; i<NCV-1; i++){
    if( TMath::Abs(Vsense[i])<TMath::Abs(Vsearch) && TMath::Abs(Vsense[i+1])>TMath::Abs(Vsearch) ){
      Isearch = Itot[i]+(Itot[i]-Itot[i+1])/(Vsense[i]-Vsense[i+1])*(Vsearch-Vsense[i]);
      if (SoP=='S') iIVdeps[index] = i;
      if (SoP=='P') iIVdepp[index] = i;
      break;
    }  
  }

  return Isearch;
}


/**
 This method draws the I-V curve 
*/
void TCV::DrawLeakageCurrent(TCanvas *c){
  c->cd(2);
  //We observed the first cv->Itot is, many times, very high
  Double_t val = Itot[0];
  Itot[0]=Itot[1];
  TGraph *Icurr = new TGraph(NCV, Vsense, Itot);
  Icurr->SetMarkerStyle(20);
  Icurr->SetMarkerSize(0.5);
  Icurr->GetXaxis()->SetTitle("Voltage [V]");
  Icurr->GetYaxis()->SetTitle("I_{tot} [A]");
  Icurr->GetYaxis()->SetTitleOffset( 0.78 );
  Icurr->Draw("lap");
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.035); 
  latex.SetTextAlign(12);
  latex.DrawLatex(0.14,0.95,Form("Compl=%4.3f [#muA]", Compl/1e-6));
  Itot[0]=val ;
}
  
void TCV::GetICurrentAboveVdep(){
  for(Int_t j=0; j<NFit; j++){
    IVdepp[j]= SearchICurrentAbove(VSCALE*Vdepp[j],j,'P');
    IVdeps[j]= SearchICurrentAbove(VSCALE*Vdeps[j],j,'S');
  }  
}


