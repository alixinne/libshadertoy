#version 440

precision highp float;
precision highp int;
precision highp sampler2D;

// Standard shadertoy uniforms
uniform vec3 iResolution;
uniform float iTime;
uniform float iTimeDelta;
uniform int iFrame;
uniform float iFrameRate;
uniform float iChannelTime[4];
uniform vec3 iChannelResolution[4];
uniform vec4 iMouse;
layout(binding = 0) uniform sampler2D iChannel0;
layout(binding = 1) uniform sampler2D iChannel1;
layout(binding = 2) uniform sampler2D iChannel2;
layout(binding = 3) uniform sampler2D iChannel3;
uniform vec4 iDate;
uniform float iSampleRate;

// Geometry uniforms
uniform mat4 eMVP;

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
