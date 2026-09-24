#include "SessionText.hpp"

#include <iostream>

namespace {
int hexDigit(char value) {
  if (value >= '0' && value <= '9') return value - '0';
  if (value >= 'a' && value <= 'f') return value - 'a' + 10;
  if (value >= 'A' && value <= 'F') return value - 'A' + 10;
  return -1;
}
}

// El alfabeto admitido es el de la actividad: 0-9, A-F, CR y LF.
bool decodeText(const std::string& encoded, std::string& text) {
  if (encoded.size() % 2 || encoded.size() > 20000) return false;
  for (size_t i = 0; i < encoded.size(); i += 2) {
    int high = hexDigit(encoded[i]), low = hexDigit(encoded[i + 1]);
    if (high < 0 || low < 0) return false;
    char current = static_cast<char>(16 * high + low);
    if (!(current >= '0' && current <= '9') && !(current >= 'A' && current <= 'F')
        && current != '\n' && current != '\r') return false;
    text += current;
  }
  return true;
}

void writeJsonText(const std::string& text) {
  std::cout << '"';
  for (char current : text) {
    if (current == '\n') std::cout << "\\n";
    else if (current == '\r') std::cout << "\\r";
    else std::cout << current; // decodeText solo admite 0-9, A-F, CR y LF.
  }
  std::cout << '"';
}
