layout(local_size_x = 1) in;

void main() {
	imageStore(computeImage, int(gl_WorkGroupID.x),
			   vec4(float(gl_WorkGroupID.x) / float(gl_NumWorkGroups.x)));
}
