/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup ligen Generator
 * @{
 * \addtogroup LIGenGenerator Generator
 * @{
 */

#include <lipsofsuna/engine.h>
#include <lipsofsuna/network.h>
#include <lipsofsuna/system.h>
#include "generator.h"

static int private_init (
	LIGenGenerator* self);

static int
private_brush_disabled (LIGenGenerator*  self,
                        LIGenStroke*     stroke,
                        LIGenRulestroke* rstroke);

static int
private_brush_exists (LIGenGenerator*  self,
                      LIGenStroke*     stroke,
                      LIGenRulestroke* rstroke);

static int
private_brush_intersects (LIGenGenerator*  self,
                          LIGenStroke*     stroke,
                          LIGenRulestroke* rstroke);

static int
private_rule_apply (LIGenGenerator* self,
                    LIGenStroke*    stroke,
                    LIGenRule*      rule,
                    LIGenBrush*     brush);

static int
private_rule_test (LIGenGenerator* self,
                   LIGenStroke*    stroke,
                   LIGenRule*      rule);

/*****************************************************************************/

/**
 * \brief Creates a new generator module.
 *
 * \param paths Path information.
 * \param callbacks Callbacks.
 * \param sectors Sector manager.
 * \return New generator or NULL.
 */
LIGenGenerator*
ligen_generator_new (LIPthPaths*     paths,
                     LICalCallbacks* callbacks)
{
	LIGenGenerator* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIGenGenerator));
	if (self == NULL)
		return NULL;
	self->paths = paths;
	self->callbacks = callbacks;

	/* Load databases. */
	if (!private_init (self))
		goto error;

	return self;

error:
	ligen_generator_free (self);
	return NULL;
}

/**
 * \brief Frees the generator.
 *
 * \param self Generator.
 */
void
ligen_generator_free (LIGenGenerator* self)
{
	LIAlgU32dicIter iter;

	/* Free brushes. */
	if (self->brushes != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->brushes)
			ligen_brush_free (iter.value);
		lialg_u32dic_free (self->brushes);
	}

	lisys_free (self->strokes.array);
	lisys_free (self);
}

/**
 * \brief Removes all created regions.
 * \param self Generator.
 */
void
ligen_generator_clear (LIGenGenerator* self)
{
	lisys_free (self->strokes.array);
	self->strokes.array = NULL;
	self->strokes.count = 0;
}

/**
 * \brief Finds a brush by id.
 *
 * \param self Generator.
 * \param id Brush number.
 * \return Brush or NULL.
 */
LIGenBrush*
ligen_generator_find_brush (LIGenGenerator* self,
                            int             id)
{
	return lialg_u32dic_find (self->brushes, id);
}

/**
 * \brief Finds a brush by name.
 *
 * \param self Generator.
 * \param name Brush name.
 * \return Brush or NULL.
 */
LIGenBrush*
ligen_generator_find_brush_by_name (LIGenGenerator* self,
                                    const char*     name)
{
	LIAlgU32dicIter iter;
	LIGenBrush* brush;

	/* Find root brush. */
	/* TODO: Dictionary would be faster. */
	LIALG_U32DIC_FOREACH (iter, self->brushes)
	{
		brush = iter.value;
		if (!strcmp (brush->name, name))
			return brush;
	}

	return NULL;
}

/**
 * \brief Inserts a brush to the generator.
 *
 * The ownership of the brush is transferred to the generator if successful.
 *
 * \param self Generator.
 * \param brush Brush.
 * \return Nonzero on success.
 */
int
ligen_generator_insert_brush (LIGenGenerator* self,
                              LIGenBrush*     brush)
{
	if (lialg_u32dic_find (self->brushes, brush->id) != NULL)
	{
		lisys_assert (0);
		return 0;
	}
	if (!lialg_u32dic_insert (self->brushes, brush->id, brush))
		return 0;

	return 1;
}

/**
 * \brief Inserts a stroke to the generator.
 *
 * \param self Generator.
 * \param brush Brush number.
 * \param x Insertion position.
 * \param y Insertion position.
 * \param z Insertion position.
 * \return Nonzero on success.
 */
int
ligen_generator_insert_stroke (LIGenGenerator* self,
                               int             brush,
                               int             x,
                               int             y,
                               int             z)
{
	LIGenBrush* brush_;
	LIGenStroke stroke;

	brush_ = lialg_u32dic_find (self->brushes, brush);
	lisys_assert (brush_ != NULL);
	stroke.pos[0] = x;
	stroke.pos[1] = y;
	stroke.pos[2] = z;
	stroke.size[0] = brush_->size[0];
	stroke.size[1] = brush_->size[1];
	stroke.size[2] = brush_->size[2];
	stroke.brush = brush;
	if (!lialg_array_append (&self->strokes, &stroke))
		return 0;

	return 1;
}

/**
 * \brief Removes a brush from the generator.
 *
 * The brush and all the strokes referencing it are removed.
 *
 * \param self Generator.
 * \param id Brush number.
 */
void
ligen_generator_remove_brush (LIGenGenerator* self,
                              int             id)
{
	LIGenBrush* brush;

	/* Find brush. */
	brush = lialg_u32dic_find (self->brushes, id);
	if (brush == NULL)
		return;

	/* Free brush. */
	lialg_u32dic_remove (self->brushes, id);
	ligen_brush_free (brush);
}

/**
 * \brief Extends the map by one rule.
 *
 * \param self Generator.
 * \return Nonzero on success.
 */
int
ligen_generator_step (LIGenGenerator* self)
{
	int i;
	int j;
	int k;
	int tmp;
	int* rnd;
	int* rnd1;
	LIGenBrush* brush;
	LIGenRule* rule;
	LIGenStroke stroke;

	/* Randomize stroke order. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		k = rand () % self->strokes.count;
		stroke = self->strokes.array[i];
		self->strokes.array[i] = self->strokes.array[k];
		self->strokes.array[k] = stroke;
	}
	rnd = NULL;

	/* Try to expand each stroke. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		/* The stroke array may be reallocated in private_rule_apply
		 * so we need to create a copy of the stroke here. */
		stroke = self->strokes.array[i];
		brush = lialg_u32dic_find (self->brushes, stroke.brush);
		lisys_assert (brush != NULL);
		if (!brush->rules.count)
			continue;

		/* Randomize rule order. */
		rnd1 = realloc (rnd, brush->rules.count * sizeof (int));
		if (rnd1 == NULL)
		{
			lisys_free (rnd);
			return 0;
		}
		rnd = rnd1;
		for (j = 0 ; j < brush->rules.count ; j++)
			rnd[j] = j;
		for (j = 0 ; j < brush->rules.count ; j++)
		{
			k = rand () % brush->rules.count;
			tmp = rnd[j];
			rnd[j] = rnd[k];
			rnd[k] = tmp;
		}

		/* Try each rule. */
		for (j = 0 ; j < brush->rules.count ; j++)
		{
			rule = brush->rules.array[rnd[j]];
			if (private_rule_test (self, &stroke, rule))
			{
				lisys_free (rnd);
				if (!private_rule_apply (self, &stroke, rule, brush))
					return 0;
				return 1;
			}
		}
	}
	lisys_free (rnd);

	return 0;
}

/*****************************************************************************/

static int private_init (
	LIGenGenerator* self)
{
	self->brushes = lialg_u32dic_new ();
	if (self->brushes == NULL)
		return 0;
	return 1;
}

static int
private_brush_disabled (LIGenGenerator*  self,
                        LIGenStroke*     stroke,
                        LIGenRulestroke* rstroke)
{
	LIGenBrush* brush;

	brush = lialg_u32dic_find (self->brushes, rstroke->brush);
	lisys_assert (brush != NULL);

	return brush->disabled;
}

static int
private_brush_exists (LIGenGenerator*  self,
                      LIGenStroke*     stroke,
                      LIGenRulestroke* rstroke)
{
	int i;
	int pos[3];
	LIGenStroke* stroke1;

	/* Calculate world position. */
	pos[0] = stroke->pos[0] + rstroke->pos[0];
	pos[1] = stroke->pos[1] + rstroke->pos[1];
	pos[2] = stroke->pos[2] + rstroke->pos[2];

	/* Test against all strokes. */
	/* FIXME: Use space partitioning. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		stroke1 = self->strokes.array + i;
		if (stroke1->brush == rstroke->brush &&
		    stroke1->pos[0] == pos[0] &&
		    stroke1->pos[1] == pos[1] &&
		    stroke1->pos[2] == pos[2])
			return 1;
	}

	return 0;
}

static int
private_brush_intersects (LIGenGenerator*  self,
                          LIGenStroke*     stroke,
                          LIGenRulestroke* rstroke)
{
	int i;
	int min0[3];
	int min1[3];
	int max0[3];
	int max1[3];
	LIGenBrush* brush;
	LIGenStroke* stroke1;

	/* Calculate world position. */
	brush = lialg_u32dic_find (self->brushes, rstroke->brush);
	lisys_assert (brush != NULL);
	min0[0] = stroke->pos[0] + rstroke->pos[0];
	min0[1] = stroke->pos[1] + rstroke->pos[1];
	min0[2] = stroke->pos[2] + rstroke->pos[2];
	max0[0] = min0[0] + brush->size[0];
	max0[1] = min0[1] + brush->size[1];
	max0[2] = min0[2] + brush->size[2];

	/* Test against all strokes. */
	/* FIXME: Use space partitioning. */
	for (i = 0 ; i < self->strokes.count ; i++)
	{
		stroke1 = self->strokes.array + i;
		min1[0] = stroke1->pos[0];
		min1[1] = stroke1->pos[1];
		min1[2] = stroke1->pos[2];
		max1[0] = min1[0] + stroke1->size[0];
		max1[1] = min1[1] + stroke1->size[1];
		max1[2] = min1[2] + stroke1->size[2];
		if (max0[0] <= min1[0] || max1[0] <= min0[0] ||
		    max0[1] <= min1[1] || max1[1] <= min0[1] ||
		    max0[2] <= min1[2] || max1[2] <= min0[2])
			continue;
		return 1;
	}

	return 0;
}

static int
private_rule_apply (LIGenGenerator* self,
                    LIGenStroke*    stroke,
                    LIGenRule*      rule,
                    LIGenBrush*     brush)
{
	int i;
	int orig;
	LIGenBrush* brush1;
	LIGenStroke stroke1;
	LIGenRulestroke* rstroke;

	printf ("BRUSH %s RULE %s\n", brush->name, rule->name);

	orig = self->strokes.count;
	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		rstroke = rule->strokes.array + i;
		brush1 = lialg_u32dic_find (self->brushes, rstroke->brush);
		lisys_assert (brush1 != NULL);
		printf (" * CREATE %s\n", brush1->name);
		stroke1.pos[0] = stroke->pos[0] + rstroke->pos[0];
		stroke1.pos[1] = stroke->pos[1] + rstroke->pos[1];
		stroke1.pos[2] = stroke->pos[2] + rstroke->pos[2];
		stroke1.size[0] = brush1->size[0];
		stroke1.size[1] = brush1->size[1];
		stroke1.size[2] = brush1->size[2];
		stroke1.brush = brush1->id;
		if (!lialg_array_append (&self->strokes, &stroke1))
		{
			self->strokes.count = orig;
			return 0;
		}
	}

	return 1;
}

static int
private_rule_test (LIGenGenerator* self,
                   LIGenStroke*    stroke,
                   LIGenRule*      rule)
{
	int i;
	LIGenRulestroke* rstroke;

	for (i = 0 ; i < rule->strokes.count ; i++)
	{
		rstroke = rule->strokes.array + i;
		if (rstroke->flags & LIGEN_RULE_REQUIRE)
		{
			if (!private_brush_exists (self, stroke, rstroke))
				return 0;
		}
		else
		{
			if (private_brush_disabled (self, stroke, rstroke) ||
			    private_brush_intersects (self, stroke, rstroke))
				return 0;
		}
	}

	return 1;
}

/** @} */
/** @} */
