#include <iostream>
#include <vector>

using namespace std;

struct tPolinomio{vector<double> grad; vector<double> coeficiente;};

bool es_num(char c);
bool ver_si_siguiente_char_en_str_es_signo(int i, string &polinomio_str);
bool es_int_un_num_str(string &str);
bool es_double_un_num_str(string &str);
bool error_forma_pol(string &polinomio_str, int &i, int &indicador_mul, int &indicador_signo, int &indicador_x, int &indicador_e, string &num_temp, string &gra_temp, bool &coeficiente_1op, bool &coeficiente_2op, bool &fin_signo);
void separador_grads_coefs(string &polinomio_str, tPolinomio &GC);
double str_a_num(string &str);
double control_enteros_positivos(string msg);
void suma_x_semejantes(tPolinomio &GC);
void intercambiar(double &a, double &b);
void orden_ascendente(tPolinomio &GC);
void print_suma_polinomios(tPolinomio &GC);

int main(){
    string polinomio_str, msg{"Ingrese la cantidad de polinomios a sumar: "}, explicacion{"Si 'c' es un numero real, 'g' un numero entero no negativo (siendo 'c' un coeficiente y 'g' el grado de 'x')\n\nLos terminos de los polinomio solo pueden tener estas formas: c, c*xg, c*x, x, xg\n-----------------------------------------------------------------------------------------------------------------------\n\n"};
    tPolinomio GC;
    cout << explicacion;
    double cantidad_pol{control_enteros_positivos(msg)};
    while (cantidad_pol < 2){
        cout << "Error!! la cantidadad debe ser al menos 2: ";
        cantidad_pol = control_enteros_positivos("");
    }
    cout << "(" << cantidad_pol << " polinomios)" << endl << endl;
    for (int i = 1; i <= cantidad_pol ; i++){
        cout << "Polinomio " << i << ": ";
        getline(cin, polinomio_str);
        separador_grads_coefs(polinomio_str, GC);
        while (GC.grad.back() == -1){
            polinomio_str = "";
            GC.grad.pop_back();
            cout << "Error!! Corrija la forma de los terminos del polinomio: ";
            getline(cin, polinomio_str);
            separador_grads_coefs(polinomio_str, GC);
        }
    }
    suma_x_semejantes(GC);
    orden_ascendente(GC);
    print_suma_polinomios(GC);
    return 0;
}

//----------------------------------------------------------------FUNCIONES----------------------------------------------------------------

bool es_num(char c){
    if (c == '0' || c == '1' || c == '2'|| c == '3'|| c == '4'|| c == '5'|| c == '6'|| c == '7'|| c == '8'|| c == '9') return 1; 
    return 0;
}

bool ver_si_siguiente_char_en_str_es_signo(int i, string &polinomio_str){ //verifica si el siguiente caracter es un signo o la iteracion esta en el ultimo valor que puede tomar i
    if (((i < polinomio_str.length() - 1) && (polinomio_str[i+1] == '+' || polinomio_str[i+1] == '-')) || (i == polinomio_str.length() - 1)) return 1; 
    return 0;
}

bool es_int_un_num_str(string &str){
    const double ERROR = 1E-5;
    double d_num_dec, d_exp;
    int indi_punto{0}, fin_double{0}, cont_num_dec{0};
    string exp{"0"}, num_dec{"0"};
    bool indi_e{0}, dec_0{0};
    for (int i = 0; i < str.length(); i++){
        //Se cuentan decimales y grado del "10" para saber si será un entero
        if (indi_punto && !fin_double && str[i] != '.') {cont_num_dec++; num_dec += str[i];}
        if (fin_double && indi_e && str[i] != 'e') exp += str[i];
        //indicadores
        if(str[i] == '.') indi_punto++;
        else if (str[i] == 'e'){indi_e = 1; indi_punto = 0;}
        else if(indi_punto == 1 && i < str.length() - 1 && str[i+1] == 'e') fin_double++;

        try {
            d_num_dec = stod(num_dec);
            d_exp = stod(exp);
        }
        catch (const out_of_range&) {
        cout << "[Overflow] ";
        return 0;
        }

        //no se cuentas los decimales si son "0"
        dec_0 = d_num_dec >= 0 - ERROR && d_num_dec <= 0 + ERROR;
        if(dec_0 && (i == str.length() - 1)) cont_num_dec = 0;
        //errores en formato
        if ((!es_num(str[i]) && str[i] != 'e' && str[i] != '.') ||(fin_double && indi_e && str[i] != 'e' && cont_num_dec > d_exp) || !(indi_punto == 0 || indi_punto == 1) || str == "." || (indi_e == 1 && indi_punto && (str[i] != '0' && str[i] != '.')) || (!dec_0 && !indi_e && i == str.length() -1) || str[0] == 'e') return 0;
    }
    try {
        stod(str);
    }
    catch (const out_of_range&) {
        cout << "[Overflow] ";
        return 0;
    }
    return 1;
}

bool es_double_un_num_str(string &str){
    int indi_punto{0}; 
    bool indi_e{0};
    for (int i = 0; i < str.length(); i++){
        //indicadores
        if(str[i] == '.') indi_punto++;
        else if (str[i] == 'e') {indi_e = 1; indi_punto = 0;}
        //errores en formato
        if (!(indi_punto == 0 || indi_punto == 1) || (!es_num(str[i]) && !(i == 0 && str[i] == '-') && str[i] != '.' && str[i] != 'e') || str == "." || (indi_e && indi_punto == 1 && (str[i] != '0' && str[i] != '.')) || str[0] == 'e') return 0;
    }
    try {
        stod(str);
    }
    catch (const out_of_range&) {
        cout << "[Overflow] ";
        return 0;
    }
    return 1;  
}

bool error_forma_pol(string &polinomio_str, int &i, int &indicador_mul, int &indicador_signo, int &indicador_x, int &indicador_e, string &num_temp, string &grad_temp, bool &coeficiente_1op, bool &coeficiente_2op, bool &fin_signo){
    if(
        //mas de un signo
        !(indicador_mul == 0 || indicador_mul == 1) || !(indicador_signo == 0 || indicador_signo == 1) || !(indicador_x == 0 || indicador_x == 1) || !(indicador_e == 0 || indicador_e == 1)
        //caracteres que no forman parte de un polinomio
        || !(es_num(polinomio_str[i]) || polinomio_str[i] == 'x' || polinomio_str[i] == 'X' || polinomio_str[i] == '*' || polinomio_str[i] == '-' || polinomio_str[i] == '+' || polinomio_str[i] == '.' || polinomio_str[i] == ' ' || polinomio_str[i] == 'e')  
        //un '*' pero no hubo ningun numero antes o un '*' pero despues hay otro numero y no la 'x'
        || (indicador_mul == 1 && (num_temp == "" || num_temp == "-" || coeficiente_1op || coeficiente_2op)) 
        //un '*' despues de la 'x'
        || (indicador_x == 1 && indicador_mul == 1) 
        //coeficiente segido de la 'x' pero sin el '*' antes de la 'x'
        || ((indicador_mul == 0) && ((i < polinomio_str.length() - 1) && (polinomio_str[i+1] == 'x' || polinomio_str[i+1] == 'X')) && !(num_temp == "" || num_temp == "-"))
        //grados con decimal diferente de 0 o con mas de un "." y coeficientes con mas de un "."
        || (fin_signo && (!es_int_un_num_str(grad_temp) || !es_double_un_num_str(num_temp)))
        //signos que no van segidos de algun numero o x
        || ((num_temp == "-" || num_temp == "") && i == (polinomio_str.length() - 1))
    ) return 1;
    return 0;
}

void separador_grads_coefs(string &polinomio_str, tPolinomio &GC){
    if(polinomio_str == "") {GC.grad.push_back(-1); return;}
    string num_temp{""}, grad_temp{"0"};
    bool coeficiente_1op, coeficiente_2op, grad_1op, fin_signo;
    int indicador_x{0}, indicador_signo{1}, indicador_mul{0}, indicador_e{0},cont{0};
    for(int i = 0; i < polinomio_str.length(); i++){
        //indicadores
        if ((polinomio_str[i] == '+' || polinomio_str[i] == '-') && cont != 0) indicador_signo++;
        else if  (polinomio_str[i] == '*') indicador_mul++;
        else if (polinomio_str[i] == 'x' || polinomio_str[i] == 'X') {indicador_x++; indicador_mul = 0; indicador_e = 0;}
        else if (polinomio_str[i] == 'e') indicador_e++;

        coeficiente_1op = indicador_signo && !indicador_x && (es_num(polinomio_str[i]) || polinomio_str[i] == '.' || polinomio_str[i] == '-' || polinomio_str[i] == 'e');
        coeficiente_2op = indicador_signo && indicador_x && (num_temp == "" || num_temp == "-");

        grad_1op = indicador_x == 1 && (es_num(polinomio_str[i]) || polinomio_str[i] == '.' || polinomio_str[i] == 'e');

        //coeficientes
        if(coeficiente_1op) num_temp += polinomio_str[i] ;
        else if (coeficiente_2op) num_temp += "1";
        //grados
        if(grad_1op) grad_temp += polinomio_str[i] ;
        else if(indicador_x == 1 && (grad_temp == "0") && ver_si_siguiente_char_en_str_es_signo(i, polinomio_str)) grad_temp = "1";

        fin_signo = indicador_signo == 1 && ver_si_siguiente_char_en_str_es_signo(i, polinomio_str) && !(num_temp == "" || num_temp == "-");

        //exponente "1" en notacion cientifica
        if (fin_signo && num_temp[num_temp.length() - 1] == 'e') num_temp += "1";
        if (fin_signo && grad_temp[grad_temp.length() - 1] == 'e') grad_temp += "1";

        //error en formato
        if (error_forma_pol(polinomio_str, i, indicador_mul, indicador_signo, indicador_x, indicador_e, num_temp, grad_temp, coeficiente_1op, coeficiente_2op, fin_signo)) {
            for(int j = 0; j < cont; j++){
                GC.grad.pop_back();
                GC.coeficiente.pop_back();
            }
            GC.grad.push_back(-1); 
            break;
            }

        //almaceno grados-coeficientes
        if(fin_signo){
            GC.grad.push_back(stod(grad_temp)); 
            GC.coeficiente.push_back(stod(num_temp));
            indicador_mul = 0;
            indicador_signo = 0; 
            indicador_x = 0; 
            indicador_e = 0;
            num_temp = ""; 
            grad_temp = "0";
            cont++;
        }
    }
}

double str_a_num(string &str){
    int indicador_e{0};
    double d_num;
    string num{""};
    for(int i = 0; i < str.length(); i++){
        //indicacadores
        if (str[i] == 'e') indicador_e++;
        //se agregar los caracteres para formar un numero
        if (es_num(str[i]) || str[i] == '.' || str[i] == 'e') num += str[i];
        else if (str[i] != ' ') return -1;
    }
    if (num[num.length()-1] == 'e') num += "1";
    //errores en formato
    if (!(indicador_e == 0 || indicador_e == 1) || !es_int_un_num_str(num) || str == "") return -1;

    try {
    d_num = stod(num);
    }
    catch (const out_of_range&) {
    cout << "[Overflow] ";
    return -1;
    }

    return d_num;
}

double control_enteros_positivos(string msg){
    string str;
    cout << msg;
    getline(cin, str);
    double num{str_a_num(str)};
    while (num == -1){
        cout << "Error!! La cantidad solo puede ser un numero entero positivo, ingrese nuevamente: ";
        getline(cin, str);
        num = str_a_num(str);
    }
    return num;
}


void suma_x_semejantes(tPolinomio &GC){
    for (int i_grad = 0;  i_grad < GC.grad.size(); i_grad++){
        if(GC.coeficiente.at(i_grad) == 0){ //0 multiplica x, no se almacena
                    GC.grad.erase(GC.grad.begin()+i_grad);
                    GC.coeficiente.erase(GC.coeficiente.begin()+i_grad);
        }
        for(int i_coef = i_grad+1; i_coef < GC.grad.size(); i_coef++){
            if (GC.grad.at(i_coef) == GC.grad.at(i_grad)){
                GC.coeficiente.at(i_grad) += GC.coeficiente.at(i_coef);
                GC.grad.erase(GC.grad.begin()+i_coef);
                GC.coeficiente.erase(GC.coeficiente.begin()+i_coef);
                i_coef--;
            }
        }
    }
}

void intercambiar(double &a, double&b){
    double temp{a};
    a = b;
    b = temp;
}

void orden_ascendente(tPolinomio &GC){
    for (int i_grad = 0;  i_grad < GC.grad.size(); i_grad++){
        for(int i_coef = i_grad+1; i_coef < GC.grad.size(); i_coef++){
            if (GC.grad.at(i_coef) < GC.grad.at(i_grad)){
                intercambiar(GC.grad.at(i_grad), GC.grad.at(i_coef));
                intercambiar(GC.coeficiente.at(i_grad), GC.coeficiente.at(i_coef));
            }
        }
    }
}

void print_suma_polinomios(tPolinomio &GC){
    string signo_mas = " + ", signo_menos = " - ";
    if(GC.grad.size() == 0) cout << "\nLa suma de los polinomios da 0";
    else{
        cout << "\nEl polinomio suma es de grado " << GC.grad.at(GC.grad.size()-1) <<":\n";
        for(int i = 0;  i < GC.grad.size(); i++){
            //estilización
            if(i == 0){signo_mas = ""; signo_menos = "- ";}
            else{signo_mas = " + "; signo_menos = " - ";
            }
            //no se imprimen las x
            if(GC.grad.at(i) == 0){
                if(GC.coeficiente.at(i) < 0) cout << signo_menos << -GC.coeficiente.at(i); 
                else cout << signo_mas << GC.coeficiente.at(i);
            }
            //no se imprimen los coefs 
            else if (GC.coeficiente.at(i) == 1){
                if(GC.grad.at(i) == 1) cout << signo_mas <<"x";
                else cout << signo_mas << "x" << GC.grad.at(i);
            }else if (GC.coeficiente.at(i) == -1){
                if(GC.grad.at(i) == 1) cout << signo_menos <<"x";
                else cout << signo_menos << "x" << GC.grad.at(i);
            }
            //se imprime todo
            else if (GC.coeficiente.at(i) >= 0){
                if(GC.grad.at(i) == 1) cout << signo_mas << GC.coeficiente.at(i) <<"*x";
                else cout << signo_mas << GC.coeficiente.at(i) << "*x" << GC.grad.at(i);
            }else if (GC.coeficiente.at(i) < 0){
                if(GC.grad.at(i) == 1) cout << signo_menos << -GC.coeficiente.at(i) <<"*x";
                else cout << signo_menos << -GC.coeficiente.at(i) << "*x" << GC.grad.at(i);
            }
        }
    }
}