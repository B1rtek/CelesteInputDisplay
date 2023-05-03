# CelesteDisplay

Displays what keys you're pressing in the game, simple as that. Just download the latest release and play. Oh, it's only for Windows, sorry. You can change the colors of the buttons in the `settings.txt` file, that will appear after the first run:
```
Released: 200 200 200
Pressed: 120 120 120
Opacity: 128
```
Released and Pressed are decimal RGB values, while Opacity is just an alpha value applied to all buttons. Doing weird stuff to these will most likely result in a crash. If you screw up, you can always remove this file and run the app again to generate a new one.

# Building

Requires CMake 3.11, that's basically it. After it's built, remember to copy generated DLLs from `build/_deps/sfml-build/lib` to `build`.
```shell
mkdir build && cd build
cmake ..
cmake --build .
```