Shader{
	name = "deferred-lava",
	config = [[
light-count 4
uniform uni_time TIME
uniform uni_matrixmodel MATRIXMODEL
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixnormal MATRIXNORMAL
uniform uni_matrixprojection MATRIXPROJECTION
uniform uni_texturemap[0] DIFFUSETEXTURE0
uniform uni_texturemap[1] NOISETEXTURE
uniform uni_lighttype[0] LIGHTTYPE0
uniform uni_lighttype[1] LIGHTTYPE1
uniform uni_lighttype[2] LIGHTTYPE2
uniform uni_lighttype[3] LIGHTTYPE3
uniform uni_shadowmap[0] SHADOWTEXTURE0
uniform uni_shadowmap[1] SHADOWTEXTURE1
uniform uni_shadowmap[2] SHADOWTEXTURE2
uniform uni_shadowmap[3] SHADOWTEXTURE3
uniform uni_lightmatrix[0] LIGHTMATRIX0
uniform uni_lightmatrix[1] LIGHTMATRIX1
uniform uni_lightmatrix[2] LIGHTMATRIX2
uniform uni_lightmatrix[3] LIGHTMATRIX3]],

	vertex = [[
const int lightcount = 4;
const int texturecount = 2;
varying vec4 var_vertex;
varying vec4 var_color;
varying vec3 var_normal;
varying vec2 var_texcoord;
varying vec4 var_lightcoord[lightcount];
varying vec3 var_lightdir[lightcount];
varying vec3 var_lighthalfv[lightcount];
uniform mat4 uni_matrixmodel;
uniform mat4 uni_matrixmodelview;
uniform mat3 uni_matrixnormal;
uniform mat4 uni_matrixprojection;
uniform mat4 uni_lightmatrix[lightcount];
void main()
{
	int i;
	vec4 tmp;

	/* Vertex attributes. */
	tmp = uni_matrixmodelview * gl_Vertex;
	var_vertex = uni_matrixmodel * gl_Vertex;
	var_color = gl_Color;
	var_normal = uni_matrixnormal * gl_Normal;
	var_texcoord = gl_MultiTexCoord0.xy;

	/* Light sources. */
	for (i = 0 ; i < lightcount ; i++)
	{
		var_lightdir[i] = gl_LightSource[i].position.xyz - tmp.xyz;
		var_lightcoord[i] = uni_lightmatrix[i] * tmp;
		var_lighthalfv[i] = normalize (gl_LightSource[i].halfVector.xyz);
	}

	gl_Position = uni_matrixprojection * tmp;
}]],

	fragment = [[
const int lightcount = 4;
const int texturecount = 2;
varying vec4 var_vertex;
varying vec4 var_color;
varying vec3 var_normal;
varying vec2 var_texcoord;
varying vec4 var_lightcoord[lightcount];
varying vec3 var_lightdir[lightcount];
varying vec3 var_lighthalfv[lightcount];
uniform float uni_time;
uniform int uni_lighttype[lightcount];
uniform mat4 uni_lightmatrix[lightcount];
uniform sampler2D uni_texturemap[texturecount];
uniform sampler2DShadow uni_shadowmap[lightcount];

/* Shadow mapping. */
float shadowmap(in int i, in vec2 pcf)
{
	float esm_c = 80.0;
	float esm_d = var_lightcoord[i].z;
	float esm_z = var_lightcoord[i].w * shadow2DProj (uni_shadowmap[i], var_lightcoord[i] + vec4 (pcf.xy, 0.0, 0.0)).r;
	float esm_f = exp (-esm_c * esm_d + esm_c * esm_z);
	return esm_f;
}
float shadowpcf(in int i)
{
	float blend = shadowmap (i, vec2 (0.0, 0.0));
	float pcfsz = clamp (0.004 * var_lightcoord[i].z, 0.02, 0.2);
	blend = clamp (blend, 0.0, 1.0);
	blend += clamp (shadowmap (i, vec2 (-pcfsz, 0.0)), 0.0, 1.0);
	blend += clamp (shadowmap (i, vec2 (pcfsz, 0.0)), 0.0, 1.0);
	blend += clamp (shadowmap (i, vec2 (0.0, -pcfsz)), 0.0, 1.0);
	blend += clamp (shadowmap (i, vec2 (0.0, pcfsz)), 0.0, 1.0);
	blend *= 0.2;
	return blend;
}

/* Lighting models. */
vec3 lightdirectional(in int i, in vec3 normal)
{
	vec3 lightdir = normalize (var_lightdir[i]);
	float lightcoeff = dot (normal, lightdir);
	float lightfront = step (0.0, lightcoeff);
	float blend = 1.0;

	/* Ambient component. */
	vec3 ambient = gl_FrontLightProduct[i].ambient.rgb;

	/* Diffuse component. */
	blend *= lightfront * lightcoeff;
	vec3 diffuse = blend * gl_FrontLightProduct[i].diffuse.rgb;

	/* Specular component. */
	float specrefl = max (0.0, dot (normal, var_lighthalfv[i]));
	float speccoeff = pow (specrefl, gl_FrontMaterial.shininess);
	vec3 specular = blend * speccoeff * gl_FrontLightProduct[i].specular.rgb;

	return ambient + diffuse + specular;
}
vec3 lightpointphong(in int i, in vec3 normal)
{
	vec3 lightdir = normalize (var_lightdir[i]);
	float lightdist = length (var_lightdir[i]);
	float lightcoeff = dot (normal, lightdir);
	float lightfront = step (0.0, lightcoeff);
	float blend = 1.0;

	/* Light attenuation. */
	float att = 1.0 / (
		gl_LightSource[i].constantAttenuation +
		gl_LightSource[i].linearAttenuation * lightdist +
		gl_LightSource[i].quadraticAttenuation * lightdist * lightdist);

	/* Ambient component. */
	vec3 ambient = gl_FrontLightProduct[i].ambient.rgb;

	/* Diffuse component. */
	blend *= lightfront * lightcoeff;
	vec3 diffuse = blend * gl_FrontLightProduct[i].diffuse.rgb;

	/* Specular component. */
	float specrefl = max (0.0, dot (normal, var_lighthalfv[i]));
	float speccoeff = pow (specrefl, gl_FrontMaterial.shininess);
	vec3 specular = blend * speccoeff * gl_FrontLightProduct[i].specular.rgb;

	return att * (ambient + diffuse + specular);
}
vec3 lightspotphong(in int i, in vec3 normal, in float blend)
{
	vec3 lightdir = normalize (var_lightdir[i]);
	vec3 lightspot = gl_LightSource[i].spotDirection;
	float lightcoeff = dot (normal, lightdir);
	float lightfront = step (0.0, lightcoeff);
	float lightdist = length (var_lightdir[i]);

	/* Light attenuation. */
	float att = 1.0 / (
		gl_LightSource[i].constantAttenuation +
		gl_LightSource[i].linearAttenuation * lightdist +
		gl_LightSource[i].quadraticAttenuation * lightdist * lightdist);

	/* Spotlight effect. */
	float spotcoeff = clamp (dot (lightspot, -lightdir), -1.0, 1.0);
	float spotcutoff = step (gl_LightSource[i].spotCosCutoff, 0.0);
	blend *= step (gl_LightSource[i].spotCosCutoff, spotcoeff);
	blend *= max (spotcutoff, pow (spotcoeff, gl_LightSource[i].spotExponent));

	/* Ambient component. */
	vec3 ambient = gl_FrontLightProduct[i].ambient.rgb;

	/* Diffuse component. */
	blend *= lightfront * lightcoeff;
	vec3 diffuse = blend * gl_FrontLightProduct[i].diffuse.rgb;

	/* Specular component. */
	float specrefl = max (0.0, dot (normal, var_lighthalfv[i]));
	float speccoeff = pow (specrefl, gl_FrontMaterial.shininess);
	vec3 specular = blend * speccoeff * gl_FrontLightProduct[i].specular.rgb;

	return att * (ambient + diffuse + specular);
}
vec4 lighting(in vec3 normal)
{
	int i;
	vec3 color = gl_FrontMaterial.ambient.rgb * gl_LightModel.ambient.rgb;

	for (i = lightcount - 1 ; i >= 0 ; --i)
	{
		if (uni_lighttype[i] == 1)
			color += lightdirectional(i, normal);
		else if (uni_lighttype[i] == 2)
			color += lightpointphong(i, normal);
		else if (uni_lighttype[i] == 3)
			color += lightspotphong(i, normal, 1.0);
		else if (uni_lighttype[i] == 4)
			color += lightspotphong(i, normal, shadowpcf (i));
	}

	return vec4 (color, 1.0);
}

/* Simplex noise. */
float simplexnoise(in vec3 P)
{
	/* Author: Stefan Gustavson ITN-LiTH (stegu@itn.liu.se) 2004-12-05
	 * Simplex indexing functions by Bill Licea-Kane, ATI (bill@ati.com)
	 *
	 * You may use, modify and redistribute this code free of charge,
	 * provided that the author's names and this notice appear intact.
	 */
#define ONE 0.00390625
#define ONEHALF 0.001953125
#define F3 0.333333333333
#define G3 0.166666666667
	float s = (P.x + P.y + P.z) * F3;
	vec3 Pi = floor(P + s);
	float t = (Pi.x + Pi.y + Pi.z) * G3;
	vec3 P0 = Pi - t;
	Pi = Pi * ONE + ONEHALF;
	vec3 Pf0 = P - P0;
	vec3 offset0;
	vec2 isX = step( Pf0.yz, Pf0.xx );
	offset0.x  = dot( isX, vec2( 1.0 ) );
	offset0.yz = 1.0 - isX;
	float isY = step( Pf0.z, Pf0.y );
	offset0.y += isY;
	offset0.z += 1.0 - isY;
	vec3 o2 = clamp(   offset0, 0.0, 1.0 );
	vec3 o1 = clamp( --offset0, 0.0, 1.0 );
	float perm0 = texture2D(uni_texturemap[1], Pi.xy).a;
	vec3  grad0 = texture2D(uni_texturemap[1], vec2(perm0, Pi.z)).rgb * 4.0 - 1.0;
	float t0 = 0.6 - dot(Pf0, Pf0);
	float n0 = step(0.0, t0);
	t0 *= t0;
	n0 *= t0 * t0 * dot(grad0, Pf0);
	vec3 Pf1 = Pf0 - o1 + G3;
	float perm1 = texture2D(uni_texturemap[1], Pi.xy + o1.xy*ONE).a;
	vec3  grad1 = texture2D(uni_texturemap[1], vec2(perm1, Pi.z + o1.z*ONE)).rgb * 4.0 - 1.0;
	float t1 = 0.6 - dot(Pf1, Pf1);
	float n1 = step(0.0, t1);
	t1 *= t1;
	n1 *= t1 * t1 * dot(grad1, Pf1);
	vec3 Pf2 = Pf0 - o2 + 2.0 * G3;
	float perm2 = texture2D(uni_texturemap[1], Pi.xy + o2.xy*ONE).a;
	vec3  grad2 = texture2D(uni_texturemap[1], vec2(perm2, Pi.z + o2.z*ONE)).rgb * 4.0 - 1.0;
	float t2 = 0.6 - dot(Pf2, Pf2);
	float n2 = step(0.0, t2);
	t2 *= t2;
	n2 *= t2 * t2 * dot(grad2, Pf2);
	vec3 Pf3 = Pf0 - vec3(1.0-3.0*G3);
	float perm3 = texture2D(uni_texturemap[1], Pi.xy + vec2(ONE, ONE)).a;
	vec3  grad3 = texture2D(uni_texturemap[1], vec2(perm3, Pi.z + ONE)).rgb * 4.0 - 1.0;
	float t3 = 0.6 - dot(Pf3, Pf3);
	float n3 = step(0.0, t3);
	t3 *= t3;
	n3 *= t3 * t3 * dot(grad3, Pf3);
	return 32.0 * (n0 + n1 + n2 + n3);
}

void main()
{
	vec3 x;
	vec3 y;
	float a;
	float b;

	/* Texture effect. */
	x = var_vertex.xyz + vec3(0.0, 0.1*uni_time, 0.0);
	y = var_vertex.yzx + vec3(0.0, -0.3*uni_time, 0.0);
	a = simplexnoise(1.0 * x) +
	    simplexnoise(2.0 * y);
	a = pow(2.7, a);
	x = var_vertex.xyz + vec3(0.0, -0.3*uni_time, -0.3*uni_time);
	y = var_vertex.yzx + vec3(0.0, -0.3*uni_time, -0.3*uni_time);
	b = simplexnoise(2.5 * vec3(x.x, 0.8*x.y, x.z)) +
	    simplexnoise(2.5 * vec3(x.x+0.05, 0.8*x.y, x.z+0.05));
	b = pow(4.0, b);
	vec4 texture =
		vec4(0.9+0.4*a,0.2+0.4*a,0.3*a,1.0) *
		vec4(0.2+b,b,b,1.0);

	vec3 normal = normalize (var_normal);

	gl_FragColor = var_color * texture * lighting (normal);
	gl_FragColor.a = var_color.a * texture.a;
}]]}

