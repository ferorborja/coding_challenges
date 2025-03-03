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
    
    struct flags { int opt; int64_t words, lines, chars, bytes; };

    struct flags f;

    while ((f.opt = getopt(argc, argv, ":w:l:m:c:"))) {
        switch (f.opt) {
            case 'w':
                f.words = wordCount(optarg).words;
                printf("%ld %s\n",f.words,optarg);
                break;
            case 'l':
                f.lines = wordCount(optarg).lines;
                printf("%ld %s\n",f.lines,optarg);
                break;
            case 'm':
                f.chars = wordCount(optarg).chars;
                printf("%ld %s\n",f.chars,optarg);
                break;
            case 'c':
                f.bytes = wordCount(optarg).bytes;
                printf("%ld %s\n",f.bytes,optarg);
                break;
            case '?':
                if (optopt == 'w' || optopt == 'l' || optopt == 'm' || optopt == 'c' )
                    fprintf(stderr,"La opción -%c requiere un argumento\n", optopt);
                else
                 fprintf(stderr, "Opción desconocida -%c\n", optopt);
                return 1;
            default:
                fprintf(stderr,"USO: %s [-w fichero] [-c fichero] [-l fichero] [-b fichero] [fichero]\n", argv[0]);
                return 1;
        }
    }
    for (int i = optind; i < argc; i++){
            f.words = wordCount(argv[i]).words;
            f.lines = wordCount(argv[i]).lines;
            f.bytes = wordCount(argv[i]).bytes;
            printf("%ld %ld %ld %s", f.lines, f.words, f.bytes, argv[i]);
    }

    return 0;

}
