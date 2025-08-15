# zs image format

This project was made to test my basic programming skills. In college, we had an introduction to programming where I learned about:
- variable types and sizes like char, int, etc.
- flow control 
- loops
- functions
- arrays and structures
- pointers — which completely fried my brain, I didn’t understand them at all 😵

So I tried to create an image format to apply this new knowledge, and it was fun!
I built this shit using C. Man, this language is hard, but it worked in the end.

I don’t recommend actually using this format, because for example, a PNG image that’s 1.6 MiB becomes 11 MiB yeah I invented decompression. so it’s definitely not worth it. But if you're crazy:

build:

 ``gcc main.c -o zs `sdl2-config --cflags --libs` -lm``

run: 

`./exe2 "someImage.png"` or `./exe2 "someImage.png.zs"` 

 if the image have a format diferent of .zs we going to converte
if was a .zs we display the image

Dependecis are:
- STB for image load
- SDL2 for render my image format on screen


