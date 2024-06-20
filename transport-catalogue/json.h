#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <sstream>  
#include <algorithm>

namespace json {
    class Node;
    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using Data = std::variant<std::nullptr_t, int, double, std::string, Array, Dict, bool>;
    using Number = std::variant<int, double>;

    //std::ostream& operator<<(std::ostream& out , const Node& node);
    std::ostream& operator<<(std::ostream& out , Array);
    std::ostream& operator<<(std::ostream& out , const Dict& data);

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
    /* Реализуйте Node, используя std::variant */
        Node();
        Node(Array array)
            : data_(move(array)) 
        {
        }
    
        Node(Dict map)
            : data_(move(map)) 
        {
        }
    
        Node(int value)
            : data_(value) 
        {
        }
    
        Node(std::string value)
            : data_(move(value)) 
        {
        }
    
        Node(double value) 
            : data_(value)
        {
        }
    
        Node(bool value) 
            : data_(value)
        {
        }
        Node(std::nullptr_t value) 
            : data_(value)
        {
        }

        bool IsInt() const noexcept;
        bool IsDouble() const noexcept; //Возвращает true, если в Node хранится int либо double.
        bool IsPureDouble() const noexcept; //Возвращает true, если в Node хранится double.
        bool IsBool() const noexcept;
        bool IsString() const noexcept;
        bool IsNull() const noexcept;
        bool IsArray() const noexcept;
        bool IsMap() const noexcept;

        const Array& AsArray() const;
        const Dict& AsMap() const;
        int AsInt() const;
        const std::string& AsString() const;
        bool AsBool() const;
        double AsDouble() const;
        const Data& GetData()const;

        void RetText(std::ostringstream& ) const;
        bool operator==(const Node& other) const;
        bool operator!=(const Node& other) const;
    private:
        Data data_;
    };

    struct swapper{
        std::ostream& out;

        void operator()(){ 
            out<< "null";   
        }

        void  operator()(double root){
            out<< root;
        }

        void  operator()(int root){
            out<< root;
        }

        void  operator()(bool root){
            std::string text;
            if (root){
                text = "true";
            } else { 
                text = "false";
            }
            out<<text;
        }

        void  operator()(std::string root ){
            std::string text;
            text += "\"";
            for (auto c:root){
                if (c == '\"' || c == '\\'){
                    text+='\\';    
                } else if (c == '\n'){
                    text += "\\n";
                    continue;
                } else if (c == '\t'){
                    text += "\\t";
                    continue;
                } else if (c == '\r'){
                    text += "\\r";
                    continue;
                } 
                text+=c;
            }
            text+="\"";
            out<< text;
        }

        void  operator()(Array root){
            out<<root;
        }

        void  operator()(Dict root) {
            out<<root;
        }
    };

    class Document {
    public:
        explicit Document(Node root);
        const Node& GetRoot() const;
        bool operator==(const Document& other) const;
        bool operator!=(const Document& other) const;
    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json