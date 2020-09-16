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
#include "TestReswizzleFields.h"
#include "OpenVIII/Paths/Paths.h"
#include "OpenVIII/Archive/Archives.h"
#include "OpenVIII/Graphics/Lzs.hpp"
#include "OpenVIII/Graphics/Tim.hpp"
#include "OpenVIII/Graphics/Tdw.hpp"
#include "OpenVIII/Graphics/Tex.hpp"
#include "OpenVIII/Graphics/Sp1.hpp"
#include "OpenVIII/Graphics/Sp2.hpp"
#include "OpenVIII/MenuGroup/MenuGroupFile.h"
#include "OpenVIII/Graphics/background/Mim.h"
#include "OpenVIII/Graphics/background/Map.h"
#include "OpenVIII/Graphics/Color.hpp"
struct pupu_path
{
  open_viii::graphics::background::Pupu pupu{};
  std::filesystem::path path{};
};
int main()
{
  for (auto &path : open_viii::Paths::get()) {
    open_viii::Tools::replace_slashes(path);
    if (!std::filesystem::exists(path)) {
      continue;
    }
    // std::cout << path << std::endl;
    const auto archives = open_viii::archive::Archives<open_viii::LangT::en>(path);
    [[maybe_unused]] const auto &field = archives.get<open_viii::archive::ArchiveTypeT::field>();
    //    [[maybe_unused]]const auto ppms =
    //     open_viii::tools::i_path_extension_ends_with_any(std::filesystem::current_path(),{"test"},{".ppm"});

    open_viii::Tools::execute_on_directories(
      std::filesystem::current_path(), {"ecenter3"}, [&field](const std::filesystem::path &directory_path) {
        const std::string fi_filename = directory_path.filename().string() + std::string(open_viii::archive::FI::EXT);
        const std::string fl_filename = directory_path.filename().string() + std::string(open_viii::archive::FL::EXT);
        const std::string fs_filename = directory_path.filename().string() + std::string(open_viii::archive::FS::EXT);
        auto archive = field.get_fiflfs({ fi_filename, fl_filename, fs_filename });

        if (!static_cast<bool>(archive)) {
          return;// no archive for this directory.
        }
        const std::string map_filename = directory_path.filename().string() + std::string(".map");
        const std::string mim_filename = directory_path.filename().string() + std::string(".mim");
        const std::string output_prefix = (directory_path / directory_path.filename()).string();
        // std::cout << map_filename << '\n';
        // std::cout << archive << '\n';
        auto mims = archive.get_vector_of_indexs_and_files({ mim_filename });
        if (std::ranges::empty(mims)) {
          return;// no mim file.
        }
        open_viii::FI_Like auto mim_fi = archive.get_entry_by_index(mims[0].first);
        open_viii::graphics::background::MimType mim_type = open_viii::graphics::background::Mim::get_texture_type(
          mim_fi.uncompressed_size(), directory_path.filename().string());

        const auto reswizzle = [&directory_path, &mim_type, &output_prefix]<typename tileT>(
                                 [[maybe_unused]] const open_viii::graphics::background::Map<tileT> &map) {
          map.save_csv(output_prefix + "_2.csv");
          // I need the pupu's with same bppt and path.
          std::array<std::vector<pupu_path>, 3> path_grouped_by_bppt{};

          std::vector<std::uint16_t> valid_texture_ids{};
          const auto tiles = map.tiles();
          valid_texture_ids.reserve(std::ranges::size(tiles));
          std::ranges::transform(
            tiles, std::back_insert_iterator(valid_texture_ids), [](const tileT &tile) { return tile.texture_id(); });
          std::ranges::sort(valid_texture_ids);
          auto last = std::unique(std::ranges::begin(valid_texture_ids), std::ranges::end(valid_texture_ids));
          valid_texture_ids.erase(last, std::ranges::end(valid_texture_ids));

          open_viii::Tools::execute_on_directory(
            directory_path, {}, { ".ppm" }, [&mim_type, &path_grouped_by_bppt](const std::filesystem::path &file_path) {
              // if(!file_path.has_stem()) {return;}
              auto basename = file_path.stem().string();
              static constexpr auto minsize{ 24 };
              if (std::ranges::size(basename) < minsize) {
                return;
              }
              auto suffix = std::string_view(basename).substr(std::ranges::size(basename) - 7);
              if (suffix != "_mimmap") {
                return;
              }
              auto hex = std::string_view(basename).substr(std::ranges::size(basename) - 23, 16);
              // auto prefix = std::string_view(basename).substr(0, std::ranges::size(basename) - 24);

              auto pp = pupu_path{ open_viii::graphics::background::Pupu(hex), file_path };

              if (pp.pupu.depth().bpp4()) {
                path_grouped_by_bppt.at(0).emplace_back(std::move(pp));
              } else if (pp.pupu.depth().bpp8()) {
                path_grouped_by_bppt.at(1).emplace_back(std::move(pp));
              } else if (pp.pupu.depth().bpp16()) {
                path_grouped_by_bppt.at(2).emplace_back(std::move(pp));
              }
            });
          const auto scale = 1;// scale would come from size of imported image and size of canvas from map.
          const auto width = mim_type.height() * scale;
          const auto height = mim_type.height() * scale;
          const auto area = width * height;
          auto out = std::vector<open_viii::graphics::Color24<0, 1, 2>>(area);
          for (const auto &tex_id : valid_texture_ids) {
            int bpp = 4;
            for (const auto &pupu_path_vector : path_grouped_by_bppt) {
              for (const pupu_path &pp : pupu_path_vector) {
                // std::cout << pp.pupu << ' ' << pp.path << std::endl;
                // load file
                std::ifstream fp{};
                fp.open(pp.path.string(), std::ios::binary | std::ios::in);// a way to skip this unless there are tiles
                if (!fp.is_open()) {
                  continue;
                }
                auto buffer = open_viii::Tools::read_buffer<std::string>(fp);
                auto ppm = open_viii::graphics::Ppm(buffer);
                // copy tiles used to output buffer.
                for (const tileT &tile : tiles | std::views::filter([&pp, &tex_id](const tileT &t) -> bool {
                       return pp.pupu == t && t.texture_id() == tex_id;
                     })) {
                  static constexpr auto tile_size = 16U;
                  for (std::size_t y = 0; y < tile_size * scale; y++) {
                    for (std::size_t x = 0; x < tile_size * scale; x++) {
                      const auto scaled_tile_y = static_cast<std::size_t>(tile.y() * scale);
                      const auto scaled_tile_x = static_cast<std::size_t>(tile.x() * scale);
                      auto color = ppm.color(x + scaled_tile_x, y + scaled_tile_y);
                      if (!color.is_black()) {
                        const std::size_t scaled_tile_source_x = tile.source_x() * scale;
                        const std::size_t scaled_tile_source_y = tile.source_y() * scale;
                        out.at(x + scaled_tile_source_x + ((y + scaled_tile_source_y) * width)) = color;
                      }
                    }
                  }
                }
              }
              // save output;
              std::string output_name = output_prefix + "_" + std::to_string(bpp) + "_" + std::to_string(tex_id);
              open_viii::graphics::Ppm::save(out, width, height, output_name);
              std::fill(std::ranges::begin(out), std::ranges::end(out), open_viii::graphics::Color24<0, 1, 2>{});
              // increase bpp;
              bpp *= 2;// 4,8,16
              if (bpp > 16) {
                break;
              }
            }
          }
        };
        const auto map_buffer = archive.get_entry_data(map_filename);
        if (mim_type.type() == 1) {
          reswizzle(open_viii::graphics::background::Map<open_viii::graphics::background::Tile1>(map_buffer));
        } else if (mim_type.type() == 2) {
          reswizzle(open_viii::graphics::background::Map<open_viii::graphics::background::Tile2>(map_buffer));
        } else if (mim_type.type() == 3) {
          reswizzle(open_viii::graphics::background::Map<open_viii::graphics::background::Tile3>(map_buffer));
        }
      });
  }
}
