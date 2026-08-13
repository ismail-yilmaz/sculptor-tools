const char *grid_vshader = R"(

	#version 150 core

	in vec3 aPos;
	in vec3 aNorm;
	in vec2 aTex;
	in vec4 aCol;

	uniform mat4 uModelView;
	uniform mat4 uProjection;

	out vec2 vTex;

	void main()
	{
		vTex = aTex;
		gl_Position = uProjection * (uModelView * vec4(aPos, 1.0));
	}

)";

const char *grid_fshader = R"(

	#version 150 core

	in vec2 vTex;
	out vec4 FragColor;

	void main()
	{
		float scale = 2.0;
		vec2 cellCoord = vTex * scale;

		// Screen-space derivatives for nice anti-aliasing
		vec2 fw = fwidth(cellCoord);
		vec2 grid = abs(fract(cellCoord - 0.5) - 0.5);

		float thickness = 0.05;
		vec2 d = smoothstep(vec2(thickness) + fw, vec2(thickness) - fw, grid);
		float isLine = max(d.x, d.y);

		vec3 mag = vec3(200.0 / 255.0, 0.0, 200.0 / 255.0);
		vec3 gry = vec3(140.0 / 255.0, 140.0 / 255.0, 140.0 / 255.0);

		FragColor = vec4(mix(gry, mag, isLine), 1.0);
	}

)";

const char *boing_vshader = R"(

	#version 150 core

	in vec3 aPos;
	in vec3 aNorm;
	in vec2 aTex;
	in vec4 aCol;

	uniform mat4 uModelView;
	uniform mat4 uProjection;

	out vec3 vPos;
	out vec3 vNorm;

	void main()
	{
		vPos = aPos;
		vNorm = mat3(uModelView) * aNorm;
		gl_Position = uProjection * (uModelView * vec4(aPos, 1.0));
	}

)";

const char *boing_fshader = R"(

	#version 150 core

	in vec3 vPos;
	in vec3 vNorm;
	out vec4 FragColor;

	#define PI 3.14159265358979323846

	void main()
	{
		vec3 p = normalize(vPos);

		float phi = acos(clamp(p.y, -1.0, 1.0));
		float theta = atan(p.z, p.x);

		float rings = 16.0;
		float secs = 16.0;

		float u = (theta + PI) / (2.0 * PI) * secs;
		float v = phi / PI * rings;

		// Continuous 3D checkerboard wave removes the back seam artifact
		float s = sin(PI * u) * sin(PI * v);

		float fw = fwidth(s);
		float pattern = smoothstep(-fw, fw, s);

		vec3 redcol = vec3(220.0 / 255.0, 30.0 / 255.0, 30.0 / 255.0);
		vec3 whitecol = vec3(1.0, 1.0, 1.0);
		vec3 basecol = mix(whitecol, redcol, pattern);

		vec3 norm = normalize(vNorm);
		float diff = max(dot(norm, normalize(vec3(0.5, 1.0, 1.0))), 0.15);

		FragColor = vec4(basecol * diff, 1.0);
	}

)";
