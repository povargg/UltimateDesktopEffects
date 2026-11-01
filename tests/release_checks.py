#!/usr/bin/env python3
"""Fifteen file, asset and static integration checks. No Windows GUI execution."""
from pathlib import Path
from PIL import Image
import hashlib,json,re,struct
ROOT=Path(__file__).resolve().parents[1]
DATA=(ROOT/'dist/UltimateDesktopEffects.exe').read_bytes()
SRC=ROOT/'src'
def check(ok,message):
 if not ok:raise AssertionError(message)
def text(path):return (SRC/path).read_text()
def pixels(path):
 im=Image.open(path).convert('RGBA');return im,[im.getpixel((x,y)) for y in range(im.height) for x in range(im.width)]
PE=struct.unpack_from('<I',DATA,0x3c)[0];OPT=PE+24;SECTION=OPT+struct.unpack_from('<H',DATA,PE+20)[0];NSECTIONS=struct.unpack_from('<H',DATA,PE+6)[0]
def offset(rva):
 for i in range(NSECTIONS):
  at=SECTION+i*40;vsize,va,size,raw=struct.unpack_from('<IIII',DATA,at+8)
  if va<=rva<va+max(size,vsize):return raw+rva-va
 raise AssertionError('RVA outside sections')
def pe_architecture():
 check(DATA[:2]==b'MZ' and DATA[PE:PE+4]==b'PE\0\0','invalid PE signature')
 check(struct.unpack_from('<H',DATA,PE+4)[0]==0x8664 and struct.unpack_from('<H',DATA,OPT)[0]==0x20b,'not Windows x64 PE32+')
 check(struct.unpack_from('<H',DATA,OPT+68)[0]==2,'not GUI subsystem')
 check(struct.unpack_from('<H',DATA,OPT+70)[0]&0x140==0x140,'missing ASLR/NX')
def imports():
 at=offset(struct.unpack_from('<I',DATA,OPT+112+8)[0]);names=[]
 while any(DATA[at:at+20]):
  start=offset(struct.unpack_from('<I',DATA,at+12)[0]);names.append(DATA[start:DATA.index(0,start)].decode().lower());at+=20
 allowed={'advapi32.dll','comctl32.dll','d2d1.dll','d3d11.dll','dcomp.dll','dwmapi.dll','dwrite.dll','dxgi.dll','gdi32.dll','kernel32.dll','msvcrt.dll','ole32.dll','shell32.dll','shcore.dll','user32.dll','uxtheme.dll'}
 check(set(names)<=allowed,'external runtime DLLs: '+str(set(names)-allowed));print('    Imports:',', '.join(names))
def resources():
 base=offset(struct.unpack_from('<I',DATA,OPT+112+16)[0]);types=[struct.unpack_from('<I',DATA,base+16+i*8)[0] for i in range(sum(struct.unpack_from('<HH',DATA,base+12)))];check({3,14,16,24}<=set(types),'missing icon/version/manifest')
 for s in ['UltimateDesktopEffects','1.0.1','povargg','flipkips']:check(s.encode('utf-16le') in DATA,'missing version/author string '+s)
 check(b'PerMonitorV2' in DATA and b'asInvoker' in DATA,'manifest missing DPI or privilege declaration')
def dispatch_binaries():
 for name in ['Portable','Uninstall']:check((ROOT/'dist'/f'UltimateDesktopEffects-{name}.exe').read_bytes()==DATA,'variants differ')
 main=text('main.cpp');check('name.find(L"portable")' in main and 'name.find(L"uninstall")' in main and 'IsInstalledCopy()' in main,'filename dispatch incomplete')
def one_process():
 allsrc='\n'.join(p.read_text() for p in SRC.rglob('*.cpp'));main=text('main.cpp')
 check(allsrc.count('int WINAPI wWinMain(')==1,'multiple application entry points')
 check(main.count('CreateMutexW(')==1 and main.count('TrayAdd(msgWnd_)')==2,'single host/tray recovery changed')
 check('Local\\\\UltimateDesktopEffects.SingleInstance' in text('common.h'),'wrong mutex identity')
 check(not re.search(r'(?:LayerRain|LayerLeaves|LayerHalloween|LayerSnow)(?:-Portable|-Setup)?\.exe',allsrc),'standalone program launch/reference found')
 check(allsrc.count('CreateProcessW(')==1,'unexpected child process path')
def renderer_lifetime():
 render=text('render.cpp');body=render.split('bool Renderer::Init',1)[1]
 check(body.index('Shutdown();')<body.index('active_=effect'),'old GPU resources not released before selecting')
 for name in ['snow','rain','leaves','halloween']:
  check(name+'_.Shutdown()' in render and name+'_.Init(' in render,'renderer module not wired: '+name)
  shutdown=text(f'effects/{name}/render.cpp').split('void Renderer::Shutdown()',1)[1].split('\n}',1)[0]
  for pointer in ['dcompTarget_.Reset()','dcomp_.Reset()','dc_.Reset()','d3d_.Reset()']:check(pointer in shutdown,'GPU reference survives shutdown')
 check('e.Active()!=active_' in render,'draw can use stale renderer')
 for name in ['snow','rain','leaves','halloween']:
  ui=text(f'effects/{name}/ui.cpp');check('g_panel.reset();' in ui and 'g_flyout.reset();' in ui,'inactive panel resources retained')
def selectors():
 expected={'snow':2,'rain':3,'leaves':3,'halloween':3}
 for key,count in expected.items():check(text(f'effects/{key}/ui.cpp').count('::AddSuiteHeader(p,notify,')==count,key+' selector not in all windows')
 selector=text('effect_selector.h');check('WM_APP_EFFECT' in selector and 'PostMessageW' in selector,'effect switch not posted to host')
 for name in ['S_EFFECT_SNOW','S_EFFECT_RAIN','S_EFFECT_LEAVES','S_EFFECT_HALLOWEEN']:check(name in selector,'missing effect choice')
 main=text('main.cpp').split('case WM_APP_EFFECT:',1)[1].split('case WM_APP_LIGHTNING:',1)[0]
 check(main.index('UpdateMouseHook(false)')<main.index('scene_.Select') and 'CloseUi();' in main and 'renderer_.Shutdown();' in main,'switch cleanup is incomplete')
def languages():
 expected={'':99,'effects/snow/':72,'effects/rain/':88,'effects/leaves/':83,'effects/halloween/':94}
 for folder,count in expected.items():
  code=text(folder+'i18n.cpp');rows=re.findall(r'^\s*\{((?:L"(?:[^"\\]|\\.)*"\s*,?\s*)+)\},?',code,re.M)
  check(len(rows)==count,folder+' translation row count')
  for row in rows:
   entries=re.findall(r'L"(?:[^"\\]|\\.)*"',row);check(len(entries)==5,folder+' incomplete five-language row')
   for entry in entries:check(json.loads(entry[1:]).encode('utf-16le') in DATA,folder+' translated text missing from EXE')
  check('LANG_DE' in text(folder+'i18n.h'),'German language absent')
 for effect in ['snow','rain','leaves','halloween']:check('SetLanguage(v)' in text(f'effects/{effect}/ui.cpp'),'language setter not shared')
 snow_rows=re.findall(r'^\s*\{((?:L"(?:[^"\\]|\\.)*"\s*,?\s*)+)\},?',text('effects/snow/i18n.cpp'),re.M)
 for row in snow_rows:
  german=json.loads(re.findall(r'L"(?:[^"\\]|\\.)*"',row)[4][1:]);check(not re.search(r'\b(?:Regen|Wasser|Gewitter|Niesel|Starkregen)\b',german),'rain terminology in German snow mode')
 check('Basis: LayerSnow von flipkips' in text('i18n.cpp'),'upstream name changed in attribution')
def authors():
 header=text('effect_selector.h');check('S_PROJECT_BY' in header and 'S_AUTHOR' in header and 'APP_REPO' in header and 'UPSTREAM_REPO' in header,'header author links absent')
 for name in ['povargg','flipkips']:check(name in text('install.cpp') and name in text('license.h') and name in (ROOT/'LICENSE').read_text(),'author absent: '+name)
 check('MIT License' in (ROOT/'LICENSE').read_text(),'MIT notice absent')
def settings():
 counts={}
 for effect in ['snow','rain','leaves','halloween']:
  header=text(f'effects/{effect}/settings.h').split('struct Settings {',1)[1].split('};',1)[0];header=re.sub(r'//[^\n]*','',header)
  fields=set(re.findall(r'\b(\w+)\s*=',header));saved=re.findall(r'\{L"(\w+)"\s*,',text(f'effects/{effect}/settings.cpp'))
  check(fields==set(saved) and len(saved)==len(fields),'incomplete persistence table '+effect);counts[effect]=len(fields)
  check(re.search(r'\{L"lang",\s*&s.lang,\s*nullptr,\s*-1,\s*4\}',text(f'effects/{effect}/settings.cpp')) is not None,'profile does not persist all five languages')
 storage=text('settings.cpp');check('MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH' in storage and 'FlushFileBuffers' in storage,'settings are not replaced atomically')
 check('First launch only' in storage and 'LayerSnow' in storage and 'LayerRain' in storage and 'LayerLeaves' in storage and 'LayerHalloween' in storage,'legacy import missing')
 print('    Profile settings:',counts)
def asset_format():
 paths=list((ROOT/'res/leaves/sprites').glob('sakura-*.png'))+list((ROOT/'res/leaves/sprites').glob('summer-*.png'))+list((ROOT/'res/leaves/sprites').glob('autumn-*.png'))+list((ROOT/'res/halloween/sprites').glob('candy-*.png'))+list((ROOT/'res/halloween/sprites').glob('spiders-*.png'))
 check(len(paths)==22,'expected 12 leaf and 10 Halloween sprites')
 for path in paths:
  im,px=pixels(path);check(im.size==(16,16),path.name+' not 16x16');check({p[3] for p in px}<={0,255},'soft alpha');check(len({p for p in px if p[3]})<=8,'palette too large')
def pixel_identity():
 groups=[('leaves','leaf_sprites.h',[(kind,i) for kind in ['sakura','summer','autumn'] for i in range(1,5)]),('halloween','sprites.h',[(kind,i) for kind,count in [('candy',6),('spiders',4)] for i in range(1,count+1)])]
 for effect,header,variants in groups:
  values=[int(x,16) for x in re.findall(r'0x([0-9A-Fa-f]{8})u',text(f'effects/{effect}/{header}'))];check(len(values)==len(variants)*256,'header count mismatch')
  for index,(kind,n) in enumerate(variants):
   _,px=pixels(ROOT/'res'/effect/'sprites'/f'{kind}-{n}.png');expected=[(a<<24)|(r<<16)|(g<<8)|b if a else 0 for r,g,b,a in px]
   check(values[index*256:(index+1)*256]==expected,'PNG/source pixels differ');check(struct.pack('<256I',*expected) in DATA,'sprite missing from EXE')
def dense_piles_and_layout():
 for effect,header,stem in [('leaves','leaves.h','leafui'),('halloween','scene.h','panel')]:
  geometry=re.sub(r'\s','',text(f'effects/{effect}/{header}'));check('kRestingCollisionHalfWidth=0.24f' in geometry or 'kRestingCollisionHalfWidth=.24f' in geometry,'large collision footprint');check('kRestingSpriteSquash=1' in geometry,'flattened resting sprites')
  check('D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR' in text(f'effects/{effect}/render.cpp'),'interpolated pixel sprites')
 for effect,stem in [('snow','snowui'),('rain','weatherui'),('leaves','leafui'),('halloween','panel')]:
  panel=text(f'effects/{effect}/{stem}.cpp');check('h_ = height + contentOffset_;' in panel,'panel height not expanded')
  methods=re.findall(r'void \w+::Add\w+\([^{}]+\)\s*\{([^}]+)',panel);check(methods and all('rc=LayoutRect(rc);' in body for body in methods),'widget layout offset incomplete')
def mouse_and_messages():
 main=text('main.cpp');check('scene_.CanCutThreads()||cutButtonHeld_' in main and 'CallNextHookEx' in main and 'UiContainsPoint(screen)' in main,'hook scope/forwarding absent')
 check('active_==EFFECT_HALLOWEEN&&halloween.CutWebAt' in text('effects.cpp'),'cut can reach inactive effect')
 ids=re.findall(r'#define WM_APP_\w+\s+\(WM_APP \+ (\d+)\)',text('common.h'));check(len(ids)==11 and len(set(ids))==11,'message IDs overlap')
def physics_fidelity():
 hashes=json.loads((ROOT/'docs/source-provenance.json').read_text())['simulation_body_sha256']
 for relative,digest in hashes.items():
  effect=relative.split('/')[0];body=text('effects/'+relative);body=body.split('namespace '+effect+'fx {\n',1)[1].rsplit('} // namespace '+effect+'fx',1)[0]
  normalized='\n'.join(line.strip() for line in body.splitlines() if line.strip() and not line.startswith('#include'))
  check(hashlib.sha256(normalized.encode()).hexdigest()==digest,'original simulation changed: '+relative)
checks=[('Windows x64 GUI executable and memory-protection flags',pe_architecture),('only Windows system DLL dependencies',imports),('icon, DPI manifest, version and author resources',resources),('setup / portable / uninstall binary dispatch',dispatch_binaries),('one application process, entry point, mutex and tray',one_process),('inactive renderer GPU resources are released',renderer_lifetime),('all settings / extra windows / tray menus have the effect selector',selectors),('all 436 translation rows have five language columns',languages),('both authors and MIT notices are visible and preserved',authors),('every original profile field is persisted; atomic write and migration',settings),('all 22 original pixel sprites preserve their native format',asset_format),('PNG, C++ array and EXE pixels are byte-identical',pixel_identity),('full-face dense piles and shifted UI geometry',dense_piles_and_layout),('Halloween-only mouse hook and unique host messages',mouse_and_messages),('all nine original simulation bodies remain unchanged',physics_fidelity)]
passed=failed=0
for title,fn in checks:
 try:fn();passed+=1;print('PASS',title)
 except Exception as e:failed+=1;print('FAIL',title,':',e)
print(f'\n{passed} release checks passed, {failed} failed')
raise SystemExit(bool(failed))
