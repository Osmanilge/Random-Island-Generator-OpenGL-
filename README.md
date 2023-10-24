################
on Linux.
Osman İlge Ünaldı
201101006
Computer Graphics Assignment
################


########################## Keyboard Shortcuts ####################################################3
- 'w' : Camera mode switches to spherical/free camera mode.
- 'c' : Camera mode switches to plane front mode. Camera will be placed to in front of the plane.
- 't' : Camera mode switches to plane back mode. Camera will be placed to behind the plane.
- 'f' : Shading mode will be switched between flat shading and grouraud shading.(default = grouraud )
- 'a' : Slows down the plane.
- 'd' : Speeds up the plane.
- Right arrow key rotates plane right in x-y axis
- Left arrow key rotates plane left in x-y axis



g++ Render.cpp InitShader.cpp -lGLEW -lGLU -lGL -lglut -o ex1

./ex1
