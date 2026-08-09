topic "Sculptor";
[i448;a25;kKO9;2 $$1,0#37138531426314131252341829483380:class]
[l288;2 $$2,2#27521748481378242620020725143825:desc]
[0 $$3,0#96390100711032703541132217272105:end]
[H6;0 $$4,0#05600065144404261032431302351956:begin]
[i448;a25;kKO9;2 $$5,0#37138531426314131252341829483370:item]
[l288;a4;*@5;1 $$6,6#70004532496200323422659154056402:requirement]
[l288;i1121;b17;O9;~~~.1408;2 $$7,0#10431211400427159095818037425705:param]
[i448;b42;O9;2 $$8,8#61672508125594000341940100500538:tparam]
[b42;2 $$9,9#13035079074754324216151401829390:normal]
[2 $$0,0#00000000000000000000000000000000:Default]
[{_} 
[ {{10000@(113.42.0) [s0;%% [*@7;4 Sculptor]]}}&]
[s3; &]
[s1;:Upp`:`:Sculptor: [@(0.0.255)3 class][3  ][*3 Sculptor]&]
[s2;%% This class is the primary rendering context and multi`-pass 
3D software rasterizer. It manages the viewport state, camera 
matrices, lighting environments, and custom shader hooks.&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Nested Structures]]}}&]
[s3; &]
[s1;:Upp`:`:Sculptor`:`:Primitive: [@(0.0.255)3 struct][3  ][*3 Primitive][3  
][@(0.0.255)3 :][3  Moveable<][*3 Primitive][3 >]&]
[s2;%% Holds pre`-transformed, near`-plane clipped triangle data 
ready for rasterization, including screen coordinates, inverse`-w 
values for perspective correction, view`-space positions, normals, 
and material flags.&]
[s3; &]
[s1;:Upp`:`:Sculptor`:`:Fragment: [@(0.0.255)3 struct][3  ][*3 Fragment][3  
][@(0.0.255)3 :][3  Moveable<][*3 Fragment][3 >]&]
[s2;%% Assembled dynamically for every rasterized pixel, containing 
interpolated per`-fragment attributes ([C@(0.0.255) viewpos], [C@(0.0.255) normal], 
[C@(0.0.255) texcoord], [C@(0.0.255) color], [C@(0.0.255) texture], 
[C@(0.0.255) mipmaps]) handed directly to custom texture samplers 
and pixel shaders.&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Constructor detail]]}}&]
[s3; &]
[s5;:Upp`:`:Sculptor`:`:Sculptor`(`): [* Sculptor]()&]
[s2;%% Default constructor.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Sculptor`(Size`,const RGBA`&`): [* Sculptor](Size 
[*@3 size], [@(0.0.255) const] RGBA[@(0.0.255) `&] [*@3 paper])&]
[s2;%% Constructor override. Initializes the software rasterizer, 
allocating the internal color buffer and Z`-buffer to match the 
specified target viewport [%-*@3 size], and clears the background 
to [%-*@3 paper ] color.&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Public Method List]]}}&]
[s3; &]
[s5;:Upp`:`:Sculptor`:`:Co`(bool`): Sculptor[@(0.0.255) `&] [* Co]([@(0.0.255) bool] 
[*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles parallel multithreaded scanline rendering across CPU 
cores. Disabled by default. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:IsCo`(`)const: [@(0.0.255) bool] [* IsCo]() [@(0.0.255) const]&]
[s2;%% Returns true if parallel multithreaded scanline rendering 
is enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Cull`(bool`): Sculptor[@(0.0.255) `&] [* Cull]([@(0.0.255) bool] 
[*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles backface culling to skip triangles facing away from 
the camera. Enabled by default. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:IsCulling`(`)const: [@(0.0.255) bool] [* IsCulling]() 
[@(0.0.255) const]&]
[s2;%% Returns true if backface culling is enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Wireframe`(bool`): Sculptor[@(0.0.255) `&] 
[* Wireframe]([@(0.0.255) bool] [*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles wireframe overlay rendering mode. Disabled by default. 
Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:IsWireFrame`(`)const: [@(0.0.255) bool] [* IsWireFrame]() 
[@(0.0.255) const]&]
[s2;%% Returns true if wireframe overlay rendering mode is enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Shading`(bool`): Sculptor[@(0.0.255) `&] [* Shading]([@(0.0.255) bo
ol] [*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles standard normal`-based directional lighting calculations. 
Enabled by default. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:IsShading`(`)const: [@(0.0.255) bool] [* IsShading]() 
[@(0.0.255) const]&]
[s2;%% Returns true if standard normal`-based directional lighting 
calculations are enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:AlphaBlend`(bool`): Sculptor[@(0.0.255) `&] 
[* AlphaBlend]([@(0.0.255) bool] [*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles translucent alpha blending mode for transparent or 
semi`-transparent surfaces. Disabled by default. Returns `*this 
for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:IsAlphaBlending`(`)const: [@(0.0.255) bool] 
[* IsAlphaBlending]() [@(0.0.255) const]&]
[s2;%% Returns true if translucent alpha blending mode for transparent 
or semi`-transparent surfaces is enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Bilinear`(bool`): Sculptor[@(0.0.255) `&] [* Bilinear]([@(0.0.255) b
ool] [*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles bilinear texture filtering for smooth texture magnification 
and minification. Disabled by default. Returns `*this for method 
chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:IsBilinear`(`)const: [@(0.0.255) bool] [* IsBilinear]() 
[@(0.0.255) const]&]
[s2;%% Returns true if bilinear texture filtering for smooth texture 
magnification and minification is enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:MipMap`(bool`): Sculptor[@(0.0.255) `&] [* MipMap]([@(0.0.255) bool
] [*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Enables or disables mipmap`-based texture sampling. Disabled 
by default. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:IsMipMapping`(`)const: [@(0.0.255) bool] [* IsMipMapping]() 
[@(0.0.255) const]&]
[s2;%% Returns true if mipmapping is enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:AmbientLight`(const RGBA`&`): Sculptor[@(0.0.255) `&] 
[* AmbientLight]([@(0.0.255) const] RGBA[@(0.0.255) `&] [*@3 c])&]
[s2;%% Sets the global baseline ambient illumination color to [%-*@3 c]. 
Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:SetCanvas`(Size`,const RGBA`&`): [@(0.0.255) void] 
[* SetCanvas](Size [*@3 size], [@(0.0.255) const] RGBA[@(0.0.255) `&] 
[*@3 paper])&]
[s2;%% Resizes and reallocates the internal color buffer and depth 
buffer to [%-*@3 sz], resetting the background to [%-*@3 paper] color.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:ClearCanvas`(const RGBA`&`): [@(0.0.255) void] 
[* ClearCanvas]([@(0.0.255) const] RGBA[@(0.0.255) `&] [*@3 paper])&]
[s2;%% Clears both the color buffer using [%-*@3 paper] color and the 
depth buffer, preparing the canvas for a new frame.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:ClearColorBuffer`(const RGBA`&`): [@(0.0.255) void] 
[* ClearColorBuffer]([@(0.0.255) const] RGBA[@(0.0.255) `&] [*@3 paper])&]
[s2;%% Clears only the internal color buffer using [%-*@3 paper ]color, 
leaving the depth buffer intact.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:ClearDepthBuffer`(`): [@(0.0.255) void] [* ClearDepthBuffer]()&]
[s2;%% Resets only the depth buffer (Z`-buffer) to the maximum distance, 
leaving the color buffer intact.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Perspective`(float`,float`,float`,float`): [@(0.0.255) void] 
[* Perspective]([@(0.0.255) float] [*@3 fov], [@(0.0.255) float] [*@3 aspect], 
[@(0.0.255) float] [*@3 znear], [@(0.0.255) float] [*@3 zfar])&]
[s2;%% Configures a perspective projection matrix. The field`-of`-view 
[%-*@3 fov] is specified in radians, [%-*@3 aspect] represents the 
viewport aspect ratio width`-to`-height, and [%-*@3 znear]/[%-*@3 zfar] 
define the depth clipping planes.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Orthographic`(const Rectf`&`,float`,float`): [@(0.0.255) void] 
[* Orthographic]([@(0.0.255) const] Rectf[@(0.0.255) `&] [*@3 v], [@(0.0.255) float] 
[*@3 znear], [@(0.0.255) float] [*@3 zfar])&]
[s2;%% Configures an orthographic projection matrix using view rectangle 
bounds [%-*@3 v] along with [%-*@3 znear] and [%-*@3 zfar] clipping 
planes.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Frustum`(const Rectf`&`,float`,float`): [@(0.0.255) void] 
[* Frustum]([@(0.0.255) const] Rectf[@(0.0.255) `&] [*@3 view], [@(0.0.255) float] 
[*@3 fnear], [@(0.0.255) float] [*@3 ffar])&]
[s2;%% Configures a perspective projection matrix using an asymmetric 
viewing frustum. The [%-*@3 view] rectangle defines the left, right, 
top, and bottom coordinates at the near clipping plane ([%-*@3 fnear]), 
while [%-*@3 ffar] defines the far depth clipping plane.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Isometric`(const Rectf`&`,float`,float`): [@(0.0.255) void] 
[* Isometric]([@(0.0.255) const] Rectf[@(0.0.255) `&] [*@3 view], [@(0.0.255) float] 
[*@3 fnear], [@(0.0.255) float] [*@3 ffar])&]
[s2;%% Configures a true isometric projection matrix. It establishes 
an orthographic viewing volume defined by [%-*@3 view], [%-*@3 fnear], 
and [%-*@3 ffar] , and then automatically applies the precise mathematical 
rotations (45° around the Y`-axis and 35.264° around the X`-axis) 
required to achieve a classic 2.5D isometric perspective.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Isometric`(float`,float`,float`,float`): [@(0.0.255) void] 
[* Isometric]([@(0.0.255) float] [*@3 zoom], [@(0.0.255) float] [*@3 aspectratio], 
[@(0.0.255) float] [*@3 fnear], [@(0.0.255) float] [*@3 ffar])&]
[s2;%% A convenience overload for configuring an isometric projection 
matrix. It automatically calculates the orthographic view bounds 
using the provided [%-*@3 zoom] scale and viewport [%-*@3 aspectratio], 
alongside the [%-*@3 fnear] and [%-*@3 ffar] depth clipping planes.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:LookAt`(const Point3D`&`,const Point3D`&`,const Point3D`&`): [@(0.0.255) v
oid] [* LookAt]([@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 eye], 
[@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 center], [@(0.0.255) const] 
Point3D[@(0.0.255) `&] [*@3 up])&]
[s2;%% Configures the camera view matrix, positioning the camera 
at [%-*@3 eye], aiming it toward [%-*@3 center], and orienting its 
upward vertical axis along vector [%-*@3 up].&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:PushMatrix`(`): [@(0.0.255) void] [* PushMatrix]()&]
[s2;%% Duplicates the top matrix on the internal matrix stack and 
pushes it to the top.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:PopMatrix`(`): [@(0.0.255) void] [* PopMatrix]()&]
[s2;%% Removes the top transformation matrix from the stack, restoring 
the previous state.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Identity`(`): Sculptor[@(0.0.255) `&] [* Identity]()&]
[s2;%% Resets the top matrix of the stack to an identity matrix. 
Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Translate`(float`,float`,float`): Sculptor[@(0.0.255) `&] 
[* Translate]([@(0.0.255) float] [*@3 x], [@(0.0.255) float] [*@3 y], [@(0.0.255) float] 
[*@3 z])&]
[s5;:Upp`:`:Sculptor`:`:Translate`(const Point3D`&`): Sculptor[@(0.0.255) `&] 
[* Translate]([@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 p])&]
[s2;%% Shifts the geometry along the active coordinate axes by [%-*@3 x], 
[%-*@3 y], and [%-*@3 z], or by [%-*@3 p]. If called after a Rotate, 
the translation occurs along the newly rotated local axes. Returns 
`*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Rotate`(float`,float`,float`): Sculptor[@(0.0.255) `&] 
[* Rotate]([@(0.0.255) float] [*@3 rx], [@(0.0.255) float] [*@3 ry], [@(0.0.255) float] 
[*@3 rz])&]
[s5;:Upp`:`:Sculptor`:`:Rotate`(const Point3D`&`): Sculptor[@(0.0.255) `&] 
[* Rotate]([@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 p])&]
[s2;%% Applies a 3D rotation to the geometry. The angles [%-*@3 rx] 
, [%-*@3 ry] , and [%-*@3 rz], or [%-*@3 p] must be provided in radians. 
The rotations are applied internally in a fixed order (typically 
X, then Y, then Z axis). To achieve complex quaternion`-like 
orbital rotations, you should apply separate, sequential Rotate() 
calls. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Scale`(float`,float`,float`): Sculptor[@(0.0.255) `&] 
[* Scale]([@(0.0.255) float] [*@3 sx], [@(0.0.255) float] [*@3 sy], [@(0.0.255) float] 
[*@3 sz])&]
[s5;:Upp`:`:Sculptor`:`:Scale`(const Point3D`&`): Sculptor[@(0.0.255) `&] 
[* Scale]([@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 p])&]
[s2;%% Applies non`-uniform scaling to stretch or shrink geometry 
independently along the local X, Y, and Z axes. Returns `*this 
for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Scale`(float`): Sculptor[@(0.0.255) `&] [* Scale]([@(0.0.255) float
] [*@3 scale])&]
[s2;%% Uniform scaling overload applying scalar [%-*@3 scale] equally 
across all three axes to resize an object without distorting 
its proportions. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:SetLight`(const Point3D`&`,const RGBA`&`): [@(0.0.255) void] 
[* SetLight]([@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 dir], [@(0.0.255) const] 
RGBA[@(0.0.255) `&] [*@3 color])&]
[s2;%% Clears all existing light sources and establishes a single 
directional light with normalized [%-*@3 direction ]vector and 
light [%-*@3 color].&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:AddLight`(const Point3D`&`,const RGBA`&`): Sculptor[@(0.0.255) `&
] [* AddLight]([@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 dir], [@(0.0.255) const] 
RGBA[@(0.0.255) `&] [*@3 color])&]
[s2;%% Appends an additional directional light source with [%-*@3 direction] 
and [%-*@3 color]. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:ClearLights`(`): [@(0.0.255) void] [* ClearLights]()&]
[s2;%% Removes all registered directional light sources from the 
scene.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:SetNDCConverter`(Function`): [@(0.0.255) void] 
[* SetNDCConverter](Function<Pointf ([@(0.0.255) const] Point3D[@(0.0.255) `&])> 
[*@3 fn])&]
[s6;%% Pipeline stage: post`-projection, pre`-rasterization.&]
[s2;%% Overrides how 3D Normalized Device Coordinates (NDC) are mapped 
onto 2D screen pixels. By default, Sculptor linearly maps X and 
Y to the viewport width and height. &]
[s2;%% &]
[s2;%% By providing a custom callback [%-*@3 fn], you can implement 
non`-linear viewport distortions, such as fisheye lenses, spherical 
projections, or custom stereoscopic VR warping.&]
[s3;%% &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:SetTextureSampler`(Function`): [@(0.0.255) void] 
[* SetTextureSampler](Function<RGBA ([@(0.0.255) const] Fragment[@(0.0.255) `&])> 
[*@3 fn])&]
[s6; Pipeline stage: Per`-pixel rasterization (texture lookup).&]
[s2;%% Bypasses the default bilinear/nearest texture fetching. For 
every pixel drawn, Sculptor passes the interpolated [^topic`:`/`/Sculptor`/src`/Upp`_Sculptor`_en`-us`#Upp`:`:Sculptor`:`:Fragment^ F
ragment ]state to callback [%-*@3 fn] . You must return an RGBA 
color.&]
[s2;%% &]
[s2;%% Can be used with procedural textures (e.g., generating checkerboards 
or noise mathematically), triplanar mapping (reading the fragment`'s 
normal to sample textures based on world projection rather than 
UVs), or animated UV scrolling.&]
[s3;%% &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:SetPixelShader`(Function`): [@(0.0.255) void] 
[* SetPixelShader](Function<RGBA ([@(0.0.255) const] Fragment[@(0.0.255) `&], 
RGBA)> [*@3 fn])&]
[s6;%% Pipeline stage: Per`-pixel rasterization (final output).&]
[s2;%% Intercepts the pixel color after standard lighting and texture 
sampling have been calculated (passed in as base). Callback [%-*@3 fn] 
receives the full [^topic`:`/`/Sculptor`/src`/Upp`_Sculptor`_en`-us`#Upp`:`:Sculptor`:`:Fragment^ F
ragment ]context (including normals and view vectors) and the 
base color, and must return the final color written to the framebuffer.&]
[s2;%% &]
[s2;%% Can be used to implement cel`-Shading (quantizing the base 
color based on lighting thresholds), Rim Lighting (brightening 
edges where the dot product of the fragment`'s normal and view 
vector approaches zero), or normal mapping.&]
[s3;%% &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:AddPostFilter`(Event`): Sculptor[@(0.0.255) `&] 
[* AddPostFilter](Event<ImageBuffer[@(0.0.255) `&], Buffer<[@(0.0.255) float]>[@(0.0.255) `&
], Size> [*@3 fn])&]
[s6;%% pipeline stage: Post`-render.&]
[s2;%% Registers a full`-screen effect callback that executes exactly 
once per frame after Rasterize() has finished drawing all geometry. 
The callback [%-*@3 fn] provides direct, lock`-free memory access 
to the completed color ImageBuffer, the Buffer<float> Z`-buffer, 
and the canvas size.&]
[s2;%% &]
[s2;%% Can be used for screen`-space effects like Vignetting, Depth`-of`-Field 
(blurring the color buffer based on the corresponding Z`-buffer 
values), Bloom, or global color grading.&]
[s3;%% &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:ClearPostFilters`(`): [@(0.0.255) void] [* ClearPostFilters]()&]
[s2;%% Removes all registered post`-processing filter callbacks.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:GetPrimitives`(`)const: [@(0.0.255) const] 
Vector<Primitive>[@(0.0.255) `&] [* GetPrimitives]() [@(0.0.255) const]&]
[s2;%% Returns the read`-only list of currently assembled and transformed 
primitives mapped to screen space.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:ClearScene`(`): [@(0.0.255) void] [* ClearScene]()&]
[s2;%% Flushes the internal primitive list queue without modifying 
the underlying color buffer, depth buffer, or matrix stack.&]
[s3;%% &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Render`(const Model3D`&`): [@(0.0.255) void] 
[* Render]([@(0.0.255) const] Model3D[@(0.0.255) `&] [*@3 model])&]
[s2;%% Transforms, applies 3D clipping to, and converts triangles 
from [%-*@3 model] into screen`-space primitives using the active 
matrix stack.&]
[s3; &]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Rasterize`(`): [@(0.0.255) void] [* Rasterize]()&]
[s2;%% Executes the multi`-pass software rendering engine. It handles 
opaque Z`-buffer population, back`-to`-front sorting and blending 
of translucent geometry, wireframe overlays, and post`-processing 
filters.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:GetImage`(`): Image [* GetImage]()&]
[s2;%% Extracts and returns the finalized color buffer frame as an 
immutable Image.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:operator Image`(`): operator Image()&]
[s2;%% Implicit conversion operator allowing direct casting of a 
Sculptor instance into an Image.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Sculptor`:`:Clear`(`): [@(0.0.255) void] [* Clear]()&]
[s2;%% Performs a hard reset of the entire Sculptor object, clearing 
buffers, models, matrices, and settings.&]
[s3; &]
[s0;%% ]]