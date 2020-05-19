# Mandelbrot Set
A Mandelbrot Set vizualizer made with SDL2. Up and down arrows change number of itterations,
Q and A zoom in and out, W and S increase and decrease zoom speed and click and drag
pans.

To compile:
```
g++ manbrot.cpp -O2 -lSDL2 -o manbrot
```

It's painfully slow withou `-O2` optimization. Also, I made this for Linux, so you may have
to change the `#include` of the SDL2 if you compile this for Windows.
