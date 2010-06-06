

varying vec4 passcolor;
varying vec3 LightDir;
attribute vec3 tanget;
attribute vec3 binormal;
uniform vec3 lightdir;

void main(){

   passcolor = gl_Color;
   gl_Position = ftransform();
   mat3 rotmat = mat3(tanget, binormal, gl_Normal);
   LightDir = rotmat * normalize(lightdir);
   normalize(LightDir);
   gl_TexCoord[0] = gl_MultiTexCoord0;
}

