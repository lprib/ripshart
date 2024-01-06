#include "line.hpp"
#include <cmath>
#include <cstddef>
#include <algorithm>

constexpr int STATION_RADIUS = 8;

void draw_stations(std::vector<Station>& stations) {
   for(auto& station : stations) {
      DrawCircle(station.pos.x, station.pos.y, STATION_RADIUS, BLACK);
      DrawCircle(station.pos.x, station.pos.y, STATION_RADIUS - 3, WHITE);
   }
}

void draw_connection(Vector2 p1, Vector2 p2, Color color) {
   float mid_scalar = std::min(std::abs(p1.x - p2.x), std::abs(p1.y - p2.y));
   Vector2 inflection = p1;
   inflection.x += p2.x > p1.x ? mid_scalar : -mid_scalar;
   inflection.y += p2.y > p1.y ? mid_scalar : -mid_scalar;

   DrawLineEx(p1, inflection, 5.0, color);
   DrawLineEx(inflection, p2, 5.0, color);
}

void draw_connections(std::vector<Station>& stations, Color color) {
   if(stations.size() <= 2) {
      return;
   }

   for(std::size_t i = 0; i < stations.size() - 1; ++i) {
      draw_connection(stations[i].pos, stations[i + 1].pos, color);
   }
}

void Line::draw() {
   draw_connections(stations, color);
   draw_stations(stations);
}