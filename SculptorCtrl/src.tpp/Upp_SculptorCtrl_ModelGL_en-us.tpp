topic "ModelGL";
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
[ {{10000@(113.42.0) [s0;%% [*@7;4 ModelGL]]}}&]
[s3; &]
[s1;:Upp`:`:ModelGL: [@(0.0.255)3 struct ][*3 ModelGL]&]
[s2;%% This structure acts as a hardware`-accelerated container for 
3D meshes, holding vertices, triangles, and textures, and managing 
their compilation into OpenGL Vertex Array Objects (VAOs) and 
Vertex Buffer Objects (VBOs) for optimized GPU rendering.&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Nested Structures and Members]]}}&]
[s3; &]
[s1;:Upp`:`:ModelGL`:`:Vertex: [@(0.0.255)3 struct][3  ][*3 Vertex][3  ][@(0.0.255)3 :][3  
Moveable<][*3 Vertex][3 >]&]
[s2; Represents a single point in 3D space, storing its position 
([C@5 position)], normal vector ([C@5 normal]), 2D texture coordinates 
([C@5 texcoord]), vertex color ([C@5 color]), and the bound texture 
index ([C@5 textureid]).&]
[s3; &]
[s4; &]
[s1;:Upp`:`:ModelGL`:`:Triangle: [@(0.0.255)3 struct][3  ][*3 Triangle][3  
][@(0.0.255)3 :][3  Moveable<][*3 Triangle][3 >]&]
[s2;%% Defines a primitive polygon by storing indices ([C@5 a], [C@5 b], 
[C@5 c]) pointing to entries in the model’s vertex vector&]
[s3; &]
[s4; &]
[s1;:Upp`:`:ModelGL`:`:Batch: [@(0.0.255)3 struct][3  ][*3 Batch][3  ][@(0.0.255)3 :][3  
Moveable<][*3 Batch][3 >]&]
[s2;%% Defines a contiguous sequence of geometry that shares the 
same texture. It stores the texture ID ([%-C@5 textureid]), the 
starting offset in the index buffer ([C@(0.0.255) offset]), and 
the number of indices ([C@(0.0.255) count]) to be rendered in a 
single OpenGL draw call.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:vertices: Vector<Vertex> [* vertices]&]
[s2;%% Stores the raw vertex data (position, normal, texture coordinates, 
color, and texture ID) defining the physical geometry of the 
model.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:triangles: Vector<Triangle> [* triangles]&]
[s2;%% Stores the indexed triangle primitives. Each Triangle contains 
three indices that point to elements within the [^topic`:`/`/SculptorCtrl`/src`/Upp`_SculptorCtrl`_ModelGL`_en`-us`#Upp`:`:ModelGL`:`:vertices^ v
ertices] array, defining the mesh faces.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:textures: VectorMap<[@(0.128.128) int64], Image> 
[* textures]&]
[s2;%% A mapping of 64`-bit integer texture identifiers to their 
corresponding base image data, held in CPU memory until compiled 
to the GPU.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:defaultcolor: RGBA [* defaultcolor]&]
[s2;%% The fallback color applied to newly added vertices, procedurally 
generated shapes, or models loaded without explicit vertex color 
data.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:compiled: [@(0.0.255) bool] [* compiled]&]
[s2;%% A boolean flag indicating whether the model`'s geometry has 
been successfully compiled into hardware`-accelerated OpenGL 
buffers.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:vaos: VectorMap<[@(0.128.128) int64], GLuint> 
[* vaos]&]
[s2;%% A mapping of texture identifiers to their compiled OpenGL 
Vertex Array Object (VAO) handles.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:vertexbuffer: GLuint [* vertexbuffer]&]
[s2;%% The OpenGL handle for the Vertex Buffer Object (VBO) storing 
the uploaded vertex data in GPU memory.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:indexbuffer: GLuint [* indexbuffer]&]
[s2;%% The OpenGL handle for the Element Array Buffer (EBO/index 
buffer) storing the uploaded triangle indices in GPU memory.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:batches: Vector<Batch> [* batches]&]
[s2;%% A collection of rendering batches, grouping contiguous triangles 
by texture ID to minimize OpenGL state changes during drawing 
operations.&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Constructor detail]]}}&]
[s3; &]
[s5;:Upp`:`:ModelGL`:`:ModelGL`(`): [* ModelGL]()&]
[s2;%% Default constructor.&]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Method List]]}}&]
[s3; &]
[s5;:Upp`:`:ModelGL`:`:Clear`(`): [@(0.0.255) void] [* Clear]()&]
[s2;%% Resets the model by clearing all locally stored vertices, 
triangles, textures, and rendering batches, as well as freeing 
any compiled OpenGL buffers. Doesn`'t clear shared resources.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:SetDefaultColor`(const RGBA`&`): [@(0.0.255) void] 
[* SetDefaultColor]([@(0.0.255) const] RGBA[@(0.0.255) `&] [*@3 color])&]
[s2;%% Assigns [%-*@3 color] as the fallback vertex color applied when 
building procedural shapes or parsing models without explicit 
vertex colors.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:Normalize`(float`): [@(0.0.255) void] [* Normalize]([@(0.0.255) floa
t] [*@3 target])&]
[s2;%% Centers the entire mesh geometry around the origin (0,0,0) 
and scales it uniformly so that it fits neatly within a bounding 
sphere of the specified [%-*@3 target] radius. This is essential 
for standardizing raw OBJ models before rendering.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:AddVertex`(const Vertex`&`): ModelGL[@(0.0.255) `&] 
[* AddVertex]([@(0.0.255) const] Vertex[@(0.0.255) `&] [*@3 v])&]
[s2;%% Appends vertex [%-*@3 v ]to the model`'s vertex buffer. Returns 
`*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:AddTriangle`(int`,int`,int`): ModelGL[@(0.0.255) `&] 
[* AddTriangle]([@(0.0.255) int] [*@3 a], [@(0.0.255) int] [*@3 b], [@(0.0.255) int] 
[*@3 c])&]
[s2;%% Appends a triangle referencing vertex indices [%-*@3 a], [%-*@3 b], 
and [%-*@3 c]. Returns `*this for method chaining&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:AddTexture`(const Image`&`): int64 [* AddTexture]([@(0.0.255) cons
t] Image[@(0.0.255) `&] [*@3 img])&]
[s2;%% Adds texture [%-*@3 img]to the model`'s internal map, ready 
to be uploaded to the GPU during compilation. Returns its texture 
identifier.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:AddBox`(const Box3D`&`,int64`): ModelGL[@(0.0.255) `&] 
[* AddBox]([@(0.0.255) const] Box3D[@(0.0.255) `&] [*@3 box], int64 [*@3 tid] 
[@(0.0.255) `=] [@3 0])&]
[s2;%% Procedurally generates and appends a 3D box defined by spatial 
bounds [%-*@3 box], optionally mapping it to texture index/identifier 
[%-*@3 tid] . Returns `*this for method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:AddSphere`(float`,int`,int`,int64`): ModelGL[@(0.0.255) `&] 
[* AddSphere]([@(0.0.255) float] [*@3 radius], [@(0.0.255) int] [*@3 slices], 
[@(0.0.255) int] [*@3 stacks], int64 [*@3 tid] [@(0.0.255) `=] [@3 0])&]
[s0;l288;%% Procedurally generates and appends a sphere of given 
[%-*@3 radius] tessellated into horizontal [%-*@3 stacks] and vertical 
[%-*@3 slices] , mapped to texture [%-*@3 tid] . Returns `*this for 
method chaining.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:ComputeBounds`(`)const: Box3D [* ComputeBounds]() 
[@(0.0.255) const]&]
[s2;%% Calculates and returns the axis`-aligned bounding box that 
tightly encloses all vertices in the model.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:GetCenter`(`)const: Point3D [* GetCenter]() 
[@(0.0.255) const]&]
[s2;%% Computes and returns the exact 3D center point of the model`'s 
bounding box.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:Compile`(`): [@(0.0.255) void] [* Compile]()&]
[s2;%% Processes the CPU`-side geometry data, sorts triangles into 
texture batches, and uploads the data to the GPU by generating 
the necessary Vertex Buffer Objects (VBOs) and Vertex Array Objects 
(VAOs).&]
[s3; &]
[s4; &]
[s5;:Upp`:`:ModelGL`:`:GetVAO`(`): GLuint [* GetVAO]()&]
[s2;%% Returns the primary OpenGL Vertex Array Object (VAO) handle 
used to render this model`'s geometry &]
[s3; &]
[ {{10000F(128)G(128)@1 [s0;%% [* Function List]]}}&]
[s3; &]
[s5;:Upp`:`:LoadModel`(ModelGL`&`,const String`&`): [@(0.0.255) bool] 
[* LoadModel](ModelGL[@(0.0.255) `&] [*@3 model], [@(0.0.255) const] 
String[@(0.0.255) `&] [*@3 data])&]
[s2;%% Parses a Wavefront [C@(0.0.255) .obj ]ASCII string from [%-*@3 data] 
and populates [%-*@3 model]. Note that external [C@(0.0.255) .mtl 
]material files are ignored; textures must be bound programmatically 
after loading. Returns true on success.&]
[s3; &]
[s4; &]
[s5;:Upp`:`:LoadModelFromFile`(ModelGL`&`,const String`&`): [@(0.0.255) bool] 
[* LoadModelFromFile](ModelGL[@(0.0.255) `&] [*@3 model], [@(0.0.255) const] 
String[@(0.0.255) `&] [*@3 path])&]
[s2;%% Loads a Wavefront [C@(0.0.255) .obj ]file at [%-*@3 path] and 
populates [%-*@3 model]. Note that external [C@(0.0.255) .mtl ]material 
files are ignored; textures must be bound programmatically after 
loading. Returns true on success.]]