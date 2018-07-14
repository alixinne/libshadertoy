#version 440

precision highp float;
precision highp int;
precision highp sampler2D;

#pragma shadertoy part geometry:uniforms

// Vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

// Texture coord for fragment
out vec2 vtexCoord;

void main() {
	vtexCoord = texCoord;
	gl_Position = eMVP * vec4(position, 1.0);
}
