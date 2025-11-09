using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

struct Pieza{
    int indice;
    int alto;
    int ancho;
    int posX;
    int posY;
    bool rot;
    int cantRot;
};


struct Solucion{
    vector<Pieza> piezas;
    vector<int> orden;
    vector<int> alturaColumnas;
    int H_sol;
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


int evaluar(const vector<int> &orden, const vector<Pieza> &piezas){

    vector<int> alturaColumnas(W,0);
    int H_sol = 0;
    for(int j = 0; j < n; j++){
        int anchoPieza = piezas[orden[j]].ancho;
        int altoPieza = piezas[orden[j]].alto;
        int limite = W - anchoPieza;
        int x_best = -1;
        int y_best = 100000;
            
        for(int x = 0; x <= limite; x++){
            int y_pos = 0;
            for(int k = x; k < x + anchoPieza; k++){
                y_pos = max(y_pos, alturaColumnas[k]);
            }

            if(y_pos < y_best){
                y_best = y_pos;
                x_best = x;
            }
        }

        for(int x2 = x_best; x2 < x_best + anchoPieza; x2++){
            alturaColumnas[x2] = y_best + altoPieza;
            H_sol = max(H_sol, y_best + altoPieza);
        }
    }

    return H_sol;
}

void evaluar(Solucion &sol){

    vector<int> alturaColumnasAyuda(W,0);
    int H_ayuda = 0;

    for(int j = 0; j < n; j++){
        int anchoPieza = sol.piezas[sol.orden[j]].ancho;
        int altoPieza = sol.piezas[sol.orden[j]].alto;
        int limite = W - anchoPieza;
        int x_best = -1;
        int y_best = 100000;
            
        for(int x = 0; x <= limite; x++){
            int y_pos = 0;
            for(int k = x; k < x + anchoPieza; k++){
                y_pos = max(y_pos, alturaColumnasAyuda[k]);
            }

            if(y_pos < y_best){
                y_best = y_pos;
                x_best = x;
            }
        }

        for(int x2 = x_best; x2 < x_best + anchoPieza; x2++){
            alturaColumnasAyuda[x2] = y_best + altoPieza;
            H_ayuda = max(H_ayuda, y_best + altoPieza);
        }

        sol.piezas[sol.orden[j]].posX = x_best;
        sol.piezas[sol.orden[j]].posY = y_best;
    }

    sol.alturaColumnas = alturaColumnasAyuda;
    sol.H_sol = H_ayuda;
    
}

int main(){

    string nombreArchivo;
    /*cout<<"Ingresa el nombre de archivo de la instancia a resolver:"<<endl;
    cin>>nombreArchivo;*/

    nombreArchivo = "BENG01.TXT";
    vector<Pieza> piezas = leerInstancia(nombreArchivo);
    infoInicialInstancia(piezas);

    int H_max = calcularAlturaMax(piezas);
    cout<<"Altura maxima es: "<<H_max<<endl;

    H = 0;

    vector<int> orden(n);
    
    
    for(int i = 0; i < n; i++){
        orden[i] = i;
    }

    Solucion sol_inicial;
    sol_inicial.orden = orden;
    sol_inicial.piezas = piezas;

    int H1 = evaluar(orden, piezas);
    evaluar(sol_inicial);
    int H2 = sol_inicial.H_sol;
    /*for(int i = 0; i < W; i++){
        cout<<"Altura en la columna "<<i+1<<" es "<<alturaColumnas[i]<<endl;
    }*/

    cout<<"H1 es: "<<H1<<endl;
    cout<<"H2 es: "<<H2<<endl;

    for(int i = 0; i < n; i++){
        cout<<"Coordenadas pieza "<<i<<":"<<sol_inicial.piezas[i].posX<<","<<sol_inicial.piezas[i].posY<<endl;
    }

}