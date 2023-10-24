// Declare the UAV for output.
RWTexture2D<float4> output : register(u0);

cbuffer ConstantBuffer : register(b0)
{
    float4 color;
}

[numthreads(16, 16, 1)]
void CSMain (uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID)
{
    uint2 id = uint2(DTid.x, DTid.y); // 2D index for 2D texture
    output[id] = color;
}
