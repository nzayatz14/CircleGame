attribute vec4 vPosition;
uniform mat4 p;
uniform mat4 mv;

uniform float scale;  //the scale 
uniform vec2 pos;  //the location of circle 

void
main()
{
    //these have to be in column major order.
   mat4 S = mat4(scale, 0.0, 0.0, 0.0,
                 0.0, scale, 0.0, 0.0, 
                 0.0, 0.0, 1.0, 0.0, 
                 0.0, 0.0, 0.0, 1.0);
   mat4 T = mat4(1.0, 0.0, 0.0, 0.0,
                 0.0, 1.0, 0.0, 0.0, 
                 0.0, 0.0, 1.0, 0.0, 
                 pos.x, pos.y, 0.0, 1.0);
   gl_Position =  p* T* S * vPosition;
   
}
