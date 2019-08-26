#include "shadertoy/extensions.hpp"

#if SHADERTOY_HAS_OPENIMAGEIO
#include "shadertoy/backends/gx/backend.hpp"

#include <OpenImageIO/imageio.h>
using namespace OIIO;

#include "shadertoy/backends/gx/texture.hpp"

void st_oiio_load_image_to_texture(const std::string &filename, bool vflip, GLenum &output_format,
								   const shadertoy::backends::gx::texture &texture, int &width, int &height)
{
	auto image = ImageInput::open(filename);

	if (!image)
		throw std::runtime_error("Failed to load image data");

	// Get image details
	const auto &image_spec = image->spec();
	width = image_spec.width;
	height = image_spec.height;
	int channels = image_spec.nchannels;
	const auto &format = image_spec.format;

#define _TO_INTERNAL_FORMAT(suffix)    \
	(channels >= 4 ? GL_RGBA##suffix : \
					 (channels >= 3 ? GL_RGB##suffix : (channels >= 2 ? GL_RG##suffix : (GL_R##suffix))))
#define _TO_INPUT_FORMAT() \
	(channels >= 4 ? GL_RGBA : (channels >= 3 ? GL_RGB : (channels >= 2 ? GL_RG : (GL_RED))))

	GLenum internal_format = 0;
	GLenum input_format = _TO_INPUT_FORMAT();
	GLenum input_type = 0;
	TypeDesc decode_type;

	switch (format.basetype)
	{
	case TypeDesc::UINT8:
		internal_format = _TO_INTERNAL_FORMAT(8);
		input_type = GL_UNSIGNED_BYTE;
		decode_type = TypeDesc(TypeDesc::UINT8);
		break;
	case TypeDesc::INT8:
		internal_format = _TO_INTERNAL_FORMAT(8);
		input_type = GL_BYTE;
		decode_type = TypeDesc(TypeDesc::INT8);
		break;
	case TypeDesc::UINT16:
		internal_format = _TO_INTERNAL_FORMAT(16);
		input_type = GL_UNSIGNED_SHORT;
		decode_type = TypeDesc(TypeDesc::UINT16);
		break;
	case TypeDesc::INT16:
		internal_format = _TO_INTERNAL_FORMAT(16);
		input_type = GL_SHORT;
		decode_type = TypeDesc(TypeDesc::INT16);
		break;
	case TypeDesc::UINT32:
	case TypeDesc::UINT64:
		internal_format = _TO_INTERNAL_FORMAT(32UI);
		input_type = GL_UNSIGNED_INT;
		decode_type = TypeDesc(TypeDesc::UINT32);
		break;
	case TypeDesc::INT32:
	case TypeDesc::INT64:
		internal_format = _TO_INTERNAL_FORMAT(32I);
		input_type = GL_INT;
		decode_type = TypeDesc(TypeDesc::INT32);
		break;
	case TypeDesc::HALF:
		internal_format = _TO_INTERNAL_FORMAT(16F);
		input_type = GL_HALF_FLOAT;
		decode_type = TypeDesc(TypeDesc::HALF);
		break;
	case TypeDesc::FLOAT:
	case TypeDesc::DOUBLE:
		internal_format = _TO_INTERNAL_FORMAT(32F);
		input_type = GL_FLOAT;
		decode_type = TypeDesc(TypeDesc::FLOAT);
		break;
	default:
		throw std::runtime_error("Failed to find compatible format");
		break;
	}

	// Decoding buffer
	std::vector<uint8_t> buffer(width * height * channels * decode_type.size());

	// Decode image
	if (vflip)
	{
		size_t scanlinesize = width * channels * decode_type.size();
		image->read_image(decode_type, buffer.data() + (height - 1) * scanlinesize, AutoStride,
						  -scanlinesize, AutoStride);
	}
	else
	{
		image->read_image(decode_type, buffer.data());
	}

	// Load texture into GPU memory
	texture.image_2d(GL_TEXTURE_2D, 0, output_format = internal_format, width, height, 0,
					 input_format, input_type, buffer.data());
}

#endif /* SHADERTOY_HAS_OPENIMAGEIO */
