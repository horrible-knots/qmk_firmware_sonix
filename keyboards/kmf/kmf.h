// Copyright 2022 nobody (@nobody)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"
#define NC KC_NO

#define LAYOUT_default( \
      /* A0,   A1,   A2,    A3,    A4,    A5,    A6,    A7,    A8,    A9,   A10,   A11,   A12,   A13,  A14,    B0,    B1,    B2,   B3,    B4,    B5 */  \
/*C15*/ K00,        K02,   K03,   K04,   K05,   K06,   K07,   K08,   K09,   K0A,   K0B,   K0C,   K0D,  K0E,   K0F,   K0G,                               \
/*D11*/ K10,  K11,  K12,   K13,   K14,   K15,   K16,   K17,   K18,   K19,   K1A,   K1B,   K1C,   K1D,  K1E,   K1F,   K1G,   K1H,   K1I,   K1J,   K1K,   \
/*D10*/ K20,  K21,  K22,   K23,   K24,   K25,   K26,   K27,   K28,   K29,   K2A,   K2B,   K2C,   K2D,  K2E,   K2F,   K2G,   K2H,   K2I,   K2J,   K2K,   \
/*D09*/ K30,  K31,  K32,   K33,   K34,   K35,   K36,   K37,   K38,   K39,   K3A,   K3B,          K3D,                       K3H,   K3I,   K3J,          \
/*D08*/ K40,        K42,   K43,   K44,   K45,   K46,   K47,   K48,   K49,   K4A,   K4B,          K4D,         K4F,          K4H,   K4I,   K4J,   K4K,   \
/*D09*/ K50,  K51,  K53,                        K56,                        K5A,   K5B,   K5C,   K5D,  K5E,   K5F,   K5G,          K5I,   K5J           \
)  																			\
     {/* A0,   A1,   A2,    A3,    A4,    A5,    A6,    A7,    A8,    A9,   A10,   A11,   A12,   A13,  A14,    B0,    B1,    B2,   B3,    B4,    B5 */  \
/*C15*/{K00,   NC,  K02,   K03,   K04,   K05,   K06,   K07,   K08,   K09,   K0A,   K0B,   K0C,   K0D,  K0E,   K0F,   K0G,    NC,    NC,    NC,    NC  },\
/*D11*/{K10,  K11,  K12,   K13,   K14,   K15,   K16,   K17,   K18,   K19,   K1A,   K1B,   K1C,   K1D,  K1E,   K1F,   K1G,   K1H,   K1I,   K1J,   K1K  },\
/*D10*/{K20,  K21,  K22,   K23,   K24,   K25,   K26,   K27,   K28,   K29,   K2A,   K2B,   K2C,   K2D,  K2E,   K2F,   K2G,   K2H,   K2I,   K2J,   K2K  },\
/*D09*/{K30,  K31,  K32,   K33,   K34,   K35,   K36,   K37,   K38,   K39,   K3A,   K3B,    NC,   K3D,   NC,    NC,    NC,   K3H,   K3I,   K3J,    NC  },\
/*D08*/{K40,   NC,  K42,   K43,   K44,   K45,   K46,   K47,   K48,   K49,   K4A,   K4B,    NC,   K4D,   NC,   K4F,    NC,   K4H,   K4I,   K4J,   K4K  },\
/*D09*/{K50,  K51,  K53,    NC,    NC,    NC,   K56,    NC,    NC,    NC,   K5A,   K5B,   K5C,   K5D,  K5E,   K5F,   K5G,    NC,   K5I,   K5J,    NC  } \
}
