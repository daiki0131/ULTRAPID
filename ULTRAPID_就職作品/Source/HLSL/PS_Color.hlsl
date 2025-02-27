struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
};

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    // カラーをそのまま返す
    return input.color;
}