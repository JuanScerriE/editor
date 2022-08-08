#version 450

// Layout qualifier.
// layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
  mat2 transfrom;
  vec2 offset;
  vec3 color;
} push;

void main() {
  outColor = vec4(push.color, 1.0f);
}
