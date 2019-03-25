#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct sParams
{
	int len;
	int *params;

};

struct sRegs
{
	int EDI,
		ESI,
		EBP,
		ESP,
		EBX,
		EDX,
		ECX,
		EAX;
};




class SafeHook
{
public:
	SafeHook(void);
	~SafeHook(void);
	
	static void JmpPatch(void * source, void * destination, size_t num_of_nops = 0);
	static void JmpPatch(DWORD source, DWORD destination, size_t num_of_nops = 0){
		JmpPatch((void *)source, (void *)destination, num_of_nops);
	}
	static void JmpPatch(void * source, DWORD destination, size_t num_of_nops = 0){
		JmpPatch(source, (void *)destination, num_of_nops);
	}
	static void JmpPatch(DWORD source, void * destination, size_t num_of_nops = 0){
		JmpPatch((void *)source, destination, num_of_nops);
	}

	static void CallPatch(void * source, void * destination, size_t num_of_nops = 0);
	static void CallPatch(DWORD source, DWORD destination, size_t num_of_nops = 0){
		CallPatch((void *)source, (void *)destination, num_of_nops);
	}
	static void CallPatch(void * source, DWORD destination, size_t num_of_nops = 0){
		CallPatch(source, (void *)destination, num_of_nops);
	}
	static void CallPatch(DWORD source, void * destination, size_t num_of_nops = 0){
		CallPatch((void *)source, destination, num_of_nops);
	}

	static void CreateHook(void * function, 
		void * callback, size_t num_of_params = 0, 
		bool use_registers = false, 
		bool return_to_original = true,
		bool return_param_struct = false
		);

	static void CreateHook(DWORD function, 
		DWORD callback, size_t num_of_params = 0, 
		bool use_registers = false, 
		bool return_to_original = true,
		bool return_param_struct = false){
		CreateHook((void *)function, (void *)callback, num_of_params, use_registers, return_to_original, return_param_struct);
	}
	static void CreateHook(DWORD function, 
		void * callback, size_t num_of_params = 0, 
		bool use_registers = false, 
		bool return_to_original = true,
		bool return_param_struct = false){
		CreateHook((void *)function, callback, num_of_params, use_registers, return_to_original, return_param_struct);
	}
	static void CreateHook(void * function, DWORD callback, 
		size_t num_of_params = 0, 
		bool use_registers = false, 
		bool return_to_original = true,
		bool return_param_struct = false){
		CreateHook(function, (void *)callback, num_of_params, use_registers, return_to_original, return_param_struct);
	}


	static int GetMinOffset(void * code, size_t size);

	static int AsmCopy(void * source, void * destination, size_t mem_size);

	static void MemCpySecured(void * source, void * destination, size_t mem_size);
	
};
