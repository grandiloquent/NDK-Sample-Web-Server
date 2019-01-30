#ifndef _QR_H_
#define _QR_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <errno.h>
#include <stdio.h>
#if defined(WIN32) && !defined(QR_STATIC_BUILD)
#ifdef QR_DLL_BUILD
#define QR_API __declspec(dllexport)
#else
#define QR_API __declspec(dllimport)
#endif
#else
#define QR_API
#endif
#define LIBQR_VERSION "0.3.1"
typedef enum {

    QR_ERR_NONE             = 0,
    QR_ERR_USAGE            = 0x68,
    QR_ERR_NOT_IMPL         = 0x69,
    QR_ERR_SEE_ERRNO        = 0x6e,
    QR_ERR_FOPEN            = 0x6f,
    QR_ERR_FREAD            = 0x72,
    QR_ERR_STATE            = 0x73,
    QR_ERR_UNKNOWN          = 0x75,
    QR_ERR_FWRITE           = 0x77,
    QR_ERR_MEMORY_EXHAUSTED = 0x78,

    QR_ERR_INVALID_ARG     = 0x01,
    QR_ERR_INVALID_VERSION = 0x02,
    QR_ERR_INVALID_MODE    = 0x03,
    QR_ERR_INVALID_ECL     = 0x04,
    QR_ERR_INVALID_MPT     = 0x05,
    QR_ERR_INVALID_MAG     = 0x06,
    QR_ERR_INVALID_SEP     = 0x07,
    QR_ERR_INVALID_SIZE    = 0x08,
    QR_ERR_INVALID_FMT     = 0x09,
    QR_ERR_INVALID_OUT     = 0x0a,
    QR_ERR_INVALID_MAXNUM  = 0x0b,
    QR_ERR_UNSUPPORTED_FMT = 0x0c,
    QR_ERR_EMPTY_PARAM     = 0x0f,

    QR_ERR_EMPTY_SRC   = 0x10,
    QR_ERR_LARGE_SRC   = 0x11,
    QR_ERR_NOT_NUMERIC = 0x12,
    QR_ERR_NOT_ALNUM   = 0x13,
    QR_ERR_NOT_KANJI   = 0x14,

    QR_ERR_IMAGE_TOO_LARGE  = 0x30,
    QR_ERR_WIDTH_TOO_LARGE  = 0x31,
    QR_ERR_HEIGHT_TOO_LARGE = 0x32,
    QR_ERR_IMAGECREATE      = 0x33,
    QR_ERR_IMAGEFORMAT      = 0x34,
    QR_ERR_IMAGEFRAME       = 0x35,

    QR_ERR_DEFLATE = 0x40
} qr_err_t;
#define QR_STATE_BEGIN  0
#define QR_STATE_SET    1
#define QR_STATE_FINAL  2
typedef enum {
    QR_EM_AUTO    = -1,
    QR_EM_NUMERIC =  0,
    QR_EM_ALNUM   =  1,
    QR_EM_8BIT    =  2,
    QR_EM_KANJI   =  3
} qr_em_t;
#define QR_EM_COUNT 4
typedef enum {
    QR_ECL_L = 0,
    QR_ECL_M = 1,
    QR_ECL_Q = 2,
    QR_ECL_H = 3
} qr_ecl_t;
#define QR_ECL_COUNT 4
typedef enum {
    QR_FMT_PNG   =  0,
    QR_FMT_BMP   =  1,
    QR_FMT_TIFF  =  2,
    QR_FMT_PBM   =  3,
    QR_FMT_SVG   =  4,
    QR_FMT_JSON  =  5,
    QR_FMT_DIGIT =  6,
    QR_FMT_ASCII =  7,
    QR_FMT_UNAVAILABLE = -1
} qr_format_t;
#define QR_FMT_COUNT 8
#define QR_MM_DATA      0x01
#define QR_MM_BLACK     0x02
#define QR_MM_FUNC      0x04
#define QR_DIM_SEP      4
#define QR_DIM_FINDER   7
#define QR_DIM_ALIGN    5
#define QR_DIM_TIMING   6
#define QR_SRC_MAX  7089
#define QR_DIM_MAX   177
#define QR_VER_MAX    40
#define QR_DWD_MAX  2956
#define QR_ECW_MAX  2430
#define QR_CWD_MAX  3706
#define QR_RSD_MAX   123
#define QR_RSW_MAX    68
#define QR_RSB_MAX     2
#define QR_MPT_MAX     8
#define QR_APL_MAX     7
#define QR_FIN_MAX    15
#define QR_VIN_MAX    18
#define QR_MAG_MAX    16
#define QR_SEP_MAX    16
#define QR_ERR_MAX  1024
#define QR_STA_MAX    16
#define QR_STA_LEN    20
#define NAV            0
#define PADWORD1    0xec
#define PADWORD2    0x11
#define VERPOINT1      9
#define VERPOINT2     26
typedef unsigned char qr_byte_t;
typedef struct qr_rsblock_t {
    int rsbnum;
    int totalwords;
    int datawords;
    int ecnum;
} qr_rsblock_t;
typedef struct qr_eclevel_t {
    int datawords;
    int capacity[QR_EM_COUNT];
    int nrsb;
    qr_rsblock_t rsb[QR_RSB_MAX];
} qr_eclevel_t;
typedef struct qr_vertable_t {
    int          version;
    int          dimension;
    int          totalwords;
    int          remainedbits;
    int          nlen[QR_EM_COUNT];
    qr_eclevel_t ecl[QR_ECL_COUNT];
    int          aplnum;
    int          aploc[QR_APL_MAX];
} qr_vertable_t;
typedef struct qr_coord_t { int ypos, xpos; } qr_coord_t;
typedef struct qr_param_t {
    int version;
    int mode;
    int eclevel;
    int masktype;
} qr_param_t;
typedef struct qrcode_t {
    qr_byte_t *dataword;
    qr_byte_t *ecword;
    qr_byte_t *codeword;
    qr_byte_t *_symbol;
    qr_byte_t **symbol;
    qr_byte_t *source;
    size_t srcmax;
    size_t srclen;
    int enclen;
    int delta1, delta2;
    int dwpos;
    int dwbit;
    int xpos, ypos;
    int xdir, ydir;
    int state;
    int errcode;
    char errinfo[QR_ERR_MAX];
    qr_param_t param;
} QRCode;
typedef struct qrcode_sa_t {
    QRCode *qrs[QR_STA_MAX];
    QRCode *cur;
    int num;
    int max;
    int parity;
    int state;
    qr_param_t param;
} QRStructured;
typedef qr_byte_t *(*QRConverter)(QRCode *, int, int, int *);
typedef qr_byte_t *(*QRsConverter)(QRStructured *, int, int, int, int *);
QR_API QRCode *qrInit(int version, int mode, int eclevel, int masktype, int *errcode);
QR_API void qrDestroy(QRCode *qr);
QR_API int qrGetErrorCode(QRCode *qr);
QR_API char *qrGetErrorInfo(QRCode *qr);
QR_API int qrAddData(QRCode *qr, const qr_byte_t *source, int size);
QR_API int qrAddData2(QRCode *qr, const qr_byte_t *source, int size, int mode);
QR_API int qrFinalize(QRCode *qr);
QR_API int qrIsFinalized(const QRCode *qr);
QR_API int qrHasData(const QRCode *qr);
QR_API QRCode *qrClone(const QRCode *qr, int *errcode);
QR_API QRStructured *qrsInit(int version, int mode, int eclevel, int masktype, int maxnum, int *errcode);
QR_API void qrsDestroy(QRStructured *st);
QR_API int qrsGetErrorCode(QRStructured *st);
QR_API char *qrsGetErrorInfo(QRStructured *st);
QR_API int qrsAddData(QRStructured *st, const qr_byte_t *source, int size);
QR_API int qrsAddData2(QRStructured *st, const qr_byte_t *source, int size, int mode);
QR_API int qrsFinalize(QRStructured *st);
QR_API int qrsIsFinalized(const QRStructured *st);
QR_API int qrsHasData(const QRStructured *st);
QR_API QRStructured *qrsClone(const QRStructured *st, int *errcode);
QR_API int qrOutputSymbol(QRCode *qr, FILE *fp, int fmt, int sep, int mag);
QR_API int qrOutputSymbol2(QRCode *qr, const char *pathname, int fmt, int sep, int mag);
QR_API qr_byte_t *qrGetSymbol(QRCode *qr, int fmt, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToDigit(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToASCII(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToJSON(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToPBM(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToBMP(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToSVG(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToTIFF(QRCode *qr, int sep, int mag, int *size);
QR_API qr_byte_t *qrSymbolToPNG(QRCode *qr, int sep, int mag, int *size);
QR_API int qrsOutputSymbols(QRStructured *st, FILE *fp, int fmt, int sep, int mag, int order);
QR_API int qrsOutputSymbols2(QRStructured *st, const char *pathname, int fmt, int sep, int mag, int order);
QR_API qr_byte_t *qrsGetSymbols(QRStructured *st, int fmt, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToDigit(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToASCII(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToJSON(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToPBM(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToBMP(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToSVG(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToTIFF(QRStructured *st, int sep, int mag, int order, int *size);
QR_API qr_byte_t *qrsSymbolsToPNG(QRStructured *st, int sep, int mag, int order, int *size);
#ifdef __cplusplus
}
#endif
#endif