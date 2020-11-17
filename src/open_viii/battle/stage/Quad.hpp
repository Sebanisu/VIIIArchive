//
// Created by pcvii on 11/17/2020.
//

#ifndef VIIIARCHIVE_QUAD_HPP
#define VIIIARCHIVE_QUAD_HPP
#include "GpuFlags.hpp"
#include "open_viii/graphics/Color.hpp"
#include "open_viii/graphics/Point.hpp"
#include <bit>
#include <cstdint>
namespace open_viii::battle::stage {
/**
 * @see http://wiki.ffrtt.ru/index.php?title=FF8/FileFormat_X#Quad
 */
struct Quad
{
private:
  std::uint16_t m_face_indice_a{};
  std::uint16_t m_face_indice_b{};
  std::uint16_t m_face_indice_c{};
  std::uint16_t m_face_indice_d{};
  graphics::Point<std::uint8_t> m_uv1{};
  std::uint16_t m_raw_clut{};
  graphics::Point<std::uint8_t> m_uv2{};
  std::uint8_t m_raw_texture_page{};
  std::uint8_t m_raw_hide{};
  graphics::Point<std::uint8_t> m_uv3{};
  graphics::Point<std::uint8_t> m_uv4{};
  graphics::Color24<0, 1, 2> m_color{};
  GpuFlags m_raw_gpu{};

  static constexpr std::uint8_t MASK_4_BIT = 0xFU;
  static constexpr int SHIFT_2_BIT = 2;

public:
  [[nodiscard]] static constexpr std::uint8_t clut(const std::uint16_t &in_clut_raw) noexcept
  {
    return (std::rotl(in_clut_raw, SHIFT_2_BIT) & MASK_4_BIT);
  }
  [[nodiscard]] std::uint8_t clut() const noexcept
  {
    return clut(m_raw_clut);
  }
  [[nodiscard]] std::uint8_t texture_page() const noexcept
  {
    return (m_raw_texture_page & MASK_4_BIT);
  }
  [[nodiscard]] auto color() const noexcept
  {
    return m_color;
  }
  [[nodiscard]] bool hide() const noexcept
  {
    return m_raw_hide != 0U;
  }
  [[nodiscard]] auto uv(const std::uint8_t &index) const noexcept
  {
    switch (index) {
    case 0:
      return m_uv1;
    case 1:
      return m_uv2;
    case 2:
      return m_uv3;
    case 3:
      return m_uv4;
    }
  }

  [[nodiscard]] auto face_indice(const std::uint8_t &index) const noexcept
  {
    switch (index) {
    case 0:
      return m_face_indice_a;
    case 1:
      return m_face_indice_b;
    case 2:
      return m_face_indice_c;
    case 3:
      return m_face_indice_d;
    }
  }
  static constexpr std::size_t EXPECTED_SIZE = 24U;
};
static_assert(sizeof(Quad) == Quad::EXPECTED_SIZE);
}// namespace open_viii::battle::stage
#endif// VIIIARCHIVE_QUAD_HPP