using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cmath>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

struct Pieza{
    int indice;
    int ancho;
    int alto;
    int posX;
    int posY;
    int cantRot;
};


struct Solucion{
    vector<Pieza> piezas;
    vector<int> orden;
    vector<int> alturaColumnas;
    int H_sol;
};

int n,W; //cant piezas, Ancho tira
const int semEle = 20031202;
mt19937 rng(semEle); //semilla
long timestamp = time(0);

//Lee el archivo y guarda su información para ocuparla en las iteraciones
vector<Pieza> leerInstancia(string nombreArchivo){
    
    string path = "./2sp/" + nombreArchivo + ".txt";
    ifstream archivo(path);

    if(!archivo){
        cerr<<"Error al abrir el archivo de la instancia"<<endl;
        exit(1);
    } 
    
    archivo>>n;
    archivo>>W;
    vector<Pieza> piezas(n);

    for(int i = 0; i < n; i++){
        archivo >> piezas[i].indice >> piezas[i].ancho >> piezas[i].alto;
        piezas[i].posX = -1;
        piezas[i].posY = -1;
        piezas[i].cantRot = 0;
    } 

    archivo.close();
    return piezas;
}

//Muestra los valores de los parametros y altura de la solución inicial antes de comenzar con las iteraciones.
void infoInicial(int h, float temp, float alpha, int maxIter, bool debug){
    cout<<"La altura de la solución inicial es: "<<h<<endl;
    cout<<"T° inicial | alpha | maximas iteraciones | debug"<<endl;
    cout<<temp<<" | "<<alpha<<" | "<<maxIter<<" | "<<debug<<endl;
}

//Genera una solución inicial aleatoria: orden de inserción + copia de las piezas originales.
void crearSolInicial(Solucion &sol, vector<Pieza> &piezas){
    vector<int> orden(n);
    
    for(int i = 0; i < n; i++){
        orden[i] = i;
    }

    shuffle(orden.begin(), orden.end(), rng);

    sol.orden = orden;
    sol.piezas = piezas;
}

//Muestra los valores de las piezas al finalizar las iteraciones. Solo si el debug es true.
void infoFinalPiezas(Solucion &sol){
    for(int i = 0; i < n; i++){
        //indice pieza coordenada X,coordenada Y cantidad de rotaciones
        cout<<sol.piezas[i].indice<<" "<<sol.piezas[i].posX<<","<<sol.piezas[i].posY<<" "<<sol.piezas[i].cantRot<<endl;
    }
}

//Muestra el orden de inserción de laz piezas en la solución. Solo si el debug es true.
void infoOrden(Solucion &sol){
    for(int i = 0; i < n; i++){
        //id_pieza
        cout<<sol.orden[i] + 1<<" ";
    }
    cout<<endl;
}

//Llama a las diferentes funciones para mostrar la información final de la mejor solucion. Solo si el debug es true.
void infoCompleta(Solucion &sol){
    cout<<"Altura conseguida:"<<sol.H_sol<<endl;
    cout<<"Orden inserción: ";
    infoOrden(sol);
    cout<<"Coordendas piezas:"<<endl<<"id_pieza coordX,coordY cantRot"<<endl;
    infoFinalPiezas(sol);
}

/*Función que calcula la calidad (altura) de la solución entregada por parametro.
*Devuelve la solución con los valores nuevos de H_sol, alturaColumnas. Cada pieza
* se inserta tratando de quedar lo más abajo posible (first-fit). Tambien actualiza
* las posiciones de cada pieza.
*/
void evaluar(Solucion &sol){

    vector<int> alturaColumnasAyuda(W,0); //Representa el skyline: altura ocupada en cada columna.
    int H_ayuda = 0; //Altura total de la solución (se buscará minimizar).

    for(int j = 0; j < n; j++){
        int anchoPieza = sol.piezas[sol.orden[j]].ancho;
        int altoPieza = sol.piezas[sol.orden[j]].alto;
        int limite = W - anchoPieza; //Para que no se salga del ancho de la franja.
        int x_best = -1;
        int y_best = 100000;
            
        //Se prueban TODAS las posiciones posibles en X.
        for(int x = 0; x <= limite; x++){
            int y_pos = 0;

            //Se busca la altura máxima en las columnas que ocuparía la pieza.
            for(int k = x; k < x + anchoPieza; k++){ 
                y_pos = max(y_pos, alturaColumnasAyuda[k]);
            }

            //Se elige la posición que deje la pieza más abajo (lower skyline).
            if(y_pos < y_best){
                y_best = y_pos;
                x_best = x;
            }
        }

        //Se actualiza el skyline en las columnas afectadas.
        for(int x2 = x_best; x2 < x_best + anchoPieza; x2++){
            alturaColumnasAyuda[x2] = y_best + altoPieza;
            H_ayuda = max(H_ayuda, y_best + altoPieza);
        }

        sol.piezas[sol.orden[j]].posX = x_best; //Coordenada en eje X.
        sol.piezas[sol.orden[j]].posY = y_best; //Coordenada en eje Y.
    }

    sol.alturaColumnas = alturaColumnasAyuda;
    sol.H_sol = H_ayuda;
}

//Decide por probabilidad que moviento se hace para generar la solución candidata.
void hacerMovimiento(Solucion &sol, bool debug){

    uniform_int_distribution<int> distMov(0, 1); //swap = 0, rotación = 1
    uniform_int_distribution<int> distInd(0, n-1); //todos los indices de las piezas

    int tipo = distMov(rng);

    if(tipo == 0){
        
        int i = distInd(rng);
        int j = distInd(rng);
        while(i == j){//si intenta swapear de orden de inserción a la misma pieza, continua buscando dos piezas diferentes.
            j = distInd(rng);
        }

        swap(sol.orden[i], sol.orden[j]);
        if(debug)cout<<"Se hizo un swap entre "<<sol.orden[j]<<" y "<<sol.orden[i]<<endl;
    }else{

        int i = distInd(rng);

        while(sol.piezas[sol.orden[i]].alto > W){//Si al rotar la pieza se sale del ancho de la franja, busca una nueva que si pueda ser rotada.
            i = distInd(rng);
        }

        
        if(debug)cout<<"Se va a hacer una rotación en la pieza "<<sol.orden[i] + 1<<endl;
        if(debug)cout<<"Ancho antes de la rot:"<< sol.piezas[sol.orden[i]].ancho<<endl;
        swap(sol.piezas[sol.orden[i]].ancho, sol.piezas[sol.orden[i]].alto);
        sol.piezas[sol.orden[i]].cantRot += 1;
        if(debug)cout<<"Ancho después de la rot:"<< sol.piezas[sol.orden[i]].ancho<<endl;
    }
}

//Calcula el área no utilizada por las piezas dentro de la franja.
int calcularArea(Solucion &sol){

    int areaBloques = 0;

    for(int i = 0; i < n; i++){
        areaBloques += sol.piezas[i].alto * sol.piezas[i].ancho;//calcula la suma de las áreas de cada pieza.
    }

    int areaTotal = W * sol.H_sol;//calcula el área de la franja.

    int area = areaTotal - areaBloques;//ve la diferencia entre el total del área de la franja con el que ocupan las piezas.
    return area;
}

string floatToStr(float valor){
    stringstream ss;
    ss << fixed << setprecision(2) << valor;
    return ss.str();
}

//Escribe en un TXT la información final sobre la mejor solución obtenida dentro de las iteraciones.
void escribirTxtFinal(string nombreArchivo, Solucion &sol, float temp, float alpha, int maxIter, int area){
    string path = "./salidas_pauta/resultado_" + nombreArchivo +
                    "_" + floatToStr(temp) +
                    "_" + floatToStr(alpha) +
                    "_" + to_string(maxIter) +
                    "_" + to_string(semEle) +
                    "_" + to_string(timestamp) + ".txt";
    ofstream salida(path);

    if(!salida){
        cout<<"Error al crear el archivo"<<endl;
    }

    
    salida<<sol.H_sol<<endl;
    salida<<area<<endl;

    for(int i = 0; i < n; i++){
        salida<<sol.piezas[i].posX;
        salida<<" ";
        salida<<sol.piezas[i].posY;
        salida<<" ";
        salida<<sol.piezas[i].cantRot;
        salida<<endl;
    }
}

bool archivoExiste(const string &nombreArchivo){
    struct stat buffer;
    return (stat(nombreArchivo.c_str(), &buffer) == 0);
}

void escribirCSV(const string &nombreCSV,const string &nombreInstancia, float temp, float alpha, int maxIter, int H, int area, double tiempo){

    bool existe = archivoExiste(nombreCSV);

    ofstream archivo(nombreCSV, ios::app);
    if(!archivo.is_open()){
        cout<<"Error abriendo CSV"<<endl;
        return;
    }

    if(!existe){
        archivo<<"nombreInstancia,semilla,T°Inicial,alpha,maxIteraciones,anchoFranja,alturaFranja,areaNoUtilizada,tiempoEjecucion\n";
    }

    archivo << nombreInstancia <<","
            << semEle <<","
            << temp <<","
            << alpha <<","
            << maxIter <<","
            << W <<","
            << H <<","
            << area <<","
            << tiempo <<"\n";
    
    archivo.close();
}

int main(int argc, char* argv[]){

    if(argc < 2){
        cout<<"Se usa: ./2dspp nombreArchivo"<<endl;
        return 1;
    }

    string nombreArchivo = argv[1];
    vector<Pieza> piezas = leerInstancia(nombreArchivo);

    Solucion sol_actual;
    crearSolInicial(sol_actual, piezas);
    evaluar(sol_actual);
    int H = sol_actual.H_sol;
    
    Solucion mejorSol = sol_actual;
    Solucion sol_vecino;
    float temp = 100.;
    float alpha = 0.9;
    float probabilidad;
    uniform_real_distribution<double> dist01(0.0, 1.0);

    int contadorRecalentar = 0;
    int maxIter = 1000;
    bool debug = 0;

    if(argc >= 6){//si los parametros son pasados por cmd.
        temp = atof(argv[2]);
        alpha = atof(argv[3]);
        maxIter = atoi(argv[4]);
        debug = atoi(argv[5]);
    }

    if(debug)infoInicial(H, temp, alpha, maxIter, debug);
    float tempIni = temp;
    auto start = chrono::high_resolution_clock::now();
    for(int iteraciones = 0; iteraciones < maxIter && temp > 0.001; iteraciones++){
        for(int i = 0; i < 2; i++){

            sol_vecino = sol_actual;
            hacerMovimiento(sol_vecino, debug);
            evaluar(sol_vecino);

            if(debug){
                cout<<"Orden solución candidata:";
                infoOrden(sol_vecino);
                cout<<"Este es el H de la solución actual en la iter "<<iteraciones+1<<": "<<sol_actual.H_sol<<endl;
                cout<<"Este es el H de la solución candidata en la iter "<<iteraciones+1<<": "<<sol_vecino.H_sol<<endl;
            }
            
            if(sol_vecino.H_sol < sol_actual.H_sol){
                sol_actual = sol_vecino;
                contadorRecalentar = 0;

                if(debug)cout<<"Se acepto la solución candidata"<<endl;
                if(debug)cout<<"El nuevo valor de la altura de la solución actual: "<<sol_actual.H_sol<<endl;
            }else{
                probabilidad = exp(((sol_actual.H_sol - sol_vecino.H_sol)/temp));
                
                if(debug)cout<<"Se esta viendo si se acepta"<<endl;
                if(debug)cout<<"Probabilidad: "<<probabilidad<<endl;
                //si el numero al azar entre 0. y 1.0 es menor a probabilidad, se acepta    
                if( dist01(rng) < probabilidad){
                    sol_actual = sol_vecino;
                    contadorRecalentar = 0;

                    if(debug)cout<<"Se acepto la solución candidata en la 2da oportunidad"<<endl;
                    if(debug)cout<<"El nuevo valor de la altura de la solución actual: "<<sol_actual.H_sol<<endl;
                }else{
                    contadorRecalentar += 1;
                }
            }

            if(sol_vecino.H_sol < mejorSol.H_sol){
                mejorSol = sol_vecino;
            }
            if(debug)cout<<"--------------------------------------------------------------------------------------------------"<<endl;
        }
        
        if(contadorRecalentar >= 5){
            temp *= 1.2;
            contadorRecalentar = 0;
        }else{
            temp = alpha * temp;
        }
    }
    auto end = chrono::high_resolution_clock::now();
    double tiempo = chrono::duration<double>(end - start).count();

    int area = calcularArea(mejorSol);
    escribirTxtFinal(nombreArchivo, mejorSol, tempIni, alpha, maxIter, area);
    escribirCSV("resultados_experimentos.csv", nombreArchivo, tempIni, alpha, maxIter, mejorSol.H_sol, area, tiempo);
    if(debug) infoCompleta(mejorSol);
    if(debug) cout<<"Tiempo:"<<tiempo<<endl;
}