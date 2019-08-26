#version 300 es

precision highp float;
precision highp int;
precision highp sampler2D;

// Screen texture coordinate
in vec2 vtexCoord;
// Output color
layout(location = 0) out vec4 fragColor;

// Screen texture sampler
uniform sampler2D screenTexture;

void main(void) {
	fragColor = texture2D(screenTexture, vtexCoord);
}
