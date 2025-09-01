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


};


int main(int argc, char* argv[]) {
    ImageProcessor img;
    img.load(argv[1]);

    img.save(argv[2]);

    return 0;
}
