#include "Angel.h"

#include <stdlib.h>
#include <dirent.h>
#include <time.h>

// Open Asset Importer header files (in ../../assimp--3.0.1270/include)
// This is a standard open source library for loading meshes, see gnatidread.h
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GLint windowHeight=640, windowWidth=960;

// gnatidread.cpp is the CITS3003 "Graphics n Animation Tool Interface & Data
// Reader" code.  This file contains parts of the code that you shouldn't need
// to modify (but, you can).
#include <cmath>
#include "gnatidread.h"
#include "gnatidread2.h"	// MODIFIED <2D> - Animation Header File [from addingAnimation.txt]
//#define M_PI 3.1415926535897932384626433832795 // Temp

using namespace std;        // Import the C++ standard functions (e.g., min) 


// IDs for the GLSL program and GLSL variables.
GLuint shaderProgram; // The number identifying the GLSL shader program
GLuint vPosition, vNormal, vTexCoord; // IDs for vshader input vars (from glGetAttribLocation)
GLuint projectionU, modelViewU; // IDs for uniform variables (from glGetUniformLocation)
// MODIFIED <2D> Animation
GLuint vBoneIDs, vBoneWeights, boneTransformsU; // Animation variables <QOL> uBoneTransforms -> boneTransformsU

static float viewDist = 4.5; // MODIFIED <1B> Distance from the camera to the centre of the scene 
static float camRotSidewaysDeg=0; // rotates the camera sideways around the centre
static float camRotUpAndOverDeg=20; // rotates the camera up and over the centre.
mat4 projection; // Projection matrix - set in the reshape function
mat4 view; // View matrix - set in the display function.

// These are used to set the window title
char lab[] = "Project1";
char *programName = NULL; // Set in main 
int numDisplayCalls = 0; // Used to calculate the number of frames per second

//------Meshes----------------------------------------------------------------
// Uses the type aiMesh from ../../assimp--3.0.1270/include/assimp/mesh.h
//                           (numMeshes is defined in gnatidread.h)
aiMesh* meshes[numMeshes]; // For each mesh we have a pointer to the mesh to draw
GLuint vaoIDs[numMeshes]; // and a corresponding VAO ID from glGenVertexArrays
const aiScene* scenes[numMeshes]; // MODIFIED <2D> - Scenes array for Animation [from addingAnimation.txt]

// -----Textures--------------------------------------------------------------
//                           (numTextures is defined in gnatidread.h)
texture* textures[numTextures]; // An array of texture pointers - see gnatidread.h
GLuint textureIDs[numTextures]; // Stores the IDs returned by glGenTextures

//------Scene Objects---------------------------------------------------------
//
// For each object in a scene we store the following
// Note: the following is exactly what the sample solution uses, you can do things differently if you want.
typedef struct {
    vec4 loc;
    float scale;
    float angles[3]; // rotations around X, Y and Z axes.
    float diffuse, specular, ambient; // Amount of each light component
    float shine;
    vec3 rgb;
    float brightness; // Multiplies all colours
    int meshId;
    int texId;
    float texScale;

    // MODIFIED <2D> - Animation variables
    // +-----------------------------------------------------------------------------------------+
    int animLen;	// Total number of frames in the animation
    float mDistance; 	// Movement distance
    float mSpeed; 	// Movement speed

    float poseTime;	// POSE_TIME variable (in drawMesh) to calculate which frame to display
    int isPaused = 0;	// 1 - object animation paused, 0 - animation Unpaused
    int animType = -1;		// determines type of animation, -1 = no animation, 0 = circular, 1 = back/forth
    // +-----------------------------------------------------------------------------------------+
    
} SceneObject;

const int maxObjects = 1024; // Scenes with more than 1024 objects seem unlikely

SceneObject sceneObjs[maxObjects]; // An array storing the objects currently in the scene.
int nObjects = 0;    // How many objects are currenly in the scene.
int currObject = -1; // The current object
int toolObj = -1;    // The object currently being modified

//----------------------------------------------------------------------------
//
// Loads a texture by number, and binds it for later use.    
void loadTextureIfNotAlreadyLoaded(int i)
{
    if (textures[i] != NULL) return; // The texture is already loaded.

    textures[i] = loadTextureNum(i); CheckError();
    glActiveTexture(GL_TEXTURE0); CheckError();

    // Based on: http://www.opengl.org/wiki/Common_Mistakes
    glBindTexture(GL_TEXTURE_2D, textureIDs[i]); CheckError();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textures[i]->width, textures[i]->height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, textures[i]->rgbData); CheckError();
    glGenerateMipmap(GL_TEXTURE_2D); CheckError();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); CheckError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); CheckError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); CheckError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); CheckError();

    glBindTexture(GL_TEXTURE_2D, 0); CheckError(); // Back to default texture
}

//------Mesh loading----------------------------------------------------------
//
// The following uses the Open Asset Importer library via loadMesh in 
// gnatidread.h to load models in .x format, including vertex positions, 
// normals, and texture coordinates.
// You shouldn't need to modify this - it's called from drawMesh below.

void loadMeshIfNotAlreadyLoaded(int meshNumber)
{
    if (meshNumber>numMeshes || meshNumber < 0) {	// <QOL> fixed boolean check
        printf("Error - no such model number\n");	// <QOL> "\n" added
	exit(1);
    }

    if (meshes[meshNumber] != NULL)
        return; // Already loaded

    // MODIFIED <2D> - Animation [from addingAnimation.txt]
    // +----------------------------------------+
    const aiScene* scene = loadScene(meshNumber);
    scenes[meshNumber] = scene;
    aiMesh* mesh = scene->mMeshes[0];
    meshes[meshNumber] = mesh;
    // +-----------------------------------------+

    glBindVertexArrayAPPLE( vaoIDs[meshNumber] );

    // Create and initialize a buffer object for positions and texture coordinates, initially empty.
    // mesh->mTextureCoords[0] has space for up to 3 dimensions, but we only need 2.
    GLuint buffer[1];
    glGenBuffers( 1, buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(float)*(3+3+3)*mesh->mNumVertices,
                  NULL, GL_STATIC_DRAW );

    int nVerts = mesh->mNumVertices;
    // Next, we load the position and texCoord data in parts.    
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(float)*3*nVerts, mesh->mVertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(float)*3*nVerts, sizeof(float)*3*nVerts, mesh->mTextureCoords[0] );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(float)*6*nVerts, sizeof(float)*3*nVerts, mesh->mNormals);

    // Load the element index data
    GLuint elements[mesh->mNumFaces*3];
    for (GLuint i=0; i < mesh->mNumFaces; i++) {
        elements[i*3] = mesh->mFaces[i].mIndices[0];
        elements[i*3+1] = mesh->mFaces[i].mIndices[1];
        elements[i*3+2] = mesh->mFaces[i].mIndices[2];
    }

    GLuint elementBufferId[1];
    glGenBuffers(1, elementBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh->mNumFaces * 3, elements, GL_STATIC_DRAW);

    // vPosition it actually 4D - the conversion sets the fourth dimension (i.e. w) to 1.0                 
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glEnableVertexAttribArray( vPosition );

    // vTexCoord is actually 2D - the third dimension is ignored (it's always 0.0)
    glVertexAttribPointer( vTexCoord, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(float)*3*mesh->mNumVertices) );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(float)*6*mesh->mNumVertices) );
    glEnableVertexAttribArray( vNormal );
    CheckError();

    // MODIFIED <2D> - Animation [from addingAnimation.txt]
    // +-----------------------------------------------------------------------------------------+
    // Get boneIDs and boneWeights for each vertex from the imported mesh data
    GLint boneIDs[mesh->mNumVertices][4];
    GLfloat boneWeights[mesh->mNumVertices][4];
    getBonesAffectingEachVertex(mesh, boneIDs, boneWeights);
    
    GLuint buffers[2];
    glGenBuffers( 2, buffers );
    
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]); CheckError();
    glBufferData(GL_ARRAY_BUFFER, sizeof(int) * 4 * mesh->mNumVertices, boneIDs, GL_STATIC_DRAW ); CheckError();
    glVertexAttribPointer(vBoneIDs, 4, GL_INT, GL_FALSE, 0, BUFFER_OFFSET(0)); CheckError();
    glEnableVertexAttribArray(vBoneIDs);
    CheckError();
    
    glBindBuffer( GL_ARRAY_BUFFER, buffers[1] );  CheckError();
    glBufferData( GL_ARRAY_BUFFER, sizeof(float) * 4 * mesh->mNumVertices, boneWeights, GL_STATIC_DRAW );  CheckError();
    glVertexAttribPointer(vBoneWeights, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));  CheckError();
    glEnableVertexAttribArray(vBoneWeights);
    CheckError();

    // +-----------------------------------------------------------------------------------------+
}

//----------------------------------------------------------------------------

static void mouseClickOrScroll(int button, int state, int x, int y)
{
    if (button==GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (glutGetModifiers()!=GLUT_ACTIVE_SHIFT) activateTool(button);
        else activateTool(GLUT_LEFT_BUTTON);
    }
    else if (button==GLUT_LEFT_BUTTON && state == GLUT_UP) deactivateTool();
    else if (button==GLUT_MIDDLE_BUTTON && state==GLUT_DOWN) { activateTool(button); }
    else if (button==GLUT_MIDDLE_BUTTON && state==GLUT_UP) deactivateTool();

    else if (button == 3) { // scroll up
        viewDist = (viewDist < 0.0 ? viewDist : viewDist*0.8) - 0.05;
    }
    else if (button == 4) { // scroll down
        viewDist = (viewDist < 0.0 ? viewDist : viewDist*1.25) + 0.05;
    }
}

//----------------------------------------------------------------------------

static void mousePassiveMotion(int x, int y)
{
    mouseX=x;
    mouseY=y;
}

//----------------------------------------------------------------------------

mat2 camRotZ()
{
    return rotZ(-camRotSidewaysDeg) * mat2(10.0, 0, 0, -10.0);
}

//------callback functions for doRotate below and lat[from addingAnimation.txt]er-----------------------

static void adjustCamrotsideViewdist(vec2 cv)
{
    cout << cv << endl;
    camRotSidewaysDeg+=cv[0]; viewDist+=cv[1];
}

static void adjustcamSideUp(vec2 su)
{
    camRotSidewaysDeg+=su[0]; camRotUpAndOverDeg+=su[1];
}
    
static void adjustLocXZ(vec2 xz)
{
    sceneObjs[toolObj].loc[0]+=xz[0]; sceneObjs[toolObj].loc[2]+=xz[1];
}

static void adjustScaleY(vec2 sy)
{
    sceneObjs[toolObj].scale+=sy[0]; sceneObjs[toolObj].loc[1]+=sy[1];
}


//----------------------------------------------------------------------------
//------Set the mouse buttons to rotate the camera----------------------------
//------around the centre of the scene.---------------------------------------
//----------------------------------------------------------------------------

static void doRotate()
{
    setToolCallbacks(adjustCamrotsideViewdist, mat2(400,0,0,-10),
                     adjustcamSideUp, mat2(400, 0, 0,-90) );
}
                                     
//------Add an object to the scene--------------------------------------------

static void addObject(int id)
{
    if (nObjects != maxObjects) {				// <QOL> Potential bug fix
    	vec2 currPos = currMouseXYworld(camRotSidewaysDeg);
    	sceneObjs[nObjects].loc[0] = currPos[0];
    	sceneObjs[nObjects].loc[1] = 0.0;
    	sceneObjs[nObjects].loc[2] = currPos[1];
    	sceneObjs[nObjects].loc[3] = 1.0;

    	if (id!=0 && id!=55)
        	sceneObjs[nObjects].scale = 0.005;

    	// MODIFIED <2D> Animation
    	// +-----------------------------------------------------------------------------------------+
    	if(id > 55) // IDs for objects with animations
    	{
		sceneObjs[nObjects].mSpeed = 1.0;
        	sceneObjs[nObjects].mDistance = 1.0;
		(id == 56) ? sceneObjs[nObjects].animLen = 39 : sceneObjs[nObjects].animLen = 23;
		(id == 56) ? sceneObjs[nObjects].animType = 1 : sceneObjs[nObjects].animType = 0;
	}
	// +-----------------------------------------------------------------------------------------+

    	sceneObjs[nObjects].rgb[0] = 0.7; sceneObjs[nObjects].rgb[1] = 0.7;
    	sceneObjs[nObjects].rgb[2] = 0.7; sceneObjs[nObjects].brightness = 1.0;

    	sceneObjs[nObjects].diffuse = 1.0; sceneObjs[nObjects].specular = 0.5;
    	sceneObjs[nObjects].ambient = 0.7; sceneObjs[nObjects].shine = 10.0;

    	sceneObjs[nObjects].angles[0] = 0.0; sceneObjs[nObjects].angles[1] = 180.0;
    	sceneObjs[nObjects].angles[2] = 0.0;

    	sceneObjs[nObjects].meshId = id;
    	sceneObjs[nObjects].texId = rand() % numTextures;
    	sceneObjs[nObjects].texScale = 2.0;

    	toolObj = currObject = nObjects++;
    	setToolCallbacks(adjustLocXZ, camRotZ(),
                     adjustScaleY, mat2(0.05, 0, 0, 10.0) );
    	glutPostRedisplay();
    }
}

// MODIFIED <1J> Removing and duplicating objects
// +-----------------------------------------------------------------------------------------+
// +----Remove object------------------------------------------------------------------------+
static void removeObject()
{
    if (nObjects > 3) {
    	for (int i = currObject; i < nObjects; i++)
		sceneObjs[i] = sceneObjs[i+1];
    	--nObjects;
	if (currObject >= nObjects-1 && currObject > 3) { 
		currObject = nObjects-1;
		toolObj = currObject;
	}
	doRotate();					// Set toolcallback to default camera rotate/move
	glutPostRedisplay();				// Redraw
    }
}

// +-----------------------------------------------------------------------------------------+
// +----Duplicate the current object------------------------------------------------------------+
static void duplicateObject()
{
    if (nObjects != maxObjects) {
    	sceneObjs[nObjects] = sceneObjs[currObject];	// Clone currObject

    	vec2 currPos = currMouseXYworld(camRotSidewaysDeg);
    	sceneObjs[nObjects].loc[0] = currPos[0];
    	sceneObjs[nObjects].loc[1] = 0.0;
    	sceneObjs[nObjects].loc[2] = currPos[1];
    	sceneObjs[nObjects].loc[3] = 1.0;		// Duplicate item at new location (Override)

    	toolObj = currObject = nObjects++;
    	setToolCallbacks(adjustLocXZ, camRotZ(),
    		    adjustScaleY, mat2(0.05, 0, 0, 10.0) );	// Set toolcallback to move object along X,Y,Z axis + Scaling
    	glutPostRedisplay();					// Redraw
    }
}
// +-----------------------------------------------------------------------------------------+

//------The init function-----------------------------------------------------

void init( void )
{
    srand ( time(NULL) ); /* initialize random seed - so the starting scene varies */
    aiInit();

    // for (int i=0; i < numMeshes; i++)
    //     meshes[i] = NULL;

    glGenVertexArraysAPPLE(numMeshes, vaoIDs); CheckError(); // Allocate vertex array objects for meshes
    glGenTextures(numTextures, textureIDs); CheckError(); // Allocate texture objects

    // Load shaders and use the resulting shader program
    shaderProgram = InitShader( "vStart.glsl", "fStart.glsl" );

    glUseProgram( shaderProgram ); CheckError();

    // Initialize the vertex position attribute from the vertex shader        
    vPosition = glGetAttribLocation( shaderProgram, "vPosition" );
    vNormal = glGetAttribLocation( shaderProgram, "vNormal" );
    // MODIFIED <2D> Animation
    // +------------------------------------------------------------------+
    vBoneIDs = glGetAttribLocation( shaderProgram, "boneIDs" ); // boneIDs in vstart.glsl
    vBoneWeights = glGetAttribLocation( shaderProgram, "boneWeights" ); // boneWeights in vstart.glsl
    CheckError();
    // +------------------------------------------------------------------+

    // Likewise, initialize the vertex texture coordinates attribute.    
    vTexCoord = glGetAttribLocation( shaderProgram, "vTexCoord" );
    CheckError();

    projectionU = glGetUniformLocation(shaderProgram, "Projection");
    modelViewU = glGetUniformLocation(shaderProgram, "ModelView");
    // MODIFIED <2D> Animation - boneTransforms in vstart.glsl
    boneTransformsU = glGetUniformLocation(shaderProgram, "boneTransforms"); 

    // Objects 0, and 1 are the ground and the first light.
    addObject(0); // Square for the ground
    sceneObjs[0].loc = vec4(0.0, 0.0, 0.0, 1.0);
    sceneObjs[0].scale = 10.0;
    sceneObjs[0].angles[0] = 90.0; // Rotate it.
    sceneObjs[0].texScale = 2.5; // Repeat the texture.

    addObject(55); // Sphere for the first light
    sceneObjs[1].loc = vec4(1.0, 1.0, 1.0, 1.0);
    sceneObjs[1].scale = 0.1;
    sceneObjs[1].texId = 0; // Plain texture
    sceneObjs[1].brightness = 0.5; // The light's brightness is 5 times this (below).

    // MODIFIED <1I> - Adding second *directional* light
    // +-----------------------------------------------------------------------+
    // Object 2 is the second light.
    addObject(55); // Sphere for second light
    sceneObjs[2].loc = vec4(-2.0, 2.0, -1.5, 0.0);		// w = 0.0
    sceneObjs[2].scale = 0.1;
    sceneObjs[2].texId = 0;	// Plain texture
    sceneObjs[2].brightness = 0.2;
    //  +----------------------------------------------------------------------+

    addObject(rand() % numMeshes); // A test mesh

    // We need to enable the depth test to discard fragments that
    // are behind previously drawn fragments for the same pixel.
    glEnable( GL_DEPTH_TEST );
    glShadeModel( GL_SMOOTH );	// <QOL> Explicit declaration of shading mode (although GL_SMOOTH is default)
    doRotate(); // Start in camera rotate mode.
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); /* black background */
}

//----------------------------------------------------------------------------

void drawMesh(SceneObject sceneObj)
{

    // Activate a texture, loading if needed.
    loadTextureIfNotAlreadyLoaded(sceneObj.texId);
    glActiveTexture(GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, textureIDs[sceneObj.texId]);

    // Texture 0 is the only texture type in this program, and is for the rgb
    // colour of the surface but there could be separate types for, e.g.,
    // specularity and normals.
    glUniform1i( glGetUniformLocation(shaderProgram, "texture"), 0 );

    // Set the texture scale for the shaders
    glUniform1f( glGetUniformLocation( shaderProgram, "texScale"), sceneObj.texScale );

    // Set the projection matrix for the shaders
    glUniformMatrix4fv( projectionU, 1, GL_TRUE, projection );

    // Set the model matrix - this should combine translation, rotation and scaling based on what's
    // in the sceneObj structure (see near the top of the program).
    //
    // Activate the VAO for a mesh, loading if needed.
    loadMeshIfNotAlreadyLoaded(sceneObj.meshId);
    CheckError();
    glBindVertexArrayAPPLE( vaoIDs[sceneObj.meshId] );
    CheckError();

    // MODIFIED <2D> - Animation Calculations
	// 2 components - calculating <POST_TIME> [animTime] variable + calculating model displacement from original position (back+forth motion)
	// +-------------------------------------------------------------------------------------------------------------------------------------+
    		if (sceneObj.meshId >= 56 && sceneObj.isPaused != 1) {	
    		
		// Avoiding math errors
            	if (sceneObj.mSpeed <= 0.0) sceneObj.mSpeed = 0.01;
		if (sceneObj.mDistance <= 0.0) sceneObj.mDistance = 0.01;
		
		// set currentTime [in ms]
	    	GLfloat currentTime = glutGet(GLUT_ELAPSED_TIME);

		// find animLen
		//double animLen = scenes[sceneObj.meshId]->mAnimations[0]->mDuration;
    		
		float runTime = (float) glutGet(GLUT_ELAPSED_TIME) / 1000.0;			// Time in seconds		
		float cycle = 2 * sceneObj.mDistance / sceneObj.mSpeed;			// Time for one cycle		
		float phase = fmod(runTime / cycle, 2 * M_PI);					// Run Time / Time For One period = No. Of Cycles
		
		// theta			0   3p/4  p/2  p/4   p   5p/4   3p/2   5p/4    	2p	% 2p
		// 1 - sin(theta)		1    0.5  0    0.5   1    1.5    2     1.5	1
		// Frame #			0    11   0    11    0    11     0     11       0
		//				 forwards   forwards      backwards     backwards
		// sin(theta)			0    0.5  1    0.5   0    -0.5   -1    -0.5     0
		// 			                           [max+]                     [min]
		
		//sceneObj.poseTime = fmod( (1 - sin(phase * M_PI)) / 2, 2 * M_PI) *  sceneObj.animLen;		// 1 walk per animation
		sceneObj.poseTime = fmod( (1 - sin(phase * M_PI)) * sceneObj.animLen, sceneObj.animLen);	// 2 walks per animation
		vec4 translation = 0.0;
		vec4 displacement = 0.0;
		translation.z = sceneObj.mDistance * fmod(sin(phase * M_PI), 2 * M_PI);				// SIN
		mat4 rotation =  RotateX(sceneObj.angles[0]) * RotateY(sceneObj.angles[1]) * RotateZ(sceneObj.angles[2]);
		if (sceneObj.animType == 1)
			rotation *= RotateX( fmod((float)(currentTime*0.05), 360.0) ) * RotateY( fmod((float)(currentTime*0.2), 360.0) );
    		
		displacement = 0.45 * rotation * translation;			
		sceneObj.loc += displacement;							// (add displacement to loc)
	}
	// +-------------------------------------------------------------------------------------------------------------------------------------+

    // MODIFIED <1B>  - Adding Rotation
    // +-----------------------------------------------------------------------------------------+
    mat4 rotate = (RotateX(sceneObj.angles[0]) * RotateY(sceneObj.angles[1]) * RotateZ(sceneObj.angles[2]));    
    mat4 model = Translate(sceneObj.loc) * rotate * Scale(sceneObj.scale);
    // +-----------------------------------------------------------------------------------------+

    // Set the model-view matrix for the shaders
    glUniformMatrix4fv( modelViewU, 1, GL_TRUE, view * model );


    // MODIFIED <2D> - Animation [from addingAnimation.txt]
    // +-----------------------------------------------------------------------------------------+
   	int nBones = meshes[sceneObj.meshId]->mNumBones;
    	if (nBones == 0) nBones = 1;

    	// If no bones, just a single identity matrix is used
    	// get boneTransforms for the first (0th) animation at the given time (a float measured in frames)
    	// (Replace <POSE_TIME> [animTime] appropriately with a float expression
    	// giving the time relative to the start of the animation,
    	// measured in frames, like the frame numbers in Blender.

	mat4 boneTransforms[nBones];
    	calculateAnimPose(meshes[sceneObj.meshId], scenes[sceneObj.meshId], 0,
        	              	sceneObj.poseTime, boneTransforms);
    	glUniformMatrix4fv(boneTransformsU, nBones, GL_TRUE,
        	               (const GLfloat *)boneTransforms);
    // +-----------------------------------------------------------------------------------------+
    //
    glDrawElements(GL_TRIANGLES, meshes[sceneObj.meshId]->mNumFaces * 3,
                   GL_UNSIGNED_INT, NULL);
    CheckError();
}

//------- Display function ------------------------------------------------------

void display( void )
{
    numDisplayCalls++;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    CheckError(); // May report a harmless GL_INVALID_OPERATION with GLEW on the first frame
      
    // Set the view matrix.  To start with this just moves the camera
    // backwards.  You'll need to add appropriate rotations.
    // MODIFIED <1A> - Adding Rotation
    // +----------------------------------- ---------------------------------------------------------------+
    mat4 rotate = RotateX(camRotUpAndOverDeg) * RotateY(-camRotSidewaysDeg);
    view = Translate( 0.0, 0.0, -viewDist) * rotate;
    // +--------------------------------------------------------------------------------------------------+

    // MODIFIED <1I> - Adding second light
    // +--------------------------------------------------------------------------------------------------+
    SceneObject lightObj1 = sceneObjs[1]; 
    vec4 lightPosition1 = view * lightObj1.loc;	    
    SceneObject lightObj2 = sceneObjs[2];
    vec4 lightPosition2 = view * lightObj2.loc;
	
    glUniform4fv( glGetUniformLocation(shaderProgram, "LightPosition1"), 1, lightPosition1);
    glUniform4fv( glGetUniformLocation(shaderProgram, "LightPosition2"), 1, lightPosition2);
    glUniform3fv( glGetUniformLocation(shaderProgram, "LightColor1"), 1, lightObj1.rgb);
    glUniform3fv( glGetUniformLocation(shaderProgram, "LightColor2"), 1, lightObj2.rgb);
    glUniform1f( glGetUniformLocation(shaderProgram, "LightBrightness1"), lightObj1.brightness);
    glUniform1f( glGetUniformLocation(shaderProgram, "LightBrightness2"), lightObj2.brightness);
    // +--------------------------------------------------------------------------------------------------+
    CheckError();

    for (int i = 0; i < nObjects; i++) {
        SceneObject so = sceneObjs[i];

        vec3 rgb = so.rgb * lightObj1.rgb * so.brightness * lightObj1.brightness * 2.0;
        glUniform3fv( glGetUniformLocation(shaderProgram, "AmbientProduct"), 1, so.ambient * rgb );
        CheckError();
        glUniform3fv( glGetUniformLocation(shaderProgram, "DiffuseProduct"), 1, so.diffuse * rgb );
        glUniform3fv( glGetUniformLocation(shaderProgram, "SpecularProduct"), 1, so.specular * rgb );
        glUniform1f( glGetUniformLocation(shaderProgram, "Shininess"), so.shine );
	
        CheckError();
	drawMesh(sceneObjs[i]);
    }
    glutSwapBuffers();
}

//----------------------------------------------------------------------------
//------Menus-----------------------------------------------------------------
//----------------------------------------------------------------------------

static void objectMenu(int id)
{
    deactivateTool();
    addObject(id);
}

static void texMenu(int id)
{
    deactivateTool();
    if (currObject>=0) {
        sceneObjs[currObject].texId = id;
        glutPostRedisplay();
    }
}

static void groundMenu(int id)
{
    deactivateTool();
    sceneObjs[0].texId = id;
    glutPostRedisplay();
}

static void adjustBrightnessY(vec2 by)
{
    sceneObjs[toolObj].brightness+=by[0];
    sceneObjs[toolObj].loc[1]+=by[1];
}

// MODIFIED <1C> - Adjust Diffuse/Specular/Brightness/Ambient
// New functions - adjustAmbientDiffuse() and adjustSpecularShine()
//  +---------------------------------------------------+
static void adjustAmbientDiffuse(vec2 by)
{
    sceneObjs[toolObj].ambient += by[0];
    sceneObjs[toolObj].diffuse += by[1];
}

static void adjustSpecularShine(vec2 by)
{
    float specularTemp = sceneObjs[toolObj].specular + by[0];
    float shineTemp = sceneObjs[toolObj].shine + by[1];
    sceneObjs[toolObj].specular = max( 0.0f, specularTemp);
    sceneObjs[toolObj].shine = max(0.0f,shineTemp);
}
// +---------------------------------------------------+

static void adjustRedGreen(vec2 rg)
{
    sceneObjs[toolObj].rgb[0]+=rg[0];
    sceneObjs[toolObj].rgb[1]+=rg[1];
}

static void adjustBlueBrightness(vec2 bl_br)
{
    sceneObjs[toolObj].rgb[2]+=bl_br[0];
    sceneObjs[toolObj].brightness+=bl_br[1];
}

static void lightMenu(int id)
{
    deactivateTool();
    if (id == 70) {
        toolObj = 1;
        setToolCallbacks(adjustLocXZ, camRotZ(),
                         adjustBrightnessY, mat2( 1.0, 0.0, 0.0, 10.0) );
    }
    else if (id >= 71 && id <= 74) {
        toolObj = 1;
        setToolCallbacks(adjustRedGreen, mat2(1.0, 0, 0, 1.0),
                         adjustBlueBrightness, mat2(1.0, 0, 0, 1.0) );
    }
    // MODIFIED <1I> Adding second light (Menu Items)
    // +----------------------------------------------------------------------+
    else if(id == 80) {
        toolObj = 2;
        setToolCallbacks(adjustLocXZ, camRotZ(),
                         adjustBrightnessY, mat2( 1.0, 0.0, 0.0, 10.0));
    }
    else if(id >= 81 && id <= 84){
        	toolObj = 2;
        	setToolCallbacks(adjustRedGreen, mat2(1.0, 0, 0, 1.0),
                         	adjustBlueBrightness, mat2(1.0, 0, 0, 1.0));
    }
    // +----------------------------------------------------------------------+
    else {
        printf("Error in lightMenu\n");
        exit(1);
    }
}

static int createArrayMenu(int size, const char menuEntries[][128], void(*menuFn)(int))
{
    int nSubMenus = (size-1)/10 + 1;
    int subMenus[nSubMenus];

    for (int i=0; i < nSubMenus; i++) {
        subMenus[i] = glutCreateMenu(menuFn);
        for (int j = i*10+1; j <= min(i*10+10, size); j++)
            glutAddMenuEntry( menuEntries[j-1] , j);
        CheckError();
    }
    int menuId = glutCreateMenu(menuFn);

    for (int i=0; i < nSubMenus; i++) {
        char num[6];
	sprintf(num, "%d-%d", i*10+1, min(i*10+10, size));
	glutAddSubMenu(num,subMenus[i]);
        CheckError();
    }
    return menuId;
}

static void materialMenu(int id)
{
    deactivateTool();
    if (currObject < 0) return;
    if (id==10) {
        toolObj = currObject;
        setToolCallbacks(adjustRedGreen, mat2(1, 0, 0, 1),
                         adjustBlueBrightness, mat2(1, 0, 0, 1) );
    }
    // You'll need to fill in the remaining menu items here.
    	// MODIFIED <1C> - Adjust diffuse/specular/ambient/shine
    	// +-------------------------------------------------------------+    	
	else if (id == 20) {
        	toolObj = currObject;
        	setToolCallbacks(adjustAmbientDiffuse, mat2(1, 0, 0, 1),
                         	adjustSpecularShine, mat2(5, 0, 0, 50) );
    	}
	// +-------------------------------------------------------------+
    else {
        printf("Error in materialMenu\n");
    }
}

static void adjustAngleYX(vec2 angle_yx)
{
    sceneObjs[currObject].angles[1]+=angle_yx[0];
    sceneObjs[currObject].angles[0]+=angle_yx[1];
}

static void adjustAngleZTexscale(vec2 az_ts)
{
    sceneObjs[currObject].angles[2]+=az_ts[0];
    sceneObjs[currObject].texScale+=az_ts[1];
}

// MODIFIED <2D> 
// Toolcallback function for animations (varying speed and distance)
// +---------------------------------------------------------------------+

static void adjustMSpeedDistance(vec2 by)
{
    if (sceneObjs[toolObj].animType == -1) return;
    float mSpeedTemp = sceneObjs[toolObj].mSpeed + by[0];
    float mDistanceTemp = sceneObjs[toolObj].mDistance + by[1];

    if (mSpeedTemp > 0.01 && mSpeedTemp < 10.0)
    	sceneObjs[toolObj].mSpeed = mSpeedTemp;
    if (mDistanceTemp > 0.5 && mDistanceTemp < 50.0)
    	sceneObjs[toolObj].mDistance = mDistanceTemp;
}

static void pauseAnimation()
{
	if (sceneObjs[toolObj].animType == -1) return;
	
	if (sceneObjs[currObject].isPaused== 0)
		sceneObjs[currObject].isPaused = 1;
	else 
		sceneObjs[currObject].isPaused = 0;
}

// +---------------------------------------------------------------------+

static void mainmenu(int id)
{
    deactivateTool();
    if (id == 41 && currObject>=0) {
    toolObj=currObject;
    setToolCallbacks(adjustLocXZ, camRotZ(),
    		adjustScaleY, mat2(0.05, 0, 0, 10) );
    }
    if (id == 50)
        doRotate();
    // MODIFIED <1B> - Rotation of Object about X Y and Z axis.
    // +------------------------------------------------------+
    if (id == 55 && currObject>=0) {
    setToolCallbacks(adjustAngleYX, mat2(400, 0, 0, -400),
		adjustAngleZTexscale, mat2(400, 0, 0, 15) );
    }
    // MODIFIED <2D> - Toolcallback for move speed and distance
    // +------------------------------------------------------+
    if (id >= 60 && currObject>=0) {
        toolObj=currObject;
        setToolCallbacks(adjustMSpeedDistance, mat2(1.0, 0, 0, 1.0),
                         adjustMSpeedDistance, mat2(5.0, 0, 0, 5.0));
    }
    if (id >= 61 && currObject>=0) {
	pauseAnimation();
    }

    if (id == 99) exit(0);
}

// MODIFIED <1J> - Removing and Duplicating Objects + Selecting Objects as current selection
// +-----------------------------------------------------+
static void nextObject()
{
	if (currObject+1 < nObjects) toolObj = ++currObject;
}

static void previousObject()
{
	if (currObject > 3) toolObj = --currObject;
}

static void selectMenu(int id)
{
    deactivateTool();
    if(id == 62)
    	removeObject();
    else if(id == 63)
    	duplicateObject();
    else if(id == 64)
    	previousObject();
    else if(id == 65)
	nextObject();
    else { 
	printf("Error in selectMenu\n");
        exit(1);
    }
}
// +-----------------------------------------------------+

static void makeMenu()
{
    int objectId = createArrayMenu(numMeshes, objectMenuEntries, objectMenu);
    
    int materialMenuId = glutCreateMenu(materialMenu);
    glutAddMenuEntry("R/G/B/All",10);
    glutAddMenuEntry("Ambient/Diffuse/Specular/Shine",20); // MODIFIED <1C> - Adjust diffuse/specular/ambient/shine

    int texMenuId = createArrayMenu(numTextures, textureMenuEntries, texMenu);
    int groundMenuId = createArrayMenu(numTextures, textureMenuEntries, groundMenu);

    int lightMenuId = glutCreateMenu(lightMenu);
    glutAddMenuEntry("Move Light 1",70);
    glutAddMenuEntry("R/G/B/All Light 1",71);
    glutAddMenuEntry("Move Light 2",80);
    glutAddMenuEntry("R/G/B/All Light 2",81);

    // MODIFIED <1J> Removing and Duplicating Objects
    // +-----------------------------------------------------+
    int selectMenuId = glutCreateMenu(selectMenu);
    glutAddMenuEntry("Remove Current Selection", 62);
    glutAddMenuEntry("Duplicate Current Selection", 63);
    glutAddMenuEntry("Select Next Object", 64);
    glutAddMenuEntry("Select Previous Object", 65);
    // +-----------------------------------------------------+

    glutCreateMenu(mainmenu);
    glutAddSubMenu("Current Selection", selectMenuId);
    glutAddMenuEntry("Rotate/Move Camera",50);
    glutAddSubMenu("Add Object", objectId);
    glutAddMenuEntry("Position/Scale", 41);
    glutAddMenuEntry("Rotation/Texture Scale", 55);
    glutAddMenuEntry("Animation Speed/Distance", 60);
    glutAddMenuEntry("Pause/Start Animation", 61);
    glutAddSubMenu("Material", materialMenuId);
    glutAddSubMenu("Texture",texMenuId);
    glutAddSubMenu("Ground Texture",groundMenuId);
    glutAddSubMenu("Lights",lightMenuId);
    glutAddMenuEntry("EXIT", 99);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void keyboard( unsigned char key, int x, int y )
{
    // MODIFIED <1K> <2D> - Keyboard Shortcuts
    // Additional Keyboard shortcuts for commonly used menu items
    // +------------------------------------------------------------------------+
    switch ( key ) {
    case 27:			// ESCAPE
        exit( EXIT_SUCCESS ); 
	break;
    case 127:			// BACKSPACE - Remove current object
	removeObject();
	break;
    case 32: 			// SPACE - Camera Rotate
	doRotate();
	break;
    case 113:			// Q - Select Previous Object
    	previousObject(); 
	break;			
    case 119:			// W - Camera Rotate
        doRotate(); 
	break;
    case 101:			// E - Select Next Object
	nextObject();
	break;
    case 114:			// R - Add random object
	addObject(rand() % numMeshes);
	break;
    case 116:			// T - Duplicate current object
	duplicateObject();
	break;
    case 122:			// Z - Position/Scale Object
	toolObj=currObject;
    	setToolCallbacks(adjustLocXZ, camRotZ(),
    		adjustScaleY, mat2(0.05, 0, 0, 10) );  
	break;
    case 120:			// X - Rotation/Texture Scale Object
	toolObj=currObject;
    	setToolCallbacks(adjustAngleYX, mat2(400, 0, 0, -400),
		adjustAngleZTexscale, mat2(400, 0, 0, 15));
	break;
    case 99:			// C - Ambient/Specular/Diffuse/Shine
	toolObj=currObject;
       	setToolCallbacks(adjustAmbientDiffuse, mat2(1, 0, 0, 1),
                         	adjustSpecularShine, mat2(5, 0, 0, 50) );
	break;
    case 118:			// V - Adjust RGB Values (Object)
	toolObj=currObject;
	setToolCallbacks(adjustRedGreen, mat2(1, 0, 0, 1),
                         adjustBlueBrightness, mat2(1, 0, 0, 1) );
	break;
    case 97:			// A - Move Light 1
	toolObj = 1;
        setToolCallbacks(adjustLocXZ, camRotZ(),
                         adjustBrightnessY, mat2( 1.0, 0.0, 0.0, 10.0));
	break;
    case 115: 			// S - Move Light 2
        toolObj = 2;
        setToolCallbacks(adjustLocXZ, camRotZ(),
                         adjustBrightnessY, mat2( 1.0, 0.0, 0.0, 10.0));
    	break;
    case 100:			// D - RGB/All Light 1
	toolObj = 1;
        setToolCallbacks(adjustRedGreen, mat2(1.0, 0, 0, 1.0),
                         adjustBlueBrightness, mat2(1.0, 0, 0, 1.0));
	break;
    case 102:			// F - RGB/All light 2
	toolObj = 2;
        	setToolCallbacks(adjustRedGreen, mat2(1.0, 0, 0, 1.0),
                         	adjustBlueBrightness, mat2(1.0, 0, 0, 1.0));
	break;
    // +------------------------------------------------------------------------+ 
    // MODIFIED <2D> - Animation Pausing
    // +------------------------------------------------------------------------+    
    case 109:
	toolObj=currObject;	// M - Adjust Animation Speed/Distance
        setToolCallbacks(adjustMSpeedDistance, mat2(1.0, 0, 0, 1.0),
                         adjustMSpeedDistance, mat2(5.0, 0, 0, 5.0) );
	break;
    case 13:			// Enter - Pause/Start Animation
        pauseAnimation();
	break;
    }
    // +------------------------------------------------------------------------+
}

void idle( void )
{
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void reshape( int width, int height )
{
    windowWidth = width;
    windowHeight = height;

    glViewport(0, 0, width, height);

    // You'll need to modify this so that the view is similar to that in the
    // sample solution.
    // In particular: 
    //     - the view should include "closer" visible objects (slightly tricky)
    //     - when the width is less than the height, the view should adjust so
    //         that the same part of the scene is visible across the width of
    //         the window.

    // MODIFIED <1D> - Adjusting near clipping plane distance
    //          <1E> - Adjusting reshape callback function to continue to show same objects when window size is modified
    // +-------------------------------------------------------------------------------------+
    GLfloat nearDist = 0.01;
    
    if (width < height) {
	projection = Frustum(-nearDist,nearDist,
			-nearDist*(float)height/(float)width,
                        nearDist*(float)height/(float)width,
			nearDist,100.0);
    }
    else {
     	 projection = Frustum(-nearDist*(float)width/(float)height,
			 nearDist*(float)width/(float)height,
                         -nearDist, nearDist,
                         nearDist, 100.0);
    }
    // +-------------------------------------------------------------------------------------+
}

//----------------------------------------------------------------------------

void timer(int unused)
{
    char title[256];
    // MODIFIED <1J> Additional Functionality - Selecting Objects
    // +-------------------------------------------------------------------------------------+
    sprintf(title, "%s %s: %d Frames Per Second @ %d x %d | Tool Object: <%i/%i> Current Object: <%i/%i>",
                    lab, programName, numDisplayCalls, windowWidth, windowHeight, toolObj, nObjects-1, currObject, nObjects-1);
    // +-------------------------------------------------------------------------------------+

    glutSetWindowTitle(title);

    numDisplayCalls = 0;
    glutTimerFunc(1000, timer, 1);
}

//----------------------------------------------------------------------------

char dirDefault1[] = "models-textures";
char dirDefault3[] = "/tmp/models-textures";
char dirDefault4[] = "/d/models-textures";
char dirDefault2[] = "/cslinux/examples/CITS3003/project-files/models-textures";

void fileErr(char* fileName)
{
    printf("Error reading file: %s\n", fileName);
    printf("When not in the CSSE labs, you will need to include the directory containing\n");
    printf("the models on the command line, or put it in the same folder as the exectutable.");
    exit(1);
}

//----------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
    // Get the program name, excluding the directory, for the window title
    programName = argv[0];
    for (char *cpointer = argv[0]; *cpointer != 0; cpointer++)
        if (*cpointer == '/' || *cpointer == '\\') programName = cpointer+1;

    // Set the models-textures directory, via the first argument or some handy defaults.
    if (argc > 1)
        strcpy(dataDir, argv[1]);
    else if (opendir(dirDefault1)) strcpy(dataDir, dirDefault1);
    else if (opendir(dirDefault2)) strcpy(dataDir, dirDefault2);
    else if (opendir(dirDefault3)) strcpy(dataDir, dirDefault3);
    else if (opendir(dirDefault4)) strcpy(dataDir, dirDefault4);
    else fileErr(dirDefault1);

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( windowWidth, windowHeight );

    // glutInitContextVersion( 3, 2);
    // glutInitContextProfile( GLUT_CORE_PROFILE );            // May cause issues, sigh, but you
    // glutInitContextProfile( GLUT_COMPATIBILITY_PROFILE ); // should still use only OpenGL 3.2 Core
                                                            // features.
    glutCreateWindow( "Initialising..." );

    // glewInit(); // With some old hardware yields GL_INVALID_ENUM, if so use glewExperimental.
    CheckError(); // This bug is explained at: http://www.opengl.org/wiki/OpenGL_Loading_Library
    
    init();
    CheckError();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutIdleFunc( idle );

    glutMouseFunc( mouseClickOrScroll );
    glutPassiveMotionFunc(mousePassiveMotion);
    glutMotionFunc( doToolUpdateXY );
 
    glutReshapeFunc( reshape );
    glutTimerFunc( 1000, timer, 1 );
    CheckError();

    makeMenu();
    CheckError();

    glutMainLoop();
    return 0;
}
