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
   StationStyle style;
   is >> style;
   std::vector<NodeId> nodes{};
   NodeId id;
   while(is >> id) {
      nodes.push_back(id);
   }

   station.style = style;
   station.nodes = nodes;

   return is;
}

void check_length(std::string& tag, std::size_t vector_len, Id index) {
   if(index != vector_len) {
      std::cerr << "out of order index: " << index << " " << tag << std::endl;
   }
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
         check_length(entry_tag, map.stations.size(), index);
         Station station;
         ss >> station;
         map.stations.push_back(station);
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
}

} // namespace map
