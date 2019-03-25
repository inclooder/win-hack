#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>


class SafePatch
{
public:
	SafePatch(void);
	void WriteMemory(void * destination, void * source, size_t num_of_bytes);
	void WriteMemory(DWORD destination, void * source, size_t num_of_bytes){
		WriteMemory((void *) destination, source, num_of_bytes);
	}
	void EraseMemory(void * destination, BYTE value, size_t num_of_bytes);
	void EraseMemory(DWORD destination, BYTE value, size_t num_of_bytes){
		EraseMemory((void *)destination, value, num_of_bytes);
	}
	size_t CreateMemoryBackup(void * source, size_t num_of_bytes);
	void RestoreMemoryBackup(size_t backup_index);
	void RestoreDeleteMemoryBackup(size_t backup_index);
	void DeleteMemoryBackup(size_t backup_index);
	void RestoreAll();
	void DeleteAll();
	void RestoreDeleteAll();
	~SafePatch(void);
	static bool WriteSecureMemory(void * destination, void * source, size_t num_of_bytes);
	static bool EraseSecureMemory(void * destination, unsigned char value, size_t num_of_bytes);  
private:
	struct MemoryBackup{
		void * memory;
		void * original_location;
		size_t size;
	};
	std::vector<MemoryBackup> memory_backups;
};
