// Since handling materials is not a common thing for GFX RT and CPU RT, we have
// the routines here, not in base_rt.
//
namespace rt {
struct Hit_Info;

[[nodiscard]] bool
scatter(Material const &material,
        Ray const      &in,
        Hit_Info const &hi,
        Vec3           &attenuation_color,
        Ray            &out);

[[nodiscard]] Vec3
emit(Material const &material);
} // namespace rt
