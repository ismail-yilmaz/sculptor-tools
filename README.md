
# SculptorTools for U++

**SculptorTools** is a comprehensive 3D graphics nest for the [U++](https://www.ultimatepp.org/) framework. It provides a unified ecosystem for embedding 3D rendering pipelines directly into standard U++ desktop applications. 

This repository contains two distinct rendering packages that share a similar API philosophy but serve different backend requirements: a pure CPU software rasterizer (**Sculptor**) and a hardware-accelerated OpenGL widget (**SculptorCtrl**).



## Sculptor (Software Rasterization)

A lightweight, fully programmable 3D software rasterization pipeline. It renders geometry directly into standard U++ `Image` buffers entirely on the CPU, providing complete 3D capabilities without relying on external hardware graphics APIs like OpenGL or Vulkan.

*   **Multithreaded Performance:** Lock-free scanline rendering automatically scales across available CPU cores using U++'s native worker threads.
*   **Programmable C++ Pipeline:** Intercept the pipeline using C++ hooks for custom texture samplers, pixel shaders (cel-shading, rim lighting), post-processing, and non-linear NDC converters (fisheye, VR).
*   **Advanced Texturing:** Features perspective-correct mapping with optional bilinear filtering and per-pixel LOD mipmapping.
*   **Asset Management:** Includes a built-in Wavefront `.obj` ASCII parser alongside procedural shape generation.


## SculptorCtrl (Hardware Acceleration)

A high-performance, hardware-accelerated 3D graphics widget built natively as a U++ `Ctrl`. Powered by core-profile OpenGL, it embeds a robust, programmable rendering pipeline seamlessly into standard U++ gui layouts.

*   **Modern OpenGL Integration:** Utilizes Vertex Array Objects (VAOs), Vertex Buffer Objects (VBOs), and hardware-accelerated rasterization via the `ModelGL` container.
*   **Programmable GLSL Shaders:** Full support for custom vertex and fragment shaders, complete with dynamic uniform binding (`float`, `Point3D`, `RGBA`) and multi-light management.
*   **Post-Processing & Screen Effects:** Built-in Framebuffer Object (FBO) support for screen-space fragment shaders, enabling retro CRT filters, barrel distortion, chromatic aberration, and bloom.
*   **Asset & Texture Management:** Automatic texture caching, GPU mipmapping, and an efficient Wavefront `.obj` compilation pipeline.

## Shared Camera & Projections

Both engines utilize a flexible matrix stack workflow and offer native support for a wide variety of camera projection matrices:
*   **Perspective**
*   **Orthographic**
*   **Frustum**
*   **True Isometric (2.5D)**


## Examples

The SculptorTools nest ships with ready-to-run interactive examples demonstrating both the software and hardware pipelines, from basic initialization to advanced shader techniques.

| Example | Description |
| :--- | :--- |
| **Sculptor::Basics** | A minimal application serving as the standard template for embedding the CPU software rasterizer into U++ apps. |
| **Sculptor::Demo** | A software-rendered showcase featuring distinct 3D models with C++ shader effects and UI interaction. |
| **SculptorCtrl::Basics** | The foundational template for embedding the hardware-accelerated OpenGL widget and integrating GLSL shaders. |
| **SculptorCtrl::Demo** | An interactive OpenGL showcase featuring hardware-accelerated rendering, multiple models, and custom screen-space FBO effects. |


