const char *vertex_shader = R"(

	#version 150 core
	
	in vec3 aPos;
	in vec3 aNorm;
	in vec2 aTex;
	in vec4 aCol;
	
	uniform mat4 uModelView;
	uniform mat4 uProjection;
	
	out vec3 vPos;
	out vec3 vNorm;
	out vec2 vTex;
	out vec4 vCol;
	
	void main()
	{
	    vec4 viewPos = uModelView * vec4(aPos, 1.0);
	    
	    vPos = viewPos.xyz;
	    vNorm = mat3(uModelView) * aNorm;
	    vTex = aTex;
	    vCol = aCol;
	    
	    gl_Position = uProjection * viewPos;
	}

)";

const char *fragment_shader = R"(

	#version 150 core
	
	in vec3 vPos;
	in vec3 vNorm;
	in vec2 vTex;
	in vec4 vCol;
	
	uniform sampler2D uTex;
	uniform int uUseTex;
	uniform float uTime;
	
	#define MAX_LIGHTS 32
	uniform int uLightCount;
	uniform vec3 uLightDir[MAX_LIGHTS];
	uniform vec4 uLightCol[MAX_LIGHTS];
	
	out vec4 FragColor;
	
	vec2 curve(vec2 uv)
	{
	    uv = uv * 2.0 - 1.0;
	    vec2 offset = abs(uv.yx) / vec2(3.5, 3.5);
	    uv = uv + uv * offset * offset;
	    uv = uv * 0.5 + 0.5;
	    return uv;
	}
	
	void main()
	{
	    vec2 uv = curve(vTex);
	    if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
	        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	        return;
	    }
	    
	    vec4 tint = (length(vCol.rgb) > 0.0) ? vCol : vec4(1.0);
	    
	    float d = 0.008;
	    vec4 base = tint;
	    if(uUseTex != 0) {
	        float r = texture(uTex, uv + vec2(d, 0.0)).r;
	        float g = texture(uTex, uv).g;
	        float b = texture(uTex, uv - vec2(d, 0.0)).b;
	        base = vec4(r, g, b, 1.0) * tint;
	    }
	    
	    vec3 norm = normalize(vNorm);
	    vec3 diffuseSum = vec3(0.0);
	    int count = clamp(uLightCount, 0, MAX_LIGHTS);
	    for(int i = 0; i < count; i++) {
	        float diff = max(dot(norm, uLightDir[i]), 0.0);
	        diffuseSum += uLightCol[i].rgb * diff;
	    }
	    if(count == 0) {
	        float diff = max(dot(norm, vec3(0.5, 0.5, 1.0)), 0.0);
	        diffuseSum = vec3(1.0) * diff;
	    }
	    
	    // Low ambient and scaled diffuse to prevent blowing out texture details
	    vec3 lit = (vec3(0.05) + diffuseSum * 0.25) * base.rgb;
	    
	    // Global exposure scaling to keep overall brightness in check
	    lit *= 0.55;
	    
	    float scanline = sin(uv.y * 600.0 + uTime * 8.0) * 0.2 + 0.8;
	    lit *= scanline;
	    
	    float mask = 0.85 + 0.15 * sin(uv.x * 1200.0);
	    lit *= mask;
	    
	    vec2 vigCoord = uv * (1.0 - uv.yx);
	    float vignette = pow(vigCoord.x * vigCoord.y * 15.0, 0.25);
	    lit *= vignette;
	    
	    float flicker = 1.0 + 0.02 * sin(uTime * 40.0);
	    lit *= flicker;
	    
	    FragColor = vec4(lit, base.a);
	}
	
)";