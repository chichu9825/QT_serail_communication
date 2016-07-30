
#include <stdio.h>
#include "com_fifo.h"
#include <string.h>
//COM_FIFO_TYPE   rx_fifo;

bool com_fifo_init(COM_FIFO_TYPE *fifo)
{
    fifo->size = COM_FIFO_SIZE;
    fifo->data = new char[COM_FIFO_SIZE];
    fifo->WriteAddr = 0;
    fifo->ReadAddr=0;
    fifo->total = 0;
    return true;
}
char lDebugShow[128];
bool com_fifo_write(COM_FIFO_TYPE *fifo,char *data,int len)
{
    /*Fifo if full*/
    if( len>( fifo->size - fifo->total ) ){
        return false;
    }
    fifo->total += len ;

    if( len <= ( fifo->size - fifo->WriteAddr ) ){
		memcpy(lDebugShow,data,len);
        memcpy(fifo->data+fifo->WriteAddr,data,len);
		memcpy(lDebugShow,fifo->data,len);
        fifo->WriteAddr += len;
        fifo->WriteAddr %= fifo->size;

    }else{
        int lFirstLen = fifo->size - fifo->WriteAddr;
		int lSecondLen =len - lFirstLen;
        memcpy(fifo->data+fifo->WriteAddr,data, lFirstLen);
        fifo->WriteAddr = 0;
		memcpy(fifo->data,data+lFirstLen,lSecondLen );
        fifo->WriteAddr = lSecondLen;
    }
    return true;
}

bool com_fifo_read(COM_FIFO_TYPE *fifo,char *data,int len)
{
    /*Fifo is empty*/
    if( len > fifo->total ){
        return false;
    }
    fifo->total -= len ;

    if( len <= ( fifo->size - fifo->ReadAddr ) ){
		memcpy(data,fifo->data+fifo->ReadAddr,len);
        fifo->ReadAddr += len;
        fifo->ReadAddr %= fifo->size;

    }else{
        int lFirstLen = fifo->size - fifo->ReadAddr;
		int lSecondLen =len - lFirstLen;
		memcpy(data,fifo->data+fifo->ReadAddr, lFirstLen);
        fifo->ReadAddr = 0;
		memcpy(data+lFirstLen,fifo->data,lSecondLen );
        fifo->ReadAddr = lSecondLen;
    }
    return true;
}

