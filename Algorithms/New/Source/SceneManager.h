#pragma once

#include "ShaderManager.h"
#include "ShapeMeshes.h"

#include <string>
#include <vector>



/***********************************************************
 *  SceneManager
 *
 *  This class contains the code for preparing and rendering
 *  3D scenes, including the shader settings.
 ***********************************************************/
class SceneManager
{
public:
	// constructor
	SceneManager(ShaderManager *pShaderManager);
	// destructor
	~SceneManager();

	struct TEXTURE_INFO
	{
		std::string tag;
		uint32_t ID;
	};

	struct OBJECT_MATERIAL
	{
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
		float shininess;
		std::string tag;
	};

	// Defined mesh types so the renderer knows which Draw function to call
	enum MeshType {
		MESH_PLANE,
		MESH_BOX,
		MESH_CYLINDER,
		MESH_TORUS
	};

	// The physical boundaries of the object
	struct AABB {
		glm::vec3 minBounds;
		glm::vec3 maxBounds;
	};
	// A mathematical plane defined by a normal vector and distance from origin
	struct Plane {
		glm::vec3 normal;
		float distance;

		// Normalizes the plane equation so the normal vector has a length of 1
		void Normalize() {
			float mag = glm::length(normal);
			normal /= mag;
			distance /= mag;
		}

		// Returns true if the AABB is completely IN FRONT of this plane
		// Returns false if the AABB is completely BEHIND it
		bool IsOnForwardSide(const AABB& aabb) const {
			// Find the corner of the AABB that is furthest along the plane's normal
			glm::vec3 positiveVertex = aabb.minBounds;
			if (normal.x >= 0) positiveVertex.x = aabb.maxBounds.x;
			if (normal.y >= 0) positiveVertex.y = aabb.maxBounds.y;
			if (normal.z >= 0) positiveVertex.z = aabb.maxBounds.z;

			// If that furthest point is behind the plane, the whole object is behind the plane
			return (glm::dot(normal, positiveVertex) + distance) >= 0;
		}
	};

	// The 6 planes that make up the camera's field of view
	struct ViewFrustum {
		Plane leftFace;
		Plane rightFace;
		Plane bottomFace;
		Plane topFace;
		Plane nearFace;
		Plane farFace;

		// Checks the AABB against all 6 planes. If it fails ANY plane, it is off-screen.
		bool IsBoxVisible(const AABB& bounds) const {
			return (leftFace.IsOnForwardSide(bounds) &&
				rightFace.IsOnForwardSide(bounds) &&
				bottomFace.IsOnForwardSide(bounds) &&
				topFace.IsOnForwardSide(bounds) &&
				nearFace.IsOnForwardSide(bounds) &&
				farFace.IsOnForwardSide(bounds));
		}
	};

	// All the data needed to draw a single piece of the scene
	struct SceneObject {
		std::string objectName;

		// Transform Data
		glm::vec3 position;
		glm::vec3 scale;
		glm::vec3 rotation; // X, Y, Z degrees

		// Render Data
		MeshType meshType;
		std::string materialTag;
		std::string textureTag;

		// Color Data (if not using textures)
		bool bUsesColor;
		glm::vec4 color;

		// Overlay Data
		bool bHasOverlay;
		std::string overlayTag;

		// Spatial Data
		AABB boundingBox;
	};

private:
	// pointer to shader manager object
	ShaderManager* m_pShaderManager;
	// pointer to basic shapes object
	ShapeMeshes* m_basicMeshes;
	// total number of loaded textures
	int m_loadedTextures;
	// loaded textures info
	TEXTURE_INFO m_textureIDs[16];
	// defined object materials
	std::vector<OBJECT_MATERIAL> m_objectMaterials; 
	
	std::vector<SceneObject> m_sceneObjects;

	// load texture images and convert to OpenGL texture data
	bool CreateGLTexture(const char* filename, std::string tag);
	// bind loaded OpenGL textures to slots in memory
	void BindGLTextures();
	// free the loaded OpenGL textures
	void DestroyGLTextures();
	// find a loaded texture by tag
	int FindTextureID(std::string tag);
	int FindTextureSlot(std::string tag);
	// find a defined material by tag
	bool FindMaterial(std::string tag, OBJECT_MATERIAL& material);

	// set the transformation values 
	// into the transform buffer
	void SetTransformations(
		glm::vec3 scaleXYZ,
		float XrotationDegrees,
		float YrotationDegrees,
		float ZrotationDegrees,
		glm::vec3 positionXYZ);

	// set the color values into the shader
	void SetShaderColor(
		float redColorValue,
		float greenColorValue,
		float blueColorValue,
		float alphaValue);

	// set the texture data into the shader
	void SetShaderTexture(
		std::string textureTag);

	// set the UV scale for the texture mapping
	void SetTextureUVScale(
		float u, float v);

	// set the object material into the shader
	void SetShaderMaterial(
		std::string materialTag);
	void BuildSceneList();

	ViewFrustum ExtractFrustum(const glm::mat4& viewProjMatrix);
	void RenderCulledScene(const ViewFrustum& frustum);

public:

	// The following methods are for the students to 
	// customize for their own 3D scene
	void PrepareScene();
	void RenderScene(glm::mat4 viewProjectionMatrix = glm::mat4(1.0f));

	void DefineObjectMaterials();
	void SetupSceneLights();

	// loads textures from image files
	void LoadSceneTextures();
	
	// Method to set the texture overlay and it strength
	void SetOverlayTexture(std::string overlayTextureTag, float strength);
	// Method to set the overlay texture UV scale and offsets
	void SetShaderOverlayUVTransform(glm::vec2 scale, glm::vec2 offset);

};