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

// Función para convertir grados a radianes
double degToRad(double degrees) {
    return degrees * PI / 180.0;
}

// Función para convertir radianes a grados
double radToDeg(double radians) {
    return radians * 180.0 / PI;
}

// Función para calcular la declinación solar
double calcularDeclinacionSolar(int diaDelAno) {
    return 23.45 * sin(degToRad((360.0 / 365.0) * (diaDelAno - 81)));
}

// Función para calcular la hora solar
double calcularHoraSolar(double longitud, double horaLocal, int diaDelAno) {
    double B = (360.0 / 365.0) * (diaDelAno - 81);
    double EoT = 9.87 * sin(degToRad(2 * B)) - 7.53 * cos(degToRad(B)) - 1.5 * sin(degToRad(B));
    double ajusteLongitud = 4.0 * longitud;
    return horaLocal + (EoT - ajusteLongitud) / 60.0;
}

// Función para calcular la elevación solar
double calcularElevacionSolar(double latitud, double longitud, double horaLocal, int diaDelAno) {
    double declinacion = calcularDeclinacionSolar(diaDelAno);
    double horaSolar = calcularHoraSolar(longitud, horaLocal, diaDelAno);

    double latRad = degToRad(latitud);
    double decRad = degToRad(declinacion);
    double horaSolarRad = degToRad(15.0 * (horaSolar - 12.0));

    double sinElevacion = sin(latRad) * sin(decRad) + cos(latRad) * cos(decRad) * cos(horaSolarRad);
    return radToDeg(asin(sinElevacion));
}

// Función para determinar la orientación (azimut) del sol
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

// Función para obtener la fecha y hora del sistema
FechaHora obtenerFechaHoraSistema() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    int diaDelAno = tm.tm_yday + 1; // tm_yday cuenta desde 0
    double horaLocal = tm.tm_hour + tm.tm_min / 60.0 + tm.tm_sec / 3600.0;

    FechaHora fechaHora = {diaDelAno, horaLocal};
    return fechaHora;
}

// Función para calcular la posición solar
PosicionSolar calcularPosicionSolar(Coordenadas coordenadas, FechaHora fechaHora) {
    PosicionSolar posicion;
    posicion.elevacion = calcularElevacionSolar(coordenadas.latitud, coordenadas.longitud, fechaHora.horaLocal, fechaHora.diaDelAno);
    posicion.azimut = calcularAzimutSolar(coordenadas.latitud, coordenadas.longitud, fechaHora.horaLocal, fechaHora.diaDelAno);
    return posicion;
}

// Función para simular la orientación del panel solar (aquí se podría implementar el control real)
void orientarPanelSolar(PosicionSolar posicion) {
    printf("Orientando panel solar...\n");
    printf("Elevación: %.2f grados\n", posicion.elevacion);
    printf("Azimut: %.2f grados\n", posicion.azimut);
}

int main() {
    Coordenadas coordenadas;
    FechaHora fechaHora;
    PosicionSolar posicion;

    // Obtener la fecha y hora del sistema
    fechaHora = obtenerFechaHoraSistema();

    // Solicitar la latitud y longitud al usuario
    printf("Ingrese la latitud: ");
    scanf("%lf", &coordenadas.latitud);

    printf("Ingrese la longitud: ");
    scanf("%lf", &coordenadas.longitud);

    // Calcular la posición solar
    posicion = calcularPosicionSolar(coordenadas, fechaHora);

    // Mostrar la posición solar
    printf("La inclinación (elevación) del sol es: %.2f grados\n", posicion.elevacion);
    printf("La orientación (azimut) del sol es: %.2f grados\n", posicion.azimut);

    // Orientar el panel solar (simulación)
    orientarPanelSolar(posicion);

    return 0;
}
