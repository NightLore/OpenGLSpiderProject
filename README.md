# OpenGLSpiderProject

Nathan Lui

Keyboard inputs:
'w' - move forward
'a' - move left
's' - move backward
'd' - move right
'q' - rotate camera and spider left
'e' - rotate camera and spider right
'v' - switch perspective
'z' - display triangles
'c' - change player color
'SPACE' - place cobweb

Mouse inputs:
any mouse press - blur
mouse scroll
   - vertical: move camera up and down
   - horizontal: rotate camera and spider

- Objects in the screen are randomly generated and placed in a 50 by 50 size area.
- Objects are spiders, watermelon, and crates.
- The theme of the scene is "Spiders on a boat"
- Spiders come in 7 materials:
   - blue plastic
   - flat grey
   - brass
   - copper
   - dark grey (default player color)
   - bright green
   - flat green

Collision detection implemented by bounding box collision and is individually checked by each coordinate in order to allow for slipping across planes.

Src files added:
Entity.cpp
Entity.h
Constants.h

