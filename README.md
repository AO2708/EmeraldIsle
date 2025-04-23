<a id="readme-top"></a>
<h1 align="center">Computer Graphics Project</br>Toward a Futuristic Emerald Isle</h1>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#instructions">Instructions</a></li>
        <li><a href="#technical-explanations">Technical Explanations</a></li>
        <li><a href="#rendering">Rendering</a></li>
      </ul>
    </li>
    <li>
      <a href="#how-to-use-the-application">How To Use The Application</a>
      <ul>
        <li><a href="#getting-started">Getting Started</a></li>
        <li><a href="#camera-movement">Camera Movement</a></li>
      </ul>
    </li>
    <li><a href="#license">License</a></li>

  </ol>
</details>

## About The Project
The aim of this project is to develop a computer graphics application to showcase the techniques we have learned in the module in a single framework.  
As this is my first project in computer graphics, I kindly ask for your understanding regarding any rough edges.
### Instructions
- The application is implemented in C++ using shader-based OpenGL 3.3.
- A minimum frame rate of 15 FPS must be achieved.  
- The application should demonstrate an infinite scene.  
- The camera should be controllable.
- The application sould include the four basic features covered in the module : geometry rendering, texture mapping, lighting and shadow, animation.
- The application should include an implementation of one "advanced feature" which is, in my application, the instancing.

### Technical Explanations
- The objects have all been designed in Blender, have been imported as .obj files (except the robot which has been imported as .gltf file) and finally have been rendered.
- In order to transform one rendered tree into a forest, I have implemented instancing.
- The texture mapping has been used for the texture of the sign, the texture of the robot and the texture of the sand.
- For lighting, I have assumed that the trees, the island, the sign and the robot (because it is rusted) are Lambertian surfaces and that the sea has a specular term. As a consequence, I implemented a Lambertian Illumination Model for the Lambertian surfaces and a Blinn-Phong Illumination Model for the sea. For all my objects, I used a Phong shading in order to have a better lighting on my objects because most of them have been created in ”Low Poly”.
-  For the shadow, I have implemented a shadow mapping.
-  For the animation, I have chosen to represent a robot. I created it on Blender as the animations and I imported it as a .gltf file.
-  For the infinite scene, the sea is represented as a grid and moves according to the camera movement. As an additional implementation, I used a normal mapping in order to create different illuminations on the waves. That comes in addition to the sinusoidal function which shift vertices up and down in order to create waves.

### Rendering
![Rendu][rendu-image]

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## How To Use The Application
### Getting Started
All the necessary settings for CLion are configured so that you can simply open `main.cpp` and run it.
### Camera Movement
In order to move the camera, you can :
  - use the arrow keys in order to rotate the camera;
  - use the keys Z/S (W/S for a QWERTY keyboard) in order to move forwards/backwards;
  - use the keys Q/D (A/D for a QWERTY keyboard) in order to move left/right;
  - scroll with the mouse in order to move up/down.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## License
Distributed under the MIT License. See [LICENSE.txt][license] for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

[license]: LICENSE
[rendu-image]: rendu.png
