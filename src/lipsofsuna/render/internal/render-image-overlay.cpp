/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenImageOverlay ImageOverlay
 * @{
 */

#include "render-internal.h"
#include "render-image-overlay.hpp"

#define MAX_VERTEX_COUNT 1024

class LIRenTilePacker
{
public:
	LIRenTilePacker (void* ptr, float zv)
	{
		pos = 0;
		z = zv;
		verts = (float*) ptr;
		clip = NULL;
		rotation_angle = 0.0f;
		rotation_center[0] = 0.0f;
		rotation_center[1] = 0.0f;
	}
	void set_color (const float* color)
	{
		memcpy (this->color, color, 4 * sizeof (float));
	}
	void set_clipping (float* rect)
	{
		clip = rect;
	}
	void set_rotation (float angle, float x, float y, float aspect)
	{
		rotation_angle = angle;
		rotation_aspect = aspect;
		rotation_center[0] = x;
		rotation_center[1] = y;
	}
	void add_quad (float x0, float y0, float u0, float v0, float x1, float y1, float u1, float v1)
	{
		lisys_assert (x0 <= x1);
		lisys_assert (y1 <= y0);
		if (clip != NULL)
		{
			float uscale = (u1 - u0) / (x1 - x0);
			float vscale = (v1 - v0) / (y1 - y0);
			if (x1 < clip[0] || x0 >= clip[0] + clip[2] ||
			    y0 < clip[1] || y1 >= clip[1] + clip[3])
				return;
			clip_coord (x0, u0, uscale, clip[0], clip[0] + clip[2]);
			clip_coord (y0, v0, vscale, clip[1], clip[1] + clip[3]);
			clip_coord (x1, u1, uscale, clip[0], clip[0] + clip[2]);
			clip_coord (y1, v1, vscale, clip[1], clip[1] + clip[3]);
		}
		add_vertex (x0, y0, u0, v0);
		add_vertex (x1, y1, u1, v1);
		add_vertex (x1, y0, u1, v0);
		add_vertex (x0, y0, u0, v0);
		add_vertex (x0, y1, u0, v1);
		add_vertex (x1, y1, u1, v1);
	} 
protected:
	void add_vertex (float x, float y, float u, float v)
	{
		if (pos >= 6 * MAX_VERTEX_COUNT)
			return;
		if (rotation_angle != 0.0f)
		{
			float x1 = x - rotation_center[0];
			float y1 = (y - rotation_center[1]) * rotation_aspect;
			float x2 = x1 * cosf (rotation_angle) - y1 * sinf (rotation_angle);
			float y2 = x1 * sinf (rotation_angle) + y1 * cosf (rotation_angle);
			x = x2 + rotation_center[0];
			y = y2 / rotation_aspect + rotation_center[1];
		}
		verts[pos++] = x;
		verts[pos++] = y;
		verts[pos++] = z;
		verts[pos++] = u;
		verts[pos++] = v;
		uint8_t* c = (uint8_t*)(verts + pos++);
		c[0] = (uint8_t) 255 * color[0];
		c[1] = (uint8_t) 255 * color[1];
		c[2] = (uint8_t) 255 * color[2];
		c[3] = (uint8_t) 255 * color[3];
	}
	void clip_coord (float& coord, float& tex, float tex_scale, float min, float max)
	{
		float clip0 = min - coord;
		float clip1 = coord - max;
		if (clip0 > 0)
		{
			tex += tex_scale * clip0;
			coord += clip0;
		}
		if (clip1 > 0)
		{
			tex -= tex_scale * clip1;
			coord -= clip1;
		}
	}
public:
	int pos;
	float z;
	float color[4];
	float* clip;
	float* verts;
	float rotation_angle;
	float rotation_aspect;
	float rotation_center[2];
};

Ogre::String LIRenImageOverlay::type_name ("LIRenImageOverlay");

/*****************************************************************************/

LIRenImageOverlay::LIRenImageOverlay (const Ogre::String& name) : Ogre::OverlayElement (name)
{
}

LIRenImageOverlay::~LIRenImageOverlay ()
{
	delete render_op.vertexData;
}

void LIRenImageOverlay::set_color (const float* color)
{
	memcpy (this->color, color, 4 * sizeof (float));
}

void LIRenImageOverlay::set_clipping (const int* rect)
{
	if (rect != NULL)
	{
		memcpy (this->dst_clip, rect, 4 * sizeof (int));
		clipping = true;
	}
	else
		clipping = false;
}

void LIRenImageOverlay::set_rotation (
	float angle,
	float center_x,
	float center_y)
{
	rotation_angle = angle;
	rotation_center[0] = center_x;
	rotation_center[1] = center_y;
}

void LIRenImageOverlay::set_tiling (
	const int* source_position,
	const int* source_size,
	const int* source_tiling)
{
	memcpy (this->src_pos, source_position, 2 * sizeof (int));
	memcpy (this->src_size, source_size, 2 * sizeof (int));
	memcpy (this->src_tiling, source_tiling, 6 * sizeof (int));
	mGeomPositionsOutOfDate = true;
}

void LIRenImageOverlay::initialise ()
{
	if (mInitialised)
		return;
	mInitialised = true;

	/* Initialize the render operation. */
	render_op.vertexData = new Ogre::VertexData ();
	render_op.vertexData->vertexStart = 0;
	render_op.vertexData->vertexCount = 0;

	/* Setup the vertex format. */
	size_t offset = 0;
	Ogre::VertexDeclaration* format = render_op.vertexData->vertexDeclaration;
	format->addElement (0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	offset += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
	format->addElement (0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
	offset += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT2);
	format->addElement (0, offset, Ogre::VET_COLOUR_ABGR, Ogre::VES_DIFFUSE);
	offset += Ogre::VertexElement::getTypeSize (Ogre::VET_COLOUR_ABGR);

	/* Create the vertex buffer. */
	Ogre::HardwareVertexBufferSharedPtr vbuf =
		Ogre::HardwareBufferManager::getSingleton().createVertexBuffer (
		offset, MAX_VERTEX_COUNT, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	/* Bind the vertex buffer. */
	render_op.vertexData->vertexBufferBinding->setBinding (0, vbuf);
	render_op.useIndexes = false;
	render_op.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
}

const Ogre::String& LIRenImageOverlay::getTypeName () const
{
	return type_name;
}

void LIRenImageOverlay::getRenderOperation (Ogre::RenderOperation& op)
{
	op = render_op;
}

void LIRenImageOverlay::updatePositionGeometry ()
{
	/* Prevent divide by zero. */
	if (!mPixelWidth || !mPixelHeight)
	{
		render_op.vertexData->vertexCount = 0;
		return;
	}

	/* Calculate vertex coordinates. */
	float left = _getDerivedLeft() * 2.0f - 1.0f;
	float top = 1.0f - _getDerivedTop() * 2.0f;
	float xscale = 2.0f * mWidth / mPixelWidth;
	float yscale = 2.0f * mHeight / mPixelHeight;
	float xtile = xscale * src_tiling[1];
	float ytile = yscale * src_tiling[4];
	float x[4] =
	{
		left,
		left + xscale * src_tiling[0],
		left + xscale * (mPixelWidth - src_tiling[2]),
		left + xscale * mPixelWidth
	};
	float y[4] =
	{
		top,
		top - yscale * src_tiling[3],
		top - yscale * (mPixelHeight - src_tiling[5]),
		top - yscale * mPixelHeight
	};

	/* Calculate texture coordinates. */
	float uscale = 1.0f / src_size[0];
	float vscale = 1.0f / src_size[1];
	float u[4] =
	{
		uscale * (src_pos[0]),
		uscale * (src_pos[0] + src_tiling[0]),
		uscale * (src_pos[0] + src_tiling[0] + src_tiling[1]),
		uscale * (src_pos[0] + src_tiling[0] + src_tiling[1] + src_tiling[2])
	};
	float v[4] =
	{
		vscale * (src_pos[1]),
		vscale * (src_pos[1] + src_tiling[3]),
		vscale * (src_pos[1] + src_tiling[3] + src_tiling[4]),
		vscale * (src_pos[1] + src_tiling[3] + src_tiling[4] + src_tiling[5])
	};

	/* Lock the vertex buffer. */
	Ogre::HardwareVertexBufferSharedPtr vbuf =
		render_op.vertexData->vertexBufferBinding->getBuffer (0);
	float z = Ogre::Root::getSingleton().getRenderSystem()->getMaximumDepthInputValue ();

	/* Setup vertex packing. */
	LIRenTilePacker packer (vbuf->lock (Ogre::HardwareBuffer::HBL_DISCARD), z);
	packer.set_color (color);
	if (rotation_angle != 0.0f)
	{
		float cx = left + xscale * rotation_center[0];
		float cy = top - yscale * rotation_center[1];
		packer.set_rotation (rotation_angle, cx, cy, xscale / yscale);
	}
	float clip[4];
	if (clipping)
	{
		clip[0] = left + xscale * dst_clip[0];
		clip[1] = top - yscale * (dst_clip[1] + dst_clip[3]);
		clip[2] = xscale * dst_clip[2];
		clip[3] = yscale * dst_clip[3];
		packer.set_clipping (clip);
	}

	/* Add corners. */
	packer.add_quad (x[0], y[0], u[0], v[0], x[1], y[1], u[1], v[1]);
	packer.add_quad (x[2], y[0], u[2], v[0], x[3], y[1], u[3], v[1]);
	packer.add_quad (x[2], y[2], u[2], v[2], x[3], y[3], u[3], v[3]);
	packer.add_quad (x[0], y[2], u[0], v[2], x[1], y[3], u[1], v[3]);

	/* Add top and bottom borders. */
	for (float i = x[1] ; i < x[2] ; i += xtile)
	{
		float wtile = LIMAT_MIN (x[2] - i, xtile);
		float xe = i + wtile;
		float ue = u[1] + (u[2] - u[1]) * (wtile / xtile);
		packer.add_quad (i, y[0], u[1], v[0], xe, y[1], ue, v[1]);
		packer.add_quad (i, y[2], u[1], v[2], xe, y[3], ue, v[3]);
	}

	/* Add left and right borders. */
	for (float j = y[1] ; j > y[2] ; j -= ytile)
	{
		float htile = LIMAT_MIN (j - y[2], ytile);
		float ye = j - htile;
		float ve = v[1] + (v[2] - v[1]) * (htile / ytile);
		packer.add_quad (x[0], j, u[0], v[1], x[1], ye, u[1], ve);
		packer.add_quad (x[2], j, u[2], v[1], x[3], ye, u[3], ve);
	}

	/* Add the filling. */
	for (float i = x[1] ; i < x[2] ; i += xtile)
	{
		float wtile = LIMAT_MIN (x[2] - i, xtile);
		float xe = i + wtile;
		float ue = u[1] + (u[2] - u[1]) * (wtile / xtile);
		for (float j = y[1] ; j > y[2] ; j -= ytile)
		{
			float htile = LIMAT_MIN (j - y[2], ytile);
			float ye = j - htile;
			float ve = v[1] + (v[2] - v[1]) * (htile / ytile);
			packer.add_quad (i, j, u[1], v[1], xe, ye, ue, ve);
		}
	}

	/* Unlock the vertex buffer. */
	render_op.vertexData->vertexCount = packer.pos / 6;
	vbuf->unlock();
}

void LIRenImageOverlay::updateTextureGeometry ()
{
}

/** @} */
/** @} */
/** @} */
