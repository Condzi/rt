namespace rt {
struct XXC_Pipeline {
    D3D11_VIEWPORT         viewport;
    ID3D11RasterizerState *rasterizer_state = NULL;
    ID3D11VertexShader    *vs     = NULL;
    ID3D11PixelShader     *ps     = NULL;
    ID3D11InputLayout     *layout = NULL;
    ID3D11Buffer          *vbo    = NULL;
    ID3D11Buffer          *consts = NULL;
    
    Constants  constants;
    
    Vertex_XXC v[IM_TRIS_COUNT*3];
    s64        v_mark;
};

void 
gfx_im_load_compile_create_shaders_or_panic();

void
gfx_im_create_vbo_or_panic();

void 
gfx_im_create_constants_buffer_or_panic();

void
gfx_im_create_rasterizer_or_panic();


void
gfx_im_init_or_panic() {
  gD3d.xxc_pipeline = (XXC_Pipeline*)alloc_perm(sizeof(XXC_Pipeline));

  gfx_im_load_compile_create_shaders_or_panic();
  gfx_im_create_vbo_or_panic();
  gfx_im_create_constants_buffer_or_panic();
  gfx_im_create_rasterizer_or_panic();

  gD3d.xxc_pipeline->viewport = {
    0.0f,
    0.0f,
    window_get_size().width,
    window_get_size().height,
    0.0f,
    1.0f 
  };
}

void 
gfx_im_load_compile_create_shaders_or_panic() {
  // Step 1. Load (and compile) shaders

  ::ID3DBlob *vs_blob = NULL, *ps_blob = NULL;
  ::ID3DBlob *err_blob = NULL;
  ::HRESULT hr;
  
  String shader_path = pathf("%S/shaders.hlsl");
  Buffer raw_shader = os_read_entire_file_or_panic(as_cstr(shader_path));

  ::UINT constexpr static SHADER_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

  hr = ::D3DCompile(
          raw_shader.bytes, 
          raw_shader.count,
          NULL,
          NULL,
          D3D_COMPILE_STANDARD_FILE_INCLUDE,
          "vs_main", 
          "vs_5_0", 
          SHADER_FLAGS, 
          0,
          &vs_blob, 
          &err_blob
          );

  if (FAILED(hr) && err_blob) {
    errf("Failed to compile the vertex shader: %s", 
          (char const*)err_blob->GetBufferPointer());
  }
  d3d_check_hresult_(hr);

  hr = ::D3DCompile(
          raw_shader.bytes, 
          raw_shader.count,
          NULL,
          NULL,
          D3D_COMPILE_STANDARD_FILE_INCLUDE,
          "ps_main", 
          "ps_5_0", 
          SHADER_FLAGS, 
          0,
          &ps_blob, 
          &err_blob
          );

  if (FAILED(hr) && err_blob) {
    errf("Failed to compile the pixel shader: %s", 
          (char const*)err_blob->GetBufferPointer());
  }
  d3d_check_hresult_(hr);


  // Step 2. Create shader objects
  hr = gD3d.device->CreateVertexShader(
                      vs_blob->GetBufferPointer(),
                      vs_blob->GetBufferSize(), 
                      NULL,
                      &gD3d.xxc_pipeline->vs
                      );
  d3d_check_hresult_(hr);

  hr = gD3d.device->CreatePixelShader(
                      ps_blob->GetBufferPointer(),
                      ps_blob->GetBufferSize(), 
                      NULL,
                      &gD3d.xxc_pipeline->ps
                      );
  d3d_check_hresult_(hr);

  // Step 3. Describe the input of the Vertex Shader 

  ::D3D11_INPUT_ELEMENT_DESC input_layout[] = {
    {
      .SemanticName = "POS",
      .Format       = DXGI_FORMAT_R32G32_FLOAT,
      .InputSlot    = D3D11_INPUT_PER_VERTEX_DATA
    },

    {
      .SemanticName      = "COL",
      .Format            = DXGI_FORMAT_R32_UINT,
      .AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
      .InputSlotClass    = D3D11_INPUT_PER_VERTEX_DATA
    }
  };

  hr = gD3d.device->CreateInputLayout(
                      input_layout, 
                      sizeof(input_layout)/sizeof(*input_layout), 
                      vs_blob->GetBufferPointer(),
                      vs_blob->GetBufferSize(),
                      &gD3d.xxc_pipeline->layout
                      );
  d3d_check_hresult_(hr);

  d3d_safe_release_(err_blob);
  d3d_safe_release_(vs_blob);
  d3d_safe_release_(ps_blob);
}

void
gfx_im_vertex(Vec2 position, u32 color) {
  XXC_Pipeline &p = *gD3d.xxc_pipeline;

  p.v[p.v_mark] = Vertex_XXC{position, color};
  p.v_mark++;

  if (p.v_mark == sizeof(p.v)/sizeof(*p.v)) {
    gfx_im_flush();
  }
}

void
gfx_im_rect(Vec2 position, Vec2 size, u32 color) {
  Vec2 const p[6] = {
    {position.x         , position.y},         /* top-left */
    {position.x + size.x, position.y},         /* top-right */
    {position.x + size.x, position.y - size.y},/* btm-right */
    {position.x + size.x, position.y - size.y},/* btm-right */
    {position.x         , position.y - size.y},/* btm-left */
    {position.x         , position.y}          /* top-left */
  };

  for (int i = 0; i < 6; i++) {
    gfx_im_vertex(p[i], color);
  }
}

void
gfx_im_flush() {
  ::HRESULT hr;
  ::ID3D11DeviceContext &device_ctx = *gD3d.device_context;
  XXC_Pipeline          &pipeline   = *gD3d.xxc_pipeline;
  ::UINT const vertex_stride = sizeof(Vertex_XXC);
  ::UINT const vertex_offset = 0;
  ::UINT const vertex_count  = (::UINT)pipeline.v_mark;
  
  pipeline.v_mark = 0;

  // Copy new vertices to VRAM
  {
    ::D3D11_MAPPED_SUBRESOURCE vbo_data = {0};
    hr = device_ctx.Map(pipeline.vbo, 0, D3D11_MAP_WRITE_DISCARD, 0, &vbo_data);
    d3d_check_hresult_(hr);

    mem_copy_(vbo_data.pData, pipeline.v, sizeof(Vertex_XXC)*vertex_count);
    device_ctx.Unmap(pipeline.vbo, 0);
  }

  // Set the constants
  {
    ::D3D11_MAPPED_SUBRESOURCE consts_data = {0};
    hr = device_ctx.Map(pipeline.consts, 0, D3D11_MAP_WRITE_DISCARD, 0, &consts_data);
    d3d_check_hresult_(hr);

    Constants constants_transposed;
    constants_transposed.camera     = transpose(pipeline.constants.camera);
    constants_transposed.projection = transpose(pipeline.constants.projection);

    mem_copy_(consts_data.pData, &constants_transposed, sizeof(constants_transposed));

    device_ctx.Unmap(pipeline.consts, 0);
  }

  // Set the input assembler - what vertex buffer to draw, its layout, topology 

  device_ctx.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  device_ctx.IASetInputLayout(pipeline.layout);
  device_ctx.IASetVertexBuffers(0, 1, &pipeline.vbo, &vertex_stride, &vertex_offset);

  // Set the pipeline
  device_ctx.VSSetShader(pipeline.vs, NULL, 0);
  device_ctx.VSSetConstantBuffers(0, 1, &pipeline.consts);

  device_ctx.RSSetViewports( 1, &pipeline.viewport );
  device_ctx.RSSetState(pipeline.rasterizer_state);

  device_ctx.PSSetShader(pipeline.ps, NULL, 0);

  // Draw
  device_ctx.Draw(vertex_count, 0);
}

void
gfx_im_create_vbo_or_panic() {
  ::D3D11_BUFFER_DESC const vbo_desc = {
    .ByteWidth      = sizeof(XXC_Pipeline::v),
    .Usage          = D3D11_USAGE_DYNAMIC,
    .BindFlags      = D3D11_BIND_VERTEX_BUFFER,
    .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
  };

  ::HRESULT hr = gD3d.device->CreateBuffer(
                                &vbo_desc, 
                                NULL, 
                                &gD3d.xxc_pipeline->vbo
                                );
  d3d_check_hresult_(hr);
}

void 
gfx_im_create_constants_buffer_or_panic() {
  ::D3D11_BUFFER_DESC const consts_desc = {
    .ByteWidth      = sizeof(Constants),
    .Usage          = D3D11_USAGE_DYNAMIC,
    .BindFlags      = D3D11_BIND_CONSTANT_BUFFER,
    .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
  };

  ::HRESULT hr = gD3d.device->CreateBuffer(
                                &consts_desc, 
                                NULL, 
                                &gD3d.xxc_pipeline->consts
                                );
  d3d_check_hresult_(hr);  

  // Set constants to a valid, known state 
  Vec2 const win_size = window_get_size();
  gD3d.xxc_pipeline->constants.projection = ortho_proj(win_size.width, win_size.height);
  gD3d.xxc_pipeline->constants.camera     = identity();
}

void
gfx_im_create_rasterizer_or_panic() {
  ::D3D11_RASTERIZER_DESC rasterizer_desc = {
    .FillMode = D3D11_FILL_SOLID,
    .CullMode = D3D11_CULL_BACK
  };
  
  ::HRESULT hr = gD3d.device->CreateRasterizerState(
                                &rasterizer_desc, 
                                &gD3d.xxc_pipeline->rasterizer_state
                                );
  d3d_check_hresult_(hr);
}
} // namespace rt