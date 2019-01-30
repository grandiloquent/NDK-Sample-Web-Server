
#ifndef _QRCMD_H_
#define _QRCMD_H_
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "qr.h"
#include "qr_util.h"
#define QRCMD_PROG_VERSION "0.2.0"
#ifdef QRCMD_STRUCTURED_APPEND
#define QRCMD_PTR_TYPE QRStructured
#define QRCMD_EXTRA_PARAM_A int order = 0; int extra = 0;
#define QRCMD_EXTRA_PARAM_B , order
#define QRCMD_EXTRA_PARAM_C &order, &extra,
#define QRCMD_EXTRA_PARAM_D int *order, int *extra,
#define QRCMD_MAX_NUM_A int maxnum = QR_STA_MAX;
#define QRCMD_MAX_NUM_B maxnum,
#define QRCMD_DEFAULT_VERSION 1
#define qrCmdInit qrsInit
#define qrCmdDestroy qrsDestroy
#define qrCmdFinalize qrsFinalize
#define qrCmdGetErrorInfo qrsGetErrorInfo
#define qrCmdAddData qrsAddData
#define qrCmdAddData2 qrsAddData2
#define qrCmdOutputSymbol qrsOutputSymbols
#define qrCmdOutputSymbol2 qrsOutputSymbols2
#ifndef QRCMD_PROG_NAME
#define QRCMD_PROG_NAME "qrs"
#endif
#ifndef QR_SA_SRC_MAX
#define QR_SA_SRC_MAX 112233
#endif
#define QRCMD_SRC_MAX QR_SA_SRC_MAX
#else
#define QRCMD_PTR_TYPE QRCode
#define QRCMD_EXTRA_PARAM_A
#define QRCMD_EXTRA_PARAM_B
#define QRCMD_EXTRA_PARAM_C
#define QRCMD_EXTRA_PARAM_D
#define QRCMD_MAX_NUM_A
#define QRCMD_MAX_NUM_B
#define QRCMD_DEFAULT_VERSION -1
#define qrCmdInit qrInit
#define qrCmdDestroy qrDestroy
#define qrCmdFinalize qrFinalize
#define qrCmdGetErrorInfo qrGetErrorInfo
#define qrCmdAddData qrAddData
#define qrCmdAddData2 qrAddData2
#define qrCmdOutputSymbol qrOutputSymbol
#define qrCmdOutputSymbol2 qrOutputSymbol2
#ifndef QRCMD_PROG_NAME
#define QRCMD_PROG_NAME "qr"
#endif
#define QRCMD_SRC_MAX QR_SRC_MAX
#endif
QRCMD_PTR_TYPE *
qrGetParameter(int argc, char **argv,
               int *fmt, int *sep, int *mag, QRCMD_EXTRA_PARAM_D char **output);
void
qrShowHelp(void);
#endif