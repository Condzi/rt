namespace rt {
//
//  Compute shader related types.
//

struct alignas(16) RT_Constants {
  // Quality
  //
  s32 num_samples;
  s32 num_reflections;

  // Buffers sizes
  //
  s32 num_spheres;
  s32 num_quads;
  s32 num_materials;

  // Camera properties
  //
  Vec3 origin;
  Vec3 horizontal;
  Vec3 vertical;
  Vec3 lower_left_corner;
  Vec3 u, v, w;
  f32  lens_radius;
};

struct RT_Sphere {
  Vec3 center;
  f32  radius;

  Material_ID mat_id;
};

struct RT_Quad {
  Vec3 normal;
  f32  D;
  Vec3 w;

  Material_ID mat_id;
};

struct RT_Material {
  u32  type;
  Vec3 albedo;           // Common among multiple material types
  f32  fuzz;             // Only used in Metal
  f32  refraction_index; // Only used in Dielectric
};

struct RT_Pipeline {
  ::IComputeShader       *cs  = NULL;
  ::IUnorderedAccessView *uav = NULL;
  ::ITexture2D           *tex = NULL;

  ::IBuffer *gpu_consts    = NULL;
  ::IBuffer *gpu_spheres   = NULL;
  ::IBuffer *gpu_quads     = NULL;
  ::IBuffer *gpu_materials = NULL;

  ::IShaderResourceView *gpu_spheres_view;
  ::IShaderResourceView *gpu_quads_view;
  ::IShaderResourceView *gpu_materials_view;

  RT_Constants constants;

  RT_Sphere   *spheres   = NULL;
  RT_Quad     *quads     = NULL;
  RT_Material *materials = NULL;
};

//
//  Helper functions
//

void
gfx_rt_set_up_shader_world(GFX_RT_Input const &in);

void
gfx_rt_load_and_compile_shader_or_panic();

void
gfx_rt_create_output_texture_or_panic();

void
gfx_rt_create_uav_or_panic();

void
gfx_rt_create_rt_world_constants_or_panic();

void
gfx_rt_create_rt_world_srvs_or_panic();

//
//  Public functions
//

void
gfx_rt_init_or_panic(GFX_RT_Input const &in) {
  gD3d.rt_pipeline = perm<RT_Pipeline>();

  gfx_rt_set_up_shader_world(in);
  gfx_rt_load_and_compile_shader_or_panic();
  gfx_rt_create_output_texture_or_panic();
  gfx_rt_create_uav_or_panic();
  gfx_rt_create_rt_world_constants_or_panic();
  gfx_rt_create_rt_world_srvs_or_panic();
}

void
gfx_rt_start() {
  // Bind buffers
  //
  gD3d.device_context->CSSetConstantBuffers(0, 1, &(gD3d.rt_pipeline->gpu_consts));
  gD3d.device_context->CSSetShaderResources(
      0, 1, &(gD3d.rt_pipeline->gpu_spheres_view));
  /*
gD3d.device_context->CSSetShaderResources(
  1, 1, &(gD3d.rt_pipeline->gpu_quads_view));
  */
  gD3d.device_context->CSSetShaderResources(
      2, 1, &(gD3d.rt_pipeline->gpu_materials_view));

  gD3d.device_context->CSSetUnorderedAccessViews(
      0, 1, &(gD3d.rt_pipeline->uav), NULL);
  gD3d.device_context->CSSetShader(gD3d.rt_pipeline->cs, NULL, 0);
  // 512 -- Image size, 16 -- sectors? I guess 16 is not relevant for our use case,
  // should be just image_size instead?
  //
  gD3d.device_context->Dispatch(32, 32, 1);

  // Unbind buffers
  //
  ::ID3D11Buffer        *null_buffer = NULL;
  ::IShaderResourceView *null_view   = NULL;
  gD3d.device_context->CSSetConstantBuffers(0, 1, &null_buffer);
  gD3d.device_context->CSSetShaderResources(0, 1, &null_view);
  gD3d.device_context->CSSetShaderResources(1, 1, &null_view);
  gD3d.device_context->CSSetShaderResources(2, 1, &null_view);
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
gfx_rt_set_up_shader_world(GFX_RT_Input const &in) {
  RT_Constants &rcs = gD3d.rt_pipeline->constants;

  rcs = {.num_samples     = 100,
         .num_reflections = 5,

         .num_spheres   = in.w.num_spheres,
         .num_quads     = in.w.num_quads,
         .num_materials = (s32)in.w.materials.size(),

         .origin            = in.c.origin,
         .horizontal        = in.c.horizontal,
         .vertical          = in.c.vertical,
         .lower_left_corner = in.c.lower_left_corner,
         .u                 = in.c.u,
         .v                 = in.c.v,
         .w                 = in.c.w,
         .lens_radius       = in.c.lens_radius};

  gD3d.rt_pipeline->spheres = perm<RT_Sphere>(rcs.num_spheres);
  for (s32 i = 0; i < rcs.num_spheres; i++) {
    gD3d.rt_pipeline->spheres[i] = {.center = in.w.spheres[i].center,
                                    .radius = in.w.spheres[i].radius,
                                    .mat_id = in.w.spheres[i].mat_id};
  }

  gD3d.rt_pipeline->quads = perm<RT_Quad>(rcs.num_quads);
  for (s32 i = 0; i < rcs.num_quads; i++) {
    gD3d.rt_pipeline->quads[i] = {.normal = in.w.quads[i].normal,
                                  .D      = in.w.quads[i].D,
                                  .w      = in.w.quads[i].w,
                                  .mat_id = in.w.quads[i].mat_id};
  }

  gD3d.rt_pipeline->materials = perm<RT_Material>(rcs.num_materials);
  for (s32 i = 0; i < rcs.num_materials; i++) {
    Material const &m      = in.w.materials[i];
    RT_Material     result = {.type = m.type};
    // We need different values depending on the material type; this is
    // because Compute Shaders do not support unions.
    //
    switch (m.type) {
      case MaterialType_Lambertian: {
        result.albedo = m.lambertian.albedo;
      } break;

      case MaterialType_Metal: {
        result.albedo = m.metal.albedo;
        result.fuzz   = m.metal.fuzz;
      } break;

      case MaterialType_Dielectric: {
        result.refraction_index = m.dielectric.refraction_index;
      } break;

      case MaterialType_Diffuse_Light: {
        result.albedo = m.diffuse_light.albedo;
      } break;

      default: {
        assert(false);
      }
    }

    gD3d.rt_pipeline->materials[i] = result;
  }
}

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

void
gfx_rt_create_rt_world_constants_or_panic() {
  ::HRESULT hr;

  ::D3D11_SUBRESOURCE_DATA const cs_data {.pSysMem = &(gD3d.rt_pipeline->constants)};
  ::D3D11_SUBRESOURCE_DATA const sp_data {.pSysMem = gD3d.rt_pipeline->spheres};
  ::D3D11_SUBRESOURCE_DATA const qs_data {.pSysMem = gD3d.rt_pipeline->quads};
  ::D3D11_SUBRESOURCE_DATA const ms_data {.pSysMem = gD3d.rt_pipeline->materials};

  // Constants
  //
  ::D3D11_BUFFER_DESC const consts_desc = {.ByteWidth = sizeof(RT_Constants),
                                           .Usage     = D3D11_USAGE_IMMUTABLE,
                                           .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
                                           .CPUAccessFlags = 0};

  hr = gD3d.device->CreateBuffer(
      &consts_desc, &cs_data, &gD3d.rt_pipeline->gpu_consts);
  d3d_check_hresult_(hr);

  // Spheres
  //
  ::D3D11_BUFFER_DESC const spheres_desc = {
      .ByteWidth      = gD3d.rt_pipeline->constants.num_spheres * sizeof(RT_Sphere),
      .Usage          = D3D11_USAGE_IMMUTABLE,
      .BindFlags      = D3D11_BIND_SHADER_RESOURCE,
      .CPUAccessFlags = 0,
      .MiscFlags      = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
      .StructureByteStride = sizeof(RT_Sphere)};

  hr = gD3d.device->CreateBuffer(
      &spheres_desc, &sp_data, &gD3d.rt_pipeline->gpu_spheres);
  d3d_check_hresult_(hr);

  // Quads
  //
  /*
  ::D3D11_BUFFER_DESC const quads_desc = {
      .ByteWidth           = gD3d.rt_pipeline->constants.num_quads * sizeof(RT_Quad),
      .Usage               = D3D11_USAGE_IMMUTABLE,
      .BindFlags           = D3D11_BIND_SHADER_RESOURCE,
      .CPUAccessFlags      = 0,
      .MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
      .StructureByteStride = sizeof(RT_Quad)};

  hr = gD3d.device->CreateBuffer(&quads_desc, &qs_data, &gD3d.rt_pipeline->gpu_quads);
  d3d_check_hresult_(hr);
  */

  // Materials
  //
  ::D3D11_BUFFER_DESC const materials_desc = {
      .ByteWidth = gD3d.rt_pipeline->constants.num_materials * sizeof(RT_Material),
      .Usage     = D3D11_USAGE_IMMUTABLE,
      .BindFlags = D3D11_BIND_SHADER_RESOURCE,
      .CPUAccessFlags      = 0,
      .MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
      .StructureByteStride = sizeof(RT_Material)};

  hr = gD3d.device->CreateBuffer(
      &materials_desc, &ms_data, &gD3d.rt_pipeline->gpu_materials);
  d3d_check_hresult_(hr);
}

void
gfx_rt_create_rt_world_srvs_or_panic() {
  ::D3D11_SHADER_RESOURCE_VIEW_DESC const sp_desc = {
      .Format        = DXGI_FORMAT_UNKNOWN,
      .ViewDimension = D3D11_SRV_DIMENSION_BUFFER,
      .Buffer        = {.FirstElement = 0,
                        .NumElements  = (::UINT)gD3d.rt_pipeline->constants.num_spheres}};

  ::D3D11_SHADER_RESOURCE_VIEW_DESC const qs_desc = {
      .Format        = DXGI_FORMAT_UNKNOWN,
      .ViewDimension = D3D11_SRV_DIMENSION_BUFFER,
      .Buffer        = {.FirstElement = 0,
                        .NumElements  = (::UINT)gD3d.rt_pipeline->constants.num_quads}};

  ::D3D11_SHADER_RESOURCE_VIEW_DESC const ms_desc = {
      .Format        = DXGI_FORMAT_UNKNOWN,
      .ViewDimension = D3D11_SRV_DIMENSION_BUFFER,
      .Buffer        = {.FirstElement = 0,
                        .NumElements  = (::UINT)gD3d.rt_pipeline->constants.num_materials}};

  ::HRESULT hr;

  hr = gD3d.device->CreateShaderResourceView(
      gD3d.rt_pipeline->gpu_spheres, &sp_desc, &(gD3d.rt_pipeline->gpu_spheres_view));
  d3d_check_hresult_(hr);

  /*
    hr = gD3d.device->CreateShaderResourceView(
        gD3d.rt_pipeline->gpu_quads, &qs_desc, &(gD3d.rt_pipeline->gpu_quads_view));
    d3d_check_hresult_(hr);
  */

  hr = gD3d.device->CreateShaderResourceView(gD3d.rt_pipeline->gpu_materials,
                                             &ms_desc,
                                             &(gD3d.rt_pipeline->gpu_materials_view));
  d3d_check_hresult_(hr);
}
} // namespace rt
