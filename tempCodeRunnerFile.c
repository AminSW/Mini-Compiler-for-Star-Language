/* code.lex */

/* Import necessary libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Define maximum sizes */
#define MAX_IDENTIFIER_SIZE 10
#define MAX_INT_SIZE 8
#define MAX_STRING_SIZE 256

/* Token türlerinin ve yapılarının tanımlandığı kısım */

/* Define token types */
typedef enum {
    IDENTIFIER,                 // Tanımlayıcılar
    INT_CONST,                  // Tamsayı sabitleri
    OPERATOR,                   // Aritmetik operatörler
    LEFT_CURLY_BRACKET,         // Sol süslü parantez
    RIGHT_CURLY_BRACKET,        // Sağ süslü parantez
    STRING_CONST,               // Metin sabitleri
    KEYWORD,                    // Anahtar kelimeler
    END_OF_LINE,                // Satır sonu işareti
    COMMA                       // Virgül
} TokenType;

/* Structure for tokens */
typedef struct {
    TokenType type;             // Token türü
    char value[MAX_STRING_SIZE];/* Token değeri */
} Token;


/* Function prototypes */
Token getNextToken(FILE *fp);
void printToken(Token token);

/* Ana fonksiyonlar */

/* Main function */
int main() {
    /* Open input and output files */
    FILE *inputFile = fopen("code.sta", "r");    // Giriş dosyasını aç
    FILE *outputFile = fopen("code.lex", "w");   // Çıktı dosyasını aç

    if (inputFile == NULL || outputFile == NULL) {   // Dosya açma hatası kontrolü
        printf("Error opening files.\n");     // Hata mesajı
        return -1;                            // Hata kodu ile çıkış
    }

    /* Read tokens and write to output file */
    Token token;
    int endOfLineCount = 0;   // Satır sonu sayacı
    do {
        token = getNextToken(inputFile);   // Bir sonraki tokeni al
        if (token.type != END_OF_LINE) {   // Eğer token bir satır sonu değilse
            if (endOfLineCount > 0) {      // Eğer önceki token bir satır sonuysa
                printToken((Token){END_OF_LINE, ""});   // Satır sonu tokenini yaz
            }
            printToken(token);              // Tokeni yaz
            endOfLineCount = 0;            // Satır sonu sayacını sıfırla
        } else {
            endOfLineCount++;              // Eğer token satır sonu ise sayacı artır
        }
    } while (!feof(inputFile));            // Dosya sonuna gelene kadar devam et

    if (endOfLineCount > 0) {              // Eğer dosyanın sonunda satır sonu varsa
        printToken((Token){END_OF_LINE, ""});   // Satır sonu tokenini yaz
    }

    /* Close files */
    fclose(inputFile);                     // Giriş dosyasını kapat
    fclose(outputFile);                    // Çıktı dosyasını kapat

    return 0;                              // Başarılı çıkış kodu
}

Token getNextToken(FILE *fp)
{
    Token token;                            // Token yapısı oluştur
    char buffer[MAX_STRING_SIZE];          // Geçici depolama alanı
    int i = 0;                              // Buffer indeksi
    char c;                                 // Karakter depolama değişkeni

    /* Boşlukları atla */

    
    /* Skip whitespace and comments */
    do {
        c = fgetc(fp);                      // Karakteri dosyadan al
        if (c == '/') {                     // Eğer karakter bir "/" ise
            char nextChar = fgetc(fp);      // Bir sonraki karakteri al
            if (nextChar == '*') {          // Eğer bir sonraki karakter "*" ise
                int commentDepth = 1;       // Yorum içeriğini takip etmek için derinlik sayacı
                while (commentDepth > 0) {  // Yorum sonuna kadar döngü
                    char prevChar = c;     // Önceki karakteri sakla
                    c = fgetc(fp);         // Bir sonraki karakteri al
                    if (prevChar == '*' && c == '/')   // Yorum sonu kontrolü
                        commentDepth--;     // Yorum sonu bulundu, derinlik sayacını azalt
                    else if (prevChar == '/' && c == '*') // İç içe yorum kontrolü
                        commentDepth++;     // Yorum başladı, derinlik sayacını artır
                    else if (c == EOF) {   // Dosya sonu kontrolü
                        printf("Error: Unclosed comment.\n");   // Hata mesajı
                        exit(-1);           // Programı sonlandır
                    }
                }
                c = fgetc(fp);              // Yorum sonrasındaki karakteri al
            }
            else {
                fseek(fp, -1, SEEK_CUR);   // Eğer yorum değilse, karakteri geri al
                break; // Döngüden çık     
            }
        }
    } while (isspace(c));                  // Boşluk karakterlerini atla

    /* Dosya sonu kontrolü */
    
/* Satır sonu işareti */
    if (c == '.')
    {
        token.type = END_OF_LINE;          // Token türünü belirle
        strcpy(token.value, ".");           // Token değerini belirle
        return token;                       // Tokeni döndür
    }
    /* Tanımlayıcı veya Anahtar Kelime */
    if (isalpha(c))
    {
        buffer[i++] = c;                    // Buffer'a karakteri ekle
        while ((isalnum(c = fgetc(fp)) || c == '_') && i < MAX_IDENTIFIER_SIZE)
        {
            buffer[i++] = c;                // Buffer'a karakteri ekle
        }
        if (i == MAX_IDENTIFIER_SIZE && (isalnum(c) || c == '_')) {
            printf("Error: Identifier too long.\n");   // Hata mesajı
            exit(-1);                       // Programı sonlandır
        }
        buffer[i] = '\0';                   // Buffer'ın sonuna null karakter ekle
        fseek(fp, -1, SEEK_CUR);            // Dosya imlecinin bir karakter geri al
        /* Tanımlanan anahtar kelimeleri kontrol et */
        if (strcmp(buffer, "int") == 0 || strcmp(buffer, "text") == 0 ||
            strcmp(buffer, "is") == 0 || strcmp(buffer, "loop") == 0 ||
            strcmp(buffer, "times") == 0 || strcmp(buffer, "read") == 0 ||
            strcmp(buffer, "write") == 0 || strcmp(buffer, "newLine") == 0)
        {
            token.type = KEYWORD;           // Token türünü belirle
        }
        else
        {
            token.type = IDENTIFIER;        // Token türünü belirle
        }

        strcpy(token.value, buffer);        // Token değerini kopyala
        return token;                       // Tokeni döndür
    }

    /* Tamsayı sabiti */
    if (isdigit(c))
    {
        buffer[i++] = c;                    // Buffer'a karakteri ekle
        while (isdigit(c = fgetc(fp)) && i < MAX_INT_SIZE)
        {
            buffer[i++] = c;                // Buffer'a karakteri ekle
        }
        if (i == MAX_INT_SIZE && isdigit(c)) {
            printf("Error: Integer constant too long.\n");    // Hata mesajı
            exit(-1);                       // Programı sonlandır
        }
        buffer[i] = '\0';                   // Buffer'ın sonuna null karakter ekle
        fseek(fp, -1, SEEK_CUR);            // Dosya imlecinin bir karakter geri al
        token.type = INT_CONST;             // Token türünü belirle
        strcpy(token.value, buffer);        // Token değerini kopyala
        return token;                       // Tokeni döndür
    }

    /* Operatörler */
    if (c == '+' || c == '-' || c == '*' || c == '/')
    {
        token.type = OPERATOR;              // Token türünü belirle
        token.value[0] = c;                 // Token değerini belirle
        token.value[1] = '\0';              // String sonunu belirle
        return token;                       // Tokeni döndür
    }

    /* Sol Süslü Parantez */
    if (c == '{')
    {
        token.type = LEFT_CURLY_BRACKET;   // Token türünü belirle
        strcpy(token.value, "{");           // Token değerini belirle
        return token;                       // Tokeni döndür
    }

    /* Sağ Süslü Parantez */
    if (c == '}')
    {
        token.type = RIGHT_CURLY_BRACKET;  // Token türünü belirle
        strcpy(token.value, "}");           // Token değerini belirle
        return token;                       // Tokeni döndür
    }

    /* Metin sabiti */
    if (c == '"')
    {
        buffer[i++] = c;                    // Buffer'a karakteri ekle
        while ((c = fgetc(fp)) != '"' && c != EOF && i < MAX_STRING_SIZE)
        {
            buffer[i++] = c;                // Buffer'a karakteri ekle
        }
        if (c != '"')
        {
            printf("Error: String constant not terminated.\n");    // Hata mesajı
            exit(-1);                       // Programı sonlandır
        }
        buffer[i++] = c;                    // Buffer'a karakteri ekle
        buffer[i] = '\0';                   // Buffer'ın sonuna null karakter ekle
        token.type = STRING_CONST;          // Token türünü belirle
        strcpy(token.value, buffer);        // Token değerini kopyala
        return token;                       // Tokeni döndür
    }

    

    /* Virgül */
    if (c == ',')
    {
        token.type = COMMA;                // Token türünü belirle
        strcpy(token.value, ",");           // Token değerini belirle
        return token;                       // Tokeni döndür
    }
    if (c != EOF)
    {
        /*
        token.type = END_OF_LINE;          // Token türünü belirle
        return token;   
        */                    // Tokeni döndür
        /* Geçersiz karakter */
        printf("Error: Invalid character: %c\n", c);   // Hata mesajı
        exit(-1);              
        // Programı sonlandır
    }

                     
}

void printToken(Token token)
{
    /* Tokeni çıktı dosyasına yazdırmak için kullanılan fonksiyon */

    FILE *outputFile = fopen("code.lex", "a");  // Çıktı dosyasını aç
    if (outputFile == NULL)
    {
        printf("Error writing to output file.\n");  // Hata mesajı
        exit(-1);                                    // Programı sonlandır
    }

    switch (token.type)  // Token türüne göre duruma göre işlem yap
    {
    case IDENTIFIER:  // Tanımlayıcı
        fprintf(outputFile, "Identifier(%s)\n", token.value);  // Tanımlayıcıyı yaz
        break;
    case INT_CONST:  // Tamsayı sabiti
        fprintf(outputFile, "IntConst(%s)\n", token.value);  // Tamsayı sabitini yaz
        break;
    case OPERATOR:  // Operatör
        fprintf(outputFile, "Operator(%s)\n", token.value);  // Operatörü yaz
        break;
    case LEFT_CURLY_BRACKET:  // Sol süslü parantez
        fprintf(outputFile, "LeftCurlyBracket\n");  // Sol süslü parantezi yaz
        break;
    case RIGHT_CURLY_BRACKET:  // Sağ süslü parantez
        fprintf(outputFile, "RightCurlyBracket\n");  // Sağ süslü parantezi yaz
        break;
    case STRING_CONST:  // Metin sabiti
        fprintf(outputFile, "String(%s)\n", token.value);  // Metin sabitini yaz
        break;
    case END_OF_LINE:  // Satır sonu işareti
        fprintf(outputFile, "EndOfLine\n");  // Satır sonu işaretini yaz
        break;
    case COMMA:  // Virgül
        fprintf(outputFile, "Comma\n");  // Virgülü yaz
        break;
    case KEYWORD:  // Anahtar kelime
        fprintf(outputFile, "Keyword(%s)\n", token.value);  // Anahtar kelimeyi yaz
        break;
    default:
        fprintf(outputFile, "Error: Unknown token type\n");  // Bilinmeyen token türü hatası
        break;
    }

    fclose(outputFile);  // Çıktı dosyasını kapat
}
