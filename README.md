# CelesteDisplay

Displays what keys you're pressing in the game, simple as that. Just download the latest release and play. Oh, it's only for Windows, sorry.

# Building

Requires CMake 3.11, that's basically it. After it's built, remember to copy generated DLLs from `build/_deps/sfml-build/lib` to `build`.
```shell
mkdir build && cd build
cmake ..
cmake --build .
```
You'll need that if you want to change the colour or opacity of the "buttons".