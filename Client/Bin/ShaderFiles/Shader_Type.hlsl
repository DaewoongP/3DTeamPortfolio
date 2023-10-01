#include "Shader_EngineHeader.hlsli"
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

float4 g_Diffuse = float4(0.5f, 0.5f, 0.5f, 0.5f);

float g_Metallic = 0.5f;
float g_Roughness = 0.5f;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
float g_fSpotPower;

vector g_vLightDiffuse;
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);


vector g_vMtrlAmbient = vector(0.5f, 0.5f, 0.5f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

float BlurWeights[23] =
{
    0.0011, 0.0123, 0.0561, 0.1353, 0.278, 0.3001, 0.4868, 0.6666, 0.7261, 0.8712, 0.9231,
    0.9986, 0.9231, 0.8712, 0.7261, 0.6666, 0.4868, 0.3001, 0.278, 0.1353, 0.0561, 0.0123, 0.0011
};
float total = 11.4776f;

struct VS_IN_PBR
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;

    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT_PBR
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;

    float2 vTexUV : TEXCOORD0;

};
struct VS_OUT_SOFTSHADOW
{
    float4  vPosition: SV_POSITION;
    float3 vNormal : NORMAL;

    float2 vTexUV : TEXCOORD0;
    float4 Position : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
};

VS_OUT_PBR VS_MAIN_PBR(VS_IN_PBR In)
{
    VS_OUT_PBR Out = (VS_OUT_PBR) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition,1), matWVP);

    return Out;
}

VS_OUT_SOFTSHADOW VS_MAIN_SHADOW(VS_IN_PBR In)
{
    VS_OUT_SOFTSHADOW Out = (VS_OUT_SOFTSHADOW) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;
    Out.Position = Out.vPosition;
    Out.vNormal = normalize(mul(In.vNormal, (float3x3)g_WorldMatrix));
    vector WorldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.lightPos = g_vLightPos.xyz - WorldPosition.xyz;
    Out.lightPos = normalize(Out.lightPos);
    
    return Out;
}

struct PS_IN
{
   
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;

    float2 vTexUV : TEXCOORD0;
};

struct PS_IN_SHAODW
{
   
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;

    float2 vTexUV : TEXCOORD0;
    float4 Position : TEXCOORD1;
    float3 lightPos : TEXCOORD2; //¹æÇâÀÓ pos¾Æ´Ô
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

PS_OUT PS_MAIN_SHADOW(PS_IN_SHAODW In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_vLightAmbient;
    float LightIntensity = saturate(dot(In.vNormal, In.lightPos));
    if(LightIntensity>0.f)
    {
        Out.vColor += (g_Diffuse * LightIntensity);
        Out.vColor = saturate(Out.vColor);
    }
    
    vector TexColor = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
    Out.vColor = Out.vColor * TexColor;
    float2 newUV = float2(In.Position.x / In.vPosition.w / 2.f + 0.5f, -In.Position.y / In.Position.w / 2.f + 0.5f);
    vector Shadow = g_ShadowTexture.Sample(LinearSampler_Clamp, newUV).r;
    Out.vColor = Out.vColor * Shadow;
    return Out;
    
   
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

    pass Softshadow
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