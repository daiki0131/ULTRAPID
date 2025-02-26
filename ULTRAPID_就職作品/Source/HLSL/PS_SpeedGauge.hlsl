cbuffer GaugeBuffer : register(b0)
{
    float4 colors[4];
    float4 thresholds;
    float fillRatio; 
    float lowAlphaValue;
    float3 padding;
}

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 ps_main(PSInput input) : SV_TARGET
{
    float x = input.uv.x;

    // 色の選択
    float4 col = colors[3]; // デフォルトで最後の色
    for (int i = 0; i < 3; i++)
    {
        if (x >= thresholds[i] && x < thresholds[i + 1])
        {
            float t = (x - thresholds[i]) / (thresholds[i + 1] - thresholds[i]);
            col = lerp(colors[i], colors[i + 1], t); // 線形補間
            break;
        }
    }

    float alpha = step(x, fillRatio);
    alpha = alpha * 1.0 + (1.0 - alpha) * lowAlphaValue;
    return float4(col.rgb, alpha * col.a);
}