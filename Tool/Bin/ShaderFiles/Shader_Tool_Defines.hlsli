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

RasterizerState RS_WireFrame
{
	FillMode = WireFrame;
	CullMode = None;
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
