
#ifndef _QR_PRIVATE_H_
#define _QR_PRIVATE_H_
#include "qr.h"
#include <stdlib.h>
#include <string.h>
#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif
#define TRUE 1
#define FALSE 0
const qr_vertable_t qr_vertable[QR_VER_MAX+1] = {
        { NAV, NAV, NAV, NAV, { 0, 0, 0, 0 },
                {{ NAV, { 0, 0, 0, 0 }, NAV, {{ 0, 0, 0, 0 }}},
                        { NAV, { 0, 0, 0, 0 }, NAV, {{ 0, 0, 0, 0 }}},
                        { NAV, { 0, 0, 0, 0 }, NAV, {{ 0, 0, 0, 0 }}},
                        { NAV, { 0, 0, 0, 0 }, NAV, {{ 0, 0, 0, 0 }}}},
                NAV, { NAV, NAV }},
        { 1, 21, 26, 0, { 10, 9, 8, 8 },
                {{ 19, { 41, 25, 17, 10 }, 1, {{ 1, 26, 19, 2 }}},
                        { 16, { 34, 20, 14, 8 }, 1, {{ 1, 26, 16, 4 }}},
                        { 13, { 27, 16, 11, 7 }, 1, {{ 1, 26, 13, 6 }}},
                        { 9, { 17, 10, 7, 4 }, 1, {{ 1, 26, 9, 8 }}}},
                0, { NAV, NAV }},
        { 2, 25, 44, 7, { 10, 9, 8, 8 },
                {{ 34, { 77, 47, 32, 20 }, 1, {{ 1, 44, 34, 4 }}},
                        { 28, { 63, 38, 26, 16 }, 1, {{ 1, 44, 28, 8 }}},
                        { 22, { 48, 29, 20, 12 }, 1, {{ 1, 44, 22, 11 }}},
                        { 16, { 34, 20, 14, 8 }, 1, {{ 1, 44, 16, 14 }}}},
                2, { 6, 18 }},
        { 3, 29, 70, 7, { 10, 9, 8, 8 },
                {{ 55, { 127, 77, 53, 32 }, 1, {{ 1, 70, 55, 7 }}},
                        { 44, { 101, 61, 42, 26 }, 1, {{ 1, 70, 44, 13 }}},
                        { 34, { 77, 47, 32, 20 }, 1, {{ 2, 35, 17, 9 }}},
                        { 26, { 58, 35, 24, 15 }, 1, {{ 2, 35, 13, 11 }}}},
                2, { 6, 22 }},
        { 4, 33, 100, 7, { 10, 9, 8, 8 },
                {{ 80, { 187, 114, 78, 48 }, 1, {{ 1, 100, 80, 10 }}},
                        { 64, { 149, 90, 62, 38 }, 1, {{ 2, 50, 32, 9 }}},
                        { 48, { 111, 67, 46, 28 }, 1, {{ 2, 50, 24, 13 }}},
                        { 36, { 82, 50, 34, 21 }, 1, {{ 4, 25, 9, 8 }}}},
                2, { 6, 26 }},
        { 5, 37, 134, 7, { 10, 9, 8, 8 },
                {{ 108, { 255, 154, 106, 65 }, 1, {{ 1, 134, 108, 13 }}},
                        { 86, { 202, 122, 84, 52 }, 1, {{ 2, 67, 43, 12 }}},
                        { 62, { 144, 87, 60, 37 }, 2, {{ 2, 33, 15, 9 }, { 2, 34, 16, 9 }}},
                        { 46, { 106, 64, 44, 27 }, 2, {{ 2, 33, 11, 11 }, { 2, 34, 12, 11 }}}},
                2, { 6, 30 }},
        { 6, 41, 172, 7, { 10, 9, 8, 8 },
                {{ 136, { 322, 195, 134, 82 }, 1, {{ 2, 86, 68, 9 }}},
                        { 108, { 255, 154, 106, 65 }, 1, {{ 4, 43, 27, 8 }}},
                        { 76, { 178, 108, 74, 45 }, 1, {{ 4, 43, 19, 12 }}},
                        { 60, { 139, 84, 58, 36 }, 1, {{ 4, 43, 15, 14 }}}},
                2, { 6, 34 }},
        { 7, 45, 196, 0, { 10, 9, 8, 8 },
                {{ 156, { 370, 224, 154, 95 }, 1, {{ 2, 98, 78, 10 }}},
                        { 124, { 293, 178, 122, 75 }, 1, {{ 4, 49, 31, 9 }}},
                        { 88, { 207, 125, 86, 53 }, 2, {{ 2, 32, 14, 9 }, { 4, 33, 15, 9 }}},
                        { 66, { 154, 93, 64, 39 }, 2, {{ 4, 39, 13, 13 }, { 1, 40, 14, 13 }}}},
                3, { 6, 22, 38 }},
        { 8, 49, 242, 0, { 10, 9, 8, 8 },
                {{ 194, { 461, 279, 192, 118 }, 1, {{ 2, 121, 97, 12 }}},
                        { 154, { 365, 221, 152, 93 }, 2, {{ 2, 60, 38, 11 }, { 2, 61, 39, 11 }}},
                        { 110, { 259, 157, 108, 66 }, 2, {{ 4, 40, 18, 11 }, { 2, 41, 19, 11 }}},
                        { 86, { 202, 122, 84, 52 }, 2, {{ 4, 40, 14, 13 }, { 2, 41, 15, 13 }}}},
                3, { 6, 24, 42 }},
        { 9, 53, 292, 0, { 10, 9, 8, 8 },
                {{ 232, { 552, 335, 230, 141 }, 1, {{ 2, 146, 116, 15 }}},
                        { 182, { 432, 262, 180, 111 }, 2, {{ 3, 58, 36, 11 }, { 2, 59, 37, 11 }}},
                        { 132, { 312, 189, 130, 80 }, 2, {{ 4, 36, 16, 10 }, { 4, 37, 17, 10 }}},
                        { 100, { 235, 143, 98, 60 }, 2, {{ 4, 36, 12, 12 }, { 4, 37, 13, 12 }}}},
                3, { 6, 26, 46 }},
        { 10, 57, 346, 0, { 12, 11, 16, 10 },
                {{ 274, { 652, 395, 271, 167 }, 2, {{ 2, 86, 68, 9 }, { 2, 87, 69, 9 }}},
                        { 216, { 513, 311, 213, 131 }, 2, {{ 4, 69, 43, 13 }, { 1, 70, 44, 13 }}},
                        { 154, { 364, 221, 151, 93 }, 2, {{ 6, 43, 19, 12 }, { 2, 44, 20, 12 }}},
                        { 122, { 288, 174, 119, 74 }, 2, {{ 6, 43, 15, 14 }, { 2, 44, 16, 14 }}}},
                3, { 6, 28, 50 }},
        { 11, 61, 404, 0, { 12, 11, 16, 10 },
                {{ 324, { 772, 468, 321, 198 }, 1, {{ 4, 101, 81, 10 }}},
                        { 254, { 604, 366, 251, 155 }, 2, {{ 1, 80, 50, 15 }, { 4, 81, 51, 15 }}},
                        { 180, { 427, 259, 177, 109 }, 2, {{ 4, 50, 22, 14 }, { 4, 51, 23, 14 }}},
                        { 140, { 331, 200, 137, 85 }, 2, {{ 3, 36, 12, 12 }, { 8, 37, 13, 12 }}}},
                3, { 6, 30, 54 }},
        { 12, 65, 466, 0, { 12, 11, 16, 10 },
                {{ 370, { 883, 535, 367, 226 }, 2, {{ 2, 116, 92, 12 }, { 2, 117, 93, 12 }}},
                        { 290, { 691, 419, 287, 177 }, 2, {{ 6, 58, 36, 11 }, { 2, 59, 37, 11 }}},
                        { 206, { 489, 296, 203, 125 }, 2, {{ 4, 46, 20, 13 }, { 6, 47, 21, 13 }}},
                        { 158, { 374, 227, 155, 96 }, 2, {{ 7, 42, 14, 14 }, { 4, 43, 15, 14 }}}},
                3, { 6, 32, 58 }},
        { 13, 69, 532, 0, { 12, 11, 16, 10 },
                {{ 428, { 1022, 619, 425, 262 }, 1, {{ 4, 133, 107, 13 }}},
                        { 334, { 796, 483, 331, 204 }, 2, {{ 8, 59, 37, 11 }, { 1, 60, 38, 11 }}},
                        { 244, { 580, 352, 241, 149 }, 2, {{ 8, 44, 20, 12 }, { 4, 45, 21, 12 }}},
                        { 180, { 427, 259, 177, 109 }, 2, {{ 12, 33, 11, 11 }, { 4, 34, 12, 11 }}}},
                3, { 6, 34, 62 }},
        { 14, 73, 581, 3, { 12, 11, 16, 10 },
                {{ 461, { 1101, 667, 458, 282 }, 2, {{ 3, 145, 115, 15 }, { 1, 146, 116, 15 }}},
                        { 365, { 871, 528, 362, 223 }, 2, {{ 4, 64, 40, 12 }, { 5, 65, 41, 12 }}},
                        { 261, { 621, 376, 258, 159 }, 2, {{ 11, 36, 16, 10 }, { 5, 37, 17, 10 }}},
                        { 197, { 468, 283, 194, 120 }, 2, {{ 11, 36, 12, 12 }, { 5, 37, 13, 12 }}}},
                4, { 6, 26, 46, 66 }},
        { 15, 77, 655, 3, { 12, 11, 16, 10 },
                {{ 523, { 1250, 758, 520, 320 }, 2, {{ 5, 109, 87, 11 }, { 1, 110, 88, 11 }}},
                        { 415, { 991, 600, 412, 254 }, 2, {{ 5, 65, 41, 12 }, { 5, 66, 42, 12 }}},
                        { 295, { 703, 426, 292, 180 }, 2, {{ 5, 54, 24, 15 }, { 7, 55, 25, 15 }}},
                        { 223, { 530, 321, 220, 136 }, 2, {{ 11, 36, 12, 12 }, { 7, 37, 13, 12 }}}},
                4, { 6, 26, 48, 70 }},
        { 16, 81, 733, 3, { 12, 11, 16, 10 },
                {{ 589, { 1408, 854, 586, 361 }, 2, {{ 5, 122, 98, 12 }, { 1, 123, 99, 12 }}},
                        { 453, { 1082, 656, 450, 277 }, 2, {{ 7, 73, 45, 14 }, { 3, 74, 46, 14 }}},
                        { 325, { 775, 470, 322, 198 }, 2, {{ 15, 43, 19, 12 }, { 2, 44, 20, 12 }}},
                        { 253, { 602, 365, 250, 154 }, 2, {{ 3, 45, 15, 15 }, { 13, 46, 16, 15 }}}},
                4, { 6, 26, 50, 74 }},
        { 17, 85, 815, 3, { 12, 11, 16, 10 },
                {{ 647, { 1548, 938, 644, 397 }, 2, {{ 1, 135, 107, 14 }, { 5, 136, 108, 14 }}},
                        { 507, { 1212, 734, 504, 310 }, 2, {{ 10, 74, 46, 14 }, { 1, 75, 47, 14 }}},
                        { 367, { 876, 531, 364, 224 }, 2, {{ 1, 50, 22, 14 }, { 15, 51, 23, 14 }}},
                        { 283, { 674, 408, 280, 173 }, 2, {{ 2, 42, 14, 14 }, { 17, 43, 15, 14 }}}},
                4, { 6, 30, 54, 78 }},
        { 18, 89, 901, 3, { 12, 11, 16, 10 },
                {{ 721, { 1725, 1046, 718, 442 }, 2, {{ 5, 150, 120, 15 }, { 1, 151, 121, 15 }}},
                        { 563, { 1346, 816, 560, 345 }, 2, {{ 9, 69, 43, 13 }, { 4, 70, 44, 13 }}},
                        { 397, { 948, 574, 394, 243 }, 2, {{ 17, 50, 22, 14 }, { 1, 51, 23, 14 }}},
                        { 313, { 746, 452, 310, 191 }, 2, {{ 2, 42, 14, 14 }, { 19, 43, 15, 14 }}}},
                4, { 6, 30, 56, 82 }},
        { 19, 93, 991, 3, { 12, 11, 16, 10 },
                {{ 795, { 1903, 1153, 792, 488 }, 2, {{ 3, 141, 113, 14 }, { 4, 142, 114, 14 }}},
                        { 627, { 1500, 909, 624, 384 }, 2, {{ 3, 70, 44, 13 }, { 11, 71, 45, 13 }}},
                        { 445, { 1063, 644, 442, 272 }, 2, {{ 17, 47, 21, 13 }, { 4, 48, 22, 13 }}},
                        { 341, { 813, 493, 338, 208 }, 2, {{ 9, 39, 13, 13 }, { 16, 40, 14, 13 }}}},
                4, { 6, 30, 58, 86 }},
        { 20, 97, 1085, 3, { 12, 11, 16, 10 },
                {{ 861, { 2061, 1249, 858, 528 }, 2, {{ 3, 135, 107, 14 }, { 5, 136, 108, 14 }}},
                        { 669, { 1600, 970, 666, 410 }, 2, {{ 3, 67, 41, 13 }, { 13, 68, 42, 13 }}},
                        { 485, { 1159, 702, 482, 297 }, 2, {{ 15, 54, 24, 15 }, { 5, 55, 25, 15 }}},
                        { 385, { 919, 557, 382, 235 }, 2, {{ 15, 43, 15, 14 }, { 10, 44, 16, 14 }}}},
                4, { 6, 34, 62, 90 }},
        { 21, 101, 1156, 4, { 12, 11, 16, 10 },
                {{ 932, { 2232, 1352, 929, 572 }, 2, {{ 4, 144, 116, 14 }, { 4, 145, 117, 14 }}},
                        { 714, { 1708, 1035, 711, 438 }, 1, {{ 17, 68, 42, 13 }}},
                        { 512, { 1224, 742, 509, 314 }, 2, {{ 17, 50, 22, 14 }, { 6, 51, 23, 14 }}},
                        { 406, { 969, 587, 403, 248 }, 2, {{ 19, 46, 16, 15 }, { 6, 47, 17, 15 }}}},
                5, { 6, 28, 50, 72, 94 }},
        { 22, 105, 1258, 4, { 12, 11, 16, 10 },
                {{ 1006, { 2409, 1460, 1003, 618 }, 2, {{ 2, 139, 111, 14 }, { 7, 140, 112, 14 }}},
                        { 782, { 1872, 1134, 779, 480 }, 1, {{ 17, 74, 46, 14 }}},
                        { 568, { 1358, 823, 565, 348 }, 2, {{ 7, 54, 24, 15 }, { 16, 55, 25, 15 }}},
                        { 442, { 1056, 640, 439, 270 }, 1, {{ 34, 37, 13, 13 }}}},
                5, { 6, 26, 50, 74, 98 }},
        { 23, 109, 1364, 4, { 12, 11, 16, 10 },
                {{ 1094, { 2620, 1588, 1091, 672 }, 2, {{ 4, 151, 121, 15 }, { 5, 152, 122, 15 }}},
                        { 860, { 2059, 1248, 857, 528 }, 2, {{ 4, 75, 47, 14 }, { 14, 76, 48, 14 }}},
                        { 614, { 1468, 890, 611, 376 }, 2, {{ 11, 54, 24, 15 }, { 14, 55, 25, 15 }}},
                        { 464, { 1108, 672, 461, 284 }, 2, {{ 16, 45, 15, 15 }, { 14, 46, 16, 15 }}}},
                5, { 6, 30, 54, 78, 102 }},
        { 24, 113, 1474, 4, { 12, 11, 16, 10 },
                {{ 1174, { 2812, 1704, 1171, 721 }, 2, {{ 6, 147, 117, 15 }, { 4, 148, 118, 15 }}},
                        { 914, { 2188, 1326, 911, 561 }, 2, {{ 6, 73, 45, 14 }, { 14, 74, 46, 14 }}},
                        { 664, { 1588, 963, 661, 407 }, 2, {{ 11, 54, 24, 15 }, { 16, 55, 25, 15 }}},
                        { 514, { 1228, 744, 511, 315 }, 2, {{ 30, 46, 16, 15 }, { 2, 47, 17, 15 }}}},
                5, { 6, 28, 54, 80, 106 }},
        { 25, 117, 1588, 4, { 12, 11, 16, 10 },
                {{ 1276, { 3057, 1853, 1273, 784 }, 2, {{ 8, 132, 106, 13 }, { 4, 133, 107, 13 }}},
                        { 1000, { 2395, 1451, 997, 614 }, 2, {{ 8, 75, 47, 14 }, { 13, 76, 48, 14 }}},
                        { 718, { 1718, 1041, 715, 440 }, 2, {{ 7, 54, 24, 15 }, { 22, 55, 25, 15 }}},
                        { 538, { 1286, 779, 535, 330 }, 2, {{ 22, 45, 15, 15 }, { 13, 46, 16, 15 }}}},
                5, { 6, 32, 58, 84, 110 }},
        { 26, 121, 1706, 4, { 12, 11, 16, 10 },
                {{ 1370, { 3283, 1990, 1367, 842 }, 2, {{ 10, 142, 114, 14 }, { 2, 143, 115, 14 }}},
                        { 1062, { 2544, 1542, 1059, 652 }, 2, {{ 19, 74, 46, 14 }, { 4, 75, 47, 14 }}},
                        { 754, { 1804, 1094, 751, 462 }, 2, {{ 28, 50, 22, 14 }, { 6, 51, 23, 14 }}},
                        { 596, { 1425, 864, 593, 365 }, 2, {{ 33, 46, 16, 15 }, { 4, 47, 17, 15 }}}},
                5, { 6, 30, 58, 86, 114 }},
        { 27, 125, 1828, 4, { 14, 13, 16, 12 },
                {{ 1468, { 3517, 2132, 1465, 902 }, 2, {{ 8, 152, 122, 15 }, { 4, 153, 123, 15 }}},
                        { 1128, { 2701, 1637, 1125, 692 }, 2, {{ 22, 73, 45, 14 }, { 3, 74, 46, 14 }}},
                        { 808, { 1933, 1172, 805, 496 }, 2, {{ 8, 53, 23, 15 }, { 26, 54, 24, 15 }}},
                        { 628, { 1501, 910, 625, 385 }, 2, {{ 12, 45, 15, 15 }, { 28, 46, 16, 15 }}}},
                5, { 6, 34, 62, 90, 118 }},
        { 28, 129, 1921, 3, { 14, 13, 16, 12 },
                {{ 1531, { 3669, 2223, 1528, 940 }, 2, {{ 3, 147, 117, 15 }, { 10, 148, 118, 15 }}},
                        { 1193, { 2857, 1732, 1190, 732 }, 2, {{ 3, 73, 45, 14 }, { 23, 74, 46, 14 }}},
                        { 871, { 2085, 1263, 868, 534 }, 2, {{ 4, 54, 24, 15 }, { 31, 55, 25, 15 }}},
                        { 661, { 1581, 958, 658, 405 }, 2, {{ 11, 45, 15, 15 }, { 31, 46, 16, 15 }}}},
                6, { 6, 26, 50, 74, 98, 122 }},
        { 29, 133, 2051, 3, { 14, 13, 16, 12 },
                {{ 1631, { 3909, 2369, 1628, 1002 }, 2, {{ 7, 146, 116, 15 }, { 7, 147, 117, 15 }}},
                        { 1267, { 3035, 1839, 1264, 778 }, 2, {{ 21, 73, 45, 14 }, { 7, 74, 46, 14 }}},
                        { 911, { 2181, 1322, 908, 559 }, 2, {{ 1, 53, 23, 15 }, { 37, 54, 24, 15 }}},
                        { 701, { 1677, 1016, 698, 430 }, 2, {{ 19, 45, 15, 15 }, { 26, 46, 16, 15 }}}},
                6, { 6, 30, 54, 78, 102, 126 }},
        { 30, 137, 2185, 3, { 14, 13, 16, 12 },
                {{ 1735, { 4158, 2520, 1732, 1066 }, 2, {{ 5, 145, 115, 15 }, { 10, 146, 116, 15 }}},
                        { 1373, { 3289, 1994, 1370, 843 }, 2, {{ 19, 75, 47, 14 }, { 10, 76, 48, 14 }}},
                        { 985, { 2358, 1429, 982, 604 }, 2, {{ 15, 54, 24, 15 }, { 25, 55, 25, 15 }}},
                        { 745, { 1782, 1080, 742, 457 }, 2, {{ 23, 45, 15, 15 }, { 25, 46, 16, 15 }}}},
                6, { 6, 26, 52, 78, 104, 130 }},
        { 31, 141, 2323, 3, { 14, 13, 16, 12 },
                {{ 1843, { 4417, 2677, 1840, 1132 }, 2, {{ 13, 145, 115, 15 }, { 3, 146, 116, 15 }}},
                        { 1455, { 3486, 2113, 1452, 894 }, 2, {{ 2, 74, 46, 14 }, { 29, 75, 47, 14 }}},
                        { 1033, { 2473, 1499, 1030, 634 }, 2, {{ 42, 54, 24, 15 }, { 1, 55, 25, 15 }}},
                        { 793, { 1897, 1150, 790, 486 }, 2, {{ 23, 45, 15, 15 }, { 28, 46, 16, 15 }}}},
                6, { 6, 30, 56, 82, 108, 134 }},
        { 32, 145, 2465, 3, { 14, 13, 16, 12 },
                {{ 1955, { 4686, 2840, 1952, 1201 }, 1, {{ 17, 145, 115, 15 }}},
                        { 1541, { 3693, 2238, 1538, 947 }, 2, {{ 10, 74, 46, 14 }, { 23, 75, 47, 14 }}},
                        { 1115, { 2670, 1618, 1112, 684 }, 2, {{ 10, 54, 24, 15 }, { 35, 55, 25, 15 }}},
                        { 845, { 2022, 1226, 842, 518 }, 2, {{ 19, 45, 15, 15 }, { 35, 46, 16, 15 }}}},
                6, { 6, 34, 60, 86, 112, 138 }},
        { 33, 149, 2611, 3, { 14, 13, 16, 12 },
                {{ 2071, { 4965, 3009, 2068, 1273 }, 2, {{ 17, 145, 115, 15 }, { 1, 146, 116, 15 }}},
                        { 1631, { 3909, 2369, 1628, 1002 }, 2, {{ 14, 74, 46, 14 }, { 21, 75, 47, 14 }}},
                        { 1171, { 2805, 1700, 1168, 719 }, 2, {{ 29, 54, 24, 15 }, { 19, 55, 25, 15 }}},
                        { 901, { 2157, 1307, 898, 553 }, 2, {{ 11, 45, 15, 15 }, { 46, 46, 16, 15 }}}},
                6, { 6, 30, 58, 86, 114, 142 }},
        { 34, 153, 2761, 3, { 14, 13, 16, 12 },
                {{ 2191, { 5253, 3183, 2188, 1347 }, 2, {{ 13, 145, 115, 15 }, { 6, 146, 116, 15 }}},
                        { 1725, { 4134, 2506, 1722, 1060 }, 2, {{ 14, 74, 46, 14 }, { 23, 75, 47, 14 }}},
                        { 1231, { 2949, 1787, 1228, 756 }, 2, {{ 44, 54, 24, 15 }, { 7, 55, 25, 15 }}},
                        { 961, { 2301, 1394, 958, 590 }, 2, {{ 59, 46, 16, 15 }, { 1, 47, 17, 15 }}}},
                6, { 6, 34, 62, 90, 118, 146 }},
        { 35, 157, 2876, 0, { 14, 13, 16, 12 },
                {{ 2306, { 5529, 3351, 2303, 1417 }, 2, {{ 12, 151, 121, 15 }, { 7, 152, 122, 15 }}},
                        { 1812, { 4343, 2632, 1809, 1113 }, 2, {{ 12, 75, 47, 14 }, { 26, 76, 48, 14 }}},
                        { 1286, { 3081, 1867, 1283, 790 }, 2, {{ 39, 54, 24, 15 }, { 14, 55, 25, 15 }}},
                        { 986, { 2361, 1431, 983, 605 }, 2, {{ 22, 45, 15, 15 }, { 41, 46, 16, 15 }}}},
                7, { 6, 30, 54, 78, 102, 126, 150 }},
        { 36, 161, 3034, 0, { 14, 13, 16, 12 },
                {{ 2434, { 5836, 3537, 2431, 1496 }, 2, {{ 6, 151, 121, 15 }, { 14, 152, 122, 15 }}},
                        { 1914, { 4588, 2780, 1911, 1176 }, 2, {{ 6, 75, 47, 14 }, { 34, 76, 48, 14 }}},
                        { 1354, { 3244, 1966, 1351, 832 }, 2, {{ 46, 54, 24, 15 }, { 10, 55, 25, 15 }}},
                        { 1054, { 2524, 1530, 1051, 647 }, 2, {{ 2, 45, 15, 15 }, { 64, 46, 16, 15 }}}},
                7, { 6, 24, 50, 76, 102, 128, 154 }},
        { 37, 165, 3196, 0, { 14, 13, 16, 12 },
                {{ 2566, { 6153, 3729, 2563, 1577 }, 2, {{ 17, 152, 122, 15 }, { 4, 153, 123, 15 }}},
                        { 1992, { 4775, 2894, 1989, 1224 }, 2, {{ 29, 74, 46, 14 }, { 14, 75, 47, 14 }}},
                        { 1426, { 3417, 2071, 1423, 876 }, 2, {{ 49, 54, 24, 15 }, { 10, 55, 25, 15 }}},
                        { 1096, { 2625, 1591, 1093, 673 }, 2, {{ 24, 45, 15, 15 }, { 46, 46, 16, 15 }}}},
                7, { 6, 28, 54, 80, 106, 132, 158 }},
        { 38, 169, 3362, 0, { 14, 13, 16, 12 },
                {{ 2702, { 6479, 3927, 2699, 1661 }, 2, {{ 4, 152, 122, 15 }, { 18, 153, 123, 15 }}},
                        { 2102, { 5039, 3054, 2099, 1292 }, 2, {{ 13, 74, 46, 14 }, { 32, 75, 47, 14 }}},
                        { 1502, { 3599, 2181, 1499, 923 }, 2, {{ 48, 54, 24, 15 }, { 14, 55, 25, 15 }}},
                        { 1142, { 2735, 1658, 1139, 701 }, 2, {{ 42, 45, 15, 15 }, { 32, 46, 16, 15 }}}},
                7, { 6, 32, 58, 84, 110, 136, 162 }},
        { 39, 173, 3532, 0, { 14, 13, 16, 12 },
                {{ 2812, { 6743, 4087, 2809, 1729 }, 2, {{ 20, 147, 117, 15 }, { 4, 148, 118, 15 }}},
                        { 2216, { 5313, 3220, 2213, 1362 }, 2, {{ 40, 75, 47, 14 }, { 7, 76, 48, 14 }}},
                        { 1582, { 3791, 2298, 1579, 972 }, 2, {{ 43, 54, 24, 15 }, { 22, 55, 25, 15 }}},
                        { 1222, { 2927, 1774, 1219, 750 }, 2, {{ 10, 45, 15, 15 }, { 67, 46, 16, 15 }}}},
                7, { 6, 26, 54, 82, 110, 138, 166 }},
        { 40, 177, 3706, 0, { 14, 13, 16, 12 },
                {{ 2956, { 7089, 4296, 2953, 1817 }, 2, {{ 19, 148, 118, 15 }, { 6, 149, 119, 15 }}},
                        { 2334, { 5596, 3391, 2331, 1435 }, 2, {{ 18, 75, 47, 14 }, { 31, 76, 48, 14 }}},
                        { 1666, { 3993, 2420, 1663, 1024 }, 2, {{ 34, 54, 24, 15 }, { 34, 55, 25, 15 }}},
                        { 1276, { 3057, 1852, 1273, 784 }, 2, {{ 20, 45, 15, 15 }, { 61, 46, 16, 15 }}}},
                7, { 6, 30, 58, 86, 114, 142, 170 }}
};
static const unsigned char qr_exp2fac[256] = {
        1,  2,  4,  8, 16, 32, 64,128, 29, 58,116,232,205,135, 19, 38,
        76,152, 45, 90,180,117,234,201,143,  3,  6, 12, 24, 48, 96,192,
        157, 39, 78,156, 37, 74,148, 53,106,212,181,119,238,193,159, 35,
        70,140,  5, 10, 20, 40, 80,160, 93,186,105,210,185,111,222,161,
        95,190, 97,194,153, 47, 94,188,101,202,137, 15, 30, 60,120,240,
        253,231,211,187,107,214,177,127,254,225,223,163, 91,182,113,226,
        217,175, 67,134, 17, 34, 68,136, 13, 26, 52,104,208,189,103,206,
        129, 31, 62,124,248,237,199,147, 59,118,236,197,151, 51,102,204,
        133, 23, 46, 92,184,109,218,169, 79,158, 33, 66,132, 21, 42, 84,
        168, 77,154, 41, 82,164, 85,170, 73,146, 57,114,228,213,183,115,
        230,209,191, 99,198,145, 63,126,252,229,215,179,123,246,241,255,
        227,219,171, 75,150, 49, 98,196,149, 55,110,220,165, 87,174, 65,
        130, 25, 50,100,200,141,  7, 14, 28, 56,112,224,221,167, 83,166,
        81,162, 89,178,121,242,249,239,195,155, 43, 86,172, 69,138,  9,
        18, 36, 72,144, 61,122,244,245,247,243,251,235,203,139, 11, 22,
        44, 88,176,125,250,233,207,131, 27, 54,108,216,173, 71,142,  1
};
static const unsigned char qr_fac2exp[256] = {
        NAV,  0,  1, 25,  2, 50, 26,198,  3,223, 51,238, 27,104,199, 75,
        4,100,224, 14, 52,141,239,129, 28,193,105,248,200,  8, 76,113,
        5,138,101, 47,225, 36, 15, 33, 53,147,142,218,240, 18,130, 69,
        29,181,194,125,106, 39,249,185,201,154,  9,120, 77,228,114,166,
        6,191,139, 98,102,221, 48,253,226,152, 37,179, 16,145, 34,136,
        54,208,148,206,143,150,219,189,241,210, 19, 92,131, 56, 70, 64,
        30, 66,182,163,195, 72,126,110,107, 58, 40, 84,250,133,186, 61,
        202, 94,155,159, 10, 21,121, 43, 78,212,229,172,115,243,167, 87,
        7,112,192,247,140,128, 99, 13,103, 74,222,237, 49,197,254, 24,
        227,165,153,119, 38,184,180,124, 17, 68,146,217, 35, 32,137, 46,
        55, 63,209, 91,149,188,207,205,144,135,151,178,220,252,190, 97,
        242, 86,211,171, 20, 42, 93,158,132, 60, 57, 83, 71,109, 65,162,
        31, 45, 67,216,183,123,164,118,196, 23, 73,236,127, 12,111,246,
        108,161, 59, 82, 41,157, 85,170,251, 96,134,177,187,204, 62, 90,
        203, 89, 95,176,156,169,160, 81, 11,245, 22,235,122,117, 44,215,
        79,174,213,233,230,231,173,232,116,214,244,234,168, 80, 88,175
};
static const unsigned char qr_gftable[QR_RSW_MAX+1][QR_RSW_MAX] = {
        {0},{0},{0},{0},{0},{0},{0},{87,229,146,149,238,102,21},
        {0},{0},{251,67,46,61,118,70,64,94,32,45},
        {0},{0},{74,152,176,100,86,100,106,104,130,218,206,140,78},
        {0},{8,183,61,91,202,37,51,58,58,237,140,124,5,99,105},
        {120,104,107,109,102,161,76,3,91,191,147,169,182,194,225,120},
        {43,139,206,78,43,239,123,206,214,147,24,99,150,39,243,163,136},
        {215,234,158,94,184,97,118,170,79,187,152,148,252,179,5,98,96,153},
        {0},{17,60,79,50,61,163,26,187,202,180,221,225,83,239,156,164,212,212,188,190},
        {0},{210,171,247,242,93,230,14,109,221,53,200,74,8,172,98,80,219,134,160,105,165,231},
        {0},{229,121,135,48,211,117,251,126,159,180,169,152,192,226,228,218,111,0,117,232,87,96,227,21},
        {0},{173,125,158,2,103,182,118,17,145,201,111,28,165,53,161,21,245,142,13,102,48,227,153,145,218,70},
        {0},{168,223,200,104,224,234,108,180,110,190,195,147,205,27,232,201,21,43,245,87,42,195,212,119,242,37,9,123},
        {0},{41,173,145,152,216,31,179,182,50,48,110,86,239,96,222,125,42,173,226,193,224,130,156,37,251,216,238,40,192,180},
        {0},{10,6,106,190,249,167,4,67,209,138,138,32,242,123,89,27,120,185,80,156,38,69,171,60,28,222,80,52,254,185,220,241},
        {0},{111,77,146,94,26,21,108,19,105,94,113,193,86,140,163,125,58,158,229,239,218,103,56,70,114,61,183,129,167,13,98,62,129,51},
        {0},{200,183,98,16,172,31,246,234,60,152,115,0,167,152,113,248,238,107,18,63,218,37,87,210,105,177,120,74,121,196,117,251,113,233,30,120},
        {0},{159,34,38,228,230,59,243,95,49,218,176,164,20,65,45,111,39,81,49,118,113,222,193,250,242,168,217,41,164,247,177,30,238,18,120,153,60,193},
        {0},{59,116,79,161,252,98,128,205,128,161,247,57,163,56,235,106,53,26,187,174,226,104,170,7,175,35,181,114,88,41,47,163,125,134,72,20,232,53,35,15},
        {0},{250,103,221,230,25,18,137,231,0,3,58,242,221,191,110,84,230,8,188,106,96,147,15,131,139,34,101,223,39,101,213,199,237,254,201,123,171,162,194,117,50,96},
        {0},{190,7,61,121,71,246,69,55,168,188,89,243,191,25,72,123,9,145,14,247,1,238,44,78,143,62,224,126,118,114,68,163,52,194,217,147,204,169,37,130,113,102,73,181},
        {0},{112,94,88,112,253,224,202,115,187,99,89,5,54,113,129,44,58,16,135,216,169,211,36,1,4,96,60,241,73,104,234,8,249,245,119,174,52,25,157,224,43,202,223,19,82,15},
        {0},{228,25,196,130,211,146,60,24,251,90,39,102,240,61,178,63,46,123,115,18,221,111,135,160,182,205,107,206,95,150,120,184,91,21,247,156,140,238,191,11,94,227,84,50,163,39,34,108},
        {0},{232,125,157,161,164,9,118,46,209,99,203,193,35,3,209,111,195,242,203,225,46,13,32,160,126,209,130,160,242,215,242,75,77,42,189,32,113,65,124,69,228,114,235,175,124,170,215,232,133,205},
        {0},{116,50,86,186,50,220,251,89,192,46,86,127,124,19,184,233,151,215,22,14,59,145,37,242,203,134,254,89,190,94,59,65,124,113,100,233,235,121,22,76,86,97,39,242,200,220,101,33,239,254,116,51},
        {0},{183,26,201,87,210,221,113,21,46,65,45,50,238,184,249,225,102,58,209,218,109,165,26,95,184,192,52,245,35,254,238,175,172,79,123,25,122,43,120,108,215,80,128,201,235,8,153,59,101,31,198,76,31,156},
        {0},{106,120,107,157,164,216,112,116,2,91,248,163,36,201,202,229,6,144,254,155,135,208,170,209,12,139,127,142,182,249,177,174,190,28,10,85,239,184,101,124,152,206,96,23,163,61,27,196,247,151,154,202,207,20,61,10},
        {0},{82,116,26,247,66,27,62,107,252,182,200,185,235,55,251,242,210,144,154,237,176,141,192,248,152,249,206,85,253,142,65,165,125,23,24,30,122,240,214,6,129,218,29,145,127,134,206,245,117,29,41,63,159,142,233,125,148,123},
        {0},{107,140,26,12,9,141,243,197,226,197,219,45,211,101,219,120,28,181,127,6,100,247,2,205,198,57,115,219,101,109,160,82,37,38,238,49,160,209,121,86,11,124,30,181,84,25,194,87,65,102,190,220,70,27,209,16,89,7,33,240},
        {0},{65,202,113,98,71,223,248,118,214,94,0,122,37,23,2,228,58,121,7,105,135,78,243,118,70,76,223,89,72,50,70,111,194,17,212,126,181,35,221,117,235,11,229,149,147,123,213,40,115,6,200,100,26,246,182,218,127,215,36,186,110,106},
        {0},{45,51,175,9,7,158,159,49,68,119,92,123,177,204,187,254,200,78,141,149,119,26,127,53,160,93,199,212,29,24,145,156,208,150,218,209,4,216,91,47,184,146,47,140,195,195,125,242,238,63,99,108,140,230,242,31,204,11,178,243,217,156,213,231},
        {0},{5,118,222,180,136,136,162,51,46,117,13,215,81,17,139,247,197,171,95,173,65,137,178,68,111,95,101,41,72,214,169,197,95,7,44,154,77,111,236,40,121,143,63,87,80,253,240,126,217,77,34,232,106,50,168,82,76,146,67,106,171,25,132,93,45,105},
        {0},{247,159,223,33,224,93,77,70,90,160,32,254,43,150,84,101,190,205,133,52,60,202,165,220,203,151,93,84,15,84,253,173,160,89,227,52,199,97,95,231,52,177,41,125,137,241,166,225,118,2,54,32,82,215,175,198,43,238,235,27,101,184,127,3,5,8,163,238}
};
#define F0 QR_MM_FUNC
#define F1 (QR_MM_FUNC | QR_MM_BLACK)
static const qr_byte_t qr_finderpattern[QR_DIM_FINDER][QR_DIM_FINDER] = {
        { F1, F1, F1, F1, F1, F1, F1 },
        { F1, F0, F0, F0, F0, F0, F1 },
        { F1, F0, F1, F1, F1, F0, F1 },
        { F1, F0, F1, F1, F1, F0, F1 },
        { F1, F0, F1, F1, F1, F0, F1 },
        { F1, F0, F0, F0, F0, F0, F1 },
        { F1, F1, F1, F1, F1, F1, F1 }
};
static const qr_byte_t qr_alignpattern[QR_DIM_ALIGN][QR_DIM_ALIGN] = {
        { F1, F1, F1, F1, F1 },
        { F1, F0, F0, F0, F1 },
        { F1, F0, F1, F0, F1 },
        { F1, F0, F0, F0, F1 },
        { F1, F1, F1, F1, F1 }
};
#undef F0
#undef F1
static const int qr_modeid[QR_EM_COUNT] = { 0x01, 0x02, 0x04, 0x08 };
const char *qr_eclname[QR_ECL_COUNT] = { "L", "M", "Q", "H" };
static const signed char qr_alnumtable[256] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        36, -1, -1, -1, 37, 38, -1, -1, -1, -1, 39, 40, -1, 41, 42, 43,
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 44, -1, -1, -1, -1, -1,
        -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
static const qr_coord_t qr_fmtinfopos[2][QR_FIN_MAX] = {
        {{ 0, 8 }, { 1, 8 }, { 2, 8 }, { 3, 8 },
                { 4, 8 }, { 5, 8 }, { 7, 8 }, { 8, 8 },
                { -7, 8 }, { -6, 8 }, { -5, 8 }, { -4, 8 },
                { -3, 8 }, { -2, 8 }, { -1, 8 }},
        {{ 8, -1 }, { 8, -2 }, { 8, -3 }, { 8, -4 },
                { 8, -5 }, { 8, -6 }, { 8, -7 }, { 8, -8 },
                { 8, 7 }, { 8, 5 }, { 8, 4 }, { 8, 3 },
                { 8, 2 }, { 8, 1 }, { 8, 0 }}
};
static const qr_coord_t qr_fmtblackpos = { -8, 8 };
static const qr_coord_t qr_verinfopos[2][QR_VIN_MAX] = {
        {{ -11, 0 }, { -10, 0 }, { -9, 0 },
                { -11, 1 }, { -10, 1 }, { -9, 1 },
                { -11, 2 }, { -10, 2 }, { -9, 2 },
                { -11, 3 }, { -10, 3 }, { -9, 3 },
                { -11, 4 }, { -10, 4 }, { -9, 4 },
                { -11, 5 }, { -10, 5 }, { -9, 5 }},
        {{ 0, -11 }, { 0, -10 }, { 0, -9 },
                { 1, -11 }, { 1, -10 }, { 1, -9 },
                { 2, -11 }, { 2, -10 }, { 2, -9 },
                { 3, -11 }, { 3, -10 }, { 3, -9 },
                { 4, -11 }, { 4, -10 }, { 4, -9 },
                { 5, -11 }, { 5, -10 }, { 5, -9 }}
};
static const long qr_verinfo[QR_VER_MAX+1] = {
        -1L,      -1L,      -1L,      -1L,     -1L,       -1L,
        -1L, 0x07c94L, 0x085bcL, 0x09a99L, 0x0a4d3L, 0x0bbf6L,
        0x0c762L, 0x0d847L, 0x0e60dL, 0x0f928L, 0x10b78L, 0x1145dL,
        0x12a17L, 0x13532L, 0x149a6L, 0x15683L, 0x168c9L, 0x177ecL,
        0x18ec4L, 0x191e1L, 0x1afabL, 0x1b08eL, 0x1cc1aL, 0x1d33fL,
        0x1ed75L, 0x1f250L, 0x209d5L, 0x216f0L, 0x228baL, 0x2379fL,
        0x24b0bL, 0x2542eL, 0x26a64L, 0x27541L, 0x28c69L
};
typedef int (*qr_funcs)(QRCode *);
static void qrAddDataBits(QRCode *qr, int n, int word);
static int qrInitDataWord(QRCode *qr);
static int qrEncodeDataWord(QRCode *qr, const qr_byte_t *source, int size, int mode);
static int qrFinalizeDataWord(QRCode *qr);
static int qrComputeECWord(QRCode *qr);
static int qrMakeCodeWord(QRCode *qr);
static int qrFillFunctionPattern(QRCode *qr);
static int qrFillCodeWord(QRCode *qr);
static void qrInitPosition(QRCode *qr);
static void qrNextPosition(QRCode *qr);
static int qrSelectMaskPattern(QRCode *qr);
static int qrApplyMaskPattern(QRCode *qr);
static int qrApplyMaskPattern2(QRCode *qr, int type);
static long qrEvaluateMaskPattern(QRCode *qr);
static int qrFillFormatInfo(QRCode *qr);
#endif