#pragma shadertoy part glsl:header begin
#version 440
#pragma shadertoy part end

precision highp float;
precision highp int;
precision highp sampler2D;

in vec2 vtexCoord;
layout(location = 0) out vec4 fragColor;

#pragma shadertoy part *:defines

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

#pragma shadertoy part buffer:inputs
#pragma shadertoy part buffer:sources

void main(void) {
	fragColor = vec4(0., 0., 0., 1.);
	mainImage(fragColor, vtexCoord.xy * iResolution.xy);
}
