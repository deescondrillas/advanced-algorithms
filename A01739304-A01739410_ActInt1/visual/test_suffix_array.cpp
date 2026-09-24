#include "../SuffixArray.hpp"
#include "SuffixArrayState.hpp"

#include <cassert>
#include <iostream>
#include <random>

/// Longitud del substring común más largo, por fuerza bruta -- O(|a|·|b|·min)
int bruteForceLcs(const string& a, const string& b) {
  int best = 0;
  for (size_t i = 0; i < a.size(); ++i)
    for (size_t j = 0; j < b.size(); ++j) {
      int length = 0;
      while (i + length < a.size() && j + length < b.size() && a[i + length] == b[j + length])
        ++length;
      best = max(best, length);
    }
  return best;
}

// El observador no puede alterar el resultado ni el recorrido del algoritmo.
void checkLcs(const string& a, const string& b) {
  SuffixArray plain(a, b);
  const vector<int> expected = plain.lcs();

  SuffixArray observed(a, b);
  LcsState last;
  int calls = 0;
  vector<int> answer = observed.lcs(
    [&](const LcsState& state, const vector<int>& sa, const vector<int>& lcp) {
      assert(state.step == calls++);
      assert(sa.size() == a.size() + b.size() + 2 && lcp.size() == sa.size());
      if (state.phase == "ready") assert(state.i == -1 && state.step == 0);
      if (state.i >= 0) {
        assert(state.i > 0 && state.i < static_cast<int>(sa.size()));
        assert(state.lcpHere == lcp[state.i]);
        assert(state.bestLength == lcp[state.idxBest]);
      }
      last = state;
    });

  assert(answer == expected);
  assert(last.finished && last.phase == "done");

  const int length = bruteForceLcs(a, b);
  assert(last.bestLength == length);
  if (!length) {
    assert(answer == vector<int>({0, -1, 0, -1}));
    return;
  }
  // Los dos rangos son válidos y contienen el mismo substring.
  assert(answer[1] - answer[0] + 1 == length && answer[3] - answer[2] + 1 == length);
  assert(answer[0] >= 0 && answer[1] < static_cast<int>(a.size()));
  assert(answer[2] >= 0 && answer[3] < static_cast<int>(b.size()));
  assert(a.substr(answer[0], length) == b.substr(answer[2], length));
}

void checkFind(const string& text, const string& pattern) {
  const string fullText = text + '\0';
  size_t position = text.find(pattern);
  const int expected = position == string::npos ? -1 : static_cast<int>(position);

  SuffixArray plain(text);
  assert(plain.find(pattern) == expected);

  SuffixArray observed(text);
  FindState last;
  int calls = 0, compares = 0;
  int answer = observed.find(pattern,
    [&](const FindState& state, const vector<int>& sa, const vector<int>& lcp) {
      assert(state.step == calls++);
      assert(sa.size() == text.size() + 1 && lcp.size() == sa.size());
      if (state.phase == "ready") assert(state.column == -1 && state.step == 0);
      if (state.column >= 0) {
        assert(state.column < static_cast<int>(sa.size()));
        assert(sa[state.column] == state.start && state.lcpHere == lcp[state.column]);
      }
      if (state.phase == "compare") {
        ++compares;
        assert(state.comparePattern >= 0 && state.comparePattern < static_cast<int>(pattern.size()));
        assert(state.compareText == state.start + state.comparePattern);
        assert(state.compareText < static_cast<int>(fullText.size()));
        assert(state.comparison
               == (pattern[state.comparePattern] == fullText[state.compareText]));
      }
      if (state.phase == "match") assert(state.matches == static_cast<int>(pattern.size()));
      last = state;
    });

  assert(answer == expected);
  assert(last.finished && last.phase == "done" && last.comparisons == compares);
  assert(last.firstMatchIdx == expected);
}

int main() {
  int cases = 0;
  // Exhaustivo sobre un alfabeto pequeño: todos los pares hasta cuatro caracteres.
  for (int lengthA = 0, combinationsA = 1; lengthA <= 4; ++lengthA, combinationsA *= 3)
    for (int codeA = 0; codeA < combinationsA; ++codeA) {
      string a(lengthA, 'A');
      int value = codeA;
      for (char& current : a) { current += value % 3; value /= 3; }
      for (int lengthB = 0, combinationsB = 1; lengthB <= 4; ++lengthB, combinationsB *= 3)
        for (int codeB = 0; codeB < combinationsB; ++codeB) {
          string b(lengthB, 'A');
          value = codeB;
          for (char& current : b) { current += value % 3; value /= 3; }
          checkLcs(a, b);
          checkFind(a, b);
          ++cases;
        }
    }

  mt19937 random(39410);
  const string alphabet = "0123456789ABCDEF";
  auto build = [&](size_t length) {
    string text(length, '0');
    for (char& current : text) current = alphabet[random() % alphabet.size()];
    return text;
  };
  for (int test = 0; test < 300; ++test) {
    const string a = build(random() % 60), b = build(random() % 60);
    checkLcs(a, b);
    checkFind(a, a.substr(min(a.size(), size_t(random() % 6)), random() % 5));
    checkFind(a, b);
    ++cases;
  }

  // Casos de la actividad y bordes: sin coincidencias, patrón completo y repeticiones.
  checkLcs("ABACABADABACABA", "DABADABACABACABA");
  checkFind("ABACABADABACABA", "ABACA");
  checkFind("ABACABADABACABA", "ABADABACA");
  checkFind("ABACABADABACABA", "ABABA");
  checkLcs("AAAA", "BBBB");
  checkLcs("A", "A");
  checkFind("AAAAAA", "AAAAAA");
  checkFind("AAAAAA", "AAAAAAA");

  cout << "PASS: " << cases << " casos de lcs y find, observador, rangos y patrones.\n";
}
