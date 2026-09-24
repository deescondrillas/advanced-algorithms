
#include <iostream>
#include <string>

#include "SuffixArray.hpp"
#include "Manacher.hpp"

using namespace std;

int main() {
  string transmission1, transmission2, mcode1, mcode2, mcode3;
  cin >> transmission1;
  cin >> transmission2;
  cin >> mcode1;
  cin >> mcode2;
  cin >> mcode3;

  cout << "=== Parte 1 ===" << endl;
  SuffixArray t1(transmission1);
  SuffixArray t2(transmission2);
  int mcode1InT1 = t1.find(mcode1);
  int mcode2InT1 = t1.find(mcode2);
  int mcode3InT1 = t1.find(mcode3);   
  int mcode1InT2 = t2.find(mcode1);
  int mcode2InT2 = t2.find(mcode2);
  int mcode3InT2 = t2.find(mcode3);
  cout << (mcode1InT1 >= 0 ? "true " : "false ") << mcode1InT1 << endl; 
  cout << (mcode2InT1 >= 0 ? "true " : "false ") << mcode2InT1 << endl; 
  cout << (mcode3InT1 >= 0 ? "true " : "false ") << mcode3InT1 << endl; 
  cout << (mcode1InT2 >= 0 ? "true " : "false ") << mcode1InT2 << endl; 
  cout << (mcode2InT2 >= 0 ? "true " : "false ") << mcode2InT2 << endl; 
  cout << (mcode3InT2 >= 0 ? "true " : "false ") << mcode3InT2 << endl; 
  
  return 0;
}

/*
int main() {
	string textA = "";
	string textB = "";

	if (cin >> textA >> textB) {
		cout << "=== Buscador de Longest Common Substring (LCS) ===" << endl;
		cout << "Cadena A: " << textA << endl;
		cout << "Cadena B: " << textB << endl;

		SuffixArray stringMatch(textA, textB);
		string resultadoLcs = stringMatch.lcs();

		cout << "\nResultado LCS: ";
		if (resultadoLcs.empty()) {
			cout << "[Ninguno]" << endl;
		} else {
			cout << resultadoLcs << endl;
		}

		cout << "Longitud: " << resultadoLcs.length() << endl;
	}

	return 0;
}
 */
