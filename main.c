#include "main.h"

DosatoFunctionMapList functions;
void* main_vm;

void init(void* vm) {
    main_vm = vm;

    init_DosatoFunctionMapList(&functions);

    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap){"json_to_string", json_to_string});
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap){"json_parse", json_parse});
}

char* ObjectToString (Value value) {
    DosatoObject** pointers = malloc(sizeof(DosatoObject**));
    char* str = ObjectToStringSafe(value, &pointers, 0);
    free(pointers);
    return str;
}

char* ObjectToStringSafe (Value value, DosatoObject*** pointers, int count) {
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
            ValueObject* object = AS_OBJECT(value);
            for (size_t i = 0; i < object->count; i++) {
                char* new_str = COPY_STRING(object->keys[i]);

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

                string = realloc(string, strlen(string) + strlen(new_str) + 5);
                strcat(string, "\"");
                strcat(string, new_str);
                strcat(string, "\":");
                free(new_str);

                char* valueString = ObjectToStringSafe(object->values[i], pointers, count);
                if (valueString == (char*)-1) {
                    return (char*)-1;
                }
                string = realloc(string, strlen(string) + strlen(valueString) + 3);
                strcat(string, valueString);
                if (i < object->count - 1) {
                    strcat(string, ",");
                }
                free(valueString);
            }
            string = realloc(string, strlen(string) + 2);
            strcat(string, "}");
            break;
        }

        case TYPE_ARRAY: {
            for (size_t i = 0; i < count; i++) {
                if ((*pointers)[i] == value.as.objectValue) {
                    free(string);
                    return (char*)-1;
                }
            }
            (*pointers) = realloc(*pointers, (count + 1) * sizeof(DosatoObject**));
            (*pointers)[count++] = value.as.objectValue;

            string = realloc(string, strlen(string) + 2);
            strcat(string, "[");
            ValueArray* array = AS_ARRAY(value);
            for (size_t i = 0; i < array->count; i++) {
                char* valueString = ObjectToStringSafe(array->values[i], pointers, count);
                if (valueString == (char*)-1) {
                    return (char*)-1;
                }
                string = realloc(string, strlen(string) + strlen(valueString) + 3);
                strcat(string, valueString);
                if (i < array->count - 1) {
                    strcat(string, ",");
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
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value json_obj = GET_ARG(args, 0);
    if (json_obj.type != TYPE_OBJECT) {
        return BUILD_EXCEPTION(E_INVALID_TYPE);
    }

    char* str = ObjectToString(json_obj);

    if (str == (char*)-1) {
        PRINT_ERROR("%s", "Attempting to parse circular reference.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    return BUILD_STRING(str);
}

Value string_to_json(char* str, int length) {
    if (length == 0) {
        return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    }

    // // loop through the string and parse it
    // // check first character
    // while (str[0] == ' ' || str[0] == '\n' || str[0] == '\r' || str[0] == '\t') {
    //     str++;
    //     length--;
    // }

    // #define SKIP_UNTIL_NON_WHITESPACE(i) \
    //     do { \
    //         while (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t') { \
    //             i++; \
    //         } \
    //     } while (0)

    // #define SKIP_BLOCK(i) \
    //     do { \
    //         int block_count = 1; \
    //         char block_type = str[i]; \
    //                 printf("%c, %d\n", str[i], i); \
    //         switch (block_type) { \
    //             case '{': block_type = '}'; break; \
    //             case '[': block_type = ']'; break; \
    //             default: block_type = '\0'; break; \
    //         } \
    //         if (block_type == '\0') { \
    //             break; \
    //         } \
    //         int start = i; \
    //         int escape_count = 0; \
    //         bool in_string = false; \
    //         for (int j = start + 1; j < length; j++) { \
    //             if (str[j] == block_type) { \
    //                 block_count--; \
    //                 if (block_count == 0) { \
    //                     i = j + 1; \
    //                     break; \
    //                 } \
    //             } else if (str[j] == str[start]) { \
    //                 block_count++; \
    //             } \
    //         } \
    //     } while (0)

    // switch (str[0]) {
    //     case '{': { // object
    //         ValueObject* object = malloc(sizeof(ValueObject));
    //         init_ValueObject(object);

    //         int i = 1;
    //         for (; i < length; i++) {
    //             SKIP_UNTIL_NON_WHITESPACE(i);
    //             if (str[i] == '\"') {
    //                 // key
    //                 char* key = NULL;
    //                 int escape_count = 0;
    //                 for (int j = i + 1; j < length; j++) {
    //                     if (str[j] == '\"' && escape_count % 2 == 0) {
    //                         key = malloc(j - i + 1);
    //                         strncpy(key, str + i, j - i);
    //                         key[j - i] = '\0';
    //                         i = j + 1;
    //                         break;
    //                     } else if (str[j] == '\\') {
    //                         escape_count++;
    //                     }
    //                 }
    //                 if (key == NULL) {
    //                     return BUILD_EXCEPTION(E_UNCLOSED_STRING_LITERAL);
    //                 }

    //                 SKIP_UNTIL_NON_WHITESPACE(i);
    //                 if (str[i++] != ':') {
    //                     return BUILD_EXCEPTION(E_EXPECTED_COLON_OPERATOR);
    //                 }
    //                 SKIP_UNTIL_NON_WHITESPACE(i);
                    
    //                 int start_value = i;
    //                 for (int j = i; j < length; j++) {
    //                     SKIP_BLOCK(j);
    //                     printf("123: %c, %d\n", str[j], j);
    //                     if (str[j] == ',' || str[j] == '}') {
    //                         Value value = string_to_json(str + start_value, j - start_value);
    //                         if (value.type == TYPE_EXCEPTION) {
    //                             return value;
    //                         }
    //                         write_ValueObject(object, key, value);
    //                         // add key value pair to object
    //                         i = j;
    //                         break;
    //                     }
    //                 }
    //             } else {
    //                 printf("%c, %d\n", str[i], i);
    //                 return BUILD_EXCEPTION(E_EXPECTED_STRING);
    //             }
    //             if (str[i] == '}') {
    //                 break;
    //             }
    //         }

    //         // check if theres anything after the object
    //         i++;
    //         for (;i < length; i++) {
    //             if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t') {
    //                 return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //             }
    //         }
            
    //         return BUILD_OBJECT(object);
    //     }
    //     case '[': { // array
    //         ValueArray* array = malloc(sizeof(ValueArray));
    //         init_ValueArray(array);
    //         int i = 1;
    //         for (; i < length; i++) {
    //             SKIP_UNTIL_NON_WHITESPACE(i);
    //             int start_value = i;
    //             for (int j = i; j < length; j++) {
    //                 SKIP_BLOCK(j);
    //                 if (str[j] == ',' || str[j] == ']') {
    //                     Value value = string_to_json(str + start_value, j - start_value);
    //                     if (value.type == TYPE_EXCEPTION) {
    //                         return value;
    //                     }
    //                     write_ValueArray(array, value);
    //                     // add value to array
    //                     i = j;
    //                     break;
    //                 }
    //             }
    //             if (str[i] == ']') {
    //                 break;
    //             }
    //         }

    //         // check if theres anything after the array
    //         for (; i < length; i++) {
    //             if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t') {
    //                 return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //             }
    //         }

    //         return BUILD_ARRAY(array);

    //         break;
    //     }
    //     case '\"': { // string
    //         char* value = NULL;
    //         int escape_count = 0;
    //         int i = 1;
    //         for (; i < length; i++) {
    //             if (str[i] == '\"' && escape_count % 2 == 0) {
    //                 value = malloc(i - 1);
    //                 strncpy(value, str + 1, i - 1);
    //                 value[i - 1] = '\0';
    //                 i++;
    //                 break;
    //             } else if (str[i] == '\\') {
    //                 escape_count++;
    //             }
    //         }
    //         if (value == NULL) {
    //             return BUILD_EXCEPTION(E_UNCLOSED_STRING_LITERAL);
    //         }

    //         // check if theres anything after the string
    //         for (; i < length; i++) {
    //             if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t') {
    //                 return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //             }
    //         }

    //         return BUILD_STRING(value);
    //         break;
    //     }
    //     case 't': { // true
    //         if (length < 4) {
    //             return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //         }
    //         if (strncmp(str, "true", 4) != 0) {
    //             return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //         }
            
    //         // check if theres anything after the string
    //         for (int i = 4; i < length; i++) {
    //             if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t') {
    //                 return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //             }
    //         }

    //         return BUILD_BOOL(true);
    //     }
    //     case 'f': { // false
    //         if (length < 5) {
    //             return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //         }
    //         if (strncmp(str, "false", 5) != 0) {
    //             return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //         }

    //         // check if theres anything after the string
    //         for (int i = 5; i < length; i++) {
    //             if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t') {
    //                 return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //             }
    //         }

    //         return BUILD_BOOL(false);
    //     }
    //     case 'n': { // null
    //         if (length < 4) {
    //             return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //         }

    //         if (strncmp(str, "null", 4) != 0) {
    //             return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //         }

    //         // check if theres anything after the string
    //         for (int i = 4; i < length; i++) {
    //             if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t') {
    //                 return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //             }
    //         }

    //         return BUILD_NULL();
    //     }
    //     case '0':
    //     case '1':
    //     case '2':
    //     case '3':
    //     case '4':
    //     case '5':
    //     case '6':
    //     case '7':
    //     case '8':
    //     case '9':
    //     case '-': { // number
    //         if (length == 0) {
    //             return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //         }

    //         char* value = NULL;

    //         for (int i = 0; i < length; i++) {
    //             if (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t') {
    //                 value = malloc(i);
    //                 strncpy(value, str, i);
    //                 value[i] = '\0';
    //                 break;
    //             }
    //         }

    //         if (value == NULL) {
    //             return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //         }

    //         // check if theres anything after the string
    //         for (int i = 0; i < length; i++) {
    //             if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t') {
    //                 return BUILD_EXCEPTION(E_UNEXPECTED_TOKEN);
    //             }
    //         }

    //         if (strchr(value, '.') != NULL) {
    //             return BUILD_DOUBLE(atof(value));
    //         }

    //         return BUILD_LONG(atoll(value));
    //     }
    // }

    return BUILD_NULL();
}

Value json_parse(ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value json_str = GET_ARG(args, 0);
    CAST_TO_STRING(json_str);

    char* str = AS_STRING(json_str);
    // loop through the string and parse it
    Value value = string_to_json(str, strlen(str));

    return value;
}