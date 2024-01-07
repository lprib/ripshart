#include "tube_render.hpp"

#include "raylib.h"
#include "raymath.h"

namespace tube {

static void render_lines(Map const& map) {
   std::vector<int> connection_offset_counter(map.connections.size(), 0);

   for(auto& line : map.lines) {
      for(auto& conn_id : line.connection_ids) {
         auto& conn = map.connections[conn_id];
         if(conn.point_ids.size() > 1) {
            Vector2 down{0, 1};

            auto offset = connection_offset_counter[conn_id] * 5;
            auto offset_vec = Vector2Scale(down, offset);

            for(std::size_t i = 0; i < conn.point_ids.size() - 1; ++i) {
               DrawLineEx(
                  Vector2Add(map.points[conn.point_ids[i]].pos, offset_vec),
                  Vector2Add(map.points[conn.point_ids[i + 1]].pos, offset_vec),
                  5,
                  line.color
               );
            }
         }
         ++connection_offset_counter[conn_id];
      }
   }
}

static void render_stations(Map const& map) {
   for(auto& station : map.stations) {
      if(station.style == StationStyle::Circle) {
         for(auto& station_node_id : station.nodes) {
            auto& node = map.station_nodes[station_node_id];
            DrawCircle(node.pos.x, node.pos.y, 10, BLACK);
            DrawCircle(node.pos.x, node.pos.y, 5, WHITE);
         }
      }
   }
}

void render(Map const& map) {
   render_lines(map);
   render_stations(map);
}

} // namespace map
