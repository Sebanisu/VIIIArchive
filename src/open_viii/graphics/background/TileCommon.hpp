//
// Created by pcvii on 6/7/2021.
//

#ifndef OPENVIII_CPP_WIP_TILECOMMON_HPP
#define OPENVIII_CPP_WIP_TILECOMMON_HPP
#include "BlendModeT.hpp"
#include "open_viii/graphics/Rectangle.hpp"
#include <compare>
#include <cstring>
namespace open_viii::graphics::background {
enum class TileCommonConstants : std::uint16_t {

  height             = 16U,
  width              = height,
  texture_page_width = 128U,
  area               = height * width,
};
template<typename tileT> struct TileCommon : tileT
{
private:
  using tileT::m_animation_id;
  using tileT::m_animation_state;
  using tileT::m_blend_mode;
  using tileT::m_layer_id;
  using tileT::m_palette_id;
  using tileT::m_source_xy;
  using tileT::m_tex_id_buffer;
  using tileT::m_xy;
  using tileT::m_z;
  using source_type = typename decltype(m_source_xy)::value_type;
  using output_type = typename decltype(m_xy)::value_type;
  using this_type   = TileCommon<tileT>;
  using constants_type =
    typename std::underlying_type_t<decltype(TileCommonConstants::height)>;
  static this_type
    copy_tile(const std::vector<char> &buffer)
  {
    auto tile = this_type{};
    std::memcpy(
      &tile, std::data(buffer), std::min(sizeof(tileT), std::size(buffer)));
    return tile;
  }

public:
  TileCommon() = default;
  explicit TileCommon(const std::vector<char> &buffer)
    : TileCommon(copy_tile(buffer))
  {}
  using tileT::EXPECTED_SIZE;
  using tileT::FORCE_TYPE_VALUES;
  auto operator<=>(const this_type &) const = default;
  template<typename return_type = constants_type>
  [[nodiscard]] static constexpr return_type
    height() noexcept
  {
    return static_cast<return_type>(TileCommonConstants::height);
  }
  template<typename return_type = constants_type>
  [[nodiscard]] static constexpr return_type
    width() noexcept
  {
    return static_cast<return_type>(TileCommonConstants::width);
  }
  template<typename return_type = constants_type>
  [[nodiscard]] static constexpr return_type
    texture_page_width() noexcept
  {
    return static_cast<return_type>(TileCommonConstants::texture_page_width);
  }
  template<typename return_type = constants_type>
  [[nodiscard]] static constexpr return_type
    area() noexcept
  {
    return static_cast<return_type>(TileCommonConstants::area);
  }
  [[nodiscard]] constexpr this_type
    with_source_xy(decltype(m_source_xy) in_source_xy) const noexcept
  {
    auto out        = *this;
    out.m_source_xy = in_source_xy;
    return out;
  }
  [[nodiscard]] constexpr this_type
    with_source_xy(source_type in_source_x,
                   source_type in_source_y) const noexcept
  {
    auto out        = *this;
    out.m_source_xy = out.m_source_xy.with_x(in_source_x);
    out.m_source_xy = out.m_source_xy.with_y(in_source_y);
    return out;
  }
  [[nodiscard]] constexpr auto
    source_xy() const noexcept
  {
    return m_source_xy;
  }
  [[nodiscard]] constexpr this_type
    with_xy(decltype(m_xy) in_xy) const noexcept
  {
    auto out = *this;
    out.m_xy = in_xy;
    return out;
  }
  [[nodiscard]] constexpr this_type
    with_xy(output_type in_x, output_type in_y) const noexcept
  {
    auto out = *this;
    out.m_xy = out.m_xy.with_x(in_x);
    out.m_xy = out.m_xy.with_y(in_y);
    return out;
  }
  [[nodiscard]] constexpr auto
    xy() const noexcept
  {
    return m_xy;
  }
  [[nodiscard]] constexpr this_type
    with_x(output_type in_x) const noexcept
  {
    auto out = *this;
    out.m_xy = m_xy.with_x(in_x);
    return out;
  }
  [[nodiscard]] constexpr auto
    x() const noexcept
  {
    return m_xy.x();
  }
  [[nodiscard]] constexpr this_type
    with_y(output_type in_y) const noexcept
  {
    auto out = *this;
    out.m_xy = m_xy.with_y(in_y);
    return out;
  }
  [[nodiscard]] constexpr auto
    y() const noexcept
  {
    return m_xy.y();
  }
  [[nodiscard]] constexpr this_type
    with_z(decltype(m_z) in_z) const noexcept
  {
    auto out = *this;
    out.m_z  = in_z;
    return out;
  }
  [[nodiscard]] constexpr auto &
    z() const noexcept
  {
    return m_z;
  }
  [[nodiscard]] constexpr this_type
    with_texture_id(decltype(m_tex_id_buffer.id()) in_texture_id) const noexcept
  {
    auto out            = *this;
    out.m_tex_id_buffer = m_tex_id_buffer.with_id(in_texture_id);
    return out;
  }
  [[nodiscard]] constexpr auto
    texture_id() const noexcept
  {
    return m_tex_id_buffer.id();
  }
  [[nodiscard]] constexpr this_type
    with_blend(decltype(m_tex_id_buffer.blend()) in_blend) const noexcept
  {
    auto out            = *this;
    out.m_tex_id_buffer = m_tex_id_buffer.with_blend(in_blend);
    return out;
  }
  [[nodiscard]] constexpr auto
    blend() const noexcept
  {
    return m_tex_id_buffer.blend();
  }
  [[nodiscard]] constexpr this_type
    with_depth(decltype(m_tex_id_buffer.depth()) in_depth) const noexcept
  {
    auto out            = *this;
    out.m_tex_id_buffer = m_tex_id_buffer.with_depth(in_depth);
    return out;
  }
  [[nodiscard]] constexpr auto
    depth() const noexcept
  {
    return m_tex_id_buffer.depth();
  }
  [[nodiscard]] constexpr this_type
    with_draw(decltype(m_tex_id_buffer.draw()) in_draw) const noexcept
  {
    auto out            = *this;
    out.m_tex_id_buffer = m_tex_id_buffer.with_draw(in_draw);
    return out;
  }
  [[nodiscard]] constexpr bool
    draw() const noexcept
  {
    return m_tex_id_buffer.draw();
  }
  [[nodiscard]] constexpr auto
    palette_id() const noexcept
  {
    return m_palette_id.id();
  }
  [[nodiscard]] constexpr this_type
    with_source_x(source_type in_source_x) const noexcept
  {
    auto out        = *this;
    out.m_source_xy = m_source_xy.with_x(in_source_x);
    return out;
  }
  [[nodiscard]] constexpr auto
    source_x() const noexcept
  {
    return m_source_xy.x();
  }
  [[nodiscard]] constexpr this_type
    with_source_y(source_type in_source_y) const noexcept
  {
    auto out        = *this;
    out.m_source_xy = m_source_xy.with_y(in_source_y);
    return out;
  }
  [[nodiscard]] constexpr auto
    source_y() const noexcept
  {
    return m_source_xy.y();
  }
  [[nodiscard]] constexpr this_type
    with_layer_id(decltype(m_layer_id.id()) in_layer_id) const noexcept
  {
    auto out = *this;
    if constexpr (!std::is_const_v<decltype(out.m_layer_id)>) {
      out.m_layer_id = m_layer_id.with_id(in_layer_id);
    }
    return out;
  }
  [[nodiscard]] constexpr auto
    layer_id() const noexcept
  {
    return m_layer_id.id();
  }
  [[nodiscard]] constexpr this_type
    with_blend_mode(decltype(m_blend_mode) in_blend_mode) const noexcept
  {
    auto out = *this;
    if constexpr (!std::is_const_v<decltype(out.m_blend_mode)>) {
      out.m_blend_mode = in_blend_mode;
    }
    return out;
  }
  [[nodiscard]] constexpr auto
    blend_mode() const noexcept
  {
    return m_blend_mode;
  }
  [[nodiscard]] constexpr this_type
    with_animation_id(decltype(m_animation_id) in_animation_id) const noexcept
  {
    auto out = *this;
    if constexpr (!std::is_const_v<decltype(out.m_animation_id)>) {
      out.m_animation_id = in_animation_id;
    }
    return out;
  }
  [[nodiscard]] constexpr auto
    animation_id() const noexcept
  {
    return m_animation_id;
  }
  [[nodiscard]] constexpr this_type
    with_animation_state(
      decltype(m_animation_state) in_animation_state) const noexcept
  {
    auto out = *this;
    if constexpr (!std::is_const_v<decltype(out.m_animation_state)>) {
      out.m_animation_state = in_animation_state;
    }
    return out;
  }
  [[nodiscard]] constexpr auto
    animation_state() const noexcept
  {
    return m_animation_state;
  }
  [[nodiscard]] constexpr auto
    source_rectangle() const noexcept
  {
    return Rectangle<source_type>{ m_source_xy.x(),
                                   m_source_xy.y(),
                                   width<source_type>(),
                                   height<source_type>() };
  }
  [[nodiscard]] constexpr auto
    output_rectangle() const noexcept
  {
    return Rectangle<output_type>{
      m_xy.x(), m_xy.y(), width<output_type>(), height<output_type>()
    };
  }
};
}// namespace open_viii::graphics::background
#endif// OPENVIII_CPP_WIP_TILECOMMON_HPP