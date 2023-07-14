#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    RenderObject(context);

    context.out << std::endl;
}

void PrintColor::operator()(std::monostate) const {
    out << NoneColor;
}

void PrintColor::operator()(const std::string& color) const {
    out << color;
}

void PrintColor::operator()(Rgb rgb) const {
    out << "rgb("s
       << std::to_string(rgb.red) << ","s
       << std::to_string(rgb.green) << ","
       << std::to_string(rgb.blue) << ")"s;
}

void PrintColor::operator()(Rgba rgba) const {
    out << "rgba("sv
       << std::to_string(rgba.red) << ","sv
       << std::to_string(rgba.green) << ","sv
       << std::to_string(rgba.blue) << ","sv;
    out << rgba.opacity << ")"sv;
}

std::ostream& operator<<(std::ostream& out, const Color& color) {
    std::visit(PrintColor{out}, color);
    return out;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}

// ---------- Polyline ------------------

Polyline& Polyline::AddPoint(Point point) {
    points_.emplace_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    bool first = true;
    for (const auto& point : points_) {
        if (!first) {
            out << " "sv;
        }
        out << point.x << "," << point.y;
        first = false;
    }
    out << "\" "sv;
	RenderAttrs(out);
	out << "/>"sv;
}

// ---------- Text ------------------

Text& Text::SetPosition(Point pos) {
    position_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}

Text& Text::SetData(std::string data) {
    std::string str = ""s;
    for (char c : data) {
        switch (c)
        {
        case '\"':
            str += "&quot;"s;
            break;
        case '\'':
            str += "&apos;"s;
            break;
        case '<':
            str += "&lt;"s;
            break;
        case '>':
            str += "&gt;"s;
            break;
        case '&':
            str += "&amp;"s;
            break;
        default:
            str += c;
            break;
        }
    }
    data_ = std::move(str);
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
	out << "<text ";
	RenderAttrs(out);
	out << " x=\"" << position_.x << "\"";
	out << " y=\"" << position_.y << "\"";
	out << " dx=\"" << offset_.x << "\"";
	out << " dy=\"" << offset_.y << "\"";
	out << " font-size=\"" << font_size_ << "\"";
	if (font_family_ != "") {
		out << " font-family=\"" << font_family_ << "\"";
	}
	if (font_weight_ != "") {
		out << " font-weight=\"" << font_weight_ << "\"";
	}
	out << ">" << data_ << "</text>";
}

void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(std::move(obj));
}

void Document::Render(std::ostream& out) const {
    RenderContext ctx(out, 2, 2);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"s;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"s;
    for (const std::unique_ptr<Object>& obj : objects_) {
        obj->Render(ctx);
    }
    out << "</svg>"s << std::endl;
}

}  // namespace svg