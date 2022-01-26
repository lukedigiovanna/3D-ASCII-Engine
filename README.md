# 3D ASCII Engine
C++ Application which can render 3D scenes using nothing but the standard library and output terminal.

## Drawing Modes

### Rasterization mode
The standard way we view the 3D world. 

![rasterization]url(https://i.ibb.co/Jkt1CjW/snip.png)

### Mesh frame mode

Renders the outline of a 3D mesh as opposed to filling in the spaces. This does not account for depth buffering like rasterization does and you can "see through" objects. This approach to rendering is far simpler due to these allowances.

## Approach

3D graphics are built largely on the use of 4x4 matrices, so the first task was to put together a Matrix4f class (4x4 matrix of floats). 
