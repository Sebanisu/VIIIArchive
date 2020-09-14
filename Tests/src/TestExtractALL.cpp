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
#include "TestExtractALL.h"
#include "OpenVIII/Paths/Paths.h"
#include "OpenVIII/Archive/Archives.h"

int main()
{
  for (auto &path : open_viii::Paths::get()) {
    open_viii::Tools::replace_slashes(path);
    if (!std::filesystem::exists(path)) {
      continue;
    }
    std::cout << path << std::endl;
    const auto archives = open_viii::archive::Archives<open_viii::LangT::en>(path);
    [[maybe_unused]] static constexpr auto dump = [](std::vector<char> &&rvalue_buffer, std::string &&rvalue_path) {
      std::vector<char> in_buffer{ std::move(rvalue_buffer) };
      std::string in_path{ std::move(rvalue_path) };
      open_viii::Tools::write_buffer(in_buffer, in_path);
    };
    archives.execute_on({}, dump);
  }
}
