/**
 *	@file 	uEZINI.h
 *  @brief 	uEZ INI Interface
 *
 *	The uEZ interface for reading and writing INI files.
 */
#ifndef _UEZINI_H_
#define _UEZINI_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

 /**
  *	@typedef	T_uezINISession
	*	Handle for an INI Session
	*/
typedef void *T_uezINISession;

/**
 *	Open a session for doing INI transactions.
 *
 *	@param [in] 	*aFilename	INI Filename
 *	@param [out]	*aSession		Pointer to created session handle
 *
 *	@return		T_uezError
 */
T_uezError UEZINIOpen(const char *aFilename, T_uezINISession *aSession);

/**
 *	Save the given INI Session as a new file.
 *
 *	@param [in]		aSession		INI session handle
 *	@param [in] 	*aFilename	New INI Filename
 *
 *	@return		T_uezError
 */
T_uezError UEZINISave(T_uezINISession aSession, const char *aFilename);

/**
 *	Close a session for doing INI transactions.
 *
 *	@param [in]		aSession		INI session handle
 *
 *	@return		T_uezError
 */
T_uezError UEZINIClose(T_uezINISession aSession);

/**
 *	Moves the session to the a new section (either existing or not
 *  existing yet).
 *
 *	@param [in]		aSession		INI session handle
 *	@param [in]		*aSection		Section string to move session to
 *
 *	@return		T_uezError
 */
T_uezError UEZINIGotoSection(T_uezINISession aSession, const char *aSection);

/**
 *	Retrieve a 32-bit integer key/value from the active section of the INI file
 *
 *	@param [in]		aSession				INI session handle
 *	@param [in]		*aKey						Section key
 *	@param [in]		aDefaultValue		Default value for aValue if key isn't in section
 *	@param [out]	*aValue					Returned 32 bit integer
 *
 *	@return		T_uezError
 */
T_uezError UEZINIGetInteger32(
    T_uezINISession aSession,
    const char *aKey,
    TInt32 aDefaultValue,
    TInt32 *aValue);
		
/**
 *	Sets a 32-bit integer value for the active section of the INI file
 *
 *	@param [in]		aSession				INI session handle
 *	@param [in]		*aKey						Section key
 *	@param [in]		aValue					32 bit integer value
 *
 *	@return		T_uezError
 */
T_uezError UEZINISetInteger32(
    T_uezINISession aSession,
    const char *aKey,
    TInt32 aValue);
		
/**
 *	Retrieves a string value from the active section of the INI file
 *
 *	@param [in]		aSession				INI session handle
 *	@param [in]		*aKey						Section key
 *	@param [in]		*aDefaultValue	Default string value for aString if key isn't in section
 *	@param [out]	*aString				Returned string value
 *	@param [in]		aMaxBytes				Maximum size of aString
 *
 *	@return		T_uezError
 */
T_uezError UEZINIGetString(
    T_uezINISession aSession,
    const char *aKey,
    const char *aDefaultValue,
    char *aString,
    TUInt32 aMaxBytes);
		
/**
 *	Sets a string value for the active section of the INI file
 *
 *	@param [in]		aSession		INI session handle
 *	@param [in]		*aKey				Section key
 *	@param [in]		aString			String value
 *
 *	@return		T_uezError
 */
T_uezError UEZINISetString(
    T_uezINISession aSession,
    const char *aKey,
    const char *aString);
		
/**
 *	Find the Nth number section in the INI file
 *
 *	@param [in]		aSession		INI session handle
 *	@param [in]		aIndex			Section index number (N)
 *	@param [out]	*aBuffer		Buffer to store section name
 *	@param [in]		aMaxChars		String value
 *
 *	@return		T_uezError
 */
T_uezError UEZINIFindNthSection(
    T_uezINISession aSession,
    TUInt32 aIndex,
    char *aBuffer,
    TUInt32 aMaxChars);

/**
 *	Find the Nth number key in the INI file
 *
 *	@param [in]		aSession		INI session handle
 *	@param [in]		aIndex			Key index number (N)
 *	@param [out]	*aBuffer		Buffer to store key name
 *	@param [in]		aMaxChars		String value
 *
 *	@return		T_uezError
 */		
T_uezError UEZINIFindNthKey(
    T_uezINISession aSession,
    TUInt32 aIndex,
    char *aBuffer,
    TUInt32 aMaxChars);

#endif // _UEZINI_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZINI.h
 *-------------------------------------------------------------------------*/
