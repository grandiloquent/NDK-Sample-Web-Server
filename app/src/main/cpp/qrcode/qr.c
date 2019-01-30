

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "qr.h"
#include "qr_util.h"
#include "qr_private.h"
#include "qr_dwtable.h"

#define qrIsData(qr, i, j)  (((qr)->symbol[i][j] & QR_MM_DATA) != 0)
#define qrIsFunc(qr, i, j)  (((qr)->symbol[i][j] & QR_MM_FUNC) != 0)

QR_API const char *(*qrGetCurrentFunctionName)(void) = NULL;


QR_API const char *
qrVersion(void)
{
	return LIBQR_VERSION;
}


QR_API QRCode *
qrInit(int version, int mode, int eclevel, int masktype, int *errcode)
{
	QRCode *qr = NULL;

	
	qr = (QRCode *)calloc(1, sizeof(QRCode));
	if (qr == NULL) {
		*errcode = QR_ERR_MEMORY_EXHAUSTED;
		return NULL;
	}
	qr->dataword = (qr_byte_t *)calloc(1, QR_DWD_MAX);
	qr->ecword   = (qr_byte_t *)calloc(1, QR_ECW_MAX);
	qr->codeword = (qr_byte_t *)calloc(1, QR_CWD_MAX);
	if (qr->dataword == NULL || qr->ecword == NULL || qr->codeword == NULL) {
		*errcode = QR_ERR_MEMORY_EXHAUSTED;
		qrDestroy(qr);
		return NULL;
	}

	
	qr->_symbol = NULL;
	qr->symbol = NULL;
	qr->source = NULL;
	qr->srcmax = 0;
	qr->srclen = 0;
	qr->enclen = 0;
	qr->delta1 = 0;
	qr->delta2 = 0;
	qr->errcode = QR_ERR_NONE;
	qr->state = QR_STATE_BEGIN;

	
	if (version == -1 || (version >= 1 && version <= QR_VER_MAX)) {
		qr->param.version = version;
	} else {
		*errcode = QR_ERR_INVALID_VERSION;
		qrDestroy(qr);
		return NULL;
	}

	
	if (mode == QR_EM_AUTO || (mode >= QR_EM_NUMERIC && mode < QR_EM_COUNT)) {
		qr->param.mode = mode;
	} else {
		*errcode = QR_ERR_INVALID_MODE;
		qrDestroy(qr);
		return NULL;
	}

	
	if (eclevel >= QR_ECL_L && eclevel < QR_EM_COUNT) {
		qr->param.eclevel = eclevel;
	} else {
		*errcode = QR_ERR_INVALID_ECL;
		qrDestroy(qr);
		return NULL;
	}

	
	if (masktype == -1 || (masktype >= 0 && masktype < QR_MPT_MAX)) {
		qr->param.masktype = masktype;
	} else {
		*errcode = QR_ERR_INVALID_MPT;
		qrDestroy(qr);
		return NULL;
	}

	return qr;
}


QR_API QRStructured *
qrsInit(int version, int mode, int eclevel, int masktype, int maxnum, int *errcode)
{
	QRStructured *st = NULL;

	
	st = (QRStructured *)calloc(1, sizeof(QRStructured));
	if (st == NULL) {
		*errcode = QR_ERR_MEMORY_EXHAUSTED;
		return NULL;
	}

	
	st->parity = 0;
	st->state = QR_STATE_BEGIN;

	
	if (maxnum >= 2 && masktype <= QR_STA_MAX) {
		st->max = maxnum;
	} else {
		*errcode = QR_ERR_INVALID_MAXNUM;
		qrsDestroy(st);
		return NULL;
	}

	
	if (version >= 1 && version <= QR_VER_MAX) {
		st->param.version = version;
	} else {
		*errcode = QR_ERR_INVALID_VERSION;
		qrsDestroy(st);
		return NULL;
	}

	
	if (mode == QR_EM_AUTO || (mode >= QR_EM_NUMERIC && mode < QR_EM_COUNT)) {
		st->param.mode = mode;
	} else {
		*errcode = QR_ERR_INVALID_MODE;
		qrsDestroy(st);
		return NULL;
	}

	
	if (eclevel >= QR_ECL_L && eclevel < QR_EM_COUNT) {
		st->param.eclevel = eclevel;
	} else {
		*errcode = QR_ERR_INVALID_ECL;
		qrsDestroy(st);
		return NULL;
	}

	
	if (masktype == -1 || (masktype >= 0 && masktype < QR_MPT_MAX)) {
		st->param.masktype = masktype;
	} else {
		*errcode = QR_ERR_INVALID_MPT;
		qrsDestroy(st);
		return NULL;
	}

	
	st->qrs[0] = qrInit(st->param.version, st->param.mode,
			st->param.eclevel, st->param.masktype, errcode);
	if (st->qrs[0] == NULL) {
		qrsDestroy(st);
		return NULL;
	}
	st->cur = st->qrs[0];
	st->num = 1;

	return st;
}


QR_API QRCode *
qrClone(const QRCode *qr, int *errcode)
{
	QRCode *cp = NULL;

	
	cp = (QRCode *)malloc(sizeof(QRCode));
	if (cp == NULL) {
		*errcode = QR_ERR_MEMORY_EXHAUSTED;
		return NULL;
	}
	memcpy(cp, qr, sizeof(QRCode));

	
	cp->dataword = NULL;
	cp->ecword = NULL;
	cp->codeword = NULL;
	cp->_symbol = NULL;
	cp->symbol = NULL;
	cp->source = NULL;

	
	if (cp->state == QR_STATE_FINAL) {
		int i, dim;

		dim = qr_vertable[cp->param.version].dimension;

		cp->_symbol = (qr_byte_t *)calloc((size_t)dim, (size_t)dim);
		if (cp->_symbol == NULL) {
			*errcode = QR_ERR_MEMORY_EXHAUSTED;
			qrDestroy(cp);
			return NULL;
		}
		memcpy(cp->_symbol, qr->_symbol, (size_t)(dim * dim));

		cp->symbol = (qr_byte_t **)malloc(sizeof(qr_byte_t *) * (size_t)dim);
		if (cp->symbol == NULL) {
			*errcode = QR_ERR_MEMORY_EXHAUSTED;
			qrDestroy(cp);
			return NULL;
		}
		for (i = 0; i < dim; i++) {
			cp->symbol[i] = cp->_symbol + dim * i;
		}
	} else {
		cp->dataword = (qr_byte_t *)malloc(QR_DWD_MAX);
		cp->ecword   = (qr_byte_t *)malloc(QR_ECW_MAX);
		cp->codeword = (qr_byte_t *)malloc(QR_CWD_MAX);
		if (cp->dataword == NULL || cp->ecword == NULL || cp->codeword == NULL) {
			*errcode = QR_ERR_MEMORY_EXHAUSTED;
			qrDestroy(cp);
			return NULL;
		}
		memcpy(cp->dataword, qr->dataword, QR_DWD_MAX);
		memcpy(cp->ecword  , qr->ecword  , QR_ECW_MAX);
		memcpy(cp->codeword, qr->codeword, QR_CWD_MAX);
	}

	
	if (cp->srcmax > 0 && qr->source != NULL) {
		cp->source = (qr_byte_t *)malloc(cp->srcmax);
		if (cp->source == NULL) {
			*errcode = QR_ERR_MEMORY_EXHAUSTED;
			qrDestroy(cp);
			return NULL;
		}
		memcpy(cp->source, qr->source, cp->srclen);
	}

	return cp;
}


QR_API QRStructured *
qrsClone(const QRStructured *st, int *errcode)
{
	QRStructured *cps = NULL;
	int i = 0;

	
	cps = (QRStructured *)malloc(sizeof(QRStructured));
	if (cps == NULL) {
		*errcode = QR_ERR_MEMORY_EXHAUSTED;
		return NULL;
	}
	memcpy(cps, st, sizeof(QRStructured));

	
	while (i < cps->num) {
		QRCode *cp = qrClone(st->qrs[i], errcode);
		if (cp == NULL) {
			while (i > 0) {
				qrDestroy(cps->qrs[--i]);
				free(cps);
			}
			return NULL;
		}
		cps->qrs[i++] = cp;
	}
	while (i < QR_STA_MAX) {
		cps->qrs[i++] = NULL;
	}
	cps->cur = cps->qrs[0] + (st->cur - st->qrs[0]);

	return cps;
}


QR_API void
qrDestroy(QRCode *qr)
{
	if (qr == NULL) {
		return;
	}
	qrFree(qr->source);
	qrFree(qr->dataword);
	qrFree(qr->ecword);
	qrFree(qr->codeword);
	qrFree(qr->symbol);
	qrFree(qr->_symbol);
	free(qr);
}


QR_API void
qrsDestroy(QRStructured *st)
{
	int i;
	if (st == NULL) {
		return;
	}
	for (i = 0; i < st->num; i++) {
		qrDestroy(st->qrs[i]);
	}
	free(st);
}


QR_API const char *
qrMimeType(int format)
{
	switch (format) {
		case QR_FMT_PNG:    return "image/png";
		case QR_FMT_BMP:    return "image/bmp";
		case QR_FMT_TIFF:   return "image/tiff";
		case QR_FMT_PBM:    return "image/x-portable-bitmap";
		case QR_FMT_SVG:    return "image/svg+xml";
		case QR_FMT_JSON:   return "application/json";
		case QR_FMT_DIGIT:  return "text/plain";
		case QR_FMT_ASCII:  return "text/plain";
		default: return NULL;
	}
}


QR_API const char *
qrExtension(int format)
{
	switch (format) {
		case QR_FMT_PNG:    return "png";
		case QR_FMT_BMP:    return "bmp";
		case QR_FMT_TIFF:   return "tiff";
		case QR_FMT_PBM:    return "pbm";
		case QR_FMT_SVG:    return "svg";
		case QR_FMT_JSON:   return "json";
		case QR_FMT_DIGIT:  return "txt";
		case QR_FMT_ASCII:  return "txt";
		default: return NULL;
	}
}


QR_API int
qrGetErrorCode(QRCode *qr)
{
	return qr->errcode;
}


QR_API char *
qrGetErrorInfo(QRCode *qr)
{
	return &(qr->errinfo[0]);
}


QR_API int
qrsGetErrorCode(QRStructured *st)
{
	return st->cur->errcode;
}


QR_API char *
qrsGetErrorInfo(QRStructured *st)
{
	return &(st->cur->errinfo[0]);
}


QR_API const char *
qrStrError(int errcode)
{
	switch (errcode) {
	
	  case QR_ERR_NONE:
	  case QR_ERR_USAGE:
		return "";

	  case QR_ERR_SEE_ERRNO:
		return "For more information, check for errno";

	  case QR_ERR_NOT_IMPL:
		return "Not yet implemented";

	  case QR_ERR_STATE:
		return "Not allowed in the current state";

	  case QR_ERR_FOPEN:
		return "Failed to open file";

	  case QR_ERR_FREAD:
		return "Failed to read data";

	  case QR_ERR_FWRITE:
		return "Failed to write data";

	  case QR_ERR_MEMORY_EXHAUSTED:
		return "Memory exhausted";

	
	  case QR_ERR_INVALID_ARG:
		return "Invalid argument";

	  case QR_ERR_INVALID_VERSION:
		return "Invalid version number";

	  case QR_ERR_INVALID_MODE:
		return "Invalid encoding mode";

	  case QR_ERR_INVALID_ECL:
		return "Invalid error correction level";

	  case QR_ERR_INVALID_MPT:
		return "Invalid mask pattern type";

	  case QR_ERR_INVALID_MAG:
		return "Invalid pixel magnifying ratio";

	  case QR_ERR_INVALID_SEP:
		return "Invalid separator width";

	  case QR_ERR_INVALID_SIZE:
		return "Invalid output size";

	  case QR_ERR_INVALID_FMT:
		return "Invalid output format";

	  case QR_ERR_INVALID_OUT:
		return "Invalid output pathname";

	  case QR_ERR_INVALID_MAXNUM:
		return "Invalid maximum symbol number";

	  case QR_ERR_UNSUPPORTED_FMT:
		return "Unsupported output format";

	  case QR_ERR_EMPTY_PARAM:
		return "Parameter required";

	
	  case QR_ERR_EMPTY_SRC:
		return "Input data is empty";

	  case QR_ERR_LARGE_SRC:
		return "Input data too large";

	  case QR_ERR_NOT_NUMERIC:
		return "Non decimal characters found";

	  case QR_ERR_NOT_ALNUM:
		return "Non alphanumeric characters found";

	  case QR_ERR_NOT_KANJI:
		return "Non JIS X 0208 kanji sequence found";

	
	  case QR_ERR_IMAGE_TOO_LARGE:
		return "Output image size too large";

	  case QR_ERR_WIDTH_TOO_LARGE:
		return "Output image width too large";

	  case QR_ERR_HEIGHT_TOO_LARGE:
		return "Output image height too large";

	  case QR_ERR_IMAGECREATE:
		return "Failed to create image";

	  case QR_ERR_IMAGEFORMAT:
		return "Failed to convert image";

	  case QR_ERR_IMAGEFRAME:
		return "Failed to create frame";

	
	  case QR_ERR_DEFLATE:
		return "Failed to deflate";

	
	  case QR_ERR_UNKNOWN:
	  default:
		return "Unknown error";
	}
}


QR_API void
qrSetErrorInfo(QRCode *qr, int errnum, const char *param)
{
	qr->errcode = errnum;
	if (param != NULL) {
		snprintf(&(qr->errinfo[0]), QR_ERR_MAX, "%s: %s", param, qrStrError(errnum));
	} else {
		snprintf(&(qr->errinfo[0]), QR_ERR_MAX, "%s", qrStrError(errnum));
	}
}


QR_API void
qrSetErrorInfo2(QRCode *qr, int errnum, const char *param)
{
	char *info;
	int size = 0;
	info = &(qr->errinfo[0]);
	qr->errcode = QR_ERR_SEE_ERRNO;
	if (param != NULL) {
		size = snprintf(info, QR_ERR_MAX, "%s: ", param);
		if (size < 0 || size >= QR_ERR_MAX) {
			return;
		}
		info += size;
	}
#ifdef WIN32
	snprintf(info, (size_t)(QR_ERR_MAX - size), "%s", strerror(errnum));
#else
	strerror_r(errnum, info, (size_t)(QR_ERR_MAX - size));
#endif
}


QR_API void
qrSetErrorInfo3(QRCode *qr, int errnum, const char *fmt, ...)
{
	char info[QR_ERR_MAX];
	va_list ap;

	qr->errcode = errnum;
	va_start(ap, fmt);
	vsnprintf(&(info[0]), QR_ERR_MAX, fmt, ap);
	va_end(ap);
	snprintf(&(qr->errinfo[0]), QR_ERR_MAX, "%s%s", qrStrError(errnum), info);
}


QR_API int
qrDetectDataType(const qr_byte_t *source, int size)
{
	if (qrStrPosNotNumeric(source, size) == -1) {
		return QR_EM_NUMERIC;
	}
	if (qrStrPosNotAlnum(source, size) == -1) {
		return QR_EM_ALNUM;
	}
	if (qrStrPosNotKanji(source, size) == -1) {
		return QR_EM_KANJI;
	}
	return QR_EM_8BIT;
}


QR_API int
qrStrPosNotNumeric(const qr_byte_t *source, int size)
{
	int p = 0;

	while (p < size) {
		if (source[p] < '0' || source[p] > '9') {
			return p;
		}
		p++;
	}
	return -1;
}


QR_API int
qrStrPosNotAlnum(const qr_byte_t *source, int size)
{
	int p = 0;

	while (p < size) {
		if (qr_alnumtable[source[p]] == -1) {
			return p;
		}
		p++;
	}
	return -1;
}


QR_API int
qrStrPosNotKanji(const qr_byte_t *source, int size)
{
	qr_byte_t x, y;
	int p = 0;

	while (p < size - 1) {
		x = source[p++];
		if (x >= 0x81 && x <= 0x9f) {
			x -= 0x81;
		} else if (x >= 0xe0 && x <= 0xea) {
			x -= 0xc1;
		} else {
			
			return p - 1;
		}
		y = source[p++];
		if (y >= 0x40 && y <= 0xfc) {
			y -= 0x40;
		} else {
			
			return p - 1;
		}
		if (qr_dwtable_kanji[x][y] == -1) {
			
			return p - 2;
		}
	}
	if (p < size) {
		return p;
	}
	return -1;
}


QR_API int
qrStrPosNot8bit(const qr_byte_t *source, int size)
{
	qr_byte_t x, y;
	int p = 0;

	while (p < size) {
		x = source[p++];
		if (qr_alnumtable[x] != -1) {
			return p - 1;
		}
		if (p < size && ((x >= 0x81 && x <= 0x9f) || (x >= 0xe0 && x <= 0xea))) {
			if (x < 0xa0) {
				x -= 0x81;
			} else {
				x -= 0xc1;
			}
			y = source[p];
			if (y >= 0x40 && y <= 0xfc && qr_dwtable_kanji[x][y - 0x40] != -1) {
				return p - 1;
			}
		}
	}
	return -1;
}


QR_API int
qrGetEncodedLength(QRCode *qr, int size)
{
	return qrGetEncodedLength2(qr, size, qr->param.mode);
}


QR_API int
qrGetEncodedLength2(QRCode *qr, int size, int mode)
{
	int n, v;

	
	v = (qr->param.version == -1) ? QR_VER_MAX : qr->param.version;
	n = 4 + qr_vertable[v].nlen[mode];

	
	switch (mode) {
	  case QR_EM_NUMERIC:
		
		n += (size / 3) * 10;
		switch (size % 3) {
		  case 1:
			n += 4;
			break;
		  case 2:
			n += 7;
			break;
		}
		break;
	  case QR_EM_ALNUM:
		
		n += (size / 2) * 11;
		if (size % 2 == 1) {
			n += 6;
		}
		break;
	  case QR_EM_8BIT:
		
		n += size * 8;
		break;
	  case QR_EM_KANJI:
		
		n += (size / 2) * 13;
		break;
	  default:
		qrSetErrorInfo(qr, QR_ERR_INVALID_MODE, NULL);
		return -1;
	}

	return n;
}


QR_API int
qrGetEncodableLength(QRCode *qr, int size)
{
	return qrGetEncodableLength2(qr, size, qr->param.mode);
}


QR_API int
qrGetEncodableLength2(QRCode *qr, int size, int mode)
{
	int l, m, n, v;

	
	v = (qr->param.version == -1) ? QR_VER_MAX : qr->param.version;
	n = size - 4 - qr_vertable[v].nlen[mode];
	if (n <= 0) {
		return 0;
	}

	
	switch (mode) {
	  case QR_EM_NUMERIC:
		
		l = (n / 10) * 3;
		m = n % 10;
		if (m >= 7) {
			l += 2;
		} else if (m >= 4) {
			l += 1;
		}
		break;
	  case QR_EM_ALNUM:
		
		l = (n / 11) * 2;
		m = n % 11;
		if (m >= 6) {
			l += 1;
		}
		break;
	  case QR_EM_8BIT:
		
		l = n / 8;
		break;
	  case QR_EM_KANJI:
		
		l = (n / 13) * 2;
		break;
	  default:
		qrSetErrorInfo(qr, QR_ERR_INVALID_MODE, NULL);
		return -1;
	}

	return l;
}


QR_API int
qrAddData(QRCode *qr, const qr_byte_t *source, int size)
{
	if (qr->state == QR_STATE_FINAL) {
		qrSetErrorInfo(qr, QR_ERR_STATE, _QR_FUNCTION);
		return FALSE;
	}
	return qrAddData2(qr, source, size, qr->param.mode);
}


QR_API int
qrAddData2(QRCode *qr, const qr_byte_t *source, int size, int mode)
{
	int enclen, maxlen;
	int version;
	int pos, err;

	if (qr->state == QR_STATE_FINAL) {
		qrSetErrorInfo(qr, QR_ERR_STATE, _QR_FUNCTION);
		return FALSE;
	}

	if (size <= 0) {
		qrSetErrorInfo(qr, QR_ERR_EMPTY_SRC, NULL);
		return FALSE;
	}

	
	if (mode == QR_EM_AUTO) {
		mode = qrDetectDataType(source, size);
	} else if (mode < QR_EM_NUMERIC || mode >= QR_EM_COUNT) {
		qrSetErrorInfo(qr, QR_ERR_INVALID_MODE, NULL);
		return FALSE;
	}


	
	enclen = qrGetEncodedLength2(qr, size, mode);
	if (enclen == -1) {
		return FALSE;
	}
	version = (qr->param.version == -1) ? QR_VER_MAX : qr->param.version;
	maxlen = 8 * qr_vertable[version].ecl[qr->param.eclevel].datawords;
	if (qr->enclen + enclen > maxlen) {
		qrSetErrorInfo3(qr, QR_ERR_LARGE_SRC, ", %d total encoded bits"
				" (max %d bits on version=%d, ecl=%s)",
				qr->enclen + enclen, maxlen, version, qr_eclname[qr->param.eclevel]);
		return FALSE;
	}
	if (qr->param.version == -1) {
		qr->delta1 += qr_vertable[QR_VER_MAX].nlen[mode] - qr_vertable[VERPOINT1].nlen[mode];
		qr->delta2 += qr_vertable[QR_VER_MAX].nlen[mode] - qr_vertable[VERPOINT2].nlen[mode];
	}

	
	if (qr->param.version != -1) {
		qr->enclen += enclen;
		if (!qrHasData(qr)) {
			qrInitDataWord(qr);
		}
		if (qrEncodeDataWord(qr, source, size, mode) == TRUE) {
			qr->state = QR_STATE_SET;
			return TRUE;
		}
		return FALSE;
	}

	
	pos = -1;
	err = QR_ERR_NONE;
	switch (mode) {
	  case QR_EM_NUMERIC:
		pos = qrStrPosNotNumeric(source, size);
		err = QR_ERR_NOT_NUMERIC;
		break;
	  case QR_EM_ALNUM:
		pos = qrStrPosNotAlnum(source, size);
		err = QR_ERR_NOT_ALNUM;
		break;
	  case QR_EM_KANJI:
		pos = qrStrPosNotKanji(source, size);
		err = QR_ERR_NOT_KANJI;
		break;
	}
	if (pos != -1) {
		qrSetErrorInfo3(qr, err, " at offset %d", pos);
		return FALSE;
	}
	qr->enclen += enclen;

	
	while (qr->srcmax < qr->srclen + size + 6) {
		qr->srcmax += QR_SRC_MAX;
		qr->source = (qr_byte_t *)realloc(qr->source, qr->srcmax);
		if (qr->source == NULL) {
			qr->srcmax = 0;
			qrSetErrorInfo2(qr, QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
			return FALSE;
		}
	}

	
	qr->source[qr->srclen++] = (qr_byte_t)(mode | 0x80);
	qr->source[qr->srclen++] = (qr_byte_t)((size >> 24) & 0x7F);
	qr->source[qr->srclen++] = (qr_byte_t)((size >> 16) & 0xFF);
	qr->source[qr->srclen++] = (qr_byte_t)((size >> 8) & 0xFF);
	qr->source[qr->srclen++] = (qr_byte_t)(size & 0xFF);
	memcpy(&(qr->source[qr->srclen]), source, (size_t)size);
	qr->srclen += size;
	qr->source[qr->srclen] = '\0';

	qr->state = QR_STATE_SET;
	return TRUE;
}


QR_API int
qrsAddData(QRStructured *st, const qr_byte_t *source, int size)
{
	if (st->state == QR_STATE_FINAL) {
		qrSetErrorInfo(st->cur, QR_ERR_STATE, _QR_FUNCTION);
		return FALSE;
	}
	return qrsAddData2(st, source, size, st->param.mode);
}


QR_API int
qrsAddData2(QRStructured *st, const qr_byte_t *source, int size, int mode)
{
	int enclen, maxlen, limit, remain;
	int sizes[QR_STA_MAX] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int i, j;
	int p;

	if (st->state == QR_STATE_FINAL) {
		qrSetErrorInfo(st->cur, QR_ERR_STATE, _QR_FUNCTION);
		return FALSE;
	}

	if (size <= 0) {
		qrSetErrorInfo(st->cur, QR_ERR_EMPTY_SRC, NULL);
		return FALSE;
	}

	
	if (mode == QR_EM_AUTO) {
		mode = qrDetectDataType(source, size);
	}

	
	maxlen = 8 * qr_vertable[st->param.version].ecl[st->param.eclevel].datawords;
	limit = maxlen - QR_STA_LEN;
	if (!qrHasData(st->cur)) {
		remain = limit;
	} else {
		remain = qrRemainedDataBits(st->cur);
	}

	
	enclen = qrGetEncodedLength2(st->cur, size, mode);
	if (enclen == -1) {
		return FALSE;
	}
	j = 0;
	if (enclen > remain) {
		int l, r, s;
		r = remain;
		s = size;
		for (i = 0; i <= st->max - st->num; i++) {
			j++;
			l = qrGetEncodableLength2(st->cur, r, mode);
			if (s <= l) {
				sizes[i] = s;
				s = 0;
				break;
			} else {
				sizes[i] = l;
				s -= l;
				r = limit;
			}
		}
		if (s > 0) {
			int snum, reqlen;
			snum = (enclen + maxlen - 1) / maxlen - (st->num - 1);
			reqlen = maxlen * (st->num - 1);
			reqlen += maxlen - remain;
			reqlen += enclen;
			reqlen += qr_vertable[st->param.version].nlen[mode] * (snum - 1) + QR_STA_LEN * snum;
			qrSetErrorInfo3(st->cur, QR_ERR_LARGE_SRC, ", %d total encoded bits"
					" (max %d bits on version=%d, ecl=%s, num=%d)",
					reqlen, maxlen * st->max,
					st->param.version, qr_eclname[st->param.eclevel], st->max);
			return FALSE;
		}
	} else {
		j = 1;
		sizes[0] = size;
	}

	
	p = 0;
	i = 0;
	while (i < j) {
		if (sizes[i] == 0 && i != 0) {
			break;
		}
		if (!qrHasData(st->cur)) {
			
			qrInitDataWord(st->cur);
			qrAddDataBits(st->cur, 4, 0);
			qrAddDataBits(st->cur, 4, 0);
			qrAddDataBits(st->cur, 4, 0);
			qrAddDataBits(st->cur, 8, 0);
		}
		if (sizes[i] != 0) {
			st->cur->enclen += qrGetEncodedLength2(st->cur, sizes[i], mode);
			if (qrEncodeDataWord(st->cur, source + p, sizes[i], mode) == TRUE) {
				st->cur->state = QR_STATE_SET;
				st->state = QR_STATE_SET;
			} else {
				return FALSE;
			}
			p += sizes[i];
		}
		i++;
		if (i < j && sizes[i] > 0) {
			
			int errcode;
			st->qrs[st->num] = qrInit(st->param.version, st->param.mode,
					st->param.eclevel, st->param.masktype, &errcode);
			if (st->qrs[st->num] == NULL) {
				qrSetErrorInfo(st->cur, errcode, NULL);
				return FALSE;
			}
			st->cur = st->qrs[st->num];
			st->num++;
		}
	}

	
	p = 0;
	while (p < size) {
		st->parity ^= source[p++];
	}

	return TRUE;
}


static int
qrInitDataWord(QRCode *qr)
{
	
	memset(qr->dataword, '\0', QR_DWD_MAX);

	
	qr->dwpos = 0;
	qr->dwbit = 7;

	return TRUE;
}


static int
qrEncodeDataWord(QRCode *qr, const qr_byte_t *source, int size, int mode)
{
	int p = 0;
	int e = QR_ERR_NONE;
	int n = 0;
	int word = 0;
	int dwpos = qr->dwbit;
	int dwbit = qr->dwpos;

	if (mode < QR_EM_NUMERIC || mode >= QR_EM_COUNT) {
		e = QR_ERR_INVALID_MODE;
		goto err;
	}

	
	qrAddDataBits(qr, 4, qr_modeid[mode]);

	
	if (mode == QR_EM_KANJI) {
		qrAddDataBits(qr, qr_vertable[qr->param.version].nlen[mode], size / 2);
	} else {
		qrAddDataBits(qr, qr_vertable[qr->param.version].nlen[mode], size);
	}

	
	switch (mode) {
	  case QR_EM_NUMERIC:
		
		while (p < size) {
			qr_byte_t q = source[p];
			if (q < '0' || q > '9') {
				
				e = QR_ERR_NOT_NUMERIC;
				goto err;
			}
			word = word * 10 + (q - '0');
			
			if (++n >= 3) {
				qrAddDataBits(qr, 10, word);
				n = 0;
				word = 0;
			}
			p++;
		}
		
		if (n == 1) {
			qrAddDataBits(qr, 4, word);
		} else if (n == 2) {
			qrAddDataBits(qr, 7, word);
		}
		break;

	  case QR_EM_ALNUM:
		
		while (p < size) {
			signed char q = qr_alnumtable[source[p]];
			if (q == -1) {
				
				e = QR_ERR_NOT_ALNUM;
				goto err;
			}
			word = word * 45 + (int)q;
			
			if (++n >= 2) {
				qrAddDataBits(qr, 11, word);
				n = 0;
				word = 0;
			}
			p++;
		}
		
		if (n == 1) {
			qrAddDataBits(qr, 6, word);
		}
		break;

	  case QR_EM_8BIT:
		
		while (p < size) {
			qrAddDataBits(qr, 8, (int)source[p++]);
		}
		break;

	  case QR_EM_KANJI:
		
		while (p < size - 1) {
			qr_byte_t x, y;
			
			x = source[p++];
			if (x >= 0x81 && x <= 0x9f) {
				x -= 0x81;
			} else if (x >= 0xe0 && x <= 0xea) {
				x -= 0xc1;
			} else {
				
				p -= 1;
				e = QR_ERR_NOT_KANJI;
				goto err;
			}
			
			y = source[p++];
			if (y >= 0x40 && y <= 0xfc) {
				y -= 0x40;
			} else {
				
				p -= 1;
				e = QR_ERR_NOT_KANJI;
				goto err;
			}
			
			word = (int)qr_dwtable_kanji[x][y];
			if (word == -1) {
				
				p -= 2;
				e = QR_ERR_NOT_KANJI;
				goto err;
			}
			qrAddDataBits(qr, 13, word);
		}
		if (p < size) {
			
			e = QR_ERR_NOT_KANJI;
			goto err;
		}
		break;

	  default:
		e = QR_ERR_INVALID_MODE;
		goto err;
	}

	return TRUE;

  err:
	qr->dwpos = dwpos;
	qr->dwbit = dwbit;
	if (e == QR_ERR_INVALID_MODE) {
		qrSetErrorInfo(qr, e, NULL);
	} else {
		qrSetErrorInfo3(qr, e, " at offset %d", p);
	}
	return FALSE;
}


static int
qrFinalizeDataWord(QRCode *qr)
{
	int n, m;
	int word;

	
	n = qrRemainedDataBits(qr);
	if (n < 4) {
		qrAddDataBits(qr, n, 0);
		n = 0;
	} else {
		qrAddDataBits(qr, 4, 0);
		n -= 4;
	}
	
	m = n % 8;
	if (m > 0) {
		qrAddDataBits(qr, m, 0);
		n -= m;
	}

	
	word = PADWORD1;
	while (n >= 8) {
		qrAddDataBits(qr, 8, word);
		if (word == PADWORD1) {
			word = PADWORD2;
		} else {
			word = PADWORD1;
		}
		n -= 8;
	}

	return TRUE;
}


static void
qrAddDataBits(QRCode *qr, int n, int word)
{
	
	while (n-- > 0) {
		
		qr->dataword[qr->dwpos] |= ((word >> n) & 1) << qr->dwbit;
		
		if (--qr->dwbit < 0) {
			qr->dwpos++;
			qr->dwbit = 7;
		}
	}
}


QR_API int
qrRemainedDataBits(QRCode *qr)
{
	int version;
	version = (qr->param.version == -1) ? QR_VER_MAX : qr->param.version;
	return (qr_vertable[version].ecl[qr->param.eclevel].datawords - qr->dwpos) * 8 - (7 - qr->dwbit);
}


static int
qrComputeECWord(QRCode *qr)
{
	int i, j, k, m;
	int ecwtop, dwtop, nrsb, rsbnum;
	qr_byte_t rswork[QR_RSD_MAX];

	
	dwtop = 0;
	ecwtop = 0;
	nrsb = qr_vertable[qr->param.version].ecl[qr->param.eclevel].nrsb;
#define rsb qr_vertable[qr->param.version].ecl[qr->param.eclevel].rsb
#define gfvector qr_gftable[ecwlen]
	for (i = 0; i < nrsb; i++) {
		int dwlen, ecwlen;
		
		
		
		
		rsbnum = rsb[i].rsbnum;
		dwlen = rsb[i].datawords;
		ecwlen = rsb[i].totalwords - rsb[i].datawords;
		
		
		for (j = 0; j < rsbnum; j++) {
			
			memset(&(rswork[0]), '\0', QR_RSD_MAX);
			memcpy(&(rswork[0]), &(qr->dataword[dwtop]), (size_t)dwlen);
			
			for (k = 0; k < dwlen; k++) {
				int e;
				if (rswork[0] == 0) {
					
					for (m = 0; m < QR_RSD_MAX-1; m++) {
						rswork[m] = rswork[m+1];
					}
					rswork[QR_RSD_MAX-1] = 0;
					continue;
				}
				
				e = qr_fac2exp[rswork[0]];
				for (m = 0; m < QR_RSD_MAX-1; m++) {
					rswork[m] = rswork[m+1];
				}
				rswork[QR_RSD_MAX-1] = 0;
				
				for (m = 0; m < ecwlen; m++) {
					rswork[m] ^= qr_exp2fac[(gfvector[m] + e) % 255];
				}
			}
			
			memcpy(&(qr->ecword[ecwtop]), &(rswork[0]), (size_t)ecwlen);
			
			dwtop += dwlen;
			ecwtop += ecwlen;
		}
	}
#undef rsb
#undef gfvector
	return TRUE;
}


static int
qrMakeCodeWord(QRCode *qr)
{
	int i, j, k, cwtop, pos;
	int dwlenmax, ecwlenmax;
	int dwlen, ecwlen, nrsb;
	

	
	nrsb = qr_vertable[qr->param.version].ecl[qr->param.eclevel].nrsb;
	
#define rsb qr_vertable[qr->param.version].ecl[qr->param.eclevel].rsb
	dwlenmax = rsb[nrsb-1].datawords;
	ecwlenmax = rsb[nrsb-1].totalwords - rsb[nrsb-1].datawords;
	
	cwtop = 0;
	for (i = 0; i < dwlenmax; i++) {
		pos = i;
		
		for (j = 0; j < nrsb; j++) {
			dwlen = rsb[j].datawords;
			
			for (k = 0; k < rsb[j].rsbnum; k++) {
				
				if (i < dwlen) {
					qr->codeword[cwtop++] = qr->dataword[pos];
				}
				
				pos += dwlen;
			}
		}
	}
	
	for (i = 0; i < ecwlenmax; i++) {
		pos = i;
		
		for (j = 0; j < nrsb; j++) {
			ecwlen = rsb[j].totalwords - rsb[j].datawords;
			
			for (k = 0; k < rsb[j].rsbnum; k++) {
				
				if (i < ecwlen) {
					qr->codeword[cwtop++] = qr->ecword[pos];
				}
				
				pos += ecwlen;
			}
		}
	}
#undef rsb
#undef nrsb
	return TRUE;
}


static int
qrFillFunctionPattern(QRCode *qr)
{
	int i, j, n, dim, xpos, ypos;

	
	dim = qr_vertable[qr->param.version].dimension;
	
	qrFree(qr->symbol);
	qrFree(qr->_symbol);
	qr->_symbol = (qr_byte_t *)calloc((size_t)dim, (size_t)dim);
	if (qr->_symbol == NULL) {
		return FALSE;
	}
	qr->symbol = (qr_byte_t **)malloc(sizeof(qr_byte_t *) * (size_t)dim);
	if (qr->symbol == NULL) {
		free(qr->_symbol);
		return FALSE;
	}
	for (i = 0; i < dim; i++) {
		qr->symbol[i] = qr->_symbol + dim * i;
	}
	
	for (i = 0; i < QR_DIM_FINDER; i++) {
		for (j = 0; j < QR_DIM_FINDER; j++) {
			qr->symbol[i][j] = qr_finderpattern[i][j];
			qr->symbol[i][dim-1-j] = qr_finderpattern[i][j];
			qr->symbol[dim-1-i][j] = qr_finderpattern[i][j];
		}
	}
	
	for (i = 0; i < QR_DIM_FINDER+1; i++) {
		qr->symbol[i][QR_DIM_FINDER] = QR_MM_FUNC;
		qr->symbol[QR_DIM_FINDER][i] = QR_MM_FUNC;
		qr->symbol[i][dim-1-QR_DIM_FINDER] = QR_MM_FUNC;
		qr->symbol[dim-1-QR_DIM_FINDER][i] = QR_MM_FUNC;
		qr->symbol[dim-1-i][QR_DIM_FINDER] = QR_MM_FUNC;
		qr->symbol[QR_DIM_FINDER][dim-1-i] = QR_MM_FUNC;
	}
	
	n = qr_vertable[qr->param.version].aplnum;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			int x, y, x0, y0, xcenter, ycenter;
			
			ycenter = qr_vertable[qr->param.version].aploc[i];
			xcenter = qr_vertable[qr->param.version].aploc[j];
			y0 = ycenter - QR_DIM_ALIGN / 2;
			x0 = xcenter - QR_DIM_ALIGN / 2;
			if (qrIsFunc(qr, ycenter, xcenter)) {
				
				continue;
			}
			for (y = 0; y < QR_DIM_ALIGN; y++) {
				for (x = 0; x < QR_DIM_ALIGN; x++) {
					qr->symbol[y0+y][x0+x] = qr_alignpattern[y][x];
				}
			}
		}
	}
	
	for (i = QR_DIM_FINDER; i < dim-1-QR_DIM_FINDER; i++) {
		qr->symbol[i][QR_DIM_TIMING] = QR_MM_FUNC;
		qr->symbol[QR_DIM_TIMING][i] = QR_MM_FUNC;
		if ((i & 1) == 0) {
			qr->symbol[i][QR_DIM_TIMING] |= QR_MM_BLACK;
			qr->symbol[QR_DIM_TIMING][i] |= QR_MM_BLACK;
		}
	}
	
	for (i = 0; i < 2; i++) {
		for (j = 0; j < QR_FIN_MAX; j++) {
			xpos = (qr_fmtinfopos[i][j].xpos + dim) % dim;
			ypos = (qr_fmtinfopos[i][j].ypos + dim) % dim;
			qr->symbol[ypos][xpos] |= QR_MM_FUNC;
		}
	}
	xpos = (qr_fmtblackpos.xpos + dim) % dim;
	ypos = (qr_fmtblackpos.ypos + dim) % dim;
	qr->symbol[ypos][xpos] |= QR_MM_FUNC;
	
	if (qr_verinfo[qr->param.version] != -1L) {
		for (i = 0; i < 2; i++) {
			for (j = 0; j < QR_VIN_MAX; j++) {
				xpos = (qr_verinfopos[i][j].xpos + dim) % dim;
				ypos = (qr_verinfopos[i][j].ypos + dim) % dim;
				qr->symbol[ypos][xpos] |= QR_MM_FUNC;
			}
		}
	}

	return TRUE;
}


static int
qrFillCodeWord(QRCode *qr)
{
	int i, j;

	
	qrInitPosition(qr);
	
	for (i = 0; i < qr_vertable[qr->param.version].totalwords; i++) {
		
		for (j = 7; j >= 0; j--) {
			
			if ((qr->codeword[i] & (1 << j)) != 0) {
				qr->symbol[qr->ypos][qr->xpos] |= QR_MM_DATA;
			}
			
			qrNextPosition(qr);
		}
	}

	return TRUE;
}


static void
qrInitPosition(QRCode *qr)
{
	
	qr->xpos = qr->ypos = qr_vertable[qr->param.version].dimension - 1;
	
	qr->xdir = -1;
	qr->ydir = -1;
}


static void
qrNextPosition(QRCode *qr)
{
	do {
		
		qr->xpos += qr->xdir;
		if (qr->xdir > 0) {
			qr->ypos += qr->ydir;
		}
		qr->xdir = -qr->xdir;
		
		if (qr->ypos < 0 || qr->ypos >= qr_vertable[qr->param.version].dimension) {
			qr->ypos -= qr->ydir;
			qr->ydir = -qr->ydir;
			qr->xpos -= 2;
			if (qr->xpos == QR_DIM_TIMING) {
				qr->xpos--;
			}
		}
		
	} while (qrIsFunc(qr, qr->ypos, qr->xpos));
}


static int
qrSelectMaskPattern(QRCode *qr)
{
	int type;
	long penalty, xpenalty;

	if (qr->param.masktype >= 0) {
		
		return qrApplyMaskPattern(qr);
	}
	
	xpenalty = -1L;
	for (type = 0; type < QR_MPT_MAX; type++) {
		
		qrApplyMaskPattern2(qr, type);
		penalty = qrEvaluateMaskPattern(qr);
		
		if (xpenalty == -1L || penalty < xpenalty) {
			qr->param.masktype = type;
			xpenalty = penalty;
		}
	}
	
	return qrApplyMaskPattern(qr);
}


static int
qrApplyMaskPattern(QRCode *qr)
{
	return qrApplyMaskPattern2(qr, qr->param.masktype);
}


static int
qrApplyMaskPattern2(QRCode *qr, int type)
{

	int i, j, dim;

	if (type < 0 || type >= QR_MPT_MAX) {
		qrSetErrorInfo3(qr, QR_ERR_INVALID_MPT, "%d", type);
		return FALSE;
	}

	dim = qr_vertable[qr->param.version].dimension;
	
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			
			if (qrIsFunc(qr, i, j)) {
				continue;
			}
			
			if (qrIsData(qr, i, j)) {
				qr->symbol[i][j] |= QR_MM_BLACK;
			} else {
				qr->symbol[i][j] &= ~QR_MM_BLACK;
			}
		}
	}
	
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			if (qrIsFunc(qr, i, j)) {
				
				continue;
			}
			
			if ((type == 0 && (i + j) % 2 == 0) ||
				(type == 1 && i % 2 == 0) ||
				(type == 2 && j % 3 == 0) ||
				(type == 3 && (i + j) % 3 == 0) ||
				(type == 4 && ((i / 2) + (j / 3)) % 2 == 0) ||
				(type == 5 && (i * j) % 2 + (i * j) % 3 == 0) ||
				(type == 6 && ((i * j) % 2 + (i * j) % 3) % 2 == 0) ||
				(type == 7 && ((i * j) % 3 + (i + j) % 2) % 2 == 0))
			{
				qr->symbol[i][j] ^= QR_MM_BLACK;
			}
		}
	}

	return TRUE;
}


static long
qrEvaluateMaskPattern(QRCode *qr)
{
	int i, j, m, n, dim;
	long penalty;

	
	penalty = 0L;
	dim = qr_vertable[qr->param.version].dimension;
	
	for (i = 0; i < dim; i++) {
		n = 0;
		for (j = 0; j < dim; j++) {
			if (j > 0 && qrIsBlack(qr, i, j) == qrIsBlack(qr, i, j-1)) {
				
				n++;
			} else {
				
				if (n >= 5) {
					penalty += (long)(3 + (n - 5));
				}
				n = 1;
			}
		}
		
		if (n >= 5) {
			penalty += (long)(3 + (n - 5));
		}
	}
	for (i = 0; i < dim; i++) {
		n = 0;
		for (j = 0; j < dim; j++) {
			if (j > 0 && qrIsBlack(qr, j, i) == qrIsBlack(qr, j-1, i)) {
				
				n++;
			} else {
				
				if (n >= 5) {
					penalty += (long)(3 + (n - 5));
				}
				n = 1;
			}
		}
		
		if (n >= 5) {
			penalty += (long)(3 + (n - 5));
		}
	}
	
	for (i = 0; i < dim - 1; i++) {
		for (j = 0; j < dim - 1; j++) {
			if (qrIsBlack(qr, i, j) == qrIsBlack(qr, i, j+1) &&
				qrIsBlack(qr, i, j) == qrIsBlack(qr, i+1, j) &&
				qrIsBlack(qr, i, j) == qrIsBlack(qr, i+1, j+1))
			{
				
				penalty += 3L;
			}
		}
	}
	
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim - 6; j++) {
			if ((j == 0 || !qrIsBlack(qr, i, j-1)) &&
				qrIsBlack(qr, i, j+0) &&
				!qrIsBlack(qr, i, j+1) &&
				qrIsBlack(qr, i, j+2) &&
				qrIsBlack(qr, i, j+3) &&
				qrIsBlack(qr, i, j+4) &&
				!qrIsBlack(qr, i, j+5) &&
				qrIsBlack(qr, i, j+6))
			{
				int k, l;
				l = 1;
				for (k = 0; k < dim - j - 7 && k < 4; k++) {
					if (qrIsBlack(qr, i, j + k + 7)) {
						l = 0;
						break;
					}
				}
				
				if (l) {
					penalty += 40L;
				}
			}
		}
	}
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim - 6; j++) {
			if ((j == 0 || !qrIsBlack(qr, j-1, i)) &&
				qrIsBlack(qr, j+0, i) &&
				!qrIsBlack(qr, j+1, i) &&
				qrIsBlack(qr, j+2, i) &&
				qrIsBlack(qr, j+3, i) &&
				qrIsBlack(qr, j+4, i) &&
				!qrIsBlack(qr, j+5, i) &&
				qrIsBlack(qr, j+6, i) &&
				(j == dim-7 || !qrIsBlack(qr, j+7, i)))
			{
				int k, l;
				l = 1;
				for (k = 0; k < dim - j - 7 && k < 4; k++) {
					if (qrIsBlack(qr, j + k + 7, i)) {
						l = 0;
						break;
					}
				}
				
				if (l) {
					penalty += 40L;
				}
			}
		}
	}
	
	m = 0;
	n = 0;
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			m++;
			if (qrIsBlack(qr, i, j)) {
				n++;
			}
		}
	}
	penalty += (long)(abs((n * 100 / m) - 50) / 5 * 10);
	return penalty;
}


static int
qrFillFormatInfo(QRCode *qr)
{
	int i, j, dim, fmt, modulo, xpos, ypos;
	long v;

	dim = qr_vertable[qr->param.version].dimension;
	
	fmt = ((qr->param.eclevel ^ 1) << 3) | qr->param.masktype;
	modulo = fmt << 10;
	for (i = 14; i >= 10; i--) {
		if ((modulo & (1 << i)) == 0) {
			continue;
		}
		modulo ^= 0x537 << (i - 10);
	}
	fmt = ((fmt << 10) + modulo) ^ 0x5412;
	
	for (i = 0; i < 2; i++) {
		for (j = 0; j < QR_FIN_MAX; j++) {
			if ((fmt & (1 << j)) == 0) {
				continue;
			}
			xpos = (qr_fmtinfopos[i][j].xpos + dim) % dim;
			ypos = (qr_fmtinfopos[i][j].ypos + dim) % dim;
			qr->symbol[ypos][xpos] |= QR_MM_BLACK;
		}
	}
	xpos = (qr_fmtblackpos.xpos + dim) % dim;
	ypos = (qr_fmtblackpos.ypos + dim) % dim;
	qr->symbol[ypos][xpos] |= QR_MM_BLACK;
	
	v = qr_verinfo[qr->param.version];
	if (v != -1L) {
		for (i = 0; i < 2; i++) {
			for (j = 0; j < QR_VIN_MAX; j++) {
				if ((v & (1L << j)) == 0L) {
					continue;
				}
				xpos = (qr_verinfopos[i][j].xpos + dim) % dim;
				ypos = (qr_verinfopos[i][j].ypos + dim) % dim;
				qr->symbol[ypos][xpos] |= QR_MM_BLACK;
			}
		}
	}

	return TRUE;
}


QR_API int
qrFinalize(QRCode *qr)
{
	static qr_funcs funcs[] = {
		qrFinalizeDataWord,
		qrComputeECWord,
		qrMakeCodeWord,
		qrFillFunctionPattern,
		qrFillCodeWord,
		qrSelectMaskPattern,
		qrFillFormatInfo,
		NULL
	};
	int i = 0;
	int ret = TRUE;

	if (qrIsFinalized(qr)) {
		return TRUE;
	}

	
	if (qr->param.version == -1) {
		int maxlen, delta;
		int version = 0;
		while (++version <= QR_VER_MAX) {
			if (version <= VERPOINT1) {
				delta = qr->delta1;
			} else if (version <= VERPOINT2) {
				delta = qr->delta2;
			} else {
				delta = 0;
			}
			maxlen = 8 * qr_vertable[version].ecl[qr->param.eclevel].datawords;
			if (maxlen >= qr->enclen - delta) {
				break;
			}
		}
		if (version > QR_VER_MAX) {
			maxlen = 8 * qr_vertable[QR_VER_MAX].ecl[qr->param.eclevel].datawords;
			qrSetErrorInfo3(qr, QR_ERR_LARGE_SRC, ", %d total encoded bits"
					" (max %d bits on version=%d, ecl=%s)",
					qr->enclen, maxlen, QR_VER_MAX, qr_eclname[qr->param.eclevel]);
			return FALSE;
		}
		qr->param.version = version;
	}

	
	if (qr->source != NULL) {
		qr_byte_t *source;
		int mode, size;

		qrInitDataWord(qr);
		source = qr->source;
		while ((mode = (int)(*source++)) != '\0') {
			mode ^= 0x80;
			size = ((int)*source++) << 24;
			size |= ((int)*source++) << 16;
			size |= ((int)*source++) << 8;
			size |= (int)*source++;
			if (qrEncodeDataWord(qr, source, size, mode) == FALSE) {
				return FALSE;
			}
			source += size;
		}

		qrFree(qr->source);
	}

	
	while (funcs[i] && ret == TRUE) {
		ret = funcs[i++](qr);
	}

	if (ret == TRUE) {
		qrFree(qr->dataword);
		qrFree(qr->ecword);
		qrFree(qr->codeword);
		qr->state = QR_STATE_FINAL;
	}
	return ret;
}


QR_API int
qrIsFinalized(const QRCode *qr)
{
	if (qr->state == QR_STATE_FINAL) {
		return TRUE;
	}
	return FALSE;
}


QR_API int qrHasData(const QRCode *qr)
{
	if (qr->state == QR_STATE_BEGIN) {
		return FALSE;
	}
	return TRUE;
}


QR_API int
qrsFinalize(QRStructured *st)
{
	int m, n, r;

	if (!qrsHasData(st)) {
		qrSetErrorInfo(st->cur, QR_ERR_STATE, _QR_FUNCTION);
		return FALSE;
	} else if (qrsIsFinalized(st)) {
		return TRUE;
	}

	m = 0;
	n = st->num - 1;
	r = TRUE;
	while (m <= n && r == TRUE) {
		int dwpos, dwbit;
		dwpos = st->qrs[m]->dwpos;
		dwbit = st->qrs[m]->dwbit;
		st->qrs[m]->dwpos = 0;
		st->qrs[m]->dwbit = 7;
		qrAddDataBits(st->qrs[m], 4, 3);
		qrAddDataBits(st->qrs[m], 4, m);
		qrAddDataBits(st->qrs[m], 4, n);
		qrAddDataBits(st->qrs[m], 8, st->parity);
		st->qrs[m]->dwpos = dwpos;
		st->qrs[m]->dwbit = dwbit;
		r = qrFinalize(st->qrs[m]);
		m++;
	}

	if (r == TRUE) {
		st->state = QR_STATE_FINAL;
	}
	return r;
}


QR_API int
qrsIsFinalized(const QRStructured *st)
{
	if (st->state == QR_STATE_FINAL) {
		return TRUE;
	}
	return FALSE;
}


QR_API int qrsHasData(const QRStructured *st)
{
	if (st->state == QR_STATE_BEGIN) {
		return FALSE;
	}
	return TRUE;
}


QR_API qr_byte_t *
qrGetSymbol(QRCode *qr, int fmt, int sep, int mag, int *size)
{
	qr_byte_t *buf;
	int _size;

	static const QRConverter cnv[QR_FMT_COUNT] = {
		qrSymbolToPNG,
		qrSymbolToBMP,
		qrSymbolToTIFF,
		qrSymbolToPBM,
		qrSymbolToSVG,
		qrSymbolToJSON,
		qrSymbolToDigit,
		qrSymbolToASCII
	};

	if (fmt < 0 || fmt >= QR_FMT_COUNT) {
		qrSetErrorInfo(qr, QR_ERR_INVALID_FMT, NULL);
		return NULL;
	}

	if (cnv[fmt] == NULL) {
		qrSetErrorInfo(qr, QR_ERR_UNSUPPORTED_FMT, NULL);
		return NULL;
	}

	buf = cnv[fmt](qr, sep, mag, &_size);
	if (buf == NULL) {
		return NULL;
	}

	if (size) {
		*size = _size;
	}
	return buf;
}


QR_API int
qrOutputSymbol(QRCode *qr, FILE *fp, int fmt, int sep, int mag)
{
	qr_byte_t *buf;
	int size;

	buf = qrGetSymbol(qr, fmt, sep, mag, &size);
	if (buf == NULL) {
		return -1;
	}

	if (fp == NULL) {
		fp = stdout;
	}

	if (!fwrite(buf, (size_t)size, 1, fp)) {
		qrSetErrorInfo2(qr, QR_ERR_FWRITE, NULL);
		free(buf);
		return -1;
	}
	if (ferror(fp)) {
		qrSetErrorInfo(qr, QR_ERR_FWRITE, NULL);
		free(buf);
		return -1;
	}

	free(buf);

	return size;
}


QR_API int
qrOutputSymbol2(QRCode *qr, const char *pathname, int fmt, int sep, int mag)
{
	FILE *fp;
	int size;

	if (pathname == NULL || pathname[0] == '\0') {
		qrSetErrorInfo(qr, QR_ERR_EMPTY_PARAM, "(empty pathname)");
		return -1;
	}

	fp = fopen(pathname, "wb");
	if (fp == NULL) {
		qrSetErrorInfo2(qr, QR_ERR_FOPEN, pathname);
		return -1;
	}

	size = qrOutputSymbol(qr, fp, fmt, sep, mag);
	fclose(fp);

	return size;
}


QR_API qr_byte_t *
qrsGetSymbols(QRStructured *st, int fmt, int sep, int mag, int order, int *size)
{
	qr_byte_t *buf;
	int _size;

	static QRsConverter cnv[QR_FMT_COUNT] = {
		qrsSymbolsToPNG,
		qrsSymbolsToBMP,
		qrsSymbolsToTIFF,
		qrsSymbolsToPBM,
		qrsSymbolsToSVG,
		qrsSymbolsToJSON,
		qrsSymbolsToDigit,
		qrsSymbolsToASCII
	};

	if (fmt < 0 || fmt >= QR_FMT_COUNT) {
		qrSetErrorInfo(st->cur, QR_ERR_INVALID_FMT, NULL);
		return NULL;
	}

	buf = cnv[fmt](st, sep, mag, order, &_size);
	if (buf == NULL) {
		return NULL;
	}

	if (size) {
		*size = _size;
	}

	return buf;
}


QR_API int
qrsOutputSymbols(QRStructured *st, FILE *fp, int fmt, int sep, int mag, int order)
{
	qr_byte_t *buf;
	int size;


	buf = qrsGetSymbols(st, fmt, sep, mag, order, &size);
	if (buf == NULL) {
		return -1;
	}

	if (fp == NULL) {
		fp = stdout;
	}

	if (!fwrite(buf, (size_t)size, 1, fp)) {
		qrSetErrorInfo2(st->cur, QR_ERR_FWRITE, NULL);
		free(buf);
		return -1;
	}
	if (ferror(fp)) {
		qrSetErrorInfo(st->cur, QR_ERR_FWRITE, NULL);
		free(buf);
		return -1;
	}

	free(buf);

	return size;
}


QR_API int
qrsOutputSymbols2(QRStructured *st, const char *pathname, int fmt, int sep, int mag, int order)
{
	FILE *fp;
	int size;

	if (pathname == NULL || pathname[0] == '\0') {
		qrSetErrorInfo(st->cur, QR_ERR_EMPTY_PARAM, "(empty pathname)");
		return -1;
	}

	fp = fopen(pathname, "wb");
	if (fp == NULL) {
		qrSetErrorInfo2(st->cur, QR_ERR_FOPEN, pathname);
		return -1;
	}

	size = qrsOutputSymbols(st, fp, fmt, sep, mag, order);
	fclose(fp);

	return size;
}
