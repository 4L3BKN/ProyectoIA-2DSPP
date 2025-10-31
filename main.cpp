using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct Pieza{
    int indice;
    int alto;
    int ancho;
    int posX;
    int posY;
    bool rot;
    int cantRot;
};

int n,W; //cant piezas, Ancho tira
int H; //Altura a minimizar

vector<Pieza> leerInstancia(string nombreArchivo){
    
    string path = "./2sp/" + nombreArchivo;
    ifstream archivo(path);
    cout<<path<<endl;

    if(!archivo){
        cerr<<"Error al abrir el archivo"<<endl;
        exit(1);
    } 
    
    archivo>>n;
    archivo>>W;
    
    cout<<"Cantidad de piezas: "<<n<<endl;
    cout<<"Ancho tira: "<<W<<endl;


    vector<Pieza> piezas(n);

    for(int i = 0; i < n; i++){
        archivo >> piezas[i].indice >> piezas[i].ancho >> piezas[i].alto;
        piezas[i].posX = -1;
        piezas[i].posY = -1;
        piezas[i].rot = false;
        piezas[i].cantRot = 0;
    } 

    archivo.close();

    return piezas;
}

void infoInicialInstancia(vector<Pieza> &piezas){
    cout<<"Piezas #:AnchoxAlto Rot"<<endl;

    for(int i = 0; i < n; i++){
        cout<<"Pieza "<<piezas[i].indice<<":";
        cout<<piezas[i].ancho<<"x";
        cout<<piezas[i].alto<<" ";
        cout<<piezas[i].rot;
        cout<<endl;
    }
}

bool dentroTira(const Pieza &p){
    return (p.posX >= 0 && p.posY >= 0 && p.posX + p.ancho <= W && p.posY + p.alto <= H);
}

int calcularAlturaMax(const vector<Pieza> &piezas){
    int alt = 0;

    for(auto& p : piezas){
        if(p.alto >= p.ancho){
            alt += p.alto;
        }else{
            alt += p.ancho;
        }
    }
    return alt;
}

int main(){

    string nombreArchivo;
    cout<<"Ingresa el nombre de archivo de la instancia a resolver:"<<endl;
    cin>>nombreArchivo;

    vector<Pieza> piezas = leerInstancia(nombreArchivo);
    infoInicialInstancia(piezas);

    H = calcularAlturaMax(piezas);
    cout<<"Altura maxima es: "<<H<<endl;
}