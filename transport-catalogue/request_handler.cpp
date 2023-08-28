#include "request_handler.h"

namespace transport_catalogue {

namespace request_handler {

using StopPtr = const domain::Stop*;
using BusPtr = const domain::Bus*;
using BusStat = const domain::BusStat;

RequestHandler::RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer, const router::TransportRouter& router)
    : db_(db)
    , renderer_(renderer)
    , router_(router)
    {
    }

std::optional<BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
    const auto found_bus_prt = db_.FindBus(bus_name);
    if (found_bus_prt != nullptr) {
        return std::make_optional(db_.GetBusStat(found_bus_prt));
    }
    return std::nullopt;
}

const std::set<std::string_view> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
    return db_.GetBusesByStop(stop_name);
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.RenderMap(db_.GetBusNameToBus());
}

std::optional<router::RouteItems> RequestHandler::GetRouteByStops(const std::string_view& start_stop, const std::string_view& finish_stop) const {
    return router_.GetRouteByStops(db_.FindStop(start_stop), db_.FindStop(finish_stop));
}

}  // namespace request_handler
}  // namespace transport_catalogue