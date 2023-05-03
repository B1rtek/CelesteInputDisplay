# CelesteInputDisplay

Displays what keys you're pressing in the game, simple as that. Just download the latest release and play. Oh, it's only for Windows, sorry.  
You can drag it around with your mouse, to close it simply press escape (while focused).

# Building

Requires CMake 3.11, that's basically it. After it's built, remember to copy generated DLLs from `build/_deps/sfml-build/lib` to `build`.
```shell
mkdir build && cd build
cmake ..
cmake --build .
```
You'll need that if you want to change the colour or opacity of the "buttons".

# Credits
Thanks to [Alia5](https://gist.github.com/Alia5) for the [Transparent SFML Window template!](https://gist.github.com/Alia5/5d8c48941d1f73c1ef14967a5ffe33d5)
