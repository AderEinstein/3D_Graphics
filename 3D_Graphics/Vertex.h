#pragma once
#include "Graphics.h"
#include <type_traits>
#include <vector>

namespace AderVertex
{
	struct BGRAColor
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	class VertexLayout
	{
	public:

		enum ElementType
		{
			Position3D, Position2D, Normal, Texture2D, Float3Color, Float4Color, BGRAColor
		};
		template <ElementType> struct Map;
		template<> struct Map<Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
		};
		template <> struct Map<Position2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
		};
		template<> struct Map<Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
		};
		template<> struct Map<Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
		};
		template<> struct Map<Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
		};
		template<> struct Map<Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
		};
		template<> struct Map<BGRAColor>
		{
			using SysType = AderVertex::BGRAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
		};
		
		class Element
		{
		public:
			Element(ElementType type, size_t offset)
				:
				type(type),
				offset(offset)
			{}
			ElementType GetType() const noexcept(!IS_DEBUG)
			{
				return type;
			}
			template<ElementType Type>
			size_t Size() const noexcept(!IS_DEBUG)
			{
				return SizeOf(type);
			}
			static constexpr size_t SizeOf(ElementType type) noexcept(!IS_DEBUG)
			{
				switch (type)
				{
				case Position2D:
					return sizeof(Map<Position2D>::SysType);
				case Position3D:
					return sizeof(Map<Position3D>::SysType);
				case Texture2D:
					return sizeof(Map<Texture2D>::SysType);
				case Normal:
					return sizeof(Map<Normal>::SysType);
				case Float3Color:
					return sizeof(Map<Float3Color>::SysType);
				case Float4Color:
					return sizeof(Map<Float4Color>::SysType);
				case BGRAColor:
					return sizeof(Map<BGRAColor>::SysType);
				}
				assert("Invalid element type" && false);
				return 0u;
			}
			size_t GetOffset() const noexcept(!IS_DEBUG)
			{
				return offset;
			}
			size_t GetOffsetAfter() const noexcept(!IS_DEBUG)
			{
				return offset + SizeOf(type);
			}
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept(!IS_DEBUG)
			{
				switch (type)
				{
				case Position2D:
					return GenerateDesc<Position2D>(GetOffset());
				case Position3D:
					return GenerateDesc<Position3D>(GetOffset());
				case Texture2D:
					return GenerateDesc<Texture2D>(GetOffset());
				case Normal:
					return GenerateDesc<Normal>(GetOffset());
				case Float3Color:
					return GenerateDesc<Float3Color>(GetOffset());
				case Float4Color:
					return GenerateDesc<Float4Color>(GetOffset());
				case BGRAColor:
					return GenerateDesc<BGRAColor>(GetOffset());
				}
				assert("Invalid element type" && false);
				return { "INVALID",0,DXGI_FORMAT_UNKNOWN,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 };
			}
		private:
			template<ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noexcept(!IS_DEBUG)
			{
				return { Map<type>::semantic,0,Map<type>::dxgiFormat,0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0 };
			}
			ElementType type;
			size_t offset;
		};

		template<ElementType Type>
		const Element& Resolve() const noexcept(!IS_DEBUG)
		{
			for (const auto& elmt : elements)
			{
				if (elmt.GetType == Type)
				{
					return elmt;
				}
			}
			assert("Could not resolve element type" && false);
			return elements.front();
		}
		const Element& ResolveByIndex(size_t i) const noexcept(!IS_DEBUG)
		{
			return elements[i];
		}
		size_t GetElementCount() const noexcept(!IS_DEBUG)
		{
			return elements.size();
		}
		size_t Size() const noexcept(!IS_DEBUG)
		{
			return elements.empty()? 0u : elements.back().GetOffsetAfter();
		}
		VertexLayout& Append(const ElementType& type) noexcept(!IS_DEBUG)
		{
			elements.emplace_back(type, Size());
			return *this;
		}
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept(!IS_DEBUG)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
			desc.reserve(GetElementCount());
			for (const auto& e : elements)
			{
				desc.push_back(e.GetDesc());
			}
			return desc;
		}
		
	private:
		std::vector<Element> elements;
	};



class Vertex // Defines a proxy/view over the layout and data of a given vertex
{
	friend class VertexBuffer;
protected:
	Vertex(char* pData, const VertexLayout& layout) noexcept(!IS_DEBUG)
		:
		pData(pData),
		layout(layout)
	{
		assert(pData != nullptr);
	}

public:
	template<VertexLayout::ElementType type>
	auto& Attr() noexcept(!IS_DEBUG)
	{
		using namespace DirectX;
		const auto& element = layout.Resolve<type>();
		char* pAttribute = pData + element.GetOffset();
		return *reinterpret_cast<typename VertexLayout::Map<type>::Systype*> (pAttribute);
	}
	template<typename T>
	void SetAttributeByIndex(size_t i, T&& val) noexcept(!IS_DEBUG)
	{
		const auto& element = layout.ResolveByIndex(i);
		auto pAttribute = pData + element.GetOffset();
		switch (element.GetType())
		{
		case VertexLayout::Position2D:
			SetAttribute<VertexLayout::Position2D>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Position3D:
			SetAttribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Texture2D:
			SetAttribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Normal:
			SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float3Color:
			SetAttribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float4Color:
			SetAttribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::BGRAColor:
			SetAttribute<VertexLayout::BGRAColor>(pAttribute, std::forward<T>(val));
			break;
		default:
			assert("Bad element type" && false);
		}
	}

private:
	template<typename First, typename ...Rest>
	// Enables parameter pack setting of multiple parameters by element index
	void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) noexcept(!IS_DEBUG)
	{
		SetAttributeByIndex(i, std::forward<First>(first));
		SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
	}
	// Helper to reduce code duplication in SetAttributeByIndex
	template<VertexLayout::ElementType DestLayoutType, typename SrcType>
	void SetAttribute(char* pAttribute, SrcType&& val) noexcept(!IS_DEBUG)
	{
		using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
		if constexpr (std::is_assignable<Dest, SrcType>::value)
		{
			*reinterpret_cast<Dest*>(pAttribute) = val;
		}
		else
		{
			assert("Parameter attribute type mismatch" && false);
		}
	}

private:
	char* pData = nullptr;
	const VertexLayout& layout;
};



class ConstVertex
{
public:
	ConstVertex(const Vertex& v) noexcept(!IS_DEBUG)
		:
		vertex(v)
	{}
	template<VertexLayout::ElementType Type>
	const auto& Attr() const noexcept(!IS_DEBUG)
	{
		return const_cast<Vertex&>(vertex).Attr<Type>();
	}
private:
	Vertex vertex;
};



class VertexBuffer
{
public:
	VertexBuffer(VertexLayout layout) noexcept(!IS_DEBUG)
		:
		layout(std::move(layout))
	{}
	const char* GetData() const noexcept(!IS_DEBUG)
	{
		return buffer.data();
	}
	const VertexLayout& GetLayout() const noexcept
	{
		return layout;
	}
	size_t Size() const noexcept(!IS_DEBUG)
	{
		return buffer.size() / layout.Size();
	}
	size_t SizeBytes() const noexcept(!IS_DEBUG)
	{
		return buffer.size();
	}
	template<typename ...Params>
	void EmplaceBack(Params&&... params) noexcept(!IS_DEBUG)
	{
		assert(sizeof...(params) == layout.GetElementCount() && "Param count doesn't match number of vertex elements");
		buffer.resize(buffer.size() + layout.Size());
		Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
	}
	Vertex Back() noexcept(!IS_DEBUG)
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
	}
	Vertex Front() noexcept(!IS_DEBUG)
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data(),layout };
	}
	Vertex operator[](size_t i) noexcept(!IS_DEBUG)
	{
		assert(i < Size());
		return Vertex{ buffer.data() + layout.Size() * i,layout };
	}
	ConstVertex Back() const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer*>(this)->Back();
	}
	ConstVertex Front() const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}
	ConstVertex operator[](size_t i) const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}

private:
	std::vector<char> buffer;
	VertexLayout layout;
};
}