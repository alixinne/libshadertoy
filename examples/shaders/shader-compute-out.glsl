void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	fragColor = vec4(vec3(texture(computeImage, uv.x).r), 1.0);
}
