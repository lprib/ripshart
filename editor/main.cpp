#include <array>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <raylib.h>
#include <vector>

#include "tube_file.hpp"
#include "tube_map.hpp"
#include "tube_render.hpp"

static void handle_key(int keycode);
static void draw_hud();
static void draw_grid();
static Vector2 get_snap_location();
static void start_connection();

enum class UiState {
   Idle,
   AddingConnection,
};

const std::vector<std::string> STATIC_HUD_TEXT = {
   "[-] grid smaller",
   "[=] grid larger",
   "[p] print dump",
   "[s] place station",
   "[l] add line",
   "[]] next line",
   "[[] prev line",
   "[c] add/finish connection"
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
      start_connection();
   } break;
   }
}

static void draw_lines_hud() {
   auto x = window_width - 50;
   for(std::size_t i = 0; i < map.lines.size(); ++i) {
      auto rec = Rectangle{x, 10 + i * 30, 40, 20};
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
   case UiState::AddingConnection:
      state_str = "adding connection";
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

static void start_connection() {
   constexpr float EPS = 0.1;

   auto snap = get_snap_location();

   if(state == UiState::AddingConnection) {
      state = UiState::Idle;
   } else if(state == UiState::Idle) {
      int node_idx = -1;
      for(std::size_t i = 0; i < map.station_nodes.size(); ++i) {
         auto& node = map.station_nodes[i];
         if(std::abs(node.pos.x - snap.x) < EPS &&
            std::abs(node.pos.y - snap.y) < EPS) {
            node_idx = i;
            break;
         }
      }

      if(node_idx == -1) {
         popup_msg = "must start at station node";
         return;
      }

      state = UiState::AddingConnection;
   }
}