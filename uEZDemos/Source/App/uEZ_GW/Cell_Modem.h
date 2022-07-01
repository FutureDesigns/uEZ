/*-------------------------------------------------------------------------*
 * File:  Cell_Modem..h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef SOURCE_APP_CELL_MODEM_H_
#define SOURCE_APP_CELL_MODEM_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void Cell_Modem_Start(void);
TBool Cell_Modem_GetIMEI(char *aIMEI);
TBool Cell_Modem_GetCCID(char *aCCID);
TBool Cell_Modem_GetFWV(char *aFWV);
TBool Cell_Modem_GetMFWV(char *aMFWV);
TBool Cell_Modem_GetHWV(char *aHWV);

#endif // SOURCE_APP_CELL_MODEM_H_
/*-------------------------------------------------------------------------*
 * End of File:  Cell_Modem..h
 *-------------------------------------------------------------------------*/