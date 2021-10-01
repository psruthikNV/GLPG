// POSITION is the input semantic for the vtx Shader
// SV_POSITION is the output semantic

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}