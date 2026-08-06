topic "SculptorCtrl";
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
[ {{10000@(113.42.0) [s0;%% [*@7;4 SculptorCtrl]]}}&]
[s3; &]
[s1;:Upp`:`:SculptorCtrl: [@(0.0.255)3 class][3  ][*3 SculptorCtrl][3  ][@(0.0.255)3 :][3  
][@(0.0.255)3 public][3  Ctrl]&]
[s2;~~~2464;%% This class provides a hardware`-accelerated 3D rendering 
widget that embeds a programmable OpenGL viewport into U`+`+ 
applications. As the GPU`-backed counterpart to the Sculptor 
software rasterizer, it handles the underlying OpenGL context 
and framebuffers through a standard API. This provides the necessary 
infrastructure to construct 3D scenes, manage custom GLSL shaders, 
and apply instance`-bound post`-processing effects directly within 
a standard desktop UI control.&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Constructor detail]]}}&]
[s3; &]
[s5;:Upp`:`:SculptorCtrl`:`:SculptorCtrl`(`): [* SculptorCtrl]()&]
[s2;%% Default constructor. Initializes a new SculptorCtrl with default 
OpenGL context settings.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:SculptorCtrl`(int`,int`,bool`,int`): [* SculptorCtrl]([@(0.0.255) i
nt] [*@3 depthbits], [@(0.0.255) int] [*@3 stencilbits] [@(0.0.255) `=] 
[@3 8], [@(0.0.255) bool] [*@3 doublebuffer] [@(0.0.255) `=] [@(0.0.255) true], 
[@(0.0.255) int] [*@3 samples] [@(0.0.255) `=] [@3 0])&]
[s2;%% Constructor overload. Initializes a new SculptorCtrl instance 
and requests specific framebuffer attributes, including depth 
buffer bits, stencil bits, double buffering, and MSAA samples.&]
[s3;%% &]
[ {{10000F(128)G(128)@1 [s0;%% [* Public Method List]]}}&]
[s3; &]
[s5;:Upp`:`:SculptorCtrl`:`:SetDepthBits`(int`): [@(0.0.255) static] 
[@(0.0.255) void] [* SetDepthBits]([@(0.0.255) int] [*@3 bits])&]
[s2;%% Sets the global default depth buffer bit depth for future 
contexts.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:SetStencilBits`(int`): [@(0.0.255) static] 
[@(0.0.255) void] [* SetStencilBits]([@(0.0.255) int] [*@3 bits])&]
[s2;%% Sets the global default stencil buffer bit depth for future 
contexts.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:SetDoubleBuffering`(bool`): [@(0.0.255) static] 
[@(0.0.255) void] [* SetDoubleBuffering]([@(0.0.255) bool] [*@3 enable] 
[@(0.0.255) `=] [@(0.0.255) true])&]
[s2;%% Enables or disables double buffering globally for future contexts.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:SetMSAA`(int`): [@(0.0.255) static] [@(0.0.255) void] 
[* SetMSAA]([@(0.0.255) int] [*@3 samples] [@(0.0.255) `=] [@3 4])&]
[s2;%% Configures the global default number of multisampling (MSAA) 
samples for anti`-aliasing.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:SetShader`(const String`&`,const String`&`,const String`&`): [@(0.0.255) v
oid] [* SetShader]([@(0.0.255) const] String[@(0.0.255) `&] [*@3 name], 
[@(0.0.255) const] String[@(0.0.255) `&] [*@3 vertshader], [@(0.0.255) const] 
String[@(0.0.255) `&] [*@3 fragshader])&]
[s2;%% Clears the shared static shader registry entirely and registers 
this single vertex and fragment shader pair as the sole available 
custom shader. The [%-*@3 name ]acts as a unique identifier for 
binding during the render loop. [/ Note: Calling this method flushes 
any previously added shaders across all instances.]&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:AddShader`(const String`&`,const String`&`,const String`&`): [@(0.0.255) v
oid] [* AddShader]([@(0.0.255) const] String[@(0.0.255) `&] [*@3 name], 
[@(0.0.255) const] String[@(0.0.255) `&] [*@3 vertshader], [@(0.0.255) const] 
String[@(0.0.255) `&] [*@3 fragshader])&]
[s2;%% Appends a new vertex and fragment shader program pair to the 
shared static registry. The shader is only added if the specified 
[%-*@3 name] does not already exist, preserving all previously 
registered shaders.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:ClearShader`(const String`&`): [@(0.0.255) void] 
[* ClearShader]([@(0.0.255) const] String[@(0.0.255) `&] [*@3 name])&]
[s2;%% Removes a specific shader program with [%-*@3 name] and frees 
its GPU resources. Note: Although this is an instance method, 
shaders are stored in a shared static registry.  [/ Note: Calling 
this method removes the shader for all SculptorCtrl instances.]&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:ClearShaders`(`): [@(0.0.255) void] [* ClearShaders]()&]
[s2;%% Removes all registered shaders and frees their GPU resources. 
[/ Note: Calling this method removes all of the shaders for all 
SculptorCtrl instances.]&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:SetPostProcess`(const String`&`): [@(0.0.255) void] 
[* SetPostProcess]([@(0.0.255) const] String[@(0.0.255) `&] [*@3 fragshader])&]
[s2;%% Enables a custom screen`-space GLSL fragment shader for post`-processing 
on this specific ctrl instance. When defined, this SculptorCtrl 
instance alters its rendering pipeline: It renders the entire 
3D scene into an off`-screen Framebuffer Object (FBO), and then 
draws a a quad using the provided fragment shader. This is ideal 
for instance`-specific full`-screen effects like CRT distortion, 
bloom, vignette, or color grading.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:ClearPostProcess`(`): [@(0.0.255) void] 
[* ClearPostProcess]()&]
[s2;%% Disables and clears the active post`-processing shader for 
this specific control instance, returning its pipeline to standard 
forward rendering.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:ClearTexture`(int64`): [@(0.0.255) void] 
[* ClearTexture]([@(0.128.128) int64 ][*@3 tid])&]
[s2;%% Deletes a specific OpenGL texture associated with the given 
texture ID [%-*@3 tid] from GPU memory.  [/ Note:] [/ Calling this method 
completely deletes the shared static texture for all instances.]&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:ClearTextures`(`): [@(0.0.255) void] [* ClearTextures]()&]
[s2;%% Flushes all cached textures from GPU memory. [/ Note: Calling 
this method completely clears the shared static texture cache 
for all instances.]&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:InitScene`(`): [@(0.0.255) virtual void] 
[* InitScene]()&]
[s2;%% Virtual method called once to initialize scene resources like 
registering shaders or loading textures. By default, it invokes 
the [^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_en`-us`#Upp`:`:SculptorCtrl`:`:WhenInitScene^ W
henInitScene] event.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:ClearScene`(`): [@(0.0.255) void] [* ClearScene]()&]
[s2;%% Clears all internal pipeline states and buffered rendering 
data.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:Render`(GLScene`&`): [@(0.0.255) virtual 
void] [* Render](GLScene[@(0.0.255) `&] [*@3 scene])&]
[s2;%% The primary virtual rendering hook. Called every frame to 
populate the GLScene object with rendering commands. By default, 
it invokes the [^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_en`-us`#Upp`:`:SculptorCtrl`:`:WhenRender^ W
henRender] event.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:WhenInitScene: Event<> [* WhenInitScene]&]
[s2;%% Triggered during the ctrl`'s initialization phase. Bind this 
event to load models, generate textures, or compile shaders as 
an alternative to overriding [^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_en`-us`#Upp`:`:SculptorCtrl`:`:InitScene`(`)^ I
nitScene()].&]
[s3; &]
[s4; &]
[s5;:Upp`:`:SculptorCtrl`:`:WhenRender: Event<GLScene[@(0.0.255) `&]> 
[* WhenRender]&]
[s2;%% Triggered every frame. Bind this event to issue rendering 
commands to the GLScene as an alternative to overriding [^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_en`-us`#Upp`:`:SculptorCtrl`:`:Render`(GLScene`&`)^ R
ender()].&]
[s3; &]
[s0;%% ]]