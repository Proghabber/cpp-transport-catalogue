#include "json_builder.h"

namespace json{

     Builder::Builder(): nod_(),nodes_{&nod_}
    {
    }
    
    Node::Value& Builder::GetCurrentValue(){
        if (nodes_.empty()) {
            throw std::logic_error("Attempt to change finalized JSON");
        }
        return nodes_.back()->GetValue();
    };

    const Node::Value& Builder::GetCurrentValue() const{
         return const_cast<Builder*>(this)->GetCurrentValue();
    };

    void Builder::AssertNewObjectContext() const{
        if (!std::holds_alternative<std::nullptr_t>(GetCurrentValue())) {
            throw std::logic_error("New object in wrong context");
        }
    };

    void Builder::AddObject(Node::Value value, bool one_shot){
        Node::Value& host_value = GetCurrentValue();
        if (std::holds_alternative<Array>(host_value)){
            Node& node
            = std::get<Array>(host_value).emplace_back(std::move(value));
            if (!one_shot) { 
            nodes_.push_back(&node); 
            }
        } else {
            AssertNewObjectContext(); 
            host_value = std::move(value);
            if (one_shot) { 
                nodes_.pop_back(); 
            }
        }
    };

   BuilderContext Builder::Value(Node::Value input){
        AddObject(input, true);
        return BuilderContext(*this);
    };

    ContextArray Builder::StartArray(){
        AddObject(Array{}, false);
        return ContextArray(*this);
    };

    BuilderContext Builder::EndArray(){
        if (!std::holds_alternative<Array>(GetCurrentValue())) {
            throw std::logic_error("EndArray() outside a Array");
        }
        nodes_.pop_back();
        return BuilderContext(*this);
    };

    ContextDict Builder::StartDict(){
        AddObject(Dict{}, /* one_shot */ false);
        return ContextDict(*this);
    };

    BuilderContext Builder::EndDict(){
        if (!std::holds_alternative<Dict>(GetCurrentValue())) { 
            throw std::logic_error("EndDict() outside a dict");
        }
        nodes_.pop_back();
        return BuilderContext(*this);
    };

    ContextKey Builder::Key(std::string key){
        Node::Value& host_value = GetCurrentValue();
    
        if (!std::holds_alternative<Dict>(host_value)) {
            throw std::logic_error("Key() outside a dict");
        }  
        nodes_.push_back(
            &std::get<Dict>(host_value)[std::move(key)]
        );
        return ContextKey(*this);
    };

    json::Node Builder::Build(){
        if (!nodes_.empty()) { 
            throw std::logic_error("Attempt to build JSON which isn't finalized"); 
        }
        return std::move(nod_);
    };

    BuilderContext::BuilderContext(Builder &main): builder_(main)
    {
    }

    Builder& BuilderContext::GetBuilder(){
       return builder_;
    }

    BuilderContext BuilderContext::Value(Node::Value input){
        builder_.Value(input);
        return BuilderContext(builder_);
    }

    ContextArray BuilderContext::StartArray(){
        builder_.StartArray();
        return ContextArray(builder_);
    }

    BuilderContext BuilderContext::EndArray(){
        builder_.EndArray();
        return builder_;
    }

    ContextDict BuilderContext::StartDict(){
        builder_.StartDict();
        return ContextDict(builder_);
    }

   BuilderContext BuilderContext::EndDict(){
        builder_.EndDict();
        return builder_;
    }

    ContextKey BuilderContext::Key(std::string key){
        builder_.Key(key);
        return  (builder_);
    }

    json::Node BuilderContext::Build(){
        return builder_.Build();
    }

    ContextArray::ContextArray(Builder &main): BuilderContext(main)
    {
    }

    ContextArray ContextArray::Value(Node::Value input){
        builder_.Value(input);
        return ContextArray(builder_);
    }

    ContextDict::ContextDict(Builder &builder): BuilderContext(builder)
    {
    }

    ContextValueKey::ContextValueKey(Builder &builder): BuilderContext(builder)
    {
    }

    ContextKey::ContextKey(Builder &builder): BuilderContext(builder)
    {
    }

    ContextValueKey ContextKey::Value(Node::Value input){
        builder_.Value(input);
        return ContextValueKey(builder_);
    }
}