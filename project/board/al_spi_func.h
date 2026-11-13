
#ifndef __AL_SPI_FUNC_H__
#define __AL_SPI_FUNC_H__

typedef enum
{
	UMSG_SPI_EMPTY = 0,
	UMSG_REQUEST_SPI_INITS,
	UMSG_REQUEST_SPI_WRITE,
	UMSG_REQUEST_SPI_READ,
	UMSG_REQUEST_SPI_WRITEREAD,
	UMSG_REQUEST_SPI_SLAVEWRITE,
	UMSG_REQUEST_SPI_SLAVEREAD,
}msg_uspi_te;

void mg_spi_init(msg_class_ts *msg);


#endif






