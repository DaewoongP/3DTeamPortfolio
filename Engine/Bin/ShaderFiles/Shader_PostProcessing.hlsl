
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_PostProcessingTexture;
texture2D g_BloomTexture;
// 가우시안 필터 1차원 배열형태
float BlurWeights[23] =
{
    0.0011, 0.0123, 0.0561, 0.1353, 0.278, 0.3001, 0.4868, 0.6666, 0.7261, 0.8712, 0.9231,
    0.9986, 0.9231, 0.8712, 0.7261, 0.6666, 0.4868, 0.3001, 0.278, 0.1353, 0.0561, 0.0123, 0.0011
};


/* Sampler State */
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

/* Raterizer State */
RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
    FillMode = Solid;
    CullMode = front;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};

/* Depth_Stencil State */

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_Depth_Disable
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

/* Blend State */
BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_PostProcessingTexture.Sample(LinearSampler, In.vTexUV);
    vector Bloom = g_BloomTexture.Sample(LinearSampler, In.vTexUV);
    Out.vColor += Bloom;
    return Out;
}

technique11 DefaultTechnique
{
    pass PostProcessing
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}