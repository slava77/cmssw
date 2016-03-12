/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of TOTEM offline software.
* Authors: 
*   Jan Kaspar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#ifndef _CommonDef_h_
#define _CommonDef_h_

#ifdef _MONITOR_
  #define NEW_STYLE_ERRORS 1
  #define c_endl std::endl
  #include "monitor/interface/MonitorCommon.h"
#else

#include <iostream>
#define ERROR(where) std::cerr << "\033[15D\033[00;31mError\033[00m [" << where << "] : "
#define WARN(where) std::cerr << "\033[15D\033[00;35mWarning\033[00m [" << where << "] : "
#define INFO(where) std::cerr << "\033[15D\033[00;34mInfo\033[00m [" << where << "] : "
#define c_endl std::endl

#endif

#endif
