#include "main.h"

HMODULE GetModuleHandle32W(const WCHAR* szModule);


int main()
{
	HMODULE hKernel32Dll = GetModuleHandle32W(TEXT("kernel32.dll"));
	
	if (hKernel32Dll != NULL)
		printf("kernel32.dll!0x%X\n", hKernel32Dll);

	HMODULE hImageBase = GetModuleHandle32W(NULL);

	if (hImageBase != NULL)
		printf("ImageBase!0x%X\n", hImageBase);

	getchar();
	return 0;
}


HMODULE GetModuleHandle32W(const WCHAR* szModule)
{
	PEBX *Peb = NULL;
	DWORD dwImageBase = NULL;

	_asm
	{
		// Get the address of PEB
		mov eax, dword ptr fs : [30h]
		mov Peb, eax

		// Get the ImageBase from PEB
		mov eax, dword ptr[eax + 0x08]
		mov dwImageBase, eax
	}

	if (Peb == NULL)
		return NULL;

	if (!szModule)
		return (HMODULE)dwImageBase;

	PEB_LDR_DATAX *pLDR = Peb->Ldr;
	PLDR_DATA_TABLE_ENTRYX CurrentNode = (PLDR_DATA_TABLE_ENTRYX)pLDR->InLoadOrderModuleList.Flink;

	do
	{
		if (!lstrcmpiW(CurrentNode->BaseDllName.Buffer, szModule))
		{
			return (HMODULE)CurrentNode->DllBase;
		}

		CurrentNode = (PLDR_DATA_TABLE_ENTRYX)CurrentNode->InLoadOrderLinks.Flink;

	} while (CurrentNode && CurrentNode->BaseDllName.Buffer != NULL);


	return NULL;
}