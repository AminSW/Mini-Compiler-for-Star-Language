else if (strncmp(token, "String(", 7) == 0) {
                char str[MAX_STR_LEN];
                sscanf(token, "String(\"%255[^\"]\")", str); // String değerini al
                strcat(result, str); // Sonuca ekle
            } else if (strncmp(token, "Identifier(", 11) == 0) {
                char otherVarName[32];
                sscanf(token, "Identifier(%31[^)])", otherVarName); // Diğer değişken ismini al
                Variable* other = getVariable(otherVarName); // Diğer değişkeni getir
                if (other != NULL && other->type == TEXT) {
                    lastIdentifierValue = other->strValue; // Son Identifier değerini sakla
                    strcat(result, lastIdentifierValue); // Diğer değişkenin değerini sonuca ekle
                } else {
                    printf("Error: Invalid text assignment for %s.\n", otherVarName);
                    free(exprCopy); // Hatalı durumda belleği temizle ve çık
                    return;
                }
            } else if (strcmp(token, "Operator(+)") == 0) {
                // Operatör + ise bir şey yapma, bir sonraki token'a geç
            } else if (strcmp(token, "Operator(-)") == 0) {
                // Operatör - ise bir sonraki token'ı al ve çıkarılacak string'i işle
                token = strtok(NULL, " ");
                if (token != NULL) {
                    if (strncmp(token, "String(", 7) == 0) {
                        char str[MAX_STR_LEN];
                        sscanf(token, "String(\"%255[^\"]\")", str); // String değerini al
                        char* newResult = subtractStrings(result, str); // String çıkarma işlemini yap
                        strcpy(result, newResult); // Yeni sonucu ata
                        free(newResult); // Geçici sonucu temizle
                    } else if (strncmp(token, "Identifier(", 11) == 0) {
                        char otherVarName[32];
                        sscanf(token, "Identifier(%31[^)])", otherVarName); // Diğer değişken ismini al
                        Variable* other = getVariable(otherVarName); // Diğer değişkeni getir
                        if (other != NULL && other->type == TEXT) {
                            char* newResult = subtractStrings(result, other->strValue); // String çıkarma işlemini yap
                            strcpy(result, newResult); // Yeni sonucu ata
                            free(newResult); // Geçici sonucu temizle
                        } else {
                            printf("Error: Invalid text subtraction for %s.\n", otherVarName);
                            free(exprCopy); // Hatalı durumda belleği temizle ve çık
                            return;
                        }
                    } else {
                        printf("Error: Invalid text subtraction token %s.\n", token);
                        free(exprCopy); // Geçersiz token durumunda çık
                        return;
                    }
                } else {
                    printf("Error: Unexpected end of expression after Operator(-).\n");
                    free(exprCopy); // Beklenmeyen son durumunda çık
                    return;
                }
            } else {
                printf("Error: Unexpected token %s.\n", token);
                free(exprCopy); // Beklenmeyen token durumunda çık
                return;
            }