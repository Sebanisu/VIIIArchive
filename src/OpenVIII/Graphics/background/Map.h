//
// Created by pcvii on 9/3/2020.
//

#ifndef VIIIARCHIVE_MAP_H
#define VIIIARCHIVE_MAP_H
#include <cstdint>
#include "Tile1.h"
#include "Tile2.h"
#include "Mim.h"
namespace open_viii::graphics::background {
template<size_t typeT = 1> requires(typeT >= 0 && typeT <= 2) struct Map
{
private:
  mutable std::vector<Tile1> m_t1{};
  mutable std::vector<Tile2> m_t2{};

  void remove_end()
  {
    static constexpr auto cmp = [](const auto &x) {
      static constexpr auto END_X{ 0x7FFFU };
      return x.x() == END_X;//|| !x.draw();
    };

    std::erase_if(m_t1, cmp);
    std::erase_if(m_t2, cmp);
  }
  void remove_duplicates()
  {
    {
      auto last = std::unique(m_t1.begin(), m_t1.end());
      m_t1.erase(last, m_t1.end());
    }
    {
      auto last = std::unique(m_t2.begin(), m_t2.end());
      m_t2.erase(last, m_t2.end());
    }
  }
  void sort()
  {
    static constexpr auto cmp = [](const auto &l, const auto &r) -> bool {
      if (l.z() > r.z()) {
        return true;
      }
      if (l.z() < r.z()) {
        return false;
      }
      if (l.layer_id() < r.layer_id()) {
        return true;
      }
      if (l.layer_id() > r.layer_id()) {
        return false;
      }
      if (l.animation_id() < r.animation_id()) {
        return true;
      }
      if (l.animation_id() > r.animation_id()) {
        return false;
      }
      if (l.animation_state() < r.animation_state()) {
        return true;
      }
      if (l.animation_state() > r.animation_state()) {
        return false;
      }
      if (l.x() > r.x()) {
        return true;
      }
      if (l.x() < r.x()) {
        return false;
      }
      if (l.y() > r.y()) {
        return true;
      }
      return false;
    };
    std::sort(m_t1.begin(), m_t1.end(), cmp);
    std::sort(m_t2.begin(), m_t2.end(), cmp);
  }

public:
  Map() = default;
  explicit Map([[maybe_unused]] std::span<const char> buffer)
  {
    if constexpr (typeT == 0 || typeT == 1) {
      auto t1 = std::span(
        reinterpret_cast<const Tile1 *>(std::ranges::data(buffer)), std::ranges::size(buffer) / sizeof(Tile1));
      m_t1 = { t1.begin(), t1.end() };
    }
    if constexpr (typeT == 0 || typeT == 2) {
      auto t2 = std::span(
        reinterpret_cast<const Tile2 *>(std::ranges::data(buffer)), std::ranges::size(buffer) / sizeof(Tile2));
      m_t2 = { t2.begin(), t2.end() };
    }
    remove_end();
    remove_duplicates();
    sort();
  }
  [[nodiscard]] const auto &tiles() const noexcept
  {
    if constexpr (typeT == 1) {
      return m_t1;
    } else if constexpr (typeT == 2) {
      return m_t2;
    } else {
      return nullptr;
    }
  }
  [[maybe_unused]] [[nodiscard]] const auto &min_x() const noexcept
  {
    const auto &t = tiles();
    if constexpr (!std::is_null_pointer_v<decltype(t)>) {
      return (
        std::min_element(t.cbegin(), t.cend(), [](const auto &a, const auto &b) -> bool { return a.x() < b.x(); }))
        ->x();
    } else {
      return 0;
    }
  }
  [[nodiscard]] auto minmax_x() const noexcept
  {
    const auto &t = tiles();
    if constexpr (!std::is_null_pointer_v<decltype(t)>) {
      return std::minmax_element(
        t.cbegin(), t.cend(), [](const auto &a, const auto &b) -> bool { return a.x() < b.x(); });
    } else {
      return nullptr;
    }
  }

  [[maybe_unused]] [[nodiscard]] const auto &max_x() const noexcept
  {
    const auto &t = tiles();
    if constexpr (!std::is_null_pointer_v<decltype(t)>) {
      return (
        std::max_element(t.cbegin(), t.cend(), [](const auto &a, const auto &b) -> bool { return a.x() < b.x(); }))
        ->x();
    } else {
      return 0;
    }
  }
  [[nodiscard]] auto minmax_y() const noexcept
  {
    const auto &t = tiles();
    if constexpr (!std::is_null_pointer_v<decltype(t)>) {
      return std::minmax_element(
        t.cbegin(), t.cend(), [](const auto &a, const auto &b) -> bool { return a.y() < b.y(); });
    } else {
      return nullptr;
    }
  }
  [[maybe_unused]] [[nodiscard]] const auto &min_y() const noexcept
  {
    const auto &t = tiles();
    if constexpr (!std::is_null_pointer_v<decltype(t)>) {
      return (
        std::min_element(t.cbegin(), t.cend(), [](const auto &a, const auto &b) -> bool { return a.y() < b.y(); }))
        ->y();
    } else {
      return 0;
    }
  }

  [[maybe_unused]] [[nodiscard]] const auto &max_y() const noexcept
  {
    const auto &t = tiles();
    if constexpr (!std::is_null_pointer_v<decltype(t)>) {
      return (
        std::max_element(t.cbegin(), t.cend(), [](const auto &a, const auto &b) -> bool { return a.y() < b.y(); }))
        ->y();
    } else {
      return 0;
    }
  }

  [[nodiscard]] auto unique_pupu() const
  {

    const auto &t = tiles();
    if constexpr (!std::is_null_pointer_v<decltype(t)>) {
      std::vector<std::uint64_t> out{};
      out.resize(std::ranges::size(t));
      std::transform(std::ranges::cbegin(t), std::ranges::cend(t), std::ranges::begin(out), [this](const auto &tile) {
        return generate_pupu(tile);
      });
      std::sort(out.begin(), out.end());
      auto last = std::unique(std::ranges::begin(out), std::ranges::end(out));
      out.erase(last, std::ranges::end(out));
      return out;
    } else {
      return nullptr;
    }
  }
  [[nodiscard]] auto used_depth_and_palette() const
  {
    const auto &t = tiles();
    if constexpr (!std::is_null_pointer_v<decltype(t)>) {
      /* What do I use...
       * I need to Get all unique palette and depth combinations.
       * transform lets me put the values into new container
       * unique_copy, oh this requires sorted data to work correctly. but i don't wanna sort my data again.
       * */
      using T = typename std::decay<decltype(*t.begin())>::type;
      std::vector<T> out_s1{};
      static constexpr auto default_size = 16U;
      out_s1.reserve(default_size);
      std::unique_copy(t.begin(), t.end(), std::back_inserter(out_s1), [](const auto &left, const auto &right) {
        return left.depth() == right.depth() && left.palette_id() == right.palette_id();
      });
      std::vector<std::pair<BPPT, std::uint8_t>> out{};
      out.reserve(std::ranges::size(out_s1));
      std::transform(out_s1.begin(), out_s1.end(), std::back_inserter(out), [](const auto &tile) {
        return std::make_pair(tile.depth(), tile.palette_id());
      });
      std::sort(out.begin(), out.end(), [](const auto &left, const auto &right) {
        if (left.first < right.first) {
          return true;
        }
        if (left.first > right.first) {
          return false;
        }
        if (left.second < right.second) {
          return true;
        }
        return false;
      });
      auto last = std::unique(out.begin(), out.end());
      out.erase(last, out.end());
      return out;
    } else {
      return nullptr;
    }
  }
  [[nodiscard]] Rectangle<std::int32_t> canvas() const noexcept
  {
    const auto l_minmax_x = minmax_x();
    const auto l_minmax_y = minmax_y();

    if constexpr (!std::is_null_pointer_v<decltype(l_minmax_x)> || !std::is_null_pointer_v<decltype(l_minmax_y)>) {
      const auto &[l_min_x, l_max_x] = l_minmax_x;
      const auto &[l_min_y, l_max_y] = l_minmax_y;
      constexpr static auto tile_size = 16;

      open_viii::graphics::Rectangle<std::int32_t> l_canvas{};
      l_canvas.x(l_min_x->x());
      l_canvas.y(l_min_y->y());
      l_canvas.height(static_cast<std::int32_t>(std::abs(l_max_y->y()) + std::abs(l_min_y->y()) + tile_size));
      l_canvas.width(static_cast<std::int32_t>(std::abs(l_max_x->x()) + std::abs(l_min_x->x()) + tile_size));
      return l_canvas;
    } else {
      return {};
    }
  }
  void shift_to_origin() const noexcept
  {
    const auto l_minmax_x = minmax_x();
    const auto l_minmax_y = minmax_y();

    if constexpr (!std::is_null_pointer_v<decltype(l_minmax_x)> || !std::is_null_pointer_v<decltype(l_minmax_y)>) {
      const auto &[l_min_x, l_max_x] = l_minmax_x;
      const auto &[l_min_y, l_max_y] = l_minmax_y;
      const auto abs_x = std::abs(l_min_x->x());
      const auto abs_y = std::abs(l_min_y->y());

      if constexpr (typeT == 1 || typeT == 0) {
        std::transform(std::execution::seq, m_t1.begin(), m_t1.end(), m_t1.begin(), [&abs_x, &abs_y](Tile1 t) {
          t.x(static_cast<std::int16_t>(t.x() + abs_x));
          t.y(static_cast<std::int16_t>(t.y() + abs_y));
          return t;
        });
      }

      if constexpr (typeT == 2 || typeT == 0) {
        std::transform(std::execution::seq, m_t2.begin(), m_t2.end(), m_t2.begin(), [&abs_x, &abs_y](Tile2 t) {
          t.x(static_cast<std::int16_t>(t.x() + abs_x));
          t.y(static_cast<std::int16_t>(t.y() + abs_y));
          return t;
        });
      }
    }
  }
  [[nodiscard]] friend std::ostream &operator<<(std::ostream &os, const Map &m)
  {
    return os << std::ranges::size(m.m_t1) << ", " << std::ranges::size(m.m_t2);
  }
  template<typename tileT>
  requires(std::is_convertible_v<tileT, Tile1> || std::is_convertible_v<tileT, Tile2>) std::uint64_t
    generate_pupu(const tileT &tile) const noexcept
  {
    static constexpr auto bits_per_long = sizeof(std::uint64_t) * 8U;
    static constexpr auto bits_per_short = sizeof(std::uint16_t) * 8U;
    static constexpr auto bits_per_byte = sizeof(std::uint8_t) * 8U;// 0-255
    static constexpr auto bits_per_nibble = bits_per_byte / 2U;// 0-15
    static constexpr auto bits_per_crumb = bits_per_nibble / 2U;// 0-3
    [[maybe_unused]] static constexpr auto short_mask = 0xFFFFU;
    [[maybe_unused]] static constexpr auto byte_mask = 0xFFU;
    [[maybe_unused]] static constexpr auto nibble_mask = 0xFU;
    [[maybe_unused]] static constexpr auto crumb_mask = 0x3U;
    auto bits = bits_per_long;
    std::uint64_t pupu_id{};
    [[maybe_unused]] static const auto add_crumb = [&bits, &pupu_id](const auto &value) {
      bits -= bits_per_crumb;
      pupu_id = (static_cast<std::uint64_t>(value) & crumb_mask) << bits;
    };
    [[maybe_unused]] static const auto add_nibble = [&bits, &pupu_id](const auto &value) {
      bits -= bits_per_nibble;
      pupu_id = (static_cast<std::uint64_t>(value) & nibble_mask) << bits;
    };

    [[maybe_unused]] static const auto add_byte = [&bits, &pupu_id](const auto &value) {
      bits -= bits_per_byte;
      pupu_id += (static_cast<std::uint64_t>(value) & byte_mask) << bits;
    };
    [[maybe_unused]] static const auto add_short = [&bits, &pupu_id](const auto &value) {
      bits -= bits_per_short;
      pupu_id += (static_cast<std::uint64_t>(value) & short_mask) << bits;
    };
    add_crumb(tile.depth().raw());
    add_nibble(tile.layer_id());
    add_nibble(tile.blend_mode());
    add_byte(tile.animation_id());
    add_byte(tile.animation_state());
    add_short(tile.z());
    //  Debug.Assert(((pupu_id & 0xF0000000) >> 28) == LayerID);
    //  Debug.Assert((pupu_id)((PupuID & 0x0F000000) >> 24) == BlendMode);
    //  Debug.Assert(((pupu_id & 0x00FF0000) >> 16) == AnimationID);
    //  Debug.Assert(((pupu_id & 0x0000FF00) >> 8) == AnimationState);
    return pupu_id;
  }
  void save([[maybe_unused]] const Mim &in_mim, [[maybe_unused]] const std::string_view &in_path) const
  {

    const auto &ts = tiles();
    if constexpr (!std::is_null_pointer_v<decltype(ts)>) {
      Rectangle<std::int32_t> rect = canvas();
      const auto dnps = used_depth_and_palette();
      const auto upupu = unique_pupu();
      if constexpr (!std::is_null_pointer_v<decltype(dnps)> && !std::is_null_pointer_v<decltype(upupu)>) {

//        for (const auto &pupu : upupu) {
//          std::cout << std::uppercase << std::hex << std::setfill('0') << std::setw(16U) << pupu << '\n';
//        }
//        return;
        for (const auto &[depth, palette] : dnps) {
          const auto ppm_save = [this, &rect, &depth, &palette, &ts](const std::span<const Color16> &data,
                                  const std::size_t &width,
                                  const std::size_t &height,
                                  std::string local_filename) {
            // Ppm::save(data, width, height, local_filename);
            std::vector<Color16> out{};
            out.resize(width * height);
            for (const auto &t : ts) {
              // auto x = t.x();
              // auto y = t.y();

              if (depth != t.depth() || palette != t.palette_id() || !t.draw())
                continue;
              for (std::uint32_t y = {}; y < t.HEIGHT; y++) {
                for (std::uint32_t x = {}; x < t.WIDTH; x++) {
                  auto pixel_in = (t.source_x() + x) + ((t.source_y() + y) * width);
                  if (t.depth().bpp4()) {
                    pixel_in += t.TEXTURE_PAGE_WIDTH * t.texture_id() * 2U;
                  } else if (t.depth().bpp8()) {
                    pixel_in += t.TEXTURE_PAGE_WIDTH * t.texture_id();
                  }

                  else if (t.depth().bpp16()) {
                    pixel_in += (t.TEXTURE_PAGE_WIDTH * t.texture_id()) / 2U;
                  }
                  auto pixel_out =
                    (static_cast<std::uint32_t>(t.x()) + x)
                    + ((static_cast<std::uint32_t>(t.y()) + y) * static_cast<std::uint32_t>(rect.width()));
                  Color16 color = data[pixel_in];
                  if (!color.is_black()) {
                    out.at(pixel_out) = data[pixel_in];
                  }
                }
              }
            }
            auto p = std::filesystem::path(local_filename);
            local_filename = (p.parent_path() / p.stem()).string() + "_tiled_mim.map";
            Ppm::save(
              out, static_cast<std::uint32_t>(rect.width()), static_cast<std::uint32_t>(rect.height()), local_filename);
          };
          in_mim.get_colors(in_path, depth, palette, ppm_save, false);

          std::cout << depth << ", " << static_cast<std::uint32_t>(palette) << '\n';
        }
      }
    }
  }
};
}// namespace open_viii::graphics::background

#endif// VIIIARCHIVE_MAP_H