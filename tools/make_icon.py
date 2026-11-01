"""Compile the generated UE chef artwork to a crisp 32px source and Windows ICO frames."""
from pathlib import Path
from PIL import Image
root=Path(__file__).resolve().parents[1]
source=Image.open(root/'res/icon/ue-chef-generated.png').convert('RGBA')
# Fixed native grid, binary transparency and a small palette are the same asset
# preparation steps used for the falling 16px motifs. No new shapes are drawn.
native=source.resize((32,32),Image.Resampling.NEAREST)
alpha=native.getchannel('A').point(lambda a:255 if a>=128 else 0)
native=native.convert('RGB').quantize(colors=12,method=Image.Quantize.MEDIANCUT,dither=Image.Dither.NONE).convert('RGBA')
native.putalpha(alpha)
native.save(root/'res/icon/ue-chef-32.png')
sizes=[16,24,32,48,64,128,256]
frames=[native.resize((size,size),Image.Resampling.NEAREST) for size in sizes]
frames[-1].save(root/'res/ultimate.ico',format='ICO',sizes=[(n,n) for n in sizes],append_images=frames[:-1])
# Internal visual QA at the real native resolution, enlarged only for inspection.
(root/'build').mkdir(exist_ok=True)
native.resize((384,384),Image.Resampling.NEAREST).save(root/'build/icon-preview.png')
print('Built UE chef ICO frames:',sizes)
