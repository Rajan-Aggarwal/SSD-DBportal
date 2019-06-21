void GetNerOfMeasurement( string detid , string meastype ) {
   
   gSystem->Load("TIV_cpp.so");   
   gSystem->Load("TCV_cpp.so");   

   //Convert input to TStrings
   TString MeasType = TString( meastype );
   TString DetID    = TString( detid );

   //Open existing ROOT file and Get the tree
   TFile *file0 = TFile::Open("CVIV.root") ;
   TTree *tree = (TTree *) file0->Get("cviv") ;
   
   //Build the right selection string
   TString what, sel ;
   if ( MeasType.Contains("iv")) {
     what = TString("iv->ivent") ;
     sel  = TString("iv->sensor.Contains(\"") + DetID.Data()+ TString("\") && Type==1") ;
   }
   if ( MeasType.Contains("cv")) {
     what = TString("cv->cvent") ;
     sel  = TString("cv->sensor.Contains(\"") + DetID.Data()+ TString("\") && Type==2") ;
   }
   
   //Get Ner of Measurements of type meastype
   int Ner = tree->Draw( what , sel ,"goff");
   
   cout << Ner << endl;
   
   //return Ner ;
   
}

