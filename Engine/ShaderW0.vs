struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};

cbuffer ConstantBuffer : register(b0)
{
    row_major float4x4 mvp; // Row-major·Î ÀÐ±â
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.Position = mul(mvp, input.Position);


    output.Color = input.Color.rgb;
    return output;
}