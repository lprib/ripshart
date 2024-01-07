#include "mapfile.hpp"

#include <cstdio>
#include <sstream>
#include <string>

namespace map {

std::istream& operator>>(std::istream& is, StationStyle& style) {
   std::string style_str;
   is >> style_str;
   if(style_str == "circle") {
      style = StationStyle::Circle;
   } else if(style_str == "tick") {
      style = StationStyle::Tick;
   } else {
      std::cerr << "Bad style string: " << style_str << std::endl;
      style = StationStyle::Circle;
   }
   return is;
}

std::istream& operator>>(std::istream& is, Station& station) {
   is >> station.style;
   std::vector<NodeId> nodes{};
   NodeId id;
   while(is >> id) {
      station.nodes.push_back(id);
   }
   return is;
}

std::istream& operator>>(std::istream& is, StationNode& station_node) {
   is >> station_node.pos.x >> station_node.pos.y;
   return is;
}

std::istream& operator>>(std::istream& is, Line& line) {
   int byte;
   is >> byte;
   line.color.r = (unsigned char)byte;
   is >> byte;
   line.color.g = (unsigned char)byte;
   is >> byte;
   line.color.b = (unsigned char)byte;
   line.color.a = 0xff;
   ConnectionId id;
   while(is >> id) {
      line.connection_ids.push_back(id);
   }
   return is;
}

std::istream& operator>>(std::istream& is, Connection& conn) {
   is >> conn.start >> conn.end;
   CornerId id;
   while(is >> id) {
      conn.corners.push_back(id);
   }
   return is;
}

std::istream& operator>>(std::istream& is, Corner& corner) {
   is >> corner.pos.x >> corner.pos.y;
   return is;
}

void check_length(std::string& tag, std::size_t vector_len, Id index) {
   if(index != vector_len) {
      std::cerr << "out of order index: " << index << " " << tag << std::endl;
   }
}

template <typename T>
void append_to_vec(
   std::vector<T>& vec, std::istream& is, std::string tag, Id index
) {
   check_length(tag, vec.size(), index);
   T item;
   is >> item;
   vec.push_back(item);
}

Map load(std::istream& in) {
   Map map{};

   std::string line;
   while(std::getline(in, line)) {
      std::istringstream ss(line);

      std::string entry_tag;
      std::getline(ss, entry_tag, '.');

      Id index;
      ss >> index;

      if(entry_tag == "station") {
         append_to_vec<Station>(map.stations, ss, entry_tag, index);
      } else if(entry_tag == "station_node") {
         append_to_vec<StationNode>(map.station_nodes, ss, entry_tag, index);
      } else if(entry_tag == "line") {
         append_to_vec<Line>(map.lines, ss, entry_tag, index);
      } else if(entry_tag == "connection") {
         append_to_vec<Connection>(map.connections, ss, entry_tag, index);
      } else if(entry_tag == "corner") {
         append_to_vec<Corner>(map.corners, ss, entry_tag, index);
      } else {
         std::cerr << "unknown tag " << entry_tag << std::endl;
      }
   }

   return map;
}

void save(Map const& map, std::ostream& out) {
   for(std::size_t i = 0; i < map.stations.size(); ++i) {
      out << "station." << i << " ";
      switch(map.stations[i].style) {
      case StationStyle::Circle:
         out << "circle";
         break;
      case StationStyle::Tick:
         out << "tick";
         break;
      }
      for(auto node_id : map.stations[i].nodes) {
         out << " " << node_id;
      }
      out << std::endl;
   }

   for(std::size_t i = 0; i < map.station_nodes.size(); ++i) {
      auto& n = map.station_nodes.at(i);
      out << "station_node." << i << " " << n.pos.x << " " << n.pos.y
          << std::endl;
   }

   for(std::size_t i = 0; i < map.lines.size(); ++i) {
      auto& l = map.lines.at(i);
      out << "line." << i << " " << (int)l.color.r << " " << (int)l.color.g
          << " " << (int)l.color.b;
      for(auto conn_id : l.connection_ids) {
         out << " " << conn_id;
      }
      out << std::endl;
   }

   for(std::size_t i = 0; i < map.connections.size(); ++i) {
      auto& c = map.connections.at(i);
      out << "connection." << i << " " << c.start << " " << c.end;
      for(auto corner : c.corners) {
         out << " " << corner;
      }
      out << std::endl;
   }

   for(std::size_t i = 0; i < map.corners.size(); ++i) {
      auto& c = map.corners.at(i);
      out << "corner." << i << " " << c.pos.x << " " << c.pos.y << std::endl;
   }
}

} // namespace map
