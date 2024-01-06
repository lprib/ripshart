#include <cstdio>

#include <raylib.h>
#include <vector>
#include <fstream>

#include "line.hpp"
#include "mapfile.hpp"

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
   exit(0);

   InitWindow(640, 480, "ripshart unshackled");
   SetTargetFPS(60);

   auto line = gen_random_line();


   while(!WindowShouldClose()) {
      if(IsKeyPressed(KEY_SPACE)) {
         line = gen_random_line();
      }

      BeginDrawing();
      {
         line.stations.back().pos = GetMousePosition();
         ClearBackground(WHITE);
         line.draw();
      }
      EndDrawing();
   }
}
