/*
Copyright (C) 2007-2009 Olli Hinkka

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/


#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#ifdef ANDROID
	#define LOG_TAG "nanoGL"

	#include <android/log.h>
	
	#define LOG __android_log_print
	#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
	#define LOGD(...) if (DEBUG_NANO) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
	#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)
	#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG,__VA_ARGS__)
	
#elif defined PALM
	#define EVENT_LOGGING
	#ifdef EVENT_LOGGING
		#include <syslog.h>
		#define SYSLOG(...) syslog(__VA_ARGS__)
		
		#define LOGE(...) SYSLOG(LOG_ERR,__VA_ARGS__)
		#define LOGD(...) SYSLOG(LOG_ERR,__VA_ARGS__)
		#define LOGW(...) SYSLOG(LOG_ERR,__VA_ARGS__)
		#define LOGI(...) SYSLOG(LOG_ERR,__VA_ARGS__)
		
		#define printf LOGE
	#else
		#define SYSLOG(...) do { } while(0)
	#endif

#endif

#include "nanogl.h"
#include "qgl.h"

#define DEBUG_NANO 0


#define GL_ENTRY(_r, _api, ...) #_api,

extern void InitGLStructs();

/**
 * Init
 */
int nanoGL_Init()
{
	
	// Init nanoGL
	InitGLStructs();
	return 1;
}

void nanoGL_Destroy()
{
}
    
