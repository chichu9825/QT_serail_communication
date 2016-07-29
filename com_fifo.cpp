#include <stdio.h>
#include "com_fifo.h"
#include <string.h>
//COM_FIFO_TYPE   rx_fifo;

bool com_fifo_init(COM_FIFO_TYPE *fifo)
{
    fifo->data = new unsigned char[COM_FIFO_SIZE];
    fifo->WriteAddr = 0;
    fifo->ReadAddr=0;
    fifo->total = 0;
    return true;
}

bool com_fifo_write(COM_FIFO_TYPE *fifo,unsigned char *data,unsigned int len)
{
    /*Fifo if full*/
    if( len>( COM_FIFO_SIZE - fifo->total ) ){
        return false;
    }

    if( len <= (COM_FIFO_SIZE - fifo->WriteAddr) ){
        memcpy(fifo->data+fifo->WriteAddr,data,len);
        fifo->WriteAddr += len;
        if((COM_FIFO_SIZE == fifo->WriteAddr)){
            fifo->WriteAddr = 0;
        }
    }else{
        memcpy(fifo->data+fifo->WriteAddr,data,(COM_FIFO_SIZE - fifo->WriteAddr) );
        fifo->WriteAddr = 0;
        memcpy(fifo->data+fifo->WriteAddr,data,(len + fifo->WriteAddr - COM_FIFO_SIZE ) );
        fifo->WriteAddr = len + fifo->WriteAddr - COM_FIFO_SIZE;
    }
    return true;
}

bool com_fifo_read(COM_FIFO_TYPE *fifo,unsigned char *data,unsigned int len)
{
    /*Fifo is empty*/
    if( len>fifo->total ){
        return false;
    }
    memcpy(fifo->data,data,len);
    return true;
}

