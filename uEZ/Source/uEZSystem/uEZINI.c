/*-------------------------------------------------------------------------*
 * File:  uEZINI.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZINI
 *  @{
 *  @brief     uEZ INI Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    The uEZ INI interface.
 *
 * @par Example code:
 * Example task to open INI and read a string from the network section
 * @par
 * @code
 * #include <uEZ.h>
 * #include <uEZINI.h>
 * #include <uEZFile.h>
 *
 * TUInt32 networkINIRead(T_uezTask aMyTask, void *aParams)
 * {
 *  T_uezINISession aSession;
 *  char buffer[40];
 *  if (UEZINIOpen("1:CONFIG.INI", &aSession) == UEZ_ERROR_NONE) {
 *    	// the ini opened properly
 *	    UEZINIGotoSection(aSession, "Network");
 *		// default value of IP is 192.168.10.20, load new value from ini into buffer
 *		UEZINIGetString(aSession, "ip", "192.168.10.20", buffer, sizeof(buffer) - 1);
 *      if (UEZINIClose(aSession) != UEZ_ERROR_NONE) {
 *            // error closing the ini session
 *      }
 *  } else {
 *      // an error occurred opening the ini
 *  }
 *     return 0;
 * }
 * @endcode
 */
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZINI.h>
#include <uEZFile.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef MAX_INI_STRING
#define MAX_INI_STRING                  128
#endif

#ifndef INI_BLOCK_READ_SIZE
#define INI_BLOCK_READ_SIZE             128
#endif

#ifndef MAX_INI_FILENAME_LENGTH
#define MAX_INI_FILENAME_LENGTH         128
#endif

#ifndef MAX_INI_SECTION_LENGTH
#define MAX_INI_SECTION_LENGTH          128
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct _T_INIKeyPair {
    struct _T_INIKeyPair *iNext;
    char *iKey;
    char *iData;
} T_INIKeyPair;

typedef struct _T_INISection {
    struct _T_INISection *iNext;
    char *iSectionName;
    T_INIKeyPair *iKeyPairList;
    T_INIKeyPair *iKeyPairTail;
} T_INISection;

typedef struct {
    char iFilename[MAX_INI_FILENAME_LENGTH + 1];
    char iSection[MAX_INI_SECTION_LENGTH + 1];
    struct _T_INISection *iSectionList;
    struct _T_INISection *iSectionTail;
} T_INISession;

typedef enum {
    PARSE_NEED_SECTION,
    PARSE_SECTION,
    PARSE_NEXT_SECTION_OR_KEY,
    PARSE_KEY,
    PARSE_DATA_PAST_WHITESPACE,
    PARSE_DATA,
} T_INIParseMode;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  IFindSection
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]    *p	TODO
 *
 *  @param [in]    *aSectionName	TODO
 *
 *  @return        *T_INISection	TODO
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_INISection *IFindSection(T_INISession *p, const char *aSectionName)
{
    T_INISection *p_section = p->iSectionList;
    while (p_section) {
        if (strcmp(p_section->iSectionName, aSectionName) == 0)
            break;
        p_section = p_section->iNext;
    }
    return p_section;
}

/*---------------------------------------------------------------------------*
 * Routine:  IFindKeyPair
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]    *p	TODO
 *
 *  @param [in]    *p_section	TODO
 *
 *  @param [in]    *aKey	TODO
 *
 *  @return        *T_INIKeyPair	TODO
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_INIKeyPair *IFindKeyPair(
    T_INISession *p,
    T_INISection *p_section,
    const char *aKey)
{
    T_INIKeyPair *p_keypair;

    if (p_section == 0)
        return 0;

    p_keypair = p_section->iKeyPairList;
    while (p_keypair) {
        if (strcmp(p_keypair->iKey, aKey) == 0)
            break;
        p_keypair = p_keypair->iNext;
    }
    return p_keypair;
}

/*---------------------------------------------------------------------------*
 * Routine:  IFindOrCreateSection
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]    *p	TODO
 *
 *  @param [in]    *aSectionName	TODO
 *
 *  @return        *T_INISection	TODO
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_INISection *IFindOrCreateSection(T_INISession *p, const char *aSectionName)
{
    T_INISection *p_section = IFindSection(p, aSectionName);

    if (!p_section) {
        // End of section found.  Setup this section
        p_section = UEZMemAlloc(sizeof(T_INISection));
        if (p_section) {
            if (p->iSectionList) {
                p->iSectionTail->iNext = p_section;
                p->iSectionTail = p_section;
            } else {
                p->iSectionList = p->iSectionTail = p_section;
            }
            p_section->iKeyPairList = p_section->iKeyPairTail = 0;
            p_section->iSectionName = UEZMemAlloc(strlen(aSectionName) + 1);
            p_section->iNext = 0;
            if (p_section->iSectionName) {
                strcpy(p_section->iSectionName, aSectionName);
            } else {
                UEZMemFree(p_section);
                p_section = 0;
            }
        }
    }
    return p_section;
}

/*---------------------------------------------------------------------------*
 * Routine:  IFindOrCreateKeyPair
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]    *p	TODO
 *
 *  @param [in]    *p_section	TODO
 *
 *  @param [in]    *aKeyName	TODO
 *
 *  @return        *T_INIKeyPair	TODO
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_INIKeyPair *IFindOrCreateKeyPair(
    T_INISession *p,
    T_INISection *p_section,
    const char *aKeyName)
{
    T_INIKeyPair *p_keypair = IFindKeyPair(p, p_section, aKeyName);
    if (!p_keypair) {
        p_keypair = UEZMemAlloc(sizeof(T_INIKeyPair));
        if (p_keypair) {
            if (p_section->iKeyPairTail) {
                p_section->iKeyPairTail->iNext = p_keypair;
                p_section->iKeyPairTail = p_keypair;
            } else {
                p_section->iKeyPairList = p_section->iKeyPairTail = p_keypair;
            }
            p_keypair->iKey = UEZMemAlloc(strlen(aKeyName) + 1);
            p_keypair->iData = 0;
            p_keypair->iNext = 0;
            if (p_keypair->iKey) {
                strcpy(p_keypair->iKey, aKeyName);
            } else {
                UEZMemFree(p_keypair);
                p_keypair = 0;
            }
        }
    }
    return p_keypair;
}

/*---------------------------------------------------------------------------*
 * Routine:  IParseFile
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]    *p	TODO
 *
 *  @param [in]    file	TODO
 *
 *  @return        T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError IParseFile(T_INISession *p, T_uezFile file)
{
    char string[MAX_INI_STRING + 1];
    int stringLen = 0;
    char block[INI_BLOCK_READ_SIZE];
    char c;
    T_INIParseMode mode = PARSE_NEED_SECTION;
    TUInt32 len = 0;
    TUInt32 i = 0;
    T_uezError error = UEZ_ERROR_NONE;
    T_INISection *p_section;
    T_INIKeyPair *p_keypair = 0;

    while (error == UEZ_ERROR_NONE) {
        // Read more data, if we need more
        if (i >= len) {
            len = INI_BLOCK_READ_SIZE;
            error = UEZFileRead(file, block, sizeof(block), &len);
            i = 0;
            // No more data? Then stop
            if (!len) {
                error = UEZ_ERROR_NONE;
                break;
            }
            if (error != UEZ_ERROR_NONE)
                break;
        }
        // Parse what we have in the block
        c = block[i++];
        // Ignore \r in lines
        if (c == '\r')
            continue;
        switch (mode) {
            case PARSE_NEED_SECTION:
                if (c == '[') {
                    stringLen = 0;
                    mode = PARSE_SECTION;
                }
                break;
            case PARSE_SECTION:
                if (c == ']') {
                    // End of section found.  Setup this section
                    string[stringLen] = '\0';
                    stringLen = 0;
//printf("[%s]\n", string);
                    p_section = IFindOrCreateSection(p, string);
                    if (!p_section)
                        error = UEZ_ERROR_OUT_OF_MEMORY;

                    mode = PARSE_NEXT_SECTION_OR_KEY;
                } else {
                    // Add to the section name if there is room
                    // and continue reading.
                    if (stringLen < MAX_INI_STRING)
                        string[stringLen++] = c;
                }
                break;
            case PARSE_NEXT_SECTION_OR_KEY:
                if (c == '[') {
                    stringLen = 0;
                    mode = PARSE_SECTION;
                } else if (!isspace(c)) {
                    // Key is starting
                    string[0] = c;
                    stringLen = 1;
                    mode = PARSE_KEY;
                }
                break;
            case PARSE_KEY:
                if (!isspace(c)) {
                    if (c == '=') {
                        // End of key found.  Setup this section
                        string[stringLen] = '\0';
                        stringLen = 0;
//printf("  Key: %s -> ", string);

                        p_section = p->iSectionTail;
                        p_keypair = IFindOrCreateKeyPair(p, p_section, string);
                        if (!p_keypair)
                            error = UEZ_ERROR_OUT_OF_MEMORY;

                        stringLen = 0;
                        mode = PARSE_DATA_PAST_WHITESPACE;
                    } else {
                        // Add to the section name if there is room
                        // and continue reading.
                        if (stringLen < MAX_INI_STRING)
                            string[stringLen++] = c;
                    }
                }
                break;
            case PARSE_DATA_PAST_WHITESPACE:
                if ((isspace(c)) && (c != '\n'))
                    break;
                mode = PARSE_DATA;
                // fall through
            case PARSE_DATA:
                if (c == '\n') {
                    // End of the line
                    string[stringLen] = '\0';
                    if (p_keypair->iData)
                        UEZMemFree(p_keypair->iData);
                    p_keypair->iData = UEZMemAlloc(stringLen + 1);
                    stringLen = 0;
//printf("  Data: %s\n", string);
                    strcpy(p_keypair->iData, string);
                    mode = PARSE_NEXT_SECTION_OR_KEY;
                } else {
                    string[stringLen++] = c;
                }
                break;
        }
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZINIOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open a session for doing INI transactions.  It may not actually
 *  open an INI file at this time.
 *
 *  @param [in]    *aFilename		INI Filename
 *
 *  @param [in]    *aSession 		Pointer to created session handle
 *
 *  @return        T_uezError 		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *
 *  T_uezINISession aSession;
 *  if (UEZINIOpen("1:CONFIG.INI", &aSession) == UEZ_ERROR_NONE) {
 *    	// the ini opened properly
 *  } else {
 *      // an error occurred opening the ini
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZINIOpen(const char *aFilename, T_uezINISession *aSession)
{
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_INISession *p_session;
    T_uezFile file;

    *aSession = 0;
    // Check that the name is not too long
    if (strlen(aFilename) > MAX_INI_FILENAME_LENGTH)
        return UEZ_ERROR_INVALID_PARAMETER;
    p_session = (T_INISession *)UEZMemAlloc(sizeof(T_INISession));
    if (p_session) {
        memset(p_session, 0, sizeof(*p_session));
        strcpy(p_session->iFilename, aFilename);

        error = UEZFileOpen(aFilename, FILE_FLAG_READ_ONLY, &file);
        if (!error) {
            error = IParseFile(p_session, file);
            UEZFileClose(file);
        } else if (error == UEZ_ERROR_NOT_FOUND) {
            // No file exists, make an empty one.
            error = UEZ_ERROR_NONE;
        }
        if (error != UEZ_ERROR_NONE) {
            // Free up the memory
            UEZMemFree(p_session);
        } else {
            *aSession = (T_uezINISession)p_session;
        }
    } else {
        error = UEZ_ERROR_OUT_OF_MEMORY;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZINIClose
 *---------------------------------------------------------------------------*/
/**
 *  Open a session for doing INI transactions.  It may not actually
 *  open an INI file at this time.
 *
 *  @param [in]    aSession 		INI Session
 *
 *  @return        T_uezError 		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *
 *  T_uezINISession aSession;
 *  if (UEZINIOpen("1:CONFIG.INI", &aSession) == UEZ_ERROR_NONE) {
 *    	// the ini opened properly
 *	 
 *      if (UEZINIClose(aSession) != UEZ_ERROR_NONE) {
 *            // error closing the ini session
 *      }
 *  } else {
 *      // an error occurred opening the ini
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZINIClose(T_uezINISession aSession)
{
    T_INISession *p = (T_INISession *)aSession;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_INISection *p_section;
    T_INISection *p_section_next;
    T_INIKeyPair *p_keypair;
    T_INIKeyPair *p_keypair_next;

    if (p) {
        for (p_section = p->iSectionList; p_section; p_section = p_section_next) {
            for (p_keypair = p_section->iKeyPairList; p_keypair; p_keypair
                = p_keypair_next) {
                p_keypair_next = p_keypair->iNext;
                if (p_keypair->iKey)
                    UEZMemFree(p_keypair->iKey);
                if (p_keypair->iData)
                    UEZMemFree(p_keypair->iData);
                UEZMemFree(p_keypair);
            }
            p_section_next = p_section->iNext;
            UEZMemFree(p_section->iSectionName);
            UEZMemFree(p_section);
        }
        UEZMemFree(p);
        error = UEZ_ERROR_NONE;
    } else {
        error = UEZ_ERROR_HANDLE_INVALID;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZINISave
 *---------------------------------------------------------------------------*/
/**
 *  Save the given INI Session as a new file.
 *
 *  @param [in]    aSession 		INI Session
 *
 *  @param [in]    *aFilename 		Name of file to save
 *
 *  @return        T_uezError 		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *
 *  T_uezINISession aSession;
 *  if (UEZINIOpen("1:CONFIG.INI", &aSession) == UEZ_ERROR_NONE) {
 *    	// the ini opened properly
 *	    UEZINISave(aSession, "1:NEWFILE.INI");
 *      if (UEZINIClose(aSession) != UEZ_ERROR_NONE) {
 *            // error closing the ini session
 *      }
 *  } else {
 *      // an error occurred opening the ini
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZINISave(T_uezINISession aSession, const char *aFilename)
{
    T_INISession *p = (T_INISession *)aSession;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_INISection *p_section;
    T_INISection *p_section_next;
    T_INIKeyPair *p_keypair;
    T_INIKeyPair *p_keypair_next;
    T_uezFile file;
    char string[MAX_INI_STRING + 10];

    error = UEZFileOpen(aFilename, FILE_FLAG_WRITE, &file);
    if (!error) {
        for (p_section = p->iSectionList; p_section; p_section = p_section_next) {
            sprintf(string, "[%s]\r\n", p_section->iSectionName);
            UEZFileWrite(file, string, strlen(string), 0);
            for (p_keypair = p_section->iKeyPairList; p_keypair; p_keypair
                = p_keypair_next) {
                p_keypair_next = p_keypair->iNext;
                sprintf(string, "%s=", p_keypair->iKey);
                UEZFileWrite(file, string, strlen(string), 0);
                sprintf(string, "%s\r\n", p_keypair->iData);
                UEZFileWrite(file, string, strlen(string), 0);
            }
            p_section_next = p_section->iNext;
        }
        UEZFileClose(file);
        error = UEZ_ERROR_NONE;
    } else {
        error = UEZ_ERROR_HANDLE_INVALID;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZINIGotoSection
 *---------------------------------------------------------------------------*/
/**
 *  Moves the session to the a new section (either existing or not
 *  existing yet).
 *
 *  @param [in]    aSession 		INI Session
 *
 *  @param [in]    *aSection 		Section within INI file
 *
 *  @return        T_uezError		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *
 *  T_uezINISession aSession;
 *  if (UEZINIOpen("1:CONFIG.INI", &aSession) == UEZ_ERROR_NONE) {
 *    	// the ini opened properly
 *	    UEZINIGotoSection(aSession, "Network");
 *      if (UEZINIClose(aSession) != UEZ_ERROR_NONE) {
 *            // error closing the ini session
 *      }
 *  } else {
 *      // an error occurred opening the ini
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZINIGotoSection(T_uezINISession aSession, const char *aSection)
{
    T_INISession *p = (T_INISession *)aSession;
    T_uezError error = UEZ_ERROR_UNKNOWN;

    if (p) {
        if (strlen(aSection) > MAX_INI_SECTION_LENGTH)
            return UEZ_ERROR_INVALID_PARAMETER;

        strcpy(p->iSection, aSection);
        error = UEZ_ERROR_NONE;
    } else {
        error = UEZ_ERROR_HANDLE_INVALID;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZINIGetInteger32
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]    aSession	TODO
 *
 *  @param [in]    *aKey	TODO
 *
 *  @param [in]    aDefaultValue	TODO
 *
 *  @param [in]    *aValue	TODO
 *
 *  @return        T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZINIGetInteger32(
    T_uezINISession aSession,
    const char *aKey,
    TInt32 aDefaultValue,
    TInt32 *aValue)
{
    T_INISession *p = (T_INISession *)aSession;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_INISection *p_section;
    T_INIKeyPair *p_keypair;

    // Start with the default value
    *aValue = aDefaultValue;

    if (p) {
        // Is section set?
        if (p->iSection[0]) {
            p_section = IFindSection(p, p->iSection);
            p_keypair = IFindKeyPair(p, p_section, aKey);
            if (p_keypair) {
                if ((p_keypair->iData[0] == '$') || ((p_keypair->iData[0] == '0') && (p_keypair->iData[1] == 'x'))) {
                    sscanf(p_keypair->iData, "%x", aValue);
                } else {
                    *aValue = atoi(p_keypair->iData);
                }
            } else {
                *aValue = aDefaultValue;
            }

            error = UEZ_ERROR_NONE;
        } else {
            error = UEZ_ERROR_INCOMPLETE_CONFIGURATION;
        }
    } else {
        error = UEZ_ERROR_HANDLE_INVALID;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZINISetInteger32
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]  aSession  	TODO
 *
 *  @param [in]   *aKey 	TODO
 *
 *  @param [in]   aValue 	TODO
 *
 *  @return        T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZINISetInteger32(
    T_uezINISession aSession,
    const char *aKey,
    TInt32 aValue)
{
    T_INISession *p = (T_INISession *)aSession;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_INISection *p_section;
    T_INIKeyPair *p_keypair;
    char vstr[20];
#if (COMPILER_TYPE == IAR)
    extern int itoa(int value, const char *string, int radix);
#endif
    if (p) {
        // Is section set?
        if (p->iSection[0]) {
            p_section = IFindOrCreateSection(p, p->iSection);
            p_keypair = IFindOrCreateKeyPair(p, p_section, aKey);
            if (p_keypair->iData)
                UEZMemFree(p_keypair->iData);
            itoa(aValue, vstr, 10);
            p_keypair->iData = UEZMemAlloc(strlen(vstr) + 1);
            if (!p_keypair->iData) {
                error = UEZ_ERROR_OUT_OF_MEMORY;
            } else {
                error = UEZ_ERROR_NONE;
                strcpy(p_keypair->iData, vstr);
            }
        } else {
            error = UEZ_ERROR_INCOMPLETE_CONFIGURATION;
        }
    } else {
        error = UEZ_ERROR_HANDLE_INVALID;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZINIGetString
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]  aSession  	TODO
 *
 *  @param [in]  *aKey 	TODO
 *
 *  @param [in]  *aDefaultValue 	TODO
 *
 *  @param [in]  *aString  	TODO
 *
 *  @param [in]  aMaxBytes  	TODO
 *
 *  @return      T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZINIGetString(
    T_uezINISession aSession,
    const char *aKey,
    const char *aDefaultValue,
    char *aString,
    TUInt32 aMaxBytes)
{
    T_INISession *p = (T_INISession *)aSession;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_INISection *p_section;
    T_INIKeyPair *p_keypair;
    TUInt32 len;

    if (p) {
        // Is section set?
        if (p->iSection[0]) {
            p_section = IFindSection(p, p->iSection);
            p_keypair = IFindKeyPair(p, p_section, aKey);
            if (p_keypair) {
                len = strlen(p_keypair->iData);
                if (len > aMaxBytes)
                    len = aMaxBytes;
                memcpy(aString, p_keypair->iData, len);
                aString[len] = '\0';
            } else {
                strcpy(aString, aDefaultValue);
            }

            error = UEZ_ERROR_NONE;
        } else {
            strcpy(aString, aDefaultValue);
            error = UEZ_ERROR_INCOMPLETE_CONFIGURATION;
        }
    } else {
        strcpy(aString, aDefaultValue);
        error = UEZ_ERROR_HANDLE_INVALID;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZINISetString
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]  aSession  	TODO
 *
 *  @param [in]  *aKey  	TODO
 *
 *  @param [in]  *aString  	TODO
 *
 *  @return        T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZINISetString(
    T_uezINISession aSession,
    const char *aKey,
    const char *aString)
{
    T_INISession *p = (T_INISession *)aSession;
    T_uezError error = UEZ_ERROR_UNKNOWN;
    T_INISection *p_section;
    T_INIKeyPair *p_keypair;

    if (p) {
        // Is section set?
        if (p->iSection[0]) {
            p_section = IFindOrCreateSection(p, p->iSection);
            p_keypair = IFindOrCreateKeyPair(p, p_section, aKey);
            if (p_keypair->iData)
                UEZMemFree(p_keypair->iData);

            p_keypair->iData = UEZMemAlloc(strlen(aString) + 1);
            if (!p_keypair->iData) {
                error = UEZ_ERROR_OUT_OF_MEMORY;
            } else {
                strcpy(p_keypair->iData, aString);
                error = UEZ_ERROR_NONE;
            }

            error = UEZ_ERROR_NONE;
        } else {
            error = UEZ_ERROR_INCOMPLETE_CONFIGURATION;
        }
    } else {
        error = UEZ_ERROR_HANDLE_INVALID;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZINIFindNthSection
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]   aSession 	TODO
 *
 *  @param [in]  aIndex  	TODO
 *
 *  @param [in]  *aBuffer  	TODO
 *
 *  @param [in]  aMaxChars  	TODO
 *
 *  @return        T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZINIFindNthSection(
    T_uezINISession aSession,
    TUInt32 aIndex,
    char *aBuffer,
    TUInt32 aMaxChars)
{
    T_INISession *p = (T_INISession *)aSession;
    T_INISection *p_section;
    T_uezError error;
    TUInt32 len;

    if (p) {
        for (p_section = p->iSectionList; p_section && aIndex; p_section
            = p_section->iNext) {
            aIndex--;
        }

        if (p_section) {
            len = strlen(p_section->iSectionName);
            if (len > aMaxChars)
                len = aMaxChars;
            memcpy(aBuffer, p_section->iSectionName, len);
            aBuffer[len] = '\0';
            error = UEZ_ERROR_NONE;
        } else {
            error = UEZ_ERROR_NOT_FOUND;
        }
    } else {
        error = UEZ_ERROR_HANDLE_INVALID;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZINIFindNthKey
 *---------------------------------------------------------------------------*/
/**
 *  TODO
 *
 *  @param [in]   aSession 	TODO
 *
 *  @param [in]  aIndex  	TODO
 *
 *  @param [in]  *aBuffer  	TODO
 *
 *  @param [in]  aMaxChars  	TODO
 *
 *  @return        T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZINI.h>
 *  #include <uEZFile.h>
 *  //TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZINIFindNthKey(
    T_uezINISession aSession,
    TUInt32 aIndex,
    char *aBuffer,
    TUInt32 aMaxChars)
{
    T_INISession *p = (T_INISession *)aSession;
    T_INISection *p_section;
    T_uezError error;
    T_INIKeyPair *p_keypair;
    TUInt32 len;

    if (p) {
        p_section = IFindSection(p, p->iSection);
        if (!p_section) {
            error = UEZ_ERROR_NOT_FOUND;
        } else {
            for (p_keypair = p_section->iKeyPairList; p_keypair && aIndex; p_keypair
                = p_keypair->iNext) {
                aIndex--;
            }

            if (p_keypair) {
                len = strlen(p_keypair->iKey);
                if (len > aMaxChars)
                    len = aMaxChars;
                memcpy(aBuffer, p_keypair->iKey, len);
                aBuffer[len] = '\0';
                error = UEZ_ERROR_NONE;
            } else {
                error = UEZ_ERROR_NOT_FOUND;
            }
        }
    } else {
        error = UEZ_ERROR_HANDLE_INVALID;
    }
    return error;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZINI.c
 *-------------------------------------------------------------------------*/
