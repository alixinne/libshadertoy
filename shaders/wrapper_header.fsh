#version 330

precision highp float;
precision highp int;
precision highp sampler2D;

// Input texture coordinate
in vec2 vtexCoord;
// Output fragment color
out vec4 fragColor;
// Compatibility with legacy iGlobalTime
#define iGlobalTime iTime
