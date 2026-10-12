#version 450

vec4 triangleVerticesPosition[3] = {
vec4(0.0, 0.5, 0.0, 1.0),
vec4(-0.5,-0.5, 0.0, 1.0),
vec4(0.5, -0.5, 0.0, 1.0)};

void main(){
    gl_Position = triangleVerticesPosition[gl_VertexIndex];
}