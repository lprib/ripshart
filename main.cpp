#include <cstdio>

#include <raylib.h>
#include <vector>

#include "line.hpp"

static Line gen_random_line() {
   std::vector<Station> stations{};
   for(int i = 0; i < 20; ++i) {
      stations.push_back(
         Station(GetRandomValue(10, 630), GetRandomValue(10, 470))
      );
   }

   return Line(stations, ORANGE);
}

int main(int argc, char** argv) {
   InitWindow(640, 480, "ripshart unshackled");
   SetTargetFPS(60);

   auto line =
      Line({Station(100, 100), Station(200, 100), Station(500, 200)}, BLUE);

   auto line2 = gen_random_line();

   while(!WindowShouldClose()) {
      if(IsKeyPressed(KEY_SPACE)) {
         line2 = gen_random_line();
      }

      BeginDrawing();
      {
         line.stations[2].pos = GetMousePosition();
         ClearBackground(WHITE);
         line2.draw();
         line.draw();
      }
      EndDrawing();
   }
}
