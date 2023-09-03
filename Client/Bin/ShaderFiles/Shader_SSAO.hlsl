
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_PostProcessingTexture;
//SSAO
float g_fRadius = 0.00001f;
float g_fFar = 100.f;
float g_fFalloff = 0.000002f;
float g_fStrength = 0.0007f;
float g_fTotStrength = 1.38f;
float g_fInSamples = 1.f / 16.f;


texture2D g_DepthTexture;
texture2D g_NormalTexture;
texture2D g_SSAOTexture;
texture2D g_BlurTexture;

//ray�� ������鼭 ���������Ǵ��ϱ����� rand���� �ʿ��� ���̴�������ҵ�
float3 g_vRandom[13];

float3 g_Ran[13] =
{
    float3(0.2024537f, 0.841204f, -0.9060241f),
	float3(-0.221324f, 0.324325f, -0.8234234f),
	float3(0.8724724f, 0.8547973f, -0.43252611f),
	float3(0.2698734f, 0.5684943f, -0.12515022f),
	float3(0.26482924f, 0.236820f, 0.72384287f),
	float3(0.20348342f, 0.234832f, 0.23682923f),
	float3(-0.0012315f, 0.8234823f, 0.23483244f),
	float3(-0.2342863f, 0.234982f, -0.00001524f),
	float3(-0.3426888f, 0.780742f, -0.8349823f),
	float3(-0.5234832f, 0.8291234f, 0.23941929f),
	float3(0.90889192f, 0.8123121f, -0.12812992f),
	float3(0.4520239f, 0.1201011f, -0.82943914f),
    float3(0.2024539f, -0.7101201f, 0.29143293f)
};

float BlurWeights[23] =
{
    0.0011, 0.0123, 0.0561, 0.1353, 0.278, 0.3001, 0.4868, 0.6666, 0.7261, 0.8712, 0.9231,
    0.9986, 0.9231, 0.8712, 0.7261, 0.6666, 0.4868, 0.3001, 0.278, 0.1353, 0.0561, 0.0123, 0.0011
};
float total = 11.4776f;



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
sampler BlurSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
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
    float4 vPosition : SV_Position;
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

float3 RandNormal(float2 vTexUV)
{
    float x = frac(sin(dot(vTexUV, float2(15.8989f, 76.132f) * 1.f)) * 46336.2364f);
    float y = frac(sin(dot(vTexUV, float2(11.5234f, 62.128f) * 2.f)) * 24636.2412f);
    float z = frac(sin(dot(vTexUV, float2(13.2726f, 63.723f) * 3.f)) * 56372.3263f);
    
    return normalize(float3(x, y, z));
}


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    
    if (vNormalDesc.a != 0.f)
    {
        Out.vColor = (1.f, 1.f, 1.f, 1.f);
        return Out;
    }
       vNormalDesc = normalize(vNormalDesc * 2.f - 1.f);
    //vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
        float fViewZ = vDepthDesc.r * g_fFar; //����Ʈ������ ����
        float vDepth = vDepthDesc.g * g_fFar * fViewZ; // ���忡���� ��������
    
        float3 vRay;
        float3 vReflect;
        float2 vRandomUV;
        float fOccNorm;
    
        int iColor = 0;
    
        for (int i = 0; i < 13; i++)
        {
            vRay = reflect(RandNormal(In.vTexUV), g_vRandom[i]);
            vReflect = normalize(reflect(normalize(vRay), normalize(vNormalDesc.rgb))) * g_fRadius;
            vReflect.x *= -1.f;
            vRandomUV = In.vTexUV + vReflect.xy;
             fOccNorm = g_DepthTexture.Sample(LinearSampler, vRandomUV).g * g_fFar * fViewZ;
            if (fOccNorm <= vDepth + 0.0005f)
                ++iColor;
                  
        }
    
        float4 vAmbient = abs((iColor / 13.f) - 1);
   
        Out.vColor = 1.f - vAmbient;
        return Out;
    }


PS_OUT PS_MAIN_BLURX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector SSAO = g_SSAOTexture.Sample(LinearSampler, In.vTexUV);
    //int mip = 0;
    //int width, height, levels;

    //g_SSAOTexture.GetDimensions(mip, width, height, levels);
    //float dx = 1.0f / width;
    //float dy = 1.0f / height;
    
    //float4 color = float4(0.f, 0.f, 0.f, 0.f);
    //int nrIterations = 5;
    
    //for (int i = 0; i < nrIterations; ++i)
    //{
    //    for (int j = 0; j < nrIterations; ++j)
    //    {
    //        float2 offset = float2((dx * 2 * i) - dx, (dy * 2 * j) - dy);
    //        color += g_SSAOTexture.Sample(BlurSampler, In.vTexUV + offset);
    //    }
    //}
    //color /= nrIterations * nrIterations;
    //Out.vColor = color;
    
    
    int mip = 0;
    int width, height, levels;
    g_SSAOTexture.GetDimensions(mip, width, height, levels);
    float dx = 1.0f / width;
    float dy = 1.0f / height;
    
    float2 UV = 0;
    
    for (int i = -11; i < 11; ++i)
    {
        UV = In.vTexUV + float2(dx * i, 1);
        Out.vColor += BlurWeights[11 + i] * SSAO;
    }
    Out.vColor /= total;
    //Out.vColor = (SSAO.xyz, 0.f);
    
    
    return Out;
}
PS_OUT PS_MAIN_BLURY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector SSAO = g_SSAOTexture.Sample(BlurSampler, In.vTexUV);
    
    int mip = 0;
    int width, height, levels;
    g_SSAOTexture.GetDimensions(mip, width, height, levels);
    float dx = 1.0f / width;
    float dy = 1.0f / height/2;
    
    float2 UV = 0;
   
    for (int i = -11; i < 11; ++i)
    {
        UV = In.vTexUV + float2(0,dy * i);
        Out.vColor += BlurWeights[11 + i] * SSAO;
    }
    Out.vColor /= total;
    
    
    
    return Out;
}



technique11 DefaultTechnique
{
    pass SSAO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass BlurX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_BLURX();
    }
    pass BlurY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_BLURY();
    }


}