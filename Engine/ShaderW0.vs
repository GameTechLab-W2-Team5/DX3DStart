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
    float4 Offset;      // float3 �� float4
    float4 Scale;       // float3 �� float4
    // Padding ���ŵ�
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    // ���� Scale�� Offset�� �ùٸ��� ���޵�
    float4 transformed = float4(input.Position.xyz * Scale.xyz + Offset.xyz, 1.0f);
    //float4 transformed = float4(input.Position.xyz, 1.0f);
    output.Position = transformed;
    output.Color = input.Color.rgb;
    return output;
}