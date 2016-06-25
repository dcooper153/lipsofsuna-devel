#version 120

uniform mat4 LOS_matrix_modelviewproj;
uniform vec4 LOS_time;

attribute vec3 vertex;
attribute vec3 tangent;
attribute vec4 colour;
attribute vec2 uv0;

varying vec3 F_coord;
varying vec2 F_texatlas;
varying vec2 F_texcoord;

void main()
{
	/* Atlas indexing.
	 * 
	 * uv0.x contains the texture atlas index. We convert that into the
	 * offset into the atlas texture here to reduce work in the fragment
	 * shader.
	 */
	int material = int(255.0 * colour.x + 0.5);
	F_texatlas = vec2(mod(material, 8), int(material / 8)) * 0.125;

	/* Grass animation. */
	vec3 t_vertex = vertex + tangent.x * vec3(LOS_time.y, 0.0, LOS_time.z);

	/* Vertex coordinate. */
	F_coord = t_vertex;
	F_texcoord = uv0;
	gl_Position = LOS_matrix_modelviewproj * vec4(t_vertex,1.0);
}
