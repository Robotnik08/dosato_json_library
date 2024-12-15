#include "main.h"

DosatoFunctionMapList functions;
void* main_vm;

void init(void* vm) {
    main_vm = vm;

    init_DosatoFunctionMapList(&functions);

    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap){"json_to_string", json_to_string});
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap){"json_parse", json_parse});
}

char* ObjectToString (Value value, bool formatted) {
    DosatoObject** pointers = malloc(sizeof(DosatoObject**));
    char* str = ObjectToStringSafe(value, &pointers, 0, formatted, 0);
    free(pointers);
    return str;
}

char* ObjectToStringSafe (Value value, DosatoObject*** pointers, int count, bool formatted, int depth) {
    char* string = malloc(1);
    string[0] = '\0';

    switch (value.type) {
        case TYPE_OBJECT: {
            for (size_t i = 0; i < count; i++) {
                if ((*pointers)[i] == value.as.objectValue) {
                    free(string);
                    return (char*)-1;
                }
            }
            (*pointers) = realloc(*pointers, (count + 1) * sizeof(DosatoObject**));
            (*pointers)[count++] = value.as.objectValue;

            string = realloc(string, strlen(string) + 2);
            strcat(string, "{");

            if (formatted) {
                string = realloc(string, strlen(string) + 10);
                strcat(string, "\n");
            }

            ValueObject* object = AS_OBJECT(value);
            char** keys = malloc(object->count * sizeof(char*));
            for (size_t i = 0; i < object->count; i++) {
                char* new_str = valueToString(object->keys[i], false);
                for (size_t j = 0; j < i; j++) {
                    if (strcmp(keys[j], new_str) == 0) {
                        free(new_str);
                        for (size_t k = 0; k < i; k++) {
                            free(keys[j]);
                        }
                        free(keys);
                        free(string);
                        return (char*)-2;
                    }
                }
                keys[i] = new_str;
                // Escape the string
                for (size_t j = 0; j < strlen(new_str); j++) {
                    if (new_str[j] == '\"' || new_str[j] == '\\' || new_str[j] == '\n' || new_str[j] == '\r' || new_str[j] == '\t') {
                        new_str = realloc(new_str, strlen(new_str) + 2);
                        memmove(new_str + j + 1, new_str + j, strlen(new_str) - j + 1);
                        new_str[j] = '\\';
                        j++;
                        switch (new_str[j]) {
                            case '\"': new_str[j] = '\"'; break;
                            case '\\': new_str[j] = '\\'; break;
                            case '\n': new_str[j] = 'n'; break;
                            case '\r': new_str[j] = 'r'; break;
                            case '\t': new_str[j] = 't'; break;
                        }
                    }
                }

                if (formatted) {
                    for (int j = 0; j < depth + 1; j++) {
                        string = realloc(string, strlen(string) + 10);
                        strcat(string, "\t");
                    }
                }

                string = realloc(string, strlen(string) + strlen(new_str) + 5);
                strcat(string, "\"");
                strcat(string, new_str);
                strcat(string, "\":");
                if (formatted) {
                    string = realloc(string, strlen(string) + 10);
                    strcat(string, " ");
                }

                char* valueString = ObjectToStringSafe(object->values[i], pointers, count, formatted, depth + 1);
                if ((long long int)valueString < 0) {
                    for (size_t j = 0; j <= i; j++) {
                        free(keys[j]);
                    }
                    free(keys);
                    return valueString;
                }
                string = realloc(string, strlen(string) + strlen(valueString) + 4);
                strcat(string, valueString);
                if (i < object->count - 1) {
                    strcat(string, ",");
                }
                free(valueString);
                
                if (formatted) {
                    string = realloc(string, strlen(string) + 10);
                    strcat(string, "\n");
                }
            }
            if (formatted) {
                for (int j = 0; j < depth; j++) {
                    string = realloc(string, strlen(string) + 10);
                    strcat(string, "\t");
                }
            }

            string = realloc(string, strlen(string) + 2);
            strcat(string, "}");
            for (size_t i = 0; i < object->count; i++) {
                free(keys[i]);
            }
            free(keys);
            break;
        }

        case TYPE_ARRAY: {
            for (size_t i = 0; i < count; i++) {
                if ((*pointers)[i] == value.as.objectValue) {
                    free(string);
                    return (char*)-2;
                }
            }
            (*pointers) = realloc(*pointers, (count + 1) * sizeof(DosatoObject**));
            (*pointers)[count++] = value.as.objectValue;

            string = realloc(string, strlen(string) + 2);
            strcat(string, "[");
            ValueArray* array = AS_ARRAY(value);
            for (size_t i = 0; i < array->count; i++) {
                char* valueString = ObjectToStringSafe(array->values[i], pointers, count, formatted, depth);
                if ((long long int)valueString < 0) {
                    return valueString;
                }
                string = realloc(string, strlen(string) + strlen(valueString) + 4);
                strcat(string, valueString);
                if (i < array->count - 1) {
                    strcat(string, ",");
                    if (formatted) {
                        string = realloc(string, strlen(string) + 10);
                        strcat(string, " ");
                    }
                }
                free(valueString);
            }
            string = realloc(string, strlen(string) + 2);
            strcat(string, "]");
            break;
        }

        case TYPE_FUNCTION: {
            string = realloc(string, strlen(string) + 18);
            strcat(string, "\"<function>\"");
            break;
        }

        case TYPE_UBYTE: {
            string = realloc(string, 8);
            sprintf(string, "%u", AS_UBYTE(value));
            break;
        }
        case TYPE_BYTE: {
            string = realloc(string, 8);
            sprintf(string, "%d", AS_BYTE(value));
            break;
        }
        case TYPE_USHORT: {
            string = realloc(string, 8);
            sprintf(string, "%u", AS_USHORT(value));
            break;
        }
        case TYPE_SHORT: {
            string = realloc(string, 8);
            sprintf(string, "%d", AS_SHORT(value));
            break;
        }
        case TYPE_UINT: {
            string = realloc(string, 16);
            sprintf(string, "%u", AS_UINT(value));
            break;
        }
        case TYPE_INT: {
            string = realloc(string, 16);
            sprintf(string, "%d", AS_INT(value));
            break;
        }
        case TYPE_ULONG: {
            string = realloc(string, 24);
            sprintf(string, "%llu", AS_ULONG(value));
            break;
        }
        case TYPE_LONG: {
            string = realloc(string, 24);
            sprintf(string, "%lld", AS_LONG(value));
            break;
        }
        case TYPE_STRING: {
            char* new_str = COPY_STRING(AS_STRING(value));
            
            // Escape the string
            for (size_t i = 0; i < strlen(new_str); i++) {
                if (new_str[i] == '\"' || new_str[i] == '\\' || new_str[i] == '\n' || new_str[i] == '\r' || new_str[i] == '\t') {
                    new_str = realloc(new_str, strlen(new_str) + 2);
                    memmove(new_str + i + 1, new_str + i, strlen(new_str) - i + 1);
                    new_str[i] = '\\';
                    i++;
                    switch (new_str[i]) {
                        case '\"': new_str[i] = '\"'; break;
                        case '\\': new_str[i] = '\\'; break;
                        case '\n': new_str[i] = 'n'; break;
                        case '\r': new_str[i] = 'r'; break;
                        case '\t': new_str[i] = 't'; break;
                    }
                }
            }

            string = realloc(string, strlen(new_str) + 3);
            strcat(string, "\"");
            strcat(string, new_str);
            strcat(string, "\"");

            free(new_str);
            break;
        }
        case TYPE_CHAR: {
            string = realloc(string, 4);
            sprintf(string, "\"%c\"", AS_CHAR(value));
            break;
        }
        case TYPE_FLOAT: {
            if (isnan(AS_FLOAT(value))) {
                string = realloc(string, 6);
                sprintf(string, "\"NaN\"");
                break;
            } else if (isinf(AS_FLOAT(value))) {
                if (AS_FLOAT(value) < 0) {
                    string = realloc(string, 12);
                    sprintf(string, "\"-infinity\"");
                } else {
                    string = realloc(string, 11);
                    sprintf(string, "\"infinity\"");
                }
                break;
            }

            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%.15f", AS_FLOAT(value));
            
            char *dot = strchr(buffer, '.');
            if (dot != NULL) {
                char *end = dot + strlen(dot) - 1;
                while (end > dot && *end == '0') {
                    *end-- = '\0';
                }
                if (*end == '.') {
                    *end = '\0';
                }
            }

            string = realloc(string, strlen(buffer) + 1);
            strcpy(string, buffer);
            break;
        }
        case TYPE_DOUBLE: {
            if (isnan(AS_DOUBLE(value))) {
                string = realloc(string, 6);
                sprintf(string, "\"NaN\"");
                break;
            } else if (isinf(AS_DOUBLE(value))) {
                if (AS_DOUBLE(value) < 0) {
                    string = realloc(string, 12);
                    sprintf(string, "\"-infinity\"");
                } else {
                    string = realloc(string, 11);
                    sprintf(string, "\"infinity\"");
                }
                break;
            }

            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%.15f", AS_DOUBLE(value));
            
            char *dot = strchr(buffer, '.');
            if (dot != NULL) {
                char *end = dot + strlen(dot) - 1;
                while (end > dot && *end == '0') {
                    *end-- = '\0';
                }
                if (*end == '.') {
                    *end = '\0';
                }
            }

            string = realloc(string, strlen(buffer) + 1);
            strcpy(string, buffer);
            
            break;
        }
        case TYPE_BOOL: {
            string = realloc(string, 6);
            sprintf(string, "%s", AS_BOOL(value) ? "true" : "false");
            break;
        }

        case D_NULL: {
            string = realloc(string, 6);
            sprintf(string, "null");
            break;
        }

        default: {
            string = realloc(string, 16);
            sprintf(string, "<unknown type>");
            break;
        }
    }

    return string;
}

Value json_to_string(ValueArray args, bool debug) {
    if (args.count > 2 || args.count == 0) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    bool formatted = false;
    if (args.count == 2) {
        Value formatted_val = GET_ARG(args, 1);
        CAST_SAFE(formatted_val, TYPE_BOOL);
        formatted = AS_BOOL(formatted_val);
    }

    Value json_obj = GET_ARG(args, 0);
    if (json_obj.type != TYPE_OBJECT) {
        return BUILD_EXCEPTION(E_INVALID_TYPE);
    }

    char* str = ObjectToString(json_obj, formatted);

    if (str == (char*)-1) {
        PRINT_ERROR("%s", "Attempting to parse circular reference.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    } else if (str == (char*)-2) {
        PRINT_ERROR("%s", "String representation of key is found twice.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    return BUILD_STRING(str);
}

Value string_to_json(char* str, int length) {
    if (length == 0) {
        return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    }

    JSON_Token* tokens = malloc(sizeof(JSON_Token));
    size_t token_count = 0;

    #define ADD_TOKEN(type, length) do { \
        tokens = realloc(tokens, (token_count + 1) * sizeof(JSON_Token)); \
        tokens[token_count++] = (JSON_Token){str + i, length, type}; \
    } while (0)


    // tokenize the string
    for (size_t i = 0; i < length; i++) {
        switch (str[i]) {
            case '{': ADD_TOKEN(JSON_TokenType_ObjectStart, 1); break;
            case '}': ADD_TOKEN(JSON_TokenType_ObjectEnd, 1); break;
            case '[': ADD_TOKEN(JSON_TokenType_ArrayStart, 1); break;
            case ']': ADD_TOKEN(JSON_TokenType_ArrayEnd, 1); break;
            case ':': ADD_TOKEN(JSON_TokenType_Colon, 1); break;
            case ',': ADD_TOKEN(JSON_TokenType_Comma, 1); break;
            case '\"': {
                size_t j = i + 1;
                while (j < length && str[j] != '\"') {
                    if (str[j] == '\\') {
                        j++;
                    }
                    j++;
                }
                if (j == length) {
                    return BUILD_EXCEPTION(E_UNCLOSED_STRING_LITERAL);
                }
                ADD_TOKEN(JSON_TokenType_String, j - i + 1);
                i = j;
                break;
            }
            case 't': {
                if (i + 4 <= length && strncmp(str + i, "true", 4) == 0) {
                    ADD_TOKEN(JSON_TokenType_Boolean, 4);
                    i += 3;
                } else {
                    return BUILD_EXCEPTION(E_INVALID_EXPRESSION);
                }
                break;
            }
            case 'f': {
                if (i + 5 <= length && strncmp(str + i, "false", 5) == 0) {
                    ADD_TOKEN(JSON_TokenType_Boolean, 5);
                    i += 4;
                } else {
                    return BUILD_EXCEPTION(E_INVALID_EXPRESSION);
                }
                break;
            }
            case 'n': {
                if (i + 4 <= length && strncmp(str + i, "null", 4) == 0) {
                    ADD_TOKEN(JSON_TokenType_Null, 4);
                    i += 3;
                } else {
                    return BUILD_EXCEPTION(E_INVALID_EXPRESSION);
                }
                break;
            }
            case ' ': case '\t': case '\n': case '\r': break;
            default: {
                size_t j = i;
                while (j < length && (str[j] == '-' || (str[j] >= '0' && str[j] <= '9') || str[j] == '.')) {
                    j++;
                }
                if (j == i) {
                    return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
                }
                ADD_TOKEN(JSON_TokenType_Number, j - i);
                i = j - 1;
                break;
            }
        }
    }

    #undef ADD_TOKEN

    // parse the tokens

    // first type
    JSON_NodeType type = JSON_NodeType_Object;
    switch(tokens[0].type) {
        case JSON_TokenType_ObjectStart: {
            type = JSON_NodeType_Object;
            break;
        }
        case JSON_TokenType_ArrayStart: {
            type = JSON_NodeType_Array;
            break;
        }
        case JSON_TokenType_String: {
            type = JSON_NodeType_String;
            if (token_count != 1) {
                return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
            }
            break;
        }
        case JSON_TokenType_Number: {
            type = JSON_NodeType_Number;
            if (token_count != 1) {
                return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
            }
            break;
        }
        case JSON_TokenType_Boolean: {
            type = JSON_NodeType_Boolean;
            if (token_count != 1) {
                return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
            }
            break;
        }
        case JSON_TokenType_Null: {
            type = JSON_NodeType_Null;
            if (token_count != 1) {
                return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
            }
            break;
        }
        default: {
            return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
        }
    }

    
    Value value = parse_tokens(tokens, token_count, 0, token_count - 1, type);

    return value;
}

#define SKIP_BLOCK(i) do { \
    size_t block_count = 1; \
    JSON_TokenType block_type = tokens[i].type; \
    if (block_type == JSON_TokenType_ObjectStart) { \
        block_type = JSON_TokenType_ObjectEnd; \
    } else if (block_type == JSON_TokenType_ArrayStart) { \
        block_type = JSON_TokenType_ArrayEnd; \
    } else { \
        break; \
    } \
    while (block_count > 0) { \
        i++; \
        if (i == count) { \
            return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN); \
        } \
        if (tokens[i].type == JSON_TokenType_ObjectStart || tokens[i].type == JSON_TokenType_ArrayStart) { \
            block_count++; \
        } else if (tokens[i].type == JSON_TokenType_ObjectEnd || tokens[i].type == JSON_TokenType_ArrayEnd) { \
            block_count--; \
        } \
    } \
    if (tokens[i].type != block_type) { \
        return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN); \
    } \
} while (0)

Value parse_tokens(JSON_Token* tokens, size_t count, size_t start, size_t end, JSON_NodeType type) {
    if (count == 0) {
        PRINT_ERROR("%s", "Empty Json String.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    size_t i = start;
    if (i == count) {
        return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    }

    switch (type) {
        case JSON_NodeType_Array: {
            // check if array is correct
            if (tokens[i].type != JSON_TokenType_ArrayStart) {
                return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
            }

            ValueArray* array = malloc(sizeof(ValueArray));
            init_ValueArray(array);

            if (tokens[i + 1].type == JSON_TokenType_ArrayEnd) {
                return BUILD_ARRAY(array);
            }

            while (i < end - 1) {
                // find comma or end of array
                size_t j = i + 1;
                while (j < count && tokens[j].type != JSON_TokenType_Comma && tokens[j].type != JSON_TokenType_ArrayEnd) {
                    SKIP_BLOCK(j);
                    j++;
                }

                if (j == i + 1) {
                    return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
                }

                JSON_NodeType value_type = JSON_NodeType_Object;
                switch(tokens[i + 1].type) {
                    case JSON_TokenType_ObjectStart: {
                        value_type = JSON_NodeType_Object;
                        break;
                    }
                    case JSON_TokenType_ArrayStart: {
                        value_type = JSON_NodeType_Array;
                        break;
                    }
                    case JSON_TokenType_String: {
                        value_type = JSON_NodeType_String;
                        break;
                    }
                    case JSON_TokenType_Number: {
                        value_type = JSON_NodeType_Number;
                        break;
                    }
                    case JSON_TokenType_Boolean: {
                        value_type = JSON_NodeType_Boolean;
                        break;
                    }
                    case JSON_TokenType_Null: {
                        value_type = JSON_NodeType_Null;
                        break;
                    }
                    default: {
                        return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
                    }
                }

                Value value = parse_tokens(tokens, count, i + 1, j - 1, value_type);
                if (value.type == TYPE_EXCEPTION) {
                    return value;
                }
                write_ValueArray(array, value);

                i = j;
            }

        
            if (tokens[i].type != JSON_TokenType_ArrayEnd) {
                return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
            }

            return BUILD_ARRAY(array);
        }

        case JSON_NodeType_Object: {
            // check if object is correct
            if (tokens[i].type != JSON_TokenType_ObjectStart) {
                return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
            }

            if (tokens[i + 1].type == JSON_TokenType_ObjectEnd) {
                ValueObject* object = malloc(sizeof(ValueObject));
                init_ValueObject(object);
                return BUILD_OBJECT(object);
            }
                
            if (tokens[end].type != JSON_TokenType_ObjectEnd) {
                return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
            }

            ValueObject* object = malloc(sizeof(ValueObject));
            init_ValueObject(object);

            while (i < end - 2) {
                // find comma or end of object
                size_t j = i + 1;
                while (j < count && tokens[j].type != JSON_TokenType_Comma && tokens[j].type != JSON_TokenType_ObjectEnd) {
                    SKIP_BLOCK(j);
                    j++;
                }


                if (j == i) {
                    return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
                }

                
                if (tokens[i + 1].type != JSON_TokenType_String) {
                    return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
                }

                i++;
                char* key = malloc(tokens[i].length - 2);
                strncpy(key, tokens[i].start + 1, tokens[i].length - 2);
                key[tokens[i].length - 2] = '\0';

                i++;

                if (tokens[i].type != JSON_TokenType_Colon) {
                    return BUILD_EXCEPTION(E_EXPECTED_COLON_OPERATOR);
                }

                i++;

                JSON_NodeType value_type = JSON_NodeType_Object;
                switch(tokens[i].type) {
                    case JSON_TokenType_ObjectStart: {
                        value_type = JSON_NodeType_Object;
                        break;
                    }
                    case JSON_TokenType_ArrayStart: {
                        value_type = JSON_NodeType_Array;
                        break;
                    }
                    case JSON_TokenType_String: {
                        value_type = JSON_NodeType_String;
                        break;
                    }
                    case JSON_TokenType_Number: {
                        value_type = JSON_NodeType_Number;
                        break;
                    }
                    case JSON_TokenType_Boolean: {
                        value_type = JSON_NodeType_Boolean;
                        break;
                    }
                    case JSON_TokenType_Null: {
                        value_type = JSON_NodeType_Null;
                        break;
                    }
                    default: {
                        return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
                    }
                }
                
                Value value = parse_tokens(tokens, count, i, j - 1, value_type);
                if (value.type == TYPE_EXCEPTION) {
                    free(key);
                    return value;
                }

                write_ValueObject(object, BUILD_STRING(key), value);

                i = j;
            }

            if (tokens[i].type != JSON_TokenType_ObjectEnd) {
                return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
            }

            return BUILD_OBJECT(object);
        }

        case JSON_NodeType_String: {
            char* str = malloc(tokens[i].length - 2);
            strncpy(str, tokens[i].start + 1, tokens[i].length - 2);
            str[tokens[i].length - 2] = '\0';

            // replace escape characters
            for (size_t j = 0; j < strlen(str); j++) {
                if (str[j] == '\\') {
                    switch (str[j]) {
                        case '\"': str[j] = '\"'; break;
                        case '\\': str[j] = '\\'; break;
                        case 'n': str[j] = '\n'; break;
                        case 'r': str[j] = '\r'; break;
                        case 't': str[j] = '\t'; break;
                    }
                    // shift the string
                    memmove(str + j + 1, str + j + 2, strlen(str) - j - 1);
                }
            }

            return BUILD_STRING(str);
        }

        case JSON_NodeType_Number: {
            char* str = malloc(tokens[i].length);
            strncpy(str, tokens[i].start, tokens[i].length);
            str[tokens[i].length] = '\0';

            // check if token has a non-numeric character
            for (size_t j = 0; j < strlen(str); j++) {
                if (str[j] != '-' && str[j] != '.' && (str[j] < '0' || str[j] > '9')) {
                    return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
                }
            }

            double num = atof(str);

            return BUILD_DOUBLE(num);
        }

        case JSON_NodeType_Boolean: {
            if (tokens[i].length == 4) {
                return BUILD_BOOL(true);
            } else if (tokens[i].length == 5) {
                return BUILD_BOOL(false);
            }
        }

        case JSON_NodeType_Null: {
            return BUILD_NULL();
        }
    }
    

    return BUILD_NULL();
}

#undef SKIP_BLOCK

Value json_parse(ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value json_str = GET_ARG(args, 0);
    CAST_TO_STRING(json_str);

    char* str = COPY_STRING(AS_STRING(json_str));
    // loop through the string and parse it
    Value value = string_to_json(str, strlen(str));

    free(str);

    return value;
}