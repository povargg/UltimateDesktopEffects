#!/usr/bin/env python3
"""Build a self-contained Windows x64 executable with MinGW-w64, also from Linux."""
from pathlib import Path
import argparse, os, shutil, subprocess

root=Path(__file__).resolve().parent
p=argparse.ArgumentParser()
p.add_argument('--cxx',default=os.environ.get('CXX','x86_64-w64-mingw32-g++-posix'))
args=p.parse_args()
cxx=shutil.which(args.cxx)
if not cxx: raise SystemExit('MinGW-w64 C++ compiler not found; use --cxx /path/to/x86_64-w64-mingw32-g++-posix')
cxx=Path(cxx).absolute(); bindir=cxx.parent
cc=cxx.with_name(cxx.name.replace('g++','gcc'))
windres=bindir/'x86_64-w64-mingw32-windres'
strip=bindir/'x86_64-w64-mingw32-strip'
(root/'build').mkdir(exist_ok=True);(root/'dist').mkdir(exist_ok=True)
if not (root/'res/ultimate.ico').exists(): raise SystemExit('res/ultimate.ico is required')
env=dict(os.environ);env['PATH']=str(bindir)+os.pathsep+env.get('PATH','')
subprocess.check_call([str(windres),'--preprocessor='+str(cc),'--preprocessor-arg=-E','--preprocessor-arg=-xc',
    '--preprocessor-arg=-DRC_INVOKED','-I','res','res/app.rc','-O','coff','-o','build/app.res.o'],cwd=root,env=env)
exe=root/'dist/UltimateDesktopEffects.exe'
command=[str(cxx),'-std=c++17','-O2','-static','-static-libgcc','-static-libstdc++','-municode','-mwindows',
    '-D_WIN32_WINNT=0x0A00','-DNDEBUG','-Wall','-Wextra','-Wno-missing-field-initializers',
    '-Wno-unknown-pragmas','-Wno-cast-function-type']
command += [str(x.relative_to(root)) for x in sorted((root/'src').rglob('*.cpp'))]
command += ['build/app.res.o','-o',str(exe)]
command += ['-l'+x for x in ['user32','gdi32','shell32','ole32','advapi32','comctl32','d3d11','dxgi','d2d1','dwrite','dcomp','dwmapi','shcore','uuid']]
subprocess.check_call(command,cwd=root,env=env)
subprocess.check_call([str(strip),'--strip-unneeded',str(exe)])
for name in ['UltimateDesktopEffects-Portable.exe','UltimateDesktopEffects-Uninstall.exe']: shutil.copy2(exe,root/'dist'/name)
print('Built:',exe,'('+str(exe.stat().st_size)+' bytes)')
