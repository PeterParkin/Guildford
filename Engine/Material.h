#pragma once

struct ID3D11VertexShader;
struct ID3D11PixelShader;

namespace Eng
{
	struct Material
	{
		ID3D11VertexShader* _VertexShader;
		ID3D11PixelShader* _PixelShader;
	};
}