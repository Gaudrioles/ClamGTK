#ifndef STRTOOL_H_INCLUDED
#define STRTOOL_H_INCLUDED

void strcut(char *source, const char *supp);

char *getVirusName(const char *ouputBuffer);
char *getFilePath(const char *ouputBuffer);

gchar *getFileName(const char *filePath);

gboolean checkIfClean(const gchar *src);
gboolean extractData(const char *input, int regexSelect, ExtractedData *result);

#endif /* !STRTOOL_H_INCLUDED */