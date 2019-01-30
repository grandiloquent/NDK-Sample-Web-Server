

#include "qrcnv.h"
#if defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__)
#include <stdint.h>
#endif
#include <zlib.h>
#include "crc.h"




#define QRCNV_PNG_BUFFER_UNIT 8192


#define QRCNV_PNG_SIGNATURE_SIZE 8


#define QRCNV_PNG_IHDR_SIZE 25


#define QRCNV_PNG_IHDR_DATA_SIZE 13


#define QRCNV_PNG_IEND_SIZE 12


#ifndef QRCNV_PNG_DEFLATE_LEVEL
#define QRCNV_PNG_DEFLATE_LEVEL Z_DEFAULT_COMPRESSION
#endif




#define QRCNV_PNG_REALLOC(reqsize) { \
	while (*size + (reqsize) > wsize) { \
		wsize += QRCNV_PNG_BUFFER_UNIT; \
		wbuf = realloc(wbuf, (size_t)wsize); \
		if (wbuf == NULL) { \
			free(rbuf); \
			deflateEnd(&zst); \
			QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION); \
		} \
		wptr = wbuf + *size; \
	} \
}




#define QRCNV_PNG_DEFLATE_RETURN_FAILURE(errinfo) { \
	char _info[128]; \
	if (zst.msg) { \
		snprintf(&(_info[0]), 128, "%s", zst.msg); \
	} else { \
		snprintf(&(_info[0]), 128, "%s", (errinfo)); \
	} \
	free(rbuf); \
	free(wbuf); \
	deflateEnd(&zst); \
	QRCNV_RETURN_FAILURE(QR_ERR_DEFLATE, _info); \
}




#if defined(__BIG_ENDIAN__)
#define qrPngWriteLong(ptr, n) { \
	uint32_t ul = (uint32_t)(n); \
	qr_byte_t *tmp = (qr_byte_t *)&ul; \
	*(ptr)++ = tmp[0]; \
	*(ptr)++ = tmp[1]; \
	*(ptr)++ = tmp[2]; \
	*(ptr)++ = tmp[3]; \
}
#elif defined(__LITTLE_ENDIAN__)
#define qrPngWriteLong(ptr, n) { \
	uint32_t ul = (uint32_t)(n); \
	qr_byte_t *tmp = (qr_byte_t *)&ul; \
	*(ptr)++ = tmp[3]; \
	*(ptr)++ = tmp[2]; \
	*(ptr)++ = tmp[1]; \
	*(ptr)++ = tmp[0]; \
}
#else
#define qrPngWriteLong(ptr, n) { \
	*(ptr)++ = ((n) >> 24) & 0xffU; \
	*(ptr)++ = ((n) >> 16) & 0xffU; \
	*(ptr)++ = ((n) >> 8) & 0xffU; \
	*(ptr)++ = (n) & 0xffU; \
}
#endif




#define qrPngWriteData(mode, expected) { \
	zst.next_in = &(sbuf[0]); \
	zst.avail_in = (uInt)ssize; \
	if (deflate(&zst, mode) != expected) { \
		QRCNV_PNG_DEFLATE_RETURN_FAILURE("deflate()"); \
	} \
}




#define qrPngNextPixel() { \
	if (pxshift == 0) { \
		rptr++; \
		pxshift = 7; \
	} else { \
		pxshift--; \
	} \
}

#define qrPngEOR() { \
	pxshift = 7; \
	if (++rnum == wrows) { \
		qrPngWriteData(Z_NO_FLUSH, Z_OK); \
		memset(sbuf, 0xff, QRCNV_PNG_BUFFER_UNIT); \
		sptr = &(sbuf[0]); \
		ssize = 0; \
		rnum = 0; \
	} \
}




static qr_byte_t *
qrPngWriteHeader(qr_byte_t *bof, int width, int height);

static qr_byte_t *
qrPngWriteBeginIdat(qr_byte_t *ptr);

static qr_byte_t *
qrPngWriteEndIdat(qr_byte_t *ptr, qr_byte_t *idat, int size);

static qr_byte_t *
qrPngWriteIend(qr_byte_t *ptr);





QR_API qr_byte_t *
qrSymbolToPNG(QRCode *qr, int sep, int mag, int *size)
{
	qr_byte_t *rbuf, *wbuf;
	qr_byte_t sbuf[QRCNV_PNG_BUFFER_UNIT];
	qr_byte_t zbuf[QRCNV_PNG_BUFFER_UNIT];
	qr_byte_t *rptr, *sptr, *wptr, *idat;
	int rsize, ssize, wsize, zsize; 
	int rnum, snum, pxshift, wrows;
	int i, j, ix, jx, dim, imgdim, sepdim;
	z_stream zst;

	QRCNV_CHECK_STATE();
	QRCNV_GET_SIZE();

	
	rsize = (imgdim + 7) / 8 + 1;
	wrows = QRCNV_PNG_BUFFER_UNIT / rsize;
	if (wrows == 0) {
		QRCNV_RETURN_FAILURE(QR_ERR_WIDTH_TOO_LARGE, NULL);
	}
	wsize = QRCNV_PNG_BUFFER_UNIT;

	
	rbuf = (qr_byte_t *)malloc((size_t)rsize);
	if (rbuf == NULL) {
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}
	wbuf = (qr_byte_t *)malloc((size_t)wsize);
	if (wbuf == NULL) {
		free(rbuf);
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}
	wptr = qrPngWriteHeader(wbuf, imgdim, imgdim);
	wptr = qrPngWriteBeginIdat(wptr);
	*size = (int)(wptr - wbuf);

	
	zst.zalloc = Z_NULL;
	zst.zfree  = Z_NULL;
	zst.opaque = Z_NULL;
	if (deflateInit(&zst, QRCNV_PNG_DEFLATE_LEVEL) != Z_OK) {
		free(rbuf);
		free(wbuf);
		QRCNV_RETURN_FAILURE(QR_ERR_DEFLATE, "deflateInit()");
	}
	zst.next_out = &(zbuf[0]);
	zst.avail_out = QRCNV_PNG_BUFFER_UNIT;

	
	memset(sbuf, 0, QRCNV_PNG_BUFFER_UNIT);
	sptr = &(sbuf[0]);
	ssize = 0;
	rnum = 0;
	snum = 0;
	pxshift = 7;
	
	for (i = 0; i < sepdim; i++) {
		memset(sptr, 0xff, rsize);
		*sptr = 0;
		sptr += rsize;
		ssize += rsize;
		qrPngEOR();
	}
	for (i = 0; i < dim; i++) {
		memset(rbuf, 0, (size_t)rsize);
		rptr = rbuf;
		*rptr++ = 0;
		
		for (j = 0; j < sepdim; j++) {
			*rptr |= 1 << pxshift;
			qrPngNextPixel();
		}
		
		for (j = 0; j < dim; j++) {
			if (qrIsBlack(qr, i, j)) {
				for (jx = 0; jx < mag; jx++) {
					qrPngNextPixel();
				}
			} else {
				for (jx = 0; jx < mag; jx++) {
					*rptr |= 1 << pxshift;
					qrPngNextPixel();
				}
			}
		}
		
		for (j = 0; j < sepdim; j++) {
			*rptr |= 1 << pxshift;
			qrPngNextPixel();
		}
		
		for (ix = 0; ix < mag; ix++) {
			memcpy(sptr, rbuf, (size_t)rsize);
			sptr += rsize;
			ssize += rsize;
			qrPngEOR();
		}
	}
	
	for (i = 0; i < sepdim; i++) {
		memset(sptr, 0xff, rsize);
		*sptr = 0;
		sptr += rsize;
		ssize += rsize;
		qrPngEOR();
	}
	
	qrPngWriteData(Z_FINISH, Z_STREAM_END);
	zsize = (int)zst.total_out;
	QRCNV_PNG_REALLOC(zsize);
	memcpy(wptr, zbuf, (size_t)zsize);
	wptr += zsize;
	*size += zsize;
	free(rbuf);

	
	if (deflateEnd(&zst) != Z_OK) {
		free(wbuf);
		QRCNV_RETURN_FAILURE(QR_ERR_DEFLATE, "deflateEnd()");
	}

	
	QRCNV_PNG_REALLOC(4 + QRCNV_PNG_IEND_SIZE);
	idat = wbuf + QRCNV_PNG_SIGNATURE_SIZE + QRCNV_PNG_IHDR_SIZE;
	wptr = qrPngWriteEndIdat(wptr, idat, (int)(wptr - idat) - 8);
	wptr = qrPngWriteIend(wptr);
	*size = (int)(wptr - wbuf);

	
	wbuf = (qr_byte_t *)realloc(wbuf, (size_t)*size);
	if (wbuf == NULL) {
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}

	return wbuf;
}





QR_API qr_byte_t *
qrsSymbolsToPNG(QRStructured *st, int sep, int mag, int order, int *size)
{
	QRCode *qr = st->cur;
	qr_byte_t *rbuf, *wbuf;
	qr_byte_t sbuf[QRCNV_PNG_BUFFER_UNIT];
	qr_byte_t zbuf[QRCNV_PNG_BUFFER_UNIT];
	qr_byte_t *rptr, *sptr, *wptr, *idat;
	int rsize, ssize, wsize, zsize; 
	int rnum, snum, pxshift, wrows;
	int i, j, k, ix, jx, kx;
	int cols, rows, pos, xdim, ydim, zdim;
	int dim, imgdim, sepdim;
	z_stream zst;

	QRCNV_SA_CHECK_STATE();
	QRCNV_SA_IF_ONE(qrSymbolToPNG);
	QRCNV_SA_GET_SIZE();

	
	rsize = (xdim + 7) / 8 + 1;
	wrows = QRCNV_PNG_BUFFER_UNIT / rsize;
	if (wrows == 0) {
		QRCNV_RETURN_FAILURE(QR_ERR_WIDTH_TOO_LARGE, NULL);
	}
	wsize = QRCNV_PNG_BUFFER_UNIT;

	
	rbuf = (qr_byte_t *)malloc((size_t)rsize);
	if (rbuf == NULL) {
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}
	wbuf = (qr_byte_t *)malloc((size_t)wsize);
	if (wbuf == NULL) {
		free(rbuf);
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}
	wptr = qrPngWriteHeader(wbuf, xdim, ydim);
	wptr = qrPngWriteBeginIdat(wptr);
	*size = (int)(wptr - wbuf);

	
	zst.zalloc = Z_NULL;
	zst.zfree  = Z_NULL;
	zst.opaque = Z_NULL;
	if (deflateInit(&zst, QRCNV_PNG_DEFLATE_LEVEL) != Z_OK) {
		free(rbuf);
		free(wbuf);
		QRCNV_RETURN_FAILURE(QR_ERR_DEFLATE, "deflateInit()");
	}
	zst.next_out = &(zbuf[0]);
	zst.avail_out = QRCNV_PNG_BUFFER_UNIT;

	
	memset(sbuf, 0, QRCNV_PNG_BUFFER_UNIT);
	sptr = &(sbuf[0]);
	ssize = 0;
	rnum = 0;
	snum = 0;
	pxshift = 7;
	for (k = 0; k < rows; k++) {
		
		for (i = 0; i < sepdim; i++) {
			memset(sptr, 0xff, rsize);
			*sptr = 0;
			sptr += rsize;
			ssize += rsize;
			qrPngEOR();
		}
		for (i = 0; i < dim; i++) {
			memset(rbuf, 0, (size_t)rsize);
			rptr = rbuf;
			*rptr++ = 0;
			for (kx = 0; kx < cols; kx++) {
				
				for (j = 0; j < sepdim; j++) {
					*rptr |= 1 << pxshift;
					qrPngNextPixel();
				}
				
				if (order < 0) {
					pos = k + rows * kx;
				} else {
					pos = cols * k + kx;
				}
				if (pos >= st->num) {
					for (j = 0; j < dim; j++) {
						*rptr |= 1 << pxshift;
					}
				} else {
					for (j = 0; j < dim; j++) {
						if (qrIsBlack(st->qrs[pos], i, j)) {
							for (jx = 0; jx < mag; jx++) {
								qrPngNextPixel();
							}
						} else {
							for (jx = 0; jx < mag; jx++) {
								*rptr |= 1 << pxshift;
								qrPngNextPixel();
							}
						}
					}
				}
				
				for (j = 0; j < sepdim; j++) {
					*rptr |= 1 << pxshift;
					qrPngNextPixel();
				}
			}
			
			for (ix = 0; ix < mag; ix++) {
				memcpy(sptr, rbuf, (size_t)rsize);
				sptr += rsize;
				ssize += rsize;
				qrPngEOR();
			}
		}
	}
	
	for (i = 0; i < sepdim; i++) {
		memset(sptr, 0xff, rsize);
		*sptr = 0;
		sptr += rsize;
		ssize += rsize;
		qrPngEOR();
	}
	
	qrPngWriteData(Z_FINISH, Z_STREAM_END);
	zsize = (int)zst.total_out;
	QRCNV_PNG_REALLOC(zsize);
	memcpy(wptr, zbuf, (size_t)zsize);
	wptr += zsize;
	*size += zsize;
	free(rbuf);

	
	if (deflateEnd(&zst) != Z_OK) {
		free(wbuf);
		QRCNV_RETURN_FAILURE(QR_ERR_DEFLATE, "deflateEnd()");
	}

	
	QRCNV_PNG_REALLOC(4 + QRCNV_PNG_IEND_SIZE);
	idat = wbuf + QRCNV_PNG_SIGNATURE_SIZE + QRCNV_PNG_IHDR_SIZE;
	wptr = qrPngWriteEndIdat(wptr, idat, (int)(wptr - idat) - 8);
	wptr = qrPngWriteIend(wptr);
	*size = (int)(wptr - wbuf);

	
	wbuf = (qr_byte_t *)realloc(wbuf, (size_t)*size);
	if (wbuf == NULL) {
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}

	return wbuf;
}





static qr_byte_t *
qrPngWriteHeader(qr_byte_t *bof, int width, int height)
{
	const qr_byte_t signature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	qr_byte_t *ptr = bof;
	crc_t c;

	
	memcpy(ptr, signature, 8);
	ptr += 8;

	
	qrPngWriteLong(ptr, QRCNV_PNG_IHDR_DATA_SIZE);
	*ptr++ = 'I';
	*ptr++ = 'H';
	*ptr++ = 'D';
	*ptr++ = 'R';
	qrPngWriteLong(ptr, width);  
	qrPngWriteLong(ptr, height); 
	*ptr++ = 1; 
	*ptr++ = 0; 
	*ptr++ = 0; 
	*ptr++ = 0; 
	*ptr++ = 0; 
	c = crc(bof + 12, 4 + QRCNV_PNG_IHDR_DATA_SIZE);
	qrPngWriteLong(ptr, c); 

	return ptr;
}





static qr_byte_t *
qrPngWriteBeginIdat(qr_byte_t *ptr)
{
	qrPngWriteLong(ptr, 0);
	*ptr++ = 'I';
	*ptr++ = 'D';
	*ptr++ = 'A';
	*ptr++ = 'T';

	return ptr;
}





static qr_byte_t *
qrPngWriteEndIdat(qr_byte_t *ptr, qr_byte_t *idat, int size)
{
	qr_byte_t *org_ptr;
	crc_t c;

	org_ptr = ptr;
	ptr = idat;
	qrPngWriteLong(ptr, size);

	c = crc(idat + 4, size + 4);
	ptr = org_ptr;
	qrPngWriteLong(ptr, c);

	return ptr;
}





static qr_byte_t *
qrPngWriteIend(qr_byte_t *ptr)
{
	crc_t c;

	qrPngWriteLong(ptr, 0);
	*ptr++ = 'I';
	*ptr++ = 'E';
	*ptr++ = 'N';
	*ptr++ = 'D';
	c = crc(ptr - 4, 4);
	qrPngWriteLong(ptr, c);

	return ptr;
}


