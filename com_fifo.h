#ifndef COM_FIFO_H
#define COM_FIFO_H



#define COM_FIFO_SIZE   1024*1024
typedef struct {
    unsigned char   *data;
    unsigned int    WriteAddr;
    unsigned int    ReadAddr;
    unsigned int    total;
}COM_FIFO_TYPE;

#endif // COM_FIFO_H
