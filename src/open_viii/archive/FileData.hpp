// This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef VIIIARCHIVE_FILEDATA_HPP
#define VIIIARCHIVE_FILEDATA_HPP
#include "FI.hpp"
#include "open_viii/tools/Tools.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
namespace open_viii::archive {
struct [[maybe_unused]] FileData
{
private:
  std::basic_string<char>
    m_filename{};// defined as char because some compilers default to wide char.
  std::uint64_t m_offset{};
  std::uint32_t m_size{};

public:
  constexpr FileData() = default;
  FileData(const FileData &) = default;
  FileData(FileData &&) = default;
  FileData &operator=(const FileData &) = default;
  FileData &operator=(FileData &&) = default;
  ~FileData() = default;
  [[maybe_unused]] FileData(const std::string_view &filename,
    const unsigned long offset,
    unsigned int size)
    : m_filename(filename), m_offset(offset), m_size(size)
  {
    tools::replace_slashes(m_filename);
  }
  [[nodiscard]] bool empty() const noexcept
  {
    return m_size == 0 || m_filename.empty();
  }
  explicit FileData(std::ifstream &fp, const std::uint32_t &string_length)
    : m_filename(tools::read_val<decltype(m_filename)>(fp, string_length)),
      m_offset(tools::read_val<decltype(m_offset)>(fp)),
      m_size(tools::read_val<decltype(m_size)>(fp))
  {}
  explicit FileData(std::ifstream &fp)
    : FileData(fp, tools::read_val<std::uint32_t>(fp))
  {
    tools::replace_slashes(m_filename);// make sure slashes match compiler
  }

  template<FI_Like fiT>
  requires(!std::is_same_v<fiT, FileData>) constexpr explicit FileData(
    const fiT &fi) noexcept
    : m_offset{ static_cast<decltype(m_offset)>(fi.offset()) },
      m_size{ static_cast<decltype(m_size)>(fi.uncompressed_size()) }
  {}

  // size of this file entry in the zzz file.
  [[maybe_unused]] [[nodiscard]] constexpr auto total_size()
  {
    return sizeof(unsigned int) + std::ranges::size(m_filename)
           + sizeof(m_offset) + sizeof(m_size);
  }
  // gets path as a std::filesystem::path
  [[maybe_unused]] [[nodiscard]] auto get_path() const
  {
    return std::filesystem::path(m_filename);
  }
  // convert to FI. (loss of precision size_t to unsigned int)
  //[[maybe_unused]] [[nodiscard]] constexpr auto GetFI() const
  //{
  //  return FI(static_cast<unsigned int>(size_), static_cast<unsigned
  //  int>(offset_));
  //}
  // get size of file
  [[maybe_unused]] [[nodiscard]] constexpr auto size() const noexcept
  {
    return m_size;
  }
  [[maybe_unused]] [[nodiscard]] static constexpr auto
    compression_type() noexcept
  {
    return CompressionTypeT::none;
  }
  // alias for Size that should mirror FI
  [[maybe_unused]] [[nodiscard]] constexpr auto
    uncompressed_size() const noexcept
  {
    return m_size;
  }
  // get offset of file
  [[maybe_unused]] [[nodiscard]] constexpr auto offset() const noexcept
  {
    return m_offset;
  }
  // gets path as a std::string_view
  [[maybe_unused]] [[nodiscard]] auto get_path_string() const
  {
    return std::string_view(m_filename);
  }
  [[maybe_unused]] [[nodiscard]] auto get_tuple() const
  {
    return std::make_tuple(get_path_string(), offset(), size());
  }
};
}// namespace open_viii::archive
#endif// VIIIARCHIVE_FILEDATA_HPP
