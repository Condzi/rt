namespace rt {
struct alignas(16) RT_Constants {
  Vec4 color;
};

struct RT_Pipeline {
  ::IComputeShader       *cs  = NULL;
  ::IUnorderedAccessView *uav = NULL;
  ::ITexture2D           *tex = NULL;
  RT_Constants            constants;
};

//
//  Helper functions
//

void
gfx_rt_load_and_compile_shader_or_panic();

void
gfx_rt_create_output_texture_or_panic();

void
gfx_rt_create_uav_or_panic();

//
//  Public functions
//

void
gfx_rt_init_or_panic(GFX_RT_Input const &in) {
  (void)in;

  gD3d.rt_pipeline = perm<RT_Pipeline>();

  gfx_rt_load_and_compile_shader_or_panic();
  gfx_rt_create_output_texture_or_panic();
  gfx_rt_create_uav_or_panic();
}

void
gfx_rt_start() {
  gD3d.device_context->CSSetUnorderedAccessViews(
      0, 1, &(gD3d.rt_pipeline->uav), NULL);
  gD3d.device_context->CSSetShader(gD3d.rt_pipeline->cs, NULL, 0);
  gD3d.device_context->Dispatch(512 / 16, 512 / 16, 1);
}

[[nodiscard]] void *
gfx_rt_output_as_imgui_texture() {
  D3D11_SHADER_RESOURCE_VIEW_DESC desc;
  ZeroMemory(&desc, sizeof(desc));
  desc.Format                     = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.ViewDimension              = D3D11_SRV_DIMENSION_TEXTURE2D;
  desc.Texture2D.MostDetailedMip  = 0;
  desc.Texture2D.MipLevels        = 1; // Only one MIP level

  ::IShaderResourceView *texture_srv = NULL;
  ::HRESULT              hr;
  hr = gD3d.device->CreateShaderResourceView(
      gD3d.rt_pipeline->tex, &desc, &texture_srv);

  d3d_check_hresult_(hr);

  return texture_srv;
}

//
//  Helper functions
//

void
gfx_rt_load_and_compile_shader_or_panic() {
  // Step 1. Load (and compile) the shader

  ::IBlob  *cs_blob  = NULL;
  ::IBlob  *err_blob = NULL;
  ::HRESULT hr;

  String shader_path = pathf("%S/rt.hlsl");
  Buffer raw_shader  = os_read_entire_file_or_panic(as_cstr(shader_path));

  ::UINT constexpr static SHADER_FLAGS =
      D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

  hr = ::D3DCompile(raw_shader.bytes,
                    raw_shader.count,
                    NULL,
                    NULL,
                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    "CSMain",
                    "cs_5_0",
                    SHADER_FLAGS,
                    0,
                    &cs_blob,
                    &err_blob);

  if (FAILED(hr) && err_blob) {
    errf("Failed to compile the compute shader shader: %s",
         (char const *)err_blob->GetBufferPointer());
  }
  d3d_check_hresult_(hr);

  // Step 2. Create shader object

  hr = gD3d.device->CreateComputeShader(cs_blob->GetBufferPointer(),
                                        cs_blob->GetBufferSize(),
                                        NULL,
                                        &gD3d.rt_pipeline->cs);
  d3d_check_hresult_(hr);
  d3d_safe_release_(err_blob);
  d3d_safe_release_(cs_blob);
}

void
gfx_rt_create_output_texture_or_panic() {
  ::D3D11_TEXTURE2D_DESC desc = {};
  // @ToDo: use actual image dimensions!
  //
  desc.Width              = 512;
  desc.Height             = 512;
  desc.MipLevels          = 1;
  desc.ArraySize          = 1;
  desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.Usage              = D3D11_USAGE_DEFAULT;
  desc.SampleDesc.Count   = 1;
  desc.SampleDesc.Quality = 0;
  desc.BindFlags          = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

  ::HRESULT hr;
  hr = gD3d.device->CreateTexture2D(&desc, NULL, &(gD3d.rt_pipeline->tex));
  d3d_check_hresult_(hr);
}

void
gfx_rt_create_uav_or_panic() {
  D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
  desc.ViewDimension                    = D3D11_UAV_DIMENSION_TEXTURE2D;
  desc.Format                           = DXGI_FORMAT_R8G8B8A8_UNORM;

  ::HRESULT hr;
  hr = gD3d.device->CreateUnorderedAccessView(
      gD3d.rt_pipeline->tex, &desc, &(gD3d.rt_pipeline->uav));
  d3d_check_hresult_(hr);
}
} // namespace rt
