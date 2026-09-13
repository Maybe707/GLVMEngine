#pragma once

#include "JsonParser.hpp"
#include <charconv>
#include <memory>
#include <string_view>

namespace GLVM::Core {
// A bounded JSON reader. Syntax errors never advance beyond the input buffer.
class JsonReader {
    std::string_view input;
    size_t position = 0;
    [[noreturn]] void fail() const {
        throw std::runtime_error("Invalid JSON at byte " + std::to_string(position));
    }
    void whitespace() {
        while (position < input.size() && (input[position] == ' ' || input[position] == '\n' ||
               input[position] == '\r' || input[position] == '\t')) ++position;
    }
    bool take(char c) {
        if (position < input.size() && input[position] == c) { ++position; return true; }
        return false;
    }
    void expect(char c) { if (!take(c)) fail(); }
    bool digit() const { return position < input.size() && input[position] >= '0' && input[position] <= '9'; }
    unsigned int hex4() {
        unsigned int result = 0;
        for (int i = 0; i < 4; ++i) {
            if (position == input.size()) fail();
            const char c = input[position++];
            result <<= 4;
            if (c >= '0' && c <= '9') result |= c - '0';
            else if (c >= 'a' && c <= 'f') result |= c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') result |= c - 'A' + 10;
            else fail();
        }
        return result;
    }
    std::string string() {
        expect('"');
        std::string result;
        while (position < input.size()) {
            const unsigned char c = input[position++];
            if (c == '"') return result;
            if (c < 0x20) fail();
            if (c != '\\') { result.push_back(c); continue; }
            if (position == input.size()) fail();
            switch (input[position++]) {
            case '"': result += '"'; break;
            case '\\': result += '\\'; break;
            case '/': result += '/'; break;
            case 'b': result += '\b'; break;
            case 'f': result += '\f'; break;
            case 'n': result += '\n'; break;
            case 'r': result += '\r'; break;
            case 't': result += '\t'; break;
            case 'u': {
                unsigned int code = hex4();
                if (code >= 0xd800 && code <= 0xdbff) {
                    expect('\\'); expect('u');
                    unsigned int low = hex4();
                    if (low < 0xdc00 || low > 0xdfff) fail();
                    code = 0x10000 + ((code - 0xd800) << 10) + low - 0xdc00;
                } else if (code >= 0xdc00 && code <= 0xdfff) fail();
                if (code <= 0x7f) result += static_cast<char>(code);
                else {
                    if (code > 0xffff) result += static_cast<char>(0xf0 | (code >> 18));
                    if (code > 0x7ff) result += static_cast<char>((code > 0xffff ? 0x80 : 0xe0) | ((code >> 12) & 0x3f));
                    result += static_cast<char>((code > 0x7ff ? 0x80 : 0xc0) | ((code >> 6) & 0x3f));
                    result += static_cast<char>(0x80 | (code & 0x3f));
                }
                break;
            }
            default: fail();
            }
        }
        fail();
    }
    JsonValue value(unsigned int depth) {
        if (depth > 256) fail();
        whitespace();
        if (position == input.size()) fail();
        if (input[position] == '"') return JsonValue(string());
        if (take('{')) {
            JsonValue result;
            result.type = JSON_OBJECT;
            result.value.object = new HashMap<JsonValue>;
            whitespace();
            if (take('}')) return result;
            do {
                whitespace();
                auto key = string();
                whitespace(); expect(':');
                (*result.value.object)[key.c_str()] = value(depth + 1);
                whitespace();
                if (take('}')) return result;
                expect(',');
            } while (true);
        }
        if (take('[')) {
            JsonValue result;
            result.type = JSON_ARRAY;
            result.value.array = new core::vector<JsonValue>;
            whitespace();
            if (take(']')) return result;
            do {
                result.value.array->Push(value(depth + 1));
                whitespace();
                if (take(']')) return result;
                expect(',');
            } while (true);
        }
        for (auto literal : {std::string_view("true"), std::string_view("false"), std::string_view("null")}) {
            if (input.substr(position, literal.size()) == literal) {
                position += literal.size();
                if (literal == "null") {
                    JsonValue result; result.type = JSON_NULL; result.value.null = nullptr; return result;
                }
                return JsonValue(literal == "true");
            }
        }
        const size_t start = position;
        take('-');
        if (!take('0')) {
            if (!digit()) fail();
            while (digit()) ++position;
        }
        bool floating = false;
        if (take('.')) {
            floating = true;
            if (!digit()) fail();
            while (digit()) ++position;
        }
        if (take('e') || take('E')) {
            floating = true;
            if (!take('+')) take('-');
            if (!digit()) fail();
            while (digit()) ++position;
        }
        const char* first = input.data() + start;
        const char* last = input.data() + position;
        if (!floating) {
            int number;
            const auto parsed = std::from_chars(first, last, number);
            if (parsed.ec == std::errc{} && parsed.ptr == last) return JsonValue(number);
        }
        double number;
        const auto parsed = std::from_chars(first, last, number);
        if (parsed.ec != std::errc{} || parsed.ptr != last || !std::isfinite(number)) fail();
        return JsonValue(number);
    }
public:
    explicit JsonReader(std::string_view text) : input(text) {}
    JsonValue parse() {
        auto result = value(0);
        whitespace();
        if (position != input.size()) fail();
        return result;
    }
};
}
