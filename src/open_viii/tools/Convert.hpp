//
// Created by pcvii on 2/15/2021.
//
#ifndef VIIIARCHIVE_CONVERT_HPP
#define VIIIARCHIVE_CONVERT_HPP
#include <algorithm>
#include <cctype>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <filesystem>
namespace open_viii::tools {
/**
 * Workaround there is no way to currently to print a utf8 string... streams are
 * char only pretty much.
 * @param s8 utf8 string_view
 * @return char string_view.
 * @todo needs tests?
 */
[[maybe_unused]] static std::string_view u8_to_sv(const std::u8string_view &s8)
{
  return { reinterpret_cast<const char *>(s8.data()), s8.size() };
}
/**
 * Workaround there is no way to currently to print a utf8 string... streams are
 * char only pretty much.
 * @param s8 utf8 string_view
 * @return char string
 * @todo needs tests?
 */
[[maybe_unused]] static std::string u8_to_s(const std::u8string_view &s8)
{
  auto sv = u8_to_sv(s8);
  return { sv.begin(), sv.end() };
}
/**
 * Convert character to uppercase version
 * @param ch lowercase character and such.
 * @return uppercase character
 * @note ::toupper is not constexpr
 * @see https://en.cppreference.com/w/cpp/string/byte/toupper
 */
static constexpr auto upper(int ch)
{
  if (std::is_constant_evaluated()) {
    constexpr char upper_offset = 'a' - 'A';
    // this is really basic but should cover everything we do.
    return (ch >= 'a' && ch <= 'z') ? ch - upper_offset : ch;
  }
  return ::toupper(ch);
}
static_assert(upper('a') == 'A');
static_assert(upper('a') != 'Z');


[[maybe_unused]] static void replace_slashes(std::string &haystack)
{
  std::ranges::replace(
    haystack, '\\', std::filesystem::path::preferred_separator);
}
}// namespace open_viii::tools
#endif// VIIIARCHIVE_CONVERT_HPP