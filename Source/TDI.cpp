

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
C_Image imagen;


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
    }
    else {
        cout << "Se ha cargado la imagen correctamente";
        

    }
}

void Sobelx(C_Image imgen) {
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
}

void Sobely(C_Image imgen) {
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
}