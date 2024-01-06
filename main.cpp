#include <cstdio>

#include <raylib.h>

#include "line.hpp"

int main(int argc, char** argv) {
   InitWindow(640, 480, "ripshart unshackled");
   SetTargetFPS(60);

   auto line =
      Line({Station(100, 100), Station(200, 100), Station(500, 200)}, BLUE);

   while(!WindowShouldClose()) {
      BeginDrawing();
      {
         line.stations[2].pos = GetMousePosition();
         ClearBackground(WHITE);
         line.draw();
      }
      EndDrawing();
   }
}
