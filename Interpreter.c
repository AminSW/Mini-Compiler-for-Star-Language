#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VAR_COUNT 100
#define MAX_STR_LEN 256
#define MAX_INT 99999999

typedef enum { INT, TEXT } VarType;

typedef struct {
    char name[32];
    VarType type;
    union {
        int intValue;
        char strValue[MAX_STR_LEN];
    };
} Variable;

Variable variables[MAX_VAR_COUNT];
int varCount = 0;

void execute(const char* line);

Variable* getVariable(const char* name) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return &variables[i];
        }
    }
    return NULL;
}

char* concatStrings(const char* str1, const char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* result = (char*)malloc(len1 + len2 + 1); // +1 for the null-terminator
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

char* subtractStrings(const char* str, const char* sub) {
    char* pos = strstr(str, sub);
    if (!pos) return strdup(str); // sub not found in str

    size_t len = strlen(str) - strlen(sub);
    char* result = (char*)malloc(len + 1); // +1 for the null-terminator

    strncpy(result, str, pos - str); // copy part before sub
    result[pos - str] = '\0';   
    strcat(result, pos + strlen(sub)); // copy part after sub

    return result;
}

void declareVariable(const char* name, VarType type, const char* value) {
    if (varCount >= MAX_VAR_COUNT) {
        printf("Error: Too many variables declared.\n");
        exit(1);
    }

    strcpy(variables[varCount].name, name);
    variables[varCount].type = type;

    if (type == INT) {
        if (value != NULL) {
            int intValue;
            sscanf(value, "IntConst(%d)", &intValue);
            variables[varCount].intValue = intValue;
        } else {
            variables[varCount].intValue = 0;
        }
        printf("Declared variable: %s of type INT with value %d\n", name, variables[varCount].intValue);
    } else {
        if (value != NULL) {
            char buffer[MAX_STR_LEN];
            sscanf(value, "String(\"%255[^\"]\")", buffer);
            strcpy(variables[varCount].strValue, buffer);
        } else {
            variables[varCount].strValue[0] = '\0';
        }
        printf("Declared variable: %s of type TEXT with value \"%s\"\n", name, variables[varCount].strValue);
    }

    varCount++;
}

void handleDeclaration(const char* line) {
    char* token = strtok((char*)line, " ,.");
    VarType type;

    // Determine the type of the variables
    if (strcmp(token, "Keyword(int)") == 0) {
        type = INT;
    } else if (strcmp(token, "Keyword(text)") == 0) {
        type = TEXT;
    } else {
        printf("Error: Unrecognized type keyword %s.\n", token);
        return;
    }

    // Process each variable declaration
    while ((token = strtok(NULL, " ,.")) != NULL) {
        if (strncmp(token, "Identifier(", 11) == 0) {
            char varName[32];
            sscanf(token, "Identifier(%31[^)])", varName);

            char* value = NULL;
            token = strtok(NULL, " ,.");
            if (token != NULL && strcmp(token, "Keyword(is)") == 0) {
                token = strtok(NULL, " ,.");
                if (token != NULL && (strncmp(token, "IntConst(", 9) == 0 || strncmp(token, "String(", 7) == 0)) {
                    if (strncmp(token, "String(", 7) == 0) {
                        // Allocate enough space for the entire string
                        size_t valueLen = strlen(token);
                        value = (char*)malloc(valueLen + 1);
                        strcpy(value, token);

                        char* endQuote = strstr(token, "\")");
                        while (endQuote == NULL) {
                            token = strtok(NULL, " ,");
                            if (token == NULL) {
                                printf("Error: Unterminated string.\n");
                                free(value);
                                return;
                            }
                            valueLen += strlen(token) + 1;
                            value = (char*)realloc(value, valueLen + 1);
                            strcat(value, " ");
                            strcat(value, token);
                            endQuote = strstr(token, "\")");
                        }

                        // Null-terminate the final string
                        value[valueLen] = '\0';
                    } else {
                        value = (char*)malloc(strlen(token) + 1);
                        strcpy(value, token);
                    }

                    token = strtok(NULL, " ,."); // Move to the next token
                } else {
                    printf("Error: Invalid assignment for variable %s.\n", varName);
                    return;
                }
            }

            // Declare the variable with or without an initial value
            declareVariable(varName, type, value);

            if (value) {
                free(value);
            }
        }

        if (token == NULL || strcmp(token, "Comma") != 0) {
            break;
        }
    }
}


void handleAssignment(const char* varName, const char* expression) {
    Variable* var = getVariable(varName);
    if (var == NULL) {
        // Yeni bir değişken oluştur
        if (varCount >= MAX_VAR_COUNT) {
            printf("Error: Too many variables declared.\n");
            exit(1);
        }
        strcpy(variables[varCount].name, varName);

        // String içerip içermediğini kontrol et
        if (strstr(expression, "String(") != NULL) {
            variables[varCount].type = TEXT;
            var = &variables[varCount];
            varCount++;
        } else {
            variables[varCount].type = INT;
            var = &variables[varCount];
            varCount++;
        }
    }

    if (var->type == INT) {
        int result = 0;
        char* exprCopy = strdup(expression);
        char* token = strtok(exprCopy, " ");
        while (token != NULL) {
            if (strncmp(token, "IntConst(", 9) == 0) {
                int num;
                sscanf(token, "IntConst(%d)", &num);
                result += num;
            } else if (strncmp(token, "Identifier(", 11) == 0) {
                char otherVarName[32];
                sscanf(token, "Identifier(%31[^)])", otherVarName);
                Variable* other = getVariable(otherVarName);
                if (other != NULL && other->type == INT) {
                    result += other->intValue;
                } else {
                    printf("Error: Invalid integer assignment for %s.\n", otherVarName);
                    free(exprCopy);
                    return;
                }
            } else if (strcmp(token, "Operator(/)") == 0) {
                token = strtok(NULL, " ");
                if (token != NULL && strncmp(token, "IntConst(", 9) == 0) {
                    int divisor;
                    sscanf(token, "IntConst(%d)", &divisor);
                    if (divisor != 0) {
                        result /= divisor;
                    } else {
                        printf("Error: Division by zero.\n");
                        free(exprCopy);
                        return;
                    }
                } else {
                    printf("Error: Invalid expression.\n");
                    free(exprCopy);
                    return;
                }
            } else if (strcmp(token, "Operator(+)") == 0) {
                // Do nothing, just move to the next token
            } else {
                printf("Error: Unexpected token %s.\n", token);
                free(exprCopy);
                return;
            }
            token = strtok(NULL, " ");
        }
        var->intValue = result < 0 ? 0 : (result > MAX_INT ? MAX_INT : result);
        printf("Assigned %d to variable %s\n", var->intValue, varName);
        free(exprCopy);
    } else if (var->type == TEXT) {
        // Handle assignment to TEXT variable
        char result[MAX_STR_LEN] = ""; // Sonuç string'i başlat
        char* exprCopy = strdup(expression); // İfadeyi kopyala
        char* token = strtok(exprCopy, " "); // İfadeyi token'lara ayır

        while (token != NULL) { // Tüm token'ları işleyene kadar devam et
            if (strcmp(token, "Keyword(is)") == 0) {
                // 'is' keywordünü atla ve bir sonraki token'a geç
                token = strtok(NULL, " ");
                continue;
            } else if (strncmp(token, "String(", 7) == 0) {
                char str[MAX_STR_LEN] = "";
                // String'in tamamını al
                size_t len = strlen(token);
                if (token[len - 1] != ')') {
                    // Eğer token ')' ile bitmiyorsa, devamındaki token'ları birleştir
                    strcat(str, token + 7); // "String(" kısmını atla
                    while ((token = strtok(NULL, " ")) != NULL) {
                        len = strlen(token);
                        strcat(str, " ");
                        strcat(str, token);
                        if (token[len - 1] == ')') {
                            str[strlen(str) - 1] = '\0'; // Kapanış parantezini kaldır
                            break;
                        }
                    }
                } else {
                    // Eğer token ')' ile bitiyorsa, sadece string içeriğini al
                    sscanf(token, "String(\"%255[^\"]", str); // String değerini al
                }

                // Son string'deki " karakterlerini kaldır
                char tempStr[MAX_STR_LEN] = "";
                char* p = str;
                char* q = tempStr;
                while (*p) {
                    if (*p != '\"') {
                        *q++ = *p;
                    }
                    p++;
                }
                *q = '\0';

                strcat(result, tempStr); // Sonuca ekle
            } else if (strncmp(token, "Identifier(", 11) == 0) {
                char otherVarName[32];
                sscanf(token, "Identifier(%31[^)])", otherVarName); // Diğer değişken ismini al
                Variable* other = getVariable(otherVarName); // Diğer değişkeni getir
                if (other != NULL && other->type == TEXT) {
                    strcat(result, other->strValue); // Diğer değişkenin değerini sonuca ekle
                } else {
                    printf("Error: Invalid text assignment for %s.\n", otherVarName);
                    free(exprCopy); // Hatalı durumda belleği temizle ve çık
                    return;
                }
            } else if (strcmp(token, "Operator(+)") == 0) {
                // Operatör + ise bir şey yapma, bir sonraki token'a geç
            } else {
                printf("Error: Unexpected token %s.\n", token);
                free(exprCopy); // Beklenmeyen token durumunda çık
                return;
            }
            token = strtok(NULL, " "); // Bir sonraki token'a geç
        }
        strcpy(var->strValue, result); // Sonucu değişkene ata
        printf("Assigned \"%s\" to variable %s\n", var->strValue, varName);
        free(exprCopy); // Belleği temizle
    } else {
        printf("Error: Unsupported variable type for %s.\n", varName);
    }
}

void handleIO(const char* command, const char* line) {
    if (strcmp(command, "Keyword(write)") == 0) {
        char* token = strtok((char*)line, " ,.");
        while (token != NULL) {
            if (strncmp(token, "String(", 7) == 0) {
                char str[MAX_STR_LEN];
                sscanf(token, "String(\"%255[^\"]\")", str);
                printf("%s", str);
            } else if (strncmp(token, "Identifier(", 11) == 0) {
                char varName[32];
                sscanf(token, "Identifier(%31[^)])", varName);
                Variable* var = getVariable(varName);
                if (var != NULL) {
                    if (var->type == INT) printf("%d", var->intValue);
                    else printf("%s", var->strValue);
                } else {
                    printf("Error: Undeclared variable %s.\n", varName);
                }
            }
            token = strtok(NULL, " ,.");
        }
        printf("\n");
    } else if (strcmp(command, "Keyword(read)") == 0) {
        char prompt[MAX_STR_LEN], varName[32];
        sscanf(line, "String(\"%255[^\"]\") Identifier(%31[^)])", prompt, varName);
        Variable* var = getVariable(varName);
        if (var == NULL) {
            printf("Error: Undeclared variable %s.\n", varName);
            return;
        }
        printf("%s", prompt);
        if (var->type == INT) {
            if (scanf("%d", &var->intValue) != 1) {
                printf("Warning: Invalid integer input. Assigned 0 to %s.\n", varName);
                var->intValue = 0;
            }
        } else {
            scanf("%s", var->strValue);
        }
    } else if (strcmp(command, "Keyword(newLine)") == 0) {
        printf("\n");
    }
}

void handleLoop(int loopCount, const char* loopBody) {
        for (int i = 0; i < loopCount; i++) {
            printf("Loop iteration %d:\n", i + 1);

            char loopBodyCopy[4096];
            strcpy(loopBodyCopy, loopBody);

            char* token = strtok(loopBodyCopy, " ");
            char statement[1024] = {0};
            int inStatement = 0;

            while (token != NULL) {
                if (inStatement == 0 && strncmp(token, "Keyword", 7) == 0) {
                    inStatement = 1;
                    strcat(statement, token);
                    strcat(statement, " ");
                } else if (inStatement) {
                    strcat(statement, token);
                    strcat(statement, " ");
                    if (strcmp(token, "EndOfLine") == 0) {
                        execute(statement);
                        statement[0] = '\0';
                        inStatement = 0;
                    }
                }
                token = strtok(NULL, " ");
            }
            if (inStatement) {
                execute(statement);
        
            // Her iterasyon sonunda Identifier(i) değerini 1 artır
            Variable *var = getVariable("i");
            if (var != NULL) {
                var->intValue++;
            } else {
                printf("Error: Variable 'i' is not declared.\n");
            }
        }
    }
}


void handleIsStatement(const char* line) {
    char varName[32], expression[256];
    sscanf(line, " Identifier(%31[^)]) Operator(is) %255[^\n]", varName, expression);
    handleAssignment(varName, expression);
}


void execute(const char* line) {
    printf("Line: %s\n", line);

    if (strncmp(line, "Keyword(write)", 14) == 0) {
        handleIO("Keyword(write)", line + 14);
    } else if (strncmp(line, "Keyword(int)", 12) == 0) {
        handleDeclaration(line);
    } else if (strncmp(line, "Keyword(text)", 13) == 0) {
        handleDeclaration(line);
    } else if (strncmp(line, "Keyword(read)", 13) == 0) {
        char command[32], rest[256];
        sscanf(line, "%31s %255[^\n]", command, rest);
        handleIO(command, rest);
    } else if (strncmp(line, "Keyword(newLine)", 16) == 0) {
        handleIO("Keyword(newLine)", NULL);
    } else if (strncmp(line, "Keyword(loop)", 13) == 0) {
        int loopCount;
        const char* loopCountStart = strstr(line, "IntConst(") + strlen("IntConst(");
        sscanf(loopCountStart, "%d", &loopCount);

        const char* loopBodyStart = strstr(line, "LeftCurlyBracket") + strlen("LeftCurlyBracket");
        const char* loopBodyEnd = strstr(line, "RightCurlyBracket");
        if (loopBodyEnd != NULL) {
            char loopBody[4096];
            strncpy(loopBody, loopBodyStart, loopBodyEnd - loopBodyStart);
            loopBody[loopBodyEnd - loopBodyStart] = '\0';

            handleLoop(loopCount, loopBody);
        } else {
            printf("Error: Missing RightCurlyBracket in loop.\n");
        }
    } else if (strncmp(line, "Keyword(is)", 11) == 0) {
        handleIsStatement(line + 11);
    } else if (strstr(line, "Keyword(is)") != NULL) {
        char varName[32], expression[256];
        sscanf(line, "Identifier(%31[^)]) Keyword(is) %255[^\n]", varName, expression);
        handleAssignment(varName, expression);
    } else {
        printf("Error: Unrecognized keyword.\n");
    }
}



void removeComments(char* line) {
    char* start = strstr(line, "/*");
    while (start != NULL) {
        char* end = strstr(start + 2, "*/");
        if (end != NULL) {
            memmove(start, end + 2, strlen(end + 2) + 1);
        } else {
            *start = '\0';
        }
        start = strstr(line, "/*");
    }
}

int main() {
    FILE* file = fopen("code.lex", "r");
    if (!file) {
        printf("Error: Could not open file code.lex.\n");
        return 1;
    }

    char token[256];
    char line[1024] = {0};
    int inLoop = 0;
    char loopBody[4096] = {0};

    while (fgets(token, sizeof(token), file)) {
        removeComments(token);
        strtok(token, "\n");
        if (strlen(token) == 0) continue;

        if (strncmp(token, "Keyword(loop)", 13) == 0) {
            inLoop = 1;
            strcat(line, token);
        } else if (inLoop) {
            strcat(loopBody, " ");
            strcat(loopBody, token);
            if (strstr(token, "RightCurlyBracket") != NULL) {
                inLoop = 0;
                strcat(line, " ");
                strcat(line, loopBody);
                execute(line);
                line[0] = '\0';
                loopBody[0] = '\0';
            }
        } else {
            if (strcmp(token, "EndOfLine") == 0) {
                if (strlen(line) > 0) {
                    execute(line);
                    line[0] = '\0';
                }
            } else {
                if (strlen(line) > 0) strcat(line, " ");
                strcat(line, token);
            }
        }
    }

    if (strlen(line) > 0) {
        execute(line);
    }

    fclose(file);
    return 0;
}
