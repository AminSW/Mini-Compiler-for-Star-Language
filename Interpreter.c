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

Variable* getVariable(const char* name) {
    printf("Searching for variable: %s\n", name);
    for (int i = 0; i < varCount; i++) {
        if (strcmp(variables[i].name, name) == 0 || strcmp(variables[i].name + 11, name) == 0) { // Dönüşüm
            printf("Variable found: %s\n", name);
            return &variables[i];
        }
    }
    printf("Variable not found: %s\n", name);
    return NULL;
}

void declareVariable(const char* name, VarType type) {
    printf("Declaring variable: %s of type: %s\n", name, type == INT ? "INT" : "TEXT");
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
    varCount++;
}

void handleDeclaration(const char* line) {
    printf("Handling declaration: %s\n", line);
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
        // Comma olduğunda bir sonraki tokena geç
        if (token != NULL && strcmp(token, "Comma") == 0) {
            token = strtok(NULL, " ,.");
        }
    }
}


// handleAssignment() fonksiyonunda düzeltme
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
            if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
                int num = atoi(token);
                result += num;
            } else if (strncmp(token, "Identifier(", 11) == 0) {
                char varName[32];
                sscanf(token, "Identifier(%31[^)])", varName);
                Variable* other = getVariable(varName);
                if (other != NULL && other->type == INT) {
                    result += other->intValue;
                } else {
                    printf("Error: Invalid integer assignment.\n");
                    free(exprCopy);
                    return;
                }
            } else if (strcmp(token, "Operator(/)") == 0) {
                token = strtok(NULL, " "); // Skip Operator(/)
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
            }
            token = strtok(NULL, " ");

            // Ifade işleme sonunda bir sonraki tokenın NULL olup olmadığını kontrol et
            if (token == NULL) {
                var->intValue = result < 0 ? 0 : (result > MAX_INT ? MAX_INT : result);
            }
        }
        free(exprCopy);
    }
}

// getVariable() fonksiyonunda düzeltme


void handleIO(const char* command, const char* line) {
    printf("Handling IO: %s with %s\n", command, line);
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

void execute(const char* line) {
    printf("Executing line: %s\n", line);

    if (strncmp(line, "Keyword(write)", 14) == 0) {
        handleIO("Keyword(write)", line + 14); // 'write' komutunu ayır
    } else if (strncmp(line, "Keyword(int)", 12) == 0) {
        handleDeclaration(line);
    } else if (strncmp(line, "Keyword(read)", 13) == 0) {
        char command[32], rest[256];
        sscanf(line, "%31s %255[^\n]", command, rest);
        handleIO(command, rest);
    } else if (strncmp(line, "Keyword(newLine)", 16) == 0) {
        handleIO("Keyword(newLine)", NULL); // 'newLine' komutu
    } else if (strstr(line, "Keyword(is)") != NULL) {
        char varName[32], expression[256];
        sscanf(line, "Identifier(%31[^)]) Keyword(is) %255[^\n]", varName, expression);
        handleAssignment(varName, expression);
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

    char line[1024];
    char fullLine[1024] = {0};
    
    // Dosyadan satır oku ve dosyanın sonuna ulaşılıncaya kadar döngüyü çalıştır
    while (fgets(line, sizeof(line), file)) {
        removeComments(line);
        strcat(fullLine, line);
        // Satırın sonunda satır sonu karakteri yoksa, okumaya devam et
        if (line[strlen(line) - 1] != '\n') {
            continue;
        }
        execute(fullLine);
        fullLine[0] = '\0';
    }

    fclose(file);
    return 0;
}


