#pragma once

#include <string>

// Datos de una operación; los índices internos y del resultado comienzan en 0.
struct ManacherState {
  std::string phase = "ready";
  int i = -1, center = 0, right = 0, mirror = -1, matches = 0;
  int compareLeft = -1, compareRight = -1, comparison = -1;
  int comparisons = 0, reused = 0, step = 0;
  int start = 0, end = 0, length = 0;
  bool finished = false;
};
