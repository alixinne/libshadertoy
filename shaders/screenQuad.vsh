#version 420

precision highp float;
precision highp int;
precision highp sampler2D;

// Vertex position
in vec3 position;
// Vertex texture coordinate
in vec2 texCoord;
// Texture coord for fragment
out vec2 vtexCoord;

void main() {
	vtexCoord = texCoord;
	gl_Position = vec4(position, 1.0);
}
