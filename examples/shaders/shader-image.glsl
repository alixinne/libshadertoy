void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 cd = fragCoord / iResolution.xy * 7.;
	int id = int(mod(floor(cd.y + .5 / iResolution.xy), 7.));
	if (id == 0)
		fragColor = texture(iChannel0, cd);
	else if (id == 1)
		fragColor = texture(iChannel1, cd);
	else if (id == 2)
		fragColor = texture(iChannel2, cd);
	else if (id == 3)
		fragColor = texture(noiseChannel, cd);
	else if (id == 4)
		fragColor = texture(checkerChannel, cd);
	else if (id == 5)
		fragColor = texture(errorChannel, cd);
	else if (id == 6)
		fragColor = texture(bufferChannel, cd);
}
