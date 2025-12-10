#include <iostream>
#include <string>
#include <cctype>

using namespace std;

class calculate {
private:

    string OPN;
    int baseIdx = 0;

public:

    int priority(char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        return 0;
    }

    string toRPN(const string& expr) {
        if (expr.length() == 1) return expr;

        int balance = 0;
        int opIndex = -1;
        int minP = 3;

        for (int i = 0; i < expr.length(); ++i) {
            char c = expr[i];
            if (c == '(') balance++;
            else if (c == ')') balance--;
            else if (balance == 0 && (c == '+' || c == '-' || c == '*' || c == '/')) {
                if (priority(c) <= minP) {
                    minP = priority(c);
                    opIndex = i;
                }
            }
        }

        if (opIndex != -1) {
            string left = expr.substr(0, opIndex);
            string op(1, expr[opIndex]);
            string right = expr.substr(opIndex + 1);
            return toRPN(left) + toRPN(right) + op;
        }

        if (expr[0] == '(' && expr.back() == ')') {
            return toRPN(expr.substr(1, expr.length() - 2));
        }

        cout << "неправильно введено " << expr << endl;
        return "";
    }

    double eval(const string& expr) {
        OPN = toRPN(expr);
        baseIdx = OPN.length() - 1;
        return evalRPN(baseIdx);
    }

    double evalRPN(int& idx) {
        char c = OPN[idx--];

        if (c == '+' || c == '-' || c == '*' || c == '/') {
            double b = evalRPN(idx);
            double a = evalRPN(idx);

            if (c == '+') return a + b;
            if (c == '-') return a - b;
            if (c == '*') return a * b;
            if (c == '/'){
                if (b == 0){
                    cout << "ошибка: нельзя делить на ноль";
                    return 1;
                }
                else { return a / b; }
            }
        }
        
        else{
            int g;
            switch (c) {
                case '0': g = 0; break;
                case '1': g = 1; break;
                case '2': g = 2; break;
                case '3': g = 3; break;
                case '4': g = 4; break;
                case '5': g = 5; break;
                case '6': g = 6; break;
                case '7': g = 7; break;
                case '8': g = 8; break;
                case '9': g = 9; break;
            }
            return g;
        }
    }
};

int main() {
    calculate calc;
    string expr;
    getline(cin, expr);
    cout << "ОПН: " << calc.toRPN(expr) << endl;
    cout << "Результат: " << calc.eval(expr) << endl;
    return 0;
}
