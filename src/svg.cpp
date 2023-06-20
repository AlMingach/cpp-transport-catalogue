#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = std::move(center);
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = std::move(radius);
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\"";
        RenderAttrs(out);
        out << " />"sv;
    }

    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point) {
        peaks_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        bool is_first = true;
        auto& out = context.out;
        out << "<polyline points=\""sv;
        for (const auto& peak : peaks_) {
            if (!is_first) {
                out << ' ';
            }
            out << peak.x << ',' << peak.y;
            is_first = false;
        }
        out << "\""sv;
        RenderAttrs(out);
        out << " />"sv;
    }

    // ---------- Text ------------------

    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& Text::SetPosition(Point pos) {
        pos_ = std::move(pos);
        return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset(Point offset) {
        offset_ = std::move(offset);
        return *this;
    }

    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size) {
        size_ = std::move(size);
        return *this;
    }

    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data) {
        data_ = std::move(data);
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text"sv;
        RenderAttrs(out);
        out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" dx=\""sv << offset_.x << "\" "sv;
        out << "dy=\""sv << offset_.y << "\" font-size=\""sv << size_ << "\""sv;
        if (!font_family_.empty()) {
            out << " "sv << "font-family=\""sv << font_family_ << "\""sv;;
        }
        if (!font_weight_.empty()) {
            out << " font-weight=\""sv << font_weight_ << "\"";
        }
        out << '>';
        for (const char& c : data_) {
            switch (c)
            {
            case ('"'):
                out << "&quot;"sv;
                break;
            case('\''):
                out << "&apos;"sv;
                break;
            case('<'):
                out << "&lt;"sv;
                break;
            case('>'):
                out << "&gt;"sv;
                break;
            case('&'):
                out << "&amp;"sv;
                break;
            default:
                out << c;
            }
        }
        out << "</text>"sv;
    }

    // ---------- Document ------------------

    // Добавляет в svg-документ объект-наследник svg::Object
    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    }

    // Выводит в ostream svg-представление документа
    void Document::Render(std::ostream& out) const {

        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for (const auto& object : objects_) {
            out << "  ";
            object.get()->Render(out);
        }
        out << "</svg>"sv;  
    }

    //---------------------- Out -----------------------------

    std::ostream& operator<<(std::ostream& out, const svg::StrokeLineCap line_cap) {
        using namespace std::literals;
        switch (line_cap) {
        case (svg::StrokeLineCap::BUTT):
            out << "butt"sv;
            break;
        case(svg::StrokeLineCap::ROUND):
            out << "round"sv;
            break;
        case (svg::StrokeLineCap::SQUARE):
            out << "square"sv;
            break;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const svg::StrokeLineJoin line_join) {
        using namespace std::literals;
        switch (line_join) {
        case (svg::StrokeLineJoin::ARCS):
            out << "arcs"sv;
            break;
        case(svg::StrokeLineJoin::BEVEL):
            out << "bevel"sv;
            break;
        case (svg::StrokeLineJoin::MITER):
            out << "miter"sv;
            break;
        case (svg::StrokeLineJoin::MITER_CLIP):
            out << "miter-clip"sv;
            break;
        case (svg::StrokeLineJoin::ROUND):
            out << "round"sv;
            break;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const Color color) {
        std::visit(ColorPrinter{ out }, color);
        return out;
    }

}  // namespace svg
