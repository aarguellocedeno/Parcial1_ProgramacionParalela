#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class ImageProcessor {
private:
    string format;
    int width, height, maxval;

    int** pgm;    // Para PGM
    int*** ppm;   // Para PPM

public:
    ImageProcessor() : pgm(nullptr), ppm(nullptr) {}

    void load(const string& filename) {
        ifstream in(filename);

        in >> format >> width >> height >> maxval;

        if (format == "P2") {
            // se usa new pq uno nunca sabe cual seria el tamaño maximo y si se maneja estatico, esta la posibilidad de que los datos no se guarden correctamente
            pgm = new int*[height];
            for (int i = 0; i < height; i++) {
                pgm[i] = new int[width];
                for (int j = 0; j < width; j++) {
                    in >> pgm[i][j];
                }
            }
        }
        else{
            ppm = new int**[height];
            for (int i = 0; i < height; i++) {
                ppm[i] = new int*[width];
                for (int j = 0; j < width; j++) {
                    ppm[i][j] = new int[3];   //basicamente es como el color osea si es R G B
                    for (int c = 0; c < 3; c++) {
                        in >> ppm[i][j][c];
                    }
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

    void blur() {
        if (format == "P2") {
            int** result = new int*[height];
            for (int i = 0; i < height; i++)
                result[i] = new int[width];

            for (int i = 3; i < height - 3; i++) {
                for (int j = 3; j < width - 3; j++) {
                    int suma = 0;
                    for (int a = -3; a <= 3; a++) {
                        for (int b = -3; b <= 3; b++) {
                            suma += pgm[i+a][j+b];
                        }
                    }
                    result[i][j] = suma / 49;
                }
            }

            for (int i = 3; i < height - 3; i++) {
                for (int j = 3; j < width - 3; j++) {
                    pgm[i][j] = result[i][j];
                }
            }

        } else { 
            int*** result = new int**[height];
            for (int i = 0; i < height; i++) {
                result[i] = new int*[width];
                for (int j = 0; j < width; j++) {
                    result[i][j] = new int[3];
                }
            }

            for (int i = 3; i < height - 3; i++) {
                for (int j = 3; j < width - 3; j++) {
                    int sumaR = 0, sumaG = 0, sumaB = 0;
                    for (int a = -3; a <= 3; a++) {
                        for (int b = -3; b <= 3; b++) {
                            sumaR += ppm[i+a][j+b][0];
                            sumaG += ppm[i+a][j+b][1];
                            sumaB += ppm[i+a][j+b][2];
                        }
                    }
                    result[i][j][0] = sumaR / 49;
                    result[i][j][1] = sumaG / 49;
                    result[i][j][2] = sumaB / 49;
                }
            }

            for (int i = 3; i < height - 3; i++) {
                for (int j = 3; j < width - 3; j++) {
                    for (int c = 0; c < 3; c++) {
                        ppm[i][j][c] = result[i][j][c];
                    }
                }
            }
        }
    }


    void laplace() {
        int kernel[3][3] = {
            { 0, -1,  0},
            {-1,  4, -1},
            { 0, -1,  0}
        };

        if (format == "P2") {
            int** result = new int*[height];
            for (int i = 0; i < height; i++)
                result[i] = new int[width];

            for (int i = 1; i < height - 1; i++) {
                for (int j = 1; j < width - 1; j++) {
                    int temp = 0;
                    for (int a = -1; a <= 1; a++) {
                        for (int b = -1; b <= 1; b++) {
                            temp += pgm[i+a][j+b] * kernel[a+1][b+1];
                        }
                    }
                    if (temp < 0) temp = 0;
                    if (temp > 255) temp = 255;
                    result[i][j] = temp;
                }
            }

            // despues de calcular todo, lo volvemos a colocar en la matriz original
            for (int i = 1; i < height - 1; i++){
                for (int j = 1; j < width - 1; j++){
                     pgm[i][j] = result[i][j];
                }
            }
 
        }
        else {
            int*** result = new int**[height];
            for (int i = 0; i < height; i++) {
                result[i] = new int*[width];
                for (int j = 0; j < width; j++) {
                    result[i][j] = new int[3];
                }
            }
            for (int i = 1; i < height - 1; i++) {
                for (int j = 1; j < width - 1; j++) {
                    int temp[3] = {0, 0, 0};
                    for (int a = -1; a <= 1; a++) {
                        for (int b = -1; b <= 1; b++) {
                            temp[0] += ppm[i+a][j+b][0] * kernel[a+1][b+1];
                            temp[1] += ppm[i+a][j+b][1] * kernel[a+1][b+1];
                            temp[2] += ppm[i+a][j+b][2] * kernel[a+1][b+1];
                        }
                    }
                    for (int c = 0; c < 3; c++) {
                        if (temp[c] < 0) temp[c] = 0;
                        if (temp[c] > 255) temp[c] = 255;
                        result[i][j][c] = temp[c];
                    }
                }
            }
            for (int i = 1; i < height - 1; i++){
                for (int j = 1; j < width - 1; j++){
                    for (int c = 0; c < 3; c++){
                        ppm[i][j][c] = result[i][j][c];
                    }
                }

            }
                
                        

        }
    }


    void realce() {
        float alpha = 1.0;
        int kernel[3][3] = {
            { 0, -1,  0},
            {-1,  5, -1}, 
            { 0, -1,  0}
        };

        if (format == "P2") { 
            int** result = new int*[height];
            for (int i = 0; i < height; i++)
                result[i] = new int[width];

            for (int i = 1; i < height-1; i++) {
                for (int j = 1; j < width-1; j++) {
                    int temp = 0;
                    for (int a = -1; a <= 1; a++) {
                        for (int b = -1; b <= 1; b++) {
                            temp += pgm[i+a][j+b] * kernel[a+1][b+1];
                        }
                    }

                    int val = (int)(alpha * temp);
                    if (val < 0) val = 0;
                    if (val > 255) val = 255;
                    result[i][j] = val;
                }
            }

            // ahora si se pasa a la original
            for (int i = 1; i < height-1; i++){
                for (int j = 1; j < width-1; j++){
                    pgm[i][j] = result[i][j];
                }
            }

        } else { 
            int*** result = new int**[height];
            for (int i = 0; i < height; i++) {
                result[i] = new int*[width];
                for (int j = 0; j < width; j++) {
                    result[i][j] = new int[3];
                }
            }

            for (int i = 1; i < height-1; i++) {
                for (int j = 1; j < width-1; j++) {
                    int temp[3] = {0,0,0};
                    for (int a = -1; a <= 1; a++) {
                        for (int b = -1; b <= 1; b++) {
                            for (int c = 0; c < 3; c++) {
                                temp[c] += ppm[i+a][j+b][c] * kernel[a+1][b+1];
                            }
                        }
                    }

                    for (int c = 0; c < 3; c++) {
                        int val = (int)(alpha * temp[c]);
                        if (val < 0) val = 0;
                        if (val > 255) val = 255;
                        result[i][j][c] = val;
                    }
                }
            }

            for (int i = 1; i < height-1; i++){
                for (int j = 1; j < width-1; j++){
                    for (int c = 0; c < 3; c++){
                        ppm[i][j][c] = result[i][j][c];
                    }
                }
            }
            }
        }


};



int main(int argc, char* argv[]) {

    string filtro;
    filtro = argv[4];
    ImageProcessor img;
    img.load(argv[1]);

    if (filtro == "blur"){
        img.blur();
    }else if(filtro == "laplace"){
        img.laplace();
    }else{
        img.realce();
    }
    
    img.save(argv[2]);

    return 0;
}
