#include "Sculptor.h"

#define LLOG(x) // RLOG("SculptorCtrl: " << x)

namespace Upp {

Sculptor::Sculptor()
: size(0, 0)
, scissor(Null)
, parallel(false)
, culling(true)
, wireframe(false)
, shading(true)
, alphablend(false)
, bilinear(false)
, mipmap(false)
{
    matrixstack.Add(Matrix4D::Identity());
    projection = view = Matrix4D::Identity();
    ambientlight =  { 50, 50, 50, 255 };
}

Sculptor::Sculptor(Size sz, const RGBA& paper)
: Sculptor()
{
    SetCanvas(sz, paper);
}

void Sculptor::Clear()
{
    size = Size(0, 0);
    colorbuffer.Clear();
    zbuffer.Clear();
    primitives.Clear();
    matrixstack.Clear();
    matrixstack.Add(Matrix4D::Identity());
    projection = view = Matrix4D::Identity();
    ambientlight =  { 50, 50, 50, 255 };
    toscreen.Clear();
}

void Sculptor::SetCanvas(Size sz, const RGBA& paper)
{
    colorbuffer.Create(sz);
    if(size != sz || zbuffer.IsEmpty()) {
        size = sz;
        zbuffer.Alloc(sz.cx * sz.cy);
    }

    ClearCanvas(paper);
}

void Sculptor::ClearCanvas(const RGBA& paper)
{
    ClearColorBuffer(paper);
    ClearDepthBuffer();
}

void Sculptor::ClearColorBuffer(const RGBA& paper)
{
    if(colorbuffer.IsEmpty())
        return;

    Fill(colorbuffer, paper, colorbuffer.GetLength());
}

void Sculptor::ClearDepthBuffer()
{
    if(zbuffer.IsEmpty())
        return;

    int total = size.cx * size.cy;
    for(int i = 0; i < total; i++)
        (~zbuffer)[i] = 1e20f;
}

Image Sculptor::GetImage()
{
    return colorbuffer;
}

const Vector<Sculptor::Primitive>& Sculptor::GetPrimitives() const
{
    return primitives;
}

void Sculptor::Perspective(float fov, float aspect, float znear, float zfar)
{
    projection = Matrix4D::Perspective(fov, aspect, znear, zfar);
}

void Sculptor::Orthographic(const Rectf& v, float znear, float zfar)
{
    projection = Matrix4D::Orthographic(v, znear, zfar);
}

void Sculptor::Frustum(const Rectf& v, float fnear, float ffar)
{
    projection = Matrix4D::Frustum(v, fnear, ffar);
}

void Sculptor::Isometric(const Rectf& v, float fnear, float ffar)
{
    projection = Matrix4D::Isometric(v, fnear, ffar);
}

void Sculptor::Isometric(float zoom, float aspectratio, float fnear, float ffar)
{
    projection = Matrix4D::Isometric(zoom, aspectratio, fnear, ffar);
}

void Sculptor::LookAt(const Point3D& eye, const Point3D& center, const Point3D& up)
{
    view = Matrix4D::LookAt(eye, center, up);
}

void Sculptor::PushMatrix()
{
    Matrix4D top = matrixstack.Top();
    matrixstack.Add(top);
}

void Sculptor::PopMatrix()
{
    if(matrixstack.GetCount() > 1)
        matrixstack.Drop();
}

Sculptor& Sculptor::Identity()
{
    matrixstack.Top() = Matrix4D::Identity();
    return *this;
}

Sculptor& Sculptor::Translate(float x, float y, float z)
{
    matrixstack.Top() = Matrix4D::Translation(x, y, z) * matrixstack.Top();
    return *this;
}

Sculptor& Sculptor::Rotate(float rx, float ry, float rz)
{
    matrixstack.Top() = Matrix4D::Rotation(rx, ry, rz) * matrixstack.Top();
    return *this;
}

Sculptor& Sculptor::Scale(float sx, float sy, float sz)
{
    matrixstack.Top() = Matrix4D::Scale(sx, sy, sz) * matrixstack.Top();
    return *this;
}

void Sculptor::SetLight(const Point3D& dir, const RGBA& color)
{
    ClearLights();
    AddLight(dir, color);
}

Sculptor& Sculptor::AddLight(const Point3D& dir, const RGBA& color)
{
    LightSource& l = lights.Add();
    l.dir = dir.Normalized();
    l.color = color;
    return *this;
}

void Sculptor::ClearLights()
{
    lights.Clear();
}

void Sculptor::SetTextureSampler(Function<RGBA(const Fragment&)> fn)
{
    texturesampler = fn;
}

void Sculptor::SetPixelShader(Function<RGBA(const Fragment&, RGBA)> fn)
{
    pixelshader = fn;
}

Sculptor& Sculptor::AddPostFilter(Event<ImageBuffer&, Buffer<float>&, Size> fn)
{
    postfilters.Add(fn);
    return *this;
}

void Sculptor::ClearPostFilters()
{
    postfilters.Clear();
}

void Sculptor::SetNDCConverter(Function<Pointf(const Point3D&)> f)
{
    toscreen = f;
}

struct ClipVert {
    Point4D clippos;
    Point3D viewpos;
    Point3D normal;
    RGBA    color;
    Pointf  texcoord;
};

static ClipVert LerpVert(const ClipVert& a, const ClipVert& b, float t)
{
    ClipVert r;

    r.clippos.x = a.clippos.x + (b.clippos.x - a.clippos.x) * t;
    r.clippos.y = a.clippos.y + (b.clippos.y - a.clippos.y) * t;
    r.clippos.z = a.clippos.z + (b.clippos.z - a.clippos.z) * t;
    r.clippos.w = a.clippos.w + (b.clippos.w - a.clippos.w) * t;

    r.viewpos.x = a.viewpos.x + (b.viewpos.x - a.viewpos.x) * t;
    r.viewpos.y = a.viewpos.y + (b.viewpos.y - a.viewpos.y) * t;
    r.viewpos.z = a.viewpos.z + (b.viewpos.z - a.viewpos.z) * t;

    r.normal.x = a.normal.x + (b.normal.x - a.normal.x) * t;
    r.normal.y = a.normal.y + (b.normal.y - a.normal.y) * t;
    r.normal.z = a.normal.z + (b.normal.z - a.normal.z) * t;

    r.texcoord.x = a.texcoord.x + (b.texcoord.x - a.texcoord.x) * t;
    r.texcoord.y = a.texcoord.y + (b.texcoord.y - a.texcoord.y) * t;

    int cr = a.color.r + int((b.color.r - a.color.r) * t);
    int cg = a.color.g + int((b.color.g - a.color.g) * t);
    int cb = a.color.b + int((b.color.b - a.color.b) * t);
    int ca = a.color.a + int((b.color.a - a.color.a) * t);

    r.color.r = clamp(cr, 0, 255);
    r.color.g = clamp(cg, 0, 255);
    r.color.b = clamp(cb, 0, 255);
    r.color.a = clamp(ca, 0, 255);

    return r;
}

static int ClipPoly(const ClipVert* src, int incount, ClipVert* dst, int plane, float eps)
{
    int outcount = 0;
    for(int i = 0; i < incount; i++) {
        const ClipVert& cur = src[i];
        const ClipVert& nxt = src[(i + 1) % incount];

        auto inside = [&](const ClipVert& v) {
            float w = v.clippos.w;
            switch(plane) {
                case 0: return w > eps;
                case 1: return v.clippos.x >= -w;
                case 2: return v.clippos.x <= w;
                case 3: return v.clippos.y >= -w;
                case 4: return v.clippos.y <= w;
                case 5: return v.clippos.z <= w;
            }
            return false;
        };

        bool curin = inside(cur);
        bool nxtin = inside(nxt);

        if(curin)
            dst[outcount++] = cur;

        if(curin != nxtin) {
            float dcur = 0, dnxt = 0;
            switch(plane) {
                case 0: dcur = cur.clippos.w - eps; dnxt = nxt.clippos.w - eps; break;
                case 1: dcur = cur.clippos.w + cur.clippos.x; dnxt = nxt.clippos.w + nxt.clippos.x; break;
                case 2: dcur = cur.clippos.w - cur.clippos.x; dnxt = nxt.clippos.w - nxt.clippos.x; break;
                case 3: dcur = cur.clippos.w + cur.clippos.y; dnxt = nxt.clippos.w + nxt.clippos.y; break;
                case 4: dcur = cur.clippos.w - cur.clippos.y; dnxt = nxt.clippos.w - nxt.clippos.y; break;
                case 5: dcur = cur.clippos.w - cur.clippos.z; dnxt = nxt.clippos.w - nxt.clippos.z; break;
            }
            float t = dcur / (dcur - dnxt);
            dst[outcount++] = LerpVert(cur, nxt, t);
        }
    }
    return outcount;
}

void Sculptor::Render(const Model3D& m)
{
    Matrix4D modelview = matrixstack.Top() * view;
    Matrix4D mvp = modelview * projection;

    // Normals need the inverse-transpose of modelview's linear part, not
    // modelview itself; the two only coincide under rotation, translation
    // and uniform scale. Non-uniform Scale() would otherwise skew lighting.
    // Computed once per Render() call, not per vertex.
    Matrix4D normalmat;
    bool hasnormalmat = false;
    if(shading) {
        normalmat = modelview.Inverse();
        hasnormalmat = !normalmat.IsNullInstance();
    }

    int vcount = m.vertices.GetCount();
    Vector<Point4D> cc;
    Vector<Point3D> vc;
    Vector<Point3D> vn;
    Vector<RGBA> lcol;

    cc.SetCount(vcount);
    vc.SetCount(vcount);
    vn.SetCount(vcount);
    lcol.SetCount(vcount);

    auto process = [&](int i) {
        const Model3D::Vertex& v = m.vertices[i];
        Point4D p(v.position, 1.0);
        cc[i] = p * mvp;
        vc[i] = v.position * modelview;
        lcol[i] = v.color;

        Point3D n(0, 0, 0);
        if(hasnormalmat) {
            // Inverse-transpose applied to a row vector means dotting the
            // normal with the ROWS of normalmat (not its columns, as
            // position transforms do) - that row-dot is the transpose step.
            n.x = v.normal.x * normalmat.x.x + v.normal.y * normalmat.x.y + v.normal.z * normalmat.x.z;
            n.y = v.normal.x * normalmat.y.x + v.normal.y * normalmat.y.y + v.normal.z * normalmat.y.z;
            n.z = v.normal.x * normalmat.z.x + v.normal.y * normalmat.z.y + v.normal.z * normalmat.z.z;
            n.Normalize();
        }
        vn[i] = n;
    };

    if(parallel && vcount > 100) {
        CoFor(vcount, process);
    }
    else {
        for(int i = 0; i < vcount; ++i)
            process(i);
    }

    float neareps = 0.01;

    for(int i = 0; i < m.triangles.GetCount(); i++) {
        const Model3D::Triangle& t = m.triangles[i];

        Point3D v0 = vc[t.a];
        Point3D v1 = vc[t.b];
        Point3D v2 = vc[t.c];

        Point3D n = Normal(v0, v1, v2);

        if(culling && DotProduct(v0, n) >= 0.0)
            continue;

        ClipVert cv[3];
        cv[0].clippos = cc[t.a]; cv[0].viewpos = v0; cv[0].normal = vn[t.a]; cv[0].color = lcol[t.a]; cv[0].texcoord = m.vertices[t.a].texcoord;
        cv[1].clippos = cc[t.b]; cv[1].viewpos = v1; cv[1].normal = vn[t.b]; cv[1].color = lcol[t.b]; cv[1].texcoord = m.vertices[t.b].texcoord;
        cv[2].clippos = cc[t.c]; cv[2].viewpos = v2; cv[2].normal = vn[t.c]; cv[2].color = lcol[t.c]; cv[2].texcoord = m.vertices[t.c].texcoord;

        ClipVert cp1[10];
        ClipVert cp2[10];
        for(int k = 0; k < 3; k++)
            cp1[k] = cv[k];

        ClipVert* src = cp1;
        ClipVert* dst = cp2;
        int nc = 3;

        for(int p = 0; p < 6; p++) {
            nc = ClipPoly(src, nc, dst, p, neareps);
            if(nc < 3)
                break;
            Swap(src, dst);
        }

        if(nc < 3)
            continue;

        int64 tid = m.vertices[t.a].textureid;
        const Image* tex = nullptr;
        const Vector<Image>* mips = nullptr;

        if(tid != 0) {
            if(int tix = m.textures.Find(tid); tix >= 0)
                tex = &m.textures[tix];

            if(int mix = m.mipmaps.Find(tid); mix >= 0)
                mips = &m.mipmaps[mix];
        }

        int tricount = nc - 2;
        for(int k = 0; k < tricount; k++) {
            const ClipVert& a = src[0];
            const ClipVert& b = src[k + 1];
            const ClipVert& c = src[k + 2];

            Primitive& prim = primitives.Add();

            auto GetScreenPos = [&](const ClipVert& cv) -> Pointf {
                Point3D p = cv.clippos.ToPoint3DAffine();
                if(toscreen)
                    return toscreen(p);
                return Pointf((p.x + 1.0f) * 0.5f * size.cx, (1.0f - p.y) * 0.5f * size.cy);
            };

            prim.screenpos[0] = GetScreenPos(a);
            prim.screenpos[1] = GetScreenPos(b);
            prim.screenpos[2] = GetScreenPos(c);

            prim.inversew[0] = 1.0 / a.clippos.w;
            prim.inversew[1] = 1.0 / b.clippos.w;
            prim.inversew[2] = 1.0 / c.clippos.w;

            prim.viewpos[0] = a.viewpos;
            prim.viewpos[1] = b.viewpos;
            prim.viewpos[2] = c.viewpos;

            prim.texcoords[0] = a.texcoord;
            prim.texcoords[1] = b.texcoord;
            prim.texcoords[2] = c.texcoord;

            prim.colors[0] = a.color;
            prim.colors[1] = b.color;
            prim.colors[2] = c.color;

            prim.normals[0] = a.normal;
            prim.normals[1] = b.normal;
            prim.normals[2] = c.normal;

            prim.texture = tex;
            prim.mipmaps = mips;
            prim.wireframe = wireframe;
            prim.translucent = alphablend;
            prim.shading = shading;
        }
    }
}

void Sculptor::ClearScene()
{
    primitives.Trim(0);
    matrixstack.Trim(0);
    matrixstack.Add(Matrix4D::Identity());
    projection = Matrix4D::Identity();
    view = Matrix4D::Identity();
}

void Sculptor::Rasterize()
{
    if(primitives.IsEmpty() || colorbuffer.IsEmpty())
        return;

    // Wrapper for sorting translucent polygons back-to-front
    struct TransPrim {
        int id;
        float z;
        bool operator<(const TransPrim& b) const {
            return z > b.z; // Sort descending (larger Z rendered first)
        }
    };

    Vector<int> opaque;
    Vector<TransPrim> trans;

    // Segregate geometry
    for(int i = 0; i < primitives.GetCount(); i++) {
        if(primitives[i].wireframe)
            continue;
        if(primitives[i].translucent) {
            TransPrim tp;
            tp.id = i;
            const Primitive& prim = primitives[i];
            float iwp = prim.inversew[0] + prim.inversew[1] + prim.inversew[2];
            float zp = (prim.viewpos[0].z*prim.inversew[0] +
                        prim.viewpos[1].z*prim.inversew[1] +
                        prim.viewpos[2].z*prim.inversew[2]) / iwp;
            tp.z = zp;
            trans.Add(tp);
        }
        else
            opaque.Add(i);
    }

    Sort(trans);

    Vector<int> transl;
    for(const TransPrim& t : trans)
        transl.Add(t.id);

    int cores = 1;
    if(parallel)
        cores = max(1, CPU_Cores());

    int bandh = (size.cy + cores - 1) / cores;

    auto DrawBand = [&](int c) {
        int ys = c * bandh;
        int ye = ys + bandh - 1;
        ye = min(size.cy - 1, ye);

        auto DrawPrisms = [&](const Vector<int>& list) {
            for(int i = 0; i < list.GetCount(); i++) {
                const Primitive& prim = primitives[list[i]];
                float y0 = prim.screenpos[0].y;
                float y1 = prim.screenpos[1].y;
                float y2 = prim.screenpos[2].y;

                float minpy = min(y0, min(y1, y2));
                float maxpy = max(y0, max(y1, y2));

                int miny = max(0, (int) floor(minpy));
                int maxy = min(size.cy - 1, (int) ceil(maxpy));

                if(maxy < ys || miny > ye)
                    continue;

                int clipmin = max(miny, ys);
                int clipmax = min(maxy, ye);

                RasterizeTriangle(prim, size, clipmin, clipmax);
            }
        };

        // Opaque pass populates Z-buffer
        DrawPrisms(opaque);

        // Translucent pass evaluates against Z-buffer but writes color only
        DrawPrisms(transl);
    };

    if(parallel && cores > 1)
        CoFor(cores, DrawBand);
    else
        DrawBand(0);

    // Wireframe overlay
    for(const Primitive& prim : primitives) {
        if(prim.wireframe) {
            RasterizeLine(size, prim.screenpos[0], prim.viewpos[0].z, prim.screenpos[1], prim.viewpos[1].z, prim.colors[0]);
            RasterizeLine(size, prim.screenpos[1], prim.viewpos[1].z, prim.screenpos[2], prim.viewpos[2].z, prim.colors[1]);
            RasterizeLine(size, prim.screenpos[2], prim.viewpos[2].z, prim.screenpos[0], prim.viewpos[0].z, prim.colors[2]);
        }
    }

    for(const auto& postfilter : postfilters)
        postfilter(colorbuffer, zbuffer, size);
}

void Sculptor::RasterizeTriangle(const Primitive& prim, Size size, int clipminy, int clipmaxy)
{
    float px0 = prim.screenpos[0].x;
    float px1 = prim.screenpos[1].x;
    float px2 = prim.screenpos[2].x;

    float minpx = min(px0, min(px1, px2));
    float maxpx = max(px0, max(px1, px2));

    int minx = max(0, (int)floor(minpx));
    int maxx = min(size.cx - 1, (int)ceil(maxpx));

    int miny = clipminy;
    int maxy = clipmaxy;

    if(!IsNull(scissor)) {
        minx = max(minx, scissor.left);
        maxx = min(maxx, scissor.right - 1);
        miny = max(miny, scissor.top);
        maxy = min(maxy, scissor.bottom - 1);
    }

    if(minx > maxx || miny > maxy)
        return; // Completely clipped out by scissor box

    auto edge = [](const Pointf& a, const Pointf& b, float px, float py) {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        return dx * (py - a.y) - dy * (px - a.x);
    };

    float area = edge(prim.screenpos[0], prim.screenpos[1], prim.screenpos[2].x, prim.screenpos[2].y);
    if(abs(area) < 1e-5)
        return;

    bool cw = (area < 0);
    bool hastex = (prim.texture != nullptr);

    float uw0 = prim.texcoords[0].x * prim.inversew[0];
    float vw0 = prim.texcoords[0].y * prim.inversew[0];
    float uw1 = prim.texcoords[1].x * prim.inversew[1];
    float vw1 = prim.texcoords[1].y * prim.inversew[1];
    float uw2 = prim.texcoords[2].x * prim.inversew[2];
    float vw2 = prim.texcoords[2].y * prim.inversew[2];

    float w0_dx = prim.screenpos[1].y - prim.screenpos[2].y;
    float w0_dy = prim.screenpos[2].x - prim.screenpos[1].x;
    float w1_dx = prim.screenpos[2].y - prim.screenpos[0].y;
    float w1_dy = prim.screenpos[0].x - prim.screenpos[2].x;
    float w2_dx = prim.screenpos[0].y - prim.screenpos[1].y;
    float w2_dy = prim.screenpos[1].x - prim.screenpos[0].x;

    float diwp_dx = 0, diwp_dy = 0;
    float duwp_dx = 0, dvwp_dx = 0;
    float duwp_dy = 0, dvwp_dy = 0;

    if(hastex && mipmap && prim.mipmaps && prim.mipmaps->GetCount() > 0) {
        diwp_dx = (w0_dx * prim.inversew[0] + w1_dx * prim.inversew[1] + w2_dx * prim.inversew[2]) / area;
        diwp_dy = (w0_dy * prim.inversew[0] + w1_dy * prim.inversew[1] + w2_dy * prim.inversew[2]) / area;

        duwp_dx = (w0_dx * uw0 + w1_dx * uw1 + w2_dx * uw2) / area;
        dvwp_dx = (w0_dx * vw0 + w1_dx * vw1 + w2_dx * vw2) / area;

        duwp_dy = (w0_dy * uw0 + w1_dy * uw1 + w2_dy * uw2) / area;
        dvwp_dy = (w0_dy * vw0 + w1_dy * vw1 + w2_dy * vw2) / area;
    }

    Size texsz = Size(0, 0);
    const RGBA* tp = nullptr;
    const Image* curtex = prim.texture;
    if(hastex) {
        texsz = curtex->GetSize();
        tp = ~(*curtex);
    }

    float cr0 = prim.colors[0].r * prim.inversew[0];
    float cg0 = prim.colors[0].g * prim.inversew[0];
    float cb0 = prim.colors[0].b * prim.inversew[0];
    float ca0 = prim.colors[0].a * prim.inversew[0];

    float cr1 = prim.colors[1].r * prim.inversew[1];
    float cg1 = prim.colors[1].g * prim.inversew[1];
    float cb1 = prim.colors[1].b * prim.inversew[1];
    float ca1 = prim.colors[1].a * prim.inversew[1];

    float cr2 = prim.colors[2].r * prim.inversew[2];
    float cg2 = prim.colors[2].g * prim.inversew[2];
    float cb2 = prim.colors[2].b * prim.inversew[2];
    float ca2 = prim.colors[2].a * prim.inversew[2];

    float xw0 = prim.viewpos[0].x * prim.inversew[0];
    float yw0 = prim.viewpos[0].y * prim.inversew[0];
    float zw0 = prim.viewpos[0].z * prim.inversew[0];

    float xw1 = prim.viewpos[1].x * prim.inversew[1];
    float yw1 = prim.viewpos[1].y * prim.inversew[1];
    float zw1 = prim.viewpos[1].z * prim.inversew[1];

    float xw2 = prim.viewpos[2].x * prim.inversew[2];
    float yw2 = prim.viewpos[2].y * prim.inversew[2];
    float zw2 = prim.viewpos[2].z * prim.inversew[2];

    float nx0 = prim.normals[0].x * prim.inversew[0];
    float ny0 = prim.normals[0].y * prim.inversew[0];
    float nz0 = prim.normals[0].z * prim.inversew[0];

    float nx1 = prim.normals[1].x * prim.inversew[1];
    float ny1 = prim.normals[1].y * prim.inversew[1];
    float nz1 = prim.normals[1].z * prim.inversew[1];

    float nx2 = prim.normals[2].x * prim.inversew[2];
    float ny2 = prim.normals[2].y * prim.inversew[2];
    float nz2 = prim.normals[2].z * prim.inversew[2];

    for(int y = miny; y <= maxy; y++) {
        RGBA* cb = colorbuffer[y];
        float* zb = &zbuffer[y * size.cx];

        for(int x = minx; x <= maxx; x++) {
            float xp = x + 0.5;
            float yp = y + 0.5;

            float w0 = edge(prim.screenpos[1], prim.screenpos[2], xp, yp);
            float w1 = edge(prim.screenpos[2], prim.screenpos[0], xp, yp);
            float w2 = edge(prim.screenpos[0], prim.screenpos[1], xp, yp);

            if(cw) {
                if(w0 > 0 || w1 > 0 || w2 > 0)
                    continue;
            }
            else {
                if(w0 < 0 || w1 < 0 || w2 < 0)
                    continue;
            }

            w0 = w0 / area;
            w1 = w1 / area;
            w2 = w2 / area;

            float iwp = w0 * prim.inversew[0] + w1 * prim.inversew[1] + w2 * prim.inversew[2];
            float zp = (w0 * zw0 + w1 * zw1 + w2 * zw2) / iwp;

            if(zp < zb[x]) {
                float vr = (w0 * cr0 + w1 * cr1 + w2 * cr2) / iwp;
                float vg = (w0 * cg0 + w1 * cg1 + w2 * cg2) / iwp;
                float vb = (w0 * cb0 + w1 * cb1 + w2 * cb2) / iwp;
                float va = (w0 * ca0 + w1 * ca1 + w2 * ca2) / iwp;

                int sr = 0, sg = 0, sb = 0, sa = clamp((int) va, 0, 255);

                RGBA vcol;
                vcol.r = clamp((int)vr, 0, 255);
                vcol.g = clamp((int)vg, 0, 255);
                vcol.b = clamp((int)vb, 0, 255);
                vcol.a = sa;

                float u = 0, v = 0;

                // Normal is needed by built-in lighting AND by either hook, so
                // interpolate/normalize it once here instead of duplicating the
                // work per consumer. Same reasoning for the fragment's view-space
                // position (fp), only ever needed once a hook is installed.
                bool wantnormal = prim.shading || pixelshader || texturesampler;
                Point3D np;
                if(wantnormal) {
                    float nx = (w0 * nx0 + w1 * nx1 + w2 * nx2) / iwp;
                    float ny = (w0 * ny0 + w1 * ny1 + w2 * ny2) / iwp;
                    float nz = (w0 * nz0 + w1 * nz1 + w2 * nz2) / iwp;

                    if(float nlen = sqrt(nx*nx + ny*ny + nz*nz); nlen > 1e-5) {
                        nx /= nlen;
                        ny /= nlen;
                        nz /= nlen;
                    }
                    np = Point3D(nx, ny, nz);
                }

                Point3D fp;
                if(texturesampler || pixelshader) {
                    float fx = (w0 * xw0 + w1 * xw1 + w2 * xw2) / iwp;
                    float fy = (w0 * yw0 + w1 * yw1 + w2 * yw2) / iwp;
                    fp = Point3D(fx, fy, zp);
                }

                if(hastex) {
                    u = w0 * uw0 + w1 * uw1 + w2 * uw2;
                    v = w0 * vw0 + w1 * vw1 + w2 * vw2;

                    u = u / iwp;
                    v = v / iwp;

                    if(mipmap && prim.mipmaps && prim.mipmaps->GetCount() > 0) {
                        float dudx = (duwp_dx - u * diwp_dx) / iwp;
                        float dvdx = (dvwp_dx - v * diwp_dx) / iwp;
                        float dudy = (duwp_dy - u * diwp_dy) / iwp;
                        float dvdy = (dvwp_dy - v * diwp_dy) / iwp;

                        Size basesz = (*prim.mipmaps)[0].GetSize();
                        float px_u = dudx * basesz.cx;
                        float px_v = dvdx * basesz.cy;
                        float py_u = dudy * basesz.cx;
                        float py_v = dvdy * basesz.cy;

                        float lsq_x = px_u * px_u + px_v * px_v;
                        float lsq_y = py_u * py_u + py_v * py_v;

                        float L = max(lsq_x, lsq_y);
                        int miplevel = 0;
                        if(L > 1.0f) {
                            miplevel = (int)(log2(L) * 0.5f);
                        }
                        miplevel = clamp(miplevel, 0, prim.mipmaps->GetCount() - 1);
                        curtex = &(*prim.mipmaps)[miplevel];
                        texsz = curtex->GetSize();
                        tp = ~(*curtex);
                    }

                    u = u - floor(u);
                    v = v - floor(v);

                    RGBA tc;
                    if(texturesampler) {
                        Fragment fr;
                        fr.viewpos  = fp;
                        fr.normal   = np;
                        fr.texcoord = Pointf(u, v);
                        fr.color    = vcol;
                        fr.texture  = curtex;
                        fr.mipmaps  = prim.mipmaps;
                        tc = texturesampler(fr);
                    }
                    else
                    if(bilinear) {
                        float u_px = u * texsz.cx - 0.5;
                        float v_px = (1.0 - v) * texsz.cy - 0.5;

                        int xt0 = (int)floor(u_px);
                        int yt0 = (int)floor(v_px);

                        float fx = u_px - (float)xt0;
                        float fy = v_px - (float)yt0;

                        int xt1 = clamp(xt0 + 1, 0, texsz.cx - 1);
                        int yt1 = clamp(yt0 + 1, 0, texsz.cy - 1);
                        xt0 = clamp(xt0, 0, texsz.cx - 1);
                        yt0 = clamp(yt0, 0, texsz.cy - 1);

                        RGBA c00 = tp[yt0 * texsz.cx + xt0];
                        RGBA c10 = tp[yt0 * texsz.cx + xt1];
                        RGBA c01 = tp[yt1 * texsz.cx + xt0];
                        RGBA c11 = tp[yt1 * texsz.cx + xt1];

                        auto blnd = [&](int t00, int t10, int t01, int t11) {
                            float c0 = t00 * (1.0 - fx) + t10 * fx;
                            float c1 = t01 * (1.0 - fx) + t11 * fx;
                            return (int)(c0 * (1.0 - fy) + c1 * fy);
                        };

                        tc.r = blnd(c00.r, c10.r, c01.r, c11.r);
                        tc.g = blnd(c00.g, c10.g, c01.g, c11.g);
                        tc.b = blnd(c00.b, c10.b, c01.b, c11.b);
                        tc.a = blnd(c00.a, c10.a, c01.a, c11.a);
                    }
                    else {
                        int tx = (int)(u * texsz.cx);
                        tx = clamp(tx, 0, texsz.cx - 1);

                        int ty = (int)((1.0 - v) * texsz.cy);
                        ty = clamp(ty, 0, texsz.cy - 1);

                        tc = tp[ty * texsz.cx + tx];
                    }

                    vr = (tc.r * int(vr)) >> 8;
                    vg = (tc.g * int(vg)) >> 8;
                    vb = (tc.b * int(vb)) >> 8;
                    sa = tc.a;
                }

                if(pixelshader) {
                    Fragment fr;
                    fr.viewpos  = fp;
                    fr.normal   = np;
                    fr.texcoord = Pointf(u, v);
                    fr.color    = vcol;
                    fr.texture  = curtex;
                    fr.mipmaps  = prim.mipmaps;

                    RGBA base;
                    base.r = clamp((int) vr, 0, 255);
                    base.g = clamp((int) vg, 0, 255);
                    base.b = clamp((int) vb, 0, 255);
                    base.a = sa;

                    RGBA shaded = pixelshader(fr, base);

                    sr = shaded.r;
                    sg = shaded.g;
                    sb = shaded.b;
                    sa = shaded.a;
                }
                else
                if(prim.shading) {
                    int lr = 0, lg = 0, lb = 0;
                    if(lights.IsEmpty()) {
                        lr = vr; lg = vg; lb = vb;
                    }
                    else {
                        lr = int(vr * ambientlight.r / 255.0);
                        lg = int(vg * ambientlight.g / 255.0);
                        lb = int(vb * ambientlight.b / 255.0);

                        for(int j = 0; j < lights.GetCount(); j++) {
                            const LightSource& ls = lights[j];
                            float intensity = abs(DotProduct(ls.dir, np));

                            lr += int(vr * ls.color.r * intensity / 255.0);
                            lg += int(vg * ls.color.g * intensity / 255.0);
                            lb += int(vb * ls.color.b * intensity / 255.0);
                        }
                    }

                    sr = min(255, lr);
                    sg = min(255, lg);
                    sb = min(255, lb);
                }
                else {
                    sr = min(255, max(0, int(vr)));
                    sg = min(255, max(0, int(vg)));
                    sb = min(255, max(0, int(vb)));
                }

                if(prim.translucent) {
                    if(sa > 0) {
                        int inva = 255 - sa;
                        cb[x].r = (sr * sa + cb[x].r * inva) / 255;
                        cb[x].g = (sg * sa + cb[x].g * inva) / 255;
                        cb[x].b = (sb * sa + cb[x].b * inva) / 255;
                    }
                }
                else {
                    zb[x] = zp;
                    cb[x].r = sr;
                    cb[x].g = sg;
                    cb[x].b = sb;
                    cb[x].a = 255;
                }
            }
        }
    }
}

void Sculptor::RasterizeLine(Size size, const Pointf& p0, float z0, const Pointf& p1, float z1, RGBA c)
{
    float fx0 = p0.x, fy0 = p0.y, fz0 = z0;
    float fx1 = p1.x, fy1 = p1.y, fz1 = z1;

    auto ComputeOutCode = [&](float x, float y) -> int {
        int code = 0;
        float xmin = 0, xmax = size.cx - 1;
        float ymin = 0, ymax = size.cy - 1;

        if(!IsNull(scissor)) {
            xmin = max(0.0f, (float) scissor.left);
            xmax = min((float) size.cx - 1, (float)(scissor.right - 1));
            ymin = max(0.0f, (float) scissor.top);
            ymax = min((float) size.cy - 1, (float)(scissor.bottom - 1));
        }

        if(x < xmin)
            code |= 1;
        else
        if(x > xmax)
            code |= 2;
        if(y < ymin)
            code |= 4;
        else
        if(y > ymax)
            code |= 8;
        
        return code;
    };

    int out0 = ComputeOutCode(fx0, fy0);
    int out1 = ComputeOutCode(fx1, fy1);
    bool accept = false;

    while(true) {
        if(!(out0 | out1)) {
            accept = true;
            break;
        }
        else
        if(out0 & out1) {
            break;
        }
        else {
            float x, y, z;
            int out = out0 ? out0 : out1;
            float t = 0;

            if(out & 8) {
                t = (size.cy - 1 - fy0) / (fy1 - fy0);
                x = fx0 + (fx1 - fx0) * t;
                y = size.cy - 1;
            }
            else
            if(out & 4) {
                t = (0 - fy0) / (fy1 - fy0);
                x = fx0 + (fx1 - fx0) * t;
                y = 0;
            }
            else
            if(out & 2) {
                t = (size.cx - 1 - fx0) / (fx1 - fx0);
                y = fy0 + (fy1 - fy0) * t;
                x = size.cx - 1;
            }
            else
            if(out & 1) {
                t = (0 - fx0) / (fx1 - fx0);
                y = fy0 + (fy1 - fy0) * t;
                x = 0;
            }
            z = fz0 + (fz1 - fz0) * t;

            if(out == out0) {
                fx0 = x; fy0 = y; fz0 = z;
                out0 = ComputeOutCode(fx0, fy0);
            }
            else {
                fx1 = x; fy1 = y; fz1 = z;
                out1 = ComputeOutCode(fx1, fy1);
            }
        }
    }

    if(!accept)
        return;

    int x0 = (int) fx0;
    int y0 = (int) fy0;
    int x1 = (int) fx1;
    int y1 = (int) fy1;

    int dx = abs(x1 - x0);
    int sx = -1;
    if(x0 < x1)
        sx = 1;

    int dy = -abs(y1 - y0);
    int sy = -1;
    if(y0 < y1)
        sy = 1;

    int err = dx + dy;
    int e2 = 0;

    float dist = max(dx, -dy);
    float dz = 0;

    if(dist > 0)
        dz = (fz1 - fz0) / dist;

    float z = fz0;

    RGBA* cb = ~colorbuffer;
    float* zb = ~zbuffer;

    while(true) {
        if(x0 >= 0 && x0 < size.cx && y0 >= 0 && y0 < size.cy) {
            int idx = y0 * size.cx + x0;
            if(z < zb[idx]) {
                zb[idx] = z;
                cb[idx] = c;
            }
        }

        if(x0 == x1 && y0 == y1)
            break;

        e2 = 2 * err;

        if(e2 >= dy) {
            err = err + dy;
            x0 = x0 + sx;
        }

        if(e2 <= dx) {
            err = err + dx;
            y0 = y0 + sy;
        }

        z = z + dz;
    }
}

}