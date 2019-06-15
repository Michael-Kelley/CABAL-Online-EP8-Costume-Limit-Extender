#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

#include <cstdint>
#include <string>
#include <WinSock2.h>

// DO NOT ENABLE THIS! THE CODE FOR THIS DOES NOT WORK YET.
#define EXTENDED_HOOK 0

const std::uintptr_t find_costume_mesh_addr = 0xC8CC0U;
const std::uintptr_t ech_read_meshes_addr = 0xDA190U;

std::uintptr_t base_addr = 0;
std::uintptr_t find_costume_mesh_jmp_addr = 0;

#if EXTENDED_HOOK
std::uintptr_t display_code_jmp_addr = 0;
std::uintptr_t display_code_pt1 = 0;

__declspec(naked) void display_code_switch() {
	__asm {
		movsx edx, [esp + 40]	// displaycode_pt1
		mov display_code_pt1, edx
	}

	switch (display_code_pt1) {
		case 'z':	// Martial gear
			__asm {
				mov edx, [ebp + 232]	// mesh->flags
				and edx, 0xFFFFF03F
			}
			break;
		case 'a':	// Armour gear
			__asm {
				mov edx, [ebp + 232]
				and edx, 0xFFFFF0BF
				or edx, 0x80
			}
			break;
		case 'm':	// Battle gear
			__asm {
				mov edx, [ebp + 232]
				and edx, 0xFFFFF07F
				or edx, 0x40
			}
			break;
		case 'b':	// dafuq is this?
			__asm {
				mov edx, [ebp + 232]
				and edx, 0xFFFFF0FF
				or edx, 0xC0
			}
			break;
		case 's':	// Everything from here on is costumes
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
			__asm {
				mov edx, [ebp + 232]
				and edx, 0xFFFFF0FF
				or edx, 0xC0
				mov esi, display_code_pt1
				sub esi, 115
				shl esi, 4
			}
			break;
		case 'A':	// New costume codes!
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
			__asm {
				mov edx, [ebp + 232]
				and edx, 0xFFFFF0FF
				or edx, 0xC0
				mov esi, display_code_pt1
				sub esi, 58
				shl esi, 4
			}
			break;
		default:
			__asm {
				mov edx, [ebp + 232]
				or edx, 0xFC0
			}
			break;
	}

	__asm {
		mov[ebp + 232], edx
		jmp[display_code_jmp_addr]
	}
}
#endif

__declspec(naked) void check_mesh_flag() {
	__asm {
		and eax, 0x1FF
		sub eax, 0xC0
		cmp eax, edx
		jmp[find_costume_mesh_jmp_addr]
	}
}

void hook() {
	if (base_addr != 0)
		return;

	base_addr = reinterpret_cast<decltype(base_addr)>(GetModuleHandle(nullptr));
	find_costume_mesh_jmp_addr = base_addr + find_costume_mesh_addr + 0x74U;

	auto old = 0UL;

	// ECH::ReadMeshes patches
	auto addr = reinterpret_cast<uint8_t*>(base_addr + ech_read_meshes_addr);
	VirtualProtect(addr, 8, PAGE_EXECUTE_READWRITE, &old);

	*reinterpret_cast<uint32_t*>(addr) = 0x90909090;	// nop, nop, nop, nop
	*reinterpret_cast<uint16_t*>(addr + 4) = 0x9090;	// nop, nop
	addr[6] = 0x90;										// nop

	VirtualProtect(addr, 8, old, &old);

#if EXTENDED_HOOK
	// ECH::ReadMeshes once more (switch-case replacement)
	addr = reinterpret_cast<uint8_t*>(base_addr + ech_read_meshes_addr - 0x1E4U);
	VirtualProtect(addr, 8, PAGE_EXECUTE_READWRITE, &old);

	addr[0] = 0xE9;		// jmp
	*reinterpret_cast<uint32_t*>(addr + 1) =
		reinterpret_cast<uint32_t>(&display_code_switch) - (base_addr + ech_read_meshes_addr - 0x1DFU);	// Address of display_code_switch
	
	VirtualProtect(addr, 8, old, &old);

	// Set the address for display_code_switch return jump (stupid msvc inline assembler bullshit...)
	display_code_jmp_addr = base_addr + ech_read_meshes_addr - 0xA9U;
#endif

	// FindCostumeMesh patches
	addr = reinterpret_cast<uint8_t*>(base_addr + find_costume_mesh_addr + 3);
	VirtualProtect(addr, 48, PAGE_EXECUTE_READWRITE, &old);

	*reinterpret_cast<uint16_t*>(&addr[2]) = 0x01FF;
	addr[8] = 0xBF;
	addr[12] = 0x7E;	// jle
	addr[24] = 0xE9;	// jmp
	*reinterpret_cast<uintptr_t*>(&addr[25])
		= reinterpret_cast<uintptr_t>(&check_mesh_flag) - (base_addr + find_costume_mesh_addr + 32U);	// Address of check_mesh_flag

	VirtualProtect(addr, 48, old, &old);
}

extern "C" __declspec(dllexport) void blue() {
	hook();
}

// Doesn't work when called from DllMain...
/*BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH)
		hook();
}*/