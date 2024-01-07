#ifndef MAPFILE_H
#define MAPFILE_H

#include <iostream>
#include <vector>

#include <raylib.h>

namespace map {

using Id = unsigned int;

using NodeId = Id;
using ConnectionId = Id;
using StationNodeId = Id;
using CornerId = Id;

enum class StationStyle {
   Circle,
   Tick,
};

struct Station {
   StationStyle style;
   std::vector<NodeId> nodes;
};

struct StationNode {
   Vector2 pos;
};

struct Line {
   Color color;
   std::vector<ConnectionId> connection_ids;
};

struct Connection {
   StationNodeId start;
   StationNodeId end;
   std::vector<CornerId> corners;
};

struct Corner {
   Vector2 pos;
   // radius?
};

struct Map {
   std::vector<Station> stations;
   std::vector<StationNode> station_nodes;
   std::vector<Line> lines;
   std::vector<Connection> connections;
   std::vector<Corner> corners;
};

Map load(std::istream& in);
void save(Map const& map, std::ostream& out);

} // namespace map

#endif