struct Input
{
	float4 _Position : SV_POSITION;
	float4 _Color : COLOR0;
};

struct Output
{
	float4 _Color : SV_TARGET;
};


void main(in Input input, out Output output)
{
	output._Color = input._Color;
}