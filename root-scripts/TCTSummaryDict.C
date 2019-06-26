// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME TCTSummaryDict

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
#include "TCTSummary.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_TCTSummary(void *p = 0);
   static void *newArray_TCTSummary(Long_t size, void *p);
   static void delete_TCTSummary(void *p);
   static void deleteArray_TCTSummary(void *p);
   static void destruct_TCTSummary(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TCTSummary*)
   {
      ::TCTSummary *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TCTSummary >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TCTSummary", ::TCTSummary::Class_Version(), "TCTSummary.h", 11,
                  typeid(::TCTSummary), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TCTSummary::Dictionary, isa_proxy, 4,
                  sizeof(::TCTSummary) );
      instance.SetNew(&new_TCTSummary);
      instance.SetNewArray(&newArray_TCTSummary);
      instance.SetDelete(&delete_TCTSummary);
      instance.SetDeleteArray(&deleteArray_TCTSummary);
      instance.SetDestructor(&destruct_TCTSummary);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TCTSummary*)
   {
      return GenerateInitInstanceLocal((::TCTSummary*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TCTSummary*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr TCTSummary::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TCTSummary::Class_Name()
{
   return "TCTSummary";
}

//______________________________________________________________________________
const char *TCTSummary::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TCTSummary*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TCTSummary::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TCTSummary*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TCTSummary::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TCTSummary*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TCTSummary::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TCTSummary*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void TCTSummary::Streamer(TBuffer &R__b)
{
   // Stream an object of class TCTSummary.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TCTSummary::Class(),this);
   } else {
      R__b.WriteClassBuffer(TCTSummary::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TCTSummary(void *p) {
      return  p ? new(p) ::TCTSummary : new ::TCTSummary;
   }
   static void *newArray_TCTSummary(Long_t nElements, void *p) {
      return p ? new(p) ::TCTSummary[nElements] : new ::TCTSummary[nElements];
   }
   // Wrapper around operator delete
   static void delete_TCTSummary(void *p) {
      delete ((::TCTSummary*)p);
   }
   static void deleteArray_TCTSummary(void *p) {
      delete [] ((::TCTSummary*)p);
   }
   static void destruct_TCTSummary(void *p) {
      typedef ::TCTSummary current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TCTSummary

namespace {
  void TriggerDictionaryInitialization_TCTSummaryDict_Impl() {
    static const char* headers[] = {
"TCTSummary.h",
0
    };
    static const char* includePaths[] = {
"./",
"/usr/local/include/root",
"/home/raaggarw/ssd-dbportal/root-scripts/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "TCTSummaryDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate(R"ATTRDUMP(ROOT RTTI)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(ROOT RTTI)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(ROOT RTTI)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(ROOT RTTI)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$TCTSummary.h")))  TCTSummary;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "TCTSummaryDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "TCTSummary.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"TCTSummary", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("TCTSummaryDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_TCTSummaryDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_TCTSummaryDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_TCTSummaryDict() {
  TriggerDictionaryInitialization_TCTSummaryDict_Impl();
}
