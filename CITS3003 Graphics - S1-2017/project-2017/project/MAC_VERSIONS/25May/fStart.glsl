/*
varying vec4 color;
varying vec2 texCoord;  // The third coordinate is always 0.0 and is discarded

uniform sampler2D texture;

void main()
{
    gl_FragColor = color * texture2D( texture, texCoord * 2.0 );
}
*/

// Phong Shading Model
// From Lecture 14
varying vec3 fN;
varying vec3 fL1;
varying vec3 fL2;
varying vec3 fE;
varying vec2 texCoord;
uniform vec3 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform vec4 LightPosition1;
uniform vec4 LightPosition2;
uniform float Shininess;
uniform sampler2D texture;

void main()
{
    // Unit direction vectors for Blinn-Phong shading calculation
    vec3 L1 = normalize(fL1 - fE);		// UVector to the light source 1
    vec3 L2 = normalize(fL2 - fE);		// UVector to light source 2
    vec3 E = normalize(fE);			// UVector to the eye/camera
    vec3 H1 = normalize(L1 + E);		// Halfway Vector (Source 1)
    vec3 H2 = normalize(L2 + E);		// Halfway Vector (Source 2)
    vec3 N = normalize(fN);			// Normal Vector

    // Compute terms in the illumination equation
    vec3 ambient = AmbientProduct;
    
    float Kd1 = max(dot(L1, N), 0.0);
    float Kd2 = max(dot(L2, N), 0.0);

    vec3 diffuse1 = Kd1 * DiffuseProduct;
    vec3 diffuse2 = Kd2 * DiffuseProduct;
    
    float Ks1 = pow(max(dot(N, H1), 0.0), Shininess);
    float Ks2 = pow(max(dot(N, H2), 0.0), Shininess);

    vec3 specular1 = Ks1 * SpecularProduct;
    vec3 specular2 = Ks2 * SpecularProduct;
    
    if(dot(L1,N) < 0.0)
    {
        specular1 = vec3( 0.0, 0.0, 0.0);
    }
    
    if(dot(L2,N) < 0.0)
    {
        specular2 = vec3( 0.0, 0.0, 0.0);
    }
    
    //!!!!!!!!!!! Adjusting rgb values via a weighted average following luminosity standards for human vision, to create equivalent grayscale light for specular element.
    float temp1 = 0.2989 * specular1.x + 0.5870 * specular1.y + 0.1140 * specular1.z;
    specular1.xyz = vec3(temp1,temp1,temp1);
    
    float temp2 = 0.2989 * specular2.x + 0.5870 * specular2.y + 0.1140 * specular2.z;
    specular2.xyz = vec3(temp2,temp2,temp2);
    
    // globalAmbient is independent of distance from the light source
    vec3 globalAmbient = vec3(0.1, 0.1, 0.1);
    
    // Fade brightness by distance
    float atten1 = ( 0.3*length(fL1 ) + 0.2*length(fL1) * length(fL1) );
    float atten2 =  ( 0.3*length(fL2) + 0.2*length(fL2) * length(fL2) );
    //float atten = 0.5;

    vec4 color;
    //vec3 Light1 = diffuse1.xyz + specular1.xyz;
    //vec3 Light2 = diffuse2.xyz + specular2.xyz;

    color.rgb = globalAmbient + ambient;
    color.rgb = color.rgb + (diffuse1 + specular1)/atten1;
    color.rgb = color.rgb + (diffuse2 + specular2)/atten2;
    color.a = 1.0;

    gl_FragColor = color * texture2D( texture, texCoord * 2.0 );
}
