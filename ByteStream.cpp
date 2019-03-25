#include "ByteStream.h"
#include <cstdlib>
#include <cstring>


ByteStream::ByteStream(size_t initial_buffer_size) : bytes_writen(0){
	
	buffer = (unsigned char *)malloc(initial_buffer_size);
	mem_size = initial_buffer_size;
}

ByteStream::ByteStream(void) : bytes_writen(0)
{
	buffer = (unsigned char *)malloc(default_buffer_size);
	mem_size = default_buffer_size;
}

ByteStream::~ByteStream(void)
{

	free(buffer);
}


void ByteStream::check_buffer_size(){
	if(bytes_writen >= mem_size){
		//size_t old_size = mem_size;
		mem_size += default_buffer_size;
		buffer = (unsigned char *)realloc(buffer, mem_size);
		//LOGF_INFO("ByteStream buffer with length = %d is too short, creating new with size %d", old_size, mem_size);
	}
}

ByteStream & ByteStream::operator<< (unsigned char byte){
	size_t where_to_write = bytes_writen;
	bytes_writen+=sizeof(unsigned char);
	
	check_buffer_size();
	
	buffer[where_to_write] = byte;
	return *this;
}

ByteStream & ByteStream::push_int (int decimal){
	size_t where_to_write = bytes_writen;
	bytes_writen+=sizeof(unsigned int);
	check_buffer_size();

	unsigned int * int_room = (unsigned int *)(buffer + where_to_write);
	*int_room = decimal;
	

	return *this;
}


size_t ByteStream::size(){
	return bytes_writen;
}

void ByteStream::copy_to(void * destination){
	memcpy(destination, buffer, size());
}

void ByteStream::copy_to(void * destination, size_t amount){
	memcpy(destination, buffer, amount);
}

ByteStream & ByteStream::operator<< (const char * c_string){
	size_t str_len = strlen(c_string);
	if(bytes_writen+str_len-1 >= mem_size){
		size_t old_size = mem_size;
		if(str_len < default_buffer_size){
			mem_size += default_buffer_size;
			//LOG_INFO("str_len < default_buffer_size");
		}else{
			//LOGF_INFO("memsize %d   strlen %d", mem_size, str_len);
			mem_size += str_len;
		}
		//LOGF_INFO("ByteStream buffer with length = %d is too short, creating new with size %d", old_size, mem_size);
		buffer = (unsigned char *)realloc(buffer, mem_size);
	}
	memcpy(buffer+bytes_writen, c_string, str_len);
	bytes_writen+=str_len;
	
	return *this;
}
ByteStream & ByteStream::push_data (void * data, size_t data_size){
	
	//LOGF_INFO("memsize %d   data_size %d", mem_size, data_size);
	if(bytes_writen+data_size-1 >= mem_size){
		size_t old_size = mem_size;
		if(data_size < default_buffer_size){
			mem_size += default_buffer_size;
			//LOG_INFO("str_len < default_buffer_size");
		}else{
			//LOGF_INFO("memsize %d   data_size %d", mem_size, data_size);
			mem_size += data_size;
		}
		//LOGF_INFO("ByteStream buffer with length = %d is too short, creating new with size %d", old_size, mem_size);
		buffer = (unsigned char *)realloc(buffer, mem_size);
	}
	memcpy(buffer+bytes_writen, data, data_size);
	bytes_writen+=data_size;
	return *this;
}


void ByteStream::fillWithByte(unsigned char patern, size_t amount){
	for(size_t i = 0; i < amount; i ++){
		(*(ByteStream *)this) << patern;
	}
}

unsigned char * ByteStream::CurrentPosition(){
	return buffer+size();
}

unsigned char * ByteStream::StartPosition(){
	return buffer;
}
