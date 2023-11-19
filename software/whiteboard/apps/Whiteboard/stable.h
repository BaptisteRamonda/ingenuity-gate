/*
 *	Whiteboard
 *
 *  Copyright © 2022 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

/* Add C includes here */


/* Add C++ includes here */
#if defined __cplusplus

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif

    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <winsock2.h>
#endif

#include <iostream>

#include <QtCore>
#include <QtGui>
#include <QtQml>
#include <QtQuick>

#endif


