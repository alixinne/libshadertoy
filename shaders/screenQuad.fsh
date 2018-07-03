#version 440

precision highp float;
precision highp int;
precision highp sampler2D;

// Screen texture coordinate
in vec2 vtexCoord;
// Output color
out vec4 fragColor;

// Screen texture sampler
layout(binding = 0) uniform sampler2D screenTexture;

void main(void) {
	fragColor = texture2D(screenTexture, vtexCoord);
}
