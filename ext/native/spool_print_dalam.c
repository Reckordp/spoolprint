#include <windows.h>
#include <winspool.h>
#include <stdio.h>

int spoolprint_dalam(char* namaJob, byte* resi, size_t panjang)
{
  CHAR namaPrinter[1024];
  DWORD ukuran;

  *namaPrinter = 0;
  ukuran = 1024;
  GetDefaultPrinter((LPSTR)namaPrinter, (LPDWORD)&ukuran);

  DOC_INFO_1 docInfo;
  HANDLE kendali;
  DWORD tertulis;

  ZeroMemory(&docInfo, sizeof(docInfo));
  docInfo.pDocName = namaJob;
  docInfo.pDatatype = "RAW";

  OpenPrinter(namaPrinter, &kendali, NULL);
  StartDocPrinter(kendali, 1, (LPBYTE)&docInfo);
  StartPagePrinter(kendali);
  WritePrinter(kendali, resi, panjang, &tertulis);
  EndPagePrinter(kendali);
  EndDocPrinter(kendali);
  ClosePrinter(kendali);
  return tertulis;
}
