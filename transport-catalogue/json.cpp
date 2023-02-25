#include "json.h"

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);

        using Number = std::variant<int, double>;

        Number ParseNumber(std::istream& input) {
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
            }
            else {
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
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // Считывает содержимое строкового литерала JSON-документа
        // Функцию следует использовать после считывания открывающего символа ":
        std::string ParseString(std::istream& input) {
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
                }
                else if (ch == '\\') {
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
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return s;
        }

        Node LoadArray(istream& input) {
            Array result;
            char c;
            for (; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (c != ']') {
                throw ParsingError("Failed to parse Array. Have no end character ]"s);
            }
            return Node(move(result));
        }

        Node LoadBool(istream& input) {
            std::string res;
            char c = input.peek();

            int length = (c == 't') ? 4 : 5;

            for (int i = 0; i < length; ++i) {
                if (input.get(c)) {
                    res += c;
                }
            }
            c = EOF;
            input >> c;
            // Проверка на наличие лишних символов, кроме закрывающих
            if (c != EOF && c != ',' && c != ' ' && c != ']' && c != '}') {
                throw ParsingError("Failed to parse bool node"s);
            }
            // Проверка на слово true и false
            if (res != "true"s && res != "false"s) {
                throw ParsingError("Failed to parse bool node"s);
            }
            input.putback(c);

            if (res == "true"s) {
                return Node(true);
            }
            else {
                return Node(false);
            }
        }

        Node LoadNull(istream& input) {
            std::string res;
            char c;

            for (int i = 0; i < 4; ++i) {
                if (input.get(c)) {
                    res += c;
                }
            }
            //c = input.peek();
            //Если после null остались символы
            c = EOF;
            input >> c;
            // Проверка на наличие лишних символов, кроме закрывающих
            if (c != EOF && c != ',' && c != ' ' && c != ']') {
                throw ParsingError("Failed to parse null node");
            }
            // Проверка на слово true и false
            if (res != "null"s) {
                throw ParsingError("Failed to parse null node");
            }
            input.putback(c);

            return Node(nullptr);
        }

        Node LoadNumber(istream& input) {
            auto num = ParseNumber(input);
            if (holds_alternative<int>(num)) {
                return Node(get<int>(num));
            }
            else {
                return Node(get<double>(num));
            }
        }

        Node LoadValue(istream& input) {
            char c = input.peek();
            if (c == 'n') {
                return LoadNull(input);
            }
            else if (c == 't' || c == 'f') {
                return LoadBool(input);
            }
            else if (std::isdigit(c) || c == '-') {
                return LoadNumber(input);
            }
            else {
                throw ParsingError("Failed to parse document");
            }
        }

        Node LoadString(istream& input) {
            return move(ParseString(input));
        }

        Node LoadDict(istream& input) {
            Dict result;
            char c;
            for (; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }
                string key = ParseString(input);
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }
            if (c != '}') {
                throw ParsingError("Failed to parse Array. Have no end character }"s);
            }
            return Node(move(result));
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;
            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else {
                input.putback(c);
                return LoadValue(input);
            }
        }

    }  // namespace

    bool Node::IsNull() const {
        return holds_alternative<nullptr_t>(*this);
    }

    bool Node::IsInt() const {
        return holds_alternative<int>(*this);
    }

    bool Node::IsDouble() const {
        return holds_alternative<int>(*this) || holds_alternative<double>(*this);
    }

    bool Node::IsPureDouble() const {
        return holds_alternative<double>(*this);
    }

    bool Node::IsBool() const {
        return holds_alternative<bool>(*this);
    }

    bool Node::IsString() const {
        return holds_alternative<string>(*this);
    }

    bool Node::IsArray() const {
        return holds_alternative<Array>(*this);
    }

    bool Node::IsMap() const {
        return holds_alternative<Dict>(*this);
    }

    int Node::AsInt() const {
        if (!IsInt()) {
            throw std::logic_error("Node doesn't int");
        }
        return get<int>(*this);
    }

    bool Node::AsBool() const {
        if (!IsBool()) {
            throw std::logic_error("Node doesn't bool");
        }
        return get<bool>(*this);
    }

    double Node::AsDouble() const {
        if (!IsDouble()) {
            throw std::logic_error("Node doesn't int or double");
        }
        return IsPureDouble() ? get<double>(*this) : static_cast<double>(get<int>(*this));
    }

    const std::string& Node::AsString() const {
        if (!IsString()) {
            throw std::logic_error("Node doesn't string");
        }
        return get<string>(*this);
    }

    const Array& Node::AsArray() const {
        if (!IsArray()) {
            throw std::logic_error("Node doesn't Array");
        }
        return get<Array>(*this);
    }

    const Dict& Node::AsMap() const {
        if (!IsMap()) {
            throw std::logic_error("Node doesn't map");
        }
        return get<Dict>(*this);
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    bool operator==(const Node& lhs, const Node& rhs) {
        return static_cast<Value>(lhs) == static_cast<Value>(rhs);
    }

    bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

    namespace {
        using namespace std::literals;

        struct PrintContext {
            std::ostream& out;
            int indent_step = 2;
            int indent = 0;

            void PrintIndent() const {
                for (int i = 0; i < indent; ++i) {
                    out.put(' ');
                }
            }

            // Возвращает новый контекст вывода с увеличенным смещением
            PrintContext Indented() const {
                return { out, indent_step, indent_step + indent };
            }
        };

        std::string SetEscapes(const std::string& str) {
            std::string result;
            for (const auto& c : str) {
                if (c == '\"') {
                    result += "\\\"";
                }
                else if (c == '\r') {
                    result += "\\r";
                }
                else if (c == '\n') {
                    result += "\\n";
                }
                else if (c == '\\') {
                    result += "\\\\";
                }
                else {
                    result += c;
                }
            }
            return result;
        }

        void PrintNode(const Node& node, const PrintContext& ctx);

        void PrintValue(std::nullptr_t, const PrintContext& ctx) {
            ctx.out << "null"sv;
        }

        void PrintValue(const bool value, const PrintContext& ctx) {
            ctx.out << std::boolalpha << value;
        }

        void PrintValue(const std::string& str, const PrintContext& ctx) {
            ctx.out << "\""sv;
            ctx.out << SetEscapes(str);
            ctx.out << "\""sv;
        }

        void PrintValue(const Array& arr, const PrintContext& ctx) {
            bool is_first = true;
            if (arr.size()) {
                ctx.out << "["sv << endl;
                PrintContext map_ctx{ ctx.Indented() };
                map_ctx.PrintIndent();
                for (const auto& value : arr) {
                    if (!is_first) {
                        map_ctx.out << ", "sv << endl;
                        map_ctx.PrintIndent();
                    }
                    PrintNode(value, map_ctx);
                    is_first = false;
                }
                ctx.out << endl;
                ctx.PrintIndent();
                ctx.out << "]"sv;
            }
            else {
                ctx.out << "[]"sv;
            }
        }

        void PrintValue(const Dict& dict, const PrintContext& ctx) {
            bool is_first = true;
            ctx.out << "{"sv << endl;
            if (dict.size()) {
                PrintContext map_ctx{ ctx.Indented() };
                map_ctx.PrintIndent();
                for (const auto& value : dict) {
                    if (!is_first) {
                        map_ctx.out << ", "sv << endl;
                        map_ctx.PrintIndent();
                    }
                    PrintValue(value.first, map_ctx);
                    map_ctx.out << ": "sv;
                    PrintNode(value.second, map_ctx);
                    is_first = false;
                }
                ctx.out << endl;
                ctx.PrintIndent();
                ctx.out << "}"sv;
            }
            else {
                ctx.out << "{}"sv;
            }
        }

        template <typename Number>
        void PrintValue(const Number& value, const PrintContext& ctx) {
            ctx.out << value;
        }

        void PrintNode(const Node& node, const PrintContext& ctx) {
            std::visit(
                [&ctx](const auto& value) { PrintValue(value, { ctx }); },
                static_cast<Value>(node));
        }

    } // namespace

    void Print(const Document& doc, std::ostream& output) {
        const auto& document = doc.GetRoot();
        std::visit([&output](const auto& val) {
            return PrintValue(val, { output });
            }, static_cast<Value>(document));
    }

}  // namespace json