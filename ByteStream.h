#pragma once

class ByteStream
{
public:
	static const size_t default_buffer_size = 64;
	ByteStream(size_t initial_buffer_size);
	
	ByteStream & operator<< (unsigned char byte);
	
	ByteStream & operator<< (const char * c_string);
	ByteStream & push_data (void * data, size_t data_size);
	ByteStream & push_int (int decimal);
	
	unsigned char * CurrentPosition();
	size_t size();
	void fillWithByte(unsigned char patern, size_t amount);
	void copy_to(void * destination);
	void copy_to(void * destination, size_t amount);
	ByteStream(void);
	~ByteStream(void);
	unsigned char * StartPosition();
private:
	void check_buffer_size();

	size_t mem_size, bytes_writen;
	unsigned char * buffer;
};
