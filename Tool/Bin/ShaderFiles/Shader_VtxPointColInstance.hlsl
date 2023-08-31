#include "Shader_Tool_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vCamPosition;
vector			g_vColor = vector(1.f, 1.f, 1.f, 1.f);
texture2D		g_Texture;

struct VS_IN
{
	/* 그리기 위한 정점정보 */
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;

	/* 인스턴싱정보 (도형하나를 제어하기위한 행렬) */
	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
	float4		vColor : TEXCOORD5;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
	float4		vColor : COLOR0;
};

/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;
	
	vector			vPosition;
	matrix			vTransformMatrix = matrix(In.vRight, In.vUp, In.vLook, In.vTranslation);
	vPosition = mul(vector(In.vPosition, 1.f), vTransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = float2(In.vPSize.x * length(In.vRight)
		, In.vPSize.y * length(In.vUp));

	Out.vColor = In.vColor;

	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
	float4		vColor : COLOR0;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vColor : COLOR0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT		Out[4];

	vector		vLook = g_vCamPosition - In[0].vPosition;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(vector(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
	Out[0].vTexUV = float2(0.f, 0.f);
	Out[0].vColor = In[0].vColor;

	Out[1].vPosition = mul(vector(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 0.f);
	Out[1].vColor = In[0].vColor;

	Out[2].vPosition = mul(vector(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
	Out[2].vTexUV = float2(1.f, 1.f);
	Out[2].vColor = In[0].vColor;

	Out[3].vPosition = mul(vector(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 1.f);
	Out[3].vColor = In[0].vColor;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vColor : COLOR0;
};

struct PS_OUT
{
	float4	vColor : SV_TARGET0;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	float4		vAlpha = (float4)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	vAlpha = Out.vColor;

	Out.vColor *= (In.vColor * g_vColor);
	Out.vColor.a = vAlpha.r;
	if (Out.vColor.a < 0.2f)
		discard;

	return Out;
}

technique11		DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}