// The Pixel Shader's input must match the Vertex Shader's output.
// we pass the VertexOut struct - this will return the computed color for this pixel

struct VertexOut {
	float4 oPosH  : SV_POSITION;
	float4 oColor : COLOR;
};

float4 PS(VertexOut pIn) : SV_Target	// must match render target's format
										// in our case: g_BackbufferFormat
{
	return pIn.oColor;					// we simply return the same color from the vertex
}