
float2			g_vOffset = float2(0.f, 0.f);;
float2          g_vTililing = float2(1.f, 1.f);

// ex) SplitUV(In.vTexUV, g_iWidthLength, g_iHeightLength, g_iCurIndex, Out.vTexUV);
void SplitUV(float2 In, uint iWidthLength, uint iHeightLength, uint iCurIndex, out float2 Out)
{
	// ex)
	// g_iCurIndex = 6, 
	// g_iWidthLength = 4, g_iHeightLenght = 5

	// 비율 계산
	uint iCurWidth = iCurIndex % iWidthLength;		// 6 % 4 = 2
	uint iCurHeight = uint(iCurIndex / iWidthLength); // 6 / 4 = int(1)
	float fUnitWidth = 1.f / iWidthLength;			// 1.f / 4 = 0.25f
	float fUnitHeight = 1.f / iHeightLength;			// 1.f / 5 = 0.2f;
	float2 vStartUV;

	// 스프라이트의 시작 UV 좌표 계산
	vStartUV.x = iCurWidth * fUnitWidth;	// 2 * 0.25f = 0.5f
	vStartUV.y = iCurHeight * fUnitHeight;	// 1 * 0.2f = 0.2f

	// 해당 스프라이트 내에서의 UV 계산
	Out.x = In.x * fUnitWidth + vStartUV.x;	// x * 0.25f + 0.5f
	Out.y = In.y * fUnitHeight + vStartUV.y;	// y * 0.2f + 0.2f
}

void Remap_float4(float4 In, float2 InMinMax, float2 OutMinMax, out float4 Out)
{
	Out = OutMinMax.x + (In - InMinMax.x) * (OutMinMax.y - OutMinMax.x) / (InMinMax.y - InMinMax.x);
}

void Remap_float(float In, float2 InMinMax, float2 OutMinMax, out float Out)
{
	Out = OutMinMax.x + (In - InMinMax.x) * (OutMinMax.y - OutMinMax.x) / (InMinMax.y - InMinMax.x);
}

////////////////////////////////////////////////////
void TilingAndOffset_float(float2 UV, float2 Tiling, float2 Offset, out float2 Out)
{
    Out = UV * Tiling + Offset;
}

void FresnelEffect_float(float3 Normal, float3 ViewDir, float Power, out float Out)
{
    Out = pow((1.0 - saturate(dot(normalize(Normal), normalize(ViewDir)))), Power);
}
