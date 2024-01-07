#include <cstdio>

#include <array>
#include <fstream>
#include <raylib.h>
#include <vector>

#include "tube_file.hpp"
#include "tube_map.hpp"
#include "tube_render.hpp"

static void handle_keys();
static void draw_hud();
static void draw_grid();
static Vector2 get_snap_location();

enum class UiState {
   Idle,
   AddingConnection,
};

static constexpr int WINDOW_WIDTH = 640;
static constexpr int WINDOW_HEIGHT = 480;

const Color GRID_COLOR = GetColor(0x00000030);
const Color HUD_COLOR = GetColor(0x808080ff);
const Color SNAP_POS_COLOR = GetColor(0x45a2ff80);

static constexpr std::array<int, 5> GRID_SIZES = {10, 25, 50, 100, 200};

// UI state:
static std::size_t current_grid_idx = 3;

static bool has_lines = false;
static std::size_t current_line_idx = 0;
static UiState state = UiState::Idle;
static std::string popup_msg = "popup";

tube::Map map{};

int main(int argc, char** argv) {
   std::ifstream mapfile("/home/liam/programming/raylib_game/ripshart/tube.map"
   );
   // map = tube::load(mapfile);

   InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ripshart unshackled");
   SetTargetFPS(60);

   while(!WindowShouldClose()) {
      handle_keys();

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

static void handle_keys() {
   if(IsKeyPressed(KEY_MINUS)) {
      if(current_grid_idx != 0) {
         --current_grid_idx;
      }
   }
   if(IsKeyPressed(KEY_EQUAL)) {
      if(current_grid_idx < GRID_SIZES.size() - 1) {
         ++current_grid_idx;
      }
   }
   if(IsKeyPressed(KEY_P)) {
      tube::save(map, std::cout);
   }
   if(IsKeyPressed(KEY_S)) {
      auto node_id = map.station_nodes.size();
      map.station_nodes.push_back(tube::StationNode{get_snap_location()});
      map.stations.push_back(tube::Station{
         tube::StationStyle::Circle,
         {static_cast<tube::NodeId>(node_id)}
      });
   }
   if(IsKeyPressed(KEY_L)) {
      current_line_idx = map.lines.size();
      auto color = GetColor((GetRandomValue(0, 0xffffff) << 8) | 0xff);
      map.lines.push_back(tube::Line{color, {}});
      has_lines = true;
   }
   if(IsKeyPressed(KEY_LEFT_BRACKET)) {
      if(current_line_idx != 0) {
         --current_line_idx;
      }
   }
   if(IsKeyPressed(KEY_RIGHT_BRACKET)) {
      if(current_line_idx < map.lines.size() - 1) {
         ++current_line_idx;
      }
   }
}

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

static void draw_lines_hud() {
   auto x = WINDOW_WIDTH - 50;
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

   DrawText(popup_msg.c_str(), 10, WINDOW_HEIGHT - 30, 20, HUD_COLOR);

   draw_lines_hud();
}

static void draw_grid() {
   auto grid_size = GRID_SIZES[current_grid_idx];
   for(std::size_t i = 1; i <= WINDOW_WIDTH / grid_size; ++i) {
      DrawLine(i * grid_size, 0, i * grid_size, WINDOW_HEIGHT, GRID_COLOR);
   }
   for(std::size_t i = 1; i <= WINDOW_HEIGHT / grid_size; ++i) {
      DrawLine(0, i * grid_size, WINDOW_WIDTH, i * grid_size, GRID_COLOR);
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