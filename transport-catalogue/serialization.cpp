#include "serialization.h"

#include <fstream>
#include <variant>

namespace transport_catalogue {

namespace serialization {

Serializer::Serializer(TransportCatalogue& db, renderer::MapRenderer& renderer, router::TransportRouter& router)
    : db_(db)
    , renderer_(renderer)
    , router_(router)
{
}

void Serializer::SetFileName(std::string file_name) {
    file_name_ = std::move(file_name);
}

void Serializer::SerializeDataBase() {
    std::ofstream output(file_name_, std::ios::binary);
    database::TransportCatalogue catalogue_serial;
    for (const auto& [name, stop_ptr] : db_.GetStopNameToStop()) {
        (*catalogue_serial.mutable_stops())[reinterpret_cast<uint64_t>(stop_ptr)] = CreateStopSerialize(stop_ptr);
    }
    for (const auto& [name, bus_ptr] : db_.GetBusNameToBus()) {
        (*catalogue_serial.mutable_buses())[reinterpret_cast<uint64_t>(bus_ptr)] = CreateBusSerialize(bus_ptr);
    }
    for (const auto& [stops_ptr, distance] : db_.GetDistancesBetweenStops()) {
        *catalogue_serial.add_distance_between_stops() = CreateDistanceBetweenStopsSerialize(stops_ptr.first, stops_ptr.second, distance);
    }
    *catalogue_serial.mutable_render_settings() = CreateRenderSettingsSerialize(renderer_.GetRenderSettings());
    *catalogue_serial.mutable_router_settings() = CreateRouterSettingsSerialize(router_.GetRoutingSettings());
    catalogue_serial.SerializePartialToOstream(&output);
}

void Serializer::DeserializeDataBase() {
    std::ifstream input(file_name_, std::ios::binary);
    database::TransportCatalogue catalogue_serial;
    if (catalogue_serial.ParseFromIstream(&input)) {
        for (const auto& [stop_serial, stop_ptr_serial] : catalogue_serial.stops()) {
            db_.AddStop(CreateStopDeserialize(stop_ptr_serial));
        }
        for (const auto& [bus_serial, bus_ptr_serial] : catalogue_serial.buses()) {
            db_.AddBus(CreateBusDeserialize(catalogue_serial, bus_ptr_serial));
        }
        for (const auto& entry : catalogue_serial.distance_between_stops()) {
            db_.SetDistanceBetweenStops((*catalogue_serial.mutable_stops()).at(entry.stop_first()).name(),
                                        (*catalogue_serial.mutable_stops()).at(entry.stop_second()).name(),
                                        entry.distance());
        }
        renderer_.SetRenderSettings(CreateRenderSettingsDeserialize(catalogue_serial.render_settings()));
        router_.SetRoutingSettings(CreateRouterSettingsDeserialize(catalogue_serial.router_settings()));
    }
}

database::Stop Serializer::CreateStopSerialize(StopPtr stop_ptr) {
    database::Stop stop_serial;
    stop_serial.set_name(stop_ptr->name);
    stop_serial.mutable_coordinates()->set_lat(stop_ptr->coordinates.lat);
    stop_serial.mutable_coordinates()->set_lng(stop_ptr->coordinates.lng);
    return stop_serial;
}

database::Bus Serializer::CreateBusSerialize(BusPtr bus_ptr) {
    database::Bus bus_serial;
    bus_serial.set_name(bus_ptr->name);
    bus_serial.set_is_roundtrip(bus_ptr->is_roundtrip);
    for (StopPtr stop : bus_ptr->stops) {
        bus_serial.add_stops(reinterpret_cast<uint64_t>(stop));
    }
    return bus_serial;
}

database::DistanceBetweenStops Serializer::CreateDistanceBetweenStopsSerialize(StopPtr stop_ptr_first, StopPtr stop_ptr_second, double distance) {
    database::DistanceBetweenStops distance_between_stops_serial;
    distance_between_stops_serial.set_stop_first(reinterpret_cast<uint64_t>(stop_ptr_first));
    distance_between_stops_serial.set_stop_second(reinterpret_cast<uint64_t>(stop_ptr_second));
    distance_between_stops_serial.set_distance(distance);
    return distance_between_stops_serial;
}

domain::Stop Serializer::CreateStopDeserialize(const database::Stop& stop_ptr_serial) {
    return {stop_ptr_serial.name(), {stop_ptr_serial.coordinates().lat(), stop_ptr_serial.coordinates().lng()}};
}

domain::Bus Serializer::CreateBusDeserialize(const database::TransportCatalogue& catalogue_serial, const database::Bus& bus_ptr_serial) {
    domain::Bus bus;
    bus.name = bus_ptr_serial.name();
    for (uint64_t stop_serial : bus_ptr_serial.stops()) {
        bus.stops.emplace_back(db_.FindStop((*(const_cast<database::TransportCatalogue&>(catalogue_serial)).mutable_stops()).at(stop_serial).name()));
    }
    bus.is_roundtrip = bus_ptr_serial.is_roundtrip();
    return bus;
}

database::Color Serializer::CreateColorSerialize(const svg::Color& color) {
    database::Color color_serial;
    if (std::holds_alternative<std::string>(color)) {
        color_serial.set_color(std::get<std::string>(color));
    } else if (std::holds_alternative<svg::Rgb>(color)) {
        svg::Rgb rgb(std::get<svg::Rgb>(color));
        color_serial.mutable_rgb()->set_red(rgb.red);
        color_serial.mutable_rgb()->set_green(rgb.green);
        color_serial.mutable_rgb()->set_blue(rgb.blue);
    } else if (std::holds_alternative<svg::Rgba>(color)) {
        svg::Rgba rgba(std::get<svg::Rgba>(color));
        color_serial.mutable_rgba()->set_red(rgba.red);
        color_serial.mutable_rgba()->set_green(rgba.green);
        color_serial.mutable_rgba()->set_blue(rgba.blue);
        color_serial.mutable_rgba()->set_opacity(rgba.opacity);
    }
    return color_serial;
}

database::RenderSettings Serializer::CreateRenderSettingsSerialize(const renderer::RenderSettings& settings) {
    database::RenderSettings settings_serial;
    settings_serial.set_width(settings.width);
    settings_serial.set_height(settings.height);
    settings_serial.set_padding(settings.padding);
    settings_serial.set_line_width(settings.line_width);
    settings_serial.set_stop_radius(settings.stop_radius);
    settings_serial.set_bus_label_font_size(settings.bus_label_font_size);
    settings_serial.mutable_bus_label_offset()->set_x(settings.bus_label_offset.x);
    settings_serial.mutable_bus_label_offset()->set_y(settings.bus_label_offset.y);
    settings_serial.set_stop_label_font_size(settings.stop_label_font_size);
    settings_serial.mutable_stop_label_offset()->set_x(settings.stop_label_offset.x);
    settings_serial.mutable_stop_label_offset()->set_y(settings.stop_label_offset.y);
    *settings_serial.mutable_underlayer_color() = CreateColorSerialize(settings.underlayer_color);
    settings_serial.set_underlayer_width(settings.underlayer_width);
    for (const svg::Color& color : settings.color_palette) {
        *settings_serial.add_color_palette() = CreateColorSerialize(color);
    }
    return settings_serial;
}

svg::Color Serializer::CreateColorDeserialize(const database::Color& color_serial) {
    if (!color_serial.color().empty()) {
        std::string color(color_serial.color());
        return color;
    } else if (color_serial.has_rgb()) {
        svg::Rgb rgb(color_serial.rgb().red(), color_serial.rgb().green(), color_serial.rgb().blue());
        return rgb;
    } else if (color_serial.has_rgba()) {
        svg::Rgba rgba(color_serial.rgba().red(), color_serial.rgba().green(), color_serial.rgba().blue(), color_serial.rgba().opacity());
        return rgba;
    }
    return {};
}

renderer::RenderSettings Serializer::CreateRenderSettingsDeserialize(const database::RenderSettings& settings_serial) {
    renderer::RenderSettings settings;
    settings.width = settings_serial.width();
    settings.height = settings_serial.height();
    settings.padding = settings_serial.padding();
    settings.line_width = settings_serial.line_width();
    settings.stop_radius = settings_serial.stop_radius();
    settings.bus_label_font_size = settings_serial.bus_label_font_size();
    settings.bus_label_offset = {settings_serial.bus_label_offset().x(), settings_serial.bus_label_offset().y()};
    settings.stop_label_font_size = settings_serial.stop_label_font_size();
    settings.stop_label_offset = {settings_serial.stop_label_offset().x(), settings_serial.stop_label_offset().y()};
    settings.underlayer_color = CreateColorDeserialize(settings_serial.underlayer_color());
    settings.underlayer_width = settings_serial.underlayer_width();
    for (const database::Color& color_serial : settings_serial.color_palette()) {
        settings.color_palette.emplace_back(CreateColorDeserialize(color_serial));
    }
    return settings;
}

database::RouterSettings Serializer::CreateRouterSettingsSerialize(const router::RouterSettings& settings) {
    database::RouterSettings settings_serial;
    settings_serial.set_bus_wait_time(settings.bus_wait_time);
    settings_serial.set_bus_velocity(settings.bus_velocity);
    return settings_serial;
}

router::RouterSettings Serializer::CreateRouterSettingsDeserialize(const database::RouterSettings& settings_serial) {
    return {settings_serial.bus_wait_time(), settings_serial.bus_velocity()};
}

}  // namespace serialization
}  // namespace transport_catalogue