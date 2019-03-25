#include "SafeHook.h"
#include <distorm.h>
#include "ByteStream.h"

SafeHook::SafeHook(void)
{
}

SafeHook::~SafeHook(void)
{
}

void SafeHook::JmpPatch(void * source, void * destination, size_t num_of_nops){
	DWORD OldProtect;
	VirtualProtect(source, 5 + num_of_nops, PAGE_EXECUTE_READWRITE, &OldProtect);
	*(char*)source = (char)0xE9; //JMP
	*(DWORD *)((DWORD)source +1) = (DWORD)destination - (DWORD)source - 5; //ADDRESS
	for (size_t i = 0; i < num_of_nops; ++i) {
		*(BYTE*)((DWORD)source + 5 + i) = 0x90; //NOP
	}
	VirtualProtect(source, 5 + num_of_nops, OldProtect, &OldProtect);
}

void SafeHook::CallPatch(void * source, void * destination, size_t num_of_nops){
	DWORD OldProtect;
	VirtualProtect(source, 5 + num_of_nops, PAGE_EXECUTE_READWRITE, &OldProtect);
	*(char*)source = (char)0xE8; //CALL
	*(DWORD *)((DWORD)source +1) = (DWORD)destination - (DWORD)source - 5; //ADDRESS
	for (size_t i = 0; i < num_of_nops; ++i) {
		*(BYTE*)((DWORD)source + 5 + i) = 0x90; //NOP
	}
	VirtualProtect(source, 5 + num_of_nops, OldProtect, &OldProtect);
}

void SafeHook::MemCpySecured(void * source, void * destination, size_t mem_size){
	DWORD OldProtect;
	VirtualProtect(source, mem_size, PAGE_EXECUTE_READWRITE, &OldProtect);
	memcpy(destination, source, mem_size);
	VirtualProtect(source, mem_size, OldProtect, &OldProtect);
}

int SafeHook::GetMinOffset(void * code, size_t PatchSize){
	const unsigned int MaxInstructions = 20;
		_DecodeResult result;
		_DecodedInst instructions[MaxInstructions];
		unsigned int instructionCount = 0;

		result = distorm_decode(0, (const unsigned char *)code, 20, Decode32Bits, instructions, MaxInstructions, &instructionCount);
		if (result != DECRES_SUCCESS) return -1;

		size_t offset = 0;
		for (unsigned int i = 0; offset < PatchSize && i < instructionCount; ++i)
			offset += instructions[i].size;
		// if we were unable to disassemble enough instructions we fail
		if (offset < PatchSize) return -1;

		return offset;
}

int SafeHook::AsmCopy(void * source, void * destination, size_t mem_size){
	unsigned int MaxInstructions = mem_size*3;

	_DecodeResult result;
	_DecodedInst * instructions = new _DecodedInst[MaxInstructions];
	unsigned int instructionCount = 0;

	result = distorm_decode(0, (const unsigned char *)source, mem_size, Decode32Bits, instructions, MaxInstructions, &instructionCount);
	//LOGF_INFO("RESULT = %d", result);
	if (result != DECRES_SUCCESS) return -1;

	unsigned char * src_chr = (unsigned char *)source;
	unsigned char * dst_chr = (unsigned char *)destination;

	for(unsigned int i = 0; i < instructionCount; i++){
		//We are looking for jmp (E9) and call (E8)
		if(src_chr[instructions[i].offset] == 0xE9 || src_chr[instructions[i].offset] == 0xE8){
			//Calculate jmp, call destination
			//call_destination = code + position + 5
			DWORD jmpcall_dst =  *(DWORD *)(src_chr+instructions[i].offset+1) + (DWORD)(src_chr+instructions[i].offset) + 5;
			//Make jump or call
			//LOGF_INFO("MEMCPYSECURED CREATE JUMPCALL ON 0x%X", src_chr+instructions[i].offset);
			if(src_chr[instructions[i].offset] == 0xE9){
				JmpPatch(dst_chr+instructions[i].offset, jmpcall_dst);
			}else if(src_chr[instructions[i].offset] == 0xE8){
				CallPatch(dst_chr+instructions[i].offset, jmpcall_dst);
			}

		}else{
			//Copy instruction
			//LOGF_INFO("MEMCPYSECURED Z 0x%X DO 0x%X WIELKOSC 0x%X",src_chr+instructions[i].offset, dst_chr+ instructions[i].offset, instructions[i].size);
			MemCpySecured(src_chr+instructions[i].offset, dst_chr+ instructions[i].offset, instructions[i].size);
		}
	}

	delete [] instructions;
	return 0;
}

//powinno byc unsigned char
void SafeHook::CreateHook(void * function, void * callback, size_t num_of_params, bool use_registers, bool return_to_original, bool return_param_struct){



	int bkp_size = GetMinOffset(function, 5);
	if(bkp_size == -1){
		//LOG_ERR("GetMinOffset return -1");
		return;
	}
	//LOGF_INFO("GetMinOffset = %d", bkp_size);

	ByteStream * pbs = new ByteStream(128);
	ByteStream  & bs = *pbs;


	bs << 0x60; //PUSHAD

	bs << "\x8B\xEC"; //MOV EBP,ESP
	bs << "\x8B\xDC"; //MOV EBX, ESP


	bs << "\x83\xC5\x20"; //ADD EBP,20


	//LOGF_INFO("position przed num_of_params %X", bs.CurrentPosition());
	if(return_param_struct){

		bs << 0x68; //PUSH ???????(int)
		bs.push_int((int)function);

		bs << 0x55; //PUSH EBP //pointer to the first parameter
		bs << 0x68; //PUSH ???????(int)
		bs.push_int(num_of_params);



	}else{
		bs << "\x83\xC5" << num_of_params*4; //ADD EBP, num_of_params

		for(size_t i=0; i<num_of_params; i++){
			bs << "\xFF\x75" << (unsigned char)0x00; //PUSH DWORD PTR SS:[EBP]
			//bs << 0x55; //PUSH EBP
			bs << "\x83\xED\x04"; //SUB EBP,4
		}
	}


	//LOGF_INFO("position przed use_registers %X", bs.CurrentPosition());
	if(use_registers){
		bs << 0x53; //PUSH EBX
	}
	//LOGF_INFO("position po use_registers %X", bs.CurrentPosition());
	bs << "\xFF\xFF\xFF\xFF\xFF"; //Create space for call
	//LOGF_INFO("Creating callpatch from %X to %X", bs.CurrentPosition()-5, callback);
	CallPatch(bs.CurrentPosition()-5, callback); //CALL callback

	bs << 0x61; //POPAD
	//Restore original
	//Wykrzacza sie przy kopiowaniu jmpow i calli



	if(return_to_original){
		//Make room for data
		bs.fillWithByte(0xFF, bkp_size);
		//LOGF_INFO("KOPIOWANIE Z 0x%X DO 0x%X, bkpsize = %d", function,  bs.CurrentPosition()-bkp_size, bkp_size);
		AsmCopy(function, bs.CurrentPosition()-bkp_size, bkp_size);
		//bs.push_data(function, bkp_size);


		bs << "\xFF\xFF\xFF\xFF\xFF"; //Create space for jmp
		JmpPatch(bs.CurrentPosition()-5, (unsigned char *)(function)+5); //jmp original code
	}else{
		bs << 0xC3; //RETN

	}

	JmpPatch(function, bs.StartPosition(), bkp_size - 5);
	//PUSHAD
	//MOV EBP, ESP
	//MOV EBX, ESP
	//ADD EBP, 20 //miejsce na rejestry
	//ADD EBP, 1 //ile argumentow?

	//tyle razy ile argumentow
	//PUSH EBP
	//SUB EBP,4

	//if registers?
	//PUSH EBX //wrzucenie stuktury z rejestrami
	//end

	//CALL callback
	//POPAD //przywracanie rejestrow
	//RETN


}
