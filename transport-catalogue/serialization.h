#pragma once
#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include "transport_catalogue.pb.h"

#include <string>

namespace transport_catalogue {

namespace serialization {

class Serializer {
public:
    Serializer(TransportCatalogue& db, renderer::MapRenderer& renderer, router::TransportRouter& router);

    void SetFileName(std::string file_name);

    void SerializeDataBase();

    void DeserializeDataBase();

private:
    TransportCatalogue& db_;
    renderer::MapRenderer& renderer_;
    router::TransportRouter& router_;
    std::string file_name_;

    database::Stop CreateStopSerialize(StopPtr stop_ptr);

    database::Bus CreateBusSerialize(BusPtr bus_ptr);

    database::DistanceBetweenStops CreateDistanceBetweenStopsSerialize(StopPtr stop_ptr_first, StopPtr stop_ptr_second, double distance);

    domain::Stop CreateStopDeserialize(const database::Stop& stop_ptr_serial);

    domain::Bus CreateBusDeserialize(const database::TransportCatalogue& catalogue_serial, const database::Bus& bus_ptr_serial);

    database::Color CreateColorSerialize(const svg::Color& color);

    database::RenderSettings CreateRenderSettingsSerialize(const renderer::RenderSettings& settings);

    svg::Color CreateColorDeserialize(const database::Color& color_serial);

    renderer::RenderSettings CreateRenderSettingsDeserialize(const database::RenderSettings& settings_serial);

    database::RouterSettings CreateRouterSettingsSerialize(const router::RouterSettings& settings);

    router::RouterSettings CreateRouterSettingsDeserialize(const database::RouterSettings& settings_serial);

};

}  // namespace serialization
}  // namespace transport_catalogue