/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup liwdgTree Tree
 * @{
 */

#include <algorithm/lips-algorithm.h>
#include "widget-tree.h"

#define LIWDG_TREE_NEST 10

struct _liwdgTreerow
{
	int depth;
	int expand;
	int highlight;
	char* text;
	void* data;
	lifntFont* font;
	lifntLayout* layout;
	liwdgTree* tree;
	liwdgTreerow* parent;
	struct
	{
		int count;
		liwdgTreerow** array;
	} rows;
};

struct _liwdgTree
{
	liwdgWidget base;
	liwdgTreerow root;
};

static int
private_init (liwdgTree*    self,
              liwdgManager* manager);

static void
private_free (liwdgTree* self);

static int
private_event (liwdgTree*  self,
               liwdgEvent* event);

static void
private_rebuild (liwdgTree* self);

void
private_treerow_free (liwdgTreerow* self);

static liwdgTreerow*
private_treerow_find_active (liwdgTreerow* self);

static liwdgTreerow*
private_treerow_find_clicked (liwdgTreerow* self,
                              liwdgRect*    rect,
                              int*          rowy,
                              int           x,
                              int           y);

void
private_treerow_foreach (liwdgTreerow* self,
                         void        (*call)());

int
private_treerow_render (liwdgTreerow* self,
                        liwdgRect*    rect,
                        liwdgStyle*   style,
                        int           y);

static int
private_treerow_get_height (liwdgTreerow* self);

static void
private_treerow_get_request (liwdgTreerow* self,
                             int*          w,
                             int*          h);

/****************************************************************************/

const liwdgClass liwdgTreeType =
{
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Tree", sizeof (liwdgTree),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liwdg_tree_new (liwdgManager* manager)
{
	liwdgWidget* self;

	self = liwdg_widget_new (manager, &liwdgTreeType);
	if (self == NULL)
		return NULL;

	return self;
}

void
liwdg_tree_clear (liwdgTree* self)
{
	private_treerow_free (&self->root);
	self->root.rows.array = NULL;
	self->root.rows.count = 0;
	private_rebuild (self);
}

void
liwdg_tree_foreach (liwdgTree* self,
                    void     (*call)())
{
	private_treerow_foreach (&self->root, call);
}

liwdgTreerow*
liwdg_tree_get_active (liwdgTree* self)
{
	return private_treerow_find_active (&self->root);
}

liwdgTreerow*
liwdg_tree_get_root (liwdgTree* self)
{
	return &self->root;
}

liwdgTreerow*
liwdg_treerow_append_row (liwdgTreerow* self,
                          const char*   text,
                          void*         data)
{
	int h;
	liwdgTreerow* tmp;

	/* Format row. */
	tmp = lisys_calloc (1, sizeof (liwdgTreerow));
	if (tmp == NULL)
		return NULL;
	tmp->parent = self;
	tmp->data = data;
	tmp->depth = self->depth + 1;
	tmp->tree = self->tree;
	tmp->text = listr_dup (text);
	if (tmp->text == NULL)
	{
		lisys_free (tmp);
		return NULL;
	}
	tmp->font = liwdg_manager_find_font (LIWDG_WIDGET (self->tree)->manager, "default");
	tmp->layout = lifnt_layout_new ();
	if (tmp->layout == NULL)
	{
		lisys_free (tmp->text);
		lisys_free (tmp);
		return NULL;
	}
	if (tmp->font != NULL)
	{
		h = lifnt_font_get_height (tmp->font);
		lifnt_layout_append_string (tmp->layout, tmp->font, text);
	}

	/* Append row. */
	if (!lialg_array_append (&self->rows, &tmp))
	{
		lifnt_layout_free (tmp->layout);
		lisys_free (tmp->text);
		lisys_free (tmp);
		return NULL;
	}

	/* Update view. */
	private_rebuild (self->tree);

	return self->rows.array[self->rows.count - 1];
}

void
liwdg_treerow_remove_row (liwdgTreerow* self,
                          int           index)
{
	assert (index >= 0);
	assert (index < self->rows.count);

	private_treerow_free (self->rows.array[index]);
	lialg_array_remove (&self->rows, index);
	private_rebuild (self->tree);
}

void*
liwdg_treerow_get_data (liwdgTreerow* self)
{
	return self->data;
}

void
liwdg_treerow_set_data (liwdgTreerow* self,
                        void*         value)
{
	self->data = value;
}

int
liwdg_treerow_get_expanded (liwdgTreerow* self)
{
	return self->expand;
}

void
liwdg_treerow_set_expanded (liwdgTreerow* self,
                            int           value)
{
	self->expand = value;
	private_rebuild (self->tree);
}

int
liwdg_treerow_get_highlighted (liwdgTreerow* self)
{
	return self->highlight;
}

void
liwdg_treerow_set_highlighted (liwdgTreerow* self,
                               int           value)
{
	self->highlight = value;
}

int
liwdg_treerow_get_index (liwdgTreerow* self)
{
	int i;

	assert (self->parent != NULL);
	for (i = 0 ; i < self->parent->rows.count ; i++)
	{
		if (self->parent->rows.array[i] == self)
			return i;
	}
	assert (0);

	return 0;
}

liwdgTreerow*
liwdg_treerow_get_parent (liwdgTreerow* self)
{
	return self->parent;
}

liwdgTreerow*
liwdg_treerow_get_row (liwdgTreerow* self,
                       int           index)
{
	assert (index >= 0);
	assert (index < self->rows.count);

	return self->rows.array[index];
}

int
liwdg_treerow_get_row_count (liwdgTreerow* self)
{
	return self->rows.count;
}

const char*
liwdg_treerow_get_text (liwdgTreerow* self)
{
	return self->text;
}

int
liwdg_treerow_set_text (liwdgTreerow* self,
                        const char*   value)
{
	int h;
	char* tmp;

	assert (self->text != NULL);

	/* Set new text. */
	tmp = listr_dup (value);
	if (tmp == NULL)
		return 0;
	lisys_free (self->text);
	self->text = tmp;

	/* Update layout. */
	lifnt_layout_clear (self->layout);
	if (self->font != NULL)
	{
		h = lifnt_font_get_height (self->font);
		lifnt_layout_append_string (self->layout, self->font, value);
	}

	/* Update view. */
	private_rebuild (self->tree);

	return 1;
}

/*****************************************************************************/

static int
private_init (liwdgTree*    self,
              liwdgManager* manager)
{
	if (!liwdg_widget_register_callback (LIWDG_WIDGET (self), LIWDG_CALLBACK_PRESSED, lical_marshal_DATA_PTR_PTR))
		return 0;
	self->root.tree = self;
	self->root.depth = 0;
	self->root.expand = 1;
	self->root.text = NULL;
	return 1;
}

static void
private_free (liwdgTree* self)
{
	private_treerow_free (&self->root);
}

static int
private_event (liwdgTree*  self,
               liwdgEvent* event)
{
	int y;
	liwdgRect rect;
	liwdgTreerow* row;
	liwdgStyle* style;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
			y = rect.y + rect.height;
			row = private_treerow_find_clicked (&self->root, &rect, &y, event->button.x, event->button.y);
			if (row == NULL)
				return 0;
			if (row->rows.count && event->button.x < rect.x + row->depth * LIWDG_TREE_NEST)
			{
				row->expand = !row->expand;
				private_rebuild (self);
				return 0;
			}
			else
				return lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_PRESSED, self, row);
		case LIWDG_EVENT_TYPE_RENDER:
			style = liwdg_widget_get_style (LIWDG_WIDGET (self), "tree");
			/* Draw base. */
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), "tree", &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), "tree", NULL);
			/* Draw rows. */
			private_treerow_render (&self->root, &rect, style, 0);
			return 1;
	}

	return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
}

void
private_treerow_free (liwdgTreerow* self)
{
	int i;

	if (self->layout != NULL)
		lifnt_layout_free (self->layout);
	for (i = 0 ; i < self->rows.count ; i++)
		private_treerow_free (self->rows.array[i]);
	lisys_free (self->rows.array);
	lisys_free (self->text);
	if (self->parent != NULL)
		lisys_free (self);
}

static void
private_rebuild (liwdgTree* self)
{
	liwdgSize size;

	private_treerow_get_request (&self->root, &size.width, &size.height);
	liwdg_widget_set_style_request (LIWDG_WIDGET (self), size.width, size.height, "tree");
}

static liwdgTreerow*
private_treerow_find_active (liwdgTreerow* self)
{
	int i;
	liwdgTreerow* ret;

	if (self->highlight)
		return self;
	for (i = 0 ; i < self->rows.count ; i++)
	{
		ret = private_treerow_find_active (self->rows.array[i]);
		if (ret != NULL)
			return ret;
	}

	return NULL;
}

static liwdgTreerow*
private_treerow_find_clicked (liwdgTreerow* self,
                              liwdgRect*    rect,
                              int*          rowy,
                              int           x,
                              int           y)
{
	int h;
	int i;
	liwdgTreerow* ret;

	if (self->layout != NULL)
	{
		h = private_treerow_get_height (self);
		*rowy -= h;
		if (*rowy <= y && y < *rowy + h)
			return self;
	}

	if (self->expand)
	{
		for (i = 0 ; i < self->rows.count ; i++)
		{
			ret = private_treerow_find_clicked (self->rows.array[i], rect, rowy, x, y);
			if (ret != NULL)
				return ret;
		}
	}

	return NULL;
}

void
private_treerow_foreach (liwdgTreerow* self,
                         void        (*call)())
{
	int i;

	call (self->data);
	for (i = 0 ; i < self->rows.count ; i++)
		private_treerow_foreach (self->rows.array[i], call);
}

int
private_treerow_render (liwdgTreerow* self,
                        liwdgRect*    rect,
                        liwdgStyle*   style,
                        int           y)
{
	int i;
	int h;
	int x;
	int y0;
	int pointer[2];
	liwdgManager* manager;

	/* Calculate offset. */
	h = private_treerow_get_height (self);
	y += h;
	y0 = y;
	x = rect->x + self->depth * LIWDG_TREE_NEST;

	/* Get relative pointer position. */
	manager = LIWDG_WIDGET (self->tree)->manager;
	liwdg_widget_translate_coords (LIWDG_WIDGET (self->tree),
		manager->pointer.x, manager->pointer.y, pointer + 0, pointer + 1);

	/* Render hover. */
	if (pointer[0] >= rect->x && pointer[0] < rect->x + rect->width &&
	    pointer[1] >= rect->y + rect->height - y - 1 &&
	    pointer[1] < rect->y + rect->height - y + h - 1)
	{
		glColor4fv (style->hover);
		glBindTexture (GL_TEXTURE_2D, 0);
		glBegin (GL_TRIANGLE_STRIP);
		glVertex2i (rect->x, rect->y + rect->height - y - 1);
		glVertex2i (rect->x + rect->width, rect->y + rect->height - y - 1);
		glVertex2i (rect->x, rect->y + rect->height - y + h - 1);
		glVertex2i (rect->x + rect->width, rect->y + rect->height - y + h - 1);
		glEnd ();
	}

	/* Render selection. */
	if (self->highlight)
	{
		glColor4fv (style->selection);
		glBindTexture (GL_TEXTURE_2D, 0);
		glBegin (GL_TRIANGLE_STRIP);
		glVertex2i (rect->x, rect->y + rect->height - y - 1);
		glVertex2i (rect->x + rect->width, rect->y + rect->height - y - 1);
		glVertex2i (rect->x, rect->y + rect->height - y + h - 1);
		glVertex2i (rect->x + rect->width, rect->y + rect->height - y + h - 1);
		glEnd ();
	}

	/* Render layout. */
	if (self->layout != NULL)
	{
		glColor4fv (style->color);
		lifnt_layout_render (self->layout, x, rect->y + rect->height - y - 1);
	}

	/* Render children. */
	if (self->expand)
	{
		for (i = 0 ; i < self->rows.count ; i++)
			y = private_treerow_render (self->rows.array[i], rect, style, y);
	}

	/* Render hierarchy. */
	if (self->layout != NULL)
	{
		/* FIXME */
		glColor3f (0.8f, 0.8f, 0.8f);
		glBindTexture (GL_TEXTURE_2D, 0);
		glBegin (GL_LINES);
		glVertex2f (x - 0.5f * LIWDG_TREE_NEST, rect->y + rect->height - y0 + 0.5f * h - 1);
		glVertex2f (x, rect->y + rect->height - y0 + 0.5f * h - 1);
		if (self != self->parent->rows.array[self->parent->rows.count - 1])
		{
			glVertex2f (x - 0.5f * LIWDG_TREE_NEST, rect->y + rect->height - y0 + h - 1);
			glVertex2f (x - 0.5f * LIWDG_TREE_NEST, rect->y + rect->height - y - 1);
		}
		else
		{
			glVertex2f (x - 0.5f * LIWDG_TREE_NEST, rect->y + rect->height - y0 + 1.0f * h - 1);
			glVertex2f (x - 0.5f * LIWDG_TREE_NEST, rect->y + rect->height - y0 + 0.5f * h - 1);
		}
		glEnd ();
	}

	return y;
}

static int
private_treerow_get_height (liwdgTreerow* self)
{
	int h0;
	int h1;

	if (self->layout != NULL)
	{
		if (self->font != NULL)
		{
			h0 = lifnt_layout_get_height (self->layout);
			h1 = lifnt_font_get_height (self->font);
			return LI_MAX (h0, h1);
		}
		return lifnt_layout_get_height (self->layout);
	}

	return 0;
}

static void
private_treerow_get_request (liwdgTreerow* self,
                             int*          w,
                             int*          h)
{
	int i;
	int w1;
	int h1;

	/* Get text request. */
	if (self->layout != NULL)
		*w = lifnt_layout_get_width (self->layout);
	else
		*w = 0;
	*h = private_treerow_get_height (self);
	*w += LIWDG_TREE_NEST * self->depth;

	/* Get child request. */
	if (self->expand)
	{
		for (i = 0 ; i < self->rows.count ; i++)
		{
			private_treerow_get_request (self->rows.array[i], &w1, &h1);
			*w = LI_MAX (*w, w1);
			*h += h1;
		}
	}
}

/** @} */
/** @} */
