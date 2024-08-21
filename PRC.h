#pragma once

#include "Forest.h" // This library comes from my github page as well, check it out at: https://github.com/josephbrc/Forest
#include <ctime>
#include <intrin.h>

#define PRC_KEY 0xD5FFC153B94F6ED4 // Global PRC key, change it up or make it a dynamic key for best results.

// Bitwise defines

#define LODWORD(x)  (*((unsigned int*)&(x)))
#define HIDWORD(x)  (((x) >> 0x20) & 0xFFFFFFFF)

typedef          __int64 ll;
typedef unsigned __int64 ull;

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef          char   int8;
typedef   signed char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef   signed short  sint16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef   signed int    sint32;
typedef unsigned int    uint32;
typedef ll              int64;
typedef ll              sint64;
typedef ull             uint64;

// rotate left
template<class T> T __ROL__(T value, int count)
{
	const uint nbits = sizeof(T) * 8;

	if (count > 0)
	{
		count %= nbits;
		T high = value >> (nbits - count);
		if (T(-1) < 0) // signed value
			high &= ~((T(-1) << count));
		value <<= count;
		value |= high;
	}
	else
	{
		count = -count % nbits;
		T low = value << (nbits - count);
		value >>= count;
		value |= low;
	}
	return value;
}

inline uint8  __ROL1__(uint8  value, int count) { return __ROL__((uint8)value, count); }
inline uint16 __ROL2__(uint16 value, int count) { return __ROL__((uint16)value, count); }
inline uint32 __ROL4__(uint32 value, int count) { return __ROL__((uint32)value, count); }
inline uint64 __ROL8__(uint64 value, int count) { return __ROL__((uint64)value, count); }
inline uint8  __ROR1__(uint8  value, int count) { return __ROL__((uint8)value, -count); }
inline uint16 __ROR2__(uint16 value, int count) { return __ROL__((uint16)value, -count); }
inline uint32 __ROR4__(uint32 value, int count) { return __ROL__((uint32)value, -count); }
inline uint64 __ROR8__(uint64 value, int count) { return __ROL__((uint64)value, -count); }

class InlineDataDummy {
public:
	InlineDataDummy() {}
};

class PRCManager {
private:
	InlineDataDummy* Inline;
	uint64_t* m_Offsets = new uint64_t[100];
	std::vector<uint64_t> m_Allocations;
	uint64_t m_Key;
	HANDLE g_Heap;
public:
	PRCManager(uint64_t Key) { // Default constructor
		std::srand(std::time(nullptr));
		m_Key = (Key ^ __ROR4__(std::rand(), 0x3574)) * 0x5A5A5A5A5A5A5A;
		Inline = new InlineDataDummy();
		g_Heap = Forest::Call<HANDLE, DWORD, SIZE_T, SIZE_T>("kernel32.dll", "HeapCreate", 0, 0, 0);
	}
	template<typename T>
	__forceinline T Make(T Data) {
		T allocated = reinterpret_cast<T>(Forest::Call<LPVOID, HANDLE, DWORD, SIZE_T>("ntdll.dll", "RtlAllocateHeap", g_Heap, NULL, sizeof(typename std::remove_pointer<T>::type)));
		new (allocated) typename std::remove_pointer<T>::type(*Data);
		m_Allocations.push_back(((uint64_t)allocated - (uint64_t)Inline) ^ __ROR8__(m_Key, 9));
		return allocated;
	}
	template<typename T>
	__forceinline void Add(T Data, int Idx) {
		m_Offsets[Idx] = ((uint64_t)Make<T>(Data) - (uint64_t)Inline) ^ __ROR8__(m_Key, 9);
		delete Data; // Frees original class since we don't need to use it anyways.
	}
	template<typename T>
	__forceinline T Get(int Idx) {
		uint64_t offset = m_Offsets[Idx] ^ __ROR8__(m_Key, 9);
		return reinterpret_cast<T>((uint64_t)Inline + offset);
	}
};

class PRCDummyData {
public:
	PRCDummyData() {
	}
	uint64_t m_Key;
	char pad[0x35A];
	PRCManager* m_PRC;
};

static inline PRCDummyData* DummyPRC = new PRCDummyData(); // Dummy PRC manager global

static PRCManager* GetPRCManager() {

	uint64_t retaddr = (uint64_t)_ReturnAddress();
	unsigned char opcode = *(unsigned char*)(retaddr - 5);
	auto is_address_within = [&](uint64_t module_addr, uint64_t addr) // Simple address validation lambda
	{
			if (!module_addr) return FALSE;
			uint64_t Start = module_addr;
			PIMAGE_NT_HEADERS Nt = Forest::Call<PIMAGE_NT_HEADERS, PVOID>("ntdll.dll", "RtlImageNtHeader", (PVOID)module_addr);
			uint64_t End = Start + Nt->OptionalHeader.SizeOfImage;
			if (addr >= Start && addr <= End)
			{
				return TRUE;
			}
			return FALSE;
	};

	char* retaddr_c = (char*)retaddr; // Return address
	char* func_frame = &retaddr_c[*((int*)retaddr_c - 1)]; // Function frame/stack info (TODO)
	if (!is_address_within(Forest::GetFirstModule(), retaddr)) {
		*(char*)(0x0) = 0x3467334434Fi64; // Manual exit
	}

	if (!DummyPRC->m_PRC) {
		DummyPRC->m_PRC = new PRCManager(PRC_KEY); // Template key
	}

	return DummyPRC->m_PRC;

}
