#include "map_render.hpp"

namespace map {

static void render_lines(Map const& map) {
   for(auto& line : map.lines) {
      for(auto& conn_id : line.connection_ids) {
         auto& conn = map.connections[conn_id];
         if(conn.point_ids.size() > 1) {
            for(std::size_t i = 0; i < conn.point_ids.size() - 1; ++i) {
               DrawLineEx(
                  map.points[conn.point_ids[i]].pos,
                  map.points[conn.point_ids[i + 1]].pos,
                  5,
                  line.color
               );
            }
         }
      }
   }
}

static void render_stations(Map const& map) {
   for(auto& station : map.stations) {
      if(station.style == StationStyle::Circle) {
         for(auto& station_node_id : station.nodes) {
            auto& node = map.station_nodes[station_node_id];
            DrawCircle(node.pos.x, node.pos.y, 12, BLACK);
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
