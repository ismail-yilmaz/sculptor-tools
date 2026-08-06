#include "Demo.h"

namespace Upp {

void ApplyGradient(Model3D& model, Color lo, Color hi)
{
	Box3D bounds = model.ComputeBounds();
	float base = bounds.lo.y;
	float span = max(bounds.hi.y - bounds.lo.y, 0.001f);

	for(int i = 0; i < model.vertices.GetCount(); i++) {
		auto& v = model.vertices[i];
		float t = clamp<float>((v.position.y - base) / span, 0.0f, 1.0f);

		int r = int(lo.GetR() + (hi.GetR() - lo.GetR()) * t);
		int g = int(lo.GetG() + (hi.GetG() - lo.GetG()) * t);
		int b = int(lo.GetB() + (hi.GetB() - lo.GetB()) * t);

		v.color = Color(r, g, b);
	}
}

RGBA TriplanarSampler(const Sculptor::Fragment& frag)
{
	Point3D n = frag.normal;
	float ax = abs(n.x), ay = abs(n.y), az = abs(n.z);

	float u, v;
	if(ax >= ay && ax >= az) {
		u = frag.viewpos.y; v = frag.viewpos.z;
	}
	else
	if(ay >= ax && ay >= az) {
		u = frag.viewpos.x; v = frag.viewpos.z;
	}
	else {
		u = frag.viewpos.x; v = frag.viewpos.y;
	}

	bool dark = ((int(floor(u * 2.0f)) + int(floor(v * 2.0f))) & 1) == 0;

	RGBA c = dark ? Color(50, 55, 70) : White(); c.a = 255;
	return c;
}

RGBA RimShader(const Sculptor::Fragment& frag, RGBA base)
{
	Point3D viewDir = (Point3D(0, 0, 0) - frag.viewpos).Normalized();
	float rim = 1.0f - clamp<float>(DotProduct(viewDir, frag.normal), 0.0f, 1.0f);
	rim = pow(rim, 3.0f);

	RGBA c;
	c.r = clamp(base.r + int(rim * 180), 0, 255);
	c.g = clamp(base.g + int(rim * 60),  0, 255);
	c.b = clamp(base.b + int(rim * 255), 0, 255);
	c.a = base.a;
	return c;
}

void Vignette(ImageBuffer& cb, Buffer<float>& zb, Size sz)
{
	float cx = sz.cx * 0.5f;
	float cy = sz.cy * 0.5f;
	float maxd = max(cx * cx + cy * cy, 1.0f);

	for(int y = 0; y < sz.cy; y++) {
		RGBA* row = cb[y];
		for(int x = 0; x < sz.cx; x++) {
			float dx = x - cx;
			float dy = y - cy;
			float falloff = 1.0f - clamp((dx * dx + dy * dy) / maxd * 0.8f, 0.0f, 0.8f);
			row[x].r = int(row[x].r * falloff);
			row[x].g = int(row[x].g * falloff);
			row[x].b = int(row[x].b * falloff);
		}
	}
}

}