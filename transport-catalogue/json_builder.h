#pragma once
#include "json.h"

#include <string>
#include <vector>

namespace json {

class Builder {
private:
    class DictContext;
    class ArrayContext;
    class KeyContext;

    class Context {
    public:
        Context(Builder& builder);

        KeyContext Key(std::string key);

        DictContext StartDict();
        ArrayContext StartArray();
        Builder& EndDict();
        Builder& EndArray();
    protected:
        Builder& builder_;
    };

    class KeyContext : public Context {
    public:
        KeyContext Key(std::string key) = delete;

        DictContext Value(Node::Value value);

        Builder& EndDict() = delete;
        Builder& EndArray() = delete;
    };

    class DictContext : public Context {
    public:
        DictContext StartDict() = delete;
        ArrayContext StartArray() = delete;
        Builder& EndArray() = delete;
    };

    class ArrayContext : public Context {
    public:
        KeyContext Key(std::string key) = delete;

        ArrayContext Value(Node::Value value);

        Builder& EndDict() = delete;
    };

public:
    Builder() = default;

    KeyContext Key(std::string key);

    Builder& Value(Node::Value value);

    DictContext StartDict();
    ArrayContext StartArray();
    Builder& EndDict();
    Builder& EndArray();

    Node Build();

    ~Builder() = default;
private:
    Node root_;
    std::vector<Node*> nodes_stack_;

    Node* SetNode(Node node);
};

}  // namespace json