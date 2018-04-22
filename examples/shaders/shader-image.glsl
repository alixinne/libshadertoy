void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 cd = (fragCoord + .5) / (.2 * iResolution.xy);
	int id = int(mod(floor(cd.y), 4.));
	if (id == 0)
		fragColor = texture(iChannel0, cd);
	else if (id == 1)
		fragColor = texture(iChannel1, cd);
	else if (id == 2)
		fragColor = texture(iChannel2, cd);
	else
		fragColor = texture(noiseChannel, cd);
}
