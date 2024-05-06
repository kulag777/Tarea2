#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char id[100];
    char title[100];
    List *genres;// Lista de géneros asociados con la película
    char director[300];
    float rating;
    int year;
} Film;

void mostrarMenuPrincipal() {
    printf("========================================\n");
    printf("     Base de Datos de Películas\n");
    printf("========================================\n");
    printf("1) Cargar Películas\n");
    printf("2) Buscar por id\n");
    printf("3) Buscar por director\n");
    printf("4) Buscar por género\n");
    printf("5) Buscar por década\n");
    printf("6) Buscar por rango de calificaciones\n");
    printf("7) Buscar por década y género\n");
    printf("8) Salir\n");
}

int is_equal_str(void *key1, void *key2) {
    return strcmp((char *)key1, (char *)key2) == 0; // Retorna 1 si son iguales
}


void cargar_peliculas(Map *pelis_byid, Map *pelis_bygenero, Map *pelis_bydirector, Map *pelis_bydecada, Map *pelis_byrating) {
    FILE *archivo = fopen("data/Top1500.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    char **campos = leer_linea_csv(archivo, ','); // Lee encabezados
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Film *peli = (Film *)malloc(sizeof(Film));
        if (peli == NULL) {
            perror("Error de memoria");
            fclose(archivo);
            return;
        }

        strcpy(peli->id, campos[1]);
        strcpy(peli->title, campos[5]);
        strcpy(peli->director, campos[14]);
        peli->year = atoi(campos[10]);
        peli->genres = list_create();

        // Cargar géneros
        char *generos = strdup(campos[11]);
        char *token = strtok(generos, ", ");
        while (token != NULL) {
            if (map_search(pelis_bygenero, token) == NULL) {
                map_insert(pelis_bygenero, strdup(token), list_create());
            }
            List *pelis_del_genero = map_search(pelis_bygenero, token)->value;
            list_pushBack(pelis_del_genero, peli);
            token = strtok(NULL, ", ");
        }
        free(generos);

        // Cargar director
        char *directores = strdup(campos[14]);
        token = strtok(directores, ",");
        while (token != NULL) {
            if (map_search(pelis_bydirector, token) == NULL) {
                map_insert(pelis_bydirector, strdup(token), list_create());
            }
            List *pelis_del_director = map_search(pelis_bydirector, token)->value;
            list_pushBack(pelis_del_director, peli);
            token = strtok(NULL, ",");
        }
        free(directores);

        // Cargar por década
        int decadaInt = (peli->year / 10) * 10; // Década redondeada
        char decada_str[100];
        sprintf(decada_str, "%d", decadaInt);

        if (map_search(pelis_bydecada, decada_str) == NULL) {
            map_insert(pelis_bydecada, strdup(decada_str), list_create());
        }
        List *pelis_decada = map_search(pelis_bydecada, decada_str)->value;
        list_pushBack(pelis_decada, peli);

        // Calificación
        char rate[3];
        int rate_val = (int)peli->rating + 1;
        sprintf(rate, "%d", rate_val);

        if (map_search(pelis_byrating, rate) == NULL) {
            map_insert(pelis_byrating, strdup(rate), list_create());
        }
        List *pelis_rating = map_search(pelis_byrating, rate)->value;
        list_pushBack(pelis_rating, peli);

        free(campos); // Liberar recursos
    }

    fclose(archivo);
}

void buscar_por_id(Map *pelis_byid) {
    char id[10];

    printf("Ingrese el id de la película: ");
    scanf("%s", id);

    MapPair *pair = map_search(pelis_byid, id);

    if (pair != NULL) {
        Film *peli = pair->value;
        printf("Título: %s, Año: %d, Director: %s, Calificación: %.1f\n",
               peli->title, peli->year, peli->director, peli->rating);
    } else {
        printf("La película con id %s no existe\n", id);
    }
}

void buscar_por_director(Map *pelis_bydirector) {
    char director[100];

    printf("Ingrese el director de la película: ");
    scanf(" %[^\n]", director); // Leer toda la línea, incluyendo espacios

    MapPair *pair = map_search(pelis_bydirector, director);
    if (pair != NULL) {
        List *pelis_del_director = pair->value;
        printf("Películas del director %s:\n", director);
        for (Film *peli = list_first(pelis_del_director); peli != NULL;
             peli = list_next(pelis_del_director)) {
            printf("Título: %s, Año: %d, Calificación: %.1f\n",
                   peli->title, peli->year, peli->rating);
        }
    } else {
        printf("No se encontraron películas del director %s\n", director);
    }
}

void buscar_por_genero(Map *pelis_bygenero) {
    char genero[100];

    printf("Ingrese el género de la película: ");
    scanf("%s", genero); // Leer el género especificado

    MapPair *pair = map_search(pelis_bygenero, genero);
    if (pair != NULL) {
        List *pelis_del_genero = pair->value;
        printf("Películas del género %s:\n", genero);
        for (Film *peli = list_first(pelis_del_genero); peli != NULL;
             peli = list_next(pelis_del_genero)) {
            printf("Título: %s, Año: %d, Director: %s, Calificación: %.1f\n",
                   peli->title, peli->year, peli->director, peli->rating);
        }
    } else {
        printf("No se encontraron películas del género %s\n", genero);
    }
}

void buscar_por_decada(Map *pelis_bydecada) {
    int decada;

    printf("Ingrese la década de la película: ");
    scanf("%d", &decada);
    decada = (decada / 10) * 10;

    char decada_str[100];
    sprintf(decada_str, "%d", decada);

    MapPair *pair = map_search(pelis_bydecada, decada_str);

    if (pair != NULL) {
        List *pelis_decada = pair->value;
        printf("Películas de la década %d:\n", decada);

        for (Film *peli = list_first(pelis_decada); peli != NULL;
             peli = list_next(pelis_decada)) {
            printf("Título: %s, Año: %d, Director: %s, Calificación: %.1f\n",
                   peli->title, peli->year, peli->director, peli->rating);
        }
    } else {
        printf("No se encontraron películas de la década %d\n", decada);
    }
}

void buscar_por_rating(Map *pelis_byrating) {
    char rate_str[3];

    printf("Rangos de calificación:\n");
    printf("1) 0.0-1.0\n2) 1.0-2.0\n3) 2.0-3.0\n4) 3.0-4.0\n5) 4.0-5.0\n6) 5.0-6.0\n7) 6.0-7.0\n8) 7.0-8.0\n9) 8.0-9.0\n10) 9.0-10.0\n");
    printf("Ingrese el rango de calificación (1-10): ");
    scanf("%s", rate_str); // Leemos el rango

    int rate_val = atoi(rate_str);
    if (rate_val < 1 || rate_val > 10) {
        printf("Por favor, ingrese un número entre 1 y 10.\n");
        return;
    }

    MapPair *pair = map_search(pelis_byrating, rate_str);

    if (pair != NULL) {
        List *pelis_rating = pair->value;
        printf("Películas con calificación en el rango %s:\n", rate_str);

        for (Film *peli = list_first(pelis_rating); peli != NULL;
             peli = list_next(pelis_rating)) {
            printf("Título: %s, Año: %d, Director: %s, Calificación: %.1f\n",
                   peli->title, peli->year, peli->director, peli->rating);
        }
    } else {
        printf("No se encontraron películas para el rango %s\n", rate_str);
    }
}

void buscar_por_decada_y_genero(Map *pelis_bydecada, Map *pelis_bygenero) {
    int decada;
    char genero[100];

    printf("Ingrese la década de la película: ");
    scanf("%d", &decada);
    decada = (decada / 10) * 10; // Ajustamos a década redondeada

    printf("Ingrese el género de la película: ");
    scanf("%s", genero);

    char decada_str[100];
    sprintf(decada_str, "%d", decada);

    MapPair *pair_decada = map_search(pelis_bydecada, decada_str);
    MapPair *pair_genero = map_search(pelis_bygenero, genero);

    if (pair_decada != NULL && pair_genero != NULL) {
        List *pelis_decada = pair_decada->value;
        List *pelis_del_genero = pair_genero->value;

        printf("Películas de la década %d y género %s:\n", decada, genero);

        for (Film *pelicula_decada = list_first(pelis_decada);
             pelicula_decada != NULL;
             pelicula_decada = list_next(pelis_decada)) {
            for (Film *pelicula_genero = list_first(pelis_del_genero);
                 pelicula_genero != NULL;
                 pelicula_genero = list_next(pelis_del_genero)) {
                if (strcmp(pelicula_decada->id, pelicula_genero->id) == 0) { // Comparar por ID
                    printf("Título: %s, Año: %d, Director: %s, Calificación: %.1f\n",
                           pelicula_decada->title, pelicula_decada->year,
                           pelicula_decada->director, pelicula_decada->rating);
                    break;
                }
            }
        }
    } else {
        printf("No se encontraron películas para esa combinación de década y género.\n");
    }
}

int main() {
    char opcion;

    // Crea un mapa para almacenar películas, utilizando una función de comparación que trabaja con claves de tipo string
    Map *pelis_byid = map_create(is_equal_str);
    Map *pelis_bygenero = map_create(is_equal_str);
    Map *pelis_bydirector = map_create(is_equal_str);
    Map *pelis_bydecada = map_create(is_equal_str);
    Map *pelis_byrating = map_create(is_equal_str);

    do {
        mostrarMenuPrincipal();
        printf("Ingrese su opción: ");
        scanf(" %c", &opcion); // Espacio antes del formato para evitar problemas con caracteres previos

        switch (opcion) {
        case '1':
            cargar_peliculas(pelis_byid, pelis_bygenero, pelis_bydirector, pelis_bydecada, pelis_byrating);
            break;
        case '2':
            buscar_por_id(pelis_byid);
            break;
        case '3':
            buscar_por_director(pelis_bydirector);
            break;
        case '4':
            buscar_por_genero(pelis_bygenero);
            break;
        case '5':
            buscar_por_decada(pelis_bydecada);
            break;
        case '6':
            buscar_por_rating(pelis_byrating);
            break;
        case '7':
            buscar_por_decada_y_genero(pelis_bydecada, pelis_bygenero);
            break;
        default:
            printf("Opción no válida. Intente de nuevo.\n");
        }

        printf("Presione cualquier tecla para continuar...\n");
        getchar(); // Pausa para permitir al usuario ver la salida

    } while (opcion != '8'); // Salir del bucle cuando el usuario elija "Salir"

    return 0;
}
