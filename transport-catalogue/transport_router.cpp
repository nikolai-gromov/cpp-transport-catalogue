#include "transport_router.h"

namespace transport_catalogue {

namespace router {

TransportRouter::TransportRouter(const TransportCatalogue& catalogue)
    : catalogue_(catalogue)
{
}

void TransportRouter::SetRoutingSettings(const RouterSettings& settings) {
    settings_ = {settings};
}

const RouterSettings& TransportRouter::GetRoutingSettings() {
    return settings_;
}

void TransportRouter::BuildRoutes() {
    graph_ = std::make_unique<graph::DirectedWeightedGraph<double>>(catalogue_.GetStopsCount() * 2);
    SetVertexAndEdge();
    for(const auto [bus_name, bus_ptr] : catalogue_.GetBusNameToBus()) {
        AddRouteToGraph(bus_name, bus_ptr);
    }
    router_ = std::make_unique<graph::Router<double>>(*graph_);
}

std::optional<RouteItems> TransportRouter::GetRouteByStops(StopPtr start_stop_ptr, StopPtr finish_stop_ptr) const {
    RouteItems items;
    std::optional<graph::Router<double>::RouteInfo> router_info = router_->BuildRoute(GetWaitVertex(start_stop_ptr), GetWaitVertex(finish_stop_ptr));
    if (!router_info) {
        return {};
    }
    items.total_time = router_info.value().weight;
    for (const auto& edge : router_info.value().edges) {
        items.items.push_back(edge_item_.at(edge));
    }
    return items;
}

void TransportRouter::SetVertexAndEdge() {
    graph::VertexId vertex = 0;
    for (const auto& [stop_name, stop_ptr] : catalogue_.GetStopNameToStop()) {
        stop_vertex_pair_[stop_ptr] = {vertex, vertex + 1};
        edge_item_[graph_->AddEdge({ vertex, vertex + 1, settings_.bus_wait_time })] = { ItemType::WAIT, stop_name, settings_.bus_wait_time, 1 };
        vertex += 2;
    }
}

graph::VertexId TransportRouter::GetWaitVertex(StopPtr stop_ptr) const {
    return stop_vertex_pair_.at(stop_ptr).first;
}

graph::VertexId TransportRouter::GetBusVertex(StopPtr stop_ptr) const {
    return stop_vertex_pair_.at(stop_ptr).second;
}

void TransportRouter::AddEdgeToItem(graph::VertexId start_vertex, graph::VertexId stop_vertex, Item item) {
    edge_item_[graph_->AddEdge({ start_vertex, stop_vertex, item.time })] = item;
}

void TransportRouter::AddBusEdge(StopPtr start_stop_ptr, StopPtr finish_stop_ptr, const std::string_view& bus_name, int span, double distance) {
    Item item;
    item.type = ItemType::BUS;
    item.name = bus_name;
    item.time = distance / (settings_.bus_velocity *  1000 / 60);
    item.span_count = span;
    AddEdgeToItem(GetBusVertex(start_stop_ptr), GetWaitVertex(finish_stop_ptr), item);
}

void TransportRouter::AddRouteToGraph(const std::string_view& bus_name, BusPtr bus_ptr) {
    for (int i = 0; i < static_cast<int>(bus_ptr->stops.size()) - 1; ++i) {
        double forward_distance = 0;
        double backward_distance = 0;
        for (int j = i; j < static_cast<int>(bus_ptr->stops.size()) - 1; ++j) {
            forward_distance += catalogue_.GetDistanceBetweenStops(bus_ptr->stops[j], bus_ptr->stops[j + 1]);
            AddBusEdge(bus_ptr->stops[i], bus_ptr->stops[j + 1], bus_name, j - i + 1, forward_distance);
            if (!bus_ptr->is_roundtrip){
                backward_distance += catalogue_.GetDistanceBetweenStops(bus_ptr->stops[j + 1], bus_ptr->stops[j]);
                AddBusEdge(bus_ptr->stops[j + 1], bus_ptr->stops[i], bus_name, j - i + 1, backward_distance);
            }
        }
    }
}

}  // namespace router
}  // namespace transport_catalogue