#include <cstdio>

#include <raylib.h>
#include <vector>
#include <fstream>

#include "tube_map.hpp"
#include "tube_file.hpp"
#include "tube_render.hpp"

int main(int argc, char** argv) {
   std::ifstream mapfile("/home/liam/programming/raylib_game/ripshart/tube.map");
   auto m = tube::load(mapfile);
   tube::save(m, std::cout);

   InitWindow(640, 480, "ripshart unshackled");
   SetTargetFPS(60);

   while(!WindowShouldClose()) {
      BeginDrawing();
      {
         ClearBackground(WHITE);
         tube::render(m);
      }
      EndDrawing();
   }
}
