// light mapping pixel shader

//textures seem to bind 1 to 1 with texture units
uniform sampler2D ltex;
uniform sampler2D dtex;
uniform float red;

void main()
{

  	vec4 lightTex = texture2D(ltex, gl_TexCoord[0].st);

	vec4 decalTex = texture2D(dtex, gl_TexCoord[0].st);

        if (decalTex.r < red && decalTex.g < red && decalTex.b < red) {
          decalTex.r  = 0.8;
        }
	vec4 color = lightTex*decalTex;

	gl_FragColor = color;
}

