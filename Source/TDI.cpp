

#include <math.h>
#include <fcntl.h>

#include <time.h>

#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>

#include <iostream>

string nombreImg, nombreImgCompleto;
C_Image imagen, imgSobelX, imgSobelY, imgFinal, original;
int umbral;

void Sobelx();
void Sobely();
void convolucion(C_Image imagen, C_Matrix mascara, string eje);
void umbralizar(int umbral);
void suavizadoGaussiano(C_Image imagen, double sigma);
void magnitudGradientes();
void invertirColores();


int main(int argc, char** argv)
{

    cout << "---------------------------------------------" << endl;
    cout << "|        Deteccion de bordes con Sobel      |" << endl;
    cout << "|       Realizado por Alejandro MT          |" << endl;
    cout << "---------------------------------------------" << endl;

    cout << "Introduzca el nombre de la imagen que quieres procesa\n";
    cout << "(La imagen debe estar en la carpeta del proyecto)\n";
    cin >> nombreImg;
    nombreImgCompleto = nombreImg + ".bmp";
    imagen.ReadBMP (&nombreImgCompleto[0]);
    if (imagen.Fail()) {
        cout << "Error al cargar la imagen";
        exit;
    } else {
        cout << "Se ha cargado la imagen correctamente\n";
        original = imagen;
        /*suavizadoGaussiano(imagen, 1.0);*/
        Sobelx();
        Sobely();
        cout << "Introduce el umbral\n";
        imgSobelX.WriteBMP("Prueba1.bmp");
        imgSobelY.WriteBMP("Prueba2.bmp");
        imgFinal = imagen;
        magnitudGradientes();
        invertirColores();
        imgFinal.WriteBMP("Final.bmp");
    }
}

void Sobelx() {
    C_Matrix Msobel(0, 2, 0, 2, 0);
    Msobel(0, 0) = -1;
    Msobel(1, 0) = -2;
    Msobel(2, 0) = -1;
    Msobel(0, 1) = 0;
    Msobel(1, 1) = 0;
    Msobel(2, 1) = 0;
    Msobel(0, 2) = 1;
    Msobel(1, 2) = 2;
    Msobel(2, 2) = 1;

    convolucion(imagen, Msobel, "x");
}

void Sobely() {
    C_Matrix Msobel(0, 2, 0, 2, 0);
    Msobel(0, 0) = -1;
    Msobel(1, 0) = 0;
    Msobel(2, 0) = 1;
    Msobel(0, 1) = -2;
    Msobel(1, 1) = 0;
    Msobel(2, 1) = 2;
    Msobel(0, 2) = -1;
    Msobel(1, 2) = 0;
    Msobel(2, 2) = 1;
    convolucion(imagen, Msobel, "y");
}

void convolucion(C_Image imagen, C_Matrix mascara, string eje) {
    // Ampliar la imagen en los bordes duplicando los valores
    imagen.Resize(imagen.FirstRow() - 1, imagen.LastRow() + 1, imagen.FirstCol() - 1, imagen.LastCol() + 1);

    // Aplicar la máscara de convolución
    C_Image resultado(original.FirstRow() , original.LastRow(), original.FirstCol(), original.LastCol());
    for (int i = imagen.FirstRow() + 1; i <= imagen.LastRow() - 1; i++) {
        for (int j = imagen.FirstCol() + 1; j <= imagen.LastCol() - 1; j++) {
            int suma = 0;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    suma += imagen(i + x, j + y) * mascara(1 + x, 1 + y);
                }
            }
            if (suma > 255) suma = 255;
            if (suma < 0) suma = 0;
            resultado(i, j) = (unsigned char)suma;
        }
    }

    // Asignar el resultado a la imagen correspondiente
    if (eje.compare("x") == 0) {
        imgSobelX = resultado;
        cout << "Aplicada mascara en eje x\n";
    }
    if (eje.compare("y") == 0) {
        imgSobelY = resultado;
        cout << "Aplicada mascara en eje y\n";
    }
}



void magnitudGradientes() {
    // Calcular la magnitud del gradiente
    for (int i = imgSobelX.FirstRow(); i <= imgSobelX.LastRow(); i++) {
        for (int j = imgSobelX.FirstCol(); j <= imgSobelX.LastCol(); j++) {
            int gradienteX = imgSobelX(i, j);
            int gradienteY = imgSobelY(i, j);
            int magnitud = sqrt(gradienteX * gradienteX + gradienteY * gradienteY);
            imgFinal(i, j) = (unsigned char)std::min(magnitud, 255);
        }
    }
}

void umbralizar(int umbral) {

    for (int i = imgFinal.FirstRow(); i <= imgFinal.LastRow(); i++) {
        for (int j = imgFinal.FirstCol(); j <= imgFinal.LastCol(); j++) {
            if (imgFinal(i, j) > umbral) {
                imgFinal(i, j) = 0; // Invierte los colores aquí
            }
            else {
                imgFinal(i, j) = 255;
            }
        }
    }
}

void suavizadoGaussiano(C_Image imagen, double sigma) {
    int radio = ceil(2.5 * sigma);
    int tamano = 2 * radio + 1;
    C_Matrix filtroGaussiano(0, tamano - 1, 0, tamano -1, 0);
    double suma = 0.0;

    for (int i = 0; i < tamano; ++i) {
        for (int j = 0; j < tamano; ++j) {
            double exponente = -((i - radio) * (i - radio) + (j - radio) * (j - radio)) / (2 * sigma * sigma);
            filtroGaussiano(i, j) = exp(exponente) / (2 * M_PI * sigma * sigma);
            suma += filtroGaussiano(i, j);
        }
    }

    for (int i = 0; i < tamano; ++i) {
        for (int j = 0; j < tamano; ++j) {
            filtroGaussiano(i, j) /= suma;
        }
    }

    convolucion(imagen, filtroGaussiano, "suavizado");
}

void invertirColores() {
    for (int i = imagen.FirstRow() + 1; i <= imagen.LastRow() - 1; i++) {
        for (int j = imagen.FirstCol() + 1; j <= imagen.LastCol() - 1; j++) {
            int sum = 255 - imgFinal(i, j);
                if (sum < 0) sum = sum *  -1;
                imgFinal(i, j) = sum;
        }
    }
}

