#pragma once
#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <optional>
#include <set>

namespace transport_catalogue {

namespace request_handler {

class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer, const router::TransportRouter& router);

    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    const std::set<std::string_view> GetBusesByStop(const std::string_view& stop_name) const;

    svg::Document RenderMap() const;

    std::optional<router::RouteItems> GetRouteByStops(const std::string_view& start_stop, const std::string_view& finish_stop) const;

private:
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
    const router::TransportRouter& router_;
};

}  // namespace request_handler
}  // namespace transport_catalogue