#include "test_util.h"
#include <iostream>

void out_err(crmna_err *err) {
  if (err->curret_line == 0) {
    std::cout << "no error" << std::endl;
  } else {
    for (int i = 0; i < err->curret_line; i++) {
      std::cout << err->error_msg[i] << std::endl;
    }
  }
}