#ifndef TUBE_FILE_HPP
#define TUBE_FILE_HPP

#include <iostream>
#include <vector>

#include <raylib.h>

#include "tube_map.hpp"

namespace tube {

Map load(std::istream& in);
void save(Map const& map, std::ostream& out);

} // namespace map

#endif