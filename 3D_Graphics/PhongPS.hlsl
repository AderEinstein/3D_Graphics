cbuffer LightCBuf
{
    float3 diffuseColor;
    float3 ambient;
	float3 lightPos;
    float attConst;
    float attLin;
    float attQuad;
    float diffuseIntensity;
    float specularIntensity;
    float specularPower;
};

cbuffer ObjectColorCBuf
{
    float3 materialColor;
};

float4 main( float3 worldPos : Position,float3 n : Normal ) : SV_Target
{
	// Fragment to light vector data
	const float3 vToL = lightPos - worldPos;
	const float distToL = length( vToL );
	const float3 dirToL = vToL / distToL;
	// Attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL)); // Inverse relationship between distance and light intensity
	// Diffuse intensity
	const float3 diffuse = diffuseColor * diffuseIntensity * att * max( 0.0f,dot( dirToL,n ) );
    // Reflected light vector
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
	// Calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function, and attenuate specualar result
    const float3 specular = diffuseColor * diffuseIntensity * att * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
	// Final color
	return float4(saturate( diffuse + specular + ambient ) * materialColor, 1.0f);
}