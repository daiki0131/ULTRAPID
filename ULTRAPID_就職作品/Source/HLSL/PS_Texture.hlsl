struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
};

Texture2D myTexture : register(t0);
SamplerState mySampler : register(s0);

float4 ps_main(PS_INPUT input) : SV_Target
{
    float4 color = myTexture.Sample(mySampler, input.tex);
    // テクスチャ色とマテリアル色を混ぜる
    return color * input.color;
}