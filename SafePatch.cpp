#include "SafePatch.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


SafePatch::SafePatch(void)
{
}

SafePatch::~SafePatch(void)
{
}

void SafePatch::EraseMemory(void * destination, BYTE value, size_t num_of_bytes){
	CreateMemoryBackup(destination, num_of_bytes);
	for(size_t i=0; i < num_of_bytes; i++){
		WriteSecureMemory((BYTE *)destination+i, &value, 1);
	}
}

void SafePatch::WriteMemory(void * destination, void * source, size_t num_of_bytes){
	CreateMemoryBackup(destination, num_of_bytes);
	WriteSecureMemory(destination, source, num_of_bytes);
}

bool SafePatch::WriteSecureMemory(void * destination, void * source, size_t num_of_bytes){
	DWORD oldProtect;
	VirtualProtect(destination, num_of_bytes, PAGE_READWRITE, &oldProtect);
	memcpy(destination, source, num_of_bytes); 
	VirtualProtect(destination, num_of_bytes, PAGE_READWRITE, &oldProtect);
	return true;
}
bool SafePatch::EraseSecureMemory(void * destination, unsigned char value, size_t num_of_bytes){
	DWORD oldProtect;
	VirtualProtect(destination, num_of_bytes, PAGE_READWRITE, &oldProtect);
	memset(destination, value, num_of_bytes);
	VirtualProtect(destination, num_of_bytes, PAGE_READWRITE, &oldProtect);
	return true;
}

size_t SafePatch::CreateMemoryBackup(void * source, size_t num_of_bytes){
	MemoryBackup mb;
	mb.memory = new unsigned char[num_of_bytes];;
	mb.original_location = source;
	mb.size = num_of_bytes;
	memcpy(mb.memory, source, num_of_bytes);
	memory_backups.push_back(mb);
	return memory_backups.size()-1;
}

void SafePatch::RestoreMemoryBackup(size_t backup_index){
	MemoryBackup & backup = memory_backups.at(backup_index);
	WriteSecureMemory(backup.original_location, backup.memory, backup.size);
}

void SafePatch::DeleteMemoryBackup(size_t backup_index){
	MemoryBackup & backup = memory_backups.at(backup_index);
	delete [] backup.memory;
	memory_backups.erase(memory_backups.begin()+backup_index);
}

void SafePatch::RestoreDeleteMemoryBackup(size_t backup_index){
	RestoreMemoryBackup(backup_index);
	DeleteMemoryBackup(backup_index);
}

void SafePatch::RestoreAll(){
	for(size_t i = memory_backups.size(); i > 0; i--){
		RestoreMemoryBackup(i);
	}
}
void SafePatch::DeleteAll(){
	for(size_t i = memory_backups.size(); i > 0; i--){
		DeleteMemoryBackup(i);
	}
}

void SafePatch::RestoreDeleteAll(){
	RestoreAll();
	DeleteAll();
}