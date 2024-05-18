#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Fonksiyon, sırasıyla "Keyword(loop)", "IntConst(x)", "Keyword(times)" ve "LeftCurlyBracket" kontrol eder
// "LeftCurlyBracket" ile "RightCurlyBracket" arasındaki komutları birleştirir ve tekrar sayısı kadar çalıştırır
void satir_isle(char *satir, int *deger_bulundu, int *times_bulundu, int *curly_bulundu, char *komut_buffer) {
    // Yeni satır karakterini kaldır
    satir[strcspn(satir, "\n")] = '\0';

    static int loop_bulundu = 0;
    static int intconst_bulundu = 0;
    static int komut_birlesiyor = 0;

    if (strcmp(satir, "Keyword(loop)") == 0) {
        loop_bulundu = 1;
        intconst_bulundu = 0; // IntConst değeri için bekleme başlasın
        printf("Anahtar kelime 'loop' bulundu: %s\n", satir);
    } else if (loop_bulundu && strncmp(satir, "IntConst(", 9) == 0) {
        char *parantez_ici = satir + 9;
        char *parantez_kapa = strchr(parantez_ici, ')');

        if (parantez_kapa) {
            *parantez_kapa = '\0'; // Parantezi kapatan karakteri null karakteri ile değiştir
            *deger_bulundu = atoi(parantez_ici);
            printf("Bulunan IntConst degeri: %d\n", *deger_bulundu);
            loop_bulundu = 0; // Sonrasında başka bir "Keyword(loop)" aranabilir
            intconst_bulundu = 1; // IntConst bulundu, şimdi times arıyoruz
        }
    } else if (intconst_bulundu && strcmp(satir, "Keyword(times)") == 0) {
        *times_bulundu = 1;
        printf("Anahtar kelime 'times' bulundu: %s\n", satir);
        intconst_bulundu = 0; // Sonrasında başka bir döngü aranabilir
    } else if (*times_bulundu && strcmp(satir, "LeftCurlyBracket") == 0) {
        *curly_bulundu = 1;
        komut_birlesiyor = 1;
        printf("LeftCurlyBracket bulundu: %s\n", satir);
        strcat(komut_buffer, "{\n"); // Komutlar buffer'ına ekle
    } else if (komut_birlesiyor && strcmp(satir, "RightCurlyBracket") == 0) {
        komut_birlesiyor = 0;
        printf("RightCurlyBracket bulundu: %s\n", satir);
        strcat(komut_buffer, "}\n"); // Komutlar buffer'ına ekle
        printf("Birleşik komutlar:\n%s", komut_buffer);

        // Komutları IntConst sayısı kadar tekrar çalıştır
        for (int i = 0; i < *deger_bulundu; i++) {
            printf("Komut tekrar %d:\n%s", i + 1, komut_buffer);
        }

        // Buffer'ı sıfırla
        komut_buffer[0] = '\0';
        *times_bulundu = 0; // Sonrasında başka bir döngü aranabilir
    } else if (komut_birlesiyor) {
        strcat(komut_buffer, satir);
        strcat(komut_buffer, "\n"); // Her satırı buffer'a ekle
    } else {
        printf("Okunan satir: %s\n", satir);
    }
}

void satir_islet(){
    int deger_bulundu = 0;
    int times_bulundu = 0;
    int curly_bulundu = 0;
    char komut_buffer[1024] = "";
}

int main() {
    FILE *dosya;
    char satir[256];
    int deger_bulundu = 0;
    int times_bulundu = 0;
    int curly_bulundu = 0;
    char komut_buffer[1024] = ""; // Komutları saklamak için bir buffer

    // Dosyayı aç
    dosya = fopen("code.lex", "r");
    
    // Dosya açılıp açılmadığını kontrol et
    if (dosya == NULL) {
        printf("Dosya açılamadı.");
        return 1;
    }

    // Dosyadan satır oku ve işlemek için diğer fonksiyona gönder
    while (fgets(satir, sizeof(satir), dosya) != NULL) {
        satir_isle(satir, &deger_bulundu, &times_bulundu, &curly_bulundu, komut_buffer);
    }

    // Dosyayı kapat
    fclose(dosya);

    // Sonuçları kullanabilirsiniz
    printf("Son bulunan IntConst degeri: %d\n", deger_bulundu);
    if (times_bulundu) {
        printf("Keyword(times) bulundu.\n");
    } else {
        printf("Keyword(times) bulunamadı.\n");
    }
    if (curly_bulundu) {
        printf("LeftCurlyBracket bulundu.\n");
    } else {
        printf("LeftCurlyBracket bulunamadı.\n");
    }

    return 0;
}
