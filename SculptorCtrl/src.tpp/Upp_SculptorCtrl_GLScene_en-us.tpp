topic "GLScene";
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
[ {{10000@(113.42.0) [s0;%% [*@7;4 GLScene]]}}&]
[s3; &]
[s1;:Upp`:`:GLScene: [@(0.0.255)3 class][3  ][*3 GLScene]&]
[s2;%% This class manages the active OpenGL rendering context, state 
machine, and command submission pipeline. Passed directly into 
the [^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_en`-us`#Upp`:`:SculptorCtrl`:`:Render`(GLScene`&`)^ R
ender()] hook, GLScene provides methods for camera projections, 
transformation matrices, lighting configuration, and drawing 
operations.&]
[s0; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Constructor detail]]}}&]
[s3; &]
[s5;:Upp`:`:GLScene`:`:GLScene`(GLPipeline`&`): [* GLScene](GLPipeline[@(0.0.255) `&] 
[*@3 pipeline])&]
[s2;%% Initializes a new GLScene instance bound to the provided underlying 
GLPipeline.&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Public Method List]]}}&]
[s3; &]
[s5;:Upp`:`:GLScene`:`:Cull`(bool`): GLScene[@(0.0.255) `&] [* Cull]([@(0.0.255) bool] 
[*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles backface culling to skip triangles facing away from 
the camera. Enabled by default. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:IsCulling`(`)const: [@(0.0.255) bool] [* IsCulling]() 
[@(0.0.255) const]&]
[s2;%% Returns true if backface culling is enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Wireframe`(bool`): GLScene[@(0.0.255) `&] [* Wireframe]([@(0.0.255) b
ool] [*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles wireframe overlay rendering mode. Disabled by default. 
Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:IsWireFrame`(`)const: [@(0.0.255) bool] [* IsWireFrame]() 
[@(0.0.255) const]&]
[s2;%%  Returns true if wireframe overlay rendering mode is enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Shading`(bool`): GLScene[@(0.0.255) `&] [* Shading]([@(0.0.255) bool
] [*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles standard normal`-based directional lighting calculations. 
Enabled by default. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:IsShading`(`)const: [@(0.0.255) bool] [* IsShading]() 
[@(0.0.255) const]&]
[s2;%% Returns true if standard normal`-based directional lighting 
calculations are enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:AlphaBlend`(bool`): GLScene[@(0.0.255) `&] [* AlphaBlend]([@(0.0.255) b
ool] [*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles translucent alpha blending mode for transparent or 
semi`-transparent surfaces. Disabled by default. Returns `*this 
for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:IsAlphaBlending`(`)const: [@(0.0.255) bool] 
[* IsAlphaBlending]() [@(0.0.255) const]&]
[s2;%% Returns true if translucent alpha blending mode for transparent 
or semi`-transparent surfaces is enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Bilinear`(bool`): GLScene[@(0.0.255) `&] [* Bilinear]([@(0.0.255) bo
ol] [*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles bilinear texture filtering for smooth texture magnification 
and minification. Disabled by default. Returns `*this for method 
chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:IsBilinear`(`)const: [@(0.0.255) bool] [* IsBilinear]() 
[@(0.0.255) const]&]
[s2;%% Returns true if bilinear texture filtering for smooth texture 
magnification and minification is enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:MipMap`(bool`): GLScene[@(0.0.255) `&] [* MipMap]([@(0.0.255) bool] 
[*@3 b] [@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Toggles mipmap`-based texture sampling. Returns `*this for 
method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:IsMipMapping`(`)const: [@(0.0.255) bool] [* IsMipMapping]() 
[@(0.0.255) const]&]
[s2;%% Returns true if mipmapping is enabled.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Perspective`(float`,float`,float`,float`): [@(0.0.255) void] 
[* Perspective]([@(0.0.255) float] [*@3 fov], [@(0.0.255) float] [*@3 aspect], 
[@(0.0.255) float] [*@3 znear], [@(0.0.255) float] [*@3 zfar])&]
[s2;%% Configures a perspective projection matrix. The field`-of`-view 
[%-*@3 fov] is specified in radians, [%-*@3 aspect] represents the 
viewport aspect ratio width`-to`-height, and [%-*@3 znear]/[%-*@3 zfar] 
define the depth clipping planes.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Orthographic`(const Rectf`&`,float`,float`): [@(0.0.255) void] 
[* Orthographic]([@(0.0.255) const] Rectf[@(0.0.255) `&] [*@3 view], 
[@(0.0.255) float] [*@3 znear], [@(0.0.255) float] [*@3 zfar])&]
[s2;%% Configures an orthographic projection matrix using view rectangle 
bounds [%-*@3 v] along with [%-*@3 znear] and [%-*@3 zfar] clipping 
planes.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Frustum`(const Rectf`&`,float`,float`): [@(0.0.255) void] 
[* Frustum]([@(0.0.255) const] Rectf[@(0.0.255) `&] [*@3 view], [@(0.0.255) float] 
[*@3 fnear], [@(0.0.255) float] [*@3 ffar])&]
[s2;%% Configures a perspective projection matrix using an asymmetric 
viewing frustum. The [%-*@3 view] rectangle defines the left, right, 
top, and bottom coordinates at the near clipping plane ([%-*@3 fnear]), 
while [%-*@3 ffar] defines the far depth clipping plane.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Isometric`(const Rectf`&`,float`,float`): [@(0.0.255) void] 
[* Isometric]([@(0.0.255) const] Rectf[@(0.0.255) `&] [*@3 view], [@(0.0.255) float] 
[*@3 fnear], [@(0.0.255) float] [*@3 ffar])&]
[s2;%% Configures a true isometric projection matrix. It establishes 
an orthographic viewing volume defined by [%-*@3 view], [%-*@3 fnear], 
and [%-*@3 ffar] , and then automatically applies the precise mathematical 
rotations (45° around the Y`-axis and 35.264° around the X`-axis) 
required to achieve a classic 2.5D isometric perspective.&]
[s3;%% &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Isometric`(float`,float`,float`,float`): [@(0.0.255) void] 
[* Isometric]([@(0.0.255) float] [*@3 zoom], [@(0.0.255) float] [*@3 aspectratio], 
[@(0.0.255) float] [*@3 fnear], [@(0.0.255) float] [*@3 ffar])&]
[s2;%% A convenience overload for configuring an isometric projection 
matrix. It automatically calculates the orthographic view bounds 
using the provided [%-*@3 zoom] scale and viewport [%-*@3 aspectratio], 
alongside the [%-*@3 fnear] and [%-*@3 ffar] depth clipping planes.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:LookAt`(const Point3D`&`,const Point3D`&`,const Point3D`&`): [@(0.0.255) v
oid] [* LookAt]([@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 eye], 
[@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 center], [@(0.0.255) const] 
Point3D[@(0.0.255) `&] [*@3 up])&]
[s2;%% Configures the camera view matrix, positioning the camera 
at [%-*@3 eye], aiming it toward [%-*@3 center], and orienting its 
upward vertical axis along vector [%-*@3 up].&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:PushMatrix`(`): [@(0.0.255) void] [* PushMatrix]()&]
[s2;%% Duplicates the top matrix on the internal matrix stack and 
pushes it to the top.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:PopMatrix`(`): [@(0.0.255) void] [* PopMatrix]()&]
[s2;%% Removes the top transformation matrix from the stack, restoring 
the previous state.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Identity`(`): GLScene[@(0.0.255) `&] [* Identity]()&]
[s2;%% Resets the top matrix of the stack to an identity matrix. 
Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Translate`(float`,float`,float`): GLScene[@(0.0.255) `&] 
[* Translate]([@(0.0.255) float] [*@3 x], [@(0.0.255) float] [*@3 y], [@(0.0.255) float] 
[*@3 z])&]
[s5;:Upp`:`:GLScene`:`:Translate`(const Point3D`&`): GLScene[@(0.0.255) `&] 
[* Translate]([@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 p])&]
[s2;%% Shifts the geometry along the active coordinate axes by [%-*@3 x], 
[%-*@3 y], and [%-*@3 z], or by [%-*@3 p]. If called after a Rotate, 
the translation occurs along the newly rotated local axes. Returns 
`*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Rotate`(float`,float`,float`): GLScene[@(0.0.255) `&] 
[* Rotate]([@(0.0.255) float] [*@3 rx], [@(0.0.255) float] [*@3 ry], [@(0.0.255) float] 
[*@3 rz])&]
[s5;:Upp`:`:GLScene`:`:Rotate`(const Point3D`&`): GLScene[@(0.0.255) `&] 
[* Rotate]([@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 p])&]
[s2;%% Applies a 3D rotation to the geometry. The angles [%-*@3 rx] 
, [%-*@3 ry] , and [%-*@3 rz], or [%-*@3 p] must be provided in radians. 
The rotations are applied internally in a fixed order (typically 
X, then Y, then Z axis). To achieve complex quaternion`-like 
orbital rotations, you should apply separate, sequential Rotate() 
calls. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Scale`(float`,float`,float`): GLScene[@(0.0.255) `&] 
[* Scale]([@(0.0.255) float] [*@3 sx], [@(0.0.255) float] [*@3 sy], [@(0.0.255) float] 
[*@3 sz])&]
[s2;%% Applies non`-uniform scaling to stretch or shrink geometry 
independently along the local X, Y, and Z axes. Returns `*this 
for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Scale`(float`): GLScene[@(0.0.255) `&] [* Scale]([@(0.0.255) float] 
[*@3 scale])&]
[s2;%% Uniform scaling overload applying scalar [%-*@3 scale] equally 
across all three axes to resize an object without distorting 
its proportions. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:SetLight`(const Point3D`&`,const RGBA`&`): [@(0.0.255) void] 
[* SetLight]([@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 dir], [@(0.0.255) const] 
RGBA[@(0.0.255) `&] [*@3 color])&]
[s2;%% Clears all existing light sources and establishes a single 
directional light with normalized [%-*@3 direction ]vector and 
light [%-*@3 color].&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:AddLight`(const Point3D`&`,const RGBA`&`): GLScene[@(0.0.255) `&
] [* AddLight]([@(0.0.255) const] Point3D[@(0.0.255) `&] [*@3 dir], [@(0.0.255) const] 
RGBA[@(0.0.255) `&] [*@3 color])&]
[s2;%% Appends an additional directional light source with [%-*@3 direction] 
and [%-*@3 color]. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:ClearLights`(`): [@(0.0.255) void] [* ClearLights]()&]
[s2;%% Removes all registered directional light sources from the 
scene.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:AddOverlay`(const Point`&`,const Image`&`): GLScene[@(0.0.255) `&
] [* AddOverlay]([@(0.0.255) const] Point[@(0.0.255) `&] [*@3 pos], [@(0.0.255) const] 
Image[@(0.0.255) `&] [*@3 img])&]
[s2;%% Draws a 2D screen`-space image overlay at the specified pixel 
coordinates [%-*@3 pos] over the rendered 3D scene. Returns `*this 
for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:AddOverlay`(int`,int`,const Image`&`): GLScene[@(0.0.255) `&] 
[* AddOverlay]([@(0.0.255) int] [*@3 x], [@(0.0.255) int] [*@3 y], [@(0.0.255) const] 
Image[@(0.0.255) `&] [*@3 img])&]
[s2;%% Draws a 2D screen`-space image overlay at the specified pixel 
coordinates [%-*@3 x] , and [%-*@3 y ]over the rendered 3D scene. 
Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:ClearOverlays`(`): [@(0.0.255) void] [* ClearOverlays]()&]
[s2;%% Removes all queued 2D screen`-space overlays.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Uniform`(const String`&`,float`): [@(0.0.255) void] 
[* Uniform]([@(0.0.255) const] String[@(0.0.255) `&] [*@3 name], [@(0.0.255) float] 
[*@3 v])&]
[s2;%% Passes a floating`-point uniform value [%-*@3 v]to the currently 
active custom shader under the specified uniform [%-*@3 name].&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Uniform`(const String`&`,const Point3D`&`): [@(0.0.255) void] 
[* Uniform]([@(0.0.255) const] String[@(0.0.255) `&] [*@3 name], [@(0.0.255) const] 
Point3D[@(0.0.255) `&] [*@3 v])&]
[s2;%% Passes a 3D vector uniform value [%-*@3 v] to the currently 
active custom shader under the specified uniform [%-*@3 name].&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Uniform`(const String`&`,const RGBA`&`): [@(0.0.255) void] 
[* Uniform]([@(0.0.255) const] String[@(0.0.255) `&] [*@3 name], [@(0.0.255) const] 
RGBA[@(0.0.255) `&] [*@3 v])&]
[s2;%% Passes an RGBA color uniform value [%-*@3 v] (converted to a 
vec4) to the currently active custom shader under the specified 
uniform [%-*@3 name].&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:UseShader`(const String`&`): [@(0.0.255) void] 
[* UseShader]([@(0.0.255) const] String[@(0.0.255) `&] [*@3 name])&]
[s2;%% Binds the registered custom shader program identified by [%-*@3 name] 
for subsequent rendering calls. Pass a Null string to revert 
to the default fixed`-function style shader pipeline.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:ClearBuffer`(const RGBA`&`): [@(0.0.255) void] 
[* ClearBuffer]([@(0.0.255) const] RGBA[@(0.0.255) `&] [*@3 paper] [@(0.0.255) `=] 
RGBAZero())&]
[s2;%% Clears the active color and depth buffers, setting the background 
clear color to [%-*@3 paper].&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Render`(ModelGL`&`): [@(0.0.255) void] [* Render](ModelGL[@(0.0.255) `&
] [*@3 model])&]
[s2;%% Submits the compiled ModelGL mesh for rendering using the 
current transformation matrices, lighting states, and active 
shader.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:GLScene`:`:Clear`(`): [@(0.0.255) void] [* Clear]()&]
[s2;%% Resets all scene transformation matrices, states, and settings 
back to their default values .&]
[s3; ]]