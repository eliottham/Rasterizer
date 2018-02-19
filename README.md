This project focuses on getting familiar with OpenGL. The program takes in 
object files and renders them with old OpenGL primitives. The following 
transformations can occur on each object through user input with the keyboard.

'p'/'P': scale down/up the size of each point
'a'/'d': move left/right (along the x axis) by a small amount
'w'/'s': move up/down (along the y axis) by a small amount
'z'/'Z': move out/into the screen (along the z axis) by a small amount
'c'/'C': scale down/up (about the model's center)
'r': reset position (move objects back to center w/o changing orientation or scale
'R': reset orientation and scale factor but leave objects where they are

This program also implements my own rasterizer to display the dragon model. This
can be toggled with the 'm' key. 
 
