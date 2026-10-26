# Procesador de PdL-JS

Procesador del lenguaje **PdL-JS** (una variante de JavaScript) desarrollado en C++ para la práctica de **Procesadores de Lenguajes** (curso 2026/2027) por el **Grupo 21**.

El procesador lee un programa fuente y genera los ficheros de tokens, tabla de símbolos y errores que pide la práctica. Se construye de forma incremental, entrega a entrega.

| Módulo | Estado |
| --- | --- |
| Analizador Léxico | Hecho |
| Tabla de Símbolos | Hecho (inserción de identificadores) |
| Gestor de Errores | Hecho (errores léxicos) |
| Analizador Sintáctico (descendente con tablas) | Pendiente |
| Analizador Semántico | Pendiente |

## Opciones del grupo

| Característica | Opción asignada |
| --- | --- |
| Sentencia repetitiva | `do-while` |
| Operador especial | Asignación con resta (`-=`) |
| Comentarios | De línea (`//`) |
| Cadenas | Entre comillas dobles (`"..."`) |
| Análisis sintáctico | Descendente con tablas |

Operadores comunes implementados: `+` (aritmético), `<` (relacional), `&&` (lógico) y `=` (asignación).

## Estructura del repositorio

```
.
├── analizador.cpp      # Analizador Léxico, Gestor de Errores, GestorTS y programa principal
├── TS_CPP/             # Librería de Tabla de Símbolos (MIT, Víctor Arzoz Consuegra)
├── compilar.sh         # Script de compilación para macOS / Linux
├── compilar.bat        # Script de compilación para Windows (MinGW)
└── pruebas/            # Casos de prueba con sus salidas
    └── casoN/
        ├── casoN.txt   # Programa fuente
        ├── tokens.txt  # Salida: tokens
        ├── ts.txt      # Salida: tabla de símbolos
        └── errores.txt # Salida: errores
```

## Compilación

Hace falta un compilador de C++17 (`clang++` o `g++`).

**macOS / Linux**

```sh
sh compilar.sh
```

En macOS, si no tienes compilador, instálalo una vez con `xcode-select --install`.

**Windows (MinGW)**

```bat
compilar.bat
```

**A mano** (desde la raíz del repositorio):

```sh
c++ -std=c++17 -O2 -I TS_CPP -I TS_CPP/Include -o analizador \
    analizador.cpp TS_CPP/TSManager.cpp TS_CPP/Include/*.cpp
```

## Uso

```sh
./analizador programa.txt
```

Si no se indica fichero, se lee `entrada.txt`. Los errores se muestran por pantalla en el momento en que se detectan y, al terminar, se generan tres ficheros en el directorio desde el que se ejecuta:

| Fichero | Contenido |
| --- | --- |
| `tokens.txt` | Un token por línea con el formato `<CODIGO, atributo>` |
| `ts.txt` | Volcado de la tabla de símbolos |
| `errores.txt` | Listado de errores, o `No se han detectado errores.` |

### Ejemplo

Fuente:

```js
// Comentario de linea
let total int;
total -= 5;
echo "Hola";
```

`tokens.txt`:

```
<LET, >
<ID, 1>
<INT, >
<PUNTOCOMA, >
<ID, 1>
<ASIGRESTA, >
<CTEENTERA, 5>
<PUNTOCOMA, >
<ECHO, >
<CADENA, "Hola">
<PUNTOCOMA, >
<EOF, >
```

`ts.txt`:

```
#1:
* 'total'
```

## Analizador Léxico

| Categoría | Lexemas | Token | Atributo |
| --- | --- | --- | --- |
| Palabras reservadas | `boolean do echo false float function if int let prompt return string true void while` | El nombre en mayúsculas (`BOOLEAN`, `DO`...) | — |
| Identificadores | Letra o `_`, seguido de letras, dígitos o `_` | `ID` | Posición en la TS |
| Constante entera | Dígitos, máximo `32767` | `CTEENTERA` | Valor |
| Constante real | `dígitos.dígitos`, máximo `117549436.0` | `CTEREAL` | Valor |
| Cadena | `"..."`, máximo 64 caracteres, admite secuencias de escape (`\n`, `\"`...) | `CADENA` | La cadena |
| Operadores | `+` `<` `&&` `=` `-=` | `SUMA` `MENOR` `AND` `ASIG` `ASIGRESTA` | — |
| Delimitadores | `,` `;` `(` `)` `{` `}` | `COMA` `PUNTOCOMA` `PARABRE` `PARCIERRA` `LLAVEABRE` `LLAVECIERRA` | — |
| Fin de fichero | | `EOF` | — |

El lenguaje distingue mayúsculas y minúsculas: `While` es un identificador, no la palabra reservada `while`.

## Gestor de Errores

Cada error indica su tipo, la línea y una explicación pensada para quien programa en PdL-JS:

```
Error lexico en la linea 5: La constante entera 32768 supera el valor maximo permitido (32767).
```

El análisis nunca se detiene ante un error: se descarta el texto erróneo y se continúa con el siguiente token. Errores léxicos detectados:

- Caracteres que no pertenecen al lenguaje (`*`, `#`, `ñ`...).
- `-`, `&` o `/` sueltos (los operadores del grupo son `-=`, `&&` y `//`).
- Comentarios de bloque `/* */` y cadenas con comillas simples.
- Reales mal formados (`78.`, `.378`).
- Enteros mayores que 32767 y reales mayores que 117549436.0.
- Cadenas sin cerrar y cadenas de más de 64 caracteres. Una cadena demasiado larga genera igualmente su token, para no provocar errores en cascada en el Analizador Sintáctico.

## Tabla de Símbolos

Se usa la librería [TS_CPP](https://github.com/vacmg/TS_CPP), envuelta en la clase `GestorTS`. Esta clase inserta cada identificador una sola vez y devuelve su posición, que es el atributo del token `ID`. También vuelca las tablas con el formato de la práctica. Por ahora todos los identificadores van a la tabla global (`#1`); los atributos (tipo, desplazamiento...) los añadirá el Analizador Semántico.

## Casos de prueba

| Caso | Tipo | Qué comprueba |
| --- | --- | --- |
| 1 | Correcto | Declaraciones, entrada/salida, condicional y operadores |
| 2 | Correcto | Funciones, `do-while` y `-=` |
| 3 | Correcto | Valores límite, cadena vacía, escapes y mayúsculas |
| 4 | Con errores | Constantes numéricas fuera de rango o mal formadas |
| 5 | Con errores | Comillas simples, cadena demasiado larga y cadena sin cerrar |
| 6 | Con errores | Caracteres y operadores no permitidos |

Para ejecutar un caso:

```sh
cd pruebas/caso1
../../analizador caso1.txt
```

## Autores

Grupo 21 – Procesadores de Lenguajes, curso 2026/2027:

- Fabián Alonso Valle
- Ignacio Jiménez Martín
- Hugo Platón Baña

## Licencia

Este proyecto se distribuye bajo la licencia MIT (ver [LICENSE](LICENSE)). La librería TS_CPP es de Víctor Arzoz Consuegra y también tiene licencia MIT (ver [TS_CPP/LICENSE](TS_CPP/LICENSE)).
