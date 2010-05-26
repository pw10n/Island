// vertex shader for light mapping

//pass through the texture coordinates

void main()
{

  //transform the vertex to clip space
  gl_Position = ftransform();

  //pass through the texture coordinates
  gl_TexCoord[0] = gl_MultiTexCoord0;

}
