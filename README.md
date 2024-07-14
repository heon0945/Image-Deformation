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

### Deformed Texture coordinate calculation
Deformed Texture coordinate = Existing Texture coordinates - Vector * Weight * Intensity (constant)
   
## Execution
### Loading the program
- Change VS file, SDK file
- Change image path
### Adjust Images
- Load images
- Adjust variables (SCR_WIDTH, SCR_HEIGHT)
### Control
- Right mouse button click : Add contorl point / Delete the control point
- Left mouse button drag : Move control point
- ESC : End the program
- Up : Reduce the deformation range
- Down : Increase the deformation range
- Left : Decrease the deformation intensity
- Right : Increase the deformation intensity
- R : Show/Hide the deformation ranges
- P : Show/Hide the deformation points
- Z : Compare the before and after images
   
## Sample Output
![specific](https://github.com/user-attachments/assets/90bc2998-9f62-49eb-bf68-618169e792f1)
![overall](https://github.com/user-attachments/assets/de0f046e-085a-48c4-bd72-98b90a39369a)

