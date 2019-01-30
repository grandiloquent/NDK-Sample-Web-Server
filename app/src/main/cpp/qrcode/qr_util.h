
#ifndef _QR_UTIL_H_
#define _QR_UTIL_H_
#include "qr.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <stdarg.h>
#define qrIsBlack(qr, i, j) (((qr)->symbol[(i)][(j)] & QR_MM_BLACK) != 0)
#define qrFree(ptr) { if ((ptr) != NULL) { free(ptr); (ptr) = NULL; } }
QR_API extern const char *(*qrGetCurrentFunctionName)(void);
#if defined(__FUNCTION__)
#define _QR_FUNCTION ((qrGetCurrentFunctionName) ? qrGetCurrentFunctionName() : __FUNCTION__)
#elif defined(__func__)
#define _QR_FUNCTION ((qrGetCurrentFunctionName) ? qrGetCurrentFunctionName() : __func__)
#else
#define _QR_FUNCTION ((qrGetCurrentFunctionName) ? qrGetCurrentFunctionName() : "?")
#endif
#define QR_EXT_MAX_LEN 4
extern QR_API const qr_vertable_t qr_vertable[];
extern QR_API const char *qr_eclname[];
QR_API const char *qrVersion(void);
QR_API const char *qrMimeType(int format);
QR_API const char *qrExtension(int format);
QR_API const char *qrStrError(int errcode);
QR_API void qrSetErrorInfo(QRCode *qr, int errnum, const char *param);
QR_API void qrSetErrorInfo2(QRCode *qr, int errnum, const char *param);
QR_API void qrSetErrorInfo3(QRCode *qr, int errnum, const char *fmt, ...);
QR_API int qrGetEncodedLength(QRCode *qr, int size);
QR_API int qrGetEncodedLength2(QRCode *qr, int size, int mode);
QR_API int qrGetEncodableLength(QRCode *qr, int size);
QR_API int qrGetEncodableLength2(QRCode *qr, int size, int mode);
QR_API int qrRemainedDataBits(QRCode *qr);
QR_API int qrDetectDataType(const qr_byte_t *source, int size);
QR_API int qrStrPosNotNumeric(const qr_byte_t *source, int size);
QR_API int qrStrPosNotAlnum(const qr_byte_t *source, int size);
QR_API int qrStrPosNotKanji(const qr_byte_t *source, int size);
QR_API int qrStrPosNot8bit(const qr_byte_t *source, int size);
#ifdef __cplusplus
}
#endif
#endif