

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
C_Image imagen, imgSobelX, imgSobelY, imgFinal, original, imagenIvertida;
int umbral, imprimirXY, imgComb, boceto;

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
    cout << "(La imagen debe estar en la carpeta Run del proyecto)\n";
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
        cout << "Desea imprimir las imagenes correspodientes al los ejes X e Y ? ( Introduca 1 para si)\n";
        cin >> imprimirXY;
        if (imprimirXY == 1) {
            imgSobelX.WriteBMP("EjeX.bmp");
            imgSobelY.WriteBMP("EjeY.bmp");
        }
        imgFinal = imagen;
        magnitudGradientes();
        cout << "Desea imprimir la imagen correspodientes a la combinación de los ejes X e Y ? ( Introduca 1 para si)\n";
        cin >> imgComb;
        if (imgComb == 1) imgFinal.WriteBMP("Final.bmp");
        invertirColores(); cout << "Desea imprimir la imagen final con el fondo blanco y los bordes negros? ( Introduca 1 para si)\n";
        cin >> boceto;
        if (boceto == 1) imagenIvertida.WriteBMP("FinalInvertida.bmp");
    }
}

/*void Sobelx() {
    C_Matrix Msobel(0, 2, 0, 2, 0);
    Msobel(0, 0) = -1;Msobel(0, 1) = 0;Msobel(0, 2) = 1;
    Msobel(1, 0) = -2;Msobel(1, 1) = 0;Msobel(1, 2) = 2;
    Msobel(2, 0) = -1;Msobel(2, 1) = 0;Msobel(2, 2) = 1;
    convolucion(imagen, Msobel, "x");
}

void Sobely() {
    C_Matrix Msobel(0, 2, 0, 2, 0);
    Msobel(0, 0) = -1;Msobel(0, 1) = -2;Msobel(0, 2) = -1;
    Msobel(1, 0) = 0;Msobel(1, 1) = 0;Msobel(1, 2) = 0;
    Msobel(2, 0) = 1;Msobel(2, 1) = 2;Msobel(2, 2) = 1;
    convolucion(imagen, Msobel, "y");
}*/

void Sobelx() {
    C_Matrix Msobel(0, 4, 0, 4, 0);
    Msobel(0, 0) = -1; Msobel(1, 0) = -4; Msobel(2, 0) = -6; Msobel(3, 0) = -4; Msobel(4, 0) = -1;
    Msobel(0, 1) = -2; Msobel(1, 1) = -8; Msobel(2, 1) = -12; Msobel(3, 1) = -8; Msobel(4, 1) = -2;
    Msobel(0, 2) = 0; Msobel(1, 2) = 0; Msobel(2, 2) = 0; Msobel(3, 2) = 0; Msobel(4, 2) = 0;
    Msobel(0, 3) = 2; Msobel(1, 3) = 8; Msobel(2, 3) = 12; Msobel(3, 3) = 8; Msobel(4, 3) = 2;
    Msobel(0, 4) = 1; Msobel(1, 4) = 4; Msobel(2, 4) = 6; Msobel(3, 4) = 4; Msobel(4, 4) = 1;

    convolucion(imagen, Msobel, "x");
}

void Sobely() {
    C_Matrix Msobel(0, 4, 0, 4, 0);
    Msobel(0, 0) = -1; Msobel(1, 0) = -2; Msobel(2, 0) = 0; Msobel(3, 0) = 2; Msobel(4, 0) = 1;
    Msobel(0, 1) = -4; Msobel(1, 1) = -8; Msobel(2, 1) = 0; Msobel(3, 1) = 8; Msobel(4, 1) = 4;
    Msobel(0, 2) = -6; Msobel(1, 2) = -12; Msobel(2, 2) = 0; Msobel(3, 2) = 12; Msobel(4, 2) = 6;
    Msobel(0, 3) = -4; Msobel(1, 3) = -8; Msobel(2, 3) = 0; Msobel(3, 3) = 8; Msobel(4, 3) = 4;
    Msobel(0, 4) = -1; Msobel(1, 4) = -2; Msobel(2, 4) = 0; Msobel(3, 4) = 2; Msobel(4, 4) = 1;

    convolucion(imagen, Msobel, "y");
}


void convolucion(C_Image imagen, C_Matrix mascara, string eje) {
    C_Image ampliada(imagen.FirstRow() - 1, imagen.LastRow() + 1, imagen.FirstCol() - 1, imagen.LastCol() + 1);

    for (int i = ampliada.FirstRow(); i <= ampliada.LastRow(); i++) {
        for (int j = ampliada.FirstCol(); j <= ampliada.LastCol(); j++) {
            int row = i, col = j;
            if (i < imagen.FirstRow()) row = imagen.FirstRow();
            if (i > imagen.LastRow()) row = imagen.LastRow();
            if (j < imagen.FirstCol()) col = imagen.FirstCol();
            if (j > imagen.LastCol()) col = imagen.LastCol();
            ampliada(i, j) = imagen(row, col);
        }
    }

    C_Image resultado(imagen.FirstRow(), imagen.LastRow(), imagen.FirstCol(), imagen.LastCol());
    for (int i = imagen.FirstRow(); i <= imagen.LastRow(); i++) {
        for (int j = imagen.FirstCol(); j <= imagen.LastCol(); j++) {
            int suma = 0;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    int row = i + x, col = j + y;
                    suma += ampliada(row, col) * mascara(1 + x, 1 + y);
                }
            }
            if (suma > 255) suma = 255;
            if (suma < 0) suma = 0;
            resultado(i, j) = (unsigned char)suma;
        }
    }

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
                imgFinal(i, j) = 0; 
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
    imagenIvertida = imagen;
    for (int i = imagen.FirstRow() + 1; i <= imagen.LastRow() - 1; i++) {
        for (int j = imagen.FirstCol() + 1; j <= imagen.LastCol() - 1; j++) {
            int sum = 255 - imgFinal(i, j);
                if (sum < 0) sum = sum *  -1;
                imagenIvertida(i, j) = sum;
        }
    }
}

