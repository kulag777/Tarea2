

Para ejecutar la tarea2 primero debemos compilar (en la carpeta raíz)
````
gcc tdas/*.c tarea2.c -Wno-unused-result -o tarea2
````

Y luego ejecutar:
````
./tarea2
````

Funcionalidades
Menú Principal: La función mostrarMenuPrincipal muestra las opciones disponibles en el menú principal.
El usuario puede elegir entre cargar películas, buscar por distintos criterios y salir del programa.

Cargar Películas: La función cargar_peliculas carga películas desde un archivo CSV y las inserta en varios mapas para búsquedas posteriores. Los mapas incluyen categorías como ID, género, director, década y calificación.

Búsqueda de Películas: Hay varias funciones que permiten buscar películas:

buscar_por_id: Busca una película por su ID y muestra su título y año.

buscar_por_director: Busca todas las películas dirigidas por un director específico.

buscar_por_genero: Encuentra todas las películas de un género específico.

buscar_por_decada: Encuentra películas por década.

buscar_por_rating: Busca películas dentro de un rango de calificación.

buscar_por_decada_y_genero: Busca películas que coincidan con una combinación de década y género.
