#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <optional>
#include <raylib.h>
#include <vector>

#include "tube_file.hpp"
#include "tube_map.hpp"
#include "tube_render.hpp"

static void handle_key(int keycode);
static void draw_hud();
static void draw_grid();
static Vector2 get_snap_location();
static void do_connection();
static void draw_unclaimed_connections();
static void draw_selected_connection();

enum class UiState {
   Idle,
   ConnectionSelectEnd,
   ConnectionDoPath,
};

const std::vector<std::string> STATIC_HUD_TEXT = {
   "[-] grid smaller",
   "[=] grid larger",
   "[p] print dump",
   "[o] open testmap.map",
   "[s] place station",
   "[l] add line",
   "[]] select next line",
   "[[] select prev line",
   "[c] add connection",
   "[,] select prev connection",
   "[.] select next connection",
   "[a] add selected connection to selected line"
};

static constexpr int INIT_WINDOW_WIDTH = 640;
static constexpr int INIT_WINDOW_HEIGHT = 640;

const Color GRID_COLOR = GetColor(0x00000030);
const Color HUD_COLOR = GetColor(0x808080ff);
const Color SNAP_POS_COLOR = GetColor(0x45a2ff80);

static constexpr std::array<int, 5> GRID_SIZES = {10, 25, 50, 100, 200};

// UI state:
int window_width = INIT_WINDOW_WIDTH;
int window_height = INIT_WINDOW_HEIGHT;
static std::size_t current_grid_idx = 3;

static bool has_lines = false;
static std::size_t current_line_idx = 0;
static UiState state = UiState::Idle;
static std::string popup_msg = "";
static tube::ConnectionId selected_connection_id;

tube::Map map{};

int main(int argc, char** argv) {
   std::ifstream mapfile("/home/liam/programming/raylib_game/ripshart/tube.map"
   );
   // map = tube::load(mapfile);

   SetConfigFlags(FLAG_WINDOW_RESIZABLE);
   InitWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_WIDTH, "ripshart unshackled");
   SetTargetFPS(60);

   while(!WindowShouldClose()) {
      int key;
      while(key = GetKeyPressed()) {
         handle_key(key);
      }

      if(IsWindowResized()) {
         window_width = GetScreenWidth();
         window_height = GetScreenHeight();
      }

      BeginDrawing();
      {
         ClearBackground(WHITE);
         tube::render(map);
         draw_grid();
         draw_hud();
         draw_unclaimed_connections();
         draw_selected_connection();

         DrawCircleV(get_snap_location(), 10, SNAP_POS_COLOR);
      }
      EndDrawing();
   }
}

static void handle_key(int keycode) {
   popup_msg = "";

   switch(keycode) {
   case KEY_MINUS: {
      if(current_grid_idx != 0) {
         --current_grid_idx;
      }
   } break;
   case KEY_EQUAL: {
      if(current_grid_idx < GRID_SIZES.size() - 1) {
         ++current_grid_idx;
      }
   } break;
   case KEY_P: {
      tube::save(map, std::cout);
   } break;
   case KEY_O: {
      std::ifstream file(
         "/home/liam/programming/raylib_game/ripshart/testmap.map"
      );
      map = tube::load(file);
      
      // reset state
      has_lines = map.lines.size() != 0;
      selected_connection_id = 0;
      current_line_idx = 0;
      state = UiState::Idle;
   } break;
   case KEY_S: {
      auto node_id = map.station_nodes.size();
      map.station_nodes.push_back(tube::StationNode{get_snap_location()});
      map.stations.push_back(tube::Station{
         tube::StationStyle::Circle,
         {static_cast<tube::NodeId>(node_id)}
      });
   } break;
   case KEY_L: {
      current_line_idx = map.lines.size();
      auto color = GetColor((GetRandomValue(0, 0xffffff) << 8) | 0xff);
      map.lines.push_back(tube::Line{color, {}});
      has_lines = true;
   } break;
   case KEY_LEFT_BRACKET: {
      if(current_line_idx != 0) {
         --current_line_idx;
      }
   } break;
   case KEY_RIGHT_BRACKET: {
      if(current_line_idx < map.lines.size() - 1) {
         ++current_line_idx;
      }
   } break;
   case KEY_C: {
      do_connection();
   } break;
   case KEY_SPACE: {
      if(state == UiState::ConnectionDoPath) {
         // todo re-use old points?
         auto new_point_id = map.points.size();
         map.points.push_back(tube::TrackPoint{get_snap_location()});
         map.connections.back().point_ids.push_back(new_point_id);
      }
   } break;
   case KEY_ENTER: {
      if(state == UiState::ConnectionDoPath) {
         state = UiState::Idle;
      }
   } break;
   case KEY_COMMA: {
      if(selected_connection_id != 0) {
         --selected_connection_id;
      }
   } break;
   case KEY_PERIOD: {
      if(selected_connection_id < map.connections.size() - 1) {
         ++selected_connection_id;
      }
   } break;
   case KEY_A: {
      if((selected_connection_id < map.connections.size()) && has_lines) {
         map.lines[current_line_idx].connection_ids.push_back(
            selected_connection_id
         );
      }
   } break;
   }
}

static void draw_lines_hud() {
   auto x = window_width - 50;
   for(std::size_t i = 0; i < map.lines.size(); ++i) {
      auto rec = Rectangle{(float)x, (float)(10 + i * 30), 40, 20};
      DrawRectangleRec(rec, map.lines[i].color);
      if(i == current_line_idx) {
         DrawRectangleLinesEx(rec, 5, BLACK);
      }
   }
}

static void draw_hud() {
   char grid_size_text[256];
   std::snprintf(
      grid_size_text,
      sizeof(grid_size_text),
      "Grid size: %d",
      GRID_SIZES[current_grid_idx]
   );
   DrawText(grid_size_text, 10, 10, 20, HUD_COLOR);
   for(size_t i = 0; i < STATIC_HUD_TEXT.size(); ++i) {
      DrawText(STATIC_HUD_TEXT[i].c_str(), 10, 30 + 20 * i, 20, HUD_COLOR);
   }

   DrawText(popup_msg.c_str(), 10, window_height - 30, 20, HUD_COLOR);

   std::string state_str;
   switch(state) {
   case UiState::Idle:
      state_str = "idle";
      break;
   case UiState::ConnectionSelectEnd:
      state_str = "[c] select logical end station";
      break;
   case UiState::ConnectionDoPath:
      state_str = "[space] select path nodes, [enter] finish";
      break;
   }

   auto w = MeasureText(state_str.c_str(), 20);
   DrawText(
      state_str.c_str(),
      window_width - w - 10,
      window_height - 30,
      20,
      HUD_COLOR
   );

   draw_lines_hud();
}

static void draw_grid() {
   auto grid_size = GRID_SIZES[current_grid_idx];
   for(std::size_t i = 1; i <= window_width / grid_size; ++i) {
      DrawLine(i * grid_size, 0, i * grid_size, window_height, GRID_COLOR);
   }
   for(std::size_t i = 1; i <= window_height / grid_size; ++i) {
      DrawLine(0, i * grid_size, window_width, i * grid_size, GRID_COLOR);
   }
}

static float round_to(float n, int multiple) {
   return (int)((n + multiple / 2.0) / multiple) * multiple;
}

static Vector2 get_snap_location() {
   auto mouse = GetMousePosition();
   auto grid_size = GRID_SIZES[current_grid_idx];
   return Vector2{round_to(mouse.x, grid_size), round_to(mouse.y, grid_size)};
}

static std::optional<tube::StationId> get_mouseover_station_id() {
   constexpr float EPS = 0.1;
   auto snap = get_snap_location();

   std::optional<tube::NodeId> node_id;

   for(std::size_t i = 0; i < map.station_nodes.size(); ++i) {
      auto& node = map.station_nodes[i];
      if(std::abs(node.pos.x - snap.x) < EPS &&
         std::abs(node.pos.y - snap.y) < EPS) {
         node_id = i;
         break;
      }
   }

   if(node_id.has_value()) {
      for(std::size_t i = 0; i < map.stations.size(); ++i) {
         auto& nodes = map.stations[i].nodes;
         if(std::find(nodes.begin(), nodes.end(), node_id.value()) !=
            nodes.end()) {
            return static_cast<tube::StationId>(i);
         }
      }
   }

   return std::nullopt;
}

static void do_connection() {
   if(state == UiState::ConnectionSelectEnd) {
      auto end_station = get_mouseover_station_id();
      if(end_station.has_value()) {
         map.connections.back().logical_end_id = end_station.value();
         state = UiState::ConnectionDoPath;
      } else {
         popup_msg = "must end at station node";
      }

   } else if(state == UiState::Idle) {
      auto start_station = get_mouseover_station_id();
      if(start_station.has_value()) {
         map.connections.push_back(
            tube::Connection{start_station.value(), start_station.value(), {}}
         );
      } else {
         popup_msg = "must start at station node";
         return;
      }

      state = UiState::ConnectionSelectEnd;
   }
}

static void draw_connection(
   tube::Connection const& connection, tube::Map const& map,
   std::function<void(Vector2, Vector2)> draw
) {
   if(connection.point_ids.size() > 1) {
      for(std::size_t i = 0; i < connection.point_ids.size() - 1; ++i) {
         draw(
            map.points[connection.point_ids[i]].pos,
            map.points[connection.point_ids[i + 1]].pos
         );
      }
   }
}

static void draw_unclaimed_connections() {
   for(std::size_t conn_id = 0; conn_id < map.connections.size(); ++conn_id) {
      auto& connection = map.connections[conn_id];
      bool used = false;
      for(auto& line : map.lines) {
         if(std::find(
               line.connection_ids.begin(),
               line.connection_ids.end(),
               conn_id
            ) != line.connection_ids.end()) {
            used = true;
         }
      }
      if(!used) {
         draw_connection(connection, map, [](auto a, auto b) {
            DrawLineV(a, b, HUD_COLOR);
         });
      }
   }
}

static void draw_selected_connection() {
   if(selected_connection_id < map.connections.size()) {
      draw_connection(
         map.connections[selected_connection_id],
         map,
         [](auto a, auto b) { DrawLineEx(a, b, 10, SNAP_POS_COLOR); }
      );
   }
}