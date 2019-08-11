attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexCoord;

attribute vec4 boneIDs;			// MODIFIED <2>
attribute vec4 boneWeights;		// MODIFIED <2>

// From Lecture 14
// Output values that will be interpolated per-fragment by the rasterizer further down the pipeline
varying vec3 fN;
varying vec3 fE;
varying vec3 fL1, fL2;
varying vec2 texCoord;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition1, LightPosition2;	// MODIFIED <1J>

uniform mat4 boneTransforms[64];		// MODIFIED <2>

void main()
{
    // From Lecture
    mat4 boneTransform = boneWeights[0] * boneTransforms[int(boneIDs[0])] +
                        boneWeights[1] * boneTransforms[int(boneIDs[1])] +
                        boneWeights[2] * boneTransforms[int(boneIDs[2])] +
                        boneWeights[3] * boneTransforms[int(boneIDs[3])];

    // vec3 -> vec4 + Applying Transformation
    vec4 vpos = boneTransform * vec4(vPosition,1.0);
    vec4 vnorm = mat3(boneTransform) * vec4(vNormal, 1.0);

    // Transform vertex position into eye coordinates
    vec3 pos = (boneTransform * ModelView * vpos).xyz;

    // Calculation of "varying" output to fragment shader
    fN = (ModelView * vnorm).xyz;	// Normal Vector in eye coordinates (assumes scaling is uniform across dimensions)
    fE = -pos;				// Vector to the eye/camera
    
    if (LightPosition1.w == 0.0)	// Vector to the light source 1 (if w==0, directional)
	fL1 = LightPosition1.xyz;
    else
 	fL1 = LightPosition1.xyz - pos;

    if (LightPosition2.w == 0.0)	// Vector to the light source 1 (if w==0, directional)
	fL2 = LightPosition2.xyz;
    else
 	fL2 = LightPosition2.xyz - pos;

    gl_Position = Projection * ModelView * vpos;
    texCoord = vTexCoord;
}
