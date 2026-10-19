#version 430 core
out vec4 FragColor;
uniform int mode; // 0 for dark background, 1 for white text
void main() {
    if (mode == 0) FragColor = vec4(0.0, 0.0, 0.0, 0.65);
    else           FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
