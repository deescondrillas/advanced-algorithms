
#include <iostream>
#include <string>
#include "SuffixArray.hpp"

using namespace std;

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
