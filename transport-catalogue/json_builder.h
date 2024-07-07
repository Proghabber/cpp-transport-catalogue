#pragma once
#include "json.h"
#include <vector>
#include <string>
#include <exception>
#include <variant>
#include <utility>



namespace json{
    class Builder;
    class ContextArray;
    class ContextDict;
    class ContextValueKey;
    class ContextKey;

     class BuilderContext{
        protected:
        Builder& builder_;
        public:
        BuilderContext(Builder& main);
        Builder& GetBuilder();
        BuilderContext Value(Node::Value input);
        ContextArray StartArray();
        BuilderContext EndArray();
        ContextDict StartDict();
        BuilderContext EndDict();
        ContextKey Key(std::string key);
        json::Node Build();
    };

    class ContextKey:public BuilderContext{
    public:
        ContextKey(Builder& builder);
        ContextValueKey Value(Node::Value input);
        BuilderContext EndArray() = delete;
        BuilderContext EndDict() = delete;
        ContextKey Key(std::string key) = delete;
        json::Node Build() = delete;
    };

    class ContextValueKey:public BuilderContext{
    public:
        ContextValueKey(Builder& builder);
        ContextValueKey Value(Node::Value input) = delete;
        ContextArray StartArray()  = delete;
        BuilderContext EndArray()  = delete;
        ContextDict StartDict()  = delete;
        json::Node Build() = delete;
    };

    class ContextArray:public BuilderContext{
    public:
        ContextArray(Builder& builder);
        ContextArray Value(Node::Value input);
        BuilderContext EndDict()  = delete;
        ContextKey Key(std::string key)  = delete;
        json::Node Build() = delete;
    };


    class ContextDict:public BuilderContext{
    public:
        ContextDict(Builder &builder);
        BuilderContext Value(Node::Value input) = delete;
        ContextArray StartArray() = delete;
        BuilderContext EndArray() = delete;
        ContextDict StartDict() = delete;
        json::Node Build() = delete;
    };

   
    class Builder{
    private:
        json::Node nod_;
        std::vector<Node*> nodes_;

        Node::Value& GetCurrentValue(); 
        const Node::Value& GetCurrentValue() const; 
        void AssertNewObjectContext() const;
        void AddObject(Node::Value value, bool one_shot);
    
    public:
        Builder();
        BuilderContext Value(Node::Value input);
        ContextArray StartArray();
        BuilderContext EndArray();
        ContextDict StartDict();
        BuilderContext EndDict();
        ContextKey Key(std::string key);
        json::Node Build();
    };
}