#include "SculptorCtrl.h"

#define LLOG(x) // RLOG("SculptorCtrl: " << x)

namespace Upp {

int  SculptorCtrl::depthbits   = 24;
int  SculptorCtrl::stencilbits = 8;
int  SculptorCtrl::msaasamples = 1;
bool SculptorCtrl::doublebuffering = true;
Size SculptorCtrl::currentviewport;

extern void (*restore_gl_viewport__)();

SculptorCtrl::SculptorCtrl()
{
	Init();
}

SculptorCtrl::SculptorCtrl(int depthbits, int stencilbits, bool doublebuffer, int samples)
{
	Init();
	SetDepthBits(depthbits);
	SetStencilBits(stencilbits);
	SetDoubleBuffering(doublebuffer);
	SetMSAA(samples);
}

void SculptorCtrl::SetShader(const String& name, const String& vshader, const String& fshader)
{
	ClearShaders();
	AddShader(name, vshader, fshader);
}

void SculptorCtrl::AddShader(const String& name, const String& vshader, const String& fshader)
{
	GLuint newprg = glpipeline.CompileProg(vshader, fshader);
	if(!newprg)
		return;

	int i = GLPipeline::userprograms.Find(name);
	if(i >= 0) {
		GLuint oldprg = GLPipeline::userprograms[i];
		if(oldprg)
			glDeleteProgram(oldprg);
		GLPipeline::userprograms[i] = newprg;
	}
	else
		GLPipeline::userprograms.Add(name, newprg);
}

void SculptorCtrl::RemoveShader(const String& name)
{
	int i = GLPipeline::userprograms.Find(name);
	if(i >= 0) {
		GLuint prg = GLPipeline::userprograms[i];
		if(prg)
			glDeleteProgram(prg);
		GLPipeline::userprograms.Remove(i);
	}
}

void SculptorCtrl::ClearShaders()
{
	for(GLuint prg : GLPipeline::userprograms) {
		if(prg)
			glDeleteProgram(prg);
	}
	GLPipeline::userprograms.Clear();
	glpipeline.curprogram = 0;
}

void SculptorCtrl::SetPostProcess(const String& fsh)
{
	ClearPostProcess();
	
	constexpr const char *vsh = R"(
		#version 150 core
		in  vec2 aPos;
		in  vec2 aTex;
		out vec2 vTex;
		void main()
		{
		    gl_Position = vec4(aPos, 0.0, 1.0);
		    vTex = aTex;
		}
	)";

	glpipeline.postprogram = glpipeline.CompileProg(vsh, fsh);
}

void SculptorCtrl::ClearPostProcess()
{
	if(glpipeline.postprogram)
		glDeleteProgram(glpipeline.postprogram);
	glpipeline.postprogram = 0;
}

void SculptorCtrl::RemoveTexture(int64 id)
{
	int i = GLPipeline::texturecache.Find(id);
	if(i >= 0) {
		GLuint tex = GLPipeline::texturecache[i];
		if(tex)
			glDeleteTextures(1, &tex);
		GLPipeline::texturecache.Remove(i);
	}
}

void SculptorCtrl::ClearTextures()
{
	for(GLuint tex : GLPipeline::texturecache) {
		if(tex)
			glDeleteTextures(1, &tex);
	}
	GLPipeline::texturecache.Clear();
}

void SculptorCtrl::DoRender()
{
	MemoryIgnoreLeaksBlock __;

	Size sz = GetSize();
	if(sz.cx <= 0 || sz.cy <= 0)
		return;

	currentviewport = sz;
	SetCurrentViewport();

	if(!initialized) {
		glpipeline.Init();
		initialized = true;
		InitScene();
	}

	glEnable(GL_MULTISAMPLE);
	glpipeline.Begin(sz);
	GLScene scene(glpipeline);
	Render(scene);
	glpipeline.End(sz);
}

void SculptorCtrl::Init()
{
	Transparent();
#ifdef PLATFORM_WIN32
	pane.ctrl = this;
	Add(pane.SizePos());
#endif
	restore_gl_viewport__ = SetCurrentViewport;
	MemoryIgnoreNonMainLeaks();
	MemoryIgnoreNonUppThreadsLeaks();
}

Image SculptorCtrl::MouseEvent(int event, Point p, int zdelta, dword keyflags)
{
	if(mousetarget)
		return mousetarget->MouseEvent(event, p + GetScreenView().TopLeft()
		     - mousetarget->GetScreenView().TopLeft(), zdelta, keyflags);
	return Ctrl::MouseEvent(event, p, zdelta, keyflags);
}

void SculptorCtrl::SetCurrentViewport()
{
	glViewport(0, 0, (GLsizei) currentviewport.cx, (GLsizei) currentviewport.cy);
}

int64 GetCurrentGLContext()
{
#ifdef PLATFORM_WIN32
	return (int64)(intptr_t) wglGetCurrentContext();
#else
	return (int64)(intptr_t) glXGetCurrentContext();
#endif
}

}