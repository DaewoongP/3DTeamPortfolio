float PI = 3.141592f;

//�����þ� ǥ���� ���ݻ� BRDF���
float3 CalculateLambertianBRDF(float3 vAlbedo)
{
    return vAlbedo / PI;
}

float NormalDistributionGGXTR(float3 vNormal, float3 vHalfVector, float fRoughness)
{
    float NdotH = saturate(dot(vNormal, vHalfVector));
    float NdotH2 = NdotH * NdotH;
	
    float fNum = fRoughness * fRoughness;
    float fDenom = (NdotH2 * (fNum - 1.f) + 1.f);
    fDenom = PI * fDenom * fDenom;
	
    return fNum / fDenom;
}

float GeometrySchlickGGX(float fNormalDotPointToCamera, float fRoughness)
{
    float r = (fRoughness + 1.f);
    float k = (r * r) / 8.f;

    float fNum = fNormalDotPointToCamera;
    float fDenom = fNormalDotPointToCamera * (1.0 - k) + k;
	
    return fNum / fDenom;
}

float GeometrySmith(in float3 normal, in float3 pointToCamera, in float3 pointToLight, in float roughness)
{
    float normalDotPointToCamera = max(dot(normal, pointToCamera), 0.0);
    float normalDotPointToLight = max(dot(normal, pointToLight), 0.0);
    float ggx2 = GeometrySchlickGGX(normalDotPointToCamera, roughness);
    float ggx1 = GeometrySchlickGGX(normalDotPointToLight, roughness);
	
    return ggx1 * ggx2;
}

//������ ��(ǥ�鸶�� �ٸ� ������ ���(F0),Normal���Ϳ��� ���� cosTheta�� �־������� �ݻ�Ǵ� '����'
float3 FresnelSchlick(float cosTheta, float3 F0)
{ 
    return F0 + (1.f - F0) * pow(clamp(1.f - cosTheta, 0.f, 1.f), 5.f);
}

float3 CalculateCookTorranceBRDF(
        in float3 normal,
        in float3 pointToCamera,
        in float3 halfVector,
        in float3 pointToLight,
        in float roughness,
        in float3 F
    )
{
    float NDF = NormalDistributionGGXTR(normal, halfVector, roughness); //�̼��� ������ NDF���
    float G = GeometrySmith(normal, pointToCamera, pointToLight, roughness); //�̼��� �׸��� ���
                
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, pointToCamera), 0.0) * max(dot(normal, pointToLight), 0.0) + 0.0001f;
    float3 specular = numerator / denominator;
                
    return specular;
}