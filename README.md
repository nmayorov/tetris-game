A simple tetris game implemented for self-education purposes. Here is a [link](https://youtu.be/-8teaOoZdxA) to a video with gameplay for illustration.

Code organization
-----------------

The game logic is implemented in `tetris.cpp`. The exact game rules and mechanics are figured from several wikis about Tetris.

Class `Piece` represents a game piece ("tetrimino"), it defines how a piece rotates and kicks off obstacles. 

Class `Board` represents the geometric state of the board. It stores which tiles are occupied, the position of the current piece and processes required motions obeying geometric constraints. Class `Tetris` operates on `Board` and defines game timings, user input processing and scoring.

The drawing functions are implemented in `render.cpp`. It defines several convenience classes to render board, pieces and text using simple OpenGL shaders.

File `utility.cpp` contains classes representing a shader, a texture and a font glyph. As well as functions to load a texture and a font from a file.

The game initialization and main loop is implemented in `game.cpp` in the most straightforward manner without farther abstractions.

Building
--------
Building was reworked and tested for Ubuntu 20.04. 
It's too much hassle to properly support all platforms because of peculiar dependency libraries.

Install required packages:
```shell
sudo apt-get install libglew-dev libfreetype-dev libglfw3-dev libglm-dev libopengl-dev
```
Then use CMake to generate and execute build. 

Make sure that `resources` folder is near the executable before running.

Credits
-------

http://learnopengl.com was amazingly helpful to learn basics of OpenGL. Some utility code was based on samples from there.

Tile sprites and font by kenney.nl.

Keyboard key sprites by Xelu.
