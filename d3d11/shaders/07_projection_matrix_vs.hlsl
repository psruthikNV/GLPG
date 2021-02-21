// POSITION is the input semantic for the vtx Shader
// SV_POSITION is the output semantic

//GLPG::Matrix uses row major matrices while HLSL defaults
//to column major for matrices. Add a pragma here to force HLSL to
//store matrices in row major order.
#pragma pack_matrix( row_major )

cbuffer MatrixContents {
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
    float4 ret = mul(mul(mul(projectionMatrix, viewMatrix), modelMatrix), pos);

	return ret;
}