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
 * \addtogroup liarc Archive
 * @{
 * \addtogroup liarcSerialize Serialize
 * @{
 */

#include "archive-serialize.h"

/**
 * \brief Creates a new deserializer.
 *
 * \param file Filename.
 * \return New deserializer or NULL.
 */
liarcSerialize*
liarc_serialize_new_read (const char*  file)
{
	liarcSerialize* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liarcSerialize));
	if (self == NULL)
		return NULL;

	/* Allocate reader. */
	self->reader = li_reader_new_from_file (file);
	if (self->reader == NULL)
	{
		liarc_serialize_free (self);
		return NULL;
	}

	/* Allocate object dictionary. */
	self->objects = lialg_ptrdic_new ();
	if (self->objects == NULL)
	{
		liarc_serialize_free (self);
		return NULL;
	}

	/* Allocate pointer dictionary. */
	self->pointers = lialg_ptrdic_new ();
	if (self->pointers == NULL)
	{
		liarc_serialize_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Creates a new serializer.
 *
 * \param file Filename.
 * \return New serializer or NULL.
 */
liarcSerialize*
liarc_serialize_new_write (const char*  file)
{
	liarcSerialize* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liarcSerialize));
	if (self == NULL)
		return NULL;

	/* Allocate writer. */
	self->writer = liarc_writer_new_file (file);
	if (self->writer == NULL)
	{
		liarc_serialize_free (self);
		return NULL;
	}
	if (!lialg_list_prepend (&self->stack, self->writer))
	{
		liarc_serialize_free (self);
		liarc_writer_free (self->writer);
		return NULL;
	}

	/* Allocate object dictionary. */
	self->objects = lialg_ptrdic_new ();
	if (self->objects == NULL)
	{
		liarc_serialize_free (self);
		return NULL;
	}

	/* Allocate pointer dictionary. */
	self->pointers = lialg_ptrdic_new ();
	if (self->pointers == NULL)
	{
		liarc_serialize_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the serializer.
 *
 * \param self Serializer.
 */
void
liarc_serialize_free (liarcSerialize* self)
{
	lialgList* ptr;

	/* Free writer stack. */
	for (ptr = self->stack ; ptr != NULL ; ptr = ptr->next)
		liarc_writer_free (ptr->data);
	lialg_list_free (self->stack);

	if (self->objects != NULL)
		lialg_ptrdic_free (self->objects);
	if (self->pointers != NULL)
		lialg_ptrdic_free (self->pointers);
	if (self->reader != NULL)
		li_reader_free (self->reader);
	free (self);
}

/**
 * \brief Finds a serialized or deserialized object.
 *
 * \param self Serializer.
 * \param value Object.
 * \return Object or NULL.
 */
void*
liarc_serialize_find_object (liarcSerialize* self,
                             int             value)
{
	lialgPtrdicIter iter;

	LI_FOREACH_PTRDIC (iter, self->objects)
	{
		if (((intptr_t) iter.value) - 1 == value)
			return iter.key;
	}

	return NULL;
}

/**
 * \brief Finds the save index of a serialized or deserialized object.
 *
 * \param self Serializer.
 * \param value Object.
 * \return Index of the object or -1.
 */
int
liarc_serialize_find_object_id (liarcSerialize* self,
                                void*           value)
{
	return ((intptr_t) lialg_ptrdic_find (self->objects, value)) - 1;
}

/**
 * \brief Finds a serialized or deserialized pointer.
 *
 * \param self Serializer.
 * \param value Pointer.
 * \return Pointer or NULL.
 */
void*
liarc_serialize_find_pointer (liarcSerialize* self,
                              int             value)
{
	lialgPtrdicIter iter;

	LI_FOREACH_PTRDIC (iter, self->pointers)
	{
		if (((intptr_t) iter.value) - 1 == value)
			return iter.key;
	}

	return NULL;
}

/**
 * \brief Finds a serialized or deserialized pointer.
 *
 * \param self Serializer.
 * \param value Pointer.
 * \return Index of the pointer or -1.
 */
int
liarc_serialize_find_pointer_id (liarcSerialize* self,
                                 void*           value)
{
	return ((intptr_t) lialg_ptrdic_find (self->pointers, value)) - 1;
}

/**
 * \brief Adds an object to the list of processed userdata.
 *
 * \param self Serializer.
 * \param value Object.
 * \return Nonzero on success.
 */
int
liarc_serialize_insert_object (liarcSerialize* self,
                               void*           value)
{
	assert (lialg_ptrdic_find (self->objects, value) == NULL);

	if (!lialg_ptrdic_insert (self->objects, value, (void*)(intptr_t)(self->objects->size + 1)))
		return 0;

	return 1;
}

/**
 * \brief Adds a pointer to the list of processed userdata.
 *
 * \param self Serializer.
 * \param key Pointer id.
 * \param value Pointer.
 * \return Nonzero on success.
 */
int
liarc_serialize_insert_pointer (liarcSerialize* self,
                                int             key,
                                void*           value)
{
	if (self->writer != NULL)
	{
		assert (key == -1);
		key = self->pointers->size;
	}

	assert (liarc_serialize_find_pointer (self, key) == NULL);
	assert (lialg_ptrdic_find (self->pointers, value) == NULL);

	if (!lialg_ptrdic_insert (self->pointers, value, (void*)(intptr_t)(key + 1)))
		return 0;

	return 1;
}

/**
 * \brief Pops a writer from the top of the writer stack.
 *
 * \param self Serializer.
 * \return New writer.
 */
liarcWriter*
liarc_serialize_pop (liarcSerialize* self)
{
	liarcWriter* writer;

	assert (self->stack != NULL);
	assert (self->stack->next != NULL);

	writer = self->writer;
	self->writer = self->stack->next->data;
	lialg_list_remove (&self->stack, self->stack);

	return writer;
}

/**
 * \brief Pushes a writer to the top of the writer stack.
 *
 * \param self Serializer.
 * \return Nonzero on success.
 */
int
liarc_serialize_push (liarcSerialize* self)
{
	liarcWriter* writer;

	/* Allocate writer. */
	writer = liarc_writer_new ();
	if (writer == NULL)
		return 0;

	/* Insert to stack. */
	if (!lialg_list_prepend (&self->stack, writer))
	{
		liarc_writer_free (writer);
		return 0;
	}

	/* Make current. */
	self->writer = writer;
	return 1;
}

/**
 * \brief Tells if this is structure is for reading or writing.
 *
 * \param self Serializer.
 * \return Nonzero if for writing, zero if for reading.
 */
int
liarc_serialize_get_write (const liarcSerialize* self)
{
	if (self->writer != NULL)
		return 1;
	return 0;
}

/** @} */
/** @} */
