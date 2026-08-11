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
	uniform mat4 uNormalMatrix;

	uniform float uUseGradient;
	uniform float uGradBase;
	uniform float uGradSpan;
	uniform vec4  uColorLo;
	uniform vec4  uColorHi;
	uniform float uAlphaBase;

	out vec3 vPos;
	out vec3 vNorm;
	out vec2 vTex;
	out vec4 vCol;

	void main()
	{
		gl_Position = uProjection * uModelView * vec4(aPos, 1.0);
		vPos = vec3(uModelView * vec4(aPos, 1.0));
		vNorm = normalize(mat3(uNormalMatrix) * aNorm);
		vTex = aTex;

		if (uUseGradient > 0.5) {
			float t = clamp((aPos.y - uGradBase) / uGradSpan, 0.0, 1.0);
			vCol = mix(uColorLo, uColorHi, t);
		} else {
			vCol = aCol;
		}

		vCol.a *= uAlphaBase;
	}

)";

const char *fragmentshader = R"(

	#version 150 core

	in vec3 vPos;
	in vec3 vNorm;
	in vec2 vTex;
	in vec4 vCol;

	uniform sampler2D uTex;
	uniform int   uUseTex;
	uniform float uTriSample;
	uniform float uRimShade;

	uniform int   uDrawWire;
	uniform float uXRay;
	uniform vec3  uMouseUV;
	uniform vec3  uResolution;

	uniform int   uLightCount;
	uniform vec3  uLightDir[32];
	uniform vec4  uLightCol[32];

	out vec4 FragColor;

	void main()
	{
		// X-ray logic
		float aspect = uResolution.x / uResolution.y;
		vec2  fragUV = gl_FragCoord.xy / uResolution.xy;
		fragUV.x *= aspect;

		vec2 pointer = uMouseUV.xy;
		pointer.x *= aspect;

		float dist = distance(fragUV, pointer);

		// If we are looking at the OUTER shell and inside the mouse radius, CUT A HOLE!
		if (uXRay > 0.5 && gl_FrontFacing && dist < 0.15)
			discard; // Drops this pixel entirely, preserving the Z-buffer for the inside!

		// Intercept Pass 2: Draw strict CAD wireframe lines
		if(uDrawWire == 1) {
			FragColor = vec4(0.05, 0.05, 0.05, vCol.a);
			return;
		}

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
				base = vec4(50.0 / 255.0, 55.0 / 255.0, 70.0 / 255.0, vCol.a);
			else
				base = vec4(225.0 / 255.0, 225.0 / 255.0, 235.0 / 255.0, vCol.a);
		}
		else if(uUseTex != 0) {
			base = texture(uTex, vTex) * vCol;
		}

		// Double-sided lighting trick: flip normals for the inside shell
		vec3 norm = normalize(vNorm);
		if (!gl_FrontFacing) {
			norm = -norm;

			// Tint the inside slightly darker so it looks like a true cross-section
			base.rgb *= 0.65;
			base.rgb += vec3(0.05, 0.05, 0.1);
		}

		if(uRimShade > 0.5) {
			vec3 viewdir = normalize(-vPos);
			float rim = 1.0 - clamp(dot(viewdir, norm), 0.0, 1.0);
			rim = pow(rim, 3.0);
			base.r = clamp(base.r + rim * (180.0 / 255.0), 0.0, 1.0);
			base.g = clamp(base.g + rim * (60.0 / 255.0), 0.0, 1.0);
			base.b = clamp(base.b + rim * 1.0, 0.0, 1.0);
		}

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
			vec2 center = vec2(0.5, 0.5);
			float dist = distance(vTex, center);
			float vignetteAmt = smoothstep(0.8, 0.2, dist);
			color.rgb *= vignetteAmt;
		}

		FragColor = color;
	}

)";

}