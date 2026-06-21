#version 330 core
out vec4 fragmentColor;

in vec3 fragmentPosition;
in vec3 fragmentVertexNormal;
in vec2 fragmentTextureCoordinate;

struct Material {
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
}; 

struct DirectionalLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    bool bActive;
};

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    bool bActive;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       

    bool bActive;
};

#define TOTAL_POINT_LIGHTS 5

uniform bool bUseTexture=false;
uniform bool bUseLighting=false;
uniform vec4 objectColor = vec4(1.0f);
uniform vec3 viewPosition;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[TOTAL_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform sampler2D objectTexture;
uniform vec2 UVscale = vec2(1.0f, 1.0f);

// Uniforms for Overlay Texture Transform
uniform sampler2D overlayTexture;
uniform float overlayStrength = 1.0f;             // This can control overall overlay opacity
uniform bool bUseOverlayTexture = false;
uniform vec2 overlayUVScale = vec2(1.0f, 1.0f);   // visual scale 1.0 = 100% scale, 0.5 = 50%, etc...
uniform vec2 overlayUVOffset = vec2(0.0f, 0.0f);  // Bottom-left offset for the scaled image


// function prototypes
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 textureColor);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 textureColor);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 textureColor);

void main()
{    
    // Sample the base texture color
    vec4 baseTexColor = texture(objectTexture, fragmentTextureCoordinate * UVscale);
    
    // Initialize the final color that lighting will be applied to
    // If no base texture, start with objectColor
    vec4 finalCombinedColor = bUseTexture ? baseTexColor : objectColor; 

    // blending based on UV coordinates
    if (bUseOverlayTexture) {
        // Calculate transformed UVs for the overlay
        vec2 transformedOverlayUV = (fragmentTextureCoordinate - overlayUVOffset) / overlayUVScale;

        // Check if the current fragment's UV coordinates fall within the 0-1 range
        // after scaling and offsetting. This means it's inside the label's rectangle.
        if (transformedOverlayUV.x >= 0.0 && transformedOverlayUV.x <= 1.0 &&
            transformedOverlayUV.y >= 0.0 && transformedOverlayUV.y <= 1.0) 
        {
            // If inside the label's bounds, sample the overlay texture
            vec4 overlayTexColor = texture(overlayTexture, transformedOverlayUV);
            
            // Blend the overlay color onto the base
            finalCombinedColor = mix(finalCombinedColor, overlayTexColor, overlayTexColor.a * overlayStrength);
        }
        // else: If outside the label's bounds, finalCombinedColor remains the baseTexColor
    }

    // apply lighting to the final combined color
    if(bUseLighting == true)
    {
        vec3 phongResult = vec3(0.0f);
        vec3 norm = normalize(fragmentVertexNormal);
        vec3 viewDir = normalize(viewPosition - fragmentPosition);
    
        if(directionalLight.bActive == true)
        {
            phongResult += CalcDirectionalLight(directionalLight, norm, viewDir, finalCombinedColor.rgb);
        }
        for(int i = 0; i < TOTAL_POINT_LIGHTS; i++)
        {
	        if(pointLights[i].bActive == true)
            {
                phongResult += CalcPointLight(pointLights[i], norm, fragmentPosition, viewDir, finalCombinedColor.rgb);   
            }
        } 
        if(spotLight.bActive == true)
        {
            phongResult += CalcSpotLight(spotLight, norm, fragmentPosition, viewDir, finalCombinedColor.rgb);    
        }
    
        fragmentColor = vec4(phongResult, finalCombinedColor.a); 
    }
    else // No lighting
    {
        fragmentColor = finalCombinedColor; 
    }
}

// calculates the color when using a directional light.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 textureColor)
{
    vec3 ambient = vec3(0.0f);
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    vec3 lightDirection = normalize(-light.direction);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    ambient = light.ambient * textureColor;
    diffuse = light.diffuse * diff * material.diffuseColor * textureColor;
    specular = light.specular * spec * material.specularColor; 

    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 textureColor)
{
    vec3 ambient = vec3(0.0f);
    vec3 diffuse = vec3(0.0f);
    vec3 specular= vec3(0.0f);

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   
    ambient = light.ambient * textureColor;
    diffuse = light.diffuse * diff * material.diffuseColor * textureColor;
    specular = light.specular * specularComponent * material.specularColor;
    
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 textureColor)
{
    vec3 ambient = vec3(0.0f);
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    ambient = light.ambient * textureColor;
    diffuse = light.diffuse * diff * material.diffuseColor * textureColor;
    specular = light.specular * spec * material.specularColor;
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}