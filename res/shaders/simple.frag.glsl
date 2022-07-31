#version 450

// Layout qualifier.
layout(location = 0) out vec4 outColor;

void main() {
  outColor = vec4(1.0f, 0.5f, 0.0f, 1.0f);
}
