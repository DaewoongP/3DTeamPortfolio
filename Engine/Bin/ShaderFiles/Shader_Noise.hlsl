static float3 Gradiant[12] =
{
    { 1, 1, 0 },
    { 1, -1, 0 },
    { -1, -1, 0 },
    { -1, 1, 0 },
    { 0, 1, 1 },
    { 0, -1, 1 },
    { 0, -1, -1 },
    { 0, 1, -1 },
    { 1, 0, 1 },
    { -1, 0, 1 },
    { 1, 0, -1 },
    { -1, 0, -1 }
};

float dotGridGradient2D(int ix, int iy, float x, float y)
{
    int index = frac(dot(float2(ix, iy), float2(413.23, 635.21))) * 12; //random index
    float3 grad = Gradiant[index];

    return dot(grad, float3(x, y, 0));
}

float dotGridGradient3D(int3 pos, float3 fract)
{
    int index = frac(dot(pos, float3(413.23, 635.21, 463.15))) * 12; //random index
    float3 grad = Gradiant[index];

    return dot(grad, fract);
}

float PerlinNoise2D(float2 xy, float frequency)		//pixel coord
{
    xy = xy * frequency;

    int x0 = floor(xy.x);
    int y0 = floor(xy.y);

    float dx = xy.x - x0;
    float dy = xy.y - y0;

    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float4 noise;
    noise.x = dotGridGradient2D(x0, y0, dx, dy);
    noise.y = dotGridGradient2D(x0, y1, dx, dy - 1);
    noise.z = dotGridGradient2D(x1, y0, dx - 1, dy);
    noise.w = dotGridGradient2D(x1, y1, dx - 1, dy - 1);

    dx = smoothstep(0, 1, dx);
    dy = smoothstep(0, 1, dy);
    float2 midNoise;
    midNoise.x = lerp(noise.x, noise.z, dx);
    midNoise.y = lerp(noise.y, noise.w, dx);

    return lerp(midNoise.x, midNoise.y, dy);
}

//xyz : (0, 0, 0) - (screenWidth, screenHeight, screenHeight);
float PerlinNoise3D(float3 xyz, float frequency)		//pixel coord
{
    xyz = xyz * frequency;

    int x0 = floor(xyz.x);
    int y0 = floor(xyz.y);
    int z0 = floor(xyz.z);

    float dx = xyz.x - x0;
    float dy = xyz.y - y0;
    float dz = xyz.z - z0;

    float4 noise0, noise1;

    int x1 = x0 + 1;
    int y1 = y0 + 1;
    int z1 = z0 + 1;

    noise0.x = dotGridGradient3D(int3(x0, y0, z0), float3(dx, dy, dz));
    noise0.y = dotGridGradient3D(int3(x0, y1, z0), float3(dx, dy - 1.0, dz));
    noise0.z = dotGridGradient3D(int3(x1, y0, z0), float3(dx - 1.0, dy, dz));
    noise0.w = dotGridGradient3D(int3(x1, y1, z0), float3(dx - 1.0, dy - 1.0, dz));

    noise1.x = dotGridGradient3D(int3(x0, y0, z1), float3(dx, dy, dz - 1.0));
    noise1.y = dotGridGradient3D(int3(x0, y1, z1), float3(dx, dy - 1.0, dz - 1.0));
    noise1.z = dotGridGradient3D(int3(x1, y0, z1), float3(dx - 1.0, dy, dz - 1.0));
    noise1.w = dotGridGradient3D(int3(x1, y1, z1), float3(dx - 1.0, dy - 1.0, dz - 1.0));
	
    dx = smoothstep(0, 1, dx);
    dy = smoothstep(0, 1, dy);
    dz = smoothstep(0, 1, dz);

    float4 midNoise4;
    midNoise4.x = lerp(noise0.x, noise1.x, dz);
    midNoise4.y = lerp(noise0.y, noise1.y, dz);
    midNoise4.z = lerp(noise0.z, noise1.z, dz);
    midNoise4.w = lerp(noise0.w, noise1.w, dz);

    float2 midNoise2;
    midNoise2.x = lerp(midNoise4.x, midNoise4.y, dy);
    midNoise2.y = lerp(midNoise4.z, midNoise4.w, dy);

    return lerp(midNoise2.x, midNoise2.y, dx);
}

//===============================using Perlin================================//
float FractalNoise2D(float2 xy, float frequency)
{
    float gray = PerlinNoise2D(xy, frequency) + 0.5 * PerlinNoise2D(xy, 2 * frequency) + 0.25 * PerlinNoise2D(xy, 4 * frequency) + 0.125 * PerlinNoise2D(xy, 8 * frequency);
    gray = (gray + 1) * 0.5;
    gray = clamp(gray, 0, 1);
    return gray;
}

float FractalNoise3D(float3 xyz, float frequency)
{
    float gray = PerlinNoise3D(xyz, frequency) + 0.5 * PerlinNoise3D(xyz, 2 * frequency) + 0.25 * PerlinNoise3D(xyz, 4 * frequency) + 0.125 * PerlinNoise3D(xyz, 8 * frequency);
	//gray = (gray + 1)*0.5;
    gray = clamp(gray, 0, 1);
    return gray;
}