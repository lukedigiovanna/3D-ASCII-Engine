# 3D ASCII Engine
C++ Application which can render 3D scenes using nothing but the standard library and output terminal.

*This only works on Windows!*

## Getting a screen to draw to

This is done entirely using the standard output, which can actually be quite powerful when using C++. We can get a pointer to the console output using the Windows API and then easily call a function to draw characters at particular coordinates. There is also capabilities to check the size of the output to handle dynamic resizing of the render buffer. Extensions to this project can include adding color

## Matrix math

3D graphics are built largely on the use of 4x4 matrices, so the first task was to put together a Matrix4f class (4x4 matrix of floats). This class supports basic matrix operations such as multiplication, identities, and important 3D functions such as applying linear transormations like scaling, translating, and rotating as well as applying the perspective transformation.

## Drawing Modes

### Mesh frame mode

Renders the outline of a 3D mesh as opposed to filling in the spaces. This does not account for depth buffering like rasterization does and you can "see through" objects. This approach to rendering is far simpler due to these allowances. Notice it actually renders the cube using a bunch of triangles. This is a common approach for 3D graphics by represeting our models as none other than a bunch of triangles. This simplifies the projection and rasterization process as it is easy to rasterize triangles to the screen.

![mesh](https://i.ibb.co/pnVSzhB/snip2.png)

### Rasterization mode (w/ depth buffering)
The standard way we view the 3D world. How it works in the code is the model will compute it's perspective transformation onto a 4x4 matrix. This transformation will project the scene onto the 2D plane (our screen), however it will also "project" onto the z-axis as well. This can be thought of as the axis that extends into and out of your screen. We simply only render pixels which have the greatest projected z-coordinate compared to all other projections at that point. This provides depth buffering so we cannot see through walls.

![rasterization](https://i.ibb.co/Jkt1CjW/snip.png)

