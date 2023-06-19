#version 460 core
out vec4 FragColor;
  

uniform sampler2D _texture;
uniform ivec2 res;
uniform int Board[64];
uniform int Highlight[64];

vec4 MyBlend(vec4 source, vec4 dest, float alpha)
{
    return source * alpha + dest * (1.0-alpha);
}


void main()
{

    ivec2 tile = ivec2((gl_FragCoord.xy/res)*8);
    vec2 tile_f = vec2((gl_FragCoord.xy/res)*8);
    int piece = Board[tile.y*8+tile.x];
    if(piece != 0){
    FragColor = texture(_texture,(tile_f-tile)/vec2(6,2)+vec2(piece*sign(piece)-1,(-sign(piece)+1)/2)/vec2(6,2));
    }
    FragColor = mix(FragColor,vec4(0.1, 1.0,0.1,0.5)*Highlight[tile.y*8+tile.x],Highlight[tile.y*8+tile.x]*0.7);
} 