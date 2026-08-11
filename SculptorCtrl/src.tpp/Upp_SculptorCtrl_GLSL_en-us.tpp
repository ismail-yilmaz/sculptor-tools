topic "GLSL Reference";
[ $$0,0#00000000000000000000000000000000:Default]
[{_} 
[s0;b20;a20;pht300; [*G5 SculptorCtrl GLSL Shader `& Pipeline Technical 
Reference]&]
[s0;b20;a20;pht300; [*G4 Architectural Overview]&]
[s0;b20;t300; [G^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_en`-us`#Upp`:`:SculptorCtrl^2 S
culptorCtrl][G2  and its underlying engine (][G^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_GLScene`_en`-us`#Upp`:`:GLScene^2 G
LScene][G2 ) provide a state`-managed, high`-performance OpenGL 
rendering widget. The shader execution pipeline operates by consuming 
vertex arrays from ][G^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_ModelGL`_en`-us`#Upp`:`:ModelGL^2 M
odelGL][G2 , binding built`-in geometric and lighting matrices 
automatically, passing custom user uniforms, and managing multi`-pass 
rendering states (such as wireframe`-on`-shaded visualization 
and Order`-Independent Transparency).  The complete specification 
of vertex attributes, built`-in uniforms, user uniform containers 
 is detailed below. ]&]
[s0;H1;h(220)G2 &]
[s0;b20;a20;pht300; [*G4 1. Vertex Attributes]&]
[s0;b20;t300; [G2 The engine explicitly binds attribute locations during 
shader compilation. Following is the currently available vertext 
attributes ]&]
[ {{3333:3333:3334B20;A20;G(220)@(220.225.230) [s0; [G2 Name]]
:: [s0; [G2 Type]]
:: [s0; [G2 Description]]
::@2 [s0; [C@5$(245)2 aPos]]
:: [s0; [C@5$(245)2 vec3]]
:: [s0; [G2 Local object`-space vertex coordinate position.]]
:: [s0; [C@5$(245)2 aNorm]]
:: [s0; [C@5$(245)2 vec3]]
:: [s0; [G2 Surface normal vector used for lighting and shading computations.]]
:: [s0; [C@5$(245)2 aTex]]
:: [s0; [C@5$(245)2 vec2]]
:: [s0; [G2 Texture coordinate UV mapping values.]]
:: [s0; [C@5$(245)2 aCol]]
:: [s0; [C@5$(245)2 vec4]]
:: [s0; [G2 Per`-vertex RGBA color data.]]}}&]
[s0;H1;h(220)G2 &]
[s0;b20;a20;pht300; [*G4 2. Built`-in Uniforms]&]
[s0;b20;t300; [G2 The engine automatically populates and binds the 
following uniform variables during every frame`'s ][G^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_en`-us`#Upp`:`:SculptorCtrl`:`:Render`(GLScene`&`)^2 R
ender()][G2  loop without requiring manual application intervention. 
]&]
[ {{3333:3333:3334B20;A20;G(220)@(220.225.230) [s0; [G2 Name]]
:: [s0; [G2 Type]]
:: [s0; [G2 Description]]
::@2 [s0; [C@5$(245)2 uModelView]]
:: [s0; [C@5$(245)2 mat4]]
:: [s0; [G2 Combined model transformation and camera view matrix.]]
:: [s0; [C@5$(245)2 uProjection]]
:: [s0; [C@5$(245)2 mat4]]
:: [s0; [G2 Camera projection matrix (perspective, orthographic, or isometric).]]
:: [s0; [C@5$(245)2 uNormalMatrix]]
:: [s0; [C@5$(245)2 mat4]]
:: [s0; [G2 Inverse`-transpose of the modelview matrix for accurate non`-uniform 
scale lighting.]]
:: [s0; [C@5$(245)2 uLightCount]]
:: [s0; [C@5$(245)2 int]]
:: [s0; [G2 Total number of active light sources currently registered 
in the scene (max 32).]]
:: [s0; [C@5$(245)2 uLightDir]]
:: [s0; [C@5$(245)2 vec3`[32`]]]
:: [s0; [G2 Array of normalized light direction vectors.]]
:: [s0; [C@5$(245)2 uLightCol]]
:: [s0; [C@5$(245)2 vec4`[32`]]]
:: [s0; [G2 Array of normalized RGBA light colors.]]
:: [s0; [C@5$(245)2 uUseTex]]
:: [s0; [C@5$(245)2 int]]
:: [s0; [G2 Flag indicating whether the active mesh batch contains a bound 
texture (][C@5$(245)2 1][G2  or ][C@5$(245)2 0][G2 ).]]
:: [s0; [C@5$(245)2 uDrawWire]]
:: [s0; [C@5$(245)2 int]]
:: [s0; [G2 Pass indicator flag: ][C@5$(245)2 0][G2  for solid fill shading 
pass, ][C@5$(245)2 1][G2  for wireframe edge rendering pass.]]
:: [s0; [C@5$(245)2 uTex]]
:: [s0; [C@5$(245)2 sampler2D]]
:: [s0; [G2 Active 2D texture sampler used for model texturing, post`-processing, 
and HUD overlays.]]}}&]
[s0;H1;h(220)G2 &]
[s0;b20;a20;pht300; [*G4 3. User Uniform Containers]&]
[s0;b20;t300; [G2 Application developers can pass custom parameters 
from the application layer into active shaders dynamically using 
][G^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_GLScene`_en`-us`#Upp`:`:GLScene`:`:Uniform`(const String`&`,float`)^2 G
LScene`::Uniform()][G2  methods. These containers automatically 
stream data to the active GPU program every frame. ]&]
[ {{3333:3333:3334B20;A20;G(220)@(220.225.230) [s0; [G2 Name]]
:: [s0; [G2 Type]]
:: [s0; [G2 Description]]
::@2 [s0; [C@5$(245)2 `[User Float`]]]
:: [s0; [C@5$(245)2 float]]
:: [s0; [G2 Custom scalar floating`-point values passed via ][G^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_GLScene`_en`-us`#Upp`:`:GLScene`:`:Uniform`(const String`&`,float`)^2 G
LScene`::Uniform()][G2 .]]
:: [s0; [C@5$(245)2 `[User Vec3`]]]
:: [s0; [C@5$(245)2 vec3]]
:: [s0; [G2 Custom 3D vector values passed via ][G^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_GLScene`_en`-us`#Upp`:`:GLScene`:`:Uniform`(const String`&`,const Point3D`&`)^2 G
LScene`::Uniform()][G2  overload.]]
:: [s0; [C@5$(245)2 `[User RGBA`]]]
:: [s0; [C@5$(245)2 vec4]]
:: [s0; [G2 U`+`+ color values passed via ][G^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_GLScene`_en`-us`#Upp`:`:GLScene`:`:Uniform`(const String`&`,const RGBA`&`)^2 G
LScene`::Uniform()][G2  overload and automatically normalized to 
range 0.0 to 1.0.]]}}&]
[s0;G2 ]]