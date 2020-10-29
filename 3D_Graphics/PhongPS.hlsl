cbuffer LightCBuf
{
    float3 diffuseColor;
    float3 ambient;
	float3 lightPos;
    float attConst;
    float attLin;
    float attQuad;
    float diffuseIntensity;
	
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
	// Diffuse attenuation
    const float att = 1 / (attConst + attLin * distToL + attQuad * (distToL * distToL)); // Inverse relationship between distance and light intensity
	// Diffuse intensity
	const float3 diffuse = diffuseColor * diffuseIntensity * att * max( 0.0f,dot( dirToL,n ) );
	// Final color
	return float4(saturate( diffuse + ambient ) * materialColor, 1.0f);
}