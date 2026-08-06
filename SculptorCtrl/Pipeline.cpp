#include "SculptorCtrl.h"

#define LLOG(x) // RLOG("GLPipeline: " << x)

namespace Upp {

GLuint GLPipeline::defprogram = 0;
GLuint GLPipeline::postprogram = 0;
VectorMap<String, GLuint> GLPipeline::userprograms;
VectorMap<int64, GLuint> GLPipeline::texturecache;

GLPipeline::GLPipeline()
: curprogram(0)
, fboid(0)
, fbotex(0)
, fbodepth(0)
, fbosize(0, 0)
, quadvao(0)
, quadvbo(0)
, culling(true)
, wireframe(false)
, shading(true)
, alphablend(false)
, bilinear(false)
, mipmap(false)
{
	projection = MatrixGL::Identity();
	modelview = MatrixGL::Identity();
}

GLPipeline::~GLPipeline()
{
	if(!GetCurrentGLContext())
		return;

	if(fboid)
		glDeleteFramebuffers(1, &fboid);

	if(fbotex)
		glDeleteTextures(1, &fbotex);

	if(fbodepth)
		glDeleteRenderbuffers(1, &fbodepth);

	if(quadvao)
		glDeleteVertexArrays(1, &quadvao);

	if(quadvbo)
		glDeleteBuffers(1, &quadvbo);
}

void GLPipeline::Init()
{
	if(!defprogram) {
		static String defvsh(scgl_def_vertex_shader, scgl_def_vertex_shader_length);
		static String deffsh(scgl_def_fragment_shader, scgl_def_fragment_shader_length);
		defprogram = CompileProg(defvsh, deffsh);
	}
}

void GLPipeline::Clear()
{
	lights.Clear();
	overlays.Clear();
	modelviewstack.Clear();
	floats.Clear();
	vec3s.Clear();
	rgbas.Clear();
	curprogram = 0;
	modelview = MatrixGL::Identity();
}

void GLPipeline::Reset()
{
	overlays.Trim(0);
	modelviewstack.Trim(0);
	floats.Trim(0);
	vec3s.Trim(0);
	rgbas.Trim(0);
}

void GLPipeline::Begin(Size viewsize)
{
	modelview = MatrixGL::Identity();
	modelviewstack.Clear();
	overlays.Clear();

	if(postprogram) {
		if(fbosize != viewsize) {
			if(fboid)
				glDeleteFramebuffers(1, &fboid);

			if(fbotex)
				glDeleteTextures(1, &fbotex);

			if(fbodepth)
				glDeleteRenderbuffers(1, &fbodepth);

			glGenFramebuffers(1, &fboid);
			glBindFramebuffer(GL_FRAMEBUFFER, fboid);

			glGenTextures(1, &fbotex);
			glBindTexture(GL_TEXTURE_2D, fbotex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewsize.cx, viewsize.cy, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbotex, 0);

			glGenRenderbuffers(1, &fbodepth);
			glBindRenderbuffer(GL_RENDERBUFFER, fbodepth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, viewsize.cx, viewsize.cy);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbodepth);

			fbosize = viewsize;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, fboid);
	}
}

void GLPipeline::End(Size viewsize)
{
	if(postprogram) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(postprogram);

		if(!quadvao) {
			float qd[] = {
				-1.0f,  1.0f,  0.0f, 1.0f,
				-1.0f, -1.0f,  0.0f, 0.0f,
				 1.0f, -1.0f,  1.0f, 0.0f,
				-1.0f,  1.0f,  0.0f, 1.0f,
				 1.0f, -1.0f,  1.0f, 0.0f,
				 1.0f,  1.0f,  1.0f, 1.0f
			};
			glGenVertexArrays(1, &quadvao);
			glGenBuffers(1, &quadvbo);
			glBindVertexArray(quadvao);
			glBindBuffer(GL_ARRAY_BUFFER, quadvbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(qd), qd, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glBindVertexArray(0);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbotex);
		glUniform1i(glGetUniformLocation(postprogram, "uTex"), 0);

		for(const auto& [key, val] : ~floats)
			glUniform1f(glGetUniformLocation(postprogram, key), val);

		glBindVertexArray(quadvao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	if(!overlays.IsEmpty()) {
		if(viewsize.cx <= 0 || viewsize.cy <= 0)
			return;

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		static GLuint overlayprog = 0;
		if(!overlayprog) {
			constexpr const char* vsrc = R"(
					#version 150 core
					in vec2 aPos;
					in vec2 aTex;
					out vec2 vTex;
					void main()
					{
						gl_Position = vec4(aPos, 0.0, 1.0);
						vTex = aTex;
					}
			)";
			constexpr const char* fsrc = R"(
					#version 150 core
					in  vec2 vTex;
					out vec4 FragColor;
					uniform sampler2D uTex;
					void main()
					{
						FragColor = texture(uTex, vTex);
					}
			)";
			GLuint vs = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vs, 1, &vsrc, nullptr);
			glCompileShader(vs);
			GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fs, 1, &fsrc, nullptr);
			glCompileShader(fs);

			overlayprog = glCreateProgram();
			glAttachShader(overlayprog, vs);
			glAttachShader(overlayprog, fs);
			glBindAttribLocation(overlayprog, 0, "aPos");
			glBindAttribLocation(overlayprog, 1, "aTex");
			glLinkProgram(overlayprog);

			glDeleteShader(vs);
			glDeleteShader(fs);
		}

		glUseProgram(overlayprog);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(overlayprog, "uTex"), 0);

		for(const OverlayItem& ov : overlays) {
			if(ov.img.IsEmpty())
				continue;

			int64 serial = ov.img.GetSerialId();
			int tix = texturecache.Find(serial);
			GLuint tex = 0;

			if(tix < 0) {
				glGenTextures(1, &tex);
				glBindTexture(GL_TEXTURE_2D, tex);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ov.img.GetWidth(), ov.img.GetHeight(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, ~ov.img);
				texturecache.Add(serial, tex);
			}
			else {
				tex = texturecache[tix];
				glBindTexture(GL_TEXTURE_2D, tex);
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, bilinear ? GL_LINEAR : GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, bilinear ? GL_LINEAR : GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			Size isz = ov.img.GetSize();
			float x0 = (2.0f * ov.pos.x / viewsize.cx) - 1.0f;
			float y0 = 1.0f - (2.0f * ov.pos.y / viewsize.cy);
			float x1 = (2.0f * (ov.pos.x + isz.cx) / viewsize.cx) - 1.0f;
			float y1 = 1.0f - (2.0f * (ov.pos.y + isz.cy) / viewsize.cy);

			float vertices[] = {
				x0, y1,  0.0f, 1.0f,
				x0, y0,  0.0f, 0.0f,
				x1, y0,  1.0f, 0.0f,
				x0, y1,  0.0f, 1.0f,
				x1, y0,  1.0f, 0.0f,
				x1, y1,  1.0f, 1.0f
			};

			GLuint vao, vbo;
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
		}

		glUseProgram(0);
		glEnable(GL_DEPTH_TEST);

		if(culling)
			glEnable(GL_CULL_FACE);
	}
}

GLuint GLPipeline::CompileProg(const String& vsh, const String& fsh)
{
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	const char* vp = ~vsh;
	glShaderSource(vs, 1, &vp, nullptr);
	glCompileShader(vs);

	int vok;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &vok);
	if(!vok) {
		char log[512];
		glGetShaderInfoLog(vs, 512, nullptr, log);
		LLOG("Vertex Shader Error:\n" << log);
		glDeleteShader(vs);
		return 0;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fp = ~fsh;
	glShaderSource(fs, 1, &fp, nullptr);
	glCompileShader(fs);

	int fok;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &fok);
	if(!fok) {
		char log[512];
		glGetShaderInfoLog(fs, 512, nullptr, log);
		LLOG("Fragment Shader Error:\n" << log);
		glDeleteShader(vs);
		glDeleteShader(fs);
		return 0;
	}

	GLuint prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);

	// Bind both short-names and long names for convenience
	glBindAttribLocation(prog, 0, "aPos");
	glBindAttribLocation(prog, 0, "position");

	glBindAttribLocation(prog, 1, "aNorm");
	glBindAttribLocation(prog, 1, "normal");

	glBindAttribLocation(prog, 2, "aTex");
	glBindAttribLocation(prog, 2, "texcoord");

	glBindAttribLocation(prog, 3, "aCol");
	glBindAttribLocation(prog, 3, "color");

	glLinkProgram(prog);

	int pok;
	glGetProgramiv(prog, GL_LINK_STATUS, &pok);
	if(!pok) {
		char log[512];
		glGetProgramInfoLog(prog, 512, nullptr, log);
		LLOG("Shader Link Error:\n" << log);
		glDeleteProgram(prog);
		prog = 0;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
	return prog;
}

}