/*
$Id: db.h,v 1.36 2005/05/12 15:41:04 karman Exp $

    This file is part of Swish-e.

    Swish-e is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Swish-e is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along  with Swish-e; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
    
    See the COPYING file that accompanies the Swish-e distribution for details
    of the GNU GPL and the special exception available for linking against
    the Swish-e library.
    
** Mon May  9 18:19:34 CDT 2005
** added GPL


**
** 2001-01  jose   initial coding
**
*/

#ifndef __HasSeenModule_DB
#define __HasSeenModule_DB    1

/* Possible Open File modes */
typedef enum {
    DB_CREATE,
	DB_READ,
	DB_READWRITE
}
DB_OPEN_MODE;

/* struct to handle a linked list of wordID */
/* a unique word can contain multiple wordID if update mode is used */
/* There can be a wordID for each indexed chunk */
typedef struct DB_WORDID {
    sw_off_t  wordID;
    struct DB_WORDID *next;
} DB_WORDID;

void initModule_DB (SWISH *);
void freeModule_DB (SWISH *);

void    write_header(SWISH *ws, int merged_flag );
void    update_header(SWISH *, void *, int, int );
void    write_index(SWISH *, IndexFILE *);
void    write_word(SWISH *, ENTRY *, IndexFILE *);
void    build_worddata(SWISH *, ENTRY *);
void    write_worddata(SWISH *, ENTRY *, IndexFILE *);
sw_off_t    read_worddata(SWISH * sw, ENTRY * ep, IndexFILE * indexf, unsigned char **bufer, int *sz_buffer);
void    write_pathlookuptable_to_header(SWISH *, int id, INDEXDATAHEADER *header, void *DB);
void    write_MetaNames (SWISH *, int id, INDEXDATAHEADER *header, void *DB);
int     write_integer_table_to_header(SWISH *, int id, int table[], int table_size, void *DB);

void    read_header(SWISH *, INDEXDATAHEADER *header, void *DB);

void    parse_MetaNames_from_buffer(INDEXDATAHEADER *header, char *buffer);
void    parse_pathlookuptable_from_buffer(INDEXDATAHEADER *header, char *buffer);
void    parse_integer_table_from_buffer(int table[], int table_size, char *buffer);
char    *getfilewords(SWISH *sw, int, IndexFILE *);
void    setTotalWordsPerFile(IndexFILE *,int ,int );

int     getTotalWordsInFile(IndexFILE *indexf, int filenum);


/* Common DB api */
void   *DB_Create (SWISH *sw, char *dbname);
void   *DB_Open (SWISH *sw, char *dbname, int mode);
void    DB_Close(SWISH *sw, void *DB);
void    DB_Remove(SWISH *sw, void *DB);

int     DB_InitWriteHeader(SWISH *sw, void *DB);
int     DB_EndWriteHeader(SWISH *sw, void *DB);
int     DB_WriteHeaderData(SWISH *sw, int id, unsigned char *s, int len, void *DB);

int     DB_InitReadHeader(SWISH *sw, void *DB);
int     DB_ReadHeaderData(SWISH *sw, int *id, unsigned char **s, int *len, void *DB);
int     DB_EndReadHeader(SWISH *sw, void *DB);

int     DB_InitWriteWords(SWISH *sw, void *DB);
sw_off_t    DB_GetWordID(SWISH *sw, void *DB);
int     DB_WriteWord(SWISH *sw, char *word, sw_off_t wordID, void *DB);
int     DB_WriteWordHash(SWISH *sw, char *word, sw_off_t wordID, void *DB);
long    DB_WriteWordData(SWISH *sw, sw_off_t wordID, unsigned char *worddata, int data_size, int saved_bytes, void *DB);
int     DB_EndWriteWords(SWISH *sw, void *DB);

int     DB_InitReadWords(SWISH *sw, void *DB);
int     DB_ReadWord(SWISH *sw, char *word, DB_WORDID **wordID, void *DB);
int     DB_ReadFirstWordInvertedIndex(SWISH *sw, char *word, char **resultword, DB_WORDID **wordID, void *DB);
int     DB_ReadNextWordInvertedIndex(SWISH *sw, char *word, char **resultword, DB_WORDID **wordID, void *DB);
long    DB_ReadWordData(SWISH *sw, sw_off_t wordID, unsigned char **worddata, int *data_size, int *saved_bytes, void *DB);
int     DB_EndReadWords(SWISH *sw, void *DB);


int     DB_InitWriteSortedIndex(SWISH *sw, void *DB );
int     DB_WriteSortedIndex(SWISH *sw, int propID, unsigned char *data, int sz_data,void *DB);

int     DB_EndWriteSortedIndex(SWISH *sw, void *DB);
 
int     DB_InitReadSortedIndex(SWISH *sw, void *DB);
int     DB_ReadSortedIndex(SWISH *sw, int propID, unsigned char **data, int *sz_data,void *DB);

/* this is defined in db_native.h now 
int     DB_ReadSortedData(SWISH *sw, int *data,int index, int *value, void *DB);
*/
#define  DB_ReadSortedData(data, index) (data[index])




int     DB_EndReadSortedIndex(SWISH *sw, void *DB);


int     DB_WriteFileNum(SWISH *sw, int filenum, unsigned char *filedata,int sz_filedata, void *DB);
int     DB_ReadFileNum(SWISH *sw, unsigned char *filedata, void *DB);
int     DB_CheckFileNum(SWISH *sw, int filenum, void *DB);
int     DB_RemoveFileNum(SWISH *sw, int filenum, void *DB);

int     DB_InitWriteProperties(SWISH *sw, void *DB);
void    DB_WriteProperty( SWISH *sw, IndexFILE *indexf, FileRec *fi, int propID, char *buffer, int buf_len, int uncompressed_len, void *db);
void    DB_WritePropPositions(SWISH *sw, IndexFILE *indexf, FileRec *fi, void *db);
void    DB_ReadPropPositions(SWISH *sw, IndexFILE *indexf, FileRec *fi, void *db);
char   *DB_ReadProperty(SWISH *sw, IndexFILE *indexf, FileRec *fi, int propID, int *buf_len, int *uncompressed_len, void *db);
void    DB_Reopen_PropertiesForRead(SWISH *sw, void *DB);

int    DB_WriteTotalWordsPerFile(SWISH *sw, int idx, int wordcount, void *DB);
int    DB_ReadTotalWordsPerFile(SWISH *sw, int idx, int *wordcount, void *DB);


struct MOD_DB
{
    char *DB_name; /* short name for data source */
};


#endif
