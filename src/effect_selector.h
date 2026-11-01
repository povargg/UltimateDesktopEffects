#pragma once
#include "settings_api.h"
#include "i18n.h"
// Selectors post to the host; they never destroy their own panel in an input callback.
template<class Panel>void AddSuiteHeader(Panel& p,HWND notify,bool popup){
 if(!popup){
  p.AddLink({526,28,860,48},::Tr(::S_PROJECT_BY),APP_REPO,true);
  p.AddLink({526,52,860,74},::Tr(::S_AUTHOR),UPSTREAM_REPO,true);
  p.AddCaption({24,108,192,148},::Tr(::S_EFFECT));
 }else p.AddCaption({20,60,298,80},::Tr(::S_EFFECT));
 p.AddSegment(popup?D2D1::RectF(16,82,300,116):D2D1::RectF(204,108,860,148),
  {::Tr(::S_EFFECT_SNOW),::Tr(::S_EFFECT_RAIN),::Tr(::S_EFFECT_LEAVES),::Tr(::S_EFFECT_HALLOWEEN)},
  []{return SuiteCfg().effect;},[notify](int value){if(ValidEffect(value))PostMessageW(notify,WM_APP_EFFECT,value,0);});
 p.SetContentOffset(popup?64:52);
}
