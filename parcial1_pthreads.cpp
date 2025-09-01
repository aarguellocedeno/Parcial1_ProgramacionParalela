#include <iostream>
#include <pthread.h>
#include <fstream>
using namespace std;

class ImageProcessor {
private:
    string format;
    int width, height, maxval;
    int** pgm;
    int*** ppm;

public:
    ImageProcessor() : pgm(nullptr), ppm(nullptr) {}

    void load(const string& filename) {
        ifstream in(filename);
        in >> format >> width >> height >> maxval;

        if (format == "P2") {
            pgm = new int*[height];
            for(int i=0;i<height;i++){
                pgm[i] = new int[width];
                for(int j=0;j<width;j++) in>>pgm[i][j];
            }
        } else {
            ppm = new int**[height];
            for(int i=0;i<height;i++){
                ppm[i] = new int*[width];
                for(int j=0;j<width;j++){
                    ppm[i][j] = new int[3];
                    for(int c=0;c<3;c++) in>>ppm[i][j][c];
                }
            }
        }
    }

    void save(const string& filename) {
        ofstream out(filename);

        out << format << "\n";
        out << width << " " << height << "\n";
        out << maxval << "\n";

        if (format == "P2") {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    out << pgm[i][j] << " ";
                }
                out << "\n";
            }
        }
        else {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    for (int c = 0; c < 3; c++) {
                        out << ppm[i][j][c] << " ";
                    }
                }
                out << "\n";
            }
        }
    }

    void blur(int ini, int fin, int ini2, int fin2) {
        ini = max(ini, 3);
        fin = min(fin, height - 3);
        ini2 = max(ini2, 3);
        fin2 = min(fin2, width - 3);

        if (format == "P2") {
            for (int i = ini; i < fin; i++) {
                for (int j = ini2; j < fin2; j++) {
                    int suma = 0;
                    for (int a = -3; a <= 3; a++) {
                        for (int b = -3; b <= 3; b++) {
                            suma += pgm[i+a][j+b];
                        }
                    }
                    pgm[i][j] = suma / 49;
                }
            }
        } else {
            for (int i = ini; i < fin; i++) {
                for (int j = ini2; j < fin2; j++) {
                    int sumaR = 0, sumaG = 0, sumaB = 0;
                    for (int a = -3; a <= 3; a++) {
                        for (int b = -3; b <= 3; b++) {
                            sumaR += ppm[i+a][j+b][0];
                            sumaG += ppm[i+a][j+b][1];
                            sumaB += ppm[i+a][j+b][2];
                        }
                    }
                    ppm[i][j][0] = sumaR / 49;
                    ppm[i][j][1] = sumaG / 49;
                    ppm[i][j][2] = sumaB / 49;
                }
            }
        }
    }

    void laplace(int ini, int fin, int ini2, int fin2) {
        int kernel[3][3] = {
            { 0, -1,  0},
            {-1,  4, -1},
            { 0, -1,  0}
        };
        ini = max(ini, 3);
        fin = min(fin, height - 3);
        ini2 = max(ini2, 3);
        fin2 = min(fin2, width - 3);

        if (format == "P2") { 
            int** result = new int*[height];
            for (int i = 0; i < height; i++) result[i] = new int[width];

            for (int i = ini+1; i < fin-1; i++) {
                for (int j = ini2+1; j < fin2-1; j++) {
                    int temp = 0;
                    for (int a = -1; a <= 1; a++) {
                        for (int b = -1; b <= 1; b++) {
                            temp += pgm[i+a][j+b] * kernel[a+1][b+1];
                        }
                    }
                    if (temp < 0) temp = 0;
                    if (temp > maxval) temp = maxval;
                    result[i][j] = temp;
                }
            }
            // despues de calcular todo, lo volvemos a colocar en la matriz original
            for (int i = ini+1; i < fin-1; i++) {
                for (int j = ini2+1; j < fin2-1; j++) {
                    pgm[i][j] = result[i][j];
                }
            }

        }
        else if (format == "P3") { 
            int*** result = new int**[height];
            for (int i = 0; i < height; i++) {
                result[i] = new int*[width];
                for (int j = 0; j < width; j++) {
                    result[i][j] = new int[3];
                }
            }

            for (int i = ini+1; i < fin-1; i++) {
                for (int j = ini2+1; j < fin2-1; j++) {
                    for (int c = 0; c < 3; c++) { // R, G, B
                        int temp = 0;
                        for (int a = -1; a <= 1; a++) {
                            for (int b = -1; b <= 1; b++) {
                                temp += ppm[i+a][j+b][c] * kernel[a+1][b+1];
                            }
                        }
                        if (temp < 0) temp = 0;
                        if (temp > maxval) temp = maxval;
                        result[i][j][c] = temp;
                    }
                }
            }

            for (int i = ini; i < fin; i++){
                for (int j = ini2; j < fin2; j++){
                    for (int c = 0; c < 3; c++){
                         ppm[i][j][c] = result[i][j][c];
                    }
                }
            }
                
                    
                       
        }
    }


    void realce(int ini, int fin, int ini2, int fin2) {
        int kernel[3][3] = {
            { 0, -1,  0},
            {-1,  5, -1},
            { 0, -1,  0}
        };
        ini = max(ini, 3);
        fin = min(fin, height - 3);
        ini2 = max(ini2, 3);
        fin2 = min(fin2, width - 3);
        if (format == "P2") { 
            int** result = new int*[height];
            for (int i = 0; i < height; i++) result[i] = new int[width];

            for (int i = ini+1; i < fin-1; i++) {
                for (int j = ini2+1; j < fin2-1; j++) {
                    int temp = 0;
                    for (int a = -1; a <= 1; a++) {
                        for (int b = -1; b <= 1; b++) {
                            temp += pgm[i+a][j+b] * kernel[a+1][b+1];
                        }
                    }
                    if (temp < 0) temp = 0;
                    if (temp > maxval) temp = maxval;
                    result[i][j] = temp;
                }
            }
                // ahora si se pasa a la original
            for (int i = ini+1; i < fin-1; i++) {
                for (int j = ini2+1; j < fin2-1; j++) {
                    pgm[i][j] = result[i][j];
                }
            }

        }
        else if (format == "P3") {  // Imagen en color
            int*** result = new int**[height];
            for (int i = 0; i < height; i++) {
                result[i] = new int*[width];
                for (int j = 0; j < width; j++) {
                    result[i][j] = new int[3];
                }
            }

            for (int i = ini+1; i < fin-1; i++) {
                for (int j = ini2+1; j < fin2-1; j++) {
                    for (int c = 0; c < 3; c++) {
                        int temp = 0;
                        for (int a = -1; a <= 1; a++) {
                            for (int b = -1; b <= 1; b++) {
                                temp += ppm[i+a][j+b][c] * kernel[a+1][b+1];
                            }
                        }
                        if (temp < 0) temp = 0;
                        if (temp > maxval) temp = maxval;
                        result[i][j][c] = temp;
                    }
                }
            }

            for (int i = ini; i < fin; i++){
                for (int j = ini2; j < fin2; j++){

                    for (int c = 0; c < 3; c++){
                        ppm[i][j][c] = result[i][j][c];
                    }
                        
                }

            }

                



        }
}


static void* todosHilos(void* arg){
    uintptr_t* data = (uintptr_t*)arg;
    ImageProcessor* obj = (ImageProcessor*) data[0];

    int ini  = (int)data[1]; // fila inicio
    int fin  = (int)data[2]; // fila fin
    int ini2 = (int)data[3]; // col inicio
    int fin2 = (int)data[4]; // col fin
    int filtro = (int)data[5];

    if(filtro == 0) {
        obj->blur(ini, fin, ini2, fin2);
    }
    else if(filtro == 1) {
        obj->laplace(ini, fin, ini2, fin2);
    }
    else if(filtro == 2) {
        obj->realce(ini, fin, ini2, fin2);
    }

    delete[] data;
    return NULL;
}

    void run(int filtro){  
        pthread_t threads[4];
        int midRow = height / 2;
        int midCol = width / 2;

            
        int raya = 3;

        int hilos[4][4] = {
            {0, midRow + raya, 0, midCol + raya},         // sup-izq
            {0, midRow + raya, midCol - raya, width},     // sup-der
            {midRow - raya, height, 0, midCol + raya},    // inf-izq
            {midRow - raya, height, midCol - raya, width} // inf-der
        };
        for(int i=0;i<4;i++){
            uintptr_t* data = new uintptr_t[6];
            data[0] = (uintptr_t)this;
            data[1] = hilos[i][0];  // ini fila
            data[2] = hilos[i][1];  // fin fila   
            data[3] = hilos[i][2];  // ini columna 
            data[4] = hilos[i][3];  // fin columna 
            data[5] = filtro;
            
            pthread_create(&threads[i], NULL, todosHilos, data);
        }

        for(int i=0;i<4;i++){
            pthread_join(threads[i], NULL);
        }
    }

};


int main(int argc, char* argv[]) {
    ImageProcessor img;
    string filtro;
    filtro= argv[4];
    img.load(argv[1]);

    if (filtro == "blur"){
        img.run(0);
    }else if(filtro == "laplace"){
        img.run(1);
    }else{
        img.run(2);
    }
    
    img.save(argv[2]);

    return 0;
}
