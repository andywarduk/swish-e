/*
**
** This program and library is free software; you can redistribute it and/or
** modify it under the terms of the GNU (Library) General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU (Library) General Public License for more details.
**
** You should have received a copy of the GNU (Library) General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
**
**
** 2001-05-07 jmruiz init coding
**
*/

#include "swish.h"
#include "merge.h"
#include "docprop.h"
#include "hash.h"
#include "string.h"
#include "mem.h"
#include "db.h"
#include "compress.h"
#include "index.h"
#include "search.h"
#include "result_output.h"
#include "metanames.h"
#include "dump.h"

void dump_memory_file_list( SWISH *sw, IndexFILE *indexf ) 
{
    int     i;
    struct  file *fi = NULL;
   
    printf("\n\n-----> FILES in index %s <-----\n", indexf->line );
    for (i = 0; i < indexf->filearray_cursize; i++)
    {
        fi = indexf->filearray[ i ];

        fflush(stdout);
        printf("%d: %s\n", i+1, fi->filename);


        dump_file_properties( indexf, fi );
        
        printf("\n");
    }
    printf("\nNumber of File Entries: %d\n", indexf->header.totalfiles);
    fflush(stdout);
}



void dump_index_file_list( SWISH *sw, IndexFILE *indexf ) 
{
    int     i;
    struct  file *fi = NULL;
   
    printf("\n\n-----> FILES in index %s <-----\n", indexf->line );
    for (i = 0; i < indexf->header.totalfiles; i++)
    {
        fi = readFileEntry(sw, indexf, i + 1);

        fflush(stdout);
        printf("%d: %s\n", i+1, fi->filename);


        dump_file_properties( indexf, fi );
        printf("\n");


#ifdef PROPFILE
        printf("ReadAllDocProperties:\n");
        fi->docProperties =  ReadAllDocPropertiesFromDisk( sw, indexf, i+1 );
        dump_file_properties( indexf, fi );

        if ( fi->docProperties )
            freeDocProperties( fi->docProperties );
        fi->docProperties = NULL;
        printf("\n");


{
    propEntry *p;
    int j;
    struct metaEntry *meta_entry;

    printf("ReadSingleDocPropertiesFromDisk:\n");

    for (j=0; j<= 20; j++) // just for testing
    {
        if ( !(p = ReadSingleDocPropertiesFromDisk(sw, indexf, i+1, j, 0 )) )
            continue;

        meta_entry = getPropNameByID( &indexf->header, j );
        dump_single_property( p, meta_entry );
        freeProperty( p );
    }
}
#endif
        printf("\n");


        freefileinfo(fi);
    }
    printf("\nNumber of File Entries: %d\n", indexf->header.totalfiles);
    fflush(stdout);
}




/* Prints out the data in an index DB */
void    DB_decompress(SWISH * sw, IndexFILE * indexf)
{
    int     i,
            j,
            c,
            fieldnum,
            frequency,
            metaname,
            structure,
            tmpval,
            filenum,
           *position;
    unsigned long    nextposmetaname;
    char    word[2];
    char   *resultword;
    unsigned char   *worddata, *s, flag;
    int     sz_worddata;
    long    wordID;


    
    indexf->DB = DB_Open(sw, indexf->line);

    metaname = 0;

    nextposmetaname = 0L;

    c = 0;

    frequency = 0;

        /* Read header */
    read_header(sw, &indexf->header, indexf->DB);
    
        /* Allocate size for fileinfo */
    indexf->filearray_cursize = indexf->header.totalfiles;
    indexf->filearray_maxsize = indexf->header.totalfiles;
    indexf->filearray = emalloc(indexf->header.totalfiles * sizeof(struct file *));
    for(i = 0; i < indexf->header.totalfiles; i++)
        indexf->filearray[i] = NULL;

    if (DEBUG_MASK & (DEBUG_INDEX_ALL | DEBUG_INDEX_HEADER) )
        resultPrintHeader(sw, 0, &indexf->header, indexf->line, 0);

    fieldnum = 0;


    /* Do metanames first as that will be helpful for decoding next */
    if (DEBUG_MASK & (DEBUG_INDEX_ALL | DEBUG_INDEX_METANAMES)  )
        dump_metanames( sw, indexf );

    if (DEBUG_MASK & DEBUG_INDEX_WORDS_ONLY)
    {
        DB_InitReadWords(sw, indexf->DB);

        for( j = 0; j < 256; j++ )
        {
            word[0] = (unsigned char) j;
            word[1] = '\0';
            DB_ReadFirstWordInvertedIndex(sw, word,&resultword,&wordID,indexf->DB);

            while(wordID)
            {
                printf("%s\n",resultword);

                
                efree(resultword);
                DB_ReadNextWordInvertedIndex(sw, word,&resultword,&wordID,indexf->DB);

            }
        }
        DB_EndReadWords(sw, indexf->DB);
    }


    else if (DEBUG_MASK & (DEBUG_INDEX_ALL | DEBUG_INDEX_WORDS | DEBUG_INDEX_WORDS_FULL | DEBUG_INDEX_WORDS_META)  )
    {
        int     *meta_used;
        int     end_meta = 0;

        printf("\n-----> WORD INFO <-----\n");

        for(i = 0; i < indexf->header.metaCounter; i++)
            if ( indexf->header.metaEntryArray[i]->metaID > end_meta )
                end_meta = indexf->header.metaEntryArray[i]->metaID;

        meta_used = emalloc( sizeof(int) * ( end_meta + 1) );  
    
        /* _META only reports which tags the words are found in */
        for(i = 0; i <= end_meta; i++)
            meta_used[i] = 0;


        DB_InitReadWords(sw, indexf->DB);

        for(j=0;j<256;j++)
        {
            word[0] = (unsigned char) j; word[1] = '\0';
            DB_ReadFirstWordInvertedIndex(sw, word,&resultword,&wordID,indexf->DB);

            while(wordID)
            {
                printf("\n%s",resultword);

                /* Read Word's data */
                DB_ReadWordData(sw, wordID, &worddata, &sz_worddata, indexf->DB);

                /* parse and print word's data */
                s = worddata;

                tmpval = uncompress2(&s);     /* tfrequency */
                metaname = uncompress2(&s);     /* metaname */
                if (metaname)
                {
                    nextposmetaname = UNPACKLONG2(s); s += sizeof(long);
                }
                filenum = 0;
                while(1)
                {                   /* Read on all items */
                    uncompress_location_values(&s,&flag,&tmpval,&structure,&frequency);
                    filenum += tmpval;
                    position = (int *) emalloc(frequency * sizeof(int));
                    uncompress_location_positions(&s,flag,frequency,position);


                    // if (sw->verbose >= 4)
                    if (DEBUG_MASK & (DEBUG_INDEX_ALL|DEBUG_INDEX_WORDS_FULL))
                    {
                        struct file *fileInfo;
                        printf("\n Meta:%d", metaname);
                        fileInfo = readFileEntry(sw, indexf, filenum);
                        printf(" %s", fileInfo->filename);
                        printf(" Struct:%x", structure);
                        printf(" Freq:%d", frequency);
                        printf(" Pos:");
                    }
                    else if ( DEBUG_MASK & DEBUG_INDEX_WORDS_META)
                        meta_used[ metaname ]++;
                    else
                    {
                        printf(" [%d", metaname);
                        printf(" %d", filenum);
                        printf(" %x", structure);
                        printf(" %d (", frequency);
                    }
                    

                    for (i = 0; i < frequency; i++)
                    {
                        if (DEBUG_MASK & (DEBUG_INDEX_ALL | DEBUG_INDEX_WORDS_FULL))
                        //if (sw->verbose >= 4)
                        {
                            if (i)
                                printf(",%d", position[i]);
                            else
                                printf("%d", position[i]);
                        }
                        else if ( DEBUG_MASK & DEBUG_INDEX_WORDS)
                        {
                            if (i)
                                 printf(" %d", position[i]);
                            else
                                 printf("%d", position[i]);
                        }
                    }

                    efree(position);

                    if ( DEBUG_MASK & DEBUG_INDEX_WORDS )
                        printf(")]");

                    if ((s - worddata) == sz_worddata)
                        break;   /* End of worddata */

                    if ((unsigned long)(s - worddata) == nextposmetaname)
                    {
                        filenum = 0;
                        metaname = uncompress2(&s);
                        if (metaname)
                        {
                            nextposmetaname = UNPACKLONG2(s); 
                            s += sizeof(long);
                        }
                        else
                            nextposmetaname = 0L;
                    }
                }

                if ( DEBUG_MASK & DEBUG_INDEX_WORDS_META)
                {
                    for(i = 0; i <= end_meta; i++)
                    {
                        if ( meta_used[i] )
                            printf( "\t%d", i );
                        meta_used[i] = 0;
                    }
                }
                

                if ( !( DEBUG_MASK & DEBUG_INDEX_WORDS_META ))
                    printf("\n");

                efree(worddata);
                efree(resultword);
                DB_ReadNextWordInvertedIndex(sw, word,&resultword,&wordID,indexf->DB);
            }
        }
        DB_EndReadWords(sw, indexf->DB);

        efree( meta_used );
    }



    /* Decode Stop Words: All them are in just one line */
    if (DEBUG_MASK & (DEBUG_INDEX_ALL | DEBUG_INDEX_STOPWORDS)  )
    {
        printf("\n\n-----> STOP WORDS <-----\n");
        for(i=0;i<indexf->header.stopPos;i++)
            printf("%s ",indexf->header.stopList[i]);
        printf("\n");
    }



    /* Decode File Info */
    if (DEBUG_MASK & (DEBUG_INDEX_ALL | DEBUG_INDEX_FILES)  )
        dump_index_file_list( sw, indexf );


    DB_Close(sw, indexf->DB);

}


int check_sorted_index( SWISH *sw, IndexFILE *indexf, struct metaEntry *m )
{
    unsigned char *buffer;
    int     sz_buffer;

    DB_InitReadSortedIndex(sw, indexf->DB);
    
    /* Get the sorted index of the property */
    DB_ReadSortedIndex(sw, m->metaID, &buffer, &sz_buffer, indexf->DB);

    if ( sz_buffer )
        efree( buffer );

    /* Table doesn't exist */
    return sz_buffer;
}


void dump_metanames( SWISH *sw, IndexFILE *indexf )
{
    struct metaEntry *meta_entry;
    int i;

    printf("\n\n-----> METANAMES <-----\n");
    for(i = 0; i < indexf->header.metaCounter; i++)
    {
        meta_entry = indexf->header.metaEntryArray[i];
        
        printf("%s id:%d type:%d ",meta_entry->metaName, meta_entry->metaID, meta_entry->metaType);

        if ( is_meta_index( meta_entry ) )
            printf(" META_INDEX");

        if ( is_meta_internal( meta_entry ) )
            printf(" META_INTERNAL");
            

        if ( is_meta_property( meta_entry ) )
        {
            printf(" META_PROP:");

            if  ( is_meta_string(meta_entry) )
                printf("STRING");

            else if ( is_meta_date(meta_entry) )
                printf("DATE");

            else if ( is_meta_number(meta_entry) )
                printf("NUMBER");

            else
                printf("unknown!");
        }

        if ( check_sorted_index( sw, indexf, meta_entry)  )
            printf(" *presorted*");


        if ( meta_entry->alias )
        {
            struct metaEntry *m = is_meta_index( meta_entry )
                                  ? getMetaNameByID( &indexf->header, meta_entry->alias )
                                  : getPropNameByID( &indexf->header, meta_entry->alias );

            printf(" [Alias for %s (%d)]", m->metaName, m->metaID );
        }


        printf("\n");
        
    }
    printf("\n");
}

