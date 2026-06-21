///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";

	const char* g_OverlayTextureName = "overlayTexture";        // For the second texture sampler
	const char* g_OverlayStrengthName = "overlayStrength";      // For blend strength (0.0 to 1.0)
	const char* g_UseOverlayTextureName = "bUseOverlayTexture"; // To toggle overlay in shader
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	stbi_set_flip_vertically_on_load(true);

	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set the texture wrapping parameters based on the tag
		// For the overlay texture, use GL_CLAMP_TO_EDGE ---
		if (tag == "label_overlay")
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else // For other textures (like the base candle wax), use GL_REPEAT
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		// set texture filtering params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (colorChannels == 3)
 			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/


/***********************************************************
 * SetShaderOverlayTexture()
 *
 * This method is used for setting the overlay texture data
 * associated with the passed in tag and its strength into the shader.
 ***********************************************************/
void SceneManager::SetOverlayTexture(std::string overlayTextureTag, float strength)
{
	if (NULL != m_pShaderManager)
	{
		// Activate GL_TEXTURE1 for the overlay texture
		glActiveTexture(GL_TEXTURE1);

		// Get the texture ID for the overlay
		int overlayTextureID = FindTextureID(overlayTextureTag);

		// Bind the overlay texture
		glBindTexture(GL_TEXTURE_2D, overlayTextureID);

		// Tell the shader's 'overlayTexture' uniform to sample from GL_TEXTURE1
		m_pShaderManager->setSampler2DValue(g_OverlayTextureName, 1);

		// Set the blend strength for the overlay
		m_pShaderManager->setFloatValue(g_OverlayStrengthName, strength);

		// Enable the overlay texture usage in the shader
		m_pShaderManager->setIntValue(g_UseOverlayTextureName, true);
	}
}

/***********************************************************
 * SetShaderOverlayUVTransform()
 *
 * This method is used for setting the overlay texture UV scale
 * and offset values into the shader.
 ***********************************************************/
void SceneManager::SetShaderOverlayUVTransform(glm::vec2 scale, glm::vec2 offset)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("overlayUVScale", scale);
		m_pShaderManager->setVec2Value("overlayUVOffset", offset);
	}
}
/***********************************************************
* DefineObjectMaterials()
 *
* This method is used for configuring the various material
* settings for all of the objects in the 3D scene.
***********************************************************/
void SceneManager::DefineObjectMaterials()
{
	OBJECT_MATERIAL goldMaterial;
	goldMaterial.diffuseColor = glm::vec3(0.3f, 0.3f, 0.2f);
	goldMaterial.specularColor = glm::vec3(0.6f, 0.5f, 0.4f);
	goldMaterial.shininess = 22.0;
	goldMaterial.tag = "gold";
	m_objectMaterials.push_back(goldMaterial);
	OBJECT_MATERIAL cementMaterial;
	cementMaterial.diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
	cementMaterial.specularColor = glm::vec3(0.4f, 0.4f, 0.4f);
	cementMaterial.shininess = 0.5;
	cementMaterial.tag = "cement";
	m_objectMaterials.push_back(cementMaterial);
	OBJECT_MATERIAL woodMaterial;
	woodMaterial.diffuseColor = glm::vec3(0.1f, 0.1f, 0.1f);
	woodMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
	woodMaterial.shininess = 0.1;
	woodMaterial.tag = "wood";
	m_objectMaterials.push_back(woodMaterial);
	OBJECT_MATERIAL tileMaterial;
	tileMaterial.diffuseColor = glm::vec3(0.3f, 0.2f, 0.1f);
	tileMaterial.specularColor = glm::vec3(0.4f, 0.5f, 0.6f);
	tileMaterial.shininess = 25.0;
	tileMaterial.tag = "tile";
	m_objectMaterials.push_back(tileMaterial);
	OBJECT_MATERIAL glassMaterial;
	glassMaterial.diffuseColor = glm::vec3(0.3f, 0.3f, 0.3f);
	glassMaterial.specularColor = glm::vec3(0.6f, 0.6f, 0.6f);
	glassMaterial.shininess = 85.0;
	glassMaterial.tag = "glass";
	m_objectMaterials.push_back(glassMaterial);
	OBJECT_MATERIAL clayMaterial;
	clayMaterial.diffuseColor = glm::vec3(0.4f, 0.4f, 0.5f);
	clayMaterial.specularColor = glm::vec3(0.2f, 0.2f, 0.4f);
	clayMaterial.shininess = 0.5;
	clayMaterial.tag = "clay";
	m_objectMaterials.push_back(clayMaterial);
}

/***********************************************************
* SetupSceneLights()
 *
* This method is used for configuring the various scene lights
* settings for all of the lights in the 3D scene.
***********************************************************/
void SceneManager::SetupSceneLights()
{
	//Setup Point Light
	m_pShaderManager->setVec3Value("pointLights[0].position", 3.0f, 14.0f, -6.0f); // put in position
	m_pShaderManager->setVec3Value("pointLights[0].ambient", 0.5f, 0.5f, 0.5f);
	m_pShaderManager->setVec3Value("pointLights[0].diffuse", 1.0f, 0.0f, 1.0f);    // Brighter diffuse
	m_pShaderManager->setVec3Value("pointLights[0].specular", 1.0f, 1.0f, 1.0f);   // Strong specular
	m_pShaderManager->setBoolValue("pointLights[0].bActive", true); // Activate the point light

	// Setup Spot Light
	m_pShaderManager->setVec3Value("spotLight.position", 0.0f, 5.0f, 0.0f); // put in position
	m_pShaderManager->setVec3Value("spotLight.direction", 0.0f, -1.0f, -1.0f); // point at candle label
	m_pShaderManager->setFloatValue("spotLight.cutOff", glm::cos(glm::radians(12.5f))); // Inner cone angle
	m_pShaderManager->setFloatValue("spotLight.outerCutOff", glm::cos(glm::radians(20.0f))); // Outer cone angle
	m_pShaderManager->setFloatValue("spotLight.constant", 1.0f);
	m_pShaderManager->setFloatValue("spotLight.linear", 0.09f);
	m_pShaderManager->setFloatValue("spotLight.quadratic", 0.032f); 
	m_pShaderManager->setVec3Value("spotLight.ambient", 0.5f, 0.5f, 0.5f); 
	m_pShaderManager->setVec3Value("spotLight.diffuse", 0.6f, 0.6f, 0.6f);
	m_pShaderManager->setVec3Value("spotLight.specular", 0.8f, 0.8f, 0.8f);
	m_pShaderManager->setBoolValue("spotLight.bActive", true); // Activate the spot light


	// Setup Spot Light
	m_pShaderManager->setVec3Value("spotLight.position", 0.0f, 50.0f, 0.0f); // put in position
	m_pShaderManager->setVec3Value("spotLight.direction", 0.0f, -2.0f, 0.0f); // point at candle label
	m_pShaderManager->setFloatValue("spotLight.cutOff", glm::cos(glm::radians(25.0f))); // Inner cone angle
	m_pShaderManager->setFloatValue("spotLight.outerCutOff", glm::cos(glm::radians(40.0f))); // Outer cone angle
	m_pShaderManager->setFloatValue("spotLight.constant", 1.0f);
	m_pShaderManager->setFloatValue("spotLight.linear", 0.09f);
	m_pShaderManager->setFloatValue("spotLight.quadratic", 0.032f);
	m_pShaderManager->setVec3Value("spotLight.ambient", 0.5f, 0.5f, 0.5f);
	m_pShaderManager->setVec3Value("spotLight.diffuse", 0.6f, 0.6f, 0.6f);
	m_pShaderManager->setVec3Value("spotLight.specular", 0.8f, 0.8f, 0.8f);
	m_pShaderManager->setBoolValue("spotLight.bActive", true); // Activate the spot light

	m_pShaderManager->setBoolValue("bUseLighting", true); // Enable lighting in shader
}
 /***********************************************************
  *  LoadSceneTextures()
  *
  *  This method is used for preparing the 3D scene by loading
  *  the shapes, textures in memory to support the 3D scene
  *  rendering
  ***********************************************************/
void SceneManager::LoadSceneTextures()
{
	bool bReturn = false;
	/*** STUDENTS - add the code BELOW for loading the textures that ***/
	/*** will be used for mapping to objects in the 3D scene. Up to  ***/
	/*** 16 textures can be loaded per scene. Refer to the code in   ***/
	/*** the OpenGL Sample for help.                                 ***/
	bReturn = CreateGLTexture(
		"textures/candle.png",
		"candle_wax");

	bReturn = CreateGLTexture(
		"textures/label_overlay.png",
		"label_overlay");

	bReturn = CreateGLTexture(
		"textures/table_material.png",
		"table_material");

	bReturn = CreateGLTexture(
		"textures/coaster.png",
		"coaster");


	// after the texture image data is loaded into memory, the
	// loaded textures need to be bound to texture slots - there
	// are a total of 16 available slots for scene textures
	BindGLTextures();
}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// load the textures for the 3D scene
	LoadSceneTextures();
	DefineObjectMaterials();
	SetupSceneLights();

	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadTorusMesh();
	m_basicMeshes->LoadBoxMesh();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(10.0f, 1.0f, 10.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("table_material");
	SetShaderMaterial("wood"); //set the material

	// draw the mesh with transformation values
	m_basicMeshes->DrawPlaneMesh();
	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);

	XrotationDegrees = 90.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 10.0f, -10.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);
	SetShaderMaterial("wood"); //set the material

	// draw the mesh with transformation values
	m_basicMeshes->DrawPlaneMesh();
	/****************************************************************/

#pragma region Candle

	scaleXYZ = glm::vec3(2.0f, 2.0f, 2.0f);  // set the XYZ scale for the mesh
	XrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(0.0f, 0.0f, -6.0f); // set the XYZ position for the mesh
	
	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("candle_wax"); 
	SetShaderMaterial("glass"); //set the material

	// Set the overlay texture UV scale and offset
	glm::vec2 overlayScale = glm::vec2(0.2f, 0.2f); // Make the texture cover 20% of the UV space
	glm::vec2 overlayOffset = ((glm::vec2(1.0f, 1.0f) - overlayScale) / 2.0f) + glm::vec2(0.25f, 0.0f); // Calculate offset to center it to the camera starting view.
	SetShaderOverlayUVTransform(overlayScale, overlayOffset);
	

	SetOverlayTexture("label_overlay", 1.0f);
	m_basicMeshes->DrawCylinderMesh();            // draw the mesh with transformation values

	// This makes sure that objects following this are not accidentally rendered with the overlay.
	if (NULL != m_pShaderManager) {
		m_pShaderManager->setIntValue(g_UseOverlayTextureName, false);
		SetShaderOverlayUVTransform(glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
		glActiveTexture(GL_TEXTURE0);
	}

	scaleXYZ = glm::vec3(1.65f, 1.65f, 1.65f);  // set the XYZ scale for the mesh
	positionXYZ = glm::vec3(0.0f, 2.0f, -6.0f); // set the XYZ position for the mesh
	XrotationDegrees = 90;

	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);
	

	SetShaderTexture("candle_wax");
	SetShaderMaterial("glass");
	m_basicMeshes->DrawTorusMesh();            // draw the mesh with transformation values

	scaleXYZ = glm::vec3(1.65f, 0.95f, 1.65f);  // set the XYZ scale for the mesh
	positionXYZ = glm::vec3(0.0f, 2.0f, -6.0f); // set the XYZ position for the mesh
	XrotationDegrees = 0;

	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("candle_wax");
	SetShaderMaterial("glass");
	m_basicMeshes->DrawCylinderMesh();            // draw the mesh with transformation values

	scaleXYZ = glm::vec3(1.55f, 1.55f, 0.75f);  // set the XYZ scale for the mesh
	positionXYZ = glm::vec3(0.0f, 2.85f, -6.0f); // set the XYZ position for the mesh
	XrotationDegrees = 90;

	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("candle_wax");
	SetShaderMaterial("glass");
	m_basicMeshes->DrawTorusMesh();            // draw the mesh with transformation valuess

#pragma endregion

#pragma region Lamp

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(4.5f, 1.0f, 2.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.0f, 0.2f, -6.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.4745098039215686f, 0.4509803921568627f, 0.4235294117647059f, 1.0f);
	SetShaderMaterial("wood"); //set the material

	// draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(3.0f, 0.8f, 0.32f);
	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.0f, 1.0f, -6.0f);
	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);
	SetShaderMaterial("wood"); //set the material
	// draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.8f, 7.2f, 0.32f);
	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(3.9f, 3.4f, -6.0f);
	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderColor(0.0f, 0.0f, 0.0f, 1.0f);
	SetShaderMaterial("wood"); //set the material
	// draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();


	positionXYZ = glm::vec3(6.1f, 3.4f, -6.0f);
	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	// draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(3.0f, 0.8f, 0.32f);
	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.0f, 7.2f, -6.0f);
	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	// draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.15f, 1.2f, 0.15f);
	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.0f, 7.6f, -6.0f);
	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	// draw the mesh with transformation values
	m_basicMeshes->DrawCylinderMesh();

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(5.5f, 5.0f, 3.5f);
	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.0f, 11.0f, -6.0f);
	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	SetShaderColor(0.603921568627451f, 0.6588235294117647f, 0.7137254901960784f, 1.0f);
	// draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();
#pragma endregion

#pragma region Coasters

	scaleXYZ = glm::vec3(2.7f, 0.2f, 2.7f);  // set the XYZ scale for the mesh
	XrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-5.0f, 0.1f, -5.2f); // set the XYZ position for the mesh

	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);

	SetShaderTexture("coaster");
	SetShaderMaterial("tile"); //set the material

	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values

	YrotationDegrees = 3.0f;
	positionXYZ = glm::vec3(-5.0f, 0.3f, -5.2f); // set the XYZ position for the mesh
	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);
	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values

	YrotationDegrees = -3.0f;
	positionXYZ = glm::vec3(-5.0f, 0.5f, -5.2f); // set the XYZ position for the mesh
	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);
	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values

	YrotationDegrees = 15.0f;
	positionXYZ = glm::vec3(-5.2f, 0.7f, -5.2f); // set the XYZ position for the mesh
	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);
	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values
#pragma endregion

#pragma region Remotes
#pragma region Remote1

	scaleXYZ = glm::vec3(1.0f, 0.5f, 4.0f);  // set the XYZ scale for the mesh
	YrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-4.0f, 0.1f, 3.2f); // set the XYZ position for the mesh

	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.2235294117647059f, 0.2588235294117647f, 0.2980392156862745f, 1.0f);
	SetShaderMaterial("glass"); //set the material

	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values

#pragma region Buttons
	scaleXYZ = glm::vec3(0.2f, 0.1f, 0.2f);  // set the XYZ scale for the mesh
	positionXYZ += glm::vec3(0.3f, 0.25f, -1.8f); // set the XYZ position for the mesh

	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.2235294117647059f, 0.2588235294117647f, 0.2980392156862745f, 1.0f);
	SetShaderMaterial("glass"); //set the material

	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values


	scaleXYZ = glm::vec3(0.1f, 0.1f, 0.1f);  // set the XYZ scale for the mesh
	positionXYZ += glm::vec3(-0.6f, 0.0f, 0.0f); // set the XYZ position for the mesh

	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);

	SetShaderColor(1.0f, 0.0f, 0.0f, 1.0f);
	SetShaderMaterial("glass"); //set the material

	m_basicMeshes->DrawCylinderMesh();            // draw the mesh with transformation values

	positionXYZ += glm::vec3(0.6f, 0.0f, 0.0f); // set the XYZ position for the mesh
	glm::vec3 orgPositionXYZ = positionXYZ;
	SetShaderColor(0.2235294117647059f, 0.2588235294117647f, 0.2980392156862745f, 1.0f);
	
	for (int z = 1; z <= 3; z++) {
		for (int x = 0; x < 3; x++) {
			positionXYZ += glm::vec3((-0.3f * x), 0.0f, (0.3f * z)); // set the XYZ position for the mesh
			SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
				scaleXYZ, XrotationDegrees,
				YrotationDegrees, ZrotationDegrees,
				positionXYZ);
			m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values
			positionXYZ = orgPositionXYZ;
		}
	}
	

	scaleXYZ = glm::vec3(0.2f, 0.1f, 0.4f);  // set the XYZ scale for the mesh
	positionXYZ += glm::vec3(0.0f, 0.0f, 1.4f); // set the XYZ position for the mesh
	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);
	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values

	positionXYZ += glm::vec3(-0.6f, 0.0f, 0.0f); // set the XYZ position for the mesh
	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);
	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values


	positionXYZ += glm::vec3(0.6f, 0.0f, 0.2f); // set the XYZ position for the mesh
	orgPositionXYZ = positionXYZ;
	scaleXYZ = glm::vec3(0.2f, 0.1f, 0.2f);  // set the XYZ scale for the mesh

	for (int z = 1; z <= 5; z++) {
		for (int x = 0; x < 3; x++) {
			positionXYZ += glm::vec3((-0.3f * x), 0.0f, (0.3f * z)); // set the XYZ position for the mesh
			SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
				scaleXYZ, XrotationDegrees,
				YrotationDegrees, ZrotationDegrees,
				positionXYZ);
			m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values
			positionXYZ = orgPositionXYZ;
		}
	}
	
#pragma endregion
#pragma endregion

#pragma region Remote2

	scaleXYZ = glm::vec3(1.0f, 0.5f, 4.0f);  // set the XYZ scale for the mesh
	YrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-2.0f, 0.1f, 3.2f); // set the XYZ position for the mesh

	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.2235294117647059f, 0.2588235294117647f, 0.2980392156862745f, 1.0f);
	SetShaderMaterial("glass"); //set the material

	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values

#pragma region Buttons
	scaleXYZ = glm::vec3(0.2f, 0.1f, 0.2f);  // set the XYZ scale for the mesh
	positionXYZ += glm::vec3(0.3f, 0.25f, -1.8f); // set the XYZ position for the mesh

	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.2235294117647059f, 0.2588235294117647f, 0.2980392156862745f, 1.0f);
	SetShaderMaterial("glass"); //set the material

	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values


	scaleXYZ = glm::vec3(0.1f, 0.1f, 0.1f);  // set the XYZ scale for the mesh
	positionXYZ += glm::vec3(-0.6f, 0.0f, 0.0f); // set the XYZ position for the mesh

	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);

	SetShaderColor(1.0f, 0.0f, 0.0f, 1.0f);
	SetShaderMaterial("glass"); //set the material

	m_basicMeshes->DrawCylinderMesh();            // draw the mesh with transformation values

	positionXYZ += glm::vec3(0.6f, 0.0f, 0.0f); // set the XYZ position for the mesh
	orgPositionXYZ = positionXYZ;
	SetShaderColor(0.2235294117647059f, 0.2588235294117647f, 0.2980392156862745f, 1.0f);

	for (int z = 1; z <= 3; z++) {
		for (int x = 0; x < 3; x++) {
			positionXYZ += glm::vec3((-0.3f * x), 0.0f, (0.3f * z)); // set the XYZ position for the mesh
			SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
				scaleXYZ, XrotationDegrees,
				YrotationDegrees, ZrotationDegrees,
				positionXYZ);
			m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values
			positionXYZ = orgPositionXYZ;
		}
	}


	scaleXYZ = glm::vec3(0.2f, 0.1f, 0.4f);  // set the XYZ scale for the mesh
	positionXYZ += glm::vec3(0.0f, 0.0f, 1.4f); // set the XYZ position for the mesh
	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);
	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values

	positionXYZ += glm::vec3(-0.6f, 0.0f, 0.0f); // set the XYZ position for the mesh
	SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
		scaleXYZ, XrotationDegrees,
		YrotationDegrees, ZrotationDegrees,
		positionXYZ);
	m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values


	positionXYZ += glm::vec3(0.6f, 0.0f, 0.2f); // set the XYZ position for the mesh
	orgPositionXYZ = positionXYZ;
	scaleXYZ = glm::vec3(0.2f, 0.1f, 0.2f);  // set the XYZ scale for the mesh

	for (int z = 1; z <= 5; z++) {
		for (int x = 0; x < 3; x++) {
			positionXYZ += glm::vec3((-0.3f * x), 0.0f, (0.3f * z)); // set the XYZ position for the mesh
			SetTransformations(                        // set the transformations into memory to be used on the drawn meshes
				scaleXYZ, XrotationDegrees,
				YrotationDegrees, ZrotationDegrees,
				positionXYZ);
			m_basicMeshes->DrawBoxMesh();            // draw the mesh with transformation values
			positionXYZ = orgPositionXYZ;
		}
	}

#pragma endregion
#pragma endregion

#pragma endregion

}
