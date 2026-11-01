// Shared configuration codec, also exercised by the native tests.
#include "settings.h"
#include <charconv>
#include <map>
#include <sstream>
static SuiteSettings suite;
SuiteSettings& SuiteCfg(){return suite;}
const char* ProfileName(int effect){static const char* names[]={"snow","rain","leaves","halloween"};return names[ValidEffect(effect)?effect:0];}
std::vector<SettingField> ProfileFields(int effect){
 switch(effect){case EFFECT_SNOW:return snowfx::Fields();case EFFECT_RAIN:return rainfx::Fields();case EFFECT_LEAVES:return leavesfx::Fields();case EFFECT_HALLOWEEN:return halloweenfx::Fields();default:return {};}
}
void SetLanguage(int value){
 suite.lang=std::clamp(value,-1,4);
 snowfx::Cfg().lang=rainfx::Cfg().lang=leavesfx::Cfg().lang=halloweenfx::Cfg().lang=suite.lang;
}
void ResetAllProfiles(){snowfx::Cfg()=snowfx::Settings();rainfx::Cfg()=rainfx::Settings();leavesfx::Cfg()=leavesfx::Settings();halloweenfx::Cfg()=halloweenfx::Settings();suite=SuiteSettings();}
static std::string Narrow(const wchar_t* key){std::string s;while(*key)s+=(char)*key++;return s;}
static std::string Trim(std::string s){auto a=s.find_first_not_of(" \t\r");if(a==std::string::npos)return {};return s.substr(a,s.find_last_not_of(" \t\r")-a+1);}
using Ini=std::map<std::string,std::map<std::string,int>>;
static Ini Parse(const std::string& text){
 Ini ini;std::string section,line;std::istringstream input(text);int lines=0;
 while(std::getline(input,line)&&++lines<=10000){
  line=Trim(line);if(line.empty()||line[0]==';'||line[0]=='#')continue;
  if(line.size()>2&&line.front()=='['&&line.back()==']'){section=Trim(line.substr(1,line.size()-2));continue;}
  auto eq=line.find('=');if(eq==std::string::npos)continue;
  auto key=Trim(line.substr(0,eq)),value=Trim(line.substr(eq+1));int n=0;
  auto parsed=std::from_chars(value.data(),value.data()+value.size(),n);
  if(parsed.ec==std::errc()&&parsed.ptr==value.data()+value.size())ini[section][key]=n;
 }
 return ini;
}
void DecodeSettings(const std::string& text,bool reset){
 if(reset)ResetAllProfiles();
 auto ini=Parse(text);
 for(int e=0;e<EFFECT_COUNT;++e){auto section=ini.find(ProfileName(e));if(section==ini.end())continue;
  for(auto f:ProfileFields(e)){auto p=section->second.find(Narrow(f.key));if(p==section->second.end())continue;int value=std::clamp(p->second,f.lo,f.hi);if(f.i)*f.i=value;else *f.b=value!=0;}
 }
 auto section=ini.find("suite");if(section!=ini.end()){
  auto& kv=section->second;if(kv.count("effect"))suite.effect=std::clamp(kv["effect"],0,3);
  if(kv.count("lang"))suite.lang=std::clamp(kv["lang"],-1,4);
 }
 SetLanguage(suite.lang);
}
std::string EncodeSettings(){
 std::ostringstream out;out<<"; UltimateDesktopEffects 1.0.1\n[suite]\neffect="<<std::clamp(suite.effect,0,3)<<"\nlang="<<std::clamp(suite.lang,-1,4)<<"\n";
 for(int e=0;e<EFFECT_COUNT;++e){out<<"\n["<<ProfileName(e)<<"]\n";for(auto f:ProfileFields(e)){int value=f.i?*f.i:(*f.b?1:0);out<<Narrow(f.key)<<'='<<std::clamp(value,f.lo,f.hi)<<'\n';}}
 return out.str();
}
CommonSettings ActiveSettings(){
 auto get=[](const auto& c){return CommonSettings{c.enabled,c.showFalling,c.pauseFullscreen,c.intensity,c.fps,c.mode};};
 switch(suite.effect){case EFFECT_RAIN:return get(rainfx::Cfg());case EFFECT_LEAVES:return get(leavesfx::Cfg());case EFFECT_HALLOWEEN:return get(halloweenfx::Cfg());default:return get(snowfx::Cfg());}
}
