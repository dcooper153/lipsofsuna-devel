Shader{
	name = "forward-envcube",
	config = [[
light-count 4
uniform uni_modelviewinverse MODELVIEWINVERSE
uniform uni_matrixnormal MATRIXNORMAL
uniform uni_cubemap[0] CUBETEXTURE0
uniform uni_texturemap[0] DIFFUSETEXTURE0
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixprojection MATRIXPROJECTION
uniform uni_shadowmap[0] SHADOWTEXTURE0
uniform uni_shadowmap[1] SHADOWTEXTURE1
uniform uni_shadowmap[2] SHADOWTEXTURE2
uniform uni_shadowmap[3] SHADOWTEXTURE3
uniform uni_lightmatrix[0] LIGHTMATRIX0
uniform uni_lightmatrix[1] LIGHTMATRIX1
uniform uni_lightmatrix[2] LIGHTMATRIX2
uniform uni_lightmatrix[3] LIGHTMATRIX3]],

	vertex = [[
#version 150
const int cubemapcount = 1;
const int lightcount = 4;
const int texturecount = 1;
varying vec4 var_color;
varying vec3 var_vertex;
varying vec3 var_normal;
varying vec3 var_reflect;
varying vec2 var_texcoord;
varying vec4 var_lightcoord[lightcount];
varying vec3 var_lightdir[lightcount];
uniform mat3 uni_matrixnormal;
uniform mat4 uni_modelviewinverse;
uniform mat4 uni_lightmatrix[lightcount];
void main()
{
	int i;
	vec4 tmp;
	/* Vertex attributes. */
	tmp = gl_ModelViewMatrix * gl_Vertex;
	var_color = gl_Color;
	var_vertex = tmp.xyz;
	var_normal = uni_matrixnormal * gl_Normal;
	var_texcoord = gl_MultiTexCoord0.xy;
	/* Environment mapping. */
	var_reflect = reflect(normalize(var_vertex), var_normal);
	var_reflect = mat3(uni_modelviewinverse) * var_reflect;
	/* Light sources. */
	for (i = 0 ; i < lightcount ; i++)
	{
		var_lightdir[i] = gl_LightSource[i].position.xyz - var_vertex * gl_LightSource[i].position.w;
		var_lightcoord[i] = uni_lightmatrix[i] * tmp;
	}
	gl_Position = ftransform();
}]],

	fragment = [[
#version 150
const int cubemapcount = 1;
const int lightcount = 4;
const int texturecount = 1;
varying vec4 var_color;
varying vec3 var_vertex;
varying vec3 var_normal;
varying vec3 var_reflect;
varying vec2 var_texcoord;
varying vec4 var_lightcoord[lightcount];
varying vec3 var_lightdir[lightcount];
uniform mat4 uni_modelviewinverse;
uniform mat4 uni_lightmatrix[lightcount];
uniform samplerCube uni_cubemap[cubemapcount];
uniform sampler2D uni_texturemap[texturecount];
uniform sampler2DShadow uni_shadowmap[lightcount];
/* FIXME: Should be configurable. */
const float envmapblend = 0.5;
/* Shadow mapping. */
float shadowmap(int i, vec2 pcf)
{
	float esm_c = 80.0;
	float esm_d = var_lightcoord[i].z;
	float esm_z = var_lightcoord[i].w * shadow2DProj (uni_shadowmap[i], var_lightcoord[i] + vec4 (pcf.xy, 0.0, 0.0)).r;
	float esm_f = exp (-esm_c * esm_d + esm_c * esm_z);
	return esm_f;
}
float shadowpcf(int i)
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
void main()
{
	int i;
	vec3 eye = normalize (-var_vertex);
	vec3 normal = normalize (var_normal);
	vec4 ambient = gl_FrontMaterial.ambient * gl_LightModel.ambient;
	vec4 diffuse = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 texture = texture2D (uni_texturemap[0], var_texcoord);
	vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
	/* Environment mapping. */
	vec4 envmap = textureCube (uni_cubemap[0], normalize(var_reflect));
	/* Lighting. */
	for (i = 0 ; i < lightcount ; i++)
	{
		/* Attenuation. */
		vec3 lightdir = normalize (var_lightdir[i]);
		float lightdist = length (var_lightdir[i]);
		float attenuation = 1.0 / (
			gl_LightSource[i].constantAttenuation +
			gl_LightSource[i].linearAttenuation * lightdist +
			gl_LightSource[i].quadraticAttenuation * lightdist * lightdist);
		/* Ambient component. */
		ambient += attenuation * gl_FrontLightProduct[i].ambient;
		/* Spotlight. */
		float spotcoeff = clamp (dot (gl_LightSource[i].spotDirection, -lightdir), -1.0, 1.0);
		if (spotcoeff >= gl_LightSource[i].spotCosCutoff)
		{
			/* Shadow mapping. */
			float blend = shadowpcf (i);
			/* Diffuse component. */
			blend *= clamp (dot (normal, lightdir), 0.0, 1.0);
			if (gl_LightSource[i].spotCosCutoff >= 0.0)
				blend *= pow (spotcoeff, gl_LightSource[i].spotExponent);
			diffuse += attenuation * blend * gl_FrontLightProduct[i].diffuse;
			/* Specular component. */
			if (dot (normal, lightdir) > 0.0)
			{
				vec3 lightrfl = reflect (-lightdir, normal);
				blend *= pow (max (0.0, dot (lightrfl, eye)), gl_FrontMaterial.shininess);
				specular += attenuation * blend * gl_FrontLightProduct[i].specular;
			}
		}
	}
	gl_FragColor = var_color * mix (texture * (ambient + diffuse + specular), envmap, envmapblend);
	gl_FragColor.a = var_color.a * texture.a;
}]]}
