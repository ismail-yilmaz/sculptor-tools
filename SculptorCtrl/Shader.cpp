#include "SculptorCtrl.h"

namespace Upp {

const char *scgl_def_vertex_shader = R"(

	#version 150 core

	in vec3 aPos;
	in vec3 aNorm;
	in vec2 aTex;
	in vec4 aCol;

	uniform mat4 uProjection;
	uniform mat4 uModelView;

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
		vCol = aCol;
	}

)";

const char *scgl_def_fragment_shader = R"(

	#version 150 core

	in vec3 vPos;
	in vec3 vNorm;
	in vec2 vTex;
	in vec4 vCol;

	uniform sampler2D uTex;
	uniform int uUseTex;

	#define MAX_LIGHTS 32

	uniform int uLightCount;
	uniform vec3 uLightDir[MAX_LIGHTS];
	uniform vec4 uLightCol[MAX_LIGHTS];

	out vec4 FragColor;

	void main()
	{
		vec4 base = vCol;
		if(uUseTex != 0)
			base = texture(uTex, vTex) * vCol;

		vec3 diffuseSum = vec3(0.0);
		int count = clamp(uLightCount, 0, MAX_LIGHTS);
		for(int i = 0; i < count; i++) {
			float diff = max(dot(vNorm, uLightDir[i]), 0.0);
			diffuseSum += uLightCol[i].rgb * diff;
		}

		if(count == 0) {
			float diff = max(dot(vNorm, vec3(0.5, 0.5, 1.0)), 0.0);
			diffuseSum = vec3(1.0) * diff;
		}

		vec3 lit = (vec3(0.2) + diffuseSum) * base.rgb;
		FragColor = vec4(min(lit, vec3(1.0)), base.a);
	}

)";

}