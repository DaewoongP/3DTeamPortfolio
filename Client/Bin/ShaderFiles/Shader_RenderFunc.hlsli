
float3 rtt_and_odt_fit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
float3x3 aces_input_matrix =
{
    float3(0.59719f, 0.35458f, 0.04823f),
    float3(0.07600f, 0.90834f, 0.01566f),
    float3(0.02840f, 0.13383f, 0.83777f)
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
float3x3 aces_output_matrix =
{
    float3(1.60475f, -0.53108f, -0.07367f),
    float3(-0.10208f, 1.10813f, -0.00605f),
    float3(-0.00327f, -0.07276f, 1.07602f)
};

float3 ACESToneMapping(float3 colorIn)
{
    colorIn = mul(aces_input_matrix, colorIn);
    colorIn = rtt_and_odt_fit(colorIn);
    colorIn = mul(aces_output_matrix, colorIn);
    
    return saturate(colorIn);
}