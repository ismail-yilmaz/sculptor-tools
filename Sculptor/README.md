# Sculptor

Sculptor is a lightweight, fully programmable 3D software rasterization pipeline built natively for [U++](www.ultimatepp.org) framework. It renders 3D geometry directly into standard `Image` buffers entirely on the CPU, providing a complete 3D pipeline without relying on external hardware graphics APIs like OpenGL or Vulkan.

## Features

*   **Multithreaded Performance:** Lock-free scanline rendering automatically scales across available CPU cores using U++'s worker threads.
*   **Programmable Pipeline:** 
    *   **Texture Samplers:** Override lookups for procedural generation or triplanar mapping.
    *   **Pixel Shaders:** Intercept fragment data (normals, view vectors) for cel-shading, rim lighting, or custom blending.
    *   **Post-Processing:** Direct buffer access for screen-space effects (e.g., CRT barrel distortion, chromatic aberration, bloom).
    *   **NDC Converters:** Warp Normalized Device Coordinates for non-linear projections like fisheye or VR.
*   **Advanced Texturing:** Perspective-correct mapping with optional bilinear filtering and per-pixel LOD mipmapping.
*   **Flexible Cameras:** Native support for Perspective, Orthographic, Frustum, and true Isometric (2.5D) projection matrices.
*   **Asset Management:** Built-in Wavefront `.obj` ASCII model parser alongside procedural shape generation.

## Examples

Sculptor ships with three ready-to-run examples demonstrating the pipeline from basic initialization to advanced rendering techniques.

| Example       | Description |
| :---          | :---        |
| **Basics**    | A minimal application serving as the standard template for embedding Sculptor into U++ apps, and utilizing shaders. |
| **Headless**  | Same minimal example, utilizing software renderer for **headless** (no GUI code) rendering.       |
| **Demo**      | A showcase featuring three distinct 3D models with various shader effects and UI interaction.     |


