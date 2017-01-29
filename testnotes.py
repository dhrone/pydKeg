import graphics as g
import fonts
fp = fonts.size5x8.latin1.fontpkg
buf = { }
g.message(buf,"12:35 pm",0,0, fp)
g.show(buf,0,0,10,100)
buf1 = { }
buf2 = { }
buf3 = { }
g.message(buf1,"P\nL\nA\nY",0,0,fp)
g.message(buf2,"This\nis\nthe\nmessage",0,0,fp)


import graphics as g
import fonts
import numpy as np
import time
fp = fonts.size5x8.latin1.fontpkg
from PIL import Image


img = Image.new( "1", (156,20), 0)
msg = "Prince and the Revolutions\nPurple Rain"
width = g.msgwidth(msg, fp)[0]
g.message(img,msg,0,0, fp)
fb = g.getframe(img, 0,0,width,16)
g.show(fb,width,2)

simg = img.crop((0,0,width+20,16))

t = [0,0,0]
for i in range(0,16):
  start = time.time()
  g.scrollbuffer(simg, 'down')
  t[0] += time.time() - start
  start = time.time()
  fb = g.getframe(simg, 0,0,width+20,16)
  t[1] += time.time() - start
  start = time.time()
  print '-------------------------------------------------'
  g.show(fb,width+20,2)
  print '-------------------------------------------------'
  t[2] += time.time() - start

print "Time in scroll {0}\nTime in Frame {1}\nTime in Show {2}".format(t[0],t[1],t[2])
#

g.line(simg,0,0,0,8) # vertical left line
g.line(simg,0,0,100,0) # horizontal top line
g.line(simg,99,0,99,8) # vertical right line
g.line(simg,0,8,100,8) # horizontal bottom line
fb = g.getframe(simg,0,0,width+20,16)
g.show(fb,width+20,2)

cols = 100
rows = 16
g.line(img,0,0,cols,rows,1)
fb = g.getframe(img,0,0,cols,rows)
g.show(fb,100,2)
j=0
for i in range(0,99):
  j = i+1
  g.line(img,i,0,cols-i,rows,0)
  g.line(img,j,0,cols-j,rows,1)
  fb = g.getframe(img,0,0,cols,rows)
  g.show(fb,100,2)

g.line(img,j,0,cols-j,rows,0)
fb = g.getframe(img,0,0,cols,rows)
g.show(fb,100,2)

g.line(img,cols,0,0,rows,1)
fb = g.getframe(img,0,0,cols,rows)
g.show(fb,100,2)
for i in range(0,16):
  j = i+1
  g.line(img,cols,i,0,rows-i,0)
  g.line(img,cols,j,0,rows-j,1)
  fb = g.getframe(img,0,0,cols,rows)
  g.show(fb,100,2)

g.line(img,0,0,cols,rows,0)
fb = g.getframe(img,0,0,cols,rows)
g.show(fb,100,2)


#-----------------------


import graphics as g
import fonts
import numpy as np
import time
fp = fonts.size5x8.latin1.fontpkg
from PIL import Image

img = Image.new( "1", (96,128), 0)

for i in range(0,16):
  for j in range(0,16):
    g.message(img,unichr((i*16)+j),j*6,i*8,fp)

img.save("latin1_5x8.png", "PNG")


import bmfont
font = bmfont.bmfont('latin1_5x8.fnt')


import graphics as g
import fonts
from PIL import Image

font = fonts.bmfont.bmfont('latin1_5x8.fnt')
img = Image.new( "1", (156,20), 0)
msg = "Prince and the Revolutions\nPurple Rain"
width = g.msgwidth(msg, font.fontpkg)[0]
g.message(img,msg,0,0, font.fontpkg)
fb = g.getframe(img, 0,0,width,16)
g.show(fb,width,2)
