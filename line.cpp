#include "line.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <raymath.h>

constexpr float STATION_RADIUS = 8;
constexpr float INFLECTION_POINT_SPLINE_RADIUS = 30;
constexpr float CONNECTION_THICK = 5.0;
constexpr float INFLECTION_BEZ_CONTROL_OFFSET = 20;

constexpr bool DRAW_BEZ_DEBUG = false;

static void draw_stations(std::vector<Station>& stations) {
   for(auto& station : stations) {
      DrawCircle(station.pos.x, station.pos.y, STATION_RADIUS, BLACK);
      DrawCircle(station.pos.x, station.pos.y, STATION_RADIUS - 3, WHITE);
   }
}

static void draw_connection(Vector2 p1, Vector2 p2, Color color) {
   float mid_scalar = std::min(std::abs(p1.x - p2.x), std::abs(p1.y - p2.y));
   auto inflection = p1;
   inflection.x += p2.x > p1.x ? mid_scalar : -mid_scalar;
   inflection.y += p2.y > p1.y ? mid_scalar : -mid_scalar;

   auto p1_to_inflect = Vector2Normalize(Vector2Subtract(inflection, p1));
   auto p1_pre_inflect = Vector2Subtract(
      inflection,
      Vector2Scale(p1_to_inflect, INFLECTION_POINT_SPLINE_RADIUS)
   );
   DrawLineEx(p1, p1_pre_inflect, CONNECTION_THICK, color);

   auto p2_to_inflect = Vector2Normalize(Vector2Subtract(inflection, p2));

   auto p2_post_inflect = Vector2Subtract(
      inflection,
      Vector2Scale(p2_to_inflect, INFLECTION_POINT_SPLINE_RADIUS)
   );
   DrawLineEx(p2_post_inflect, p2, CONNECTION_THICK, color);

   auto control_1 = Vector2Add(
      p1_pre_inflect,
      Vector2Scale(p1_to_inflect, INFLECTION_BEZ_CONTROL_OFFSET)
   );

   auto control_2 = Vector2Add(
      p2_post_inflect,
      Vector2Scale(p2_to_inflect, INFLECTION_BEZ_CONTROL_OFFSET)
   );

   DrawSplineSegmentBezierCubic(
      p1_pre_inflect,
      control_1,
      control_2,
      p2_post_inflect,
      CONNECTION_THICK,
      color
   );

   // debug
   if(DRAW_BEZ_DEBUG) {
      DrawCircleLines(control_1.x, control_1.y, 7, RED);
      DrawCircleLines(control_2.x, control_2.y, 7, ORANGE);
      DrawCircleLines(
         inflection.x,
         inflection.y,
         INFLECTION_POINT_SPLINE_RADIUS,
         GREEN
      );
   }
}

static void draw_connections(std::vector<Station>& stations, Color color) {
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