# SculptorCtrl

**SculptorCtrl** is a high-performance, hardware-accelerated 3D graphics widget (ctrl) built natively for [U++](ultimatepp.org) framework. By leveraging modern OpenGL (Core Profile), it provides a robust, programmable 3D rendering pipeline embedded directly into standard U++ window layouts.

## Features

*   **Modern OpenGL Integration:** Powered by core-profile OpenGL, utilizing Vertex Array Objects (VAOs), Vertex Buffer Objects (VBOs), and hardware-accelerated rasterization.
*   **Programmable GLSL Shaders:** Full support for custom vertex and fragment shaders, complete with dynamic uniform binding (`float`, `Point3D`, `RGBA`) and multi-light management.
*   **Post-Processing & Screen Effects:** Perfect for screen-space fragment shaders, enabling retro CRT filters, barrel distortion, chromatic aberration, and bloom.
*   **Flexible Projection Matrix Stack:** Native support for Perspective, Orthographic, Frustum, and true Isometric (2.5D) projection matrices.
*   **Asset & Texture Management:** Automatic texture caching, mipmapping, bilinear filtering, and an efficient Wavefront `.obj` model compilation pipeline via `ModelGL`.

## Examples

SculptorCtrl ships with four ready-to-run interactive examples demonstrating hardware-accelerated rendering and custom GLSL pipeline integration.

| Example       | Description |
| :---          | :---        |
| **Basics**    | A minimal application serving as the standard template for embedding SculptorCtrl into U++ apps, and utilizing shaders. |
| **Boing**     | A simple OpenGL implementation of the famous Amiga Boing Ball demo (tutorial example code).       |
| **Demo**      | A showcase featuring three distinct 3D models with various shader effects and UI interaction.     |
| **Game1**     | A rudimentary labyrinth game, demonstrating SculptorCtrl, animation and UI interaction.           |
| **Game2**     | A rudimentary asteroids clone, demonstrating SculptorCtrl, animation and UI interaction.          |


