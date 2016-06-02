cbuffer VertexShader_PerFrame : register(b0)
{
	float _TextureWidth;
	float _TextureHeight;
	float _Seconds;
	float _DeltaTime;
	uint _FrameIndex;
}

cbuffer VertexShader_PerObject : register(b1)
{
	float4x3 _World;
	float4x4 _WorldViewProjection;
}

struct Input
{
	float4 _Position : POSITION;
	float4 _Color : COLOR;
};

struct Output
{
	float4 _Position : SV_POSITION;
	float4 _Color : COLOR0;
};

void main( in Input input, out Output output )
{
	output._Position = mul(input._Position, _WorldViewProjection);
	output._Color = input._Color;
}
