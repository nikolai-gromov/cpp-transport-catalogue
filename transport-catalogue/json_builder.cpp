#include "json_builder.h"

namespace json {

Builder::KeyContext Builder::Key(std::string key) {
    using namespace std::literals;
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
        throw std::logic_error("Cannot call Key outside a dictionary"s);
    }

    Dict& current_node = const_cast<Dict&>(nodes_stack_.back()->AsDict());
    current_node.emplace(key, key);
    nodes_stack_.push_back(&current_node[key]);
    return { *this };
}

Builder& Builder::Value(Node::Value value) {
    std::visit(
                [this](auto node)
                {
                    SetNode(node);
                }
                , value);
    return *this;
}

Builder::DictContext Builder::StartDict() {
    nodes_stack_.push_back(SetNode(Dict{}));
    return { *this };
}

Builder::ArrayContext Builder::StartArray() {
    nodes_stack_.push_back(SetNode(Array{}));
    return { *this };
}

Builder& Builder::EndDict() {
    using namespace std::literals;
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
        throw std::logic_error("Cannot call EndDict outside a dictionary"s);
    }
    nodes_stack_.pop_back();
    return *this;
}

Builder& Builder::EndArray() {
    using namespace std::literals;
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
        throw std::logic_error("Cannot call EndArray outside an array"s);
    }
    nodes_stack_.pop_back();
    return *this;
}

Node Builder::Build() {
    using namespace std::literals;
    if (!(nodes_stack_.empty()) || root_.IsNull()) {
        throw std::logic_error("Cannot call Build on an unfinished object"s);
    }
    return root_;
}

Node* Builder::SetNode(Node node) {
    using namespace std::literals;
    if (nodes_stack_.empty() && root_.IsNull()) {
        root_ = node;
        return &root_;
    }

    if (nodes_stack_.empty()) {
        throw std::logic_error("Invalid context for setting node"s);
    }


    if (nodes_stack_.back()->IsArray()) {
        Array& current_node = const_cast<Array&>(nodes_stack_.back()->AsArray());
        current_node.push_back(node);
        return &current_node.back();
    }

    if (nodes_stack_.back()->IsString()) {
        const std::string key = nodes_stack_.back()->AsString();
        nodes_stack_.pop_back();
        if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
            throw std::logic_error("Cannot call Key outside a dictionary"s);
        }
        Dict& current_node = const_cast<Dict&>(nodes_stack_.back()->AsDict());
        current_node[key] = node;
        return &current_node[key];
    }

    throw std::logic_error("Invalid context for setting node"s);
}

Builder::Context::Context(Builder& builder)
    : builder_(builder)
{
}

Builder::DictContext Builder::Context::StartDict() {
    builder_.StartDict();
    return { builder_ };
}

Builder::ArrayContext Builder::Context::StartArray() {
    builder_.StartArray();
    return { builder_ };
}

Builder& Builder::Context::EndDict() {
    builder_.EndDict();
    return builder_;
}

Builder& Builder::Context::EndArray() {
    builder_.EndArray();
    return builder_;
}

Builder::KeyContext Builder::Context::Key(std::string key) {
    builder_.Key(key);
    return { builder_ };
}

Builder::DictContext Builder::KeyContext::Value(Node::Value value) {
    builder_.Value(value);
    return { builder_ };
}

Builder::ArrayContext Builder::ArrayContext::Value(Node::Value value) {
    builder_.Value(value);
    return { builder_ };
}

}  // namespace json