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
        //ContextArray StartArray() override ;
        BuilderContext EndArray() = delete;
        //ContextDict StartDict() override ;
        BuilderContext EndDict() = delete;
        ContextKey Key(std::string key) = delete;
        json::Node Build() = delete;

    };

    class ContextValueKey:public BuilderContext{
    public:// для dict
        ContextValueKey(Builder& builder);
        ContextValueKey Value(Node::Value input) = delete;
        ContextArray StartArray()  = delete;
        BuilderContext EndArray()  = delete;
        ContextDict StartDict()  = delete;
        //BuilderContext EndDict() override ;
        //ContextKey Key(std::string key)  = delete;
        json::Node Build() = delete;

    };

    class ContextArray:public BuilderContext{
    public:
        ContextArray(Builder& builder);
        //ContextValueKey Value(Node::Value input) = delete;
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
        //BuilderContext EndDict() override ;
        //ContextKey Key(std::string key) override ;
        json::Node Build() = delete;


    };

    // class ContextDictValue:public BuilderContext {
    //     public:
    //     ContextDictValue(Builder &builder);
    //     BuilderContext Value(Node::Value input) = delete;
    //     ContextArray StartArray() = delete;
    //     //BuilderContext EndArray();
    //     ContextDict StartDict() = delete;
    //     BuilderContext EndDict();
    //     ContextKey Key(std::string key) = delete;
    //     json::Node Build() = delete;

    // };
    

    class Builder{
    private:
        json::Node nod_;
        std::vector<Node*> nodes_;

        Node::Value& GetCurrentValue(); // понравилось у автора
        const Node::Value& GetCurrentValue() const; // понравилось у автора
        void AssertNewObjectContext() const; // понравилось у автора
        void AddObject(Node::Value value, bool one_shot);// понравилось у автора
    
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