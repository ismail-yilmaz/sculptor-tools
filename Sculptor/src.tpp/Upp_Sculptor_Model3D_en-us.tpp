topic "Model3D";
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
[ {{10000@(113.42.0) [s0;%% [*@7;4 Model3D]]}}&]
[s3; &]
[s1;:Upp`:`:Model3D: [@(0.0.255)3 struct][3  ][*3 Model3D]&]
[s2;%% This structure acts as a self`-contained container for 3D 
meshes, holding vertices, triangles, and embedded textures alongside 
their pre`-calculated mipmap chains.&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Nested Structures and Members]]}}&]
[s3; &]
[s1;:Upp`:`:Model3D`:`:Vertex: [@(0.0.255)3 struct][3  ][*3 Vertex][3  ][@(0.0.255)3 :][3  
Moveable<][*3 Vertex][3 >]&]
[s2; Represents a single point in 3D space, storing its position 
([C@5 position)], normal vector ([C@5 normal]), 2D texture coordinates 
([C@5 texcoord]), vertex color ([C@5 color]), and the bound texture 
index ([C@5 textureid]).&]
[s3; &]
[s4; &]
[s1;:Upp`:`:Model3D`:`:Triangle: [@(0.0.255)3 struct][3  ][*3 Triangle][3  
][@(0.0.255)3 :][3  Moveable<][*3 Triangle][3 >]&]
[s2;%% Defines a primitive polygon by storing indices ([C@5 a], [C@5 b], 
[C@5 c]) pointing to entries in the model’s vertex vector.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:vertices: Vector<Vertex> [* vertices]&]
[s2;%% Stores the raw vertex data (position, normal, texture coordinates, 
color, and texture ID) defining the physical geometry of the 
model.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:triangles: Vector<Triangle> [* triangles]&]
[s2;%% Stores the indexed triangle primitives. Each Triangle contains 
three indices that point to elements within the [^topic`:`/`/Sculptor`/src`/Upp`_Sculptor`_Model3D`_en`-us`#Upp`:`:Model3D`:`:vertices^ v
ertices ]array, defining the mesh faces.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:textures: VectorMap<int64, Image> [* textures]&]
[s2;%% A mapping of 64`-bit integer texture identifiers to their 
corresponding base image data.&]
[s3; &]
[s4;%% &]
[s5;:Upp`:`:Model3D`:`:mipmaps: Vector<Vector<Image>> [* mipmaps]&]
[s2;%% A mapping of texture identifiers to their respective pre`-computed 
mipmap chains. Each vector contains progressively downscaled, 
half`-resolution versions of the base texture for optimized Level`-of`-Detail 
(LOD) sampling.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:defaultcolor: RGBA [* defaultcolor]&]
[s2;%% The fallback color applied to newly added vertices, procedurally 
generated shapes, or models loaded without explicit vertex color 
data.&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Constructor]]}}&]
[s3; &]
[s5;:Upp`:`:Model3D`:`:Model3D`(`): [* Model3D]()&]
[s2; Default constructor.&]
[s3;%% &]
[ {{10000F(128)G(128)@1 [s0;%% [* Method List]]}}&]
[s3;%% &]
[s5;:Upp`:`:Model3D`:`:Clear`(`): [@(0.0.255) void] [* Clear]()&]
[s2;%% Resets the model by clearing all stored vertices, triangles, 
raw textures, and generated mipmap arrays.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:SetDefaultColor`(const RGBA`&`): [@(0.0.255) void] 
[* SetDefaultColor]([@(0.0.255) const] RGBA[@(0.0.255) `&] [*@3 color])&]
[s2;%% Assigns [%-*@3 color] as the fallback vertex color applied when 
building procedural shapes or parsing models without explicit 
vertex colors.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:Normalize`(float`): [@(0.0.255) void] [* Normalize]([@(0.0.255) floa
t] [*@3 radius])&]
[s2;%% Centers the entire mesh geometry around the origin (0,0,0) 
and scales it uniformly so that it fits neatly within a bounding 
sphere of the specified target [%-*@3 radius]. This is essential 
for standardizing raw OBJ models before rendering.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:AddVertex`(const Vertex`&`): Model3D[@(0.0.255) `&] 
[* AddVertex]([@(0.0.255) const] Vertex[@(0.0.255) `&] [*@3 v])&]
[s2;%% Appends vertex [%-*@3 v] to the model`'s vertex buffer. Returns 
`*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:AddTriangle`(int`,int`,int`): Model3D[@(0.0.255) `&] 
[* AddTriangle]([@(0.0.255) int] [*@3 a], [@(0.0.255) int] [*@3 b], [@(0.0.255) int] 
[*@3 c])&]
[s2;%% Appends a triangle referencing vertex indices [%-*@3 a], [%-*@3 b], 
and [%-*@3 c]. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:AddTexture`(const Image`&`): [@(0.128.128) int64 
][* AddTexture]([@(0.0.255) const] Image[@(0.0.255) `&] [*@3 img])&]
[s0;l288;%% Adds texture [%-*@3 img] to the model, automatically computes 
its complete downscaled mipmap chain for texture filtering. Returns 
its texture identifier.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:AddBox`(const Box3D`&`,int64`): Model3D[@(0.0.255) `&] 
[* AddBox]([@(0.0.255) const] Box3D[@(0.0.255) `&] [*@3 box], [@(0.128.128) int64 
][*@3 tid] [@(0.0.255) `=] [@(0.0.255) `-][@3 1])&]
[s2;%% Procedurally generates and appends a 3D box defined by spatial 
bounds [%-*@3 box], optionally mapping it to texture index [%-*@3 tid]. 
Teturns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:AddSphere`(float`,int`,int`,int64`): Model3D[@(0.0.255) `&] 
[* AddSphere]([@(0.0.255) float] [*@3 radius], [@(0.0.255) int] [*@3 slices], 
[@(0.0.255) int] [*@3 stacks], [@(0.128.128) int64 ][*@3 tid] [@(0.0.255) `=] 
[@(0.0.255) `-][@3 1])&]
[s2;%% Procedurally generates and appends a sphere of given [%-*@3 radius], 
tessellated into horizontal [%-*@3 stacks] and vertical [%-*@3 slices], 
mapped to texture [%-*@3 tid]. Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:ComputeBounds`(`)const: Box3D [* ComputeBounds]() 
[@(0.0.255) const]&]
[s2;%% Calculates and returns the axis`-aligned bounding box that 
tightly encloses all vertices in the model.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:Model3D`:`:GetCenter`(`)const: Point3D [* GetCenter]() 
[@(0.0.255) const]&]
[s2;%% Computes and returns the exact 3D center point of the model`'s 
bounding box.&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Function List]]}}&]
[s3; &]
[s5;:Upp`:`:LoadModel`(Model3D`&`,const String`&`): [@(0.0.255) bool] 
[* LoadModel](Model3D[@(0.0.255) `&] [*@3 model], [@(0.0.255) const] 
String[@(0.0.255) `&] [*@3 data])&]
[s2;%% Parses a Wavefront [C@(0.0.255) .obj ]ASCII string from [%-*@3 data] 
and populates [%-*@3 model]. Note that external [C@(0.0.255) .mtl 
]material files are ignored; textures must be bound programmatically 
after loading. Returns true on success.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:LoadModelFromFile`(Model3D`&`,const String`&`): [@(0.0.255) bool] 
[* LoadModelFromFile](Model3D[@(0.0.255) `&] [*@3 model], [@(0.0.255) const] 
String[@(0.0.255) `&] [*@3 path])&]
[s2;%% Loads a Wavefront [C@(0.0.255) .obj ] at [%-*@3 path] and populates 
[%-*@3 model]. Note that external [C@(0.0.255) .mtl ]material files 
are ignored; textures must be bound programmatically after loading. 
Returns true on success.]]