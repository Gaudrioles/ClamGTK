#ifndef STRTOOL_H_INCLUDED
#define STRTOOL_H_INCLUDED

void strcut(char *source, const char *supp);

char *getVirusName(const char *ouputBuffer);
char *getFilePath(const char *ouputBuffer);
char *extract_version(const char *input);
char *getClamscanVersion(void);

gchar *getFileName(const char *filePath);
gchar *extract_db_version(const char *input);
gchar *extract_date(const char *input);

gboolean checkIfClean(const gchar *src);
gboolean extractData(const char *input, int regexSelect, ExtractedData *result);

#endif /* !STRTOOL_H_INCLUDED */