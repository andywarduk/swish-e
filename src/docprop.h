/*
 * DocProperties.c, DocProperties.h
 *
 * Functions to manage the index's Document Properties 
 *
 * File Created.
 * M. Gaulin 8/10/98
 * Jose Ruiz 2000/10 many modifications
 * Jose Ruiz 2001/01 many modifications
 *
 * 2001-01-26  rasc  getPropertyByname changed
 * 2001-02-09  rasc  printSearchResultProperties changed
 */


void freeProperty( propEntry *prop );
void freeDocProperties (docProperties *);

unsigned char *storeDocProperties (docProperties *, int *);

propEntry *CreateProperty(struct metaEntry *meta_entry, unsigned char *propValue, int propLen, int preEncoded, int *error_flag );
int addDocProperty (docProperties **, struct metaEntry * , unsigned char* ,int, int );
int Compare_Properties( struct metaEntry *meta_entry, propEntry *p1, propEntry *p2 );

unsigned char *fetchDocProperties ( struct file *, char * );

int initSearchResultProperties (SWISH *);
void addSearchResultDisplayProperty (SWISH *, char* );
void addSearchResultSortProperty (SWISH *, char*, int );
void printStandardResultProperties(SWISH *, FILE *, RESULT *);

void swapDocPropertyMetaNames (docProperties **, struct metaMergeEntry *);

char *getResultPropAsString(RESULT *, int);
char *DecodeDocProperty( struct metaEntry *meta_entry, propEntry *prop );
void getSwishInternalProperties(struct file *, IndexFILE *);


PropValue *getResultPropValue (SWISH *sw, RESULT *r, char *name, int ID);

int isAutoProperty (char *propname);

void dump_single_property( propEntry *prop, struct metaEntry *meta_entry );
void dump_file_properties(IndexFILE * indexf, struct  file *fi );

#ifdef PROPFILE
void     WritePropertiesToDisk( SWISH *sw );
docProperties *ReadAllDocPropertiesFromDisk( SWISH *sw, IndexFILE *indexf, int filenum );
propEntry *ReadSingleDocPropertiesFromDisk( SWISH *sw, IndexFILE *indexf, int filenum, int metaID, int max_size );
unsigned char *PackPropLocations( struct file *fi, int *propbuflen );
unsigned char *UnPackPropLocations( struct file *fi, char *buf );
#endif



/*
   -- Mapping AutoProperties <-> METANAMES  
   -- should be the same
*/

/* all AutoPropteries start with this string ! */

#define AUTOPROPERTY__ID_START__  "swish"

#define AUTOPROPERTY_REC_COUNT    "swishreccount"
#define AUTOPROP_ID__REC_COUNT    1
#define AUTOPROPERTY_RESULT_RANK  "swishrank"
#define AUTOPROP_ID__RESULT_RANK  2
#define AUTOPROPERTY_DOCPATH      "swishdocpath"
#define AUTOPROP_ID__DOCPATH      3
#define AUTOPROPERTY_TITLE        "swishtitle"
#define AUTOPROP_ID__TITLE        4
#define AUTOPROPERTY_DOCSIZE      "swishdocsize"
#define AUTOPROP_ID__DOCSIZE      5
#define AUTOPROPERTY_LASTMODIFIED "swishlastmodified"
#define AUTOPROP_ID__LASTMODIFIED 6
#define AUTOPROPERTY_SUMMARY      "swishdescription"
#define AUTOPROP_ID__SUMMARY      7
#define AUTOPROPERTY_STARTPOS     "swishstartpos"
#define AUTOPROP_ID__STARTPOS     8
#define AUTOPROPERTY_INDEXFILE    "swishdbfile"
#define AUTOPROP_ID__INDEXFILE    9
#define AUTOPROPERTY_FILENUM      "swishfilenum"
#define AUTOPROP_ID__FILENUM      10




