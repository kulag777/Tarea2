#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char id[100];
  char title[100];
  List *genres;
  char director[300];
  float rating;
  int year;
} Film;

// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("     Base de Datos de Películas");
  puts("========================================");

  puts("1) Cargar Películas");
  puts("2) Buscar por id");
  puts("3) Buscar por director");
  puts("4) Buscar por género");
  puts("5) Buscar por década");
  puts("6) Buscar por rango de calificaciones");
  puts("7) Buscar por década y género");
  puts("8) Salir");
}

/**
 * Compara dos claves de tipo string para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo string.
 *
 * @param key1 Primer puntero a la clave string.
 * @param key2 Segundo puntero a la clave string.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

/**
 * Compara dos claves de tipo entero para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo entero.
 *
 * @param key1 Primer puntero a la clave entera.
 * @param key2 Segundo puntero a la clave entera.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_int(void *key1, void *key2) {
  return *(int *)key1 == *(int *)key2; // Compara valores enteros directamente
}

/**
 * Carga películas desde un archivo CSV y las almacena en un mapa por ID.
 */
void cargar_peliculas(Map *pelis_byid, Map *pelis_bygenero,
                      Map *pelis_bydirector, Map *pelis_bydecada,
                      Map *pelis_byrating) {
  // Intenta abrir el archivo CSV que contiene datos de películas
  FILE *archivo = fopen("data/Top1500.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  char **campos;
  // Leer y parsear una línea del archivo CSV. La función devuelve un array de
  // strings, donde cada elemento representa un campo de la línea CSV procesada.
  campos = leer_linea_csv(archivo, ','); // Lee los encabezados del CSV

  // Lee cada línea del archivo CSV hasta el final
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    // Crea una nueva estructura Film y almacena los datos de cada película
    Film *peli = (Film *)malloc(sizeof(Film));
    strcpy(peli->id, campos[1]);        // Asigna ID
    strcpy(peli->title, campos[5]);     // Asigna título
    strcpy(peli->director, campos[14]); // Asigna director
    peli->genres = list_create();       // Inicializa la lista de géneros
    peli->year =
        atoi(campos[10]); // Asigna año, convirtiendo de cadena a entero

    // Inserta la película en el mapa usando el ID como clave
    map_insert(pelis_byid, peli->id, peli);

    // Inserta la película a la lista de peliculas que hay en el mapa usando el
    // director como clave
    char *directores = strdup(peli->director);
    char *comma_check_directores = strchr(directores, ',');
    if (comma_check_directores == NULL) {
      if (map_search(pelis_bydirector, directores) == NULL) {
        List *pelis_del_director = list_create();
        map_insert(pelis_bydirector, directores, pelis_del_director);
      }
      List *pelis_del_director =
          map_search(pelis_bydirector, directores)->value;
      list_pushBack(pelis_del_director, peli);
    } else {
      char *token = strtok(directores, ",");
      while (token != NULL) {
        if (map_search(pelis_bydirector, token) == NULL) {
          List *pelis_del_director = list_create();
          map_insert(pelis_bydirector, token, pelis_del_director);
        }
        List *pelis_del_director = map_search(pelis_bydirector, token)->value;
        list_pushBack(pelis_del_director, peli);
        token = strtok(NULL, ",");
      }
    }

    // Inserta la pelicula a la lista de peliculas que hay en el mapa usando el
    // genero como clave
    char *generos = strdup(campos[11]);
    char *comma_check_genres = strchr(generos, ',');
    if (comma_check_genres == NULL) {
      if (map_search(pelis_bygenero, generos) == NULL) {
        List *pelis_del_genero = list_create();
        map_insert(pelis_bygenero, generos, pelis_del_genero);
      }
      List *pelis_del_genero = map_search(pelis_bygenero, generos)->value;
      list_pushBack(pelis_del_genero, peli);
    } else {
      char *token = strtok(generos, ", ");
      while (token != NULL) {
        if (map_search(pelis_bygenero, token) == NULL) {
          List *pelis_del_genero = list_create();
          map_insert(pelis_bygenero, token, pelis_del_genero);
        }
        List *pelis_del_genero = map_search(pelis_bygenero, token)->value;
        list_pushBack(pelis_del_genero, peli);
        token = strtok(NULL, ", ");
      }
    }
    // Inserta la película a la lista de peliculas que hay en el mapa usando una
    // decada como clave

    int decadaInt = (peli->year / 10) * 10; // Calcular la década
    char decada[100]; // Tamaño ajustado para guardar una década
    sprintf(decada, "%d", decadaInt); // Convertir a cadena

    MapPair *pair = map_search(pelis_bydecada, decada);

    if (pair == NULL) {
      // Si no existe, crear y agregar la lista al mapa
      List *pelis_decada = list_create();
      map_insert(pelis_bydecada, decada, pelis_decada);
      list_pushBack(pelis_decada, peli); // Insertar la película
    } else {
      // Si existe, obtener la lista y agregar la película
      List *pelis_decada = pair->value;
      list_pushBack(pelis_decada, peli);
    }

    char rate[3];

    if (peli->rating >= 0.0 && peli->rating < 1.0) {
      strcpy(rate, "1");
    } else if (peli->rating >= 1.0 && peli->rating < 2.0) {
      strcpy(rate, "2");
    } else if (peli->rating >= 2.0 && peli->rating < 3.0) {
      strcpy(rate, "3");
    } else if (peli->rating >= 3.0 && peli->rating < 4.0) {
      strcpy(rate, "4");
    } else if (peli->rating >= 4.0 && peli->rating < 5.0) {
      strcpy(rate, "5");
    } else if (peli->rating >= 5.0 && peli->rating < 6.0) {
      strcpy(rate, "6");
    } else if (peli->rating >= 6.0 && peli->rating < 7.0) {
      strcpy(rate, "7");
    } else if (peli->rating >= 7.0 && peli->rating < 8.0) {
      strcpy(rate, "8");
    } else if (peli->rating >= 8.0 && peli->rating < 9.0) {
      strcpy(rate, "9");
    } else {
      strcpy(rate, "10");
    }

    if (map_search(pelis_byrating, rate) == NULL) {
      List *pelis_rating = list_create();
      map_insert(pelis_byrating, rate, pelis_rating);
    }
    List *pelis_rating = map_search(pelis_byrating, rate)->value;
    list_pushBack(pelis_rating, peli);
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas

  // Itera sobre el mapa para mostrar las películas cargadas
  MapPair *pair = map_first(pelis_byid);
  while (pair != NULL) {
    Film *peli = pair->value;
    printf("ID: %s, Título: %s, Director: %s, Año: %d\n", peli->id, peli->title,
           peli->director, peli->year);
    pair = map_next(pelis_byid); // Avanza al siguiente par en el mapa
  }
}

/**
 * Busca y muestra la información de una película por su ID en un mapa.
 */
void buscar_por_id(Map *pelis_byid) {
  char id[10]; // Buffer para almacenar el ID de la película

  // Solicita al usuario el ID de la película
  printf("Ingrese el id de la película: ");
  scanf("%s", id); // Lee el ID del teclado

  // Busca el par clave-valor en el mapa usando el ID proporcionado
  MapPair *pair = map_search(pelis_byid, id);

  // Si se encontró el par clave-valor, se extrae y muestra la información de la
  // película
  if (pair != NULL) {
    Film *peli =
        pair->value; // Obtiene el puntero a la estructura de la película
    // Muestra el título y el año de la película
    printf("Título: %s, Año: %d\n", peli->title, peli->year);
  } else {
    // Si no se encuentra la película, informa al usuario
    printf("La película con id %s no existe\n", id);
  }
}
void buscar_por_director(Map *pelis_bydirector) {
  char director[100];

  printf("Ingrese el director de la película: ");
  scanf(" %[^\n]", director);

  MapPair *pair = map_search(pelis_bydirector, director);
  if (pair != NULL) {
    List *pelis_del_director = pair->value;
    printf("Películas del director %s:\n", director);
    for (Film *peli = list_first(pelis_del_director); peli != NULL;
         peli = list_next(pelis_del_director)) {
      printf("Título: %s, Año: %d\n", peli->title, peli->year);
    }
  } else {
    // Si no se encuentra la película, informa al usuario
    printf("La película con ese director no existe\n");
  }
}
void buscar_por_genero(Map *pelis_bygenero) {
  char genero[100];
  printf("Ingrese el género de la película: ");
  scanf("%s", genero);

  MapPair *pair = map_search(pelis_bygenero, genero);
  if (pair != NULL) {
    List *pelis_del_genero = pair->value;
    printf("Películas del género %s:\n", genero);
    for (Film *peli = list_first(pelis_del_genero); peli != NULL;
         peli = list_next(pelis_del_genero))
      printf("Título: %s, Año: %d\n", peli->title, peli->year);
  } else {
    // Si no se encuentra la película, informa al usuario
    printf("No existen peliculas con ese género\n");
  }
}
void buscar_por_decada(Map *pelis_bydecada) {
  int decada;
  int decadaCmp;
  printf("Ingrese la década de la película: ");
  scanf("%d", &decada);
  decada = (decada / 10) * 10;

  MapPair *pair = map_first(pelis_bydecada);
  while (pair != NULL) {
    Film *peli = pair->value;
    decadaCmp = (peli->year / 10) * 10;
    if (decada == decadaCmp) {
      printf("Título: %s, Año: %d\n", peli->title, peli->year);
    }
    pair = map_next(pelis_bydecada);
  }
}
void buscar_por_rating(Map *pelis_byrating) {
  char rate[3];
  printf("Rangos de calificación:\n");
  printf("1) 0.0-1.0\n2) 1.0-2.0\n3) 2.0-3.0\n4) 3.0-4.0\n5) 4.0-5.0\n6) "
         "5.0-6.0\n7) 6.0-7.0\n8) 7.0-8.0\n9) 8.0-9.0\n10) 9.0-10.0\n");
  printf("Ingrese el rango de calificación (1-10): ");
  scanf("%s", rate);

  int rate_num = atoi(rate);
  if (rate_num < 1 || rate_num > 10) {
    printf("Por favor, ingrese un número entre 1 y 10.\n");
    return;
  }

  MapPair *pair = map_search(pelis_byrating, rate);
  if (pair == NULL) {
    printf("No se encontraron películas para el rango %s.\n", rate);
    return;
  }

  List *pelis_rating = pair->value;

  Film *first_film = list_first(pelis_rating); // Obtener el primer elemento
  if (first_film == NULL) {
    printf("No hay películas para el rango %s.\n", rate); // Lista vacía
  } else {
    printf("Películas con calificación en el rango %s:\n", rate);
    for (Film *peli = first_film; peli != NULL;
         peli = list_next(pelis_rating)) {
      printf("Título: %s, Año: %d, Calificación: %.1f\n", peli->title,
             peli->year, peli->rating);
    }
  }
}

    void buscar_por_decada_y_genero(Map *pelis_bydecada, Map *pelis_bygenero) {
        int decada;
        char genero[100];

        printf("Ingrese la década de la película: ");
        scanf("%d", &decada);
        printf("Ingrese el género de la película: ");
        scanf("%s", genero);

        // Busca las listas para la década y el género
        MapPair *pair_decada = map_search(pelis_bydecada, &decada);
        MapPair *pair_genero = map_search(pelis_bygenero, genero);

        if (pair_decada != NULL && pair_genero != NULL) {
            List *pelis_decada = pair_decada->value;
            List *pelis_del_genero = pair_genero->value;

            printf("Películas de la década %d y género %s:\n", decada, genero);

            // Creamos un iterador para recorrer la lista de películas por década
            Film *pelicula_decada = list_first(pelis_decada);

            // Recorre la lista de películas por década
            while (pelicula_decada != NULL) {
                // Recorre la lista de películas por género
                for (Film *pelicula_genero = list_first(pelis_del_genero);
                     pelicula_genero != NULL;
                     pelicula_genero = list_next(pelis_del_genero)) {

                    // Si la película aparece en ambas listas, la mostramos
                    if (pelicula_decada == pelicula_genero) {
                        printf("Título: %s, Año: %d\n",
                               pelicula_decada->title, pelicula_decada->year);
                        break; // No necesitamos seguir recorriendo esta lista
                    }
                }

                pelicula_decada = list_next(pelis_decada);
            }
        } else {
            printf("No se encontraron películas para esa combinación de década y género.\n");
        }
    }


int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
               // (sin uso en este fragmento)

  // Crea un mapa para almacenar películas, utilizando una función de
  // comparación que trabaja con claves de tipo string.
  Map *pelis_byid = map_create(is_equal_str);
  Map *pelis_bygenero = map_create(is_equal_str);
  Map *pelis_bydirector = map_create(is_equal_str);
  Map *pelis_bydecada = map_create(is_equal_str);
  Map *pelis_byrating = map_create(is_equal_str);

  // Recuerda usar un mapa por criterio de búsqueda

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      cargar_peliculas(pelis_byid, pelis_bygenero, pelis_bydirector,
                       pelis_bydecada, pelis_byrating);
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
      buscar_por_decada(pelis_byid);
      break;
    case '6':
      buscar_por_rating(pelis_byrating);
      break;
    case '7':
      buscar_por_decada_y_genero(pelis_bydecada, pelis_bygenero);
      break;
    default:
    }
    presioneTeclaParaContinuar();

  } while (opcion != '8');

  return 0;
}
