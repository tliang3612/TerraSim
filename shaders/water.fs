#version 330 core

precision highp float;

in vec2 vTextureCoords;
in vec4 vClipSpace;
in vec3 vVertexToCamera;

out vec4 oFragColor;

uniform float uMoveFactor;   
uniform float uBrightness;
uniform float uSunFalloff;
uniform float uSunIntensity;
uniform vec3 uLightDirection;  
uniform float uWaterShininess;
uniform float uWaterHeight;
uniform vec3 uSunColor;
uniform sampler2D uReflectionTexture; 
uniform sampler2D uRefractionTexture; 
uniform sampler2D uDudvmap;          
uniform sampler2D uNormalmap;         
uniform sampler2D uDepthmap;         

const float reflectivity = 0.5f;    
const vec4 waterColor = vec4(0.229f, 0.808f, 0.922f, 1.f);
const vec4 murkinessColor = vec4(0.02f, 0.05f, 0.2f, 1.f); // Slightly less murky

//calculate depth 
float CalculateDepth(float near, float far, float depthValue) {
    return 2.f * near * far / (far + near - (2.f * depthValue - 1.f) * (far - near));
}

//apply distortion using dudv map
vec2 ApplyDistortion() {
    vec2 distorted = texture(uDudvmap, vec2(vTextureCoords.x + uMoveFactor, vTextureCoords.y)).rg * 0.15f;
    return vec2(distorted.x, distorted.y + uMoveFactor);
}

//calculate specular
vec3 CalculateSpecular(vec3 unitNormal, vec3 viewVector) {
    //specular calculation
    vec3 reflectedLight = reflect(uLightDirection, unitNormal);
    float alignment = dot(reflectedLight, -viewVector); 
    float shininessFactor = clamp(uWaterShininess / 100.f, 0.01f, 1.f); 
    float specular = smoothstep(1.f - shininessFactor, 1.f, alignment); 
    vec3 specularColor =  mix(uSunColor, vec3(1.f), 0.6f);

    return (specular * specularColor * reflectivity  * uSunIntensity * 1.5f);
}

void main() {
    vec3 ambient = waterColor.xyz * 0.5f;

    vec2 ndc = (vClipSpace.xy / vClipSpace.w) * 0.5f + 0.5f;

    //texture coordinates for reflection and refraction
    vec2 refractionTexCoords = vec2(ndc.x, ndc.y);
    vec2 reflectionTexCoords = vec2(ndc.x, -ndc.y);

    //depth 
    float near = 10.f;
    float far = 5000.f;
    float floorDepth = texture(uDepthmap, refractionTexCoords).r;
    float floorDistance = CalculateDepth(near, far, floorDepth);
    float waterDepth = gl_FragCoord.z;
    float waterDistance = CalculateDepth(near, far, waterDepth);
    float depth = floorDistance - waterDistance;

    //apply distortion 
    vec2 distortedTexCoords = ApplyDistortion();
    vec2 totalDistortion = (texture(uDudvmap, distortedTexCoords).rg * 2.f - 1.f) * 0.03f;
    refractionTexCoords += totalDistortion;
    reflectionTexCoords += totalDistortion;

    refractionTexCoords = clamp(refractionTexCoords, 0.01f, 0.99f);
    reflectionTexCoords.x = clamp(reflectionTexCoords.x, 0.01f, 0.99f);
    reflectionTexCoords.y = clamp(reflectionTexCoords.y, -0.01f, -0.99f);

    vec4 reflectionColor = texture(uReflectionTexture, reflectionTexCoords);
    vec4 refractionColor = texture(uRefractionTexture, refractionTexCoords);

    //murky water effect
    float murkinessFactor = clamp(depth / 70.f, 0.f, 1.f);
    refractionColor = mix(refractionColor, murkinessColor, murkinessFactor);

    //calculate water normal and viewVector
    vec3 normalmapColor = texture(uNormalmap, distortedTexCoords).xyz;
	vec3 unitNormal = normalize(vec3(normalmapColor.r * 2.f - 1.f, normalmapColor.b * 3.f, normalmapColor.g * 2.f - 1.f));

    //refractive factor calculations
    vec3 viewVector = normalize(vVertexToCamera);
    float refractiveFactor = pow(dot(viewVector, unitNormal), .5f);
    refractiveFactor = clamp(refractiveFactor, 0.f, 1.f);

    //diffuse calculations
    float diffuseFactor = max(dot(unitNormal,normalize(uLightDirection)), 0.f); //the more front facing towards light, the brighter
	vec3 diffuse = ambient * diffuseFactor * uBrightness;

    //specular factor calculations
    vec3 specularFactor = CalculateSpecular(unitNormal, viewVector);

    //sun highlight calculations
    float sunHighlight = pow(diffuseFactor, uSunFalloff * .25f);
    vec3 sunlightEffect = uSunColor * sunHighlight * uSunIntensity;

    vec3 totalLighting = max(ambient + diffuse + sunlightEffect, 0.f);
    vec4 color = mix(reflectionColor, refractionColor, refractiveFactor);
    oFragColor = mix(color, waterColor, 0.1f) * vec4(totalLighting, 1.f) + vec4(specularFactor, 1.f);

    //ensure alpha of water near the edges fade out
    oFragColor.a = clamp(depth / 10.f, 0.f, 1.f);
}
