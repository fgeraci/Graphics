
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;							// is read from a constant buffer
};

struct VertexIn {
	// all these attributes MUST be described in the Input Layout struct
	float3 iPosL	: POSITION;							// All members as described in the Input Layout Element Desc
	float4 iColor	: COLOR;							// this are the defined SEMANTIC names
};

struct VertexOut {
	float4 oPosH	: SV_POSITION;						// We will return SV (homogeneous clip space) ONLY if there isn't a Geometry Shader
	float4 oColor	: COLOR;							// The most basic task can be to simply color the already given color
};

VertexOut VS(VertexIn vIn) {

	VertexOut vOut;

	// Transform vertex to homegeneous clip space
	vOut.oPosH = mul(float4(vIn.iPosL, 1.0f),			// constructs a point - note w = 1 - (iPosL.<x,y,z> , 1.0)
					gWorldViewProj);					// multiples by gWorldViewProj matrix

	// Pass vertex color
	vOut.oColor = vIn.iColor;

	return vOut;
}

/*
	IFF there is no Geometry Shader in the pipeline, then the output position from this Shader must be a SV_POSITION (Homogeneous clip space vertex)
	We do NOT do the perspective divide (to put the position in NDC) not in the Vertex Shader NOR in the Geometry Shader.
*/