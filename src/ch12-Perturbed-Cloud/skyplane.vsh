
cbuffer MatrixBuffer : register(b0)
{
  matrix g_Model;
  matrix g_View;
  matrix g_Proj;
};

struct VS_IN
{
  float4 Pos :  POSITION;
  float2 Tex :  TEXCOORD0;
};

struct VS_OUT
{
  float4 Pos : SV_POSITION;
  float2 Tex : TEXCOORD0;
};

VS_OUT VS(VS_IN vs_in)
{

   VS_OUT vs_out;
   vs_in.Pos.w = 1.0f;
   vs_out.Pos = mul(vs_in.Pos, g_Model);
   vs_out.Pos = mul(vs_out.Pos, g_View);
   vs_out.Pos = mul(vs_out.Pos, g_Proj);

   vs_out.Tex = vs_in.Tex;

   return vs_out;
}
