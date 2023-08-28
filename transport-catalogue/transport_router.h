#pragma once
#include "transport_catalogue.h"
#include "router.h"

#include <map>
#include <memory>
#include <string_view>
#include <vector>

namespace transport_catalogue {

namespace router {

struct RouterSettings {
    double bus_wait_time = 0.;
    double bus_velocity = 0.;
};

enum class ItemType {
    BUS,
    WAIT
};

struct Item {
    ItemType type;
    std::string_view name;
    double time = 0.;
    int span_count = 0;
};

struct RouteItems {
    double total_time = 0.;
    std::vector<Item> items;
};

class TransportRouter {
public:
    TransportRouter(const TransportCatalogue& catalogue);

    void SetRoutingSettings(const RouterSettings& settings);

    void BuildRoutes();

    std::optional<RouteItems> GetRouteByStops(StopPtr start_stop, StopPtr finish_stop) const;

private:
    const TransportCatalogue& catalogue_;
    RouterSettings settings_;
    std::unique_ptr<graph::DirectedWeightedGraph<double>> graph_;
    std::unique_ptr<graph::Router<double>> router_;
    std::map<StopPtr, std::pair<int, int>> stop_vertex_pair_;
    std::map<graph::EdgeId, Item> edge_item_;

    void SetVertexAndEdge();

    graph::VertexId GetWaitVertex(StopPtr stop_ptr) const;

    graph::VertexId GetBusVertex(StopPtr stop_ptr) const;

    void AddEdgeToItem(graph::VertexId start_vertex, graph::VertexId stop_vertex, Item item);

    void AddBusEdge(StopPtr start_stop, StopPtr finish_stop, const std::string_view& bus_name, int span, double distance);

    void AddRouteToGraph(const std::string_view& bus_name, BusPtr bus_ptr);
};

}  // namespace router
}  // namespace transport_catalogue