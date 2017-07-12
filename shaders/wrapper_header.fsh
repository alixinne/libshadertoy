#version 330

precision highp float;
precision highp int;
precision highp sampler2D;

// Input texture coordinate
in vec2 vtexCoord;
// Output fragment color
out vec4 fragColor;

// From ShaderToy documentation
uniform vec3 iResolution; // The viewport resolution (z is pixel aspect ratio, usually 1.0)
uniform float iGlobalTime; // Current time in seconds
uniform float iTimeDelta; // Time it takes to render a frame, in seconds
uniform int iFrame; // Current frame
uniform float iFrameRate; // Number of frames rendered per second
// uniform float iChannelTime[4]; // Time for channel (if video or sound), in seconds
uniform vec3 iChannelResolution[4]; // Input texture resolution for each channel
uniform vec4 iMouse; // xy = current pixel coords (if LMB is down). zw = click pixel
uniform sampler2D iChannel0; // Sampler for input texture 0
uniform sampler2D iChannel1; // Sampler for input texture 1
uniform sampler2D iChannel2; // Sampler for input texture 2
uniform sampler2D iChannel3; // Sampler for input texture 3
uniform vec4 iDate; // Year, month, day, time in seconds in .xyzw
// uniform float iSampleRate; // The sound sample rate
