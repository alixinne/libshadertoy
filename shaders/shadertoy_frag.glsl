#pragma shadertoy part glsl:header begin
#version 440
#pragma shadertoy part end

precision highp float;
precision highp int;
precision highp sampler2D;

in vec2 vtexCoord;
out vec4 fragColor;

#pragma shadertoy part *:defines

#pragma shadertoy part *:uniforms

#pragma shadertoy part buffer:inputs
#pragma shadertoy part buffer:sources

void main(void) {
	fragColor = vec4(0., 0., 0., 1.);
	mainImage(fragColor, vtexCoord.xy * iResolution.xy);
}
