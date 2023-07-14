#include "json.h"

namespace json {

namespace {

using namespace std::literals;

Node LoadNode(std::istream& in);
Node LoadString(std::istream& in);

std::string LoadLiteral(std::istream& in) {
    std::string s;
    while (std::isalpha(in.peek())) {
        s.push_back(static_cast<char>(in.get()));
    }
    return s;
}

Node LoadArray(std::istream& in) {
    std::vector<Node> result;
    for (char c; in >> c && c != ']';) {
        if (c != ',') {
            in.putback(c);
        }
        result.push_back(LoadNode(in));
    }
    if (!in) {
        throw ParsingError("Array parsing error"s);
    }
    return Node(std::move(result));
}

Node LoadDict(std::istream& in) {
    Dict dict;
    for (char c; in >> c && c != '}';) {
        if (c == '"') {
            std::string key = LoadString(in).AsString();
            if (in >> c && c == ':') {
                if (dict.find(key) != dict.end()) {
                    throw ParsingError("Duplicate key '"s + key + "' have been found");
                }
                dict.emplace(std::move(key), LoadNode(in));
            } else {
                throw ParsingError(": is expected but '"s + c + "' has been found"s);
            }
        } else if (c != ',') {
            throw ParsingError(R"(',' is expected but ')"s + c + "' has been found"s);
        }
    }
    if (!in) {
        throw ParsingError("Dictionary parsing error"s);
    }
    return Node(std::move(dict));
}

Node LoadString(std::istream& in) {
    auto it = std::istreambuf_iterator<char>(in);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            ++it;
            break;
        } else if (ch == '\\') {
            ++it;
            if (it == end) {
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            switch (escaped_char)
            {
            case 'n':
                s.push_back('\n');
                break;
            case 't':
                s.push_back('\t');
                break;
            case 'r':
                s.push_back('\r');
                break;
            case '"':
                s.push_back('"');
                break;
            case '\\':
                s.push_back('\\');
                break;
            default:
                throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            throw ParsingError("Unexpected end of line");
        } else {
            s.push_back(ch);
        }
        ++it;
    }
    return Node(std::move(s));
}

Node LoadBool(std::istream& in) {
    const auto s = LoadLiteral(in);
    if (s == "true"sv) {
        return Node{ true };
    } else if (s == "false"sv) {
        return Node{ false };
    } else {
        throw ParsingError("Failed to parse '"s + s + "' as bool"s);
    }
}

Node LoadNull(std::istream& in) {
    if (auto literal = LoadLiteral(in); literal == "null"sv) {
        return Node{ nullptr };
    } else {
        throw ParsingError("Failed to parse '"s + literal + "' as null"s);
    }
}

Node LoadNumber(std::istream& in) {
    std::string parsed_num;
    auto read_char = [&parsed_num, &in] {
        parsed_num += static_cast<char>(in.get());
        if (!in) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };
    auto read_digits = [&in, read_char] {
        if (!std::isdigit(in.peek())) {
            throw ParsingError("A digit is expected");
        }
        while (std::isdigit(in.peek())) {
            read_char();
        }
    };
    if (in.peek() == '-') {
        read_char();
    }
    if (in.peek() == '0') {
        read_char();
    } else {
        read_digits();
    }
    bool is_int = true;
    if (in.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }
    if (int ch = in.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = in.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }
    try {
        if (is_int) {
            try {
                return std::stoi(parsed_num);
            } catch (...) {

            }
        }
        return std::stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

Node LoadNode(std::istream& in) {
    char c;
    if (!(in >> c)) {
        throw ParsingError("Unexpected EOF");
    }
    switch (c)
    {
    case '[':
        return LoadArray(in);
    case '{':
        return LoadDict(in);
    case '"':
        return LoadString(in);
    case 't':
        [[fallthrough]]; /*go to the next instruction*/
    case 'f':
        in.putback(c);
        return LoadBool(in);
    case 'n':
        in.putback(c);
        return LoadNull(in);
    default:
        in.putback(c);
        return LoadNumber(in);
    }
}

struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    PrintContext Indented() const {
        return { out, indent_step, indent_step + indent };
    }
};

void PrintNode(const Node& value, const PrintContext& ctx);

template <typename Value>
void PrintValue(const Value& value, const PrintContext& ctx) {
    ctx.out << value;
}

void PrintString(const std::string& value, std::ostream& out) {
    out.put('"');
    for (const char c : value) {
        switch (c)
        {
        case '\r':
            out << "\\r"sv;
            break;
        case '\n':
            out << "\\n"sv;
            break;
        case '"':
            [[fallthrough]];
        case '\\':
            out.put('\\');
            [[fallthrough]];
        default:
            out.put(c);
            break;
        }
    }
    out.put('"');
}

template <>
void PrintValue<std::string>(const std::string& value, const PrintContext& ctx) {
    PrintString(value, ctx.out);
}

template <>
void PrintValue<std::nullptr_t>(const std::nullptr_t&, const PrintContext& ctx) {
    ctx.out << "null"sv;
}

template <>
void PrintValue<bool>(const bool& value, const PrintContext& ctx) {
    ctx.out << (value ? "true"sv : "false"sv);
}

template <>
void PrintValue<Array>(const Array& nodes, const PrintContext& ctx) {
    std::ostream& out = ctx.out;
    out << "[\n"sv;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const Node& node : nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        inner_ctx.PrintIndent();
        PrintNode(node, inner_ctx);
    }
    out.put('\n');
    ctx.PrintIndent();
    out.put(']');
}

template <>
void PrintValue<Dict>(const Dict& nodes, const PrintContext& ctx) {
    std::ostream& out = ctx.out;
    out << "{\n"sv;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const auto& [key, node] : nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        inner_ctx.PrintIndent();
        PrintString(key, ctx.out);
        out << ": "sv;
        PrintNode(node, inner_ctx);
    }
    out.put('\n');
    ctx.PrintIndent();
    out.put('}');
}

void PrintNode(const Node& node, const PrintContext& ctx) {
    std::visit(
        [&ctx](const auto& value) {
            PrintValue(value, ctx);
        },
        node.GetValue());
}

}  // namespace

Document Load(std::istream& in) {
    return Document{ LoadNode(in) };
}

void Print(const Document& doc, std::ostream& out) {
    PrintNode(doc.GetRoot(), PrintContext{ out });
}

}  // namespace json