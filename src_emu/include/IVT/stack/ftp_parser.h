/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    ftp_parser.h
Abstract:
	This module defines all the functions used in the file "ftp_parser.c".
Author:
    Luo hongbo
Revision History:
	2002.5		created
---------------------------------------------------------------------------*/
#ifndef FTP_PARSER_H
#define FTP_PARSER_H

typedef DWORD FTP_DATA_HDL ;		/* XML parser handle */
#define INVALID_FTP_DATA_HDL 0		/* invalid XML parser handle */

FTP_DATA_HDL  FTP_ParseFirstFile(FILE_HDL file_hdl,FTP_FIND_DATA* find_data);
int FTP_ParseNextFile(FTP_DATA_HDL hdl, FILE_HDL file_hdl, FTP_FIND_DATA* find_data);
void FTP_ParseClose(FTP_DATA_HDL hdl);

#endif
