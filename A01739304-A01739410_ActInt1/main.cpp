
#include <iostream>
#include <string>
#include "SuffixArray.hpp"

using namespace std;

int main() {
  string A, B;
  cin >> A >> B;
  SuffixArray suffix(A);
  cout << suffix.find(B) << endl;
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
