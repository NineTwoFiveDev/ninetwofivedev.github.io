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
void SceneManager::BuildSceneList()
{
	// Clear any existing objects just in case
	m_sceneObjects.clear();

	// Helper macro/lambda for dummy AABB generation
	auto calculateDummyAABB = [](SceneObject& obj) {
		obj.boundingBox.minBounds = obj.position - obj.scale;
		obj.boundingBox.maxBounds = obj.position + obj.scale;
		};

	// --- TABLE FLOOR ---
	SceneObject tableFloor;
	tableFloor.objectName = "Table_Floor";
	tableFloor.meshType = MESH_PLANE;
	tableFloor.position = glm::vec3(0.0f, 0.0f, 0.0f);
	tableFloor.scale = glm::vec3(10.0f, 1.0f, 10.0f);
	tableFloor.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	tableFloor.textureTag = "table_material";
	tableFloor.materialTag = "wood";
	tableFloor.bUsesColor = false;
	tableFloor.bHasOverlay = false;
	calculateDummyAABB(tableFloor);
	m_sceneObjects.push_back(tableFloor);

	// --- BACK WALL ---
	SceneObject backWall = tableFloor;
	backWall.objectName = "Back_Wall";
	backWall.position = glm::vec3(0.0f, 10.0f, -10.0f);
	backWall.scale = glm::vec3(20.0f, 1.0f, 10.0f);
	backWall.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
	backWall.bUsesColor = true;
	backWall.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	backWall.textureTag = "";
	calculateDummyAABB(backWall);
	m_sceneObjects.push_back(backWall);

	// ==========================================
	// CANDLE
	// ==========================================

	SceneObject candlePiece;
	candlePiece.textureTag = "candle_wax";
	candlePiece.materialTag = "glass";
	candlePiece.bUsesColor = false;
	candlePiece.bHasOverlay = false;
	candlePiece.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	// Candle Base (With Overlay)
	candlePiece.objectName = "Candle_Base";
	candlePiece.meshType = MESH_CYLINDER;
	candlePiece.position = glm::vec3(0.0f, 0.0f, -6.0f);
	candlePiece.scale = glm::vec3(2.0f, 2.0f, 2.0f);
	candlePiece.bHasOverlay = true;
	candlePiece.overlayTag = "label_overlay";
	calculateDummyAABB(candlePiece);
	m_sceneObjects.push_back(candlePiece);

	candlePiece.bHasOverlay = false; // Turn off overlay for rest of candle

	// Candle Lower Torus
	candlePiece.objectName = "Candle_Lower_Torus";
	candlePiece.meshType = MESH_TORUS;
	candlePiece.position = glm::vec3(0.0f, 2.0f, -6.0f);
	candlePiece.scale = glm::vec3(1.65f, 1.65f, 1.65f);
	candlePiece.rotation.x = 90.0f;
	calculateDummyAABB(candlePiece);
	m_sceneObjects.push_back(candlePiece);

	// Candle Inner Cylinder
	candlePiece.objectName = "Candle_Inner_Cyl";
	candlePiece.meshType = MESH_CYLINDER;
	candlePiece.position = glm::vec3(0.0f, 2.0f, -6.0f);
	candlePiece.scale = glm::vec3(1.65f, 0.95f, 1.65f);
	candlePiece.rotation.x = 0.0f;
	calculateDummyAABB(candlePiece);
	m_sceneObjects.push_back(candlePiece);

	// Candle Upper Torus
	candlePiece.objectName = "Candle_Upper_Torus";
	candlePiece.meshType = MESH_TORUS;
	candlePiece.position = glm::vec3(0.0f, 2.85f, -6.0f);
	candlePiece.scale = glm::vec3(1.55f, 1.55f, 0.75f);
	candlePiece.rotation.x = 90.0f;
	calculateDummyAABB(candlePiece);
	m_sceneObjects.push_back(candlePiece);

	// ==========================================
	// LAMP
	// ==========================================

	SceneObject lampPiece;
	lampPiece.meshType = MESH_BOX;
	lampPiece.materialTag = "wood";
	lampPiece.bUsesColor = true;
	lampPiece.bHasOverlay = false;
	lampPiece.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	// Lamp Base
	lampPiece.objectName = "Lamp_Base";
	lampPiece.position = glm::vec3(5.0f, 0.2f, -6.0f);
	lampPiece.scale = glm::vec3(4.5f, 1.0f, 2.5f);
	lampPiece.color = glm::vec4(0.47f, 0.45f, 0.42f, 1.0f);
	calculateDummyAABB(lampPiece);
	m_sceneObjects.push_back(lampPiece);

	// Lamp Parts (Black Wood)
	lampPiece.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	lampPiece.objectName = "Lamp_Stand_Bottom";
	lampPiece.position = glm::vec3(5.0f, 1.0f, -6.0f);
	lampPiece.scale = glm::vec3(3.0f, 0.8f, 0.32f);
	calculateDummyAABB(lampPiece);
	m_sceneObjects.push_back(lampPiece);

	lampPiece.objectName = "Lamp_Arm_Left";
	lampPiece.position = glm::vec3(3.9f, 3.4f, -6.0f);
	lampPiece.scale = glm::vec3(0.8f, 7.2f, 0.32f);
	calculateDummyAABB(lampPiece);
	m_sceneObjects.push_back(lampPiece);

	lampPiece.objectName = "Lamp_Arm_Right";
	lampPiece.position = glm::vec3(6.1f, 3.4f, -6.0f);
	// scale remains same as left arm
	calculateDummyAABB(lampPiece);
	m_sceneObjects.push_back(lampPiece);

	lampPiece.objectName = "Lamp_Stand_Top";
	lampPiece.position = glm::vec3(5.0f, 7.2f, -6.0f);
	lampPiece.scale = glm::vec3(3.0f, 0.8f, 0.32f);
	calculateDummyAABB(lampPiece);
	m_sceneObjects.push_back(lampPiece);

	lampPiece.objectName = "Lamp_Neck";
	lampPiece.meshType = MESH_CYLINDER;
	lampPiece.position = glm::vec3(5.0f, 7.6f, -6.0f);
	lampPiece.scale = glm::vec3(0.15f, 1.2f, 0.15f);
	calculateDummyAABB(lampPiece);
	m_sceneObjects.push_back(lampPiece);

	// Lamp Shade
	lampPiece.objectName = "Lamp_Shade";
	lampPiece.meshType = MESH_BOX;
	lampPiece.position = glm::vec3(5.0f, 11.0f, -6.0f);
	lampPiece.scale = glm::vec3(5.5f, 5.0f, 3.5f);
	lampPiece.color = glm::vec4(0.60f, 0.65f, 0.71f, 1.0f);
	calculateDummyAABB(lampPiece);
	m_sceneObjects.push_back(lampPiece);

	// ==========================================
	// COASTERS
	// ==========================================

	SceneObject coaster;
	coaster.meshType = MESH_BOX;
	coaster.textureTag = "coaster";
	coaster.materialTag = "tile";
	coaster.bUsesColor = false;
	coaster.bHasOverlay = false;
	coaster.scale = glm::vec3(2.7f, 0.2f, 2.7f);
	coaster.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	coaster.objectName = "Coaster_1";
	coaster.position = glm::vec3(-5.0f, 0.1f, -5.2f);
	calculateDummyAABB(coaster);
	m_sceneObjects.push_back(coaster);

	coaster.objectName = "Coaster_2";
	coaster.position = glm::vec3(-5.0f, 0.3f, -5.2f);
	coaster.rotation.y = 3.0f;
	calculateDummyAABB(coaster);
	m_sceneObjects.push_back(coaster);

	coaster.objectName = "Coaster_3";
	coaster.position = glm::vec3(-5.0f, 0.5f, -5.2f);
	coaster.rotation.y = -3.0f;
	calculateDummyAABB(coaster);
	m_sceneObjects.push_back(coaster);

	coaster.objectName = "Coaster_4";
	coaster.position = glm::vec3(-5.2f, 0.7f, -5.2f);
	coaster.rotation.y = 15.0f;
	calculateDummyAABB(coaster);
	m_sceneObjects.push_back(coaster);

	// ==========================================
	// REMOTES
	// ==========================================

	// Helper lambda to build a remote with all its buttons
	auto buildRemote = [&](std::string prefix, glm::vec3 basePosition) {
		SceneObject remotePart;
		remotePart.bHasOverlay = false;
		remotePart.bUsesColor = true;
		remotePart.rotation = glm::vec3(0.0f, 0.0f, 0.0f);

		// --- Remote Body ---
		remotePart.objectName = prefix + "_Body";
		remotePart.meshType = MESH_BOX;
		remotePart.position = basePosition;
		remotePart.scale = glm::vec3(1.0f, 0.5f, 4.0f);
		remotePart.color = glm::vec4(0.22f, 0.25f, 0.29f, 1.0f);
		remotePart.materialTag = "glass";
		calculateDummyAABB(remotePart);
		m_sceneObjects.push_back(remotePart);

		// --- Single Top Button ---
		glm::vec3 currentPos = basePosition + glm::vec3(0.3f, 0.25f, -1.8f);
		remotePart.objectName = prefix + "_TopButton";
		remotePart.position = currentPos;
		remotePart.scale = glm::vec3(0.2f, 0.1f, 0.2f);
		calculateDummyAABB(remotePart);
		m_sceneObjects.push_back(remotePart);

		// --- Power Button (Red Cylinder) ---
		currentPos += glm::vec3(-0.6f, 0.0f, 0.0f);
		remotePart.objectName = prefix + "_PowerBtn";
		remotePart.meshType = MESH_CYLINDER;
		remotePart.position = currentPos;
		remotePart.scale = glm::vec3(0.1f, 0.1f, 0.1f);
		remotePart.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		calculateDummyAABB(remotePart);
		m_sceneObjects.push_back(remotePart);

		// --- 3x3 Button Grid ---
		currentPos += glm::vec3(0.6f, 0.0f, 0.0f);
		glm::vec3 grid1Base = currentPos;
		remotePart.meshType = MESH_BOX;
		remotePart.color = glm::vec4(0.22f, 0.25f, 0.29f, 1.0f);

		int btnCount = 1;
		for (int z = 1; z <= 3; z++) {
			for (int x = 0; x < 3; x++) {
				remotePart.objectName = prefix + "_Grid1_Btn" + std::to_string(btnCount++);
				remotePart.position = grid1Base + glm::vec3((-0.3f * x), 0.0f, (0.3f * z));
				calculateDummyAABB(remotePart);
				m_sceneObjects.push_back(remotePart);
			}
		}

		// --- Long Buttons ---
		currentPos = grid1Base + glm::vec3(0.0f, 0.0f, 1.4f);
		remotePart.objectName = prefix + "_LongBtn1";
		remotePart.position = currentPos;
		remotePart.scale = glm::vec3(0.2f, 0.1f, 0.4f);
		calculateDummyAABB(remotePart);
		m_sceneObjects.push_back(remotePart);

		currentPos += glm::vec3(-0.6f, 0.0f, 0.0f);
		remotePart.objectName = prefix + "_LongBtn2";
		remotePart.position = currentPos;
		calculateDummyAABB(remotePart);
		m_sceneObjects.push_back(remotePart);

		// --- 5x3 Button Grid ---
		currentPos += glm::vec3(0.6f, 0.0f, 0.2f);
		glm::vec3 grid2Base = currentPos;
		remotePart.scale = glm::vec3(0.2f, 0.1f, 0.2f);

		btnCount = 1;
		for (int z = 1; z <= 5; z++) {
			for (int x = 0; x < 3; x++) {
				remotePart.objectName = prefix + "_Grid2_Btn" + std::to_string(btnCount++);
				remotePart.position = grid2Base + glm::vec3((-0.3f * x), 0.0f, (0.3f * z));
				calculateDummyAABB(remotePart);
				m_sceneObjects.push_back(remotePart);
			}
		}
		};

	// Build Remote 1
	buildRemote("Remote1", glm::vec3(-4.0f, 0.1f, 3.2f));

	// Build Remote 2
	buildRemote("Remote2", glm::vec3(-2.0f, 0.1f, 3.2f));
}

SceneManager::ViewFrustum SceneManager::ExtractFrustum(const glm::mat4& viewProjMatrix)
{
	ViewFrustum frustum;

	// Left Plane
	frustum.leftFace.normal.x = viewProjMatrix[0][3] + viewProjMatrix[0][0];
	frustum.leftFace.normal.y = viewProjMatrix[1][3] + viewProjMatrix[1][0];
	frustum.leftFace.normal.z = viewProjMatrix[2][3] + viewProjMatrix[2][0];
	frustum.leftFace.distance = viewProjMatrix[3][3] + viewProjMatrix[3][0];
	frustum.leftFace.Normalize();

	// Right Plane
	frustum.rightFace.normal.x = viewProjMatrix[0][3] - viewProjMatrix[0][0];
	frustum.rightFace.normal.y = viewProjMatrix[1][3] - viewProjMatrix[1][0];
	frustum.rightFace.normal.z = viewProjMatrix[2][3] - viewProjMatrix[2][0];
	frustum.rightFace.distance = viewProjMatrix[3][3] - viewProjMatrix[3][0];
	frustum.rightFace.Normalize();

	// Bottom Plane
	frustum.bottomFace.normal.x = viewProjMatrix[0][3] + viewProjMatrix[0][1];
	frustum.bottomFace.normal.y = viewProjMatrix[1][3] + viewProjMatrix[1][1];
	frustum.bottomFace.normal.z = viewProjMatrix[2][3] + viewProjMatrix[2][1];
	frustum.bottomFace.distance = viewProjMatrix[3][3] + viewProjMatrix[3][1];
	frustum.bottomFace.Normalize();

	// Top Plane
	frustum.topFace.normal.x = viewProjMatrix[0][3] - viewProjMatrix[0][1];
	frustum.topFace.normal.y = viewProjMatrix[1][3] - viewProjMatrix[1][1];
	frustum.topFace.normal.z = viewProjMatrix[2][3] - viewProjMatrix[2][1];
	frustum.topFace.distance = viewProjMatrix[3][3] - viewProjMatrix[3][1];
	frustum.topFace.Normalize();

	// Near Plane
	frustum.nearFace.normal.x = viewProjMatrix[0][3] + viewProjMatrix[0][2];
	frustum.nearFace.normal.y = viewProjMatrix[1][3] + viewProjMatrix[1][2];
	frustum.nearFace.normal.z = viewProjMatrix[2][3] + viewProjMatrix[2][2];
	frustum.nearFace.distance = viewProjMatrix[3][3] + viewProjMatrix[3][2];
	frustum.nearFace.Normalize();

	// Far Plane
	frustum.farFace.normal.x = viewProjMatrix[0][3] - viewProjMatrix[0][2];
	frustum.farFace.normal.y = viewProjMatrix[1][3] - viewProjMatrix[1][2];
	frustum.farFace.normal.z = viewProjMatrix[2][3] - viewProjMatrix[2][2];
	frustum.farFace.distance = viewProjMatrix[3][3] - viewProjMatrix[3][2];
	frustum.farFace.Normalize();

	return frustum;
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

	BuildSceneList();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene(glm::mat4 viewProjectionMatrix)
{
	// Extract the 6 planes of the camera's field of view
	ViewFrustum frustum = ExtractFrustum(viewProjectionMatrix);

	int objectsDrawn = 0;
	int objectsCulled = 0;

	// Loop through every object in our database
	for (size_t i = 0; i < m_sceneObjects.size(); i++)
	{
		SceneObject& obj = m_sceneObjects[i];

		// --- THE CULLING MAGIC ---
		// If the object's bounding box is outside the camera's view, skip drawing it!
		if (!frustum.IsBoxVisible(obj.boundingBox)) {
			objectsCulled++;
			continue;
		}

		objectsDrawn++; // Track what actually gets sent to the GPU

		// Set Transformations
		SetTransformations(
			obj.scale,
			obj.rotation.x,
			obj.rotation.y,
			obj.rotation.z,
			obj.position
		);

		// Set Appearance (Color vs Texture)
		if (obj.bUsesColor) {
			SetShaderColor(obj.color.r, obj.color.g, obj.color.b, obj.color.a);
		}
		else {
			SetShaderTexture(obj.textureTag);
		}

		// Set Material
		SetShaderMaterial(obj.materialTag);

		// Handle Specific Overlays
		if (obj.bHasOverlay) {
			glm::vec2 overlayScale = glm::vec2(0.2f, 0.2f);
			glm::vec2 overlayOffset = ((glm::vec2(1.0f, 1.0f) - overlayScale) / 2.0f) + glm::vec2(0.25f, 0.0f);
			SetShaderOverlayUVTransform(overlayScale, overlayOffset);
			SetOverlayTexture(obj.overlayTag, 1.0f);
		}
		else {
			// Turn overlay off for normal objects
			if (NULL != m_pShaderManager) {
				m_pShaderManager->setIntValue("bUseOverlayTexture", false);
				SetShaderOverlayUVTransform(glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f));
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE0);
			}
		}

		// Draw the Correct Mesh
		switch (obj.meshType) {
		case MESH_PLANE:    m_basicMeshes->DrawPlaneMesh(); break;
		case MESH_BOX:      m_basicMeshes->DrawBoxMesh(); break;
		case MESH_CYLINDER: m_basicMeshes->DrawCylinderMesh(); break;
		case MESH_TORUS:    m_basicMeshes->DrawTorusMesh(); break;
		}
	}

	// Print to console to prove the optimization is actually working
	std::cout << "Rendered: " << objectsDrawn << " | Culled (Skipped): " << objectsCulled << "\r";
}
