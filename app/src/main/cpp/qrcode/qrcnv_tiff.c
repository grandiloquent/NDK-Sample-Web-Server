

#include "qrcnv.h"
#if defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__)
#include <stdint.h>
#endif
#include <zlib.h>




#define QRCNV_TIFF_STRIP_SIZE 8192


#define QRCNV_TIFF_ZBUFFER_SIZE QRCNV_TIFF_STRIP_SIZE


#define QRCNV_TIFF_BUFFER_UNIT 8192


#define QRCNV_TIFF_HEADER_SIZE 8


#define QRCNV_TIFF_IFD_ENTRIES 12


#define QRCNV_TIFF_IFD_SIZE (2 + 12 * QRCNV_TIFF_IFD_ENTRIES + 2)


#define QRCNV_TIFF_IFD_STRIPBYTECOUNTS_OFFSET (QRCNV_TIFF_HEADER_SIZE + 2 + 12 * 7)


#define QRCNV_TIFF_DATA_OFFSET (QRCNV_TIFF_HEADER_SIZE + QRCNV_TIFF_IFD_SIZE + 8 + 8)


#define QRCNV_TIFF_T_SHORT      3
#define QRCNV_TIFF_T_LONG       4
#define QRCNV_TIFF_T_RATIONAL   5


#define QRCNV_TIFF_COMPRESSION_NONE 1
#define QRCNV_TIFF_COMPRESSION_ZIP  8


#ifndef QRCNV_TIFF_DEFLATE_LEVEL
#define QRCNV_TIFF_DEFLATE_LEVEL Z_DEFAULT_COMPRESSION
#endif




#define QRCNV_TIFF_REALLOC(reqsize) { \
	while (*size + (reqsize) > wsize) { \
		wsize += QRCNV_TIFF_BUFFER_UNIT; \
		wbuf = realloc(wbuf, (size_t)wsize); \
		if (wbuf == NULL) { \
			free(rbuf); \
			if (compression == QRCNV_TIFF_COMPRESSION_ZIP) { \
				deflateEnd(&zst); \
			} \
			QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION); \
		} \
		wptr = wbuf + *size; \
	} \
}




#define QRCNV_TIFF_DEFLATE_RETURN_FAILURE(errinfo) { \
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




#define qrTiffWriteAscii(ptr, n) { \
	*(ptr)++ = (qr_byte_t)((n) & 0xffU); \
}

#if defined(__BIG_ENDIAN__)
#define qrTiffWriteShort(ptr, n) { \
	uint16_t us = (uint16_t)((n) & 0xffffU); \
	qr_byte_t *tmp = (qr_byte_t *)&us; \
	*(ptr)++ = tmp[0]; \
	*(ptr)++ = tmp[1]; \
}
#elif defined(__LITTLE_ENDIAN__)
#define qrTiffWriteShort(ptr, n) { \
	uint16_t us = (uint16_t)((n) & 0xffffU); \
	qr_byte_t *tmp = (qr_byte_t *)&us; \
	*(ptr)++ = tmp[1]; \
	*(ptr)++ = tmp[0]; \
}
#else
#define qrTiffWriteShort(ptr, n) { \
	*(ptr)++ = ((n) >> 8) & 0xffU; \
	*(ptr)++ = (n) & 0xffU; \
}
#endif

#if defined(__BIG_ENDIAN__)
#define qrTiffWriteLong(ptr, n) { \
	uint32_t ul = (uint32_t)(n); \
	qr_byte_t *tmp = (qr_byte_t *)&ul; \
	*(ptr)++ = tmp[0]; \
	*(ptr)++ = tmp[1]; \
	*(ptr)++ = tmp[2]; \
	*(ptr)++ = tmp[3]; \
}
#elif defined(__LITTLE_ENDIAN__)
#define qrTiffWriteLong(ptr, n) { \
	uint32_t ul = (uint32_t)(n); \
	qr_byte_t *tmp = (qr_byte_t *)&ul; \
	*(ptr)++ = tmp[3]; \
	*(ptr)++ = tmp[2]; \
	*(ptr)++ = tmp[1]; \
	*(ptr)++ = tmp[0]; \
}
#else
#define qrTiffWriteLong(ptr, n) { \
	*(ptr)++ = ((n) >> 24) & 0xffU; \
	*(ptr)++ = ((n) >> 16) & 0xffU; \
	*(ptr)++ = ((n) >> 8) & 0xffU; \
	*(ptr)++ = (n) & 0xffU; \
}
#endif

#define qrTiffWriteRational(ptr, m, n) { \
	qrTiffWriteLong((ptr), (m)); \
	qrTiffWriteLong((ptr), (n)); \
}

#define qrTiffWriteIfd(ptr, tag, type, count, data) { \
	qrTiffWriteShort((ptr), (tag)); \
	qrTiffWriteShort((ptr), (type)); \
	qrTiffWriteLong((ptr), (count)); \
	if ((type) == QRCNV_TIFF_T_SHORT) { \
		qrTiffWriteShort((ptr), (data)); \
		qrTiffWriteShort((ptr), 0); \
	} else { \
		qrTiffWriteLong((ptr), (data)); \
	} \
}




#define qrTiffWriteStrip() { \
	if (compression == QRCNV_TIFF_COMPRESSION_ZIP) { \
		if (deflateReset(&zst) != Z_OK) { \
			QRCNV_TIFF_DEFLATE_RETURN_FAILURE("deflateReset()"); \
		} \
		zst.next_in = &(sbuf[0]); \
		zst.avail_in = (uInt)ssize; \
		zst.next_out = &(zbuf[0]); \
		zst.avail_out = QRCNV_TIFF_ZBUFFER_SIZE; \
		if (deflate(&zst, Z_FINISH) != Z_STREAM_END) { \
			QRCNV_TIFF_DEFLATE_RETURN_FAILURE("deflate()"); \
		} \
		zptr = &(zbuf[0]); \
		zsize = (int)zst.total_out; \
	} else { \
		zptr = &(sbuf[0]); \
		zsize = ssize; \
	} \
	if (totalstrips > 1) { \
		qrTiffUpdateStripInfoTables(wbuf, totalstrips, snum++, *size, zsize); \
	} else { \
		qrTiffUpdateStripByteCount(wbuf, zsize); \
	} \
	QRCNV_TIFF_REALLOC(zsize); \
	memcpy(wptr, zptr, (size_t)zsize); \
	wptr += zsize; \
	*size += zsize; \
}




#define qrTiffNextPixel() { \
	if (pxshift == 0) { \
		rptr++; \
		pxshift = 7; \
	} else { \
		pxshift--; \
	} \
}

#define qrTiffEOR() { \
	pxshift = 7; \
	if (++rnum == rowsperstrip) { \
		qrTiffWriteStrip(); \
		memset(&(sbuf[0]), 0, QRCNV_TIFF_STRIP_SIZE); \
		sptr = &(sbuf[0]); \
		ssize = 0; \
		rnum = 0; \
	} \
}




static qr_byte_t *
qrTiffWriteHeader(qr_byte_t *bof, int width, int height,
		int rowsperstrip, int totalstrips, int compression);

static void
qrTiffUpdateStripByteCount(qr_byte_t *bof, int size);

static void
qrTiffUpdateStripInfoTables(qr_byte_t *bof,
		int totalstrips, int stripnumber, int offset, int size);





QR_API qr_byte_t *
qrSymbolToTIFF(QRCode *qr, int sep, int mag, int *size)
{
	qr_byte_t *rbuf, *wbuf;
	qr_byte_t sbuf[QRCNV_TIFF_STRIP_SIZE];
	qr_byte_t zbuf[QRCNV_TIFF_ZBUFFER_SIZE];
	qr_byte_t *rptr, *sptr, *wptr, *zptr;
	int rsize, ssize, wsize, zsize; 
	int rowsperstrip, totalstrips, compression; 
	int rnum, snum, pxshift;
	int i, j, ix, jx, dim, imgdim, sepdim;
	z_stream zst;

	QRCNV_CHECK_STATE();
	QRCNV_GET_SIZE();

	if (mag > 1) {
		compression = QRCNV_TIFF_COMPRESSION_ZIP;
	} else {
		compression = QRCNV_TIFF_COMPRESSION_NONE;
	}

	
	rsize = (imgdim + 7) / 8;
	rowsperstrip = QRCNV_TIFF_STRIP_SIZE / rsize;
	if (rowsperstrip == 0) {
		QRCNV_RETURN_FAILURE(QR_ERR_WIDTH_TOO_LARGE, NULL);
	} else if (rowsperstrip > imgdim) {
		rowsperstrip = imgdim;
	}
	totalstrips = (imgdim + rowsperstrip - 1) / rowsperstrip;
	wsize = QRCNV_TIFF_BUFFER_UNIT;

	
	rbuf = (qr_byte_t *)malloc((size_t)rsize);
	if (rbuf == NULL) {
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}
	wbuf = (qr_byte_t *)malloc((size_t)wsize);
	if (wbuf == NULL) {
		free(rbuf);
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}
	wptr = qrTiffWriteHeader(wbuf, imgdim, imgdim, rowsperstrip, totalstrips, compression);
	*size = (int)(wptr - wbuf);

	
	if (compression == QRCNV_TIFF_COMPRESSION_ZIP) {
		zst.zalloc = Z_NULL;
		zst.zfree  = Z_NULL;
		zst.opaque = Z_NULL;
		if (deflateInit(&zst, QRCNV_TIFF_DEFLATE_LEVEL) != Z_OK) {
			free(rbuf);
			free(wbuf);
			QRCNV_RETURN_FAILURE(QR_ERR_DEFLATE, "deflateInit()");
		}
	}

	
	memset(&(sbuf[0]), 0, QRCNV_TIFF_STRIP_SIZE);
	sptr = &(sbuf[0]);
	ssize = 0;
	rnum = 0;
	snum = 0;
	pxshift = 7;
	
	for (i = 0; i < sepdim; i++) {
		sptr += rsize;
		ssize += rsize;
		qrTiffEOR();
	}
	for (i = 0; i < dim; i++) {
		memset(rbuf, 0, (size_t)rsize);
		rptr = rbuf;
		
		for (j = 0; j < sepdim; j++) {
			qrTiffNextPixel();
		}
		
		for (j = 0; j < dim; j++) {
			if (qrIsBlack(qr, i, j)) {
				for (jx = 0; jx < mag; jx++) {
					*rptr |= 1 << pxshift;
					qrTiffNextPixel();
				}
			} else {
				for (jx = 0; jx < mag; jx++) {
					qrTiffNextPixel();
				}
			}
		}
		
		for (ix = 0; ix < mag; ix++) {
			memcpy(sptr, rbuf, (size_t)rsize);
			sptr += rsize;
			ssize += rsize;
			qrTiffEOR();
		}
	}
	
	for (i = 0; i < sepdim; i++) {
		sptr += rsize;
		ssize += rsize;
		qrTiffEOR();
	}
	
	if (ssize > 0) {
		qrTiffWriteStrip();
	}
	free(rbuf);

	
	if (compression == QRCNV_TIFF_COMPRESSION_ZIP) {
		if (deflateEnd(&zst) != Z_OK) {
			free(wbuf);
			QRCNV_RETURN_FAILURE(QR_ERR_DEFLATE, "deflateEnd()");
		}
	}

	
	wbuf = (qr_byte_t *)realloc(wbuf, (size_t)*size);
	if (wbuf == NULL) {
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}

	return wbuf;
}





QR_API qr_byte_t *
qrsSymbolsToTIFF(QRStructured *st, int sep, int mag, int order, int *size)
{
	QRCode *qr = st->cur;
	qr_byte_t *rbuf, *wbuf;
	qr_byte_t sbuf[QRCNV_TIFF_STRIP_SIZE];
	qr_byte_t zbuf[QRCNV_TIFF_ZBUFFER_SIZE];
	qr_byte_t *rptr, *sptr, *wptr, *zptr;
	int rsize, ssize, wsize, zsize; 
	int rowsperstrip, totalstrips, compression; 
	int rnum, snum, pxshift;
	int i, j, k, ix, jx, kx;
	int cols, rows, pos, xdim, ydim, zdim;
	int dim, imgdim, sepdim;
	z_stream zst;

	QRCNV_SA_CHECK_STATE();
	QRCNV_SA_IF_ONE(qrSymbolToTIFF);
	QRCNV_SA_GET_SIZE();

	if (mag > 1) {
		compression = QRCNV_TIFF_COMPRESSION_ZIP;
	} else {
		compression = QRCNV_TIFF_COMPRESSION_NONE;
	}

	
	rsize = (xdim + 7) / 8;
	rowsperstrip = QRCNV_TIFF_STRIP_SIZE / rsize;
	if (rowsperstrip == 0) {
		QRCNV_RETURN_FAILURE(QR_ERR_WIDTH_TOO_LARGE, NULL);
	} else if (rowsperstrip > ydim) {
		rowsperstrip = ydim;
	}
	totalstrips = (ydim + rowsperstrip - 1) / rowsperstrip;
	wsize = QRCNV_TIFF_BUFFER_UNIT;

	
	rbuf = (qr_byte_t *)malloc((size_t)rsize);
	if (rbuf == NULL) {
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}
	wbuf = (qr_byte_t *)malloc((size_t)wsize);
	if (wbuf == NULL) {
		free(rbuf);
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}
	wptr = qrTiffWriteHeader(wbuf, xdim, ydim, rowsperstrip, totalstrips, compression);
	*size = (int)(wptr - wbuf);

	
	if (compression == QRCNV_TIFF_COMPRESSION_ZIP) {
		zst.zalloc = Z_NULL;
		zst.zfree  = Z_NULL;
		zst.opaque = Z_NULL;
		if (deflateInit(&zst, QRCNV_TIFF_DEFLATE_LEVEL) != Z_OK) {
			free(rbuf);
			free(wbuf);
			QRCNV_RETURN_FAILURE(QR_ERR_DEFLATE, "deflateInit()");
		}
	}

	
	memset(&(sbuf[0]), 0, QRCNV_TIFF_STRIP_SIZE);
	sptr = &(sbuf[0]);
	ssize = 0;
	rnum = 0;
	snum = 0;
	pxshift = 7;
	for (k = 0; k < rows; k++) {
		
		for (i = 0; i < sepdim; i++) {
			sptr += rsize;
			ssize += rsize;
			qrTiffEOR();
		}
		for (i = 0; i < dim; i++) {
			memset(rbuf, 0, (size_t)rsize);
			rptr = rbuf;
			for (kx = 0; kx < cols; kx++) {
				
				for (j = 0; j < sepdim; j++) {
					qrTiffNextPixel();
				}
				
				if (order < 0) {
					pos = k + rows * kx;
				} else {
					pos = cols * k + kx;
				}
				if (pos >= st->num) {
					break;
				}
				for (j = 0; j < dim; j++) {
					if (qrIsBlack(st->qrs[pos], i, j)) {
						for (jx = 0; jx < mag; jx++) {
							*rptr |= 1 << pxshift;
							qrTiffNextPixel();
						}
					} else {
						for (jx = 0; jx < mag; jx++) {
							qrTiffNextPixel();
						}
					}
				}
			}
			
			for (ix = 0; ix < mag; ix++) {
				memcpy(sptr, rbuf, (size_t)rsize);
				sptr += rsize;
				ssize += rsize;
				qrTiffEOR();
			}
		}
	}
	
	for (i = 0; i < sepdim; i++) {
		sptr += rsize;
		ssize += rsize;
		qrTiffEOR();
	}
	
	if (ssize > 0) {
		qrTiffWriteStrip();
	}
	free(rbuf);

	
	if (compression == QRCNV_TIFF_COMPRESSION_ZIP) {
		if (deflateEnd(&zst) != Z_OK) {
			free(wbuf);
			QRCNV_RETURN_FAILURE(QR_ERR_DEFLATE, "deflateEnd()");
		}
	}

	
	wbuf = (qr_byte_t *)realloc(wbuf, (size_t)*size);
	if (wbuf == NULL) {
		QRCNV_RETURN_FAILURE2(QR_ERR_MEMORY_EXHAUSTED, _QR_FUNCTION);
	}

	return wbuf;
}





static qr_byte_t *
qrTiffWriteHeader(qr_byte_t *bof, int width, int height,
		int rowsperstrip, int totalstrips, int compression)
{
	qr_byte_t *ptr;
	int xresoffset, yresoffset;
	int stripoffsets, stripbytecounts;

	xresoffset = QRCNV_TIFF_HEADER_SIZE + QRCNV_TIFF_IFD_SIZE;
	yresoffset = xresoffset + 8;
	if (totalstrips > 1) {
		
		stripoffsets = yresoffset + 8;
		
		stripbytecounts = stripoffsets + 4 * totalstrips;
	} else {
		
		stripoffsets = yresoffset + 8;
		
		stripbytecounts = 0;
	}

	ptr = bof;

	
	qrTiffWriteAscii(ptr, 'M'); 
	qrTiffWriteAscii(ptr, 'M'); 
	qrTiffWriteShort(ptr, 42);  
	qrTiffWriteLong(ptr, QRCNV_TIFF_HEADER_SIZE);

	
	qrTiffWriteShort(ptr, QRCNV_TIFF_IFD_ENTRIES);

	
	qrTiffWriteIfd(ptr, 0x0100, QRCNV_TIFF_T_LONG, 1, width);

	
	qrTiffWriteIfd(ptr, 0x0101, QRCNV_TIFF_T_LONG, 1, height);

	
	qrTiffWriteIfd(ptr, 0x0102, QRCNV_TIFF_T_SHORT, 1, 1); 

	
	qrTiffWriteIfd(ptr, 0x0103, QRCNV_TIFF_T_SHORT, 1, compression);

	
	qrTiffWriteIfd(ptr, 0x0106, QRCNV_TIFF_T_SHORT, 1, 0); 

	
	qrTiffWriteIfd(ptr, 0x0111, QRCNV_TIFF_T_LONG, totalstrips, stripoffsets);

	
	qrTiffWriteIfd(ptr, 0x0116, QRCNV_TIFF_T_LONG, 1, rowsperstrip);

	
	qrTiffWriteIfd(ptr, 0x0117, QRCNV_TIFF_T_LONG, totalstrips, stripbytecounts);

	
	qrTiffWriteIfd(ptr, 0x011A, QRCNV_TIFF_T_RATIONAL, 1, xresoffset);

	
	qrTiffWriteIfd(ptr, 0x011B, QRCNV_TIFF_T_RATIONAL, 1, yresoffset);

	
	qrTiffWriteIfd(ptr, 0x0128, QRCNV_TIFF_T_SHORT, 1, 2); 

	
	qrTiffWriteIfd(ptr, 0x0140, QRCNV_TIFF_T_SHORT, 3 * 2, 0); 

	
	qrTiffWriteShort(ptr, 0);

	
	qrTiffWriteRational(ptr, 150, 1); 
	qrTiffWriteRational(ptr, 150, 1); 

	
	if (totalstrips > 1) {
		ptr += 4 * totalstrips;
	}

	
	if (totalstrips > 1) {
		ptr += 4 * totalstrips;
	}

	return ptr;
}





static void
qrTiffUpdateStripByteCount(qr_byte_t *bof, int size)
{
	qr_byte_t *ptr;

	ptr = bof + QRCNV_TIFF_IFD_STRIPBYTECOUNTS_OFFSET + 2 + 2 + 4;
	qrTiffWriteLong(ptr, size);
}





static void
qrTiffUpdateStripInfoTables(qr_byte_t *bof,
		int totalstrips, int stripnumber, int offset, int size)
{
	qr_byte_t *optr, *sptr;

	optr = bof + QRCNV_TIFF_DATA_OFFSET + 4 * stripnumber;
	sptr = bof + QRCNV_TIFF_DATA_OFFSET + 4 * totalstrips + 4 * stripnumber;
	qrTiffWriteLong(optr, offset);
	qrTiffWriteLong(sptr, size);
}


