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