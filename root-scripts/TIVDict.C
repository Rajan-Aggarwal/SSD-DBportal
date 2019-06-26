// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME TIVDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "TIV.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_TIV(void *p = 0);
   static void *newArray_TIV(Long_t size, void *p);
   static void delete_TIV(void *p);
   static void deleteArray_TIV(void *p);
   static void destruct_TIV(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TIV*)
   {
      ::TIV *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TIV >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TIV", ::TIV::Class_Version(), "TIV.h", 15,
                  typeid(::TIV), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TIV::Dictionary, isa_proxy, 4,
                  sizeof(::TIV) );
      instance.SetNew(&new_TIV);
      instance.SetNewArray(&newArray_TIV);
      instance.SetDelete(&delete_TIV);
      instance.SetDeleteArray(&deleteArray_TIV);
      instance.SetDestructor(&destruct_TIV);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TIV*)
   {
      return GenerateInitInstanceLocal((::TIV*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TIV*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr TIV::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TIV::Class_Name()
{
   return "TIV";
}

//______________________________________________________________________________
const char *TIV::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TIV*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TIV::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TIV*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TIV::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TIV*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TIV::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TIV*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void TIV::Streamer(TBuffer &R__b)
{
   // Stream an object of class TIV.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TIV::Class(),this);
   } else {
      R__b.WriteClassBuffer(TIV::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TIV(void *p) {
      return  p ? new(p) ::TIV : new ::TIV;
   }
   static void *newArray_TIV(Long_t nElements, void *p) {
      return p ? new(p) ::TIV[nElements] : new ::TIV[nElements];
   }
   // Wrapper around operator delete
   static void delete_TIV(void *p) {
      delete ((::TIV*)p);
   }
   static void deleteArray_TIV(void *p) {
      delete [] ((::TIV*)p);
   }
   static void destruct_TIV(void *p) {
      typedef ::TIV current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TIV

namespace ROOT {
   static TClass *maplEpairlETStringcOTStringgRcOdoublegR_Dictionary();
   static void maplEpairlETStringcOTStringgRcOdoublegR_TClassManip(TClass*);
   static void *new_maplEpairlETStringcOTStringgRcOdoublegR(void *p = 0);
   static void *newArray_maplEpairlETStringcOTStringgRcOdoublegR(Long_t size, void *p);
   static void delete_maplEpairlETStringcOTStringgRcOdoublegR(void *p);
   static void deleteArray_maplEpairlETStringcOTStringgRcOdoublegR(void *p);
   static void destruct_maplEpairlETStringcOTStringgRcOdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<pair<TString,TString>,double>*)
   {
      map<pair<TString,TString>,double> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<pair<TString,TString>,double>));
      static ::ROOT::TGenericClassInfo 
         instance("map<pair<TString,TString>,double>", -2, "map", 96,
                  typeid(map<pair<TString,TString>,double>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEpairlETStringcOTStringgRcOdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(map<pair<TString,TString>,double>) );
      instance.SetNew(&new_maplEpairlETStringcOTStringgRcOdoublegR);
      instance.SetNewArray(&newArray_maplEpairlETStringcOTStringgRcOdoublegR);
      instance.SetDelete(&delete_maplEpairlETStringcOTStringgRcOdoublegR);
      instance.SetDeleteArray(&deleteArray_maplEpairlETStringcOTStringgRcOdoublegR);
      instance.SetDestructor(&destruct_maplEpairlETStringcOTStringgRcOdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<pair<TString,TString>,double> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const map<pair<TString,TString>,double>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEpairlETStringcOTStringgRcOdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const map<pair<TString,TString>,double>*)0x0)->GetClass();
      maplEpairlETStringcOTStringgRcOdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void maplEpairlETStringcOTStringgRcOdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEpairlETStringcOTStringgRcOdoublegR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<pair<TString,TString>,double> : new map<pair<TString,TString>,double>;
   }
   static void *newArray_maplEpairlETStringcOTStringgRcOdoublegR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) map<pair<TString,TString>,double>[nElements] : new map<pair<TString,TString>,double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEpairlETStringcOTStringgRcOdoublegR(void *p) {
      delete ((map<pair<TString,TString>,double>*)p);
   }
   static void deleteArray_maplEpairlETStringcOTStringgRcOdoublegR(void *p) {
      delete [] ((map<pair<TString,TString>,double>*)p);
   }
   static void destruct_maplEpairlETStringcOTStringgRcOdoublegR(void *p) {
      typedef map<pair<TString,TString>,double> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class map<pair<TString,TString>,double>

namespace {
  void TriggerDictionaryInitialization_TIVDict_Impl() {
    static const char* headers[] = {
"TIV.h",
0
    };
    static const char* includePaths[] = {
"./",
"/usr/local/include/root",
"/home/raaggarw/ssd-dbportal/root-scripts/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "TIVDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate(R"ATTRDUMP(IV data class)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$TIV.h")))  TIV;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "TIVDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "TIV.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"TIV", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("TIVDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_TIVDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_TIVDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_TIVDict() {
  TriggerDictionaryInitialization_TIVDict_Impl();
}
