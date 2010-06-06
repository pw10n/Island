//GLSL shader which compute diffuse shading
//light position set in DiffuseShader.cpp
//ZJW 2/2010

//global variable that rarely change
uniform vec3 LightPos;
uniform vec3 EyePos;

void main()
{

	// transform the vertex into world space- required
	vec4 pos = gl_ModelViewMatrix*gl_Vertex;
	vec3 pos3 = vec3(pos[0], pos[1], pos[2]);
	
	//xform normal from model-space to view-space
	vec3 normalVec = normalize(gl_NormalMatrix*gl_Normal);

	//has no camera transform on light fine for this example
	vec3 lightVec = normalize(vec3(LightPos.x-pos.x, LightPos.y-pos.y, LightPos.z-pos.z));

	//calculate the diffuse contribution
	float diffuse = max(dot(lightVec, normalVec), 0.0);

	//blue diffuse material
	vec4 diffuseMaterial = vec4(0.2, 0.2, 0.8, 1.0);
	//vec4 diffuseMaterial = vec4(0.0, 0.8, 0.2, 1.0);
	
	vec3 _H = lightVec + (EyePos - pos3);
	
	vec3 H = normalize(_H);
	
	float spec = (max(dot(normalVec,H),0.0))*(max(dot(normalVec,H),0.0));
	

	//combine terms to make color
	gl_FrontColor = diffuse*diffuseMaterial + 0.3*diffuseMaterial + 0.5*spec*vec4(0.5,0.5,0.5,1.0);

	//full transform required
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	
}
