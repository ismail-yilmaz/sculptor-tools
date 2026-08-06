#include "Demo.h"

namespace Upp {

const char *vertexshader = R"(

	#version 150 core
	
	in vec3 aPos;
	in vec3 aNorm;
	in vec2 aTex;
	in vec4 aCol;
	
	uniform mat4 uProjection;
	uniform mat4 uModelView;
	
	uniform float uUseGradient;
	uniform float uGradBase;
	uniform float uGradSpan;
	uniform vec4  uColorLo;
	uniform vec4  uColorHi;
	
	out vec3 vPos;
	out vec3 vNorm;
	out vec2 vTex;
	out vec4 vCol;
	
	void main()
	{
		gl_Position = uProjection * uModelView * vec4(aPos, 1.0);
		vPos = vec3(uModelView * vec4(aPos, 1.0));
		vNorm = normalize(mat3(uModelView) * aNorm);
		vTex = aTex;
		
		if (uUseGradient > 0.5) {
			float t = clamp((aPos.y - uGradBase) / uGradSpan, 0.0, 1.0);
			vCol = mix(uColorLo, uColorHi, t);
		} else {
			vCol = aCol;
		}
	}

)";

const char *fragmentshader = R"(

	#version 150 core
	
	in vec3 vPos;
	in vec3 vNorm;
	in vec2 vTex;
	in vec4 vCol;
	
	uniform sampler2D uTex;
	uniform int uUseTex;
	uniform float uTriSample;
	uniform float uRimShade;
	
	uniform int  uLightCount;
	uniform vec3 uLightDir[32];
	uniform vec4 uLightCol[32];
	
	out vec4 FragColor;
	
	void main()
	{
		vec4 base = vCol;
		
		if(uTriSample > 0.5) {
			vec3 n = abs(normalize(vNorm));
			vec2 uv;
			if(n.x >= n.y && n.x >= n.z)
				uv = vPos.yz;
			else if(n.y >= n.x && n.y >= n.z)
				uv = vPos.xz;
			else
				uv = vPos.xy;
				
			float checker = mod(floor(uv.x * 2.0) + floor(uv.y * 2.0), 2.0);
			if(checker < 0.5)
				base = vec4(50.0 / 255.0, 55.0 / 255.0, 70.0 / 255.0, 1.0);
			else
				base = vec4(225.0 / 255.0, 225.0 / 255.0, 235.0 / 255.0, 1.0);
		}
		else if(uUseTex != 0) {
			base = texture(uTex, vTex) * vCol;
		}
	
		if(uRimShade > 0.5) {
			vec3 viewDir = normalize(-vPos);
			float rim = 1.0 - clamp(dot(viewDir, normalize(vNorm)), 0.0, 1.0);
			rim = pow(rim, 3.0);
			base.r = clamp(base.r + rim * (180.0 / 255.0), 0.0, 1.0);
			base.g = clamp(base.g + rim * (60.0 / 255.0), 0.0, 1.0);
			base.b = clamp(base.b + rim * 1.0, 0.0, 1.0);
		}
	
		vec3 norm = normalize(vNorm);
		vec3 lit = vec3(0.2) * base.rgb;
	
		for(int i = 0; i < uLightCount; i++) {
			float diff = max(dot(norm, normalize(uLightDir[i])), 0.0);
			lit += uLightCol[i].rgb * diff * base.rgb;
		}
	
		FragColor = vec4(lit, base.a);
	}

)";

const char *pixelshader = R"(

	#version 150 core
	
	in vec2 vTex;
	uniform sampler2D uTex;
	
	uniform float uVignette;
	
	out vec4 FragColor;
	
	void main()
	{
		vec4 color = texture(uTex, vTex);
	
		if (uVignette > 0.5) {
			// Calculate distance from the center (0.5, 0.5)
			vec2 center = vec2(0.5, 0.5);
			float dist = distance(vTex, center);
			
			// Smoothly darken the edges
			float vignetteAmt = smoothstep(0.8, 0.2, dist);
			color.rgb *= vignetteAmt;
		}
	
		FragColor = color;
	}

)";

}