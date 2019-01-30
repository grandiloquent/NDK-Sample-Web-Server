

#include "qrcnv.h"
#if defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__)
#include <stdint.h>
#endif



#define QRCNV_BMP_BISIZE 40  
#define QRCNV_BMP_OFFBITS 62 
#define QRCNV_BMP_PPM 3780   
                             




#if defined(__BIG_ENDIAN__)
#define qrBmpWriteShort(ptr, n) { \
	uint16_t us = (uint16_t)((n) & 0xffffU); \
	qr_byte_t *tmp = (qr_byte_t *)&us; \
	*(ptr)++ = tmp[1]; \
	*(ptr)++ = tmp[0]; \
}
#elif defined(__LITTLE_ENDIAN__)
#define qrBmpWriteShort(ptr, n) { \
	uint16_t us = (uint16_t)((n) & 0xffffU); \
	qr_byte_t *tmp = (qr_byte_t *)&us; \
	*(ptr)++ = tmp[0]; \
	*(ptr)++ = tmp[1]; \
}
#else
#define qrBmpWriteShort(ptr, n) { \
	*(ptr)++ = (n) & 0xffU; \
	*(ptr)++ = ((n) >> 8) & 0xffU; \
}
#endif

#if defined(__BIG_ENDIAN__)
#define qrBmpWriteLong(ptr, n) { \
	uint32_t ul = (uint32_t)(n); \
	qr_byte_t *tmp = (qr_byte_t *)&ul; \
	*(ptr)++ = tmp[3]; \
	*(ptr)++ = tmp[2]; \
	*(ptr)++ = tmp[1]; \
	*(ptr)++ = tmp[0]; \
}
#elif defined(__LITTLE_ENDIAN__)
#define qrBmpWriteLong(ptr, n) { \
	uint32_t ul = (uint32_t)(n); \
	qr_byte_t *tmp = (qr_byte_t *)&ul; \
	*(ptr)++ = tmp[0]; \
	*(ptr)++ = tmp[1]; \
	*(ptr)++ = tmp[2]; \
	*(ptr)++ = tmp[3]; \
}
#else
#define qrBmpWriteLong(ptr, n) { \
	*(ptr)++ = (n) & 0xffU; \
	*(ptr)++ = ((n) >> 8) & 0xffU; \
	*(ptr)++ = ((n) >> 16) & 0xffU; \
	*(ptr)++ = ((n) >> 24) & 0xffU; \
}
#endif

#define qrBmpNextPixel() { \
	if (pxshift == 0) { \
		rptr++; \
		pxshift = 7; \
	} else { \
		pxshift--; \
	} \
}




static qr_byte_t *
qrBmpWriteHeader(qr_byte_t *bof, int size, int width, int height, int imagesize);





QR_API qr_byte_t *
qrSymbolToBMP(QRCode *qr, int sep, int mag, int *size)
{
	qr_byte_t *rbuf, *rptr;
	qr_byte_t *sbuf, *sptr;
	int rsize, rmod, imgsize;
	int sepskips, pxshift;
	int i, j, ix, jx, dim, imgdim, sepdim;

	QRCNV_CHECK_STATE();
	QRCNV_GET_SIZE();

	
	rsize = (imgdim + 7) / 8;
	if ((rmod = (rsize % 4)) != 0) {
		rsize += 4 - rmod;
	}
	imgsize = rsize * imgdim;
	*size = QRCNV_BMP_OFFBITS + imgsize;
	QRCNV_MALLOC(rsize, *size);

	
	sptr = qrBmpWriteHeader(sbuf, *size, imgdim, imgdim, imgsize);

	
	sepskips = rsize * sepdim;
	
	if (sepskips) {
		memset(sptr, 0, (size_t)sepskips);
		sptr += sepskips;
	}
	for (i = dim - 1; i >= 0; i--) {
		memset(rbuf, 0, (size_t)rsize);
		pxshift = 7;
		rptr = rbuf;
		
		for (j = 0; j < sepdim; j++) {
			qrBmpNextPixel();
		}
		
		for (j = 0; j < dim; j++) {
			if (qrIsBlack(qr, i, j)) {
				for (jx = 0; jx < mag; jx++) {
					*rptr |= 1 << pxshift;
					qrBmpNextPixel();
				}
			} else {
				for (jx = 0; jx < mag; jx++) {
					qrBmpNextPixel();
				}
			}
		}
		
		for (ix = 0; ix < mag; ix++) {
			memcpy(sptr, rbuf, (size_t)rsize);
			sptr += rsize;
		}
	}
	
	if (sepskips) {
		memset(sptr, 0, (size_t)sepskips);
		sptr += sepskips;
	}

	free(rbuf);

	return sbuf;
}





QR_API qr_byte_t *
qrsSymbolsToBMP(QRStructured *st, int sep, int mag, int order, int *size)
{
	QRCode *qr = st->cur;
	qr_byte_t *rbuf, *rptr;
	qr_byte_t *sbuf, *sptr;
	int rsize, rmod, imgsize;
	int sepskips, pxshift;
	int i, j, k, ix, jx, kx;
	int cols, rows, pos, xdim, ydim, zdim;
	int dim, imgdim, sepdim;

	QRCNV_SA_CHECK_STATE();
	QRCNV_SA_IF_ONE(qrSymbolToBMP);
	QRCNV_SA_GET_SIZE();

	
	rsize = (xdim + 7) / 8;
	if ((rmod = (rsize % 4)) != 0) {
		rsize += 4 - rmod;
	}
	imgsize = rsize * ydim;
	*size = QRCNV_BMP_OFFBITS + imgsize;
	QRCNV_MALLOC(rsize, *size);

	
	sptr = qrBmpWriteHeader(sbuf, *size, xdim, ydim, imgsize);

	
	sepskips = rsize * sepdim;
	for (k = rows - 1; k >= 0; k--) {
		
		if (sepskips) {
			memset(sptr, 0, (size_t)sepskips);
			sptr += sepskips;
		}
		for (i = dim - 1; i >= 0; i--) {
			memset(rbuf, 0, (size_t)rsize);
			pxshift = 7;
			rptr = rbuf;
			for (kx = 0; kx < cols; kx++) {
				
				for (j = 0; j < sepdim; j++) {
					qrBmpNextPixel();
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
							qrBmpNextPixel();
						}
					} else {
						for (jx = 0; jx < mag; jx++) {
							qrBmpNextPixel();
						}
					}
				}
			}
			
			for (ix = 0; ix < mag; ix++) {
				memcpy(sptr, rbuf, (size_t)rsize);
				sptr += rsize;
			}
		}
	}
	
	if (sepskips) {
		memset(sptr, 0, (size_t)sepskips);
		sptr += sepskips;
	}

	free(rbuf);

	return sbuf;
}




static qr_byte_t *
qrBmpWriteHeader(qr_byte_t *bof, int size, int width, int height, int imagesize)
{
	qr_byte_t *ptr;

	ptr = bof;

	
	*ptr++ = 'B'; 
	*ptr++ = 'M'; 
	qrBmpWriteLong(ptr, size); 
	qrBmpWriteShort(ptr, 0); 
	qrBmpWriteShort(ptr, 0); 
	qrBmpWriteLong(ptr, QRCNV_BMP_OFFBITS); 

	
	qrBmpWriteLong(ptr, QRCNV_BMP_BISIZE); 
	qrBmpWriteLong(ptr, width);  
	qrBmpWriteLong(ptr, height); 
	qrBmpWriteShort(ptr, 1); 
	qrBmpWriteShort(ptr, 1); 
	qrBmpWriteLong(ptr, 0); 
	qrBmpWriteLong(ptr, imagesize);     
	qrBmpWriteLong(ptr, QRCNV_BMP_PPM); 
	qrBmpWriteLong(ptr, QRCNV_BMP_PPM); 
	qrBmpWriteLong(ptr, 2); 
	qrBmpWriteLong(ptr, 2); 

	
	*ptr++ = 255; 
	*ptr++ = 255; 
	*ptr++ = 255; 
	*ptr++ = 0;   

	
	*ptr++ = 0; 
	*ptr++ = 0; 
	*ptr++ = 0; 
	*ptr++ = 0; 

	return ptr;
}


