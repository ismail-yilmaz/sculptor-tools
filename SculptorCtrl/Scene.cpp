#include "SculptorCtrl.h"

#define LLOG(x) // RLOG("GLScene: " << x)

namespace Upp {

GLScene::GLScene(GLPipeline& pipeline)
: glpipeline(pipeline)
{
}

GLScene& GLScene::Cull(bool b)
{
	glpipeline.culling = b;
	return *this;
}

bool GLScene::IsCulling() const
{
	return glpipeline.culling;
}

GLScene& GLScene::Wireframe(bool b)
{
	glpipeline.wireframe = b;
	return *this;
}

bool GLScene::IsWireFrame() const
{
	return glpipeline.wireframe;
}

GLScene& GLScene::Shading(bool b)
{
	glpipeline.shading = b;
	return *this;
}

bool GLScene::IsShading() const
{
	return glpipeline.shading;
}

GLScene& GLScene::AlphaBlend(bool b)
{
	glpipeline.alphablend = b;
	return *this;
}

bool GLScene::IsAlphaBlending() const
{
	return glpipeline.alphablend;
}

GLScene& GLScene::Bilinear(bool b)
{
	glpipeline.bilinear = b;
	return *this;
}

bool GLScene::IsBilinear() const
{
	return glpipeline.bilinear;
}

GLScene& GLScene::MipMap(bool b)
{
	glpipeline.mipmap = b;
	return *this;
}

bool GLScene::IsMipMapping() const
{
	return glpipeline.mipmap;
}

void GLScene::Frustum(const Rectf& v, float fnear, float ffar)
{
	glpipeline.projection = MatrixGL::Frustum(v, fnear, ffar);
}

void GLScene::Perspective(float fov, float aspect, float znear, float zfar)
{
	glpipeline.projection = MatrixGL::Perspective(fov, aspect, znear, zfar);
}

void GLScene::Orthographic(const Rectf& view, float znear, float zfar)
{
	glpipeline.projection = MatrixGL::Orthographic(view, znear, zfar);
}

void GLScene::Isometric(const Rectf& view, float fnear, float ffar)
{
	glpipeline.projection = MatrixGL::Isometric(view, fnear, ffar);
}

void GLScene::Isometric(float zoom, float aspectratio, float fnear, float ffar)
{
	glpipeline.projection = MatrixGL::Isometric(zoom, aspectratio, fnear, ffar);
}

void GLScene::LookAt(const Point3D& eye, const Point3D& center, const Point3D& up)
{
	glpipeline.modelview *= MatrixGL::LookAt(eye, center, up);
}

void GLScene::PushMatrix()
{
	glpipeline.modelviewstack.Add(glpipeline.modelview);
}

void GLScene::PopMatrix()
{
	if(glpipeline.modelviewstack.GetCount()) {
		glpipeline.modelview = glpipeline.modelviewstack.Top();
		glpipeline.modelviewstack.Drop();
	}
}

GLScene& GLScene::Identity()
{
	glpipeline.modelview = MatrixGL::Identity();
	return *this;
}

GLScene& GLScene::Translate(float x, float y, float z)
{
	glpipeline.modelview *= MatrixGL::Translation(x, y, z);
	return *this;
}

GLScene& GLScene::Translate(const Point3D& p)
{
	return Translate(p.x, p.y, p.z);
}

GLScene& GLScene::Rotate(float rx, float ry, float rz)
{
	glpipeline.modelview *= MatrixGL::Rotation(rx, ry, rz);
	return *this;
}

GLScene& GLScene::Rotate(const Point3D& p)
{
	return Rotate(p.x, p.y, p.z);
}

GLScene& GLScene::Rotate(const Quaternion& q)
{
	glpipeline.modelview *= q.GetMatrixGL();
	return *this;
}

GLScene& GLScene::Scale(float sx, float sy, float sz)
{
	glpipeline.modelview *= MatrixGL::Scale(sx, sy, sz);
	return *this;
}

GLScene& GLScene::Scale(const Point3D& p)
{
	return Scale(p.x, p.y, p.z);
}

GLScene& GLScene::Scale(float scale)
{
	glpipeline.modelview *= MatrixGL::Scale(scale);
	return *this;
}

void GLScene::SetLight(const Point3D& dir, const RGBA& color)
{
	glpipeline.lights.Clear();
	AddLight(dir, color);
}

GLScene& GLScene::AddLight(const Point3D& dir, const RGBA& color)
{
	GLPipeline::LightSource& l = glpipeline.lights.Add();
	l.dir = dir.Normalized();
	l.color = color;
	return *this;
}

void GLScene::ClearLights()
{
	glpipeline.lights.Clear();
}

GLScene& GLScene::AddOverlay(const Point& pos, const Image& img)
{
	GLPipeline::OverlayItem& o = glpipeline.overlays.Add();
	o.pos = pos;
	o.img = img;
	return *this;
}

GLScene& GLScene::AddOverlay(int x, int y, const Image& img)
{
	return AddOverlay(Point(x, y), img);
}

void GLScene::ClearOverlays()
{
	glpipeline.overlays.Clear();
}

void GLScene::Uniform(const String& name, float v)
{
	glpipeline.floats.GetAdd(name) = v;
}

void GLScene::Uniform(const String& name, const Point3D& v)
{
	glpipeline.vec3s.GetAdd(name) = v;
}

void GLScene::Uniform(const String& name, const RGBA& v)
{
	glpipeline.rgbas.GetAdd(name) = v;
}

void GLScene::UseShader(const String& name)
{
	if(name.IsEmpty()) {
		glpipeline.curprogram = 0;
	}
	else {
		int i = glpipeline.userprograms.Find(name);
		if(i >= 0)
			glpipeline.curprogram = glpipeline.userprograms[i];
		else
			glpipeline.curprogram = 0;
	}
}

void GLScene::ClearBuffer(const RGBA& c)
{
	glClearDepth(1);
	glClearColor(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GLScene::Clear()
{
	glpipeline.Clear();
}

void GLScene::Reset()
{
	glpipeline.Reset();
}

Image GLScene::GetImage(Size sz)
{
	ImageBuffer ib(sz);
	glReadPixels(0, 0, sz.cx, sz.cy, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*) ~ib);

	if(glGetError() != GL_NO_ERROR)
		return Image();

	Buffer<RGBA> tmp(sz.cx);
	for(int y = 0; y < sz.cy / 2; y++) {
		memcpy(tmp, ib[y], sz.cx * sizeof(RGBA));
		memcpy(ib[y], ib[sz.cy - 1 - y], sz.cx * sizeof(RGBA));
		memcpy(ib[sz.cy - 1 - y], tmp, sz.cx * sizeof(RGBA));
	}

	for(RGBA& t : ib)
		t.a = 255;

	return Image(ib);
}

void GLScene::Render(ModelGL& model)
{
	if(!model.compiled)
		model.Compile();

	if(!model.vertexbuffer)
		return;

	glEnable(GL_DEPTH_TEST);

	GLuint prog = glpipeline.curprogram ? glpipeline.curprogram : glpipeline.defprogram;
	glUseProgram(prog);

	glUniformMatrix4fv(glGetUniformLocation(prog, "uModelView"), 1, GL_FALSE, &glpipeline.modelview.x.x);
	glUniformMatrix4fv(glGetUniformLocation(prog, "uProjection"), 1, GL_FALSE, &glpipeline.projection.x.x);

	MatrixGL nmat = glpipeline.modelview.Inverse();
	glUniformMatrix4fv(glGetUniformLocation(prog, "uNormalMatrix"), 1, GL_TRUE, &nmat.x.x);

	int lightcount = min(glpipeline.lights.GetCount(), 32);
	glUniform1i(glGetUniformLocation(prog, "uLightCount"), lightcount);

	for(int i = 0; i < lightcount; i++) {
		String dnm = Format("uLightDir[%d]", i);
		String cnm = Format("uLightCol[%d]", i);
		glUniform3f(glGetUniformLocation(prog, ~dnm), glpipeline.lights[i].dir.x, glpipeline.lights[i].dir.y, glpipeline.lights[i].dir.z);
		glUniform4f(glGetUniformLocation(prog, ~cnm), glpipeline.lights[i].color.r / 255.f, glpipeline.lights[i].color.g / 255.f, glpipeline.lights[i].color.b / 255.f, 1.0f);
	}

	for(const auto& [key, val] : ~glpipeline.floats)
		glUniform1f(glGetUniformLocation(prog, ~key), val);

	for(const auto& [key, pt] : ~glpipeline.vec3s)
		glUniform3f(glGetUniformLocation(prog, ~key), pt.x, pt.y, pt.z);

	for(const auto& [key, c] : ~glpipeline.rgbas)
		glUniform4f(glGetUniformLocation(prog, ~key), c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f);

	glBindVertexArray(model.GetVAO());

	GLint utx = glGetUniformLocation(prog, "uUseTex");
	GLint dwr = glGetUniformLocation(prog, "uDrawWire");

	// Reusable draw loop for handling multi-pass execution
	auto draw = [&]() {
		for(const ModelGL::Batch& b : model.batches) {
			bool usetex = false;
			if(b.textureid != 0) {
				if(int i = GLPipeline::texturecache.Find(b.textureid); i >= 0) {
					glActiveTexture(GL_TEXTURE0);
					GLuint tid = (GLuint) GLPipeline::texturecache[i];
					glBindTexture(GL_TEXTURE_2D, tid);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glpipeline.mipmap ? GL_LINEAR_MIPMAP_LINEAR : (glpipeline.bilinear ? GL_LINEAR : GL_NEAREST));
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glpipeline.bilinear ? GL_LINEAR : GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					usetex = true;
				}
			}
			if(utx >= 0)
				glUniform1i(utx, usetex ? 1 : 0);
			glDrawElements(GL_TRIANGLES, b.count, GL_UNSIGNED_INT, (void*)(intptr_t)(b.offset * sizeof(int)));
		}
	};

	bool shading = glpipeline.shading;
	bool wireframe = glpipeline.wireframe;

	if(!shading && !wireframe)
		shading = true;

	auto dps = [&]() {
		if(shading) { // Pass 1: Solid shading
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			if(wireframe) {
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0f, 1.0f);
			}
			if(dwr >= 0)
				glUniform1i(dwr, 0);
			draw();
			if(wireframe)
				glDisable(GL_POLYGON_OFFSET_FILL);
		}
		if(wireframe) { // Pass 2: Wireframe edges
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if(dwr >= 0)
				glUniform1i(dwr, 1);
			draw();
		}
	};

	if(glpipeline.alphablend) {
		// Two-Pass OIT
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		dps(); // Pass 1: Inner geometry
		glCullFace(GL_BACK);
		dps(); // Pass 2: Outer geometry
		glDepthMask(GL_TRUE);
	}
	else {
		glDisable(GL_BLEND);
		if(glpipeline.culling) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else
			glDisable(GL_CULL_FACE);
		dps();
	}

	// Restore safe default polygon mode state
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(0);
	glUseProgram(0);
}

}