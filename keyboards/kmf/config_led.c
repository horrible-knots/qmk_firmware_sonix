#ifdef RGB_MATRIX_ENABLE

#include "rgb_matrix.h"
#include "config_led.h"
#define XXX

const led_config_t g_led_config = { {
  {   0, NO_LED,   1,   2,   3,   4,   5,      6,   7,      8,      9,     10,     11,     12,     13,     14,     15,  NO_LED, NO_LED, NO_LED, NO_LED },
  {  16,     17,  18,  19,  20,  21,  22,     23,  24,     25,     26,     27,     28,     29,     30,     31,     32,      33,     34,     35,     36 },
  {  37,     38,  39,  40,  41,  42,  43,     44,  45,     46,     47,     48,     49,     50,     51,     52,     53,      54,     55,     56,     57 },
  {  58,     59,  60,  61,  62,  63,  64,     65,  66,     67,     68,     69,     NO_LED, 70, NO_LED, NO_LED, NO_LED,      71,     72,     73, NO_LED },
  {  74, NO_LED,  75,  76,  77,  78,  79,     80,  81,     82,     83,     84,     NO_LED, 85, NO_LED,     86, NO_LED,      87,     88,     89,     90 },
  {  91,     92,  93,NO_LED,NO_LED,NO_LED,94, NO_LED,NO_LED,NO_LED,95,     96,     97,     98,     99,    100,    101,      NO_LED,102,    103, NO_LED }
}, {
    { 1 *12, 0  }, /************/ { 3 *12, 0  }, { 4 *12, 0  }, { 5 *12,   0 }, { 6 *12,  0  }, { 7 *12,  0  }, { 8 *12,  0 }, { 9 *12,   0 }, { 10 *12, 0 }, { 11*12, 0  }, { 12*12, 0  }, { 13*12, 0  }, { 14*12,  0 }, { 15*12,  0 }, { 16*12, 0  }, { 17*12, 0  }, /************/ /************/ /************/ /************/
    { 1 *12, 11 }, { 2 *12, 11 }, { 3 *12, 11 }, { 4 *12, 11 }, { 5 *12,  11 }, { 6 *12,  11 }, { 7 *12,  11 }, { 8 *12, 11 }, { 9 *12,  11 }, { 10*12, 11 }, { 11*12, 11 }, { 12*12, 11 }, { 13*12, 11 }, { 14*12, 11 }, { 15*12, 11 }, { 16*12, 11 }, { 17*12, 11 }, { 18*12, 11 }, { 19*12, 11 }, { 20*12, 11 }, { 21*12, 11 },
    { 1 *12, 22 }, { 2 *12, 22 }, { 3 *12, 22 }, { 4 *12, 22 }, { 5 *12,  22 }, { 6 *12,  22 }, { 7 *12,  22 }, { 8 *12, 22 }, { 9 *12,  22 }, { 10*12, 22 }, { 11*12, 22 }, { 12*12, 22 }, { 13*12, 22 }, { 14*12, 22 }, { 15*12, 22 }, { 16*12, 22 }, { 17*12, 22 }, { 18*12, 22 }, { 19*12, 22 }, { 20*12, 22 }, { 21*12, 22 },
    { 1 *12, 33 }, { 2 *12, 33 }, { 3 *12, 33 }, { 4 *12, 33 }, { 5 *12,  33 }, { 6 *12,  33 }, { 7 *12,  33 }, { 8 *12, 33 }, { 9 *12,  33 }, { 10*12, 33 }, { 11*12, 33 }, { 12*12, 33 }, /************/ { 14*12, 33 }, /************/ /************/ /************/ { 18*12, 33 }, { 19*12, 33 }, { 20*12, 33 }, /************/
    { 1 *12, 44 }, /************/ { 3 *12, 44 }, { 4 *12, 44 }, { 5 *12,  44 }, { 6 *12,  44 }, { 7 *12,  44 }, { 8 *12, 44 }, { 9 *12,  44 }, { 10*12, 44 }, { 11*12, 44 }, { 12*12, 44 }, /************/ { 14*12, 44 }, /************/ { 16*12, 44 }, /************/ { 18*12, 44 }, { 19*12, 44 }, { 20*12, 44 }, { 21*12, 44 },
    { 1 *12, 64 }, { 2 *12, 64 }, { 3 *12, 64 }, /************/ /*************/ /*************/ { 7 *12,  64 }, /*************/ /************/ /************/ { 11*12, 64 }, { 12*12, 64 }, { 13*12, 64 }, { 14*12, 64 }, { 15*12, 64 }, { 16*12, 64 }, { 17*12, 64 }, /***********/  { 19*12, 64 }, { 20*12, 64 }, /************/
}, {
      4,      XXX  4,   4,   4,   4,   4,       4,  4,       4,      4,      4,     4,      4,      4,      4,      4,       XXX     XXX     XXX     XXX
      4,      4,   4,   4,   4,   4,   4,       4,  4,       4,      4,      4,     4,      4,      4,      4,      4,       4,      4,      4,      4,
      4,      4,   4,   4,   4,   4,   4,       4,  4,       4,      4,      4,     4,      4,      4,      4,      4,       4,      4,      4,      4,
      4,      XXX  4,   4,   4,   4,   4,       4,  4,       4,      4,      4,     4,      XXX     4,      XXX     XXX      XXX     4,      4,      4,
      4,      XXX  4,   4,   4,   4,   4,       4,  4,       4,      4,      4,     XXX     4,      XXX     4,      XXX      4,      4,      4,      4,
      4,      4,   4,   XXX  XXX  XXX  4,       XXX XXX      XXX     4,      4,     4,      4,      4,      4,      4,       XXX     4,      4,      XXX
} };

#endif
