PVOID(__fastcall* FunctionPTR)(PVOID a1, unsigned int a2, PVOID a3, unsigned int a4, PVOID a5);

typedef struct _MEMORY_STRUCT
{
	BYTE type;
	LONG usermode_pid;
	LONG target_pid;
	ULONG64 base_address;
	void* address;
	LONG size;
	void* output;
	ULONG magic;
} MEMORY_STRUCT, * PMEMORY_STRUCT;
template<typename ... A>
uint64_t call_driver_control(void* control_function, const A ... arguments)
{
	if (!control_function)
		return 0;

	using tFunction = uint64_t(__stdcall*)(A...);
	const auto control = static_cast<tFunction>(control_function);

	return control(arguments ...);
}
void* kernel_control_function()
{
	HMODULE hModule = LoadLibrary((L"win32u.dll"));

	if (!hModule)
		return nullptr;

	void* func = reinterpret_cast<void*>(GetProcAddress(hModule, ("NtUserGetObjectInformation")));

	*(PVOID*)&FunctionPTR = GetProcAddress(
		GetModuleHandle(L"win32u.dll"),
		"NtUserGetObjectInformation"
	);


	return func;
}
void* m_driver_control;
DWORD64 m_pid;
DWORD64 usermode_pid;
DWORD64 m_base;
DWORD64 m_unityplayer;
DWORD64 m_gameassembly;

DWORD64 GetBaseAddress()
{
	MEMORY_STRUCT memory_struct = { 0 };
	memory_struct.type = 17;
	memory_struct.magic = 0x1337;
	memory_struct.usermode_pid = m_pid;

	FunctionPTR(NULL, NULL, (PVOID)&memory_struct, NULL, NULL);
	return memory_struct.base_address;
}

template <typename T>
T read(uintptr_t address)
{
	T buffer{ };
	MEMORY_STRUCT memory_struct = { 0 };
	memory_struct.type = 3;
	memory_struct.magic = 0x1337;
	memory_struct.usermode_pid = usermode_pid;
	memory_struct.target_pid = m_pid;
	memory_struct.address = reinterpret_cast<void*>(address);
	memory_struct.output = &buffer;
	memory_struct.size = sizeof(T);

	FunctionPTR(NULL, NULL, (PVOID)&memory_struct, NULL, NULL);
	return buffer;
}

//template<typename T>
//T write(uint64_t address, T buffer)
//{
//	MEMORY_STRUCT memory_struct = { 0 };
//	memory_struct.type = 7;
//	memory_struct.magic = 0x1337;
//	memory_struct.usermode_pid = usermode_pid;
//	memory_struct.target_pid = m_pid;
//	memory_struct.address = reinterpret_cast<void*>(address);
//	memory_struct.size = sizeof(T);
//	memory_struct.output = &buffer;
//
//	FunctionPTR(NULL, NULL, (PVOID)&memory_struct, NULL, NULL);
//	return true;
//}