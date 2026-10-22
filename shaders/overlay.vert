#version 430 core
layout(location=0) in vec2 aPos;
uniform int mode; // 0 = background quad, 1 = text triangles
void main() { gl_Position = vec4(aPos, 0.0, 1.0); }
