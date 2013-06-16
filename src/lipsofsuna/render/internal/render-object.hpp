/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __RENDER_INTERNAL_OBJECT_HPP__
#define __RENDER_INTERNAL_OBJECT_HPP__

class LIRenAttachment;

class LIRenObject
{
public:
	LIRenObject(
		LIRenRender* render,
		int          id);

	~LIRenObject();

	void add_model (
		LIRenModel* model);

	int channel_animate (
		int                     channel,
		int                     keep,
		const LIMdlPoseChannel* info);

	void channel_edit (
		int                   channel,
		int                   frame,
		const char*           node,
		const LIMatTransform* transform,
		float                 scale);

	void channel_fade (
		int   channel,
		float time);

	LIMdlPoseChannel* channel_get_state (
		int channel) const;

	void clear_animations ();

	void clear_models ();

	int find_node (
		const char*     name,
		int             world,
		LIMatTransform* result) const;

	void particle_animation (
		float        start,
		int          loop);

	void model_changed (
		LIRenModel* model);

	void remove_model (
		LIRenModel* model);

	void replace_model (
		LIRenModel* model_old,
		LIRenModel* model_new);

	void replace_texture (
		const char*  name,
		int          width,
		int          height,
		const void*  pixels);

	void update (
		float secs);

	int set_effect (
		const char*  shader,
		const float* params);

	int get_attachment_count () const { return attachments.size (); }

	int get_id () const;

	int get_loaded () const;

	int set_model (
		LIRenModel* model);

	int set_particle (
		const char* name);

	void set_particle_emitting (
		int value);

	int get_visible () const { return visible; }

	int set_visible (
		int value);

	void set_render_distance (
		float value);

	int get_shadow_casting () const { return shadow_casting; }

	void set_shadow_casting (
		int value);

	void set_transform (
		const LIMatTransform* value);

private:

	LIMdlPose* channel_animate (
		LIMdlPose*              pose,
		int                     channel,
		int                     keep,
		const LIMdlPoseChannel* info);

	void channel_fade (
		LIMdlPose* pose,
		int        channel,
		float      time);

	void rebuild_skeleton ();

	void remove_entity (
		int index);

	void update_entity_settings ();

private:
	int id;
	int visible;
	int shadow_casting;
	int skeleton_rebuild_needed;
	float render_distance;
	LIMatTransform transform;
	LIMdlPose* pose;
	std::vector<LIRenAttachment*> attachments;

// FIXME
public:
	LIMdlPoseSkeleton* pose_skeleton;
	LIRenRender* render;
	Ogre::SceneNode* node;
};

#endif
