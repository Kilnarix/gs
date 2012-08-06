#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#ifdef GL_EXT_gpu_shader4
    flat varying mat2 lightMatrix; 
#else
    varying mat2 lightMatrix; 
#endif

varying vec2 texCoord;
//varying vec2 texCoord2;
varying vec2 texCoord3;
 
varying vec3 inColor;
 
//uniform sampler2DArray base_texture;
uniform sampler2D base_texture;

void main() 
{
	//float gamma = 2.2f;

	vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
	vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);

	float tmp = dot(vx, lightMatrix * vy);

 
    
    vec3 color = (tmp*texture2D(base_texture, texCoord3).rgb);
    //vec3 color = (tmp*texture2D(base_texture, texCoord).rgb);

    //vec3 color = tmp*vec3(1.0,0.0,0.0);

	//float intensity = (inColor.r + inColor.g + inColor.b) / 3.0f;
	//vec3 color2 = vec3(inColor.r/intensity, inColor.g/intensity, inColor.b/intensity);
	float intensity = 3.0f / (inColor.r + inColor.g + inColor.b);
	vec3 color2 = intensity*inColor;	
    color = color2 * color;

	gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
	//gl_FragColor.rgb = vec3(1.0,0,0);

}


/*
void main() 
{
        //float gamma = 2.2f;
 
        vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
        vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);
 
        float tmp = dot(vx, lightMatrix0 * vy);
       

	//float att  = tmp / float(inColor.r + inColor.g + inColor.b);
	//float intensity = (inColor.r + inColor.g + inColor.b) / att;
	//intensity /= att;


	//float intensity = (inColor.r + inColor.g + inColor.b) / ( tmp / float(inColor.r + inColor.g + inColor.b) )

        vec3 color = tmp*(texture2DArray(base_texture, texCoord.xyz).rgb);      

        vec3 color2 = vec3(inColor.r/intensity, inColor.g/intensity, inColor.b/intensity);
        
        color = color2 * color;
        gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
}
*/

/*
varying vec3 texCoord;
varying mat2 lightMatrix0;

varying vec4 inColor;

uniform sampler2DArray base_texture;
*/


