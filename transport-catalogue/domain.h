#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <variant>


#include "transport_catalogue.h"
#include "graph.h"

namespace transport_catalogue {



struct StopEdge {
    std::string_view name;
    double time = 0;
};

struct BusEdge {
    std::string_view bus_name;
    size_t span_count = 0;
    double time = 0;
};

struct RouterByStop {
    graph::VertexId bus_wait_start;
    graph::VertexId bus_wait_end;
};

struct RouteInfo {
    double total_time = 0.;
    std::vector<std::variant<StopEdge, BusEdge>> edges;
};
namespace detail {
namespace bus {




}//end namespace bus

namespace stop {





}//end namespace stop



}//end namespace detail
}//end namespace transport_catalogue

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
