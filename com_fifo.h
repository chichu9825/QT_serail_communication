#ifndef COM_FIFO_H
#define COM_FIFO_H



#define COM_FIFO_SIZE   1024*64
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



#pragma pack(push, 1)
typedef struct {
	unsigned short frame_header;
	unsigned int uid;
	unsigned int time_stamp;
	float gyro_x;
	float gyro_y;
	float gyro_z;
	unsigned short temp_gyro;
	float accel_x;
	float accel_y;
	float accel_z;
	unsigned int baro;
	unsigned short temp_baro;

}TYPE_sensors_data_t;
#pragma pack(pop)



#endif // COM_FIFO_H
