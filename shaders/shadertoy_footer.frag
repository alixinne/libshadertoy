
// GLSL entry point
void main(void) {
	fragColor = vec4(0.,0.,0.,1.);
	mainImage(fragColor, vtexCoord.xy * iResolution.xy);
}
