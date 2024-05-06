#integrantes 
CRISTOBAL MARTINEZ,
BENJAMIN ESPINOZA

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

buscar_por_decada: Encuentra películas por década. (se tiene que poner como 1990, 1970, y asi), no funciona si se escribe el anho mal.

buscar_por_rating: Busca películas dentro de un rango de calificación.

buscar_por_decada_y_genero: Busca películas que coincidan con una combinación de década y género.

observaciones: NO FUNCIONAN LAS SIGUIENTES OPCIONES DEL MENU. opcion de rango (6) por calificaciones no funciona y realmente no sabemos cual es el error, lo mismo con la opcion (7) de decada y genero 
