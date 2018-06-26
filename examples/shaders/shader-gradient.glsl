void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	fragColor = vec4(uv.x+0.5*sin(iTime),uv.y,0.5+0.5*sin(iTime),1.0);
}
