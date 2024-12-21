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
void handleNestedLoop(const char* loopBody);

Variable* getVariable(const char* name) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return &variables[i];
        }
    }
    return NULL;
}

void declareVariable(const char* name, VarType type) {
    if (varCount >= MAX_VAR_COUNT) {
        printf("Error: Too many variables declared.\n");
        exit(1);
    }
    strcpy(variables[varCount].name, name);
    variables[varCount].type = type;
    if (type == INT) {
        variables[varCount].intValue = 0;
    } else {
        variables[varCount].strValue[0] = '\0';
    }
    printf("Declared variable: %s of type %s\n", name, type == INT ? "INT" : "TEXT");
    varCount++;
}

void handleDeclaration(const char* line) {
    char* token = strtok((char*)line, " ,.");
    VarType type = strcmp(token, "Keyword(int)") == 0 ? INT : TEXT;
    token = strtok(NULL, " ,.");
    while (token != NULL) {
        if (strncmp(token, "Identifier(", 11) == 0) {
            char varName[32];
            sscanf(token, "Identifier(%31[^)])", varName);
            declareVariable(varName, type);
        }
        token = strtok(NULL, " ,.");
        if (token != NULL && strcmp(token, "Comma") == 0) {
            token = strtok(NULL, " ,.");
        }
    }
}

void handleAssignment(const char* varName, const char* expression) {
    Variable* var = getVariable(varName);
    if (var == NULL) {
        printf("Error: Undeclared variable %s.\n", varName);
        return;
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
    char loopBodyCopy[4096];
    int incrementValue = 1; // Varsayılan artış miktarı

    for (int i = 0; i < loopCount; i++) {
        printf("Loop iteration %d:\n", i + 1);

        strcpy(loopBodyCopy, loopBody); // loopBodyCopy'yi tekrar doldur
        char* token = strtok(loopBodyCopy, " ");
        char statement[1024] = {0};
        int inStatement = 0;
        int inInnerLoop = 0;
        int innerLoopActive = 0;
        char innerLoopBody[4096] = {0};
        int innerLoopCount = 0;

        while (token != NULL) {
            printf("Token: %s\n", token); // Debug çıktısı

            if (innerLoopActive) {
                if (strcmp(token, "LeftCurlyBracket") == 0) {
                    inInnerLoop = 1;
                } else if (strcmp(token, "RightCurlyBracket") == 0) {
                    inInnerLoop = 0;
                    innerLoopActive = 0;
                    printf("Inner Loop Body: %s\n", innerLoopBody); // Debug çıktısı
                    handleLoop(innerLoopCount, innerLoopBody); // İç döngüyü işle
                    innerLoopBody[0] = '\0'; // İç döngü gövdesini sıfırla
                    token = strtok(NULL, " ");
                    continue;
                }
                strcat(innerLoopBody, token);
                strcat(innerLoopBody, " ");
            } else if (strncmp(token, "Keyword(loop)", 13) == 0) {
                // Inner loop processing
                printf("Found inner loop\n"); // Debug çıktısı
                token = strtok(NULL, " ");
                if (token != NULL && strncmp(token, "IntConst(", 9) == 0) {
                    sscanf(token, "IntConst(%d)", &innerLoopCount);
                    token = strtok(NULL, " ");
                    if (token != NULL && strcmp(token, "Keyword(times)") == 0) {
                        innerLoopActive = 1;
                        token = strtok(NULL, " ");
                        continue;
                    }
                }
            } else if (strncmp(token, "LeftCurlyBracket", 16) == 0) {
                inInnerLoop = 1;
            } else if (strncmp(token, "Keyword", 7) == 0) {
                inStatement = 1;
            }

            if (inStatement && !inInnerLoop && !innerLoopActive) {
                strcat(statement, token);
                strcat(statement, " ");
                if (strcmp(token, "EndOfLine") == 0) {
                    printf("Line: %s\n", statement); // Debug çıktısı
                    execute(statement);
                    statement[0] = '\0';
                    inStatement = 0;
                }
            }

            token = strtok(NULL, " ");
        }

        if (inStatement && !innerLoopActive) {
            execute(statement);
        }

        // Her iterasyon sonunda Identifier(i) değerini incrementValue kadar artır
        Variable *var = getVariable("i");
        if (var != NULL) {
            var->intValue += incrementValue;
        } else {
            printf("Error: Variable 'i' is not declared.\n");
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
        const char* loopBodyEnd = strrchr(line, 'R'); // "RightCurlyBracket" yerine son 'R' karakterini bulma
        if (loopBodyEnd != NULL) {
            char loopBody[4096];
            strncpy(loopBody, loopBodyStart, loopBodyEnd - loopBodyStart - 1); // Sağ küme parantezden bir önceki karaktere kadar al
            loopBody[loopBodyEnd - loopBodyStart - 1] = '\0';

            printf("Outer Loop Body: %s\n", loopBody); // Dıştaki döngüyü göster
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
