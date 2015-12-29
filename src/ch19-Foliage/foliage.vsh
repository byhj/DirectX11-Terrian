
cbuffer MatrixBuffer: register(b0)
{
  matrix g_View;
  matrix g_Proj;
};

struct VS_IN 
{
  float4 Pos : POSITION;
  float2 Tex : TEXCOORD0;
  row_major  matrix  InstanceWorld : WORLD;
  float3 InstanceColor : TEXCOORD1;
};


struct VS_OUT
{
   float4 Pos   : SV_POSITION;
   float2 Tex   : TEXCOORD0;
   float3 Color : TEXCOORD1;
};

VS_OUT FoliageVS(VS_IN vs_in)
{
    VS_OUT  vs_out;

	vs_in.Pos.w = 1.0f;
	vs_out.Pos = mul(vs_in.Pos, vs_in.InstanceWorld);
	vs_out.Pos = mul(vs_out.Pos, g_View);
	vs_out.Pos = mul(vs_out.Pos, g_Proj);

	vs_out.Tex = vs_in.Tex;
	vs_out.Color = vs_in.InstanceColor;

	return vs_out;
}


