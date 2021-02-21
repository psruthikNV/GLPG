// POSITION is the input semantic for the vtx Shader
// SV_POSITION is the output semantic

//GLPG::Matrix uses row major matrices while HLSL defaults
//to column major for matrices. Add a pragma here to force HLSL to
//load matrices in row major order.
#pragma pack_matrix( row_major )

cbuffer MatrixContents {
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 pos : POSITION;
    float2 texCoords : TEXCOORD0;
};

struct PixelInputType
{
    float4 pos : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.pos = mul(modelMatrix, input.pos);
    output.pos = mul(viewMatrix, output.pos);
    output.pos = mul(projectionMatrix, output.pos);
    output.texCoords = input.texCoords;
    
	return output;
}