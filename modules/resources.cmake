# Build resource files from shaders, cf. http://stackoverflow.com/a/27206982
# Creates C resources file from files in given directory
function(create_resources dir output output_h h_id)
	# Create empty output file
	file(WRITE ${output} "#include <stddef.h>\n#include \"${output_h}\"\n\n")
	file(WRITE ${output_h} "#ifndef ${h_id}\n#define ${h_id}\nnamespace shadertoy {\n")
	# Collect input files
	file(GLOB bins ${dir}/*)
	# Iterate through input files
	foreach(bin ${bins})
		# Get short filename
		string(REGEX MATCH "([^/]+)$" filename ${bin})
		# Replace filename spaces & extension separator for C compatibility
		string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
		# Read hex data from file
		file(READ ${bin} filedata HEX)
		# Convert hex data for C compatibility
		string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
		# Append data to output file
		file(APPEND ${output} "const char shadertoy::${filename}[] = {${filedata}};\nconst size_t shadertoy::${filename}_size = sizeof(shadertoy::${filename});\n")
		file(APPEND ${output_h} "extern const char ${filename}[];\nextern const size_t ${filename}_size;\n")
	endforeach()
	# Complete .h
	file(APPEND ${output} "\n")
	file(APPEND ${output_h} "\n}\n#endif /* ${h_id} */\n")
endfunction()

create_resources(${RESOURCES_INPUT} ${RESOURCES_OUTPUT}
				 ${RESOURCES_H_OUTPUT} ${RESOURCES_H_ID})
