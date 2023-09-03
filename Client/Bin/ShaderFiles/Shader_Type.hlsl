const static float PI = 3.14159265359;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_vLightView;
matrix g_vLightProj;


texture2D g_Texture;
vector g_vCamPosition;
float g_fCamFar;

texture2D g_NormalTexture;
texture2D g_DiffuseTexture;
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;
texture2D g_ShadowTexture;
texture2D g_vLightDepthTexture;

texture2D g_SSAOTexture;
texture2D g_BlurTexture;

float3 g_Diffuse = float3(1.f, 1.f, 1.f);

float g_Metallic = 0.5f;
float g_Roughness = 0.5f;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
float g_fSpotPower;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;


vector g_vMtrlAmbient = vector(0.5f, 0.5f, 0.5f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);



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

BlendState BS_BlendOne
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};

matrix MyMatrixLookAtLH(float4 vEye, float4 vAt)
{
    matrix ViewMatrix = matrix(
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f);
    
    vector vLook = float4(normalize(vAt.xyz - vEye.xyz), 0.f);
    vector vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)), 0.f);
    vector vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)), 0.f);
    
    ViewMatrix = matrix(vRight, vUp, vLook, float4(0.f, 0.f, 0.f, 1.f));
    matrix TransposeViewMatrix = transpose(ViewMatrix);
    
    vector vPosition = float4(
    -1.f * dot(vEye, vRight),
    -1.f * dot(vEye, vUp),
    -1.f * dot(vEye, vLook),
    1.f);
    
    TransposeViewMatrix._41 = vPosition.x;
    TransposeViewMatrix._42 = vPosition.y;
    TransposeViewMatrix._43 = vPosition.z;
    TransposeViewMatrix._44 = vPosition.w;

    return TransposeViewMatrix;
}

struct VS_IN_PBR
{
    float4 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
    float3 vNormal : NORMAL;
};

struct VS_OUT_PBR
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
    float3 vNormal : NORMAL;
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
VS_OUT_PBR VS_MAIN_PBR(VS_IN_PBR In)
{
    VS_OUT_PBR Out = (VS_OUT_PBR) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(In.vPosition, matWVP);

    return Out;
}

VS_OUT_PBR VS_MAIN_SHADOW(VS_IN_PBR In)
{
    VS_OUT_PBR Out = (VS_OUT_PBR) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(In.vPosition, matWVP);
    Out.vTexUV = In.vTexUV;

    return Out;
}



struct PS_IN
{
   
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
    float3 vNormal : NORMAL;
};

struct PS_IN_SHADOW
{
   
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
    float3 vNormal : NORMAL;
};


struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_PBR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

    vector Normalied_Normal = normalize(vector(In.vNormal, 0.f));
    vector LookAt = normalize(g_vCamPosition - In.vPosition);
    vector LightLook = normalize(g_vLightPos - In.vPosition);
    vector MiddleVector = normalize(LightLook + LookAt);
    
    float NdotL = max(dot(Normalied_Normal, LightLook), 0.0);
    float NdotM = max(dot(Normalied_Normal, MiddleVector), 0.0);
    
    vector diffuseTerm = /*(g_Diffuse) */(NdotL * g_vLightDiffuse);
    vector specularTerm = pow(NdotM, g_Roughness) * g_vLightSpecular;

    
    Out.vColor = vDiffuse * (diffuseTerm * g_vLightAmbient) * vShade + vSpecular * specularTerm;

    return Out;
}

PS_OUT PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

	//depthdesc y 는 rgba값이라고 생각하면된다.
    float fViewZ = vDepthDesc.y * g_fCamFar;
    vector vPosition;
    if (fViewZ == 0)
        discard;
	/* 투영스페이스 상의 위치 */
    vPosition.x = In.vTexUV.x * 2.f - 1.f;
    vPosition.y = In.vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 뷰스페이스 상의 위치. */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치. */
    vPosition = mul(vPosition, g_ViewMatrixInv);



    vPosition = mul(vPosition, g_vLightView);

    vPosition = mul(vPosition, g_vLightProj);
	//w 나누기.
    vPosition = vPosition / vPosition.w;
	//광원으로의깊이값.
    float2 LightUV = float2((vPosition.x + 1.f) / 2.f, (vPosition.y - 1.f) / -2.f);

    vector vLightDepth = g_vLightDepthTexture.Sample(LinearSampler, LightUV);

	
    float LightDepth_W = vLightDepth.y * g_fCamFar;
    float LightDepth_Z = vLightDepth.x * LightDepth_W;
    float CamDepth = vPosition.z - 0.001f / g_fCamFar;

    if (CamDepth > vLightDepth.x)
    {
        Out.vColor.x = 0.2f;
    }
    else
        discard;


    return Out;


}



technique11 DefaultTechnique
{
    pass PBS
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_PBR();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_PBR();
    }

    pass Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

}