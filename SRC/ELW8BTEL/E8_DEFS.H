/* Build 26.10.28.1 12:58PM */
/*****************************************************************************
 *   E8_DEFS.H                     8Ball for Teleconference - common header  *
 *                                                                           *
 *   Copyright (C) 2026 Elwynor Technologies.                                *
 *   Originally HOTELeconference 8Ball by Randall R. Spangler, annex.com.    *
 *                                                                           *
 *   Version numbers and the includes every source file needs.  Keep         *
 *   ELW8BTEL_VER_* in step with ELW8BTEL.rc, CHANGELOG.MD and ELW8BTEL.RLN. *
 *                                                                           *
 *   Licensed under the GNU Affero General Public License v3.0.              *
 *****************************************************************************/

#ifndef E8_DEFS_H
#define E8_DEFS_H

#include "gcomm.h"
#include "majorbbs.h"

#define ELW8BTEL_VER_MAJOR  1
#define ELW8BTEL_VER_MINOR  0
#define ELW8BTEL_VER_PATCH  0
#define ELW8BTEL_VERSION    "1.0.0"

#define E8_APPNAME   "8Ball for Teleconference"
#define E8_CMDWORD   "8BALL"         /* fixed: GALTELE can't unregister      */

#define E8_MAXQ      150             /* longest question kept, in characters */
#define E8_PUBSIZ    240             /* longest line sent to the channel.    */
                                     /* Teleconference lines are at most 255 */
                                     /* (GALTELE.MSG TINPSZ).                */
#define E8_NREPLY    30              /* REPLY01..REPLY30 in ELW8BTEL.MSG     */

#endif /* E8_DEFS_H */
