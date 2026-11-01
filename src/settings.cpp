#include "settings.h"
#include <shlobj.h>
static const wchar_t kRunKey[]=L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
std::wstring SettingsDir(){PWSTR p=nullptr;std::wstring dir;if(SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData,0,nullptr,&p))){dir=std::wstring(p)+L"\\" APP_NAME;CoTaskMemFree(p);}return dir;}
static std::string ReadFileText(const std::wstring& path){
 HANDLE h=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);if(h==INVALID_HANDLE_VALUE)return {};
 LARGE_INTEGER size{};std::string text;
 if(GetFileSizeEx(h,&size)&&size.QuadPart>=0&&size.QuadPart<=1024*1024){text.resize((size_t)size.QuadPart);DWORD got=0;if(!ReadFile(h,text.data(),(DWORD)text.size(),&got,nullptr))text.clear();else text.resize(got);}
 CloseHandle(h);return text;
}
void LoadSettings(){
 auto dir=SettingsDir();auto ini=dir+L"\\settings.ini";
 if(GetFileAttributesW(ini.c_str())!=INVALID_FILE_ATTRIBUTES){DecodeSettings(ReadFileText(ini));return;}
 // First launch only: import each standalone app's existing profile, without modifying it.
 ResetAllProfiles();auto slash=dir.find_last_of(L"\\/");auto parent=dir.substr(0,slash);
 const wchar_t* legacy[]={L"LayerSnow",L"LayerRain",L"LayerLeaves",L"LayerHalloween"};
 std::string combined;
 for(int e=0;e<EFFECT_COUNT;++e){auto text=ReadFileText(parent+L"\\"+legacy[e]+L"\\settings.ini");if(e==0&&text.empty())text=ReadFileText(parent+L"\\SnowLayer\\settings.ini");combined+=text+'\n';}
 DecodeSettings(combined);
}
void SaveSettings(){
 auto dir=SettingsDir();if(dir.empty())return;
 if(!CreateDirectoryW(dir.c_str(),nullptr)&&GetLastError()!=ERROR_ALREADY_EXISTS)return;
 auto path=dir+L"\\settings.ini",temporary=dir+L"\\settings.ini.tmp";auto text=EncodeSettings();
 HANDLE h=CreateFileW(temporary.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);if(h==INVALID_HANDLE_VALUE)return;
 DWORD written=0;bool ok=WriteFile(h,text.data(),(DWORD)text.size(),&written,nullptr)&&written==text.size();if(ok)ok=FlushFileBuffers(h)!=0;CloseHandle(h);
 if(ok)ok=MoveFileExW(temporary.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
 if(!ok)DeleteFileW(temporary.c_str());
}
bool IsAutostartEnabled(){wchar_t buf[MAX_PATH*2];DWORD size=sizeof(buf);return RegGetValueW(HKEY_CURRENT_USER,kRunKey,APP_NAME,RRF_RT_REG_SZ,nullptr,buf,&size)==ERROR_SUCCESS;}
void SetAutostart(bool on,const std::wstring& exe){HKEY key;if(RegCreateKeyExW(HKEY_CURRENT_USER,kRunKey,0,nullptr,0,KEY_SET_VALUE,nullptr,&key,nullptr)!=ERROR_SUCCESS)return;if(on){auto cmd=L"\""+exe+L"\" /autostart";RegSetValueExW(key,APP_NAME,0,REG_SZ,(const BYTE*)cmd.c_str(),(DWORD)((cmd.size()+1)*sizeof(wchar_t)));}else RegDeleteValueW(key,APP_NAME);RegCloseKey(key);}
