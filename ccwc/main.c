// Coding Challenges no1
// Build your own wc (ccwc)
// Borja Fernández <lem@raspberrypi-nbsd>
// Valencia 02/03/2025

#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
    int64_t bytes, lines, words, chars;
} FileSpecs; 

FileSpecs wordCount(char* filename){

    setlocale(LC_ALL, "");

    FileSpecs fileSpecs = { .bytes = 0 , .lines = 0 , .words = 0 , .chars = 0 };

    FILE* file = fopen(filename, "r");

    if(!file){
        printf("ERROR: Can't open file!\n");
        exit(EXIT_FAILURE);
    }

    // Inicializamos buffer y contador de espacio previo
    // para la función de contar palabras.
    wint_t b; 
    int prevSpace;

    char buffer[8];

    while ((b = fgetwc(file)) != WEOF) {
        fileSpecs.chars++; // Incrementamos los bytes siempre

        fileSpecs.bytes += wctomb(buffer, b);

        if(b == L'\n'){
            fileSpecs.lines++; // Si el caracter es salto de línea
                               // Incrementamos lineas
        }
        
        // Iniciamos el contador de espacios, tiene un valor
        // distinto a 0 si contiene espacio, salto de linea o 
        // tabulador
        int isSpace = iswspace(b); 
        if (prevSpace && !isSpace){
            fileSpecs.words++; // Si anteriormente había un espacio y 
                               // ahora no, es que estamos en una palabra
                               // nueva, aumentamos el contador de palabras
        }
        prevSpace = isSpace; // Le damos el valor del espacio actual al 
                             // contador de espacio anterior al acabar
                             // el bucle.
    }
    
    fclose(file);

    return fileSpecs;

}

int main(int argc, char** argv){

    setlocale(LC_ALL, "");

    /* if (argc != 3 ){ */
    /*     fprintf(stderr, "ERROR: Use --help to show usage.\n"); */ 
    /*     return EXIT_FAILURE; */
    /* } */


    /* if(strcmp("-c", argv[1]) == 0){ */
    /*     int64_t numberBytes= wordCount(argv[2]).bytes; */
    /*     printf("%ld %s\n",numberBytes,argv[2]); */
    /* } else if (strcmp("-l", argv[1]) == 0) { */
    /*     int64_t numberLines = wordCount(argv[2]).lines; */
    /*     printf("%ld %s\n",numberLines,argv[2]); */
    /* } else if (strcmp("-w", argv[1]) == 0) { */
    /*     int64_t numberWords = wordCount(argv[2]).words; */
    /*     printf("%ld %s\n",numberWords,argv[2]); */
    /* } else if (strcmp("-m", argv[1]) == 0) { */
    /*     int64_t numberChars = wordCount(argv[2]).chars; */
    /*     printf("%ld %s\n",numberChars,argv[2]); */
    /* } else { */
    /*     printf("Only -c implemented by the moment\n"); */
    /*     return EXIT_FAILURE; */
    /* } */
    
    struct flags { int opt; 
        bool wflag, lflag, mflag, cflag, default_flag; 
        FileSpecs specs;
    };

    struct flags f = {
        .default_flag = true
    };

    while ((f.opt = getopt(argc, argv, "wlmc")) != 1) {
        switch (f.opt) {
            case 'w':
                f.wflag = true;
                f.default_flag = false;
                break;
            case 'l':
                f.lflag = true;
                f.default_flag = false;
                break;
            case 'm':
                f.mflag = true;
                f.default_flag = false;
                break;
            case 'c':
                f.cflag = true;
                f.default_flag = false;
                break;
            case '?':
                if (optopt == 'w' || optopt == 'l' || optopt == 'm' || optopt == 'c' )
                    fprintf(stderr,"La opción -%c requiere un argumento\n", optopt);
                else
                 fprintf(stderr, "Opción desconocida -%c\n", optopt);
                return 1;
        }
    }
    for (int i = optind; i < argc; i++) {
        f.specs = wordCount(argv[i]);    
        // Si no se especificó ninguna opción, mostrar todo (acción por defecto)
        if (f.default_flag) {
            printf("%ld %ld %ld %ld %s\n", f.specs.lines, f.specs.words, f.specs.chars, f.specs.bytes, argv[i]);
        } else {
            // Mostrar solo las estadísticas solicitadas
            if (f.lflag) printf("%ld ", f.specs.lines);
            if (f.wflag) printf("%ld ", f.specs.words);
            if (f.mflag) printf("%ld ", f.specs.chars);
            if (f.cflag) printf("%ld ", f.specs.bytes);
            printf("%s\n", argv[i]);
        }
    }
    return 0;

}
