#ifndef LINE_HPP
#define LINE_HPP

#include <raylib.h>
#include <vector>

struct Station {
   Vector2 pos = Vector2{};

   Station(float x, float y) {
      pos.x = x;
      pos.y = y;
   }
};

struct Line {
   std::vector<Station> stations;
   Color color;

   Line(std::vector<Station> stations, Color color) :
      stations{std::move(stations)},
      color{color} {}
   void draw();
};

#endif