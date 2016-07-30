#ifndef COM_FIFO_H
#define COM_FIFO_H



#define COM_FIFO_SIZE   31
typedef struct {
    int    size;
    char   *data;
    int    WriteAddr;
    int    ReadAddr;
    int    total;
}COM_FIFO_TYPE;
bool com_fifo_init(COM_FIFO_TYPE *fifo);
bool com_fifo_write(COM_FIFO_TYPE *fifo,char *data,int len);
bool com_fifo_read(COM_FIFO_TYPE *fifo,char *data,int len);
#endif // COM_FIFO_H
