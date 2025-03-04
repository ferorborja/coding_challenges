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

// Podrían ser variables individuales pero
// lo meto todo en un struct hell yeah
struct flags { int opt; 
    bool wflag, lflag, mflag, cflag, default_flag; 
    FileSpecs specs;
};

FileSpecs wordCount(char* filename){

    setlocale(LC_ALL, "");

    FileSpecs fileSpecs = { .bytes = 0 , .lines = 0 , .words = 0 , .chars = 0 };
    FILE* file;
    if (filename == NULL){
        file = stdin;
    }else if (filename) {
        file = fopen(filename, "r");
        if(!file){
            printf("ERROR: Can't open file!\n");
            exit(EXIT_FAILURE);
        }
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
    
    if(file != stdin) fclose(file);

    return fileSpecs;

}

void printWc(struct flags *args, char* filename){
    if(filename == NULL) filename = "";
    if (args->default_flag) {
        printf("%ld %ld %ld %s\n", args->specs.lines, args->specs.words, args->specs.bytes, filename);
    } else {
        // Mostrar solo las estadísticas solicitadas
        if (args->lflag) printf("%ld ", args->specs.lines);
        if (args->wflag) printf("%ld ", args->specs.words);
        if (args->mflag) printf("%ld ", args->specs.chars);
        if (args->cflag) printf("%ld ", args->specs.bytes);
        printf("%s\n", filename);
    }
}

int main(int argc, char** argv){

    setlocale(LC_ALL, "");

    struct flags f = {
        .default_flag = true
    };

    while ((f.opt = getopt(argc, argv, "wlmc")) != -1) {
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
    if(optind == argc){
        if(isatty(STDIN_FILENO)){
            fprintf(stderr,"USO: Proporciona un archivo o entrada por stdin");
            return EXIT_FAILURE;
        }
        f.specs = wordCount(NULL);
        printWc(&f,NULL);
        return 0;
    } else {
        for (int i = optind; i < argc; i++) {
            f.specs = wordCount(argv[i]);    
            printWc(&f, argv[i]);
        }
    }
    return 0;

}
