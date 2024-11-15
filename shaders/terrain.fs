#version 150

// Input from the vertex shader
in vec3 vPosition;   // World position of the fragment
in vec2 vTexture;    // Texture coordinates
in vec3 vColor;      // Color passed from the vertex shader (grayscale from heightmap)
in vec3 vNormal;	// Calculated surface normal from the vertex shader

uniform vec3 uLightDirection; // Light direction. changes according to user input
uniform float uLightIntensity;
uniform mat4 uLightViewProjection;
uniform vec3 uCameraPosition;

uniform float uMinHeight;
uniform float uMaxHeight;

uniform vec2 uIndicatorPosition;
uniform float uIndicatorRadius;

uniform float uTextureScale;
uniform sampler2D uBaseTexture;
uniform sampler2D uGroundTexture;
uniform sampler2D uRockTexture;
uniform sampler2D uPeaksTexture;
uniform sampler2D uShadowmap;



// Output to the framebuffer
out vec4 oFragColor;

// Blend all textures together. some textures will be more prominent than others in certain heights
vec3 BlendTextures(vec2 texScale){
    vec3 unitNormal = normalize(vNormal);

    vec3 baseColor = texture(uBaseTexture, texScale).rgb;
    vec3 groundColor = texture(uGroundTexture, texScale).rgb;
    vec3 rockColor = texture(uRockTexture, texScale).rgb;
    vec3 peaksColor = texture(uPeaksTexture, texScale).rgb;
    
    float epsilon = 0.0001f; // remove divide by 0 case
    float threshold = uMinHeight + .4f * (uMaxHeight - uMinHeight); //add 40% of maxheight to minheight
    float normalizedMaxHeight = clamp((vPosition.y + uMaxHeight) / (2.0 * uMaxHeight), 0.0, 1.0);
    float normalizedMinHeight = clamp((vPosition.y - uMinHeight) / (threshold - uMinHeight + epsilon), 0.0f, 1.0f); //[0, 1]

    // blend rock color with base
    float rockFactor = smoothstep(0.2, 1.0, normalizedMinHeight); 
    vec3 currentColor = mix(baseColor, rockColor, normalizedMinHeight); //once position reaches threshold, rockColor takes over

    vec3 upVector = vec3(0.0f, 1.0f, 0.0f);
    float slopeFactor = smoothstep(.5f, 1.f, dot(unitNormal, upVector)); //slope 
    float minHeightFactor = smoothstep(.6f, 1.f, normalizedMinHeight) * slopeFactor; //closer to minHeight, lower the value
    float maxHeightFactor = smoothstep(0.1f, .6f, 1.0f - normalizedMaxHeight) * slopeFactor; //further away from maxheight, higher the value
    float groundFactor = min(minHeightFactor, maxHeightFactor);
    currentColor = mix(currentColor, groundColor, groundFactor);

    // blend current color with peaks
    float peaksFactor = smoothstep(0.7f, 1.0f, normalizedMaxHeight); //rapidly increase bias as position reaches higher
    currentColor = mix(currentColor, peaksColor, peaksFactor);

    return currentColor;
}

float CalculateShadow(){
    float shadow = 0.f;

    vec4 fragPositionInLightSpace = uLightViewProjection * vec4(vPosition, 1.f);
    vec3 lightSpacePosition = fragPositionInLightSpace.xyz / fragPositionInLightSpace.w; //[-1,1]
    lightSpacePosition = lightSpacePosition * 0.5f + 0.5f; //[0,1]

    // if not outside of far range
    if(lightSpacePosition.z <= 1.f + 0.005){
        float closestDepth = texture(uShadowmap, lightSpacePosition.xy).r;
        float currentDepth = lightSpacePosition.z;
        
        //an object is blocking the light
        if(currentDepth > closestDepth){
            shadow = 1.f;
        }
    }
    return shadow;
}

void main() {
	vec3 unitNormal = normalize(vNormal); // Must normalize for dot product math calculation things
    vec3 lightColor = vec3(1.f, 1.f,1.f);
    vec3 ambient = lightColor * 0.3f;
    float shadow = CalculateShadow();

    //diffuse calculations
    float diffuseFactor = max(dot(unitNormal, uLightDirection), 0.f); //the more front facing towards light, the brighter
	vec3 diffuse = lightColor * diffuseFactor * uLightIntensity * (1.f - shadow);
    
    //specular calculations using phong model
    vec3 viewDirection = normalize(uCameraPosition - vPosition); 
    vec3 reflectDirection = reflect(-uLightDirection, unitNormal);  
    float shininess = 5.f;
    float specularStrength = 0.1f; 
    float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.f), shininess); //the more front facing the camera is to the reflection, the shinier
    vec3 specular = lightColor * specularStrength * specularFactor * (1.f - shadow);

    //calculate texture color
	vec2 texScale = vTexture * uTextureScale;
    vec3 textureColor = BlendTextures(texScale);

    oFragColor = vec4(textureColor * (diffuse + ambient + specular), 1.f);
    float distanceFromEdge = abs(length(vPosition.xz - uIndicatorPosition) - uIndicatorRadius);
    if (distanceFromEdge <= 4.f && uIndicatorRadius > 0.f){
        float intensity = smoothstep(4.f, 0.f, distanceFromEdge)*2.2f + 1.f; // Smooth transition for indicator ring to make it not look flat
        oFragColor *= intensity;
    }
}

