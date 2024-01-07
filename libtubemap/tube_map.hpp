#ifndef TUBE_MAP_HPP
#define TUBE_MAP_HPP

#include <raylib.h>
#include <vector>

namespace tube {

using Id = unsigned int;

using NodeId = Id;
using StationId = Id;
using ConnectionId = Id;
using TrackPointId = Id;

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
   StationId logical_start_id;
   StationId logical_end_id;
   std::vector<TrackPointId> point_ids;
};

struct TrackPoint {
   Vector2 pos;
   // radius?
};

struct Map {
   std::vector<Station> stations;
   std::vector<StationNode> station_nodes;
   std::vector<Line> lines;
   std::vector<Connection> connections;
   std::vector<TrackPoint> points;
};

} // namespace tube

#endif