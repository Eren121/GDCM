//gdcmCommon.h

#ifndef GDCMCOMMON_H
#define GDCMCOMMON_H

#ifdef __GNUC__
#include <stdint.h>
#define guint16 uint16_t
#define guint32 uint32_t
#define gint16  int16_t
#define gint32  int32_t
#endif

#ifdef _MSC_VER 
typedef  unsigned short guint16;
typedef  unsigned int   guint32;
typedef  short 		gint16;
typedef  int   		gint32;
#endif

#ifdef _MSC_VER
#define GDCM_EXPORT __declspec( dllexport )
#else
#define GDCM_EXPORT
#endif

#include <string>
typedef string TagKey;
typedef string TagName;

#endif
