#include "helper.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>
#include <sstream>
// global variables
bool error_happened = false;
std::map<Node, std::vector<std::pair<Node, bool>>> graph;


// Node class
bool Node::operator<(const Node& other) const {
    if (id != other.id) {
        return id < other.id;
    }
    return name > other.name;
}

Node::Node(std::string v, std::string m, int t, bool b) : name(v), value(m), id(t), type(b) {}


// Parser class
Token Parser::currentToken() {
    if (currentIndex < tokens.size()) {
        return tokens[currentIndex];
    }
    return Token("", "EOF");
}

void Parser::eat(std::string expectedType) {
    if (currentToken().type == expectedType) {
        currentIndex++;
    } else {
        throw std::runtime_error("Syntax Error: Expected " + expectedType + ", found " + currentToken().type);
    }
}

void Parser::parseStmtSeq() {
    Node prv = parseStmt();
    root = prv;
    while (currentIndex < tokens.size()) {
        eat("SEMICOLON");
        Node cur = parseStmt();
        graph[prv].emplace_back(cur, 0);
        prv = cur;
    }
}

Node Parser::parseStmt() {
    if (currentToken().type == "IF") {
        return parseIfStmt();
    } else if (currentToken().type == "REPEAT") {
        return parseRepeatStmt();
    } else if (currentToken().type == "IDENTIFIER") {
        return parseAssignStmt();
    } else if (currentToken().type == "READ") {
        return parseReadStmt();
    } else if (currentToken().type == "WRITE") {
        return parseWriteStmt();
    } else {
        throw std::runtime_error("Syntax Error: Unexpected token " + currentToken().type);
    }
}

Node Parser::parseIfStmt() {
    Node if_node("if", "", currentIndex, true);
    eat("IF");
    Node left_child = parseExp();
    eat("THEN");
    graph[if_node].emplace_back(left_child, 1);
    Node right_child = parseStmt();
    graph[if_node].emplace_back(right_child, 1);
    Node prv(right_child);
    while (currentToken().type != "END" && currentToken().type != "ELSE") {
        eat("SEMICOLON");
        Node cur = parseStmt();
        graph[prv].emplace_back(cur, 0);
        prv = cur;
    }

    // Check for optional "ELSE" block
    if (currentToken().type == "ELSE") {
        Node else_node("else", "", currentIndex, true);
        eat("ELSE");
        Node left_else_node = parseStmt();
        graph[else_node].emplace_back(left_else_node, 1);
        Node prv_else(left_else_node);
        while (currentToken().type != "END") {
            eat("SEMICOLON");
            Node cur = parseStmt();
            graph[prv_else].emplace_back(cur, 0);
            prv_else = cur;
        }
        graph[if_node].emplace_back(else_node, 1);
    }
    eat("END");
    return if_node;
}

Node Parser::parseRepeatStmt() {
    Node repeat("Repeat", "", currentIndex, true);
    eat("REPEAT");
    Node left_child = parseStmt();
    graph[repeat].emplace_back(left_child, 1);
    Node prv(left_child);
    while (currentToken().type != "UNTIL") {
        eat("SEMICOLON");
        Node cur = parseStmt();
        graph[prv].emplace_back(cur, 0);
        prv = cur;
    }
    eat("UNTIL");
    Node right_child = parseExp();
    graph[repeat].emplace_back(right_child, 1);
    return repeat;
}

Node Parser::parseAssignStmt() {
    Node parent("Assign", currentToken().value, currentIndex, true);
    eat("IDENTIFIER");
    eat("ASSIGN");
    Node child = parseExp();
    graph[parent].emplace_back(child, 1);
    // if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
    return parent;
}

Node Parser::parseReadStmt() {
    eat("READ");
    Node read("read", currentToken().value, currentIndex - 1, true);
    eat("IDENTIFIER");
    // if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
    return read;
}

Node Parser::parseWriteStmt() {
    Node write("write", "", currentIndex, true);
    eat("WRITE");
    Node child("", "", -1, 0);
    if (currentToken().type == "IDENTIFIER") {
        child.name = "id";
        child.value = currentToken().value;
        child.id = currentIndex;
        child.type = false;
        eat("IDENTIFIER");
    }
    else if (currentToken().type == "NUMBER") {
        child.name = "const";
        child.value = currentToken().value;
        child.id = currentIndex;
        child.type = false;
        eat("NUMBER");
    }
    graph[write].emplace_back(child, 1);
    // if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
    return write;
}

Node Parser::parseExp() {
    Node left_node = parseSimpleExp();
    if (currentToken().type == "LESSTHAN" || currentToken().type == "EQUAL") {
        Node parent_node("op", currentToken().value, currentIndex, false);
        eat(currentToken().type);
        Node right_node = parseSimpleExp();
        graph[parent_node].emplace_back(left_node, 1);
        graph[parent_node].emplace_back(right_node, 1);
        return parent_node;
    }
    //if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
    return left_node;
}

Node Parser::parseSimpleExp() {
    Node left_node = parseTerm();
    if (currentToken().type == "PLUS" || currentToken().type == "MINUS") {
        Node parent_node("op", currentToken().value, currentIndex, false);
        eat(currentToken().type);
        Node right_node = parseExp();
        graph[parent_node].emplace_back(left_node, 1);
        graph[parent_node].emplace_back(right_node, 1);
        return parent_node;
    }
    //if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
    return left_node;
}



Node Parser::parseTerm() {
    Node left_node = parseFactor();
    if (currentToken().type == "MULT" || currentToken().type == "DIV") {
        Node parent_node("op", currentToken().value, currentIndex, false);
        eat(currentToken().type);
        Node right_node = parseExp();
        graph[parent_node].emplace_back(left_node, 1);
        graph[parent_node].emplace_back(right_node, 1);
        return parent_node;
    }
    //if (currentToken().type == "SEMICOLON") eat("SEMICOLON");
    return left_node;
}

Node Parser::parseFactor() {
    if (currentToken().type == "NUMBER") {
        Node number_node("const", currentToken().value, currentIndex, false);
        eat("NUMBER");
        return number_node;
    } else if (currentToken().type == "IDENTIFIER") {
        Node id_node("id", currentToken().value, currentIndex, false);
        eat("IDENTIFIER");
        return id_node;
    } else if (currentToken().type == "OPENBRACKET") {
        eat("OPENBRACKET");
        Node expr_node = parseExp();
        eat("CLOSEDBRACKET");
        return expr_node;
    } else {
        throw std::runtime_error("Syntax Error: Unexpected token " + currentToken().type);
    }
}

Parser::Parser(std::vector<Token>& t) : tokens(t), currentIndex(0), root("", "", -1, false) {
}

Node Parser::getRoot() {

    return root;
}

void Parser::parse() {
    parseStmtSeq();
}


// Token class
Token::Token(std::string v, std::string t) : value(v), type(t) {}


// general functions
std::string getTokenType(std::string& token) {
    for (const auto& pair : TOKEN_PATTERNS) {
        const std::string& type = pair.first;
        const std::string& pattern = pair.second;


        if (token == pattern) {
            return type;
        }
    }

    if (!token.empty() && all_of(token.begin(), token.end(), ::isdigit)) {
        return "NUMBER";
    }

    if (!token.empty() && isalpha(token[0])) {
        if (all_of(token.begin(), token.end(), [](char ch) { return isalpha(ch) || ch == '_'; })) {
            return "IDENTIFIER";
        }
    }

    return "UNKNOWN";
}




std::vector<Token> tokenize(std::string code) {
    error_happened = false;
    std::vector<Token> tokens;
    std::string currentToken;
    bool matched = false;

    for (size_t i = 0; i < code.size(); ++i) {
        char ch = code[i];

        if (isspace(ch)) {
            if (!currentToken.empty()) {
                std::string type = getTokenType(currentToken);
                if(type == "UNKNOWN") {
                    error_happened = true;
                    return tokens;
                }
                if (type == "IDENTIFIER" && any_of(currentToken.begin(), currentToken.end(), ::isdigit)) {
                    tokens.push_back({ currentToken, "Error identifier can't have numbers" });
                }
                else {
                    tokens.push_back({ currentToken, type });
                }
                currentToken.clear();
            }
            continue;
        }

        if (ch == ':' && i + 1 < code.size() && code[i + 1] == '=') {
            if (!currentToken.empty()) {
                std::string type = getTokenType(currentToken);
                if (type == "IDENTIFIER" && any_of(currentToken.begin(), currentToken.end(), ::isdigit)) {
                    tokens.push_back({ currentToken, "Error identifier can't have numbers" });
                }
                else {
                    tokens.push_back({ currentToken, type });
                }
                currentToken.clear();
            }
            tokens.push_back({ ":=", "ASSIGN" });
            ++i;
            continue;
        }

        for (const auto& pair : TOKEN_PATTERNS) {
            const std::string& type = pair.first;
            const std::string& pattern = pair.second;

            if (pattern.size() == 1 && pattern[0] == ch) {
                if (!currentToken.empty()) {
                    std::string type = getTokenType(currentToken);
                    if (type == "IDENTIFIER" && any_of(currentToken.begin(), currentToken.end(), ::isdigit)) {
                        tokens.push_back({ currentToken, "Error identifier can't have numbers" });
                    }
                    else {
                        tokens.push_back({ currentToken, type });
                    }
                    currentToken.clear();
                }
                tokens.push_back({std::string(1, ch), type });
                matched = true;
                break;
            }
        }

        if (!matched) {
            currentToken += ch;
        }

        matched = false;
    }

    if (!currentToken.empty()) {
        std::string type = getTokenType(currentToken);
        if (type == "IDENTIFIER" && any_of(currentToken.begin(), currentToken.end(), ::isdigit)) {
            tokens.push_back({ currentToken, "Error identifier can't have numbers" });
        }
        else {
            tokens.push_back({ currentToken, type });
        }
    }

    return tokens;
}

std::string removeComments(std::string code) {
    std::string result = code;
    size_t openBrace = result.find('{');
    size_t closeBrace = result.find('}');

    while (openBrace != std::string::npos && closeBrace != std::string::npos) {
        if (openBrace < closeBrace) {
            result.erase(openBrace, closeBrace - openBrace + 1);
            openBrace = result.find('{');
            closeBrace = result.find('}');
        } else {
            error_happened = true;
            return result;
        }
    }

    if (openBrace != std::string::npos) {
        error_happened = true;
    }

    return result;
}

void showError(QString error, QWidget *parent) {
    QMessageBox::critical(parent, "Error", error);
}

std::vector<Token> parseTokens(std::string &input) {
    std::vector<Token> tokens;
    std::istringstream stream(input);
    std::string line;

    while (std::getline(stream, line)) {
        // Remove spaces in the line
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

        // Find the comma
        size_t commaPos = line.find(',');

        if (commaPos != std::string::npos) {
            // Extract the value and type
            std::string value = line.substr(0, commaPos);
            std::string type = line.substr(commaPos + 1);

            // Push the token into the vector
            tokens.push_back({value, type});
        }
    }

    return tokens;
}



Node getNodes(QString &code,QString &Stokens) {
    try {
        error_happened = false;
        std::string strCode = removeComments(code.toStdString());
        std::string strTokens = Stokens.toStdString();
        if(error_happened) {
            throw std::runtime_error("Invalid comment structure");
        }
        std::vector<Token> tokens = tokenize(strCode);
        if(error_happened) {
            throw std::runtime_error("Invalid token");
        }
        if(tokens.empty())
            tokens = parseTokens(strTokens);

        Parser parser(tokens);
        parser.parse();
        return parser.getRoot();
    }
    catch(const QString &error){
        throw error;
    }
}
