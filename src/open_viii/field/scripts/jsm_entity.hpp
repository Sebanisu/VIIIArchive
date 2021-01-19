//
// Created by pcvii on 1/18/2021.
//

#ifndef VIIIARCHIVE_JSM_ENTITY_HPP
#define VIIIARCHIVE_JSM_ENTITY_HPP
#include <cstdint>
#include <type_traits>
#include <utility>
namespace open_viii::field::scripts {
struct jsm_entity
{
private:
  std::uint16_t m_raw{};

public:
  constexpr jsm_entity() = default;
  [[nodiscard]] constexpr std::uint16_t raw() const noexcept
  {
    return m_raw;
  }
  [[nodiscard]] constexpr std::uint8_t script_count() const noexcept
  {
    constexpr std::uint16_t mask = 0x7FU;
    return m_raw & mask;
  }
  [[nodiscard]] constexpr std::uint16_t entry_point_entity() const noexcept
  {
    constexpr std::uint16_t mask = 0x1FF;
    constexpr std::uint16_t shift = 0x7U;
    return static_cast<std::uint16_t>(m_raw >> shift) & mask;
  }


  /**
   * Get Value
   *@note required to structured binding support
   * @note can't be reference because it's a copy of 4 bits to 8 bits.
   */
  template<std::size_t I>
  requires(I < 2U) [[nodiscard]] constexpr auto get() const noexcept
  {
    if constexpr (I == 0U) {
      return script_count();
    } else if constexpr (I == 1U) {
      return entry_point_entity();
    }
  }
};
}

namespace std {
/**
 * number of arguments
 * @note required to structured binding support
 */
template<>
struct tuple_size<open_viii::field::scripts::jsm_script_data_item>
  : std::integral_constant<size_t, 2U>
{
};

/**
 * type of argument 0
 * @note required to structured binding support
 */
template<>
struct tuple_element<0U, open_viii::field::scripts::jsm_script_data_item>
{
  using type = std::uint16_t;
};
/**
 * type of argument 1
 * @note required to structured binding support
 */
template<>
struct tuple_element<1U, open_viii::field::scripts::jsm_script_data_item>
{
  using type = std::uint16_t;
};
#endif// VIIIARCHIVE_JSM_ENTITY_HPP
