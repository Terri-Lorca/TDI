

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
C_Image imagen, imgSobelX, imgSobelY, imgFinal;
int umbral;

void Sobelx();
void Sobely();
void convolucion(C_Image imagen, C_Matrix mascara, string eje);
void SumaEjes();
void invertirColores();
void umbralizar(int umbral);
void suavizadoGaussiano(C_Image imagen, double sigma);
void magnitudYDireccionGradiente();


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
        suavizadoGaussiano(imagen, 1.0);
        Sobelx();
        Sobely();
        cout << "Introduce el umbral\n";
        cin >> umbral;
        umbralizar(umbral);
        imgSobelX.WriteBMP("Prueba1.bmp");
        imgSobelY.WriteBMP("Prueba2.bmp");
        imgFinal = imagen;
        magnitudYDireccionGradiente();
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

    // Aplicar la máscara de convolución
    C_Image resultado(imagen.FirstRow(), imagen.LastRow(), imagen.FirstCol(), imagen.LastCol());
    for (int i = resultado.FirstRow(); i <= resultado.LastRow(); i++) {
        for (int j = resultado.FirstCol(); j <= resultado.LastCol(); j++) {
            int suma = 0;
            for (int y = i - 1; y <= i + 1; y++) {
                for (int x = j - 1; x <= j + 1; x++) {
                    suma += mascara(y - i + 1, x - j + 1) * ampliada(y, x);
                }
            }
            resultado(i, j) = (unsigned char) suma;
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

void magnitudYDireccionGradiente() {
    C_Image imgMagnitud(imagen.FirstRow(), imagen.LastRow(), imagen.FirstCol(), imagen.LastCol());
    C_Matrix angulos(imagen.FirstRow(), imagen.LastRow(), imagen.FirstCol(), imagen.LastCol());

    // Calcular la magnitud y la dirección del gradiente
    for (int i = imgSobelX.FirstRow(); i <= imgSobelX.LastRow(); i++) {
        for (int j = imgSobelX.FirstCol(); j <= imgSobelX.LastCol(); j++) {
            int gradienteX = imgSobelX(i, j);
            int gradienteY = imgSobelY(i, j);
            int magnitud = sqrt(gradienteX * gradienteX + gradienteY * gradienteY);
            float angulo = atan2(gradienteY, gradienteX) * 180 / M_PI;
            imgMagnitud(i, j) = (unsigned char)magnitud;
            angulos(i, j) = angulo;
        }
    }

    // Supresión de no máximos
    for (int i = imgMagnitud.FirstRow() + 1; i <= imgMagnitud.LastRow() - 1; i++) {
        for (int j = imgMagnitud.FirstCol() + 1; j <= imgMagnitud.LastCol() - 1; j++) {
            float angulo = angulos(i, j);
            int magnitud = imgMagnitud(i, j);

            // Comparar con los píxeles vecinos en la dirección del gradiente
            if (((angulo > -22.5 && angulo <= 22.5) || (angulo <= -157.5 || angulo > 157.5)) &&
                (magnitud > imgMagnitud(i, j + 1) && magnitud >= imgMagnitud(i, j - 1))) {
                imgFinal(i, j) = (unsigned char)magnitud;
            }
            else if (((angulo > 22.5 && angulo <= 67.5) || (angulo <= -112.5 && angulo > -157.5)) &&
                (magnitud > imgMagnitud(i - 1, j + 1) && magnitud >= imgMagnitud(i + 1, j - 1))) {
                imgFinal(i, j) = (unsigned char)magnitud;
            }
            else if (((angulo > 67.5 && angulo <= 112.5) || (angulo <= -67.5 && angulo > -112.5)) &&
                (magnitud > imgMagnitud(i - 1, j) && magnitud >= imgMagnitud(i + 1, j))) {
                imgFinal(i, j) = (unsigned char)magnitud;
            }
            else if (((angulo > 112.5 && angulo <= 157.5) || (angulo <= -22.5 && angulo > -67.5)) &&
                (magnitud > imgMagnitud(i - 1, j - 1) && magnitud >= imgMagnitud(i + 1, j + 1))) {
                imgFinal(i, j) = (unsigned char)magnitud;
            }
            else {
                imgFinal(i, j) = 0;
            }
        }
    }

    // Copiar los bordes de la imagen original
    for (int i = imgMagnitud.FirstRow(); i <= imgMagnitud.LastRow(); i++) {
        imgFinal(i, imgMagnitud.FirstCol()) = imgMagnitud(i, imgMagnitud.FirstCol());
        imgFinal(i, imgMagnitud.LastCol()) = imgMagnitud(i, imgMagnitud.LastCol());
    }
    for (int j = imgMagnitud.FirstCol(); j <= imgMagnitud.LastCol(); j++) {
        imgFinal(imgMagnitud.FirstRow(), j) = imgMagnitud(imgMagnitud.FirstRow(), j);
        imgFinal(imgMagnitud.LastRow(), j) = imgMagnitud(imgMagnitud.LastRow(), j);
    }
}




void umbralizar(int umbral) {

    for (int i = imagen.FirstRow(); i <= imagen.LastRow(); i++) {
        for (int j = imagen.FirstCol(); j <= imagen.LastCol(); j++) {
            if (imgSobelX(i, j) > umbral) {
                imgSobelX(i, j) = 255; // Invierte los colores aquí
            }
            else {
                imgSobelX(i, j) = 0;
            }
            if (imgSobelY(i, j) > umbral) {
                imgSobelY(i, j) = 255; // Invierte los colores aquí
            }
            else {
                imgSobelY(i, j) = 0;
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

