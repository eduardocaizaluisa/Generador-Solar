#include <stdio.h>
#include <math.h>
#include <time.h>

#define PI 3.14159265358979323846

typedef struct {
    double latitud;
    double longitud;
} Coordenadas;

typedef struct {
    double elevacion;
    double azimut;
} PosicionSolar;

typedef struct {
    int diaDelAno;
    double horaLocal;
} FechaHora;

// En esta parte del código se convierte grados a radianes
double degToRad(double degrees) {
    return degrees * PI / 180.0;
}

// En esta parte del código se convierte radianes a grados
double radToDeg(double radians) {
    return radians * 180.0 / PI;
}

// En esta parte del código se calcula la declinación solar
double calcularDeclinacionSolar(int diaDelAno) {
    return 23.45 * sin(degToRad((360.0 / 365.0) * (diaDelAno - 81)));
}

// En esta parte del código se calcula la hora solar
double calcularHoraSolar(double longitud, double horaLocal, int diaDelAno) {
    double B = (360.0 / 365.0) * (diaDelAno - 81);
    double EoT = 9.87 * sin(degToRad(2 * B)) - 7.53 * cos(degToRad(B)) - 1.5 * sin(degToRad(B));
    double ajusteLongitud = 4.0 * longitud;
    return horaLocal + (EoT - ajusteLongitud) / 60.0;
}

// En esta parte del código se calcula la elevación solar
double calcularElevacionSolar(double latitud, double longitud, double horaLocal, int diaDelAno) {
    double declinacion = calcularDeclinacionSolar(diaDelAno);
    double horaSolar = calcularHoraSolar(longitud, horaLocal, diaDelAno);

    double latRad = degToRad(latitud);
    double decRad = degToRad(declinacion);
    double horaSolarRad = degToRad(15.0 * (horaSolar - 12.0));

    double sinElevacion = sin(latRad) * sin(decRad) + cos(latRad) * cos(decRad) * cos(horaSolarRad);
    return radToDeg(asin(sinElevacion));
}

// En esta parte del código se calcula el azimut solar
double calcularAzimutSolar(double latitud, double longitud, double horaLocal, int diaDelAno) {
    double declinacion = calcularDeclinacionSolar(diaDelAno);
    double elevacion = calcularElevacionSolar(latitud, longitud, horaLocal, diaDelAno);
    double latRad = degToRad(latitud);
    double decRad = degToRad(declinacion);
    double elevacionRad = degToRad(elevacion);

    double sinAzimut = -cos(decRad) * sin(degToRad(15.0 * (horaLocal - 12.0))) / cos(elevacionRad);
    double cosAzimut = (sin(elevacionRad) * sin(latRad) - sin(decRad)) / (cos(elevacionRad) * cos(latRad));

    double azimut = radToDeg(atan2(sinAzimut, cosAzimut));
    if (azimut < 0) {
        azimut += 360;
    }
    return azimut;
}

// En esta parte del código se obtiene la fecha y hora del sistema
FechaHora obtenerFechaHoraSistema() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    int diaDelAno = tm.tm_yday + 1; // tm_yday cuenta desde 0
    double horaLocal = tm.tm_hour + tm.tm_min / 60.0 + tm.tm_sec / 3600.0;

    FechaHora fechaHora = {diaDelAno, horaLocal};
    return fechaHora;
}

// En esta parte del código se calcula la posición solar
PosicionSolar calcularPosicionSolar(Coordenadas coordenadas, FechaHora fechaHora) {
    PosicionSolar posicion;
    posicion.elevacion = calcularElevacionSolar(coordenadas.latitud, coordenadas.longitud, fechaHora.horaLocal, fechaHora.diaDelAno);
    posicion.azimut = calcularAzimutSolar(coordenadas.latitud, coordenadas.longitud, fechaHora.horaLocal, fechaHora.diaDelAno);
    return posicion;
}

// En esta parte del código se simula la orientación del panel solar
void orientarPanelSolar(PosicionSolar posicion) {
    printf("Orientando panel solar...\n");
    printf("Elevacion: %.2f grados\n", posicion.elevacion);
    printf("Azimut: %.2f grados\n", posicion.azimut);
}

// En esta parte del código se valida que la entrada sea un número válido
int validarEntrada(double *valor) {
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (sscanf(buffer, "%lf", valor) == 1) {
            return 1; // Entrada válida
        }
    }
    return 0; // Entrada no válida
}

int main() {
    Coordenadas coordenadas;
    FechaHora fechaHora;
    PosicionSolar posicion;
    char modo;

    // aqui se obtiene la fecha y hora del sistema
    fechaHora = obtenerFechaHoraSistema();

    // este tiene la funcion de seleccionar modo de operación
    printf("Seleccione el modo de operacion (A para automatico, M para manual): ");
    scanf(" %c", &modo);
    getchar(); // Limpiar el buffer

    if (modo == 'A' || modo == 'a') {
        // Modo automático (coordenadas de Loja, Ecuador)
        coordenadas.latitud = -3.99313;
        coordenadas.longitud = -79.20422;
        printf("Usando coordenadas de Loja, Ecuador.\n");
    } else if (modo == 'M' || modo == 'm') {
        // Modo manual
        // Solicitar y validar la latitud
        while (1) {
            printf("Ingrese la latitud (-90 a 90): ");
            if (validarEntrada(&coordenadas.latitud) && coordenadas.latitud >= -90 && coordenadas.latitud <= 90) {
                break; // Entrada válida
            }
            printf("Entrada no válida. Por favor, ingrese un valor entre -90 y 90.\n");
        }

        // aqui se solicita  y valida la longitud
        while (1) {
            printf("Ingrese la longitud (-180 a 180): ");
            if (validarEntrada(&coordenadas.longitud) && coordenadas.longitud >= -180 && coordenadas.longitud <= 180) {
                break; // Entrada válida
            }
            printf("Entrada no válida. Por favor, ingrese un valor entre -180 y 180.\n");
        }
    } else {
        printf("Modo no válido. Terminando programa.\n");
        return 1;
    }

    // sirve para calcular la posición solar
    posicion = calcularPosicionSolar(coordenadas, fechaHora);

    // nos muestra por pantalla la posición solar
    printf("La inclinacion (elevacion) del sol es: %.2f grados\n", posicion.elevacion);
    printf("La orientacion (azimut) del sol es: %.2f grados\n", posicion.azimut);

    // esto es lo que haria el panel solar | (simulación)
    orientarPanelSolar(posicion);

    return 0;
}
