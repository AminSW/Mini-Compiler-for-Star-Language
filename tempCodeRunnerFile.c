// Dosyanın sonuna ulaşıldığında, son satırı işle
        if (feof(file)) {
            execute(fullLine);
            break;
        }