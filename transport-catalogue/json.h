#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

class Node;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node final
    : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
public:
    using variant::variant;
    using Value = variant;

    bool IsInt() const {
        return std::holds_alternative<int>(*this);
    }
    bool IsDouble() const {
        return std::holds_alternative<double>(*this) || IsInt();
    }
    bool IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    }
    bool IsBool() const {
        return std::holds_alternative<bool>(*this);
    }
    bool IsString() const {
        return std::holds_alternative<std::string>(*this);
    }
    bool IsNull() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }
    bool IsArray() const {
        return std::holds_alternative<Array>(*this);
    }
    bool IsMap() const {
        return std::holds_alternative<Dict>(*this);
    }

    int AsInt() const {
        using namespace std::literals;
        if (!IsInt()) {
            throw std::logic_error("Is not a int");
        }
        return std::get<int>(*this);
    }
    bool AsBool() const {
        using namespace std::literals;
        if (!IsBool()) {
            throw std::logic_error("Is not a bool");
        }
        return std::get<bool>(*this);
    }
    double AsDouble() const {
        using namespace std::literals;
        if (!IsDouble()) {
            throw std::logic_error("Is not a double");
        }
        return IsPureDouble() ? std::get<double>(*this) : AsInt();
    }
    const std::string& AsString() const {
        using namespace std::literals;
        if (!IsString()) {
            throw std::logic_error("Is not a string");
        }

        return std::get<std::string>(*this);
    }
    const Array& AsArray() const {
        using namespace std::literals;
        if (!IsArray()) {
            throw std::logic_error("Is not an array");
        }

        return std::get<Array>(*this);
    }
    const Dict& AsMap() const {
        using namespace std::literals;
        if (!IsMap()) {
            throw std::logic_error("Is not a map");
        }

        return std::get<Dict>(*this);
    }

    bool operator==(const Node& rhs) const {
        return GetValue() == rhs.GetValue();
    }

    const Value& GetValue() const {
        return *this;
    }
};

inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}

class Document {
public:
    explicit Document(Node root)
        : root_(std::move(root)) {
    }

    const Node& GetRoot() const {
        return root_;
    }

private:
    Node root_;
};

inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}

inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}

Document Load(std::istream& in);

void Print(const Document& doc, std::ostream& out);

}  // namespace json