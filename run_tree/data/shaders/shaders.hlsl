cbuffer constants : register(b0) {
  row_major float4x4 transform;
  row_major float4x4 projection;
}

/* vertex attributes go here to input to the vertex shader */
struct vs_in {
  float2 position_local : POS;
  uint   packed_color   : COL;
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out {
  float4 position_clip : SV_POSITION; // required output of VS
  float4 color         : COL;
};

float4 unpack_color(uint packedColor) {
  float4 color;
  color.r = (float)((packedColor >> 24) & 0xFF) / 255.0;
  color.g = (float)((packedColor >> 16) & 0xFF) / 255.0;
  color.b = (float)((packedColor >> 8)  & 0xFF) / 255.0;
  color.a = (float)( packedColor        & 0xFF) / 255.0;
  return color;
}


vs_out vs_main(vs_in input) {
  vs_out output = (vs_out)0; // zero the memory first
  
  output.position_clip = mul(float4(input.position_local, 0.0, 1.0), mul(transform, projection));
  output.color = unpack_color(input.packed_color);
  
  return output;
}

float4 ps_main(vs_out input) : SV_TARGET {
  return input.color;
}