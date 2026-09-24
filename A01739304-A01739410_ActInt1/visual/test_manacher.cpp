#include "../Manacher.hpp"
#include "ManacherState.hpp"

#include <cassert>
#include <iostream>
#include <random>

pair<int, int> bruteForce(const string& text) {
  pair<int, int> best = {0, 0};
  int length = 0, size = static_cast<int>(text.size());
  for (int left = 0; left < size; ++left)
    for (int right = left; right < size; ++right) {
      bool palindrome = true;
      for (int a = left, b = right; a < b; ++a, --b)
        if (text[a] != text[b]) { palindrome = false; break; }
      if (palindrome && right - left + 1 > length) {
        length = right - left + 1;
        best = {left, right};
      }
    }
  return best;
}

void check(const string& text) {
  Manacher normal(text);
  const pair<int, int> expected = bruteForce(text);
  assert(normal.longestPalindrome() == expected);

  string transformed(1, '\0');
  for (char value : text) { transformed += value; transformed += '\0'; }
  vector<int> finalRadius;
  ManacherState lastState;
  int calls = 0;
  Manacher observed(text, [&](const ManacherState& state, const vector<int>& radius) {
    assert(state.step == calls++);
    assert(calls < 16 * static_cast<int>(transformed.size()) + 1);
    if (state.phase == "ready") {
      assert(state.i == -1 && state.length == 0 && state.step == 0);
    }
    if (state.phase == "compare") {
      assert(state.compareLeft >= 0);
      assert(state.compareRight < static_cast<int>(transformed.size()));
      assert(state.comparison == (transformed[state.compareLeft] == transformed[state.compareRight]));
    }
    if (state.length > 0) {
      assert(state.start >= 0 && state.end < static_cast<int>(text.size()));
      assert(state.length == state.end - state.start + 1);
      for (int left = state.start, right = state.end; left < right; ++left, --right)
        assert(text[left] == text[right]);
    }
    if (state.finished) finalRadius = radius;
    lastState = state;
  });
  assert(observed.longestPalindrome() == expected);
  assert(lastState.finished && lastState.phase == "done");
  assert(lastState.length == (text.empty() ? 0 : expected.second - expected.first + 1));

  // Cada radio se contrasta con expansión directa, independientemente del observador.
  assert(finalRadius.size() == transformed.size());
  for (int i = 0; i < static_cast<int>(transformed.size()); ++i) {
    int radius = 0;
    while (i - radius - 1 >= 0 && i + radius + 1 < static_cast<int>(transformed.size())
           && transformed[i - radius - 1] == transformed[i + radius + 1]) ++radius;
    assert(finalRadius[i] == radius);
  }
}

int main() {
  int cases = 0;
  for (int length = 0, combinations = 1; length <= 8; ++length, combinations *= 3)
    for (int code = 0; code < combinations; ++code) {
      string text(length, 'A');
      int value = code;
      for (char& current : text) { current += value % 3; value /= 3; }
      check(text);
      ++cases;
    }
  mt19937 random(39304);
  const string alphabet = "0123456789ABCDEF\r\n";
  for (int test = 0; test < 500; ++test) {
    string text(random() % 80, '0');
    for (char& current : text) current = alphabet[random() % alphabet.size()];
    check(text);
  }
  check(string("A\0A", 3)); check("FABBAC"); check("FABCBA0"); check("ABBA0CDDC"); check("A\nA");
  Manacher large(string(1000000, 'A'));
  assert(large.longestPalindrome() == make_pair(0, 999999));
  cout << "PASS: " << cases << " exhaustivos, 500 aleatorios, observador, radios y 1M caracteres.\n";
}
