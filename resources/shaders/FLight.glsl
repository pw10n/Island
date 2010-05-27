// light mapping pixel shader

//textures seem to bind 1 to 1 with texture units
uniform sampler2D ltex;
uniform sampler2D dtex;
uniform float red;

void main()
{

  	vec4 lightTex = texture2D(ltex, gl_TexCoord[0].st);

	vec4 decalTex = texture2D(dtex, gl_TexCoord[0].st);

        
    decalTex.b = 1.0;
    decalTex.g = 0.0;
    decalTex.r = 0.0;
        
	//vec4 color = lightTex*decalTex;
	vec4 color = decalTex;
	gl_FragColor = color;
}

