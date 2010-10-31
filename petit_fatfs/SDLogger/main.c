/*---------------------------------------------------------------*/
/* Petit FAT file system module test program R0.02 (C)ChaN, 2009 */
/*---------------------------------------------------------------*/

#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "diskio.h"
#include "pff.h"
#include "xitoa.h"
#include "uart.h"


/*---------------------------------------------------------*/
/* Work Area                                               */
/*---------------------------------------------------------*/


char Line[128];		/* Console input buffer */



static
void put_rc (FRESULT rc)
{
	const prog_char *p;
	static const prog_char str[] =
		"OK\0" "DISK_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
		"NOT_OPENED\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0";
	FRESULT i;

	for (p = str, i = 0; i != rc && pgm_read_byte_near(p); i++) {
		while(pgm_read_byte_near(p++));
	}
	xprintf(PSTR("rc=%u FR_%S\n"), (WORD)rc, p);
}



static
void put_drc (BYTE res)
{
	xprintf(PSTR("rc=%d\n"), res);
}



static
void get_line (char *buff, BYTE len)
{
	BYTE c, i;

	i = 0;  
	for (;;) {
		c = uart_get();
		if (c == '\r') break;
		if ((c == '\b') && i) {
			i--;
			uart_put(c);
			uart_put(' ');
			uart_put(c);
			continue;
		}
		if (c >= ' ' && i < len - 1) {	/* Visible chars */
			buff[i++] = c;
			xputc(c);
		}
	}
	buff[i] = 0;
	xputc(c);
  uart_put('\n');
}



static
void put_dump (const BYTE *buff, DWORD ofs, int cnt)
{
	BYTE n;


	xitoa(ofs, 16, -8); xputc(' ');
	for(n = 0; n < cnt; n++) {
		xputc(' ');	xitoa(buff[n], 16, -2); 
	}
	xputs(PSTR("  "));
	for(n = 0; n < cnt; n++)
		xputc(((buff[n] < 0x20)||(buff[n] >= 0x7F)) ? '.' : buff[n]);
	xputc('\n');
}



/*-----------------------------------------------------------------------*/
/* Main                                                                  */


int main (void)
{
	char *ptr;
	long p1, p2;
	BYTE res;
	WORD s1, s2, s3, ofs, cnt, w;
	FATFS fs;			/* File system object */
	DIR dir;			/* Directory object */
	FILINFO fno;		/* File information */

	PORTB = 0b00000001; // Port B
	DDRB  = 0b00000011;

	uart_init();		// Initialize UART driver

	sei();

	xfunc_out = uart_put;
	xputs(PSTR("\nPFF test monitor\n"));

	for (;;) {
		xputc('>');
		get_line(Line, sizeof(Line));
		ptr = Line;

		switch (*ptr++) {
		case '?' :
			xputs(PSTR("di                - Initialize physical drive\n"));
			xputs(PSTR("dd <sector> <ofs> - Dump partial secrtor 128 bytes\n"));
			xputs(PSTR("fi                - Mount the volume\n"));
			xputs(PSTR("fo <file>         - Open a file\n"));
#if _USE_READ
			xputs(PSTR("fd                - Read the file 128 bytes and dump it\n"));
#endif
#if _USE_WRITE
			xputs(PSTR("fw <len> <val>    - Write data to the file\n"));
			xputs(PSTR("fp                - Write console input to the file\n"));
#endif
#if _USE_LSEEK
			xputs(PSTR("fe <ofs>          - Move file pointer of the file\n"));
#endif
#if _USE_DIR
			xputs(PSTR("fl [<path>]       - Directory listing\n"));
#endif
			break;
		case 'd' :
			switch (*ptr++) {
			case 'i' :	/* di - Initialize physical drive */
				res = disk_initialize();
				put_drc(res);
				break;

			case 'd' :	/* dd <sector> <ofs> - Dump partial secrtor 128 bytes */
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2)) break;
				s2 = p2;
				res = disk_readp((BYTE*)Line, p1, s2, 128);
				if (res) { put_drc(res); break; }
				s3 = s2 + 128;
				for (ptr = Line; s2 < s3; s2 += 16, ptr += 16, ofs += 16) {
					s1 = (s3 - s2 >= 16) ? 16 : s3 - s2;
					put_dump((BYTE*)ptr, s2, s1);
				}
				break;
			}
			break;

		case 'f' :
			switch (*ptr++) {

			case 'i' :	/* fi - Mount the volume */
				put_rc(pf_mount(&fs));
				break;

			case 'o' :	/* fo <file> - Open a file */
				while (*ptr == ' ') ptr++;
				put_rc(pf_open(ptr));
				break;
#if _USE_READ
			case 'd' :	/* fd - Read the file 128 bytes and dump it */
				ofs = fs.fptr;
				res = pf_read(Line, sizeof(Line), &s1);
				if (res != FR_OK) { put_rc(res); break; }
				ptr = Line;
				while (s1) {
					s2 = (s1 >= 16) ? 16 : s1;
					s1 -= s2;
					put_dump((BYTE*)ptr, ofs, s2);
					ptr += 16; ofs += 16;
				}
				break;

			case 't' :	/* ft - Type the file data via dreadp function */
				do {
					res = pf_read(0, 32768, &s1);
					if (res != FR_OK) { put_rc(res); break; }
				} while (s1 == 32768);
				break;
#endif
#if _USE_WRITE
			case 'w' :	/* fw <len> <val> - Write data to the file */
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2)) break;
				for (s1 = 0; s1 < sizeof(Line); Line[s1++] = (BYTE)p2) ;
				p2 = 0;
				while (p1) {
					if ((UINT)p1 >= sizeof(Line)) {
						cnt = sizeof(Line); p1 -= sizeof(Line);
					} else {
						cnt = (WORD)p1; p1 = 0;
					}
					res = pf_write(Line, cnt, &w);	/* Write data to the file */
					p2 += w;
					if (res != FR_OK) { put_rc(res); break; }
					if (cnt != w) break;
				}
				res = pf_write(0, 0, &w);		/* Finalize the write process */
				put_rc(res);
				if (res == FR_OK)
					xprintf(PSTR("%lu bytes written.\n"), p2);
				break;

			case 'p' :	/* fp - Write console input to the file */
				xputs(PSTR("Type any line to write. A blank line finalize the write operation.\n"));
				for (;;) {
					get_line(Line, sizeof(Line));
					if (!Line[0]) break;
					strcat(Line, "\r\n");
					res = pf_write(Line, strlen(Line), &w);	/* Write a line to the file */
					if (res) break;
				}
				res = pf_write(0, 0, &w);		/* Finalize the write process */
				put_rc(res);
				break;
#endif
#if _USE_LSEEK
			case 'e' :	/* fe - Move file pointer of the file */
				if (!xatoi(&ptr, &p1)) break;
				res = pf_lseek(p1);
				put_rc(res);
				if (res == FR_OK)
					xprintf(PSTR("fptr = %lu(0x%lX)\n"), fs.fptr, fs.fptr);
				break;
#endif
#if _USE_DIR
			case 'l' :	/* fl [<path>] - Directory listing */
				while (*ptr == ' ') ptr++;
				res = pf_opendir(&dir, ptr);
				if (res) { put_rc(res); break; }
				s1 = 0;
				for(;;) {
					res = pf_readdir(&dir, &fno);
					if (res != FR_OK) { put_rc(res); break; }
					if (!fno.fname[0]) break;
					if (fno.fattrib & AM_DIR)
						xprintf(PSTR("   <DIR>   %s\n"), fno.fname);
					else
						xprintf(PSTR("%9lu  %s\n"), fno.fsize, fno.fname);
					s1++;
				}
				xprintf(PSTR("%u item(s)\n"), s1);
				break;
#endif
			}
			break;
		}
	}

}


