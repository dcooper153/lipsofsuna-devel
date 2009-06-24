/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * \addtogroup liimg Image
 * @{
 * \addtogroup liimgDDS DDS
 * @{
 */

#ifndef __IMAGE_DDS_H__
#define __IMAGE_DDS_H__

#include <stdio.h>
#include <system/lips-system.h>
#include <video/lips-video.h>
#define DDS_HEADER_MAGIC 0x20534444
#define DDS_FLAG_CAPS 0x00000001
#define DDS_FLAG_HEIGHT 0x00000002
#define DDS_FLAG_WIDTH 0x00000004
#define DDS_FLAG_PITCH 0x00000008
#define DDS_FLAG_PIXELFORMAT 0x00001000
#define DDS_FLAG_MIPMAPCOUNT 0x00020000
#define DDS_FLAG_LINEARSIZE 0x00080000
#define DDS_FLAG_DEPTH 0x00800000
#define DDS_FORMAT_ALPHAPIXELS 0x00000001
#define DDS_FORMAT_FOURCC 0x00000004
#define DDS_FORMAT_RGB 0x00000040
#define DDS_COMPRESS_DXT1 0x31545844
#define DDS_COMPRESS_DXT3 0x33545844
#define DDS_COMPRESS_DXT5 0x35545844
#define DDS_CAPS_COMPLEX 0x00000008
#define DDS_CAPS_TEXTURE 0x00001000
#define DDS_CAPS_MIPMAP 0x00400000

typedef struct _liimgDDSFormat liimgDDSFormat;
struct _liimgDDSFormat
{
	int compressed;
	int bpp;
	GLenum internal;
	GLenum format;
	GLenum type;
};

typedef struct _liimgDDSLevel liimgDDSLevel;
struct _liimgDDSLevel
{
	int width;
	int height;
	int size;
	void* data;
};

typedef struct _liimgDDS liimgDDS;
struct _liimgDDS
{
	/* Loaded from file. */
	struct
	{
		uint32_t magic;
		uint32_t size;
		uint32_t flags;
		uint32_t height;
		uint32_t width;
		uint32_t pitch;
		uint32_t depth;
		uint32_t mipmaps;
		uint32_t reserved[11];
	} header;
	struct
	{
		uint32_t size;
		uint32_t flags;
		uint32_t compress;
		uint32_t rgbbits;
		uint32_t rmask;
		uint32_t gmask;
		uint32_t bmask;
		uint32_t amask;
	} format;
	struct
	{
		uint32_t caps1;
		uint32_t caps2;
		uint32_t ddsx;
		uint32_t reserved[2];
	} caps;

	/* Derived from loaded data. */
	liimgDDSFormat info;
};

/**
 * \brief Initializes the DDS header for an RGBA image.
 *
 * \param self DDS.
 * \param width Image width.
 * \param height Image height.
 * \param mipmaps Mipmap count.
 */
static inline void
liimg_dds_init_rgba (liimgDDS* self,
                     int       width,
                     int       height,
                     int       mipmaps)
{
	memset (self, 0, sizeof (liimgDDS));
	self->header.magic = DDS_HEADER_MAGIC;
	self->header.size = 124;
	self->header.flags = DDS_FLAG_CAPS | DDS_FLAG_HEIGHT | DDS_FLAG_WIDTH | DDS_FLAG_PIXELFORMAT | DDS_FLAG_PITCH;
	self->header.height = height;
	self->header.width = width;
	self->header.pitch = 4 * width;
	self->header.mipmaps = mipmaps;
	self->format.size = 32;
	self->format.flags = DDS_FORMAT_RGB | DDS_FORMAT_ALPHAPIXELS;
	self->format.rgbbits = 32;
	self->format.rmask = 0xFF000000;
	self->format.gmask = 0x00FF0000;
	self->format.bmask = 0x0000FF00;
	self->format.amask = 0x000000FF;
	self->caps.caps1 = DDS_CAPS_TEXTURE;
	if (mipmaps)
	{
		self->header.flags |= DDS_FLAG_MIPMAPCOUNT;
		self->caps.caps1 |= DDS_CAPS_COMPLEX | DDS_CAPS_MIPMAP;
	}
}

/**
 * \brief Initializes the DDS header for an S3TC image.
 *
 * \param self DDS.
 * \param width Image width.
 * \param height Image height.
 * \param size Size of first mipmap level in bytes.
 * \param mipmaps Mipmap count.
 */
static inline void
liimg_dds_init_s3tc (liimgDDS* self,
                     int       width,
                     int       height,
                     int       size,
                     int       mipmaps)
{
	memset (self, 0, sizeof (liimgDDS));
	self->header.magic = DDS_HEADER_MAGIC;
	self->header.size = 124;
	self->header.flags = DDS_FLAG_CAPS | DDS_FLAG_HEIGHT | DDS_FLAG_WIDTH | DDS_FLAG_PIXELFORMAT | DDS_FLAG_LINEARSIZE;
	self->header.height = height;
	self->header.width = width;
	self->header.pitch = size;
	self->header.mipmaps = mipmaps;
	self->format.size = 32;
	self->format.flags = DDS_FORMAT_FOURCC;
	self->format.compress = DDS_COMPRESS_DXT5;
	self->caps.caps1 = DDS_CAPS_TEXTURE;
	if (mipmaps)
	{
		self->header.flags |= DDS_FLAG_MIPMAPCOUNT;
		self->caps.caps1 |= DDS_CAPS_COMPLEX | DDS_CAPS_MIPMAP;
	}
}

/**
 * \brief Used internally for swapping header byte order when necessary.
 *
 * \param self DDS.
 */
static inline void
liimg_dds_byteswap (liimgDDS* self)
{
#if LI_BYTE_ORDER == LI_BIG_ENDIAN
	#define BYTESWAP(i) ((((i)&&0xFF)<<24) | (((i)&&0xFF00)<<8) | (((i)&&0xFF0000)>>8) | (((i)&&0xFF000000)>>24))
	self->header.magic = BYTESWAP (self->header.magic);
	self->header.flags = BYTESWAP (self->header.flags);
	self->header.height = BYTESWAP (self->header.height);
	self->header.width = BYTESWAP (self->header.width);
	self->header.pitch = BYTESWAP (self->header.pitch);
	self->header.depth = BYTESWAP (self->header.depth);
	self->header.mipmaps = BYTESWAP (self->header.mipmaps);
	self->format.size = BYTESWAP (self->format.size);
	self->format.flags = BYTESWAP (self->format.flags);
	self->format.compress = BYTESWAP (self->format.compress);
	self->format.rgbbits = BYTESWAP (self->format.rgbbits);
	self->format.rmask = BYTESWAP (self->format.rmask);
	self->format.gmask = BYTESWAP (self->format.gmask);
	self->format.bmask = BYTESWAP (self->format.bmask);
	self->format.amask = BYTESWAP (self->format.amask);
	self->caps.caps1 = BYTESWAP (self->caps.caps1);
	self->caps.caps2 = BYTESWAP (self->caps.caps2);
	self->caps.ddsx = BYTESWAP (self->caps.ddsx);
	#undef BYTESWAP
#endif
}

/**
 * \brief Reads the DDS file header.
 *
 * \param self DDS.
 * \param file File.
 * \return Nonzero on success.
 */
static inline int
liimg_dds_read_header (liimgDDS* self,
                       FILE*     file)
{
	int i;
	int alpha;
	static const struct
	{
		int alpha;
		uint32_t rgbbits;
		uint32_t rmask;
		uint32_t gmask;
		uint32_t bmask;
		uint32_t amask;
		liimgDDSFormat format;
	}
	formats[] =
	{
		{ 1, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, { 0, 4, 4, GL_RGBA, GL_UNSIGNED_BYTE } },
		{ 0, 24, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000, { 0, 3, 3, GL_RGB, GL_UNSIGNED_BYTE } },
		{ 1, 32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF, { 0, 4, 4, GL_BGRA, GL_UNSIGNED_BYTE } },
		{ 0, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000, { 0, 3, 3, GL_BGR, GL_UNSIGNED_BYTE } },
		{ 1, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, { 0, 4, 4, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV } },
		{ 1, 16, 0xF000, 0x0F00, 0x00F0, 0x000F, { 0, 2, 2, GL_RGBA16, GL_UNSIGNED_SHORT_4_4_4_4 } },
		{ 1, 16, 0x00F0, 0x0F00, 0xF000, 0x000F, { 0, 2, 2, GL_RGBA16, GL_UNSIGNED_SHORT_4_4_4_4_REV } },
		{ 0, 16, 0xF800, 0x07E0, 0x001F, 0x0000, { 0, 2, 2, GL_RGB16, GL_UNSIGNED_SHORT_5_6_5 } },
		{ 0, 16, 0x001F, 0x07E0, 0xF800, 0x0000, { 0, 2, 2, GL_RGB16, GL_UNSIGNED_SHORT_5_6_5_REV } }
	};

	/* Read header. */
	if (!fread (&self->header.magic, 4, 1, file) ||
	    !fread (&self->header.size, 4, 1, file) ||
	    !fread (&self->header.flags, 4, 1, file) ||
	    !fread (&self->header.height, 4, 1, file) ||
	    !fread (&self->header.width, 4, 1, file) ||
	    !fread (&self->header.pitch, 4, 1, file) ||
	    !fread (&self->header.depth, 4, 1, file) ||
	    !fread (&self->header.mipmaps, 4, 1, file) ||
	    !fread (self->header.reserved, 44, 1, file) ||
	    !fread (&self->format.size, 4, 1, file) ||
	    !fread (&self->format.flags, 4, 1, file) ||
	    !fread (&self->format.compress, 4, 1, file) ||
	    !fread (&self->format.rgbbits, 4, 1, file) ||
	    !fread (&self->format.rmask, 4, 1, file) ||
	    !fread (&self->format.gmask, 4, 1, file) ||
	    !fread (&self->format.bmask, 4, 1, file) ||
	    !fread (&self->format.amask, 4, 1, file) ||
	    !fread (&self->caps.caps1, 4, 1, file) ||
	    !fread (&self->caps.caps2, 4, 1, file) ||
	    !fread (&self->caps.ddsx, 4, 1, file) ||
	    !fread (self->caps.reserved, 8, 1, file))
		return 0;
	liimg_dds_byteswap (self);
	if (self->header.magic != DDS_HEADER_MAGIC)
		return 0;

	/* Detect pixel format. */
	if (self->format.flags & DDS_FORMAT_FOURCC)
	{
		switch (self->format.compress)
		{
			case DDS_COMPRESS_DXT1:
				self->info.compressed = 1;
				self->info.internal = 3;
				self->info.format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				return 1;
			case DDS_COMPRESS_DXT3:
				self->info.compressed = 1;
				self->info.internal = 4;
				self->info.format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				return 1;
			case DDS_COMPRESS_DXT5:
				self->info.compressed = 1;
				self->info.internal = 4;
				self->info.format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				return 1;
		}
	}
	else
	{
		if (!(self->format.flags & DDS_FORMAT_RGB))
			return 0;
		alpha = (self->format.flags & DDS_FORMAT_ALPHAPIXELS);
		for (i = 0 ; i < (int)(sizeof (formats) / sizeof (*formats)) ; i++)
		{
			if (formats[i].alpha == alpha &&
			    formats[i].rgbbits == self->format.rgbbits &&
			    formats[i].rmask == self->format.rmask &&
			    formats[i].gmask == self->format.gmask &&
			    formats[i].bmask == self->format.bmask &&
			   (formats[i].amask == self->format.amask || !alpha))
			{
				self->info = formats[i].format;
				return 1;
			}
		}
	}

	printf ("UNSUPPORTED FORMAT %d %X %X %X %X %X\n",
		alpha,
		self->format.rgbbits,
		self->format.rmask,
		self->format.gmask,
		self->format.bmask,
		self->format.amask);
	return 0;
}

/**
 * \brief Reads a mipmap level from the DDS file.
 *
 * \param self DDS.
 * \param file File.
 * \param level Mipmap level.
 * \param data Return location for pixel data buffer.
 * \param size Return location for pixel data buffer size.
 * \return Nonzero on success.
 */
static inline int
liimg_dds_read_level (liimgDDS*      self,
                      FILE*          file,
                      int            level,
                      liimgDDSLevel* result)
{
	int mult;

	/* Get buffer size. */
	result->width = self->header.width >> level;
	result->width = result->width? result->width : 1;
	result->height = self->header.height >> level;
	result->height = result->height? result->height : 1;
	if (self->format.flags & DDS_FORMAT_FOURCC)
	{
		switch (self->format.compress)
		{
			case DDS_COMPRESS_DXT1: mult = 8; break;
			case DDS_COMPRESS_DXT3: mult = 16; break;
			case DDS_COMPRESS_DXT5: mult = 16; break;
			default: return 0;
		}
		result->size = ((result->width + 3) / 4) * ((result->height + 3) / 4) * mult;
	}
	else
		result->size = self->info.bpp * result->width * result->height;

	/* Load pixel data. */
	result->data = malloc (result->size);
	if (result->data == NULL)
		return 0;
	if (!fread (result->data, result->size, 1, file))
	{
		free (result->data);
		return 0;
	}

	return 1;
}

/**
 * \brief Writes the DDS file header.
 *
 * \param self DDS.
 * \param file File.
 * \return Nonzero on success.
 */
static inline int
liimg_dds_write_header (liimgDDS* self,
                        FILE*     file)
{
	liimg_dds_byteswap (self);
	if (!fwrite (&self->header.magic, 4, 1, file) ||
	    !fwrite (&self->header.size, 4, 1, file) ||
	    !fwrite (&self->header.flags, 4, 1, file) ||
	    !fwrite (&self->header.height, 4, 1, file) ||
	    !fwrite (&self->header.width, 4, 1, file) ||
	    !fwrite (&self->header.pitch, 4, 1, file) ||
	    !fwrite (&self->header.depth, 4, 1, file) ||
	    !fwrite (&self->header.mipmaps, 4, 1, file) ||
	    !fwrite (self->header.reserved, 44, 1, file) ||
	    !fwrite (&self->format.size, 4, 1, file) ||
	    !fwrite (&self->format.flags, 4, 1, file) ||
	    !fwrite (&self->format.compress, 4, 1, file) ||
	    !fwrite (&self->format.rgbbits, 4, 1, file) ||
	    !fwrite (&self->format.rmask, 4, 1, file) ||
	    !fwrite (&self->format.gmask, 4, 1, file) ||
	    !fwrite (&self->format.bmask, 4, 1, file) ||
	    !fwrite (&self->format.amask, 4, 1, file) ||
	    !fwrite (&self->caps.caps1, 4, 1, file) ||
	    !fwrite (&self->caps.caps2, 4, 1, file) ||
	    !fwrite (&self->caps.ddsx, 4, 1, file) ||
	    !fwrite (self->caps.reserved, 8, 1, file))
		return 0;

	return 1;
}

/**
 * \brief Writes a mipmap level to the DDS file.
 *
 * \param self DDS.
 * \param file File.
 * \param level Mipmap level.
 * \param data Pixel data.
 * \param size Pixel data size in bytes.
 * \return Nonzero on success.
 */
static inline int
liimg_dds_write_level (liimgDDS* self,
                       FILE*     file,
                       int       level,
                       void*     data,
                       int       size)
{
	if (!fwrite (data, size, 1, file))
		return 0;

	return 1;
}

/*****************************************************************************/

/**
 * \brief Loads a DDS texture file.
 *
 * \param file File pointer.
 * \param info Return location for DDS info or NULL.
 * \return New texture number or zero.
 */
static inline GLuint
liimg_dds_load_texture (FILE*     file,
                        liimgDDS* info)
{
	int i;
	int type;
	void* tmp;
	GLuint texture;
	liimgDDS dds;
	liimgDDSFormat fmt;
	liimgDDSLevel lvl;

	/* Read header. */
	if (!liimg_dds_read_header (&dds, file))
		return 0;
	fmt = dds.info;
	if (info != NULL)
		*info = dds;

	/* Load image. */
	glGenTextures (1, &texture);
	glBindTexture (GL_TEXTURE_2D, texture);
	if (!dds.header.mipmaps)
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (!fmt.compressed)
	{
		/* Uncompressed format. */
		for (i = 0 ; !i || i < (int) dds.header.mipmaps ; i++)
		{
			if (!liimg_dds_read_level (&dds, file, i, &lvl))
			{
				glDeleteTextures (1, &texture);
				return 0;
			}
			glTexImage2D (GL_TEXTURE_2D, i, fmt.internal, lvl.width,
				lvl.height, 0, fmt.format, fmt.type, lvl.data);
			free (lvl.data);
		}
	}
	else if (GLEW_EXT_texture_compression_s3tc)
	{
		/* Hardware uncompression. */
		for (i = 0 ; !i || i < (int) dds.header.mipmaps ; i++)
		{
			if (!liimg_dds_read_level (&dds, file, i, &lvl))
			{
				glDeleteTextures (1, &texture);
				return 0;
			}
			glCompressedTexImage2DARB (GL_TEXTURE_2D, i, fmt.format,
				lvl.width, lvl.height, 0, lvl.size, lvl.data);
			free (lvl.data);
		}
	}
	else
	{
		/* Software uncompression. */
		for (i = 0 ; !i || i < (int) dds.header.mipmaps ; i++)
		{
			if (!liimg_dds_read_level (&dds, file, i, &lvl))
			{
				glDeleteTextures (1, &texture);
				return 0;
			}
			switch (fmt.format)
			{
				case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: type = 1; break;
				case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT: type = 3; break;
				case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: type = 5; break;
				default:
					type = 0;
					break;
			}
			if (type)
			{
				tmp = malloc (4 * lvl.width * lvl.height);
				if (tmp != NULL)
				{
					liimg_compress_uncompress (lvl.data, lvl.width, lvl.height, type, tmp);
					glTexImage2D (GL_TEXTURE_2D, i, 4, lvl.width, lvl.height,
						0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
					free (tmp);
				}
			}
			free (lvl.data);
		}
	}

	return texture;
}

#endif

/** @} */
/** @} */
