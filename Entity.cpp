#include <Windows.h>
#include <winternl.h>
#include <TlHelp32.h>

#include "Entity.h"
#include "Utlilites.h"
#include "Global.h"

#define QWORD unsigned __int64

HANDLE hProcess;
DWORD peb_addy;
DWORD peb_indx;

typedef struct _PROCESS_BASIC_INFORMATION64 {
	NTSTATUS ExitStatus;
	UINT32 Reserved0;
	uint64_t PebBaseAddress;
	UINT64 AffinityMask;
	UINT32 BasePriority;
	UINT32 Reserved1;
	UINT64 UniqueProcessId;
	UINT64 InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION64;

DWORD get_peb_index(HANDLE hProcess)
{
	typedef NTSTATUS(NTAPI* NtQueryInformationProcessPtr)(
		IN HANDLE ProcessHandle,
		IN PROCESSINFOCLASS ProcessInformationClass,
		OUT PVOID ProcessInformation,
		IN ULONG ProcessInformationLength,
		OUT PULONG ReturnLength OPTIONAL);

	typedef ULONG(NTAPI* RtlNtStatusToDosErrorPtr)(NTSTATUS Status);

	// Locating functions
	HINSTANCE hNtDll = GetModuleHandleW(L"ntdll.dll");
	NtQueryInformationProcessPtr NtQueryInformationProcess = (NtQueryInformationProcessPtr)GetProcAddress(hNtDll, "NtQueryInformationProcess");

	ULONG ReturnLength;
	PROCESS_BASIC_INFORMATION64 info;
	NtQueryInformationProcess(hProcess, ProcessBasicInformation, &info, sizeof(info), &ReturnLength);

	peb_indx = (info.PebBaseAddress >> 12) & 0xF;
	return info.PebBaseAddress;
}

bool GetPid()
{
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(handle, &entry))
	{
		while (Process32Next(handle, &entry))
		{
			if (!wcscmp(entry.szExeFile, L"ModernWarfare.exe"))
			{
				Utils::Log("Process ID: ", entry.th32ProcessID);
				CloseHandle(handle);
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, entry.th32ProcessID);
			}
		}
	}
	peb_addy = get_peb_index(hProcess);
	CloseHandle(hProcess);

	return true;
}

namespace decrypt
{
	namespace _0x318
	{
		constexpr DWORD ENCRYPT_PTR_OFFSET = 0x10764858;
		constexpr DWORD REVERSED_ADDRESS = 0x37F8275;
	}
}

QWORD general_obfuscate(QWORD encryptedPtr, QWORD* curr_decrypt_key)
{
	encryptedPtr = (__int64)encryptedPtr * curr_decrypt_key[0];
	encryptedPtr = encryptedPtr ^ (encryptedPtr >> 0xB);
	encryptedPtr = encryptedPtr ^ (encryptedPtr >> 0x16);
	encryptedPtr = (encryptedPtr >> 0x2C) ^ encryptedPtr;
	encryptedPtr = (__int64)encryptedPtr * curr_decrypt_key[1];
	encryptedPtr = (encryptedPtr >> 0x22) ^ encryptedPtr;
	encryptedPtr = (__int64)encryptedPtr * curr_decrypt_key[2];
	encryptedPtr = encryptedPtr ^ (encryptedPtr >> 0x20);
	encryptedPtr = (__int64)encryptedPtr * curr_decrypt_key[3];
	encryptedPtr = encryptedPtr ^ (encryptedPtr >> 0x21);
	return encryptedPtr;
}

QWORD* decrypt_key_for_entity_t()
{
	static QWORD key[16][4];
	static bool Empty = true;

	if (Empty)
	{
		QWORD reversedAddr, LastKey;
		reversedAddr = Utils::ReadPtr<unsigned long long>({ Globals::pGameBase + decrypt::_0x318::REVERSED_ADDRESS }, false);
		LastKey = Utils::Read<unsigned long long>(_byteswap_uint64(reversedAddr) + 0x1D);

		key[0][0] = LastKey;
		key[0][1] = 0x69CBDD6D2FCFDD7F;
		key[0][2] = 0x906933A7A6454651;
		key[0][3] = 0x576A0C8EF709766F;

		key[1][0] = 0xD99F6CAB2C376BF3;
		key[1][1] = 0x148158D8C7580ABD;
		key[1][2] = LastKey;
		key[1][3] = 0xD57C91B977A23B6D;

		key[2][0] = LastKey;
		key[2][1] = 0xA3C3D120867F9187;
		key[2][2] = 0xB0F113F81623287B;
		key[2][3] = 0xF037D82E18C55A1;

		key[3][0] = 0x9019EA0149459A3F;
		key[3][1] = 0x78CD54516B2A4FA1;
		key[3][2] = LastKey;
		key[3][3] = 0x387AABBFF07D310F;

		key[4][0] = 0xC0718712C6B9E11D;
		key[4][1] = 0x2EB39A5E0974A941;
		key[4][2] = LastKey;
		key[4][3] = 0x85660AE8CDBBA28D;

		key[5][0] = 0xAEB14A674F3306CF;
		key[5][1] = 0x41C47A85E5C188F3;
		key[5][2] = LastKey;
		key[5][3] = 0xE5884703AEBA2E6F;

		key[6][0] = LastKey;
		key[6][1] = 0xAEE38B65C49F056F;
		key[6][2] = 0x157108DFA2FA9501;
		key[6][3] = 0xCD51346A4E8A9AFF;

		key[7][0] = 0xEC56A21C6BAFDC31;
		key[7][1] = LastKey;
		key[7][2] = 0xE8632264C9DDDFC7;
		key[7][3] = 0xA1FD180E465B57C7;

		key[8][0] = 0x3AB48FFF1DC9C449;
		key[8][1] = 0xBF7C9071CE06B19B;
		key[8][2] = 0xE96BE1133DEC06A1;
		key[8][3] = LastKey;

		key[9][0] = 0x85235707D91788E1;
		key[9][1] = 0x102C693153E87933;
		key[9][2] = LastKey;
		key[9][3] = 0x3FE02E494AA4D2DB;

		key[10][0] = 0x4CE3E417110D5817;
		key[10][1] = 0xD063CECE95901849;
		key[10][2] = 0x25814DB616316737;
		key[10][3] = LastKey;

		key[11][0] = LastKey;
		key[11][1] = 0xC068F7CF18FC2F31;
		key[11][2] = 0x196EBBC5929F4C83;
		key[11][3] = 0x71BE721839FA6967;

		key[12][0] = 0x931E2217B131AA7B;
		key[12][1] = 0x7B6C242A4517CACD;
		key[12][2] = LastKey;
		key[12][3] = 0xD32DB622EB32811F;

		key[13][0] = 0x4C0A354A73C1B613;
		key[13][1] = 0xABA3C344D53D8769;
		key[13][2] = 0x7AA0268A1C97EC85;
		key[13][3] = LastKey;

		key[14][0] = LastKey;
		key[14][1] = 0xAAA56DBE6792868B;
		key[14][2] = 0x981EE01DAA06F0AF;
		key[14][3] = 0x835DB861CF84ECCF;

		key[15][0] = LastKey;
		key[15][1] = 0x76D2A7E196713B2F;
		key[15][2] = 0x803485936DDBA421;
		key[15][3] = 0xF095D9C434D2B641;

		Empty = false;
	}

	return (QWORD*)key;
}

QWORD decrypt_for_entity_t()
{
	QWORD BaseAddress, encryptedPtr;

	BaseAddress = Utils::Read<unsigned long long>(Globals::pGameBase + decrypt::_0x318::ENCRYPT_PTR_OFFSET);

	if (BaseAddress)
	{
		encryptedPtr = Utils::Read<unsigned long long>(BaseAddress + 0x10);

		if (encryptedPtr)
		{
			return general_obfuscate(encryptedPtr, &decrypt_key_for_entity_t()[peb_indx << 2]); // peb_addy == crash, peb_indx == 0
		}
	}
	return 0;
}

namespace Entity
{
	cl_entity* GetEntityByID(unsigned long i)
	{
		QWORD decryptedPtr = decrypt_for_entity_t();

		if (decryptedPtr)
		{
			cl_entity* eii;

			eii = (cl_entity*)*(uintptr_t*)(decryptedPtr + (i * 0x318));
			if (eii)
			{
				return eii;
			}
		}
		return NULL;
	}
}


static uint64_t decrypt2(uint64_t v10, uint64_t peb_address, uint64_t entropy)
{
	uint64_t v16, v22, v23, v27, v32, v34, v38, v43, v50, v51, v52, v54, v57, v58, v62, v68, v72, v76, v81, v87;

	switch ((peb_address >> 12) & 0xf)
	{
	case 0:
		v10 *= entropy;
		v16 = (((v10 >> 11) ^ v10) >> 22) ^ (v10 >> 11) ^ v10;
		v10 = (7623430255278022015ULL * ((v16 >> 44) ^ v16) >> 34) ^ 7623430255278022015ULL * ((v16 >> 44) ^ v16);
		v10 = (10405905209158354513ULL * v10 >> 32) ^ 10405905209158354513ULL * v10;
		v10 = (6298860837000541807ULL * v10 >> 33) ^ 6298860837000541807ULL * v10;
		break;
	case 1:
		v22 = (((15681371909964393459ULL * v10 >> 11) ^ 15681371909964393459ULL * v10) >> 22) ^ (15681371909964393459ULL * v10 >> 11) ^ 15681371909964393459ULL * v10;
		v23 = (1477559840834849469ULL * (v22 ^ (v22 >> 44)) ^ (1477559840834849469ULL * (v22 ^ (v22 >> 44)) >> 34)) * entropy;
		v10 = (v23 >> 32) ^ v23;
		v10 = (15383330652952869741ULL * v10 >> 33) ^ 15383330652952869741ULL * v10;
		break;
	case 2:
		v27 = (entropy * v10 >> 11) ^ entropy * v10;
		v10 = (((v27 >> 22) ^ v27) >> 44) ^ (v27 >> 22) ^ v27;
		v10 *= 11800505386266497415ULL;
		v10 ^= v10 >> 34;
		v10 = (12749993976306804859ULL * v10 >> 32) ^ 12749993976306804859ULL * v10;
		v10 *= 1081846336582342049ULL;
		v10 ^= v10 >> 33;
		break;
	case 3:
		v10 *= 10383587707124554303ULL;
		v32 = (((v10 >> 11) ^ v10) >> 22) ^ (v10 >> 11) ^ v10;
		v10 = (v32 >> 44) ^ v32;
		v10 = (8704706363444055969ULL * v10 >> 34) ^ 8704706363444055969ULL * v10;
		v10 = (entropy * v10 >> 32) ^ entropy * v10;
		v10 = (4069754054144504079ULL * v10 >> 33) ^ 4069754054144504079ULL * v10;
		break;
	case 4:
		v10 *= 13867013242363699485ULL;
		v34 = (((v10 >> 11) ^ v10) >> 22) ^ (v10 >> 11) ^ v10;
		v10 = 3365203075252136257ULL * ((v34 >> 44) ^ v34);
		v10 = (v10 ^ (v10 >> 34)) * entropy;
		v10 ^= v10 >> 32;
		v10 = (9612382449669218957ULL * v10 >> 33) ^ 9612382449669218957ULL * v10;
		break;
	case 5:
		v10 *= 12587924241048078031ULL;
		v38 = (((v10 >> 11) ^ v10) >> 22) ^ (v10 >> 11) ^ v10;
		v10 = (v38 >> 44) ^ v38;
		v10 = (4739047423404509427ULL * v10 >> 34) ^ 4739047423404509427ULL * v10;
		v10 = (entropy * v10 >> 32) ^ entropy * v10;
		v10 *= 16539547712660057711ULL;
		v10 ^= v10 >> 33;
		break;
	case 6:
		v43 = (entropy * v10 >> 11) ^ entropy * v10;
		v10 = (((v43 >> 22) ^ v43) >> 44) ^ (v43 >> 22) ^ v43;
		v10 *= 12602069451519493487ULL;
		v10 = (1545025903769851137ULL * ((v10 >> 34) ^ v10) >> 32) ^ 1545025903769851137ULL * ((v10 >> 34) ^ v10);
		v10 *= 14794663882077674239ULL;
		v10 ^= v10 >> 33;
		break;
	case 7:
		v10 *= 17029977283897580593ULL;
		v51 = (((v10 >> 11) ^ v10) >> 22) ^ (v10 >> 11) ^ v10;
		v52 = (v51 ^ (v51 >> 44)) * entropy;
		v10 = (v52 >> 34) ^ v52;
		v50 = 11672512258803193799ULL * ((16745265655772471239ULL * v10 >> 32) ^ 16745265655772471239ULL * v10);
		v10 = (v50 >> 33) ^ v50;
		break;
	case 8:
		v10 *= 4230164275886933065ULL;
		v54 = (((v10 >> 11) ^ v10) >> 22) ^ (v10 >> 11) ^ v10;
		v10 = (v54 >> 44) ^ v54;
		v10 *= 13798062176818606491ULL;
		v10 ^= v10 >> 34;
		v10 *= 16819784706104755873ULL;
		v10 = (v10 ^ (v10 >> 32)) * entropy;
		v10 ^= v10 >> 33;
		break;
	case 9:
		v10 *= 9593607322447874273ULL;
		v57 = (((v10 >> 11) ^ v10) >> 22) ^ (v10 >> 11) ^ v10;
		v10 = (v57 >> 44) ^ v57;
		v58 = (1165422064164174131ULL * v10 ^ (1165422064164174131ULL * v10 >> 34)) * entropy;
		v10 = 4602729711492453083ULL * (v58 ^ (v58 >> 32));
		v10 ^= v10 >> 33;
		break;
	case 10:
		v10 *= 5540522754317309975ULL;
		v62 = (((v10 >> 11) ^ v10) >> 22) ^ (v10 >> 11) ^ v10;
		v10 = (v62 >> 44) ^ v62;
		v10 = (15016072969251199049ULL * v10 >> 34) ^ 15016072969251199049ULL * v10;
		v10 = (2702526695850731319ULL * v10 >> 32) ^ 2702526695850731319ULL * v10;
		v10 *= entropy;
		v10 ^= v10 >> 33;
		break;
	case 11:
		v68 = (entropy * v10 >> 11) ^ entropy * v10;
		v10 = 13864603921709543217ULL * ((((v68 >> 22) ^ v68) >> 44) ^ (v68 >> 22) ^ v68);
		v10 ^= v10 >> 34;
		v10 *= 1832608555629235331ULL;
		v10 = 8196113820238375271ULL * ((v10 >> 32) ^ v10);
		v10 ^= v10 >> 33;
		break;
	case 12:
		v72 = (((10600948058029140603ULL * v10 >> 11) ^ 10600948058029140603ULL * v10) >> 22) ^ (10600948058029140603ULL * v10 >> 11) ^ 10600948058029140603ULL * v10;
		v10 = (v72 >> 44) ^ v72;
		v10 *= 8893523128116300493ULL;
		v10 ^= v10 >> 34;
		v10 = (entropy * v10 >> 32) ^ entropy * v10;
		v10 *= 15217018977045872927ULL;
		v10 ^= v10 >> 33;
		break;
	case 13:
		v10 *= 5479250490535556627ULL;
		v76 = (((v10 >> 11) ^ v10) >> 22) ^ (v10 >> 11) ^ v10;
		v10 = (v76 >> 44) ^ v76;
		v10 = 8836104843527974021ULL * ((12367943702092285801ULL * v10 >> 34) ^ 12367943702092285801ULL * v10);
		v10 ^= v10 >> 32;
		v10 = (entropy * v10 >> 33) ^ entropy * v10;
		break;
	case 14:
		v10 *= entropy;
		v81 = (((v10 >> 11) ^ v10) >> 22) ^ (v10 >> 11) ^ v10;
		v10 = 12296355022153877131ULL * ((v81 >> 44) ^ v81);
		v10 = 10961444961077620911ULL * ((v10 >> 34) ^ v10);
		v10 = (9465924722035584207ULL * ((v10 >> 32) ^ v10) >> 33) ^ 9465924722035584207ULL * ((v10 >> 32) ^ v10);
		break;
	default:
	case 15:
		v87 = (entropy * v10 >> 11) ^ entropy * v10;
		v10 = (((v87 >> 22) ^ v87) >> 44) ^ (v87 >> 22) ^ v87;
		v10 *= 8562090428918217519ULL;
		v10 ^= v10 >> 34;
		v10 *= 9238155603893527585ULL;
		v10 ^= v10 >> 32;
		v10 *= 17336001777355634241ULL;
		v10 ^= v10 >> 33;
		break;
	}
	return v10;
}

uint64_t Entity::GetCGEntitys()
{
	const auto peb_address = (uint64_t)peb_addy;
	const auto variant = ((uint64_t)peb_address >> 12) & 0xf;

	uint64_t entropy_ptr, entropy_val, v1, v10;

	entropy_ptr = *(uint64_t*)(Globals::pGameBase + 0x3741258);
	if (!entropy_ptr)
		return 0;

	entropy_ptr = _byteswap_uint64(entropy_ptr);

	entropy_val = *(uint64_t*)(entropy_ptr + 29);
	if (!entropy_val)
		return 0;

	v1 = *(uint64_t*)((uintptr_t)Globals::pGameBase + 0x106a10c8);
	if (!v1)
		return 0;

	v10 = *(uint64_t*)(v1 + 0x10);
	if (!v10)
		return 0;

	return decrypt2(v10, peb_address, entropy_val);
}

std::vector<cl_entity*> Entity::GetMaxEntitys()
{
	std::vector<cl_entity*> vEntityVec;
	auto Cg_Entitys = GetCGEntitys();
	int index = 0;
	cl_entity* pEntity = (cl_entity*)(Cg_Entitys + 0x318 * index);
	while (pEntity)
	{
		index++;
		pEntity = (cl_entity*)(Cg_Entitys + 0x318 * index);
		if (!pEntity)
			break;
		else if (pEntity->type == 1 && pEntity->valid & 1)
			vEntityVec.push_back(pEntity);
	}

	return vEntityVec;
}

class clientinfo_t
{
public:
	unsigned long m_entity_index;  //this+0x0
	BOOL m_is_valid;               //this+0x4
	char pad_0x8[0x4];
	DWORD m_TeamId; //this+0xC
	char pad_0x10[0x7E0];
	QWORD m_PosInfoPtr; //this+0x7F0
};//size == 0x3588

namespace decrypt
{
	namespace _0x3588
	{
		DWORD ENCRYPT_PTR_OFFSET = 0x10765B88;
		DWORD REVERSED_ADDRESS = 0x37F82CA;
		DWORD BASE_OFFSET = 0x970B8;
		DWORD CAMERA_INFO_OFFSET = 0x2128;
		DWORD LOCAL_INDEX_OFFSET = 0x700A0;
	}
}

QWORD* decrypt_key_for_clientinfo_t()
{
	static QWORD key[4];

	static bool Empty = true;

	if (Empty)
	{
		QWORD reversedAddr, LastKey;
		reversedAddr = *(QWORD*)(Globals::pGameBase + decrypt::_0x3588::REVERSED_ADDRESS);
		LastKey = *(QWORD*)(_byteswap_uint64(reversedAddr) + 0x1B);
		key[0] = 0x306AE931CF3763CB;
		key[1] = 0x223ED5787CCC790D;
		key[2] = LastKey;
		key[3] = 0x3C1F8A7A912886FF;
		Empty = false;
	}

	return (QWORD*)key;
}

QWORD decrypt_clientinfo_t()
{
	QWORD encryptedPtr = *(QWORD*)(Globals::pGameBase + decrypt::_0x3588::ENCRYPT_PTR_OFFSET);

	if (encryptedPtr)
	{
		return general_obfuscate(encryptedPtr, decrypt_key_for_clientinfo_t());
	}

	return 0;
}

clientinfo_t* GetClientInfo(unsigned long i)
{
	QWORD decryptedPtr = decrypt_clientinfo_t();

	if (decryptedPtr)
	{
		QWORD BaseAddress = *(QWORD*)(decryptedPtr + decrypt::_0x3588::BASE_OFFSET);

		if (BaseAddress)
		{
			return (clientinfo_t*)(BaseAddress + (i * 0x3588));
		}
	}

	return nullptr;
}

int Entity::GetTeamID(unsigned long i)
{
	auto cInfo = GetClientInfo(i);
	return cInfo->m_TeamId;
}

unsigned long Entity::GetLocalIndex()
{
	QWORD decryptedPtr = decrypt_clientinfo_t();

	if (decryptedPtr)
	{
		QWORD BaseAddress = *(QWORD*)(decryptedPtr + decrypt::_0x3588::LOCAL_INDEX_OFFSET);

		if (BaseAddress)
		{
			return *(DWORD*)(BaseAddress + 0x1DC);
		}
	}
	return -1;//failed
}

struct name_t
{
	DWORD m_entity_index;
	char m_szName[36];
};

namespace decrypt
{
	constexpr DWORD NAME_ARRAY_OFFSET = 0x107730E8;
	constexpr DWORD NAME_LIST_OFFSET = 0x4C70;
}

//
//std::string GetName(unsigned long i)
//{
//	QWORD Ptr;
//	if (Utils(Globals::pGameBase + decrypt::NAME_ARRAY_OFFSET, &Ptr, sizeof(Ptr)) && Ptr)
//	{
//		name_t name;
//		if (read::read(Ptr + decrypt::NAME_LIST_OFFSET + ((i + (i * 8)) << 0x4), &name, sizeof(name)))
//		{
//			int length = strlen(name.m_szName);
//			for (int j = 0; j < length; ++j)
//			{
//				char ch = name.m_szName[j];
//				bool is_english = ch >= 0 && ch <= 127;/*(ch >= 0 && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= 48 && ch <= 57)*/;
//				if (!is_english)
//					return "Player";
//			}
//			return name.m_szName;
//		}
//	}
//	return "Player";
//}