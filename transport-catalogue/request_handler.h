#pragma once
#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

#include <optional>
#include <set>

namespace transport_catalogue {

namespace request_handler {

class RequestHandler {
public:
    using StopPtr = const domain::Stop*;
    using BusPtr = const domain::Bus*;
    using BusStat = const domain::BusStat;

    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);

    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    const std::set<std::string_view> GetBusesByStop(const std::string_view& stop_name) const;

    svg::Document RenderMap() const;

private:
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};

} // namespace request_handler
} // namespace transport_catalogue