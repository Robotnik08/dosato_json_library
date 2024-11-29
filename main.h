#ifndef MAIN_H
#define MAIN_H

#include "dosato_source/dosato.h" // Include the dosato library API


// each library must have a function called init that initializes the functions
void init(void* vm);

// each library must have a dosatoFunctionMapList called functions
DosatoFunctionMapList functions;

Value json_to_string(ValueArray args, bool debug);
Value json_parse(ValueArray args, bool debug);


typedef enum {
    JSON_TokenType_ObjectStart,
    JSON_TokenType_ObjectEnd,
    JSON_TokenType_ArrayStart,
    JSON_TokenType_ArrayEnd,
    JSON_TokenType_Colon,
    JSON_TokenType_String,
    JSON_TokenType_Number,
    JSON_TokenType_Boolean,
    JSON_TokenType_Null,
    JSON_TokenType_Comma
} JSON_TokenType;

typedef enum {
    JSON_NodeType_Object,
    JSON_NodeType_Array,
    JSON_NodeType_String,
    JSON_NodeType_Number,
    JSON_NodeType_Boolean,
    JSON_NodeType_Null
} JSON_NodeType;

typedef struct {
    char* start;
    size_t length;
    JSON_TokenType type;
} JSON_Token;

char* ObjectToString (Value value, bool formatted);
char* ObjectToStringSafe (Value value, DosatoObject*** pointers, int count, bool formatted, int depth);
Value string_to_json(char* str, int length);
Value parse_tokens(JSON_Token* tokens, size_t count, size_t start, size_t end, JSON_NodeType type);



#endif // MAIN_H