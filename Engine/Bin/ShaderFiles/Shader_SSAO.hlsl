
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_PostProcessingTexture;
//SSAO
float g_fRadius = 0.001f;
float g_fFar = 10000.f;
float g_fFalloff = 0.000002f;
float g_fStrength = 0.0007f;
float g_fTotStrength = 1.38f;
float g_fInSamples = 1.f / 16.f;


texture2D g_DepthTexture;
texture2D g_NormalTexture;
//ray를 뻗어나가면서 무작위로판단하기위해 rand값이 필요함 같이던져줘야할듯
float3 g_vRandom;

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


//struct VS_IN
//{
//    float3 vNormal : NORMAL;
//    float2 vTexUV : TEXCOORD0;
    
//};

//struct VS_OUT
//{
//    float4 vAmbient : SV_TARGET0;
    
//};

//VS_OUT VS_MAIN(VS_IN In)
//{
//    VS_OUT Out = (VS_OUT) 0;

//    matrix matWV, matWVP;

//    matWV = mul(g_WorldMatrix, g_ViewMatrix);
//    matWVP = mul(matWV, g_ProjMatrix);

//    //Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
//    Out.vTexUV = In.vTexUV;

//    return Out;
//}

//struct PS_IN
//{
//    float4 vPosition : SV_POSITION;
//    float2 vTexUV : TEXCOORD0;
//};

//struct PS_OUT
//{
//    float4 vColor : SV_TARGET0;
//};

//PS_OUT PS_MAIN(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;
   
//    float3 normal = normalize(g_NormalTexture.Sample(LinearSampler, In.vTexUV).xyz * 2.0 - 1.0); // Retrieve and normalize the normal
//    float depth = g_DepthTexture.Sample(LinearSampler, In.vTexUV).r * g_fFar; // Retrieve the depth value and convert to world space
    
//    float3 RandDir = normalize(g_vRandom * 2.f - 1.f);
//    float3 samplePos = In.vPosition.xyz + RandDir * (g_fRadius + depth * g_fFalloff);
    
//    float4 samplePosClip = mul(float4(samplePos, 1.f), g_ProjMatrix);
//    float sampleDepth = samplePosClip.z / samplePosClip.w;
    
//    float occlusion = (sampleDepth - depth) < g_fRadius ? 1.0 : 0.0;
    
//    float ao = 1.0 - pow(occlusion, g_fStrength);
//    ao = pow(ao, g_fTotStrength);
    
//    float3 postProcessingColor = g_PostProcessingTexture.Sample(LinearSampler, In.vTexUV).rgb;
//    float3 finalColor = postProcessingColor * ao;
    
//    Out.vColor = (finalColor, 1.f);
    
//    return Out;
//}

struct VS_IN
{
    float3 vNormal : NORMAL;
    float fDepth : TEXCOORD0;
    float fViewZ : TEXCOORD1;
    float2 vTexUV : TEXCOORD2;
  
};

struct VS_OUT
{
    float4 vAmbient : TEXCOORD0;
  
};
float3 RandomNormal(float2 tex)
{
    float noiseX = (frac(sin(dot(tex, float2(15.8989f, 76.132f) + 1.f)) * 4636.23423f));
    float noiseY = (frac(sin(dot(tex, float2(11.4575f, 62.143f) + 1.f)) * 5336.26242f));
    float noiseZ = (frac(sin(dot(tex, float2(13.2346f, 63.734f) + 1.f)) * 1536.24223f));
    return normalize(float3(noiseX, noiseY, noiseZ));
    
}

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    //float3 vRay;
    //float3 vReflect;
    //float2 vRandomUV;
    //float fOccNorm;
    
    //int iColor = 0;
    //for (int i = 0; i < 16;++i)
    //{
    //    vRay = reflect(RandomNormal(In.vTexUV), g_vRandom);
    //    vReflect = normalize(reflect(vRay, In.vNormal)) * g_fRadius;
    //    vReflect.x *= -1.f;
    //    vRandomUV = In.vTexUV + vReflect.xy;
    //    fOccNorm = Sample(g_DepthTexture, vRandomUV).g * g_fFar *In.fViewZ;
        
    //    if(fOccNorm<=In.fDepth+0.0003f)
    //    {
    //        ++iColor;
    //    }
        
    //}
       
    //Out.vAmbient = abs((iColor / 16.f) - 1);
    return Out;
}
struct PS_IN
{
    float4 vAmbient : TEXCOORD0;
};
struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out= (PS_OUT) 0;
    
    //float4 vDepth = Sample(g_DepthTexture,In.v)
    
    return Out;
}




technique11 DefaultTechnique
{
    pass SSAO
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