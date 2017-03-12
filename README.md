A simple tetris game implemented for self-education purposes.

Code organization
-----------------

The game logic is implemented in `tetris.cpp`. 

Class `Piece` represents a game piece ("tetrimino"), it defines how a piece rotates and kicks off obstacles. 

Class `Board` represents the geometric state of the board. It stores which tiles are occupied, the position of the current piece and processes required motions obeying geometric constraints. Class `Tetris` operates on `Board` and defines game timings, user input processing and scoring.

The drawing functions are implemented in `render.cpp`. It defines several convenience classes to render board, pieces and text using simple OpenGL shaders.

File `utility.cpp` contains classes representing a shader, a texture and a font glyph. As well as functions to load a texture and a font from a file.

The game initialization and main loop is implemented in `game.cpp` in the most straightforward manner without farther abstractions.

Building
--------
There is a project for Visual Studio 2017 in `vs2017_project` which contains all necessary dependencies. 

On OS X you should install `GLFW3`, `GLM` and `freetype2` with `homebrew` . Then use `cmake` to generate a makefile. Linux building wasn't tested.

Make sure that `resources` folder is near the executable before running.

Credits
-------

http://learnopengl.com was amazingly helpful to learn basics of OpenGL. Some utility code was based on samples from there.

Tile sprites and font by kenney.nl.

Keyboard key sprites by Xelu. 
