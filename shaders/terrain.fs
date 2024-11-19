#version 150

// Input from the vertex shader
in vec3 vPosition;   // World position of the fragment
in vec2 vTexture;    // Texture coordinates
in vec3 vColor;      // Color passed from the vertex shader (grayscale from heightmap)
in vec3 vNormal;	// Calculated surface normal from the vertex shader

uniform vec3 uLightDirection; // Light direction. changes according to user input
uniform float uBrightness;
uniform mat4 uLightViewProjection;
uniform vec3 uCameraPosition;

uniform float uMinHeight;
uniform float uMaxHeight;

uniform vec2 uIndicatorPosition;
uniform float uIndicatorRadius;

uniform float uSunFalloff;   
uniform float uSunIntensity;

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
    float rockFactor = smoothstep(0.2f, 1.f, normalizedMinHeight); 
    vec3 currentColor = mix(baseColor, rockColor, normalizedMinHeight); //once position reaches threshold, rockColor takes over

    vec3 upVector = vec3(0.f, 1.f, 0.f);
    float slopeFactor = smoothstep(.5f, 1.f, dot(unitNormal, upVector)); //slope 
    float minHeightFactor = smoothstep(.6f, 1.f, normalizedMinHeight) * slopeFactor; //closer to minHeight, lower the value
    float maxHeightFactor = smoothstep(0.1f, .6f, 1.f - normalizedMaxHeight) * slopeFactor; //further away from maxheight, higher the value
    float groundFactor = min(minHeightFactor, maxHeightFactor);
    currentColor = mix(currentColor, groundColor, groundFactor);

    // blend current color with peaks
    float peaksFactor = smoothstep(0.7f, 1.f, normalizedMaxHeight); //rapidly increase bias as position reaches higher
    currentColor = mix(currentColor, peaksColor, peaksFactor);

    return currentColor;
}

float CalculateShadow(){
    float shadow = 0.f;

    vec4 fragPositionInLightSpace = uLightViewProjection * vec4(vPosition, 1.f);
    vec3 lightSpacePosition = fragPositionInLightSpace.xyz / fragPositionInLightSpace.w; //[-1,1]

    //if outside of view frustrum dont do anything
    if(lightSpacePosition.z >= 1.f)
        return shadow;

    lightSpacePosition = lightSpacePosition * 0.5f + 0.5f; //[0,1]
    float currentDepth = lightSpacePosition.z;
    float bias = max(0.025f * (1.0f - dot(normalize(vNormal), uLightDirection)), 0.001f); //the more front facing towards light, the more bias. reduces shadow acne

	// smoother shadows using PCF
	int radius = 2;
    float totalSamples = 0.f;

	vec2 texelSize = 1.0 / textureSize(uShadowmap, 0);
	for(int x = -radius; x <= radius;x++){
		for(int y = -radius; y <= radius; y++){
            vec2 offset = vec2(x, y) * texelSize;
            float closestDepth = texture(uShadowmap, lightSpacePosition.xy + offset).r;

			if (currentDepth > closestDepth + bias){
				shadow += 1.f;     
            }
            totalSamples++;
		}    
	}
	//average shadow
	shadow /= totalSamples;
    return shadow;
}

void main() {
	vec3 unitNormal = normalize(vNormal); // Must normalize for dot product math calculation things
    vec3 lightColor = vec3(1.0, 0.9, 0.7); 
    vec3 ambient = lightColor * 0.3f;
    float shadow = CalculateShadow();

    //diffuse calculations
    float diffuseFactor = max(dot(unitNormal, uLightDirection), 0.f); //the more front facing towards light, the brighter
	vec3 diffuse = lightColor * diffuseFactor * uBrightness * (1.f - shadow);
    
    //sun calculations
    float sunHighlight = pow(diffuseFactor, uSunFalloff * .25f);
    vec3 sunlightEffect = lightColor * sunHighlight * uSunIntensity;

    //calculate texture color
	vec2 texScale = vTexture * uTextureScale;
    vec3 textureColor = BlendTextures(texScale);

    oFragColor = vec4(textureColor * (diffuse + ambient + sunlightEffect) , 1.f);
    float distanceFromEdge = abs(length(vPosition.xz - uIndicatorPosition) - uIndicatorRadius);
    if (distanceFromEdge <= 4.f && uIndicatorRadius > 0.f){
        float intensity = smoothstep(4.f, 0.f, distanceFromEdge)*2.2f + 1.f; // Smooth transition for indicator ring to make it not look flat
        oFragColor *= intensity;
    }
}

