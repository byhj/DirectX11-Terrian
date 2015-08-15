cbuffer MatrixBuffer : register(b0)
{
	float4x4 model;
	float4x4 view;
	float4x4 proj;
};


struct VS_IN
{
  float3 Pos: POSITION;
};

struct VS_OUT
{
  float4 Pos: SV_POSITION;
  float3 Tex: TEXCOORD;
};

VS_OUT VS(VS_IN vs_in)
{
    VS_OUT vs_out;

  float4x4 mvp;
   mvp = mul(model, view);
   mvp = mul(mvp, proj);

	//w=1.0f, make sure our skymap is always the furthest object in our scene
   vs_out.Pos = mul(float4(vs_in.Pos, 1.0f), mvp).xyww;

   vs_out.Tex = vs_in.Pos;

   return vs_out;
}