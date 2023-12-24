#if !defined(GAMETEXTSHADERS_H)
#define GAMETEXTSHADERS_H

#include <string>
namespace game
{
    std::string gameTextShaders = std::string(\
        "struct PSInput"\
        "{"\
        "float4 position : SV_POSITION;"\
        "float4 color : COLOR;"\
        "float2 uv : TEXCOORD;"\
        "};"\
        "Texture2D g_texture : register(t0);"\
        "SamplerState g_sampler : register(s0);"\
        "float4 mainPS(PSInput input) : SV_TARGET"\
        "{"\
        "return g_texture.Sample(g_sampler, input.uv) * input.color;"\
        "}"\
        "PSInput mainVS(float4 position : POSITION, float4 color : COLOR, float2 uv : TEXCOORD)"\
        "{"\
        "PSInput result;"\
        "result.position = position;"\
        "result.color = color;"\
        "result.uv = uv;"\
        "return result;"\
        "}"\
    );
}

#endif

