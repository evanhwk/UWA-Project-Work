// Phong Shading Model Example From Lecture 14
varying vec4 vnorm;
varying vec4 vpos; 
varying vec2 texCoord;

uniform vec3 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform vec4 LightPosition1, LightPosition2;
uniform vec3 LightColor1, LightColor2;
uniform float LightBrightness1, LightBrightness2;
uniform float Shininess;
uniform sampler2D texture;

void main()
{
// MODIFIED  <1G> - Moving lighting calculations into fragment shader

    // Transform vertex position into eye coordinates
    vec3 pos = (ModelView * vpos).xyz;

    // Transform vertex normal into eye coordinates
    vec3 fN = (ModelView * vnorm).xyz;
    vec3 fE = -pos;			
    vec3 fL1 = LightPosition1.xyz - pos;
    vec3 fL2 = LightPosition2.xyz;		// L2 is directional

    // Unit direction vectors for Blinn-Phong shading
    vec3 L1 = normalize(fL1);	
    vec3 L2 = normalize(fL2);		
    vec3 E = normalize(fE);			// UVector to the eye/camera
    vec3 H1 = normalize(L1 + E);		// Halfway Vectors
    vec3 H2 = normalize(L2 + E);	
    vec3 N = normalize(fN);			// Normal Vector
    
    // Compute terms in the illumination equation
    vec3 ambient1 = (LightColor1 * LightBrightness1) * AmbientProduct;
    vec3 ambient2 = (LightColor2 * LightBrightness2) * AmbientProduct;
    
    float Kd1 = max(dot(L1, N), 0.0);
    float Kd2 = max(dot(L2, N), 0.0);

    vec3 diffuse1 = Kd1 * (LightColor1 * LightBrightness1) * DiffuseProduct;
    vec3 diffuse2 = Kd2 * (LightColor2 * LightBrightness2) * DiffuseProduct;
    
    // MODIFIED <1H> - Specular componant always shines towards white
    float Ks1 = pow(max(dot(N, H1), 0.0), Shininess);
    float Ks2 = pow(max(dot(N, H2), 0.0), Shininess);

    vec3 specular1 = Ks1 * (LightBrightness1) * SpecularProduct;
    vec3 specular2 = Ks2 * (LightBrightness2) * SpecularProduct;
    
    if(dot(L1,N) < 0.0)
    {
        specular1 = vec3( 0.0, 0.0, 0.0);
    }
    if(dot(L2,N) < 0.0)
    {
        specular2 = vec3( 0.0, 0.0, 0.0);
    }
    
    // globalAmbient is independent of distance from the light source
    vec3 globalAmbient = vec3(0.1, 0.1, 0.1);
    
    float atten1 = 0.01 + length(fL1);
    float atten2 = 0.01 + length(fL2);

    vec3 color1 = globalAmbient + (ambient1 + diffuse1 + specular1) / atten1;
    vec3 color2 = globalAmbient + (ambient2 + diffuse2 + specular2) / atten2;
    
    vec4 color = vec4(color1+color2, 1.0);

    gl_FragColor = color * texture2D( texture, texCoord * 2.0 );
}
