#include "SculptorCtrl.h"

#define LLOG(x)  RLOG("GLPipeline: " << x)

namespace Upp {

GLuint GLPipeline::defprogram = 0;
GLuint GLPipeline::postprogram = 0;
VectorMap<String, GLuint> GLPipeline::userprograms;
VectorMap<int64, GLuint> GLPipeline::texturecache;
bool GLPipeline::defaultshadercache = false;
String GLPipeline::defaultshadercachedir;

namespace {

String ResolveShaderCacheDir(const String& dir)
{
	return Nvl(dir, Nvl(GLPipeline::defaultshadercachedir, AppendFileName(GetTempDirectory(), "SculptorCtrlShaderCache")));
}

String GetShaderCachePath(const String& dir, const String& vsh, const String& fsh)
{
	Sha1Stream sha;
	sha.Put(vsh); sha.Put("", 1);
	sha.Put(fsh); sha.Put("", 1);

	if(const char* vnd = (const char*) glGetString(GL_VENDOR))
		sha.Put(vnd, (int) strlen(vnd) + 1);
	if(const char* ren = (const char*) glGetString(GL_RENDERER))
		sha.Put(ren, (int) strlen(ren) + 1);
	if(const char* ver = (const char*) glGetString(GL_VERSION))
		sha.Put(ver, (int) strlen(ver) + 1);
	
	return AppendFileName(ResolveShaderCacheDir(dir), sha.FinishString() + ".bin");
}

void ClearShaderCacheDir(const String& dir)
{
	String path = ResolveShaderCacheDir(dir);
	if(!DirectoryExists(path))
		return;

	for(FindFile ff(AppendFileName(path, "*.bin")); ff; ff.Next())
		if(ff.IsFile())
			DeleteFile(ff.GetPath());
}

bool SaveProgram(Stream& s, GLuint prog)
{
#ifdef GL_PROGRAM_BINARY_RETRIEVABLE_HINT
	if(!glGetProgramBinary)
		return false;

	GLint len = 0;
	glGetProgramiv(prog, GL_PROGRAM_BINARY_LENGTH, &len);
	if(len <= 0)
		return false;

	while(glGetError() != GL_NO_ERROR)
		; // NOOP

	Buffer<byte> buf(len);
	GLenum fmt = 0;
	GLsizei wrt = 0;
	glGetProgramBinary(prog, len, &wrt, &fmt, buf);
	if(wrt <= 0 || wrt > len || glGetError() != GL_NO_ERROR)
		return false;

	int ver = 1;
	dword f = (dword) fmt;
	int sz = (int) wrt;

	s.Magic(0x42474353); // "SCGB"
	s / ver;
	s % f % sz;
	s.SerializeRaw(buf, sz);
	return !s.IsError();
#else
	return false;
#endif
}

GLuint LoadProgram(Stream& s)
{
#ifdef GL_PROGRAM_BINARY_RETRIEVABLE_HINT
	if(!glProgramBinary)
		return 0;

	int ver = 1;
	dword fmt = 0;
	int sz = 0;

	s.Magic(0x42474353); // "SCGB"
	s / ver;
	if(s.IsError() || ver != 1) {
		s.SetError();
		return 0;
	}

	s % fmt % sz;
	if(s.IsError() || sz <= 0 || sz > s.GetLeft()) {
		s.SetError();
		return 0;
	}

	Buffer<byte> buf(sz);
	s.SerializeRaw(buf, sz);
	if(s.IsError())
		return 0;

	while(glGetError() != GL_NO_ERROR)
		; // NOOP

	GLuint prog = glCreateProgram();
	glProgramBinary(prog, (GLenum)fmt, buf, sz);

	int pok = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &pok);
	if(!pok || glGetError() != GL_NO_ERROR) {
		glDeleteProgram(prog);
		return 0;
	}

	return prog;
#else
	return 0;
#endif
}

}

GLPipeline::GLPipeline()
: shadercache(Null)
, curprogram(0)
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
		extern const char *scgl_def_vertex_shader, *scgl_def_fragment_shader;
		defprogram = CompileProg(scgl_def_vertex_shader, scgl_def_fragment_shader);
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

		if(postprogram && fboid) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fboid);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, viewsize.cx, viewsize.cy, 0, 0, viewsize.cx, viewsize.cy, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glGetError();
		}
		
		// Depth sort overlays (cheap)
		StableSort(overlays, [](const auto& a, const auto& b) { return a.z > b.z; });

		glDisable(GL_CULL_FACE);

		glEnable(GL_DEPTH_TEST);	// Enable depth testing so tags obey 3D mesh occlusion
		glDepthFunc(GL_LEQUAL);		// Required so 2D HUDs at z = -1.0 pass the depth test
		glDepthMask(GL_FALSE);		// Prevent transparent overlay quads from occluding each other

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		static GLuint overlayprog = 0;
		if(!overlayprog) {
			constexpr const char* vsrc = R"(
					#version 150 core
					in vec3 aPos; // Upgraded to vec3
					in vec2 aTex;
					out vec2 vTex;
					void main()
					{
						gl_Position = vec4(aPos, 1.0); // Honors the Z coordinate
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
			float z  = clamp(ov.z, -1.0f, 1.0f);

			float vertices[] = {
				x0, y1, z,  0.0f, 1.0f,
				x0, y0, z,  0.0f, 0.0f,
				x1, y0, z,  1.0f, 0.0f,
				x0, y1, z,  0.0f, 1.0f,
				x1, y0, z,  1.0f, 0.0f,
				x1, y1, z,  1.0f, 1.0f
			};

			GLuint vao, vbo;
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
		}

		glUseProgram(0);
		glDepthMask(GL_TRUE); // Restore depth mask
		glEnable(GL_DEPTH_TEST);

		if(culling)
			glEnable(GL_CULL_FACE);
	}
}

bool GLPipeline::IsShaderCacheEnabled() const
{
	return IsNull(shadercache) ? defaultshadercache : (bool)shadercache;
}

void GLPipeline::ClearDefaultShaderCache()
{
	ClearShaderCacheDir(defaultshadercachedir);
}

void GLPipeline::ClearShaderCache() const
{
	ClearShaderCacheDir(shadercachedir);
}

GLuint GLPipeline::CompileProg(const String& vsh, const String& fsh)
{
	return CompileProg(vsh, fsh, IsNull(shadercache) ? defaultshadercache : (bool)shadercache);
}

GLuint GLPipeline::CompileProg(const String& vsh, const String& fsh, bool cache)
{
	String path;
	if(cache) {
		path = GetShaderCachePath(shadercachedir, vsh, fsh);
		if(FileIn in(path); in) {
			if(GLuint cached = LoadProgram(in)) {
				LLOG("Loaded cached shader binary: " << path);
				return cached;
			}
		}
	}

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	const char* vp = ~vsh;
	glShaderSource(vs, 1, &vp, nullptr);
	glCompileShader(vs);

	int vok;
	if(glGetShaderiv(vs, GL_COMPILE_STATUS, &vok); !vok) {
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
	if(glGetShaderiv(fs, GL_COMPILE_STATUS, &fok); !fok) {
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

#ifdef GL_PROGRAM_BINARY_RETRIEVABLE_HINT
	bool cancache = cache && glProgramParameteri && glGetProgramBinary;
	if(cancache) {
		GLint nf = 0;
		if(glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &nf); nf <= 0)
			cancache = false;
		else
			glProgramParameteri(prog, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	}
#else
	bool cancache = false;
#endif

	glLinkProgram(prog);

	int pok;
	if(glGetProgramiv(prog, GL_LINK_STATUS, &pok); !pok) {
		char log[512];
		glGetProgramInfoLog(prog, 512, nullptr, log);
		LLOG("Shader Link Error:\n" << log);
		glDeleteProgram(prog);
		prog = 0;
	}
	else
	if(cancache) {
		RealizePath(path);
		String tmp = path + Format(".%08x%08x.tmp", (int) Random(), (int) Random());
		FileOut out(tmp);
		if(out && SaveProgram(out, prog)) {
			out.Close();
			if(!out.IsError()) {
				DeleteFile(path);
				if(!FileMove(tmp, path))
					DeleteFile(tmp);
			}
			else
				DeleteFile(tmp);
		}
		else {
			out.Close();
			DeleteFile(tmp);
		}
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
	return prog;
}

}