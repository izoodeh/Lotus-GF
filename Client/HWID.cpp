#include "stdafx.h"
#include "hwid.h"
#include "VirtualizerSDK.h"
#include "MD5.h"
# pragma comment(lib, "wbemuuid.lib")

const TCHAR* g_CPath = _T("C:\\");

unsigned long int crc32_tab[256] =
{
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
  0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
  0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
  0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
  0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
  0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
  0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
  0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
  0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
  0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
  0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
  0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
  0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
  0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
  0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
  0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
  0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
  0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
  0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
  0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
  0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
  0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
  0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
  0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
  0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
  0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
  0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
  0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
  0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
  0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
  0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
  0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
  0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
  0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
  0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
  0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
  0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
  0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
  0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
  0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
  0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
  0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
  0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};
 
UINT CHWID::CRC32(wstring hash)
{
	UINT crc32 = 0;
	size_t i = 0, n = 0;
	crc32 = 0xffffffff;

	for(UINT n=0;n<hash.size();n++)
	{
		crc32 = (crc32 >> 8) ^ crc32_tab[(crc32 & 0xff) ^ (BYTE)hash[n]];
	}

	crc32 ^= 0xffffffff;
	return crc32;
}

UINT CHWID::CountDig(wstring str)
{
	UINT digs = 0;
	for(UINT n=0;n<str.size();n++)
	{
		if(str[n] >= L'0' && str[n] <= L'9')
		{
			digs++;
		}
	}
	return digs;
}


BYTE root_cimv2[11] = { 82, 78, 76, 82, 86, 76, 92, 81, 114, 31, 55};
BYTE wql[4] = { 87, 80, 79, 6};
BYTE disk_drive[30] = { 83, 68, 79, 67, 73, 91, 53, 54, 4, 107, 101, 13, 3, 123, 62, 17, 230, 170, 153, 225, 150, 97, 109, 127, 104, 55, 54, 12, 243, 179};
BYTE signature[20] = { 83, 1, 106, 6, 109, 15, 123, 28, 69, 45, 67, 66, 59, 91, 27, 120, 237, 153, 171, 190};
BYTE physical_memory[35] = { 83, 68, 79, 67, 73, 91, 53, 54, 4, 107, 101, 13, 3, 123, 62, 17, 230, 170, 153, 225, 130, 96, 103, 103, 69, 38, 62, 22, 219, 214, 188, 126, 98, 72, 83};
BYTE speed[12] = { 83, 1, 115, 6, 111, 15, 112, 28, 64, 45, 55, 66};	
BYTE capacity[18] = { 67, 1, 98, 6, 122, 15, 116, 28, 71, 45, 94, 66, 58, 91, 16, 120, 136, 153};
BYTE bios[25] = { 83, 68, 79, 67, 73, 91, 53, 54, 4, 107, 101, 13, 3, 123, 62, 17, 230, 170, 153, 225, 144, 65, 81, 71, 44};
BYTE bios_version[36] = { 83, 1, 78, 6, 72, 15, 92, 28, 107, 45, 100, 66, 12, 91, 32, 120, 199, 153, 248, 190, 132, 8, 123, 20, 94, 69, 44, 122, 255, 179, 190, 17, 126, 49, 83, 118};
BYTE version[16] = { 86, 1, 102, 6, 120, 15, 102, 28, 77, 45, 88, 66, 32, 91, 105, 120};
BYTE install_date[24] = { 73, 1, 109, 6, 121, 15, 97, 28, 69, 45, 91, 66, 34, 91, 45, 120, 233, 153, 223, 190, 183, 8, 30, 20};
BYTE serial_number[13] = { 0x53, 0x64, 0x71, 0x6f, 0x6b, 0x63, 0x5b, 0x69, 0x49, 0x4f, 0x52, 0x30, 0x4e};
BYTE processor[30] = { 0x53, 0x64, 0x6f, 0x63, 0x69, 0x7b, 0x35, 0x36, 0x4, 0x6b, 0x45, 0x2d, 0x23, 0x7b, 0x3e, 0x11, 0xe6, 0xaa, 0x99, 0xe1, 0x82, 0x7a, 0x71, 0x77, 0x49, 0x36, 0x2c, 0x15, 0xe4, 0xb3};
BYTE processId[12] = { 0x50, 0x73, 0x6c, 0x65, 0x6f, 0x7c, 0x66, 0x73, 0x56, 0x64, 0x53, 0x42};

CHWID::CHWID() : m_initialized(FALSE)
{
	m_id = 0;
	memset(m_hash, 0, 32);
}

CHWID::~CHWID()
{
}

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

    //IsWow64Process is not available on all supported versions of Windows.
    //Use GetModuleHandle to get a handle to the DLL that contains the function
    //and GetProcAddress to get a pointer to the function if available.

    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    if(NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            //handle error
        }
    }
    return bIsWow64;
}

#pragma optimize("", off)

void CHWID::Init()
{
	VIRTUALIZER_START;

	if(!m_initialized)
	{
		
		if(g_CPath[0] != L'C' && g_CPath[1] != L':')
		{
			//crash
			LPDWORD lpD = (LPDWORD)0x00;
			(*lpD) = 0x661;
		}
		vector<wstring> hddSerialNumber;
		vector<wstring> processorId;

		SYSTEM_INFO si;
		if(IsWow64())
		{
			GetNativeSystemInfo(&si);
		}else
		{
			GetSystemInfo(&si);
		}
		wstringstream cpuStream;
		cpuStream << hex << si.dwProcessorType << hex << si.wProcessorArchitecture << hex << si.wProcessorLevel << hex << si.wProcessorRevision << hex << si.dwNumberOfProcessors;
		processorId.push_back(cpuStream.str());


		TCHAR volumeName[MAX_PATH + 1] = { 0 };
		TCHAR fileSystemName[MAX_PATH + 1] = { 0 };
		DWORD serialNumber = 0;
		if (GetVolumeInformation(
			g_CPath,
			volumeName,
			ARRAYSIZE(volumeName),
			&serialNumber,
			0,
			0,
			fileSystemName,
			ARRAYSIZE(fileSystemName)))
		{
			wstringstream tempStream;
			tempStream << hex << serialNumber;
			wstring serial;
			tempStream >> serial;
			hddSerialNumber.push_back(serial);
		}else
		{
			hddSerialNumber.push_back(L"DF012142");
		}

		wstringstream hashStream;
		if(hddSerialNumber.size() > 0)
		{
			UINT index = 0;
			bool set = false;
			while(!set && index < hddSerialNumber.size())
			{
				if(CountDig(hddSerialNumber[index]) > 1)
				{
					hashStream << hddSerialNumber[index];
					set = true;
				}
				index++;
			}
		}

		WCHAR computerName[255];
		memset(computerName, 0, sizeof(computerName));
		DWORD nameLen = 255;
		if(GetComputerName(computerName, &nameLen))
		{
			hashStream << L"-" << hex << CRC32(computerName);
		}
		if(processorId.size() > 0)
		{
			hashStream << L"-" << hex << processorId[0];
		}

		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof (statex);
		if(GlobalMemoryStatusEx (&statex))
		{
			UINT64 totalRam = statex.ullTotalPhys;
			totalRam /= 1024;
			hashStream << L"-" << hex << totalRam;
		}

		wstring wHash = hashStream.str();
		string hash(wHash.size(), 0);
		for(UINT n=0;n<wHash.size();n++)
		{
			hash[n] = static_cast<char>(wHash[n]);
		}

		string md5Hash = md5(hash);
		for(UINT n=0;n<md5Hash.size();n++)
		{
			if(n < 32)
			{
				m_hash[n] = md5Hash[n];
			}
		}
		
		m_id = CRC32(hashStream.str());
		m_initialized = TRUE;

	}
	VIRTUALIZER_END;
}

#pragma optimize("", on)