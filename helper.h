#ifndef HELPER_H
#define HELPER_H
#include <QString>
#include <QStringList>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>

// class node
struct Node {
    std::string name;
    std::string value;
    int id;
    bool type;
    Node(std::string v,std::string m, int t, bool b);
    bool operator<(const Node& other) const;
};

// global variables
extern bool error_happened;
extern std::map<Node, std::vector<std::pair<Node, bool>>> graph;
const std::vector<std::pair<std::string, std::string>> TOKEN_PATTERNS = {
    {"SEMICOLON", R"(;)"},
    {"IF", "if"},
    {"ELSE","else"},
    {"THEN", "then"},
    {"END", "end"},
    {"REPEAT", "repeat"},
    {"UNTIL", "until"},
    {"READ", "read"},
    {"WRITE", "write"},
    {"ASSIGN", ":="},
    {"EQUAL", "="},
    {"LESSTHAN", "<"},
    {"PLUS", "+"},
    {"MINUS", "-"},
    {"MULT", "*"},
    {"DIV", "/"},
    {"OPENBRACKET", "("},
    {"CLOSEDBRACKET", ")"}
};



// class token
struct Token {
    std::string value;
    std::string type;
    Token(std::string v,std::string t);
};


