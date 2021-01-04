//
// Created by pcvii on 11/18/2020.
//

#ifndef VIIIARCHIVE_CAMERAANIMATION_HPP
#define VIIIARCHIVE_CAMERAANIMATION_HPP
#include "CameraAnimationFrame.hpp"
#include "ControlWord.hpp"
#include "open_viii/graphics/Vertice.hpp"
#include <cstdint>
#include <vector>
namespace open_viii::battle::stage {
/**
 * http://wiki.ffrtt.ru/index.php?title=FF8/FileFormat_X#Camera_Animation_.28WIP.29
 */
struct CameraAnimation// TODO this isn't correct. Need to adjust to match
                      // open-viii
{
private:
  static constexpr auto MAX_FRAMES = 32U;
  static constexpr auto UNK_SIZE_0 = 20U;
  static constexpr auto UNK_SIZE_2 = 128U;
  static constexpr auto UNK_SIZE_3 = 34U;
  //  std::uint8_t m_animation_id{};
  //  std::uint8_t m_key_frame_count{};
  ControlWord m_main_controller{};//, if 0xFFFF then return
  std::uint16_t m_starting_fov{};//~usually 280
  std::uint16_t m_ending_fov{};// ~006
  std::uint16_t m_starting_roll{};// usually 0
  std::uint16_t m_ending_roll{};// usually 0
  std::vector<CameraAnimationFrame> m_frames{};


public:
  CameraAnimation() = default;
  explicit CameraAnimation(std::span<char const> span)
  {
    //read methods
    const auto read = [&span]<typename T>(T &val) -> T & {
      Tools::read_val(span, val);
      span = span.subspan(sizeof(T));
      return val;
    };
    const auto r_read = [&span]<typename T>() -> T {
      const auto res = Tools::read_val<T>(span);
      span = span.subspan(sizeof(T));
      return res;
    };
    //start reading
    read(m_main_controller);
    if (m_main_controller.end()) {
      return;
    }
    switch (m_main_controller.fov()) {
    case FovTypeT::default_fov: {
      static constexpr auto default_fov = 0x200U;
      m_ending_fov = m_starting_fov = default_fov;
      break;
    }
    case FovTypeT::same_fov: {
      read(m_starting_fov);
      span = span.subspan(sizeof(m_starting_fov));
      m_ending_fov = m_starting_fov;
      break;
    }
    case FovTypeT::different_fov: {
      read(m_starting_fov);
      span = span.subspan(sizeof(m_starting_fov));
      read(m_ending_fov);
      break;
    }
    }


    switch (m_main_controller.roll()) {
    case RollTypeT::default_roll: {
      m_ending_fov = m_starting_roll = 0x0U;
      break;
    }
    case RollTypeT::same_roll: {
      read(m_starting_roll);
      // span = span.subspan(sizeof(m_starting_roll));
      m_ending_fov = m_starting_roll;
      break;
    }
    case RollTypeT::different_roll: {
      read(m_starting_fov);
      span = span.subspan(sizeof(m_starting_fov));
      read(m_ending_fov);
      break;
    }
    case RollTypeT::TODO_roll:
      // TODO, what's ff8vars.unkWord1D977A2?
      // same as above; cam->unkWord00A = ff8vars.unkWord1D977A2;
      // This probably needs more reversing
      break;
    }

    switch (m_main_controller.layout()) {
    case LayoutTypeT::default_layout:
    case LayoutTypeT::unknown_layout:
      while (true)// I'm setting this to true and breaking in code as this works
                  // on peeking on next variable via pointer and that's not
                  // possible here without unsafe block
      {
        if (r_read.template operator()<std::int16_t>() < 0) {
          break;
        }// reverse of *current_position >= 0, also cast to signed is important
         // here
        //        totalFrameCount +=
        //          (ushort)(currentPosition *
        //                   16); //here is increment of short*, but I already
        //                   did that above
        m_frames.emplace_back(
          r_read.template operator()<CameraAnimationFrame>());
        //        IsFrameDurationsShot[keyFrameCount] =
        //          (byte)(br
        //                   .ReadUInt16()
        //          ); //cam->unkByte124[keyFrameCount] = *current_position++; -
        //          looks like we are wasting one byte due to integer sizes
        //        _cameraWorldX[keyFrameCount] = br.ReadInt16();
        //        _cameraWorldY[keyFrameCount] = br.ReadInt16();
        //        _cameraWorldZ[keyFrameCount] = br.ReadInt16();
        //        IsFrameEndingShots[keyFrameCount] =
        //          (byte)(br.ReadUInt16()); //m->unkByte204[keyFrameCount] =
        //          *current_position++;
        //        _cameraLookAtX[keyFrameCount] = br.ReadInt16();
        //        _cameraLookAtY[keyFrameCount] = br.ReadInt16();
        //        _cameraLookAtZ[keyFrameCount] = br.ReadInt16();
        //        keyFrameCount++;
      }

      //      if (m_frames.size() > 2U)
      //      {
      //        //ff8Functions.Sub50D010(cam->unkWord024, cam->unkWord064,
      //        cam->unkWord0A4, cam->unkWord0E4, keyFrameCount,
      //        cam->unkByte224, cam->unkByte2A4, cam->unkByte324);
      //        //ff8Functions.Sub50D010(cam->unkWord024, cam->unkWord144,
      //        cam->unkWord184, cam->unkWord1C4, keyFrameCount,
      //        cam->unkByte3A4, cam->unkByte424, cam->unkByte4A4);
      //      }

      break;

      //    case 1:
      //    {
      //      goto case 0;
      //      //if (currentPosition >= 0)
      //      //{
      //      //    var local14 = (short)(br.BaseStream.Position + 5 * 2);
      //      //    var local10 = (short)(br.BaseStream.Position + 6 * 2);
      //      //    var local2C = (short)(br.BaseStream.Position + 7 * 2);
      //      //    var local18 = (short)(br.BaseStream.Position + 1 * 2);
      //      //    var local1C = (short)(br.BaseStream.Position + 2 * 2);
      //      //    while (true)
      //      //    {
      //      //        StartFramesOffsets[keyFrameCount] = totalFrameCount;
      //      //        currentPosition = br.ReadUInt16();
      //      //        if ((short)currentPosition < 0) //reverse of
      //      *current_position >= 0, also cast to signed is important here
      //      //            break;
      //      //        totalFrameCount += (ushort)(currentPosition * 16);
      //      //        //ff8Functions.Sub503AE0(++local18, ++local1C, ++ebx,
      //      *(BYTE*)current_position, &cam->unkWord064[keyFrameCount],
      //      &cam->unkWord0A4[keyFrameCount], &cam->unkWord0E4[keyFrameCount]);
      //      //        //ff8Functions.Sub503AE0(++local14, ++local10,
      //      ++local2C, *(BYTE*)(current_position + 4),
      //      &cam->unkWord144[keyFrameCount], &cam->unkWord184[keyFrameCount],
      //      &cam->unkWord1C4[keyFrameCount]);
      //      //        IsFrameEndingShots[keyFrameCount] = 0xFB;
      //      //        IsFrameDurationsShot[keyFrameCount] = 0xFB;
      //      //        local1C += 8;
      //      //        local18 += 8;
      //      //        currentPosition += 8;
      //      //        local2C += 8;
      //      //        //ebx += 8;
      //      //        local10 += 8;
      //      //        local14 += 8;
      //      //        keyFrameCount++;
      //      //    }
      //      //}
      //      //break;
      //    }
    }
  }
  [[nodiscard]] auto &operator[](std::size_t i) noexcept
  {
    return m_frames[i];
  }
  [[nodiscard]] auto &at(std::size_t i)
  {
    return m_frames.at(i);
  }
  [[nodiscard]] auto size() const noexcept
  {
    return m_frames.size();
  }

  //  template<size_t I>
  //  requires(I < MAX_FRAMES)
  //    [[nodiscard]] Vertice<std::int16_t> get_camera_world() const noexcept
  //  {
  //    return { static_cast<std::int16_t>(m_camera_world_x[I]),
  //      static_cast<std::int16_t>(-m_camera_world_y[I]),
  //      static_cast<std::int16_t>(-m_camera_world_z[I]) };
  //  }
  //  template<size_t I>
  //  requires(I < MAX_FRAMES)
  //    [[nodiscard]] Vertice<std::int16_t> get_camera_look_at() const noexcept
  //  {
  //    return { static_cast<std::int16_t>(m_camera_look_at_x[I]),
  //      static_cast<std::int16_t>(-m_camera_look_at_y[I]),
  //      static_cast<std::int16_t>(-m_camera_look_at_z[I]) };
  //  }
  //  template<size_t I>
  //  requires(I < MAX_FRAMES)
  //    [[nodiscard]] bool is_frame_durations_shot() const noexcept
  //  {
  //    return m_is_frame_durations_shot[I] != 0;
  //  }
  //  template<size_t I>
  //  requires(I < MAX_FRAMES)
  //    [[nodiscard]] bool is_frame_ending_shots() const noexcept
  //  {
  //    return m_is_frame_ending_shots[I] != 0;
  //  }
  //  template<size_t I>
  //  requires(I < MAX_FRAMES) [[nodiscard]] std::uint16_t
  //    start_frame_offsets() const noexcept
  //  {
  //    return m_start_frame_offsets[I];
  //  }
  //  [[nodiscard]] const std::uint8_t &animation_id() const noexcept
  //  {
  //    return m_animation_id;
  //  }
  //  [[nodiscard]] const std::uint8_t &key_frame_count() const noexcept
  //  {
  //    return m_key_frame_count;
  //  }
  [[nodiscard]] const auto &main_controller() const noexcept
  {
    return m_main_controller;//, if 0xFFFF then return
  }
  [[nodiscard]] const std::uint16_t &starting_fov_usually() const noexcept
  {
    return m_starting_fov;//~280
  }
  [[nodiscard]] const std::uint16_t &ending_fov() const noexcept
  {
    return m_ending_fov;// ~006
  }
  //  [[nodiscard]] const std::uint16_t &starting_camera_roll() const noexcept
  //  {
  //    return m_starting_camera_roll;// usually 0
  //  }
  //  [[nodiscard]] const std::uint16_t &starting_time() const noexcept
  //  {
  //    return m_starting_time;// usually 0
  //  }
  //  [[nodiscard]] const std::uint16_t &current_time() const noexcept
  //  {
  //    return m_current_time;// I'm questioning if this is part of the struct.
  //  };
  //  static constexpr auto EXPECTED_SIZE = 1092U;
};
// static_assert(sizeof(CameraAnimation) == CameraAnimation::EXPECTED_SIZE);
}// namespace open_viii::battle::stage
#endif// VIIIARCHIVE_CAMERAANIMATION_HPP