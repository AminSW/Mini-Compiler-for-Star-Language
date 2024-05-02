#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_IDENTIFIER_SIZE 10
#define MAX_INTEGER_SIZE 8
#define MAX_STRING_SIZE 256

// Token türlerini tanımlayan enum
typedef enum
{
    Identifier,
    IntConst,
    Operator,
    LeftCurlyBracket,
    RightCurlyBracket,
    String,
    Keyword,
    EndOfLine,
    Comma
} TokenType;

// Token yapısı
typedef struct
{
    TokenType type;
    char content[MAX_STRING_SIZE];
} Token;

// Fonksiyon prototipleri
void tokenize(FILE *sourceFile, FILE *outputFile);
TokenType getTokenType(char *tokenContent);

int main()
{
    FILE *sourceFile, *outputFile;

    // Kaynak dosyasını aç
    sourceFile = fopen("code.sta", "r");
    if (sourceFile == NULL)
    {
        perror("Error opening source file");
        return EXIT_FAILURE;
    }

    // Çıktı dosyasını aç
    outputFile = fopen("code.lex", "w");
    if (outputFile == NULL)
    {
        perror("Error opening output file");
        fclose(sourceFile);
        return EXIT_FAILURE;
    }

    // Tokenleme işlemini gerçekleştir
    tokenize(sourceFile, outputFile);

    // Dosyaları kapat
    fclose(sourceFile);
    fclose(outputFile);

    printf("done");

    return EXIT_SUCCESS;
}

// Tokenleme işlevi
void tokenize(FILE *sourceFile, FILE *outputFile)
{
    //int stringIsNotClose = 0;
    char buffer[MAX_STRING_SIZE];
    char c;
    int error = 0;

    while ((c = fgetc(sourceFile)) != EOF && error == 0)
    {
        // Boşlukları ve satır sonlarını atla
        if (isspace(c))
        {
            if (c == '\n')
            {
                fprintf(outputFile, "EndOfLine\n");
            }
            continue;
        }

        // İdentiﬁer tanımlama
        if (isalpha(c))
        {
            int i = 0;
            buffer[i++] = c;
            while ((c = fgetc(sourceFile)) != EOF && (isalnum(c) || c == '_'))
            {
                buffer[i++] = c;
            }
            buffer[i] = '\0';
            fseek(sourceFile, -1, SEEK_CUR);
            if (i > MAX_IDENTIFIER_SIZE)
            {
                fprintf(stderr, "Error: Identifier '%s' exceeds maximum size\n", buffer);
                exit(EXIT_FAILURE);
            }
            fprintf(outputFile, "Identifier(%s)\n", buffer);
        }

        // Integer Sabiti tanımlama
        else if (isdigit(c))
        {
            int i = 0;
            buffer[i++] = c;
            while ((c = fgetc(sourceFile)) != EOF && isdigit(c))
            {
                buffer[i++] = c;
            }
            buffer[i] = '\0';
            fseek(sourceFile, -1, SEEK_CUR);
            if (i > MAX_INTEGER_SIZE)
            {
                fprintf(stderr, "Error: Integer constant '%s' exceeds maximum size\n", buffer);
                exit(EXIT_FAILURE);
            }
            fprintf(outputFile, "IntConst(%s)\n", buffer);
        }

        // String tanımlama
        else if (c == '"')
        {
            int i = 0;
            buffer[i++] = c;
            while ((c = fgetc(sourceFile)) != EOF && c != '"')
            {
                if(c == '.')
                {
                    printf("string constant must terminate before the file end");
                    error = 1;
                    exit(EXIT_FAILURE);
                }
                if (i >= MAX_STRING_SIZE - 1)
                {
                    fprintf(stderr, "Error: String constant exceeds maximum size\n");
                    exit(EXIT_FAILURE);
                }
                buffer[i++] = c;
            }
            if (c == EOF)
            {
                fprintf(stderr, "Error: String constant does not terminate before end of file\n");
                exit(EXIT_FAILURE);
            }
            buffer[i++] = '"';
            buffer[i] = '\0';
            fprintf(outputFile, "String(%s)\n", buffer);
        }

        else if(c == ',')
        {
            fprintf(outputFile, "Comma\n", buffer);
        }

        // Keyword, Operator veya LeftCurlyBracket tanımlama
        else if (c == '{')
        {
            fprintf(outputFile, "LeftCurlyBracket\n");
        }
        else if (c == '}')
        {
            fprintf(outputFile, "RightCurlyBracket\n");
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        {
            fprintf(outputFile, "Operator(%c)\n", c);
        }

        // Yorum işlemi
        else if (c == '/')
        {
            error = 1;
            char nextChar = fgetc(sourceFile);
            if (nextChar == '*')
            {
                while ((c = fgetc(sourceFile)) != EOF)
                {
                    if (c == '*')
                    {
                        char potentialEndComment = fgetc(sourceFile);
                        if (potentialEndComment == '/')
                        {
                            error = 0;
                            break;
                        }
                        else
                        {
                            fseek(sourceFile, -1, SEEK_CUR);
                        }
                    }
                }
            }
            else
            {
                fprintf(outputFile, "Operator(/)\n");
                fseek(sourceFile, -1, SEEK_CUR);
            }
        }
        if (error == 1)
        {
            printf("Yorum satırları kapanmalıdır");
        }
        
    }
}