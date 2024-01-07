#include <cstdio>

#include <raylib.h>
#include <vector>
#include <fstream>

#include "line.hpp"
#include "map_file.hpp"
#include "map_render.hpp"

static Line gen_random_line() {
   std::vector<Station> stations{};
   for(int i = 0; i < 4; ++i) {
      stations.push_back(
         Station(GetRandomValue(10, 630), GetRandomValue(10, 470))
      );
   }

   return Line(stations, RED);
}

int main(int argc, char** argv) {
   std::ifstream mapfile("/home/liam/programming/raylib_game/ripshart/tube.map");
   auto m = map::load(mapfile);
   map::save(m, std::cout);

   InitWindow(640, 480, "ripshart unshackled");
   SetTargetFPS(60);



   while(!WindowShouldClose()) {
      BeginDrawing();
      {
         map::render(m);
      }
      EndDrawing();
   }
}
