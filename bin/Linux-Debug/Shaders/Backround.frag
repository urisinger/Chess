#version 460 core
out vec4 FragColor;
  
uniform ivec2 res;
uniform vec4 col1;
uniform vec4 col2;

void main()
{

    ivec2 tile = ivec2((gl_FragCoord.xy/res)*8);
    FragColor = vec4(((tile.y*8+tile.x+tile.y%2)%2)*col2+(((tile.y*8+tile.x+tile.y%2)%2)-1)*-col1);
} 