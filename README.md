# Image-Deformation

This project attempted to implement a two-dimensional Image Distortion function using OpenGL based on the C++ language, and aims to create Interactive Image Deformation haing high DOF.

## Goal
- High DOF User Interactive Program
- Local Deformation Parallel Calculation

## Local Deformation
### Vector of control point moving
For each control point, the movement from the original position to the position moved by drag is stored as a vector.

![vector](https://github.com/user-attachments/assets/a6d3f96a-a333-4423-a526-fa8071c0b6c2)

### Weight calculation using Gaussian
Set deformation weight using Gaussian equation so that pixels outside and within the deforamtion range are smoothly connected.

![gaussian](https://github.com/user-attachments/assets/ca76d9a5-ee19-4af2-a011-0c3cf5ae6d57)

###Deformed Texture coordinate calculation
Deformed Texture coordinate = Existing Texture coordinates - Vector * Weight * Intensity (constant)
