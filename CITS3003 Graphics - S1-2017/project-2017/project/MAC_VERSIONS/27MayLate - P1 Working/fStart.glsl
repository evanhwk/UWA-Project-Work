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
    vec3 fL1 = LightPosition1.xyz - (LightPosition1.w * pos);
    vec3 fL2 = LightPosition2.xyz - (LightPosition2.w * pos);		// w = 0.0 Therefore fL2 = LightPosition.xyz;

    // Unit direction vectors for Blinn-Phong shading
    vec3 L1 = normalize(fL1);	
    vec3 L2 = normalize(fL2);		
    vec3 E = normalize(fE);			// UVector to the eye/camera
    vec3 H1 = normalize(L1 + E);		// Halfway Vectors
    vec3 H2 = normalize(L2 + E);	
    vec3 N = normalize(fN);			// Normal Vector

    // Combine light brightness and rgb values
    vec3 LightCB1 = LightColor1 * LightBrightness1;
    vec3 LightCB2 = LightColor2 * LightBrightness2;
    
    // Compute terms in the illumination equation
    vec3 ambient1 = LightCB1 * AmbientProduct;
    vec3 ambient2 = LightCB2 * AmbientProduct;
    
    float Kd1 = max(dot(L1, N), 0.0);
    float Kd2 = max(dot(L2, N), 0.0);

    vec3 diffuse1 = Kd1 * LightCB1 * DiffuseProduct;
    vec3 diffuse2 = Kd2 * LightCB2 * DiffuseProduct;
    
    // MODIFIED <1H> - Specular componant shines white
    float Ks1 = pow(max(dot(N, H1), 0.0), Shininess);
    float Ks2 = pow(max(dot(N, H2), 0.0), Shininess);

    vec3 specular1 = Ks1 * LightCB1 * SpecularProduct;
    vec3 specular2 = Ks2 * LightCB2 * SpecularProduct;
    
    if(dot(L1,N) < 0.0) {
        specular1 = vec3( 0.0, 0.0, 0.0);
    }
    if(dot(L2,N) < 0.0) {
        specular2 = vec3( 0.0, 0.0, 0.0);
    }
    
    // globalAmbient is independent of distance from the light source
    vec3 globalAmbient = vec3(0.1, 0.1, 0.1);

    // MODIFIED <1F> Light reduces with distance
    // attenuation follows inverse square law for lighting falloff (1/len^2)

    float atten1 = 1.0;

    if (LightPosition1.w != 0.0) {
    	float len1 = length(fL1);
	if (len1 > 0.0)
		atten1 = len1 * len1;
	else
 		atten1 = 0.01;
    }

    float atten2 = 1.0;

    if (LightPosition2.w != 0.0) {
    	float len2 = length(fL2);
    	if (len2 > 0.0)
        	atten2 = len2 * len2;
	else
		atten2 = 0.01;
    }

    // MODIFIED <1H> - Specular componant shines white
    vec3 color1 = globalAmbient + (ambient1 + diffuse1) / atten1;
    vec3 color2 = globalAmbient + (ambient2 + diffuse2) / atten2;
    vec4 sp = vec4(specular1 / atten1 + specular2/atten2, 1.0);

    vec4 fcolor = vec4(color1+color2, 1.0) * texture2D( texture, texCoord * 2.0);
    vec4 color = fcolor + sp;

    gl_FragColor = color;
}
