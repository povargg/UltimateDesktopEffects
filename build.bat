@echo off
rem Build one self-contained UltimateDesktopEffects Windows x64 application.
setlocal
cd /d "%~dp0"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSDIR=%%i"
if not defined VSDIR (
 echo Visual Studio Build Tools ^(C++ desktop workload^) not found
 exit /b 1
)
call "%VSDIR%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1 || exit /b 1
if not exist build mkdir build
if not exist dist mkdir dist
rc /nologo /fo build\app.res res\app.rc || exit /b 1
rem Use unique object names because the four modules deliberately retain render.cpp/ui.cpp names.
type nul > build\objects.rsp
set /a ude_unit_count=0 >nul
for /r src %%f in (*.cpp) do (
 call :compile "%%f" || exit /b 1
)
link /nologo @build\objects.rsp build\app.res /OUT:dist\UltimateDesktopEffects.exe /SUBSYSTEM:WINDOWS ^
 user32.lib gdi32.lib shell32.lib ole32.lib advapi32.lib comctl32.lib d3d11.lib dxgi.lib d2d1.lib ^
 dwrite.lib dcomp.lib dwmapi.lib shcore.lib uuid.lib || exit /b 1
copy /y dist\UltimateDesktopEffects.exe dist\UltimateDesktopEffects-Portable.exe >nul || exit /b 1
copy /y dist\UltimateDesktopEffects.exe dist\UltimateDesktopEffects-Uninstall.exe >nul || exit /b 1
echo Built dist\UltimateDesktopEffects.exe
exit /b 0
:compile
set "unit_path=%~1"
set /a ude_unit_count+=1 >nul
set "unit_name=unit_%ude_unit_count%"
cl /nologo /c /std:c++17 /O2 /MT /W3 /utf-8 /EHsc /DNDEBUG /D_WIN32_WINNT=0x0A00 /Fo"build\%unit_name%.obj" "%unit_path%" || exit /b 1
>>build\objects.rsp echo "build\%unit_name%.obj"
exit /b 0
