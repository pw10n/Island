uniform sampler2D BumpTex;
uniform sampler2D DecalTex;
varying vec4 passcolor; // color from vertex shader
varying vec3 LightDir;  // lightdir from vertex shader

void main(){
   vec3 BumpNorm = vec3(texture2D(BumpTex, gl_TexCoord[0].xy));
   vec3 DecalCol = vec3(texture2D(DecalTex, gl_TexCoord[0].xy));

   BumpNorm = (BumpNorm -0.5) * 2.0;

   float NdotL = max(dot(BumpNorm, LightDir), 0.0);

   vec3 diffuse = NdotL * passcolor.xyz * DecalCol;

   gl_FragColor = vec4(diffuse, passcolor.w);
}

