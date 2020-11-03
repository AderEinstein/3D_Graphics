#include "TexturedBox.h"
#include "BindableBase.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"
#include "Cube.h"
#include "GfxThrowMacros.h"

TexturedBox::TexturedBox( Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist )
	:
	r( rdist( rng ) ),
	droll( ddist( rng ) ),
	dpitch( ddist( rng ) ),
	dyaw( ddist( rng ) ),
	dphi( odist( rng ) ),
	dtheta( odist( rng ) ),
	dchi( odist( rng ) ),
	chi( adist( rng ) ),
	theta( adist( rng ) ),
	phi( adist( rng ) )
{
	if( !IsStaticInitialized() )
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
			dx::XMFLOAT2 tc;
		};
		auto model = Cube::MakeIndependentTextured<Vertex>();
		model.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<Sampler>(gfx));

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs = std::make_unique<VertexShader>( gfx,L"TexturedPhongVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"TexturedPhongPS.cso" ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,model.indices ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	//////////////////////////////// Per Instance Attributes //////////////////////////////////

	// Select a random texture for each textured box
	if (rdist(rng) < 10)
		AddBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\box texture dirty metal.jpg")));
	else if(rdist(rng) < 14)
		AddBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\box texture framed.jpg")));
	else if(rdist(rng) < 18)
		AddBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\box texture wood.jpg")));
	else
		AddBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\box texture metal.jpg")));

	// Model deformation transform 
	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);

	// Bind model transformation
	AddBind( std::make_unique<TransformCBuffer>( gfx,*this ) );
}

void TexturedBox::Update( float dt ) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	chi += dchi * dt;
}

dx::XMMATRIX TexturedBox::GetTransformXM() const noexcept
{
	return dx::XMLoadFloat3x3(&mt) *
		dx::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
		dx::XMMatrixTranslation( r,0.0f,0.0f ) *
		dx::XMMatrixRotationRollPitchYaw( theta,phi,chi );
}
