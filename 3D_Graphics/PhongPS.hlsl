cbuffer LightCBuf
{
	float3 lightPos;
};

static const float attConst = 1.0f;
static const float attLin = 0.027f;
static const float attQuad = 0.0028f;
static const float3 materialColor = { 0.9f,0.5f,0.2f };
static const float3 ambient = { 0.05f,0.05f,0.05f };
static const float3 diffuseColor = { 1.0f,7.0f,1.0f };
static const float diffuseIntensity = 1.0f;

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