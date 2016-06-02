#include "EnginePCH.h"
#include "GraphicsManager.h"

#include <d3d11_1.h>

namespace Eng
{
	void GraphicsManager::CommitConstantBuffers()
	{
		bool update_v1 = false;
		bool update_p0 = false;
		XMVECTOR det;
		if (_ViewDirty)
		{
			_ViewDirty = 0;
			update_v1 = true;
			update_p0 = true;
			_InverseView = XMMatrixInverse(&det, _View);
		}
		else if (_InverseViewDirty)
		{
			_InverseViewDirty = 0;
			update_v1 = true;
			update_p0 = true;
			_View = XMMatrixInverse(&det, _InverseView);
		}
		if (_WorldDirty)
		{
			_WorldDirty = 0;
			update_v1 = true;
		}
		if (_ProjectionDirty)
		{
			_ProjectionDirty = 0;
			update_v1 = true;
			update_p0 = true;
		}
		if (_VertexShader0Dirty)
		{
			D3D11_MAPPED_SUBRESOURCE MappedResource;
			HRESULT hr = Engine::_ImmediateContext->Map(ConstantBuffer[ECB_VertexShader0], 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			if (FAILED(hr))
			{
				ENGINE_LOG(Error, "Unable to map constant buffer for writing");
			}
			memcpy(MappedResource.pData, &_VertexShader0, sizeof(_VertexShader0));
			Engine::_ImmediateContext->Unmap(ConstantBuffer[ECB_VertexShader0], 0);
		}
		if (update_v1)
		{
			XMMATRIX world_t = XMMatrixTranspose(_World);
			_VertexShader1._World[0] = world_t.r[0];
			_VertexShader1._World[1] = world_t.r[1];
			_VertexShader1._World[2] = world_t.r[2];
			XMMATRIX world_view_projection = _World * _View * _Projection;
			_VertexShader1._WorldViewProjection = XMMatrixTranspose(world_view_projection);
			D3D11_MAPPED_SUBRESOURCE MappedResource;
			HRESULT hr = Engine::_ImmediateContext->Map(ConstantBuffer[ECB_VertexShader1], 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			if (FAILED(hr))
			{
				ENGINE_LOG(Error, "Unable to map constant buffer for writing");
			}
			memcpy(MappedResource.pData, &_VertexShader1, sizeof(_VertexShader1));
			Engine::_ImmediateContext->Unmap(ConstantBuffer[ECB_VertexShader1], 0);
		}
		if (update_p0)
		{
			_PixelShader0._InverseViewProjection = XMMatrixTranspose(_InverseView * _Projection);
			_PixelShader0._InverseProjection = XMMatrixTranspose(XMMatrixInverse(&det, _Projection));
			D3D11_MAPPED_SUBRESOURCE MappedResource;
			HRESULT hr = Engine::_ImmediateContext->Map(ConstantBuffer[ECB_PixelShader0], 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
			if (FAILED(hr))
			{
				ENGINE_LOG(Error, "Unable to map constant buffer for writing");
			}
			memcpy(MappedResource.pData, &_PixelShader0, sizeof(_PixelShader0));
			Engine::_ImmediateContext->Unmap(ConstantBuffer[ECB_VertexShader1], 0);
		}
	}

	GraphicsManager::GraphicsManager()
	{
		D3D11_BUFFER_DESC desc = { 0 };
		HRESULT hr = S_OK;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		unsigned byte_width_array[] =
		{
			(unsigned)sizeof(VS_0),
			(unsigned)sizeof(VS_1),
			(unsigned)sizeof(PS_0),
		};
#if ENGINE_USE_DEBUG_DIRECTX
		const char* buffer_names[] =
		{
			"VS_0",
			"VS_1",
			"PS_0",
		};
#endif
		for (int iter = 0; iter < ECB_Num; ++iter)
		{
			desc.ByteWidth = byte_width_array[iter];
			hr = Engine::_Device->CreateBuffer(&desc, nullptr, ConstantBuffer + iter);
			if (FAILED(hr))
			{
				ENGINE_LOG(Error, "Unable to create constant buffer");
			}
			ENGINE_SET_DEBUG_OBJECT_NAME(ConstantBuffer[iter], buffer_names[iter]);
		}
	}
	GraphicsManager::~GraphicsManager()
	{
		for (int iter = ECB_Num - 1; iter >= 0; --iter)
		{
			SAFE_RELEASE(ConstantBuffer[iter]);
		}
	}
}