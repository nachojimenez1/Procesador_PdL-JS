// =====================================================================
//  Procesador de PdL-JS  -  Grupo 21  (curso 2026/2027)
//
//  Modulos de esta entrega: Analizador Lexico, Tabla de Simbolos y
//  Gestor de Errores.
//
//  Opciones del grupo:
//    - Sentencia repetitiva do-while         -> palabras 'do' y 'while'
//    - Asignacion con resta                  -> '-='
//    - Comentario de linea                   -> '//'
//    - Cadenas con comillas dobles           -> "..."
//    - Analisis sintactico descendente con tablas (siguiente entrega)
//  Operadores comunes elegidos: '+' (aritmetico), '<' (relacional),
//  '&&' (logico), '=' (asignacion).
//
//  El Analizador Lexico se ofrece como una funcion siguienteToken()
//  para que el Analizador Sintactico pueda pedir los tokens de uno en
//  uno en la siguiente entrega.
//
//  Compilar (desde la carpeta que contiene analizador.cpp y TS_CPP/):
//    g++ -std=c++17 -O2 -I TS_CPP -I TS_CPP/Include -o analizador ^
//        analizador.cpp TS_CPP/TSManager.cpp TS_CPP/Include/*.cpp
//  (o ejecutar compilar.bat en Windows)
//  Uso:       analizador fichero_fuente.txt
//  Genera:    tokens.txt, ts.txt y errores.txt
// =====================================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <algorithm>

#include "TS_CPP/TSManager.h"

using namespace std;

// =====================================================================
// GESTOR DE ERRORES
// =====================================================================
enum class TipoError { LEXICO, SINTACTICO, SEMANTICO };

class GestorErrores {
public:
    void notificar(TipoError tipo, int linea, const string& mensaje) {
        errores.push_back({tipo, linea, mensaje});
        cout << formatear(errores.back()) << '\n';   // tambien por pantalla
    }

    bool hayErrores() const { return !errores.empty(); }

    void volcar(ostream& out) const {
        if (errores.empty()) {
            out << "No se han detectado errores.\n";
            return;
        }
        for (const auto& e : errores) out << formatear(e) << '\n';
    }

private:
    struct Error {
        TipoError tipo;
        int linea;
        string mensaje;
    };
    vector<Error> errores;

    static string nombre(TipoError t) {
        switch (t) {
            case TipoError::LEXICO:     return "lexico";
            case TipoError::SINTACTICO: return "sintactico";
            default:                    return "semantico";
        }
    }

    static string formatear(const Error& e) {
        return "Error " + nombre(e.tipo) + " en la linea " + to_string(e.linea) + ": " + e.mensaje;
    }
};

// =====================================================================
// TABLA DE SIMBOLOS
// Se usa la libreria TS_CPP (https://github.com/vacmg/TS_CPP) para
// almacenar las tablas. Esta clase la envuelve para:
//   - insertar un lexema solo si no existe (la libreria da un aviso por
//     pantalla si se intenta crear dos veces la misma entrada),
//   - volcar las tablas con el formato pedido:
//         #1:
//         * 'lexema'
// En esta entrega el Analizador Lexico solo inserta lexemas en la tabla
// global. Los atributos (tipo, desplazamiento, parametros...) los
// rellenara el Analizador Semantico en entregas posteriores.
// =====================================================================
class GestorTS {
public:
    GestorTS() : manager(1) {
        tablaGlobal = manager.createNewTable("GLOBAL");
        tablaActiva = tablaGlobal;
    }

    // Inserta el lexema en la tabla activa si no estaba y devuelve el
    // identificador de su entrada (es el atributo del token ID)
    int insertar(const string& lexema) {
        TS_CPP::Entry* e = tablaActiva->searchEntry(lexema);
        if (e == nullptr) e = tablaActiva->createNewEntry(lexema);
        return e->getId();
    }

    // Devuelve el identificador de la entrada o -1 si no existe en la tabla activa
    int buscar(const string& lexema) {
        TS_CPP::Entry* e = tablaActiva->searchEntry(lexema);
        return e == nullptr ? -1 : e->getId();
    }

    TS_CPP::SymbolTable* global() { return tablaGlobal; }
    TS_CPP::SymbolTable* activa() { return tablaActiva; }

    // Vuelca una tabla con el formato de la practica
    static void volcar(TS_CPP::SymbolTable* tabla, ostream& out) {
        out << "#" << tabla->getId() << ":\n";
        for (TS_CPP::Entry* e : tabla->_getEntriesStorage()) {
            if (e->_isDeleted()) continue;
            out << "* '" << e->getLexeme() << "'\n";
        }
    }

private:
    TS_CPP::TSManager manager;
    TS_CPP::SymbolTable* tablaGlobal = nullptr;
    TS_CPP::SymbolTable* tablaActiva = nullptr;
};

// =====================================================================
// ANALIZADOR LEXICO
// =====================================================================
struct Token {
    string codigo;
    string atributo;   // vacio si el token no lleva atributo
    int linea = 0;
};

// Palabras reservadas que corresponden al grupo
static const map<string, string> PALABRAS_RESERVADAS = {
    {"boolean",  "BOOLEAN"},
    {"do",       "DO"},
    {"echo",     "ECHO"},
    {"false",    "FALSE"},
    {"float",    "FLOAT"},
    {"function", "FUNCTION"},
    {"if",       "IF"},
    {"int",      "INT"},
    {"let",      "LET"},
    {"prompt",   "PROMPT"},
    {"return",   "RETURN"},
    {"string",   "STRING"},
    {"true",     "TRUE"},
    {"void",     "VOID"},
    {"while",    "WHILE"}
};

static const long   MAX_ENTERO = 32767;
static const double MAX_REAL   = 117549436.0;
static const int    MAX_CADENA = 64;

// Numero de bytes de un caracter UTF-8 a partir de su primer byte
static size_t longitudUtf8(unsigned char c) {
    if (c < 0x80)          return 1;
    if ((c >> 5) == 0x6)   return 2;
    if ((c >> 4) == 0xE)   return 3;
    if ((c >> 3) == 0x1E)  return 4;
    return 1;
}

class AnalizadorLexico {
public:
    AnalizadorLexico(const string& fuente, GestorTS& ts, GestorErrores& ge)
        : src(fuente), ts(ts), ge(ge) {}

    Token siguienteToken();
    int lineaActual() const { return linea; }

private:
    const string& src;
    GestorTS& ts;
    GestorErrores& ge;
    size_t pos = 0;
    int linea = 1;

    bool fin() const { return pos >= src.size(); }
    char ver(size_t k = 0) const { return pos + k < src.size() ? src[pos + k] : '\0'; }
    static bool esDigito(char c) { return isdigit((unsigned char)c) != 0; }
    static bool esLetra(char c)  { return isalpha((unsigned char)c) != 0; }

    Token generar(const string& codigo, const string& atributo = "") const {
        return Token{codigo, atributo, linea};
    }
    void error(const string& mensaje) { ge.notificar(TipoError::LEXICO, linea, mensaje); }

    string leerCaracter() {   // consume un caracter completo (UTF-8)
        size_t l = min(longitudUtf8((unsigned char)src[pos]), src.size() - pos);
        string s = src.substr(pos, l);
        pos += l;
        return s;
    }

    void saltarBlancosYComentarios();
    Token identificador();
    bool numero(Token& t);
    bool cadena(Token& t);
    void cadenaComillasSimples();
};

void AnalizadorLexico::saltarBlancosYComentarios() {
    while (!fin()) {
        char c = ver();
        if (c == '\n') {
            linea++;
            pos++;
        } else if (c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v') {
            pos++;
        } else if (c == '/' && ver(1) == '/') {
            // Comentario de linea: hasta el salto de linea (que no se consume aqui)
            pos += 2;
            while (!fin() && ver() != '\n') pos++;
        } else {
            break;
        }
    }
}

Token AnalizadorLexico::identificador() {
    // Letra o '_' seguido de letras, digitos o '_'
    size_t ini = pos;
    while (!fin() && (esLetra(ver()) || esDigito(ver()) || ver() == '_')) pos++;
    string lexema = src.substr(ini, pos - ini);

    auto it = PALABRAS_RESERVADAS.find(lexema);
    if (it != PALABRAS_RESERVADAS.end()) return generar(it->second);

    int p = ts.insertar(lexema);
    return generar("ID", to_string(p));
}

bool AnalizadorLexico::numero(Token& t) {
    size_t ini = pos;
    while (esDigito(ver())) pos++;

    if (ver() == '.') {
        if (esDigito(ver(1))) {
            // Constante real: digitos '.' digitos
            pos++;
            while (esDigito(ver())) pos++;
            string lexema = src.substr(ini, pos - ini);
            bool valida = true;
            try {
                if (stod(lexema) > MAX_REAL) valida = false;
            } catch (...) {
                valida = false;
            }
            if (!valida) {
                error("La constante real " + lexema + " supera el valor maximo permitido (117549436.0).");
                return false;
            }
            t = generar("CTEREAL", lexema);
            return true;
        }
        // "78." -> falta la parte decimal
        pos++;
        error("La constante real '" + src.substr(ini, pos - ini) +
              "' esta mal formada: debe haber al menos un digito despues del punto.");
        return false;
    }

    // Constante entera
    string lexema = src.substr(ini, pos - ini);
    size_t k = 0;
    while (k + 1 < lexema.size() && lexema[k] == '0') k++;   // quitar ceros a la izquierda
    string valor = lexema.substr(k);
    if (valor.size() > 5 || stol(valor) > MAX_ENTERO) {
        error("La constante entera " + lexema + " supera el valor maximo permitido (32767).");
        return false;
    }
    t = generar("CTEENTERA", valor);
    return true;
}

bool AnalizadorLexico::cadena(Token& t) {
    pos++;   // comilla de apertura
    string contenido;
    int nCaracteres = 0;

    while (!fin() && ver() != '"' && ver() != '\n' && ver() != '\r') {
        // Secuencia de escape (\n, \t, \", \\...): cuenta como un solo caracter
        if (ver() == '\\' && pos + 1 < src.size() && ver(1) != '\n' && ver(1) != '\r') {
            contenido += '\\';
            pos++;
        }
        contenido += leerCaracter();
        nCaracteres++;
    }

    if (ver() != '"') {
        error("La cadena que empieza por \"" + contenido +
              "\" no esta cerrada: falta la comilla doble final antes del fin de la linea.");
        return false;
    }
    pos++;   // comilla de cierre

    if (nCaracteres > MAX_CADENA) {
        // Se informa del error pero se genera el token igualmente, para que
        // el Analizador Sintactico pueda seguir sin errores en cascada
        error("La cadena \"" + contenido + "\" es demasiado larga (tiene " + to_string(nCaracteres) +
              " caracteres y el maximo permitido es 64).");
    }
    t = generar("CADENA", "\"" + contenido + "\"");
    return true;
}

void AnalizadorLexico::cadenaComillasSimples() {
    // En este lenguaje las cadenas van con comillas dobles. Se salta la
    // cadena completa para dar un unico error en vez de muchos.
    pos++;
    while (!fin() && ver() != '\'' && ver() != '\n') leerCaracter();
    if (ver() == '\'') pos++;
    error("Las cadenas deben escribirse entre comillas dobles (\"...\"), no entre comillas simples.");
}

Token AnalizadorLexico::siguienteToken() {
    while (true) {
        saltarBlancosYComentarios();
        if (fin()) return generar("EOF");

        char c = ver();
        Token t;

        if (esLetra(c) || c == '_') return identificador();

        if (esDigito(c)) {
            if (numero(t)) return t;
            continue;
        }

        if (c == '"') {
            if (cadena(t)) return t;
            continue;
        }

        switch (c) {
            case '=': pos++; return generar("ASIG");
            case '+': pos++; return generar("SUMA");
            case '<': pos++; return generar("MENOR");
            case ';': pos++; return generar("PUNTOCOMA");
            case ',': pos++; return generar("COMA");
            case '(': pos++; return generar("PARABRE");
            case ')': pos++; return generar("PARCIERRA");
            case '{': pos++; return generar("LLAVEABRE");
            case '}': pos++; return generar("LLAVECIERRA");

            case '-':
                if (ver(1) == '=') { pos += 2; return generar("ASIGRESTA"); }
                pos++;
                error("El caracter '-' no esta permitido; el unico operador que lo usa es '-='.");
                continue;

            case '&':
                if (ver(1) == '&') { pos += 2; return generar("AND"); }
                pos++;
                error("El caracter '&' no esta permitido; el operador logico es '&&'.");
                continue;

            case '/':
                if (ver(1) == '*') {
                    pos += 2;
                    error("Los comentarios de bloque '/*' no estan permitidos; use comentarios de linea '//'.");
                } else {
                    pos++;
                    error("El caracter '/' no esta permitido; los comentarios se escriben con '//'.");
                }
                continue;

            case '.':
                if (esDigito(ver(1))) {
                    size_t ini = pos;
                    pos++;
                    while (esDigito(ver())) pos++;
                    error("La constante real '" + src.substr(ini, pos - ini) +
                          "' esta mal formada: debe haber al menos un digito antes del punto.");
                } else {
                    pos++;
                    error("El caracter '.' no esta permitido en este lenguaje.");
                }
                continue;

            case '\'':
                cadenaComillasSimples();
                continue;

            default:
                break;
        }

        // Cualquier otro caracter no pertenece al lenguaje
        unsigned char uc = (unsigned char)c;
        string car = leerCaracter();
        if (uc < 32 || uc == 127) {
            error("Caracter de control no permitido (codigo " + to_string((int)uc) + ").");
        } else {
            error("El caracter '" + car + "' no pertenece al lenguaje.");
        }
    }
}

// =====================================================================
// PROGRAMA PRINCIPAL
// =====================================================================
int main(int argc, char* argv[]) {
    string nombreEntrada = (argc > 1) ? argv[1] : "entrada.txt";

    ifstream entrada(nombreEntrada, ios::binary);
    if (!entrada.is_open()) {
        cerr << "No se pudo abrir el fichero fuente '" << nombreEntrada << "'.\n";
        return 1;
    }
    string fuente((istreambuf_iterator<char>(entrada)), istreambuf_iterator<char>());
    entrada.close();

    // Quitar la marca BOM de UTF-8 si el editor la ha puesto
    if (fuente.size() >= 3 && (unsigned char)fuente[0] == 0xEF &&
        (unsigned char)fuente[1] == 0xBB && (unsigned char)fuente[2] == 0xBF) {
        fuente.erase(0, 3);
    }

    GestorErrores gestorErrores;
    GestorTS gestorTS;
    AnalizadorLexico lexico(fuente, gestorTS, gestorErrores);

    // Fichero de tokens: se piden los tokens de uno en uno, como hara el
    // Analizador Sintactico en la siguiente entrega
    ofstream fTokens("tokens.txt");
    Token t;
    do {
        t = lexico.siguienteToken();
        fTokens << "<" << t.codigo << ", " << t.atributo << ">\n";
    } while (t.codigo != "EOF");
    fTokens.close();

    // Volcado de la Tabla de Simbolos (al terminar, antes de destruirla)
    ofstream fTS("ts.txt");
    GestorTS::volcar(gestorTS.global(), fTS);
    fTS.close();

    // Fichero de errores: se crea siempre para no dejar uno antiguo
    ofstream fErr("errores.txt");
    gestorErrores.volcar(fErr);
    fErr.close();

    if (!gestorErrores.hayErrores()) {
        cout << "Analisis completado sin errores.\n";
    }
    cout << "Ficheros generados: tokens.txt, ts.txt y errores.txt\n";
    return 0;
}
