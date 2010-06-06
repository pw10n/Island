//GLSL shader which compute diffuse shading
//light position set in DiffuseShader.cpp
//ZJW 2/2010

//global variable that rarely change
varying vec3 normal;
varying vec3 vertex_to_light;
uniform vec3 EyePos;
varying vec3 vertex_to_eye;
uniform vec3 LightPos;

void main()
{

	// transform the vertex into world space- required
	vec4 pos = gl_ModelViewMatrix*gl_Vertex;
	
	//xform normal from model-space to view-space
	//don't bother to normalize will be passed through
	normal = gl_NormalMatrix*gl_Normal;

	//has no camera transform on light fine for this example
	vertex_to_light = vec3(LightPos.x-pos.x, LightPos.y-pos.y, LightPos.z-pos.z);
	vertex_to_eye = vec3(EyePos.x-pos.x, EyePos.y-pos.y, EyePos.z-pos.z);

	//full transform required
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	
}
