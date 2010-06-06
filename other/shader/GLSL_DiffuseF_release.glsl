//GLSL shader which compute diffuse shading
//light position set in DiffuseShader.cpp
//ZJW 2/2010

//global variable that rarely change
varying vec3 normal;
varying vec3 vertex_to_light;
uniform vec3 EyePos;
varying vec3 vertex_to_eye;

void main()
{
	//normalize the interpolated normal and light
	vec3 normalVec = normalize(normal);

	//has no camera transform on light fine for this example
	vec3 lightVec = normalize(vertex_to_light);
	vec3 eyeVec = normalize(vertex_to_eye);

	//calculate the diffuse contribution
	float diffuse = clamp(dot(lightVec, normalVec), 0.0, 1.0);

	//blue diffuse material
	const vec4 diffuseMaterial = vec4(0.2, 0.2, 0.8, 1.0);

	//combine terms to make color
	gl_FragColor = diffuse*diffuseMaterial + 0.3*diffuseMaterial;

	/*now add a specular contribution */	
	vec3 h = lightVec + eyeVec;
	
	vec3 norm_h = normalize(h);
	
	gl_FragColor = gl_FragColor + 0.8*(pow(dot(normalVec,norm_h),100.0));
	
}
