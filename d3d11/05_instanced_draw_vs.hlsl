// POSITION is the input semantic for the vtx Shader
// SV_POSITION is the output semantic

struct VertexInput {
	float4 pos : POSITION;
	float3 instancePos : TEXCOORD;
};

float4 main( VertexInput vtxInput ) : SV_POSITION
{
	float4 ret = vtxInput.pos + float4(vtxInput.instancePos, 1.0);
	return ret;
}