#include "json.h"

using namespace std;

namespace json {  
    namespace {
        Number LoadNumber(std::istream& input){
            using namespace std::literals;

            std::string parsed_num;
            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            } else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }
            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    } catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            } catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        std::string LoadS(std::istream& input){
            using namespace std::literals;    
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                } else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
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
                            // Встретили неизвестную escape-последовательность
                            throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                } else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                } else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }
            return s;
        }

        Node LoadNode(istream& input);//обьявим пораньше

        Node LoadArray(istream& input) {
            Array result;  
            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (!input){
                throw ParsingError("LoadArray() нечего парсить");
            }
            return Node(move(result));
        }

        Node LoadString(istream& input){
            if (!input){
                throw ParsingError("LoadStriny() нечего парсить");
            }
            string line = LoadS(input);
            return Node(move(line));
        }

        Node LoadDict(istream& input){
            Dict result;
            for (char c; input >> c && c != '}';) {
                if (c=='\"'){
                    std::string key =  LoadString(input).AsString();// строим строку до следующей ковычки
                    if (input>>c && c==':'){ //и начинаем ловить значение
                        if (result.find(key)!= result.end()){ //может ключ уже есть
                            throw ParsingError("ключ уже есть " + key );
                        }
                        result.emplace(std::move(key), LoadNode(input));//в input останутся данные на основе которых построю Node
                    } else {//
                        throw ParsingError(" ошибка вместо : имеем " + c );
                    }

                } else if (c!=','){// следующий ключ должен идти через запятую
                    throw ParsingError(" ошибка вместо , имеем " + c );
                } 
            }
            if (!input){//если нечего парсить
                throw ParsingError(" ошибка парсинга Dict "  );
            } else {
                return Node(move(result));
            }
        }

        std::string SeparateAlpha(std::istream& input){
            std::string retur;
            bool flag = true;
            while (flag){
                flag = std::isalpha(input.peek());
                if (flag){
                    retur.push_back(static_cast<char>(input.get()));
                }    
            }
            return retur;
        }


        Node LoadNull(istream& input){
            std::string empty = SeparateAlpha(input);
            if (empty != "null"){
                throw ParsingError("LoadNull() нечего парсить либо ошибка ");
            }
            return Node();
        }
        Node LoadNode(istream& input);
        
        Node LoadBool(istream& input){
            if (!input){
                throw ParsingError("LoadBool() нечего парсить");
            }
            string boolle= SeparateAlpha(input);
            if (boolle != "true" && boolle != "false"){
                throw ParsingError("error bool");
            }
            if (boolle == "true"){
                return Node{true};
            }
            return Node{false};
            
        }

        Node LoadNode(istream& input){
            char c;
            input >> c;
            if (c == '[') {
                return LoadArray(input);
            } else if (c == '{') {
                return LoadDict(input);
            } else if (c == '"') {
                return LoadString(input);
            }else if( c=='n'){
                input.putback(c);
                return LoadNull(input);
            }else if(c=='t' || c=='f'){
                input.putback(c);
                return LoadBool(input);
            } else {
                input.putback(c);
            
                auto digit=LoadNumber(input);
                if (holds_alternative<int>(digit)){
                    return std::get<int>(digit);
                }
                if (holds_alternative<double>(digit)){
                    return std::get<double>(digit);
                }   
            }
            return Node();
        }

    }  // namespace
    Node::Node()
    {
    }

    bool Node::IsInt() const noexcept {
        return std::holds_alternative<int>(data_);
    }


    bool Node::IsPureDouble() const noexcept {
        return std::holds_alternative<double>(data_);
    } 

    bool Node::IsDouble() const noexcept {
        if (IsInt() || IsPureDouble()){
            return true;
        }
        return false;
    } 

    bool Node::IsBool() const noexcept {
        return std::holds_alternative<bool>(data_);
    }
    bool Node::IsString() const noexcept {
        return std::holds_alternative<std::string>(data_);
    }
    bool Node::IsNull() const noexcept {
        return std::holds_alternative<std::nullptr_t>(data_);
    }
    bool Node::IsArray() const noexcept {
        return std::holds_alternative<Array>(data_);
    }
    bool Node::IsMap() const noexcept {
        return std::holds_alternative<Dict>(data_);
    }

    const Array& Node::AsArray() const {  
        if (!IsArray()){
            throw std::logic_error("arror value");
        }
        return std::get<Array>(data_);
    }

    const Dict& Node::AsMap() const {
        if (!IsMap()){
            throw std::logic_error("arror value");
        }
        return std::get<Dict>(data_);
    }

    int Node::AsInt() const {
        if (!IsInt()){
            throw std::logic_error("arror value");
        }
        return std::get<int>(data_);
    }

    const string& Node::AsString() const {
        if (!IsString()){
            throw std::logic_error("arror value");
        }
        return std::get<std::string>(data_);
    }

    bool Node::AsBool() const {
        if (!IsBool()){
            throw std::logic_error("arror value");
        }
        return std::get<bool>(data_);
    }

    double Node::AsDouble() const {
        if (!IsDouble()){
            throw std::logic_error("arror value");
        }
        if (IsInt()){
            return std::get<int>(data_);
        }
        return std::get<double>(data_);  
    }

    const Data& Node::GetData() const {
        return data_;
    }

    void Node::RetText(std::ostringstream& text)  const {
        if (IsInt()){
            std::visit(swapper{text},data_);
        } else if (IsDouble()){
            std::visit(swapper{text},data_);
        } else if (IsPureDouble()){
            std::visit(swapper{text},data_);
        } else if (IsBool()){
            std::visit(swapper{text},data_);
        } else if (IsString()){
            std::visit(swapper{text},data_);
        } else if (IsNull()){
            std::visit(swapper{text});
        } else if (IsArray()){
            std::visit(swapper{text},data_);
        } else if (IsMap()){
            std::visit(swapper{text},data_);
        }  
    }

    inline std::ostream& operator<<(std::ostream& out , Array data){
        std::ostringstream text;
        bool flag= false;
        for (Node& n: data){
            if (flag){
                text<<", ";
            }
            n.RetText(text);
            flag=true;  
        }
        out<<"["<<text.str()<<"]";
        return out;
    }

    std::ostream &operator<<(std::ostream &out, const Dict &data){
        std::ostringstream text;
        bool flag = false;
        for (const auto& [product, price] : data){
            if(flag){
                text<<",";
            }
            text<<"\""<<product<<"\""<<":";
            price.RetText(text);
            flag=true; 
        
        }
        out<<"{"<<text.str()<<"}";
        return out;
    }

    bool Node::operator==(const Node& other)  const {
        return GetData() == other.GetData();
    }

    bool Node::operator!=(const Node& other)  const {
        return !(*this == other);
    }

    Document::Document(Node root): root_(move(root)) 
    {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    bool Document::operator==(const Document& other)  const {
        return GetRoot() == other.GetRoot();
    }

    bool Document::operator!=(const Document& other)  const {
        return !(*this == other);
    }

    Document Load(std::istream &input){
        return Document{LoadNode(input)};
    }

    void Print(const Document& doc, std::ostream& output) {
        (void) &doc;
        (void) &output;

        std::ostringstream strm;
        auto s = doc.GetRoot();
        s.RetText(strm);
        output << strm.str();
    }
}  // namespace json