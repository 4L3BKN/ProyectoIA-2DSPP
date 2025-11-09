using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cmath>

struct Pieza{
    int indice;
    int ancho;
    int alto;
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
mt19937 rng(02122003);

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

void infoAlturasColumnas(Solucion &sol){
    for(int i = 0; i < W; i++){
        cout<<"Altura en la columna "<<i+1<<" es "<<sol.alturaColumnas[i]<<endl;
    }
}

void infoCoordPiezas(Solucion &sol){
    for(int i = 0; i < n; i++){
        cout<<"Coordenadas pieza "<<i<<":"<<sol.piezas[i].posX<<","<<sol.piezas[i].posY<<endl;
    }
}

void infoOrden(Solucion &sol){
    for(int i = 0; i < n; i++){
        cout<<sol.orden[i]<<endl;
    }
}

int calcularAlturaMax(vector<Pieza> &piezas){
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


void hacerMovimiento(Solucion &sol){

    uniform_int_distribution<int> distMov(0, 1); //swap = 0, rotación = 1
    uniform_int_distribution<int> distInd(0, n-1);

    int tipo = distMov(rng);

    if(tipo == 0){
        
        int i = distInd(rng);
        int j = distInd(rng);
        while(i == j){
            j = distInd(rng);
        }

        swap(sol.orden[i], sol.orden[j]);
        cout<<"Se hizo un swap entre "<<sol.orden[j]<<" y "<<sol.orden[i]<<endl;
    }else{

        int i = distInd(rng);
        sol.piezas[sol.orden[i]].rot = !sol.piezas[sol.orden[i]].rot;
        cout<<"Se va a hacer una rotación en la pieza "<<sol.orden[i] + 1<<endl;
        cout<<"Ancho antes de la rot:"<< sol.piezas[sol.orden[i]].ancho<<endl;
        swap(sol.piezas[sol.orden[i]].ancho, sol.piezas[sol.orden[i]].alto);
        sol.piezas[sol.orden[i]].cantRot += 1;
        cout<<"Ancho después de la rot:"<< sol.piezas[sol.orden[i]].ancho<<endl;
    }
}

int main(){

    string nombreArchivo;
    /*cout<<"Ingresa el nombre de archivo de la instancia a resolver:"<<endl;
    cin>>nombreArchivo;*/

    nombreArchivo = "BENG01.TXT";
    vector<Pieza> piezas = leerInstancia(nombreArchivo);
    //infoInicialInstancia(piezas);

    int H_max = calcularAlturaMax(piezas);
    cout<<"Altura maxima es: "<<H_max<<endl;

    
    vector<int> orden(n);
    
    
    for(int i = 0; i < n; i++){
        orden[i] = i;
    }

    shuffle(orden.begin(), orden.end(), rng);

    
    Solucion sol_actual;
    sol_actual.orden = orden;
    sol_actual.piezas = piezas;

    evaluar(sol_actual);
    int H = sol_actual.H_sol;
    
    cout<<"H es: "<<H<<endl;

    Solucion mejorSol = sol_actual;
    Solucion sol_vecino;
    float temp = 10.;
    float alpha = 0.9;
    float probabilidad;
    uniform_real_distribution<double> dist01(0.0, 1.0);

    for(int i = 0; i < 10; i++){
        sol_vecino = sol_actual;
        hacerMovimiento(sol_vecino);
        evaluar(sol_vecino);
        cout<<"Este es el H de la sol actual en la iter "<<i+1<<":"<<sol_actual.H_sol<<endl;
        cout<<"Este es el H de la sol vecina en la iter "<<i+1<<":"<<sol_vecino.H_sol<<endl;
        if(sol_vecino.H_sol < sol_actual.H_sol){
            sol_actual = sol_vecino;
            cout<<"Se acepto la sol vecina"<<endl;
            cout<<"El nuevo valor de H sol actual:"<<sol_actual.H_sol<<endl;
        }else{
            cout<<"Se esta viendo si se acepta"<<endl;

            probabilidad = exp(((sol_actual.H_sol - sol_vecino.H_sol)/temp));
            cout<<"Prob:"<<probabilidad<<endl;
            //si el numero al azar entre 0. y 1.0 es menor a probabilidad se acepta    
            if( dist01(rng) < probabilidad){
                sol_actual = sol_vecino;
                cout<<"Se acepto la sol vecina en la 2da oportunidad"<<endl;
                cout<<"El nuevo valor de H sol actual:"<<sol_actual.H_sol<<endl;
            }
        }

        if(sol_vecino.H_sol < mejorSol.H_sol){
            mejorSol = sol_vecino;
        }

        temp = alpha * temp;
    }

    cout<<"Este es el H minimo encontrado: "<<mejorSol.H_sol<<endl;
}