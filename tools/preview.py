"""Render real unified-controller states offline. Approximation of GPU drawing, not Windows screenshots."""
from pathlib import Path
from PIL import Image,ImageDraw,ImageFont
import json,math
root=Path(__file__).resolve().parents[1]
font=ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf',15)
font_title=ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf',19)
sprite_sets={2:[[Image.open(root/'res/leaves/sprites'/f'autumn-{i}.png').convert('RGBA') for i in range(1,5)]],3:[[Image.open(root/'res/halloween/sprites'/f'{name}-{i}.png').convert('RGBA') for i in range(1,count+1)] for name,count in [('candy',6),('spiders',4)]]}
def tile(effect,family,variant,size,degrees,squash=1):
 src=sprite_sets[effect][family][variant];side=max(1,round(size))
 return src.resize((side,max(1,round(side*squash))),Image.Resampling.NEAREST).rotate(-round(degrees/22.5)*22.5,resample=Image.Resampling.NEAREST,expand=True)
def support(effect,family,variant,size,angle,base):
 src=sprite_sets[effect][family][variant];a=math.radians(round(angle/22.5)*22.5);co=math.cos(a);si=math.sin(a)
 return base+size*max(((x+dx)/16-.5)*si+((y+dy)/16-.5)*co for y in range(16) for x in range(16) if src.getpixel((x,y))[3] for dx in [0,1] for dy in [0,1])
thumbs=[]
for effect,(name,label,bg) in enumerate([('snow','Schnee',(22,35,51)),('rain','Regen',(15,33,46)),('leaves','Blätter',(39,31,25)),('halloween','Halloween',(28,20,39))]):
 state=json.loads((root/'build'/f'preview-{name}.json').read_text());im=Image.new('RGBA',(1280,800),bg+(255,));d=ImageDraw.Draw(im)
 for x,y,r,b in [(120,240,640,640),(720,400,1200,700)]:
  d.rounded_rectangle((x,y,r,b),radius=7,fill=(226,232,239),outline=(119,132,145),width=1);d.rectangle((x+1,y+1,r-1,y+32),fill=(191,205,220));d.text((x+18,y+10),'Fenster',font=font,fill=(38,50,63))
 d.rectangle((0,758,1279,799),fill=(45,54,67))
 for edge in state['surfaces']:
  if effect<2:
   shape=[(edge['left'],edge['top'])]+[(edge['left']+i*2,edge['top']-h) for i,h in enumerate(edge['h'])]+[(edge['left']+edge['width'],edge['top'])]
   d.polygon(shape,fill=(222,239,253) if effect==0 else (47,120,174));d.line(shape[1:-1],fill=(251,253,255) if effect==0 else (146,210,243),width=1)
  else:
   layer=Image.new('RGBA',im.size)
   for x,size,base,angle,family,variant in edge['objects']:
    family=int(family);variant=int(variant);part=tile(effect,family,variant,size,angle);cx=edge['left']+x;cy=edge['top']-support(effect,family,variant,size,angle,base)
    layer.alpha_composite(part,(round(cx-part.width/2),round(cy-part.height/2)))
   crop=layer.crop((edge['left'],0,edge['left']+edge['width'],edge['top']));im.alpha_composite(crop,(edge['left'],0));d=ImageDraw.Draw(im)
 for x,y,size,rotation,family,variant,kind in state['airborne']:
  if effect==0:
   if kind==1:
    for angle in range(0,180,60):
     a=math.radians(angle)+rotation;dx=math.cos(a)*max(2,size*.6);dy=math.sin(a)*max(2,size*.6);d.line((x-dx,y-dy,x+dx,y+dy),fill=(232,245,255),width=1)
   else:d.ellipse((x-size*.35,y-size*.35,x+size*.35,y+size*.35),fill=(219,235,249))
  elif effect==1:d.line((x-2,y-11,x+2,y+3),fill=(117,181,227),width=max(1,round(size*.4)))
  else:
   squash=1-.65*(.62-.62*abs(math.cos(rotation*.7+kind)));part=tile(effect,int(family),int(variant),size,math.degrees(rotation),squash)
   im.alpha_composite(part,(round(x-part.width/2),round(y-part.height/2)))
 for x,y,size,variant,mode,ax,ay,bx,by in state['webs']:
  d=ImageDraw.Draw(im);d.line((ax,ay,bx,by),fill=(13,8,20),width=3);d.line((ax,ay,bx,by),fill=(204,191,222),width=1);part=tile(3,1,int(variant),size,180 if mode==2 else 0)
  im.alpha_composite(part,(round(x-part.width/2),round(y-part.height/2)))
 d=ImageDraw.Draw(im);d.rounded_rectangle((16,12,1220,74),radius=8,fill=bg+(255,));d.text((28,20),'UltimateDesktopEffects · '+label,font=font_title,fill=(241,245,249));d.text((28,48),'Offline-Simulationsvorschau · vereinfachte Zeichnung · keine Windows-Aufnahme',font=font,fill=(178,193,210))
 im.convert('RGB').save(root/'build'/f'preview-{name}.png');thumbs.append(im.convert('RGB').resize((640,400),Image.Resampling.NEAREST))
 print(name,'flying',len(state['airborne']),'settled',sum(len(s['objects']) for s in state['surfaces']),'attached',len(state['webs']))
collage=Image.new('RGB',(1280,800))
for i,thumb in enumerate(thumbs):collage.paste(thumb,((i%2)*640,(i//2)*400))
collage.save(root/'build/effects-preview.png')
