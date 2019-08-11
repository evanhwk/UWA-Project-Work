attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexCoord;

attribute vec4 boneIDs;			// MODIFIED <2>
attribute vec4 boneWeights;		// MODIFIED <2>

varying vec4 vnorm;
varying vec4 vpos;
varying vec2 texCoord;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat4 boneTransforms[64];		// MODIFIED <2>

void main()
{
    mat4 boneTransform = boneWeights[0] * boneTransforms[int(boneIDs[0])] +
                        boneWeights[1] * boneTransforms[int(boneIDs[1])] +
                        boneWeights[2] * boneTransforms[int(boneIDs[2])] +
                        boneWeights[3] * boneTransforms[int(boneIDs[3])];

    // vec3 -> vec4 + Applying Transformation
    vpos = boneTransform * vec4(vPosition,1.0);
    vnorm = boneTransform * vec4(vNormal, 0.0);

    gl_Position = Projection * ModelView * vpos;
    texCoord = vTexCoord;
}
