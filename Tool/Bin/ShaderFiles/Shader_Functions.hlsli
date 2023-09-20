
float2			g_vOffset;
float2          g_vTililing;

// ex) SplitUV(In.vTexUV, g_iWidthLength, g_iHeightLength, g_iCurIndex, Out.vTexUV);
void SplitUV(float2 In, int iWidthLength, int iHeightLength, int iCurIndex, out float2 Out)
{
	// ex)
	// g_iCurIndex = 6, 
	// g_iWidthLength = 4, g_iHeightLenght = 5

	// ���� ���
	int iCurWidth = iCurIndex % iWidthLength;		// 6 % 4 = 2
	int iCurHeight = int(iCurIndex / iWidthLength); // 6 / 4 = int(1)
	float fUnitWidth = 1.f / iWidthLength;			// 1.f / 4 = 0.25f
	float fUnitHeight = 1.f / iHeightLength;			// 1.f / 5 = 0.2f;
	float2 vStartUV;

	// ��������Ʈ�� ���� UV ��ǥ ���
	vStartUV.x = iCurWidth * fUnitWidth;	// 2 * 0.25f = 0.5f
	vStartUV.y = iCurHeight * fUnitHeight;	// 1 * 0.2f = 0.2f

	// �ش� ��������Ʈ �������� UV ���
	Out.x = In.x * fUnitWidth + vStartUV.x;	// x * 0.25f + 0.5f
	Out.y = In.y * fUnitHeight + vStartUV.y;	// y * 0.2f + 0.2f
}

///////////////// �޸� ������ 1D //////////////////

float g_fPersistence;
float g_fOctaves;

float Noise1(int x)
{
	int n = x * 57;
	n = (n << 13) ^ n;
	return (1.0 - (float)((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float SmoothNoise_1(float x)
{
	float corners = (Noise1(int(x - 1)) + Noise1(int(x + 1))) / 4.0;
	float center = Noise1(int(x)) / 2.0;
	return corners + center;
}

float InterpolateNoise_1(float x)
{
	int integer_X = int(x);
	float fractional_X = x - integer_X;

	float v1 = SmoothNoise_1(integer_X);
	float v2 = SmoothNoise_1(integer_X + 1);

	return lerp(v1, v2, fractional_X);
}

float PerlinNoise_1D(float x)
{
	float total = 0.0;
	float p = g_fPersistence; // Assuming persistence is a global variable or constant
	int n = g_fOctaves - 1;

	for (int i = 0; i <= n; i++)
	{
		float frequency = pow(2.0, i);
		float amplitude = pow(p, i);

		total += InterpolateNoise_1(x * frequency) * amplitude;
	}

	return total;
}
////////////////////////////////////////////////////
////////////////// �޸� ������ 2D //////////////////
float Noise2(int x, int y)
{
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return (1.0 - (float)((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float SmoothNoise_2(float x, float y)
{
	float corners = (Noise2(x - 1, y - 1) + Noise2(x + 1, y - 1) + Noise2(x - 1, y + 1) + Noise2(x + 1, y + 1)) / 16.0;
	float sides = (Noise2(x - 1, y) + Noise2(x + 1, y) + Noise2(x, y - 1) + Noise2(x, y + 1)) / 8.0;
	float center = Noise2(x, y) / 4.0;
	return corners + sides + center;
}

float InterpolateNoise_2(float x, float y)
{
	int integer_X = int(x);
	float fractional_X = x - integer_X;

	int integer_Y = int(y);
	float fractional_Y = y - integer_Y;

	float v1 = SmoothNoise_2(integer_X, integer_Y);
	float v2 = SmoothNoise_2(integer_X + 1, integer_Y);
	float v3 = SmoothNoise_2(integer_X, integer_Y + 1);
	float v4 = SmoothNoise_2(integer_X + 1, integer_Y + 1);

	float i1 = lerp(v1, v2, fractional_X);
	float i2 = lerp(v3, v4, fractional_X);

	return lerp(i1, i2, fractional_Y);
}

float PerlinNoise_2D(float x, float y)
{
	float total = 0.0;
	float p = g_fPersistence; // Assuming persistence is a global variable or constant
	int n = g_fOctaves - 1;

	for (int i = 0; i <= n; i++)
	{
		float frequency = pow(2.0, i);
		float amplitude = pow(p, i);

		total += InterpolateNoise_2(x * frequency, y * frequency) * amplitude;
	}

	return total;
}
////////////////////////////////////////////////////
///////////////// �޸������� 3D /////////////////////

float Noise3(int x, int y, int z)
{
	int n = x + y * 57 + z * 131;
	n = (n << 13) ^ n;
	return (1.0 - (float)((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float SmoothNoise_3(float x, float y, float z)
{
	float corners = (Noise3(x - 1, y - 1, z - 1) + Noise3(x + 1, y - 1, z - 1) +
		Noise3(x - 1, y + 1, z - 1) + Noise3(x + 1, y + 1, z - 1) +
		Noise3(x - 1, y - 1, z + 1) + Noise3(x + 1, y - 1, z + 1) +
		Noise3(x - 1, y + 1, z + 1) + Noise3(x + 1, y + 1, z + 1)) / 16.0;

	float sides = (Noise3(x - 1, y, z) + Noise3(x + 1, y, z) + Noise3(x, y - 1, z) +
		Noise3(x, y + 1, z) + Noise3(x, y, z - 1) + Noise3(x, y, z + 1)) / 8.0;

	float center = Noise3(x, y, z) / 4.0;
	return corners + sides + center;
}

float InterpolateNoise_3(float x, float y, float z)
{
	int integer_X = int(x);
	float fractional_X = x - integer_X;

	int integer_Y = int(y);
	float fractional_Y = y - integer_Y;

	int integer_Z = int(z);
	float fractional_Z = z - integer_Z;

	// Blending weights
	float v1 = SmoothNoise_3(integer_X, integer_Y, integer_Z);
	float v2 = SmoothNoise_3(integer_X + 1, integer_Y, integer_Z);
	float v3 = SmoothNoise_3(integer_X, integer_Y + 1, integer_Z);
	float v4 = SmoothNoise_3(integer_X + 1, integer_Y + 1, integer_Z);
	float v5 = SmoothNoise_3(integer_X, integer_Y, integer_Z + 1);
	float v6 = SmoothNoise_3(integer_X + 1, integer_Y, integer_Z + 1);
	float v7 = SmoothNoise_3(integer_X, integer_Y + 1, integer_Z + 1);
	float v8 = SmoothNoise_3(integer_X + 1, integer_Y + 1, integer_Z + 1);

	float i1 = lerp(v1, v2, fractional_X);
	float i2 = lerp(v3, v4, fractional_X);
	float i3 = lerp(v5, v6, fractional_X);
	float i4 = lerp(v7, v8, fractional_X);

	float j1 = lerp(i1, i2, fractional_Y);
	float j2 = lerp(i3, i4, fractional_Y);

	return lerp(j1, j2, fractional_Z);
}

float PerlinNoise_3D(float x, float y, float z)
{
	float total = 0.0;
	float p = g_fPersistence;
	int n = g_fOctaves - 1;

	for (int i = 0; i <= n; i++)
	{
		float frequency = pow(2.0, i);
		float amplitude = pow(p, i);

		total += InterpolateNoise_3(x * frequency, y * frequency, z * frequency) * amplitude;
	}

	return total;
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
