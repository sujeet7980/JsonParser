
#include <bits/stdc++.h>
using namespace std;

enum class JSONType {
    Null,
    Bool,
    String,
    Object,
    Array
};

class JSONValue;

using JSONObject = map<string, shared_ptr<JSONValue>>;

using JSONArray = vector<shared_ptr<JSONValue>>;

class JSONValue {
public:
    JSONType type;
    union {
        bool boolValue;
        string* stringValue;
        JSONObject* objectValue;
        JSONArray* arrayValue;
    };

    JSONValue() : type(JSONType::Null) {}

    JSONValue(bool b) : type(JSONType::Bool) {
        boolValue = b;
    }

    JSONValue(const string& s) : type(JSONType::String) {
        stringValue = new string(s);
    }

    JSONValue(const JSONObject& o) : type(JSONType::Object) {
        objectValue = new JSONObject(o);
    }

    JSONValue(const JSONArray& a) : type(JSONType::Array) {
        arrayValue = new JSONArray(a);
    }

    JSONValue(const JSONValue& other) {
        type = other.type;
        switch (type) {
            case JSONType::Null:
                break;
            case JSONType::Bool:
                boolValue = other.boolValue;
                break;
            case JSONType::String:
                stringValue = new string(*other.stringValue);
                break;
            case JSONType::Object:
                objectValue = new JSONObject(*other.objectValue);
                break;
            case JSONType::Array:
                arrayValue = new JSONArray(*other.arrayValue);
                break;
        }
    }


    bool isNull() const { return type == JSONType::Null; }
    bool isBool() const { return type == JSONType::Bool; }
    bool isString() const { return type == JSONType::String; }
    bool isObject() const { return type == JSONType::Object; }
    bool isArray() const { return type == JSONType::Array; }

    bool asBool() const {
        if (type == JSONType::Bool) return boolValue;
        throw runtime_error("Not a bool type");
    }

    const string& asString() const {
        if (type == JSONType::String) return *stringValue;
        throw runtime_error("Not a string type");
    }

    const JSONObject& asObject() const {
        if (type == JSONType::Object) return *objectValue;
        throw runtime_error("Not an object type");
    }

    const JSONArray& asArray() const {
        if (type == JSONType::Array) return *arrayValue;
        throw runtime_error("Not an array type");
    }
};

shared_ptr<JSONValue> parseValue(const string& json, size_t& pos);
shared_ptr<JSONValue> parseObject(const string& json, size_t& pos);
shared_ptr<JSONValue> parseArray(const string& json, size_t& pos);
string parseString(const string& json, size_t& pos);
string parseNumber(const string& json, size_t& pos);
void skipWhitespace(const string& json, size_t& pos);

shared_ptr<JSONValue> parseJSON(const string& json) {
    size_t pos = 0;
    skipWhitespace(json, pos);
    auto result = parseValue(json, pos);
    skipWhitespace(json, pos);
    if (pos != json.size()) {
        throw runtime_error("Unexpected characters at the end of JSON input.");
    }
    return result;
}

// Function to skip whitespace
void skipWhitespace(const string& json, size_t& pos) {
    while (pos < json.size() && isspace(json[pos])) {
        ++pos;
    }
}

shared_ptr<JSONValue> parseValue(const string& json, size_t& pos) {
    skipWhitespace(json, pos);
    if (pos >= json.size()) {
        throw runtime_error("Unexpected end of JSON input.");
    }

    char ch = json[pos];
    if (ch == 'n') {
        pos += 4; // Skip "null"
        return make_shared<JSONValue>();
    } else if (ch == 't') {
        pos += 4; // Skip "true"
        return make_shared<JSONValue>(true);
    } else if (ch == 'f') {
        pos += 5; // Skip "false"
        return make_shared<JSONValue>(false);
    } else if (ch == '"') {
        return make_shared<JSONValue>(parseString(json, pos));
    } else if (ch == '{') {
        return parseObject(json, pos);
    } else if (ch == '[') {
        return parseArray(json, pos);
    } else if (isdigit(ch) || ch == '-') {
        return make_shared<JSONValue>(parseNumber(json, pos));
    } else {
        throw runtime_error(string("Unexpected character in JSON: ") + ch);
    }
}

shared_ptr<JSONValue> parseObject(const string& json, size_t& pos) {
    JSONObject obj;
    ++pos; // Skip start
    skipWhitespace(json, pos);
    if (json[pos] == '}') {
        ++pos;
        return make_shared<JSONValue>(obj);
    }

    while (true) {
        skipWhitespace(json, pos);
        string key = parseString(json, pos);
        skipWhitespace(json, pos);

        if (json[pos] != ':') {
            throw runtime_error("Expected ':' in JSON object.");
        }
        ++pos;
        skipWhitespace(json, pos);

        obj[key] = parseValue(json, pos);
        skipWhitespace(json, pos);

        if (json[pos] == '}') {
            ++pos;
            break;
        }

        if (json[pos] != ',') {
            throw runtime_error("Expected ',' in JSON object.");
        }
        ++pos;
    }

    return make_shared<JSONValue>(obj);
}

shared_ptr<JSONValue> parseArray(const string& json, size_t& pos) {
    JSONArray arr;
    ++pos; // Skip starting
    skipWhitespace(json, pos);
    if (json[pos] == ']') {
        ++pos;
        return make_shared<JSONValue>(arr);
    }

    while (true) {
        skipWhitespace(json, pos);
        arr.push_back(parseValue(json, pos));
        skipWhitespace(json, pos);

        if (json[pos] == ']') {
            ++pos;
            break;
        }

        if (json[pos] != ',') {
            throw runtime_error("Expected ',' in JSON array.");
        }
        ++pos;
    }

    return make_shared<JSONValue>(arr);
}

string parseString(const string& json, size_t& pos) {
    string result;
    ++pos; // Skip initial '"'

    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\') {
            ++pos;
            if (pos >= json.size()) {
                throw runtime_error("Unexpected end of JSON string.");
            }
            switch (json[pos]) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                default: throw runtime_error("Invalid escape sequence in JSON string.");
            }
        } else {
            result += json[pos];
        }
        ++pos;
    }

    if (pos >= json.size() || json[pos] != '"') {
        throw runtime_error("Unterminated JSON string.");
    }

    ++pos; // Skip closing 
    return result;
}

string parseNumber(const string& json, size_t& pos) {
    size_t start = pos;
    if (json[pos] == '-') ++pos;
    while (pos < json.size() && isdigit(json[pos])) ++pos;
    if (pos < json.size() && json[pos] == '.') {
        ++pos;
        while (pos < json.size() && isdigit(json[pos])) ++pos;
    }
    if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
        ++pos;
        if (json[pos] == '+' || json[pos] == '-') ++pos;
        while (pos < json.size() && isdigit(json[pos])) ++pos;
    }
    return json.substr(start, pos - start);
}

void printJSONValue(const shared_ptr<JSONValue>& value, int indent = 0) {
    if (!value) {
        cout << "null";
        return;
    }

    switch (value->type) {
        case JSONType::Null:
            cout << "null";
            break;
        case JSONType::Bool:
            cout << (value->asBool() ? "true" : "false");
            break;
        case JSONType::String:
            cout << "\"" << value->asString() << "\"";
            break;
        case JSONType::Object: {
            cout << "{\n";
            const auto& obj = value->asObject();
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                cout << string(indent + 2, ' ') << "\"" << it->first << "\": ";
                printJSONValue(it->second, indent + 2);
                if (next(it) != obj.end()) {
                    cout << ",";
                }
                cout << "\n";
            }
            cout << string(indent, ' ') << "}";
            break;
        }
        case JSONType::Array: {
            cout << "[\n";
            const auto& arr = value->asArray();
            for (size_t i = 0; i < arr.size(); ++i) {
                cout << string(indent + 2, ' ');
                printJSONValue(arr[i], indent + 2);
                if (i + 1 < arr.size()) {
                    cout << ",";
                }
                cout << "\n";
            }
            cout << string(indent, ' ') << "]";
            break;
        }
    }
}

int main() {
    string jsonStr = R"({
        "name": "Sujeet kumar",
        "age": 30,
        "scores": [90, 85, 88],
        "address": {
            "state": "Uttar Pradesh",
            "city": "Agra"
        }
    })";

    try {
        auto jsonValue = parseJSON(jsonStr);
        printJSONValue(jsonValue, 0);
        cout << endl;
    } catch (const exception& e) {
        cerr << "Error parsing JSON: " << e.what() << endl;
    }

    return 0;
}
