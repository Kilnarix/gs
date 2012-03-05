#version 120

/*
Attributes
*/

attribute vec3 InVertex;
attribute vec4 InTexCoord;
attribute vec3 InRGB;

//attribute int InNormal;

attribute vec4 InLightMatrix; //intensity for AO at each corner of quad

/*
Uniform
*/

uniform vec3 ChunkPosition;
uniform vec3 NormalArray[6];

/*
Varying
*/

varying vec2 texCoord;
varying vec2 texCoord2;

flat varying mat2 lightMatrix; 
varying vec3 inColor;

void main(void) 
{                      
        //vec3 Normal = NormalArray[inColor[4]*255];

        vec4 vertex = vec4(InVertex+ChunkPosition, 1.0);
        gl_Position = gl_ModelViewProjectionMatrix * vertex;
 
        inColor = InRGB;
 
        texCoord = InTexCoord.xy;

        texCoord2 = (0.0625f*InTexCoord.xy)+InTexCoord.zw;

        lightMatrix = mat2(InLightMatrix[0], InLightMatrix[1], InLightMatrix[2],InLightMatrix[3] );
 
}
