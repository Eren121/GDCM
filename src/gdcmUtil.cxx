/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmUtil.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/06 19:10:07 $
  Version:   $Revision: 1.74 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmUtil.h"
#include "gdcmDebug.h"

// For GetCurrentDate, GetCurrentTime
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdarg.h>  //only included in implementation file
#include <stdio.h>   //only included in implementation file

#if defined(_MSC_VER)
   #include <winsock.h>  // for gethostname & gethostbyname
   #undef GetCurrentTime
#else
#ifndef __BORLANDC__
   #include <unistd.h>  // for gethostname
   #include <netdb.h>   // for gethostbyname
#endif
#endif

namespace gdcm 
{
/**
 * \ingroup Globals
 * \brief Provide a better 'c++' approach for sprintf
 * For example c code is:
 * sprintf(trash, "%04x|%04x", group , element);
 *
 * c++ is 
 * std::ostringstream buf;
 * buf << std::right << std::setw(4) << std::setfill('0') << std::hex
 *     << group << "|" << std::right << std::setw(4) << std::setfill('0') 
 *     << std::hex <<  element;
 * buf.str();
 */

std::string Util::Format(const char* format, ...)
{
   char buffer[2048];
   va_list args;
   va_start(args, format);
   vsprintf(buffer, format, args);  //might be a security flaw
   va_end(args); // Each invocation of va_start should be matched 
                 // by a corresponding invocation of va_end
                 // args is then 'undefined'
   return buffer;
}


/**
 * \ingroup Globals
 * \brief Because not available in C++ (?)
 */
void Util::Tokenize (const std::string& str,
                     std::vector<std::string>& tokens,
                     const std::string& delimiters)
{
   std::string::size_type lastPos = str.find_first_not_of(delimiters,0);
   std::string::size_type pos     = str.find_first_of    (delimiters,lastPos);
   while (std::string::npos != pos || std::string::npos != lastPos)
   {
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      lastPos = str.find_first_not_of(delimiters, pos);
      pos     = str.find_first_of    (delimiters, lastPos);
   }
}

/**
 * \ingroup Globals
 * \brief Because not available in C++ (?)
 *        Counts the number of occurences of a substring within a string
 */
 
int Util::CountSubstring (const std::string& str,
                          const std::string& subStr)
{
   int count = 0;   // counts how many times it appears
   std::string::size_type x = 0;       // The index position in the string

   do
   {
      x = str.find(subStr,x);       // Find the substring
      if (x != std::string::npos)   // If present
      {
         count++;                  // increase the count
         x += subStr.length();     // Skip this word
      }
   }
   while (x != std::string::npos);  // Carry on until not present

   return count;
}

/**
 * \ingroup Globals
 * \brief  Weed out a string from the non-printable characters (in order
 *         to avoid corrupting the terminal of invocation when printing)
 * @param s string to remove non printable characters from
 */
std::string Util::CreateCleanString(std::string const & s)
{
   std::string str = s;

   for(unsigned int i=0; i<str.size(); i++)
   {
      if(!isprint((unsigned char)str[i]))
      {
         str[i] = '.';
      }
   }

   if(str.size() > 0)
   {
      if(!isprint((unsigned char)s[str.size()-1]))
      {
         if(s[str.size()-1] == 0)
         {
            str[str.size()-1] = ' ';
         }
      }
   }

   return str;
}

/**
 * \ingroup Globals
 * \brief   Add a SEPARATOR to the end of the name is necessary
 * @param   pathname file/directory name to normalize 
 */
std::string Util::NormalizePath(std::string const & pathname)
{
   const char SEPARATOR_X      = '/';
   const char SEPARATOR_WIN    = '\\';
   const std::string SEPARATOR = "/";
   std::string name = pathname;
   int size = name.size();

   if( name[size-1] != SEPARATOR_X && name[size-1] != SEPARATOR_WIN )
   {
      name += SEPARATOR;
   }
   return name;
}

/**
 * \ingroup Globals
 * \brief   Get the (directory) path from a full path file name
 * @param   fullName file/directory name to extract Path from
 */
std::string Util::GetPath(std::string const & fullName)
{
   std::string res = fullName;
   int pos1 = res.rfind("/");
   int pos2 = res.rfind("\\");
   if( pos1 > pos2)
   {
      res.resize(pos1);
   }
   else
   {
      res.resize(pos2);
   }

   return res;
}

/**
 * \ingroup Util
 * \brief   Get the (last) name of a full path file name
 * @param   fullName file/directory name to extract end name from
 */
std::string Util::GetName(std::string const & fullName)
{   
  std::string filename = fullName;

  std::string::size_type slash_pos = filename.rfind("/");
  std::string::size_type backslash_pos = filename.rfind("\\");
  slash_pos = slash_pos > backslash_pos ? slash_pos : backslash_pos;
  if(slash_pos != std::string::npos)
    {
    return filename.substr(slash_pos + 1);
    }
  else
    {
    return filename;
    }
} 

/**
 * \ingroup Util
 * \brief   Get the current date of the system in a dicom string
 */
std::string Util::GetCurrentDate()
{
    char tmp[512];
    time_t tloc;
    time (&tloc);    
    strftime(tmp,512,"%Y%m%d", localtime(&tloc) );
    return tmp;
}

/**
 * \ingroup Util
 * \brief   Get the current time of the system in a dicom string
 */
std::string Util::GetCurrentTime()
{
    char tmp[512];
    time_t tloc;
    time (&tloc);
    strftime(tmp,512,"%H%M%S", localtime(&tloc) );
    return tmp;  
}

/**
 * \brief Create a /DICOM/ string:
 * It should a of even length (no odd length ever)
 * It can contain as many (if you are reading this from your
 * editor the following character is is backslash followed by zero
 * that needed to be escaped with an extra backslash for doxygen) \\0
 * as you want.
 */
std::string Util::DicomString(const char* s, size_t l)
{
   std::string r(s, s+l);
   assert( !(r.size() % 2) ); // == basically 'l' is even
   return r;
}

/**
 * \ingroup Util
 * \brief Create a /DICOM/ string:
 * It should a of even lenght (no odd length ever)
 * It can contain as many (if you are reading this from your
 * editor the following character is is backslash followed by zero
 * that needed to be escaped with an extra backslash for doxygen) \\0
 * as you want.
 * This function is similar to DicomString(const char*), 
 * except it doesn't take a lenght. 
 * It only pad with a null character if length is odd
 */
std::string Util::DicomString(const char* s)
{
   size_t l = strlen(s);
   if( l%2 )
   {
      l++;
   }
   std::string r(s, s+l);
   assert( !(r.size() % 2) );
   return r;
}

/**
 * \ingroup Util
 * \brief Safely compare two Dicom String:
 *        - Both string should be of even lenght
 *        - We allow padding of even lenght string by either a null 
 *          character of a space
 */
bool Util::DicomStringEqual(const std::string& s1, const char *s2)
{
  // s2 is the string from the DICOM reference: 'MONOCHROME1'
  std::string s1_even = s1; //Never change input parameter
  std::string s2_even = DicomString( s2 );
  if( s1_even[s1_even.size()-1] == ' ')
  {
    s1_even[s1_even.size()-1] = '\0'; //replace space character by null
  }
  return s1_even == s2_even;
}



/**
 * \ingroup Util
 * \brief   tells us if the processor we are working with is BigEndian or not
 */
bool Util::IsCurrentProcessorBigEndian()
{
   uint16_t intVal = 1;
   uint8_t bigEndianRepr[4] = { 0x00, 0x00, 0x00, 0x01 };
   int res = memcmp(reinterpret_cast<const void*>(&intVal),
                    reinterpret_cast<const void*>(bigEndianRepr), 4);
   if (res == 0)
      return true;
   else
      return false;
}


#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> //for bzero
#include <unistd.h>

#ifdef _WIN32
#include <snmp.h>
#include <conio.h>
#include <stdio.h>
typedef BOOL(WINAPI * pSnmpExtensionInit) (
        IN DWORD dwTimeZeroReference,
        OUT HANDLE * hPollForTrapEvent,
        OUT AsnObjectIdentifier * supportedView);

typedef BOOL(WINAPI * pSnmpExtensionTrap) (
        OUT AsnObjectIdentifier * enterprise,
        OUT AsnInteger * genericTrap,
        OUT AsnInteger * specificTrap,
        OUT AsnTimeticks * timeStamp,
        OUT RFC1157VarBindList * variableBindings);

typedef BOOL(WINAPI * pSnmpExtensionQuery) (
        IN BYTE requestType,
        IN OUT RFC1157VarBindList * variableBindings,
        OUT AsnInteger * errorStatus,
        OUT AsnInteger * errorIndex);

typedef BOOL(WINAPI * pSnmpExtensionInitEx) (
        OUT AsnObjectIdentifier * supportedView);
#endif //_WIN32

#ifdef __linux__
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>
#endif

#ifdef __HP_aCC
#include <netio.h>
#endif

#ifdef _AIX
#include <sys/ndd_var.h>
#include <sys/kinfo.h>
#endif

long GetMacAddrSys ( u_char *addr)
{
#ifdef _WIN32
   WSADATA WinsockData;
   if (WSAStartup(MAKEWORD(2, 0), &WinsockData) != 0) {
       fprintf(stderr, "This program requires Winsock 2.x!\n");
       return;
   }

   HINSTANCE m_hInst;
   pSnmpExtensionInit m_Init;
   pSnmpExtensionInitEx m_InitEx;
   pSnmpExtensionQuery m_Query;
   pSnmpExtensionTrap m_Trap;
   HANDLE PollForTrapEvent;
   AsnObjectIdentifier SupportedView;
   UINT OID_ifEntryType[] = {
       1, 3, 6, 1, 2, 1, 2, 2, 1, 3
   };
   UINT OID_ifEntryNum[] = {
       1, 3, 6, 1, 2, 1, 2, 1
   };
   UINT OID_ipMACEntAddr[] = {
       1, 3, 6, 1, 2, 1, 2, 2, 1, 6
   };                          //, 1 ,6 };
   AsnObjectIdentifier MIB_ifMACEntAddr =
       { sizeof(OID_ipMACEntAddr) / sizeof(UINT), OID_ipMACEntAddr };
   AsnObjectIdentifier MIB_ifEntryType = {
       sizeof(OID_ifEntryType) / sizeof(UINT), OID_ifEntryType
   };
   AsnObjectIdentifier MIB_ifEntryNum = {
       sizeof(OID_ifEntryNum) / sizeof(UINT), OID_ifEntryNum
   };
   RFC1157VarBindList varBindList;
   RFC1157VarBind varBind[2];
   AsnInteger errorStatus;
   AsnInteger errorIndex;
   AsnObjectIdentifier MIB_NULL = {
       0, 0
   };
   int ret;
   int dtmp;
   int i = 0, j = 0;
   BOOL found = FALSE;
   char TempEthernet[13];
   m_Init = NULL;
   m_InitEx = NULL;
   m_Query = NULL;
   m_Trap = NULL;

   /* Load the SNMP dll and get the addresses of the functions
      necessary */
   m_hInst = LoadLibrary("inetmib1.dll");
   if (m_hInst < (HINSTANCE) HINSTANCE_ERROR) {
       m_hInst = NULL;
       return;
   }
   m_Init =
       (pSnmpExtensionInit) GetProcAddress(m_hInst, "SnmpExtensionInit");
   m_InitEx =
       (pSnmpExtensionInitEx) GetProcAddress(m_hInst,
                                             "SnmpExtensionInitEx");
   m_Query =
       (pSnmpExtensionQuery) GetProcAddress(m_hInst,
                                            "SnmpExtensionQuery");
   m_Trap =
       (pSnmpExtensionTrap) GetProcAddress(m_hInst, "SnmpExtensionTrap");
   m_Init(GetTickCount(), &PollForTrapEvent, &SupportedView);

   /* Initialize the variable list to be retrieved by m_Query */
   varBindList.list = varBind;
   varBind[0].name = MIB_NULL;
   varBind[1].name = MIB_NULL;

   /* Copy in the OID to find the number of entries in the
      Inteface table */
   varBindList.len = 1;        /* Only retrieving one item */
   SNMP_oidcpy(&varBind[0].name, &MIB_ifEntryNum);
   ret =
       m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus,
               &errorIndex);
//   printf("# of adapters in this system : %i\n",
//          varBind[0].value.asnValue.number); varBindList.len = 2;

   /* Copy in the OID of ifType, the type of interface */
   SNMP_oidcpy(&varBind[0].name, &MIB_ifEntryType);

   /* Copy in the OID of ifPhysAddress, the address */
   SNMP_oidcpy(&varBind[1].name, &MIB_ifMACEntAddr);

   do {

       /* Submit the query.  Responses will be loaded into varBindList.
          We can expect this call to succeed a # of times corresponding
          to the # of adapters reported to be in the system */
       ret =
           m_Query(ASN_RFC1157_GETNEXTREQUEST, &varBindList, &errorStatus,
                   &errorIndex); if (!ret) ret = 1;

       else
           /* Confirm that the proper type has been returned */
           ret =
               SNMP_oidncmp(&varBind[0].name, &MIB_ifEntryType,
                            MIB_ifEntryType.idLength); if (!ret) {
           j++;
           dtmp = varBind[0].value.asnValue.number;
           //printf("Interface #%i type : %i\n", j, dtmp);

           /* Type 6 describes ethernet interfaces */
           if (dtmp == 6) {

               /* Confirm that we have an address here */
               ret =
                   SNMP_oidncmp(&varBind[1].name, &MIB_ifMACEntAddr,
                                MIB_ifMACEntAddr.idLength);
               if ((!ret)
                   && (varBind[1].value.asnValue.address.stream != NULL)) {
                   if (
                       (varBind[1].value.asnValue.address.stream[0] ==
                        0x44)
                       && (varBind[1].value.asnValue.address.stream[1] ==
                           0x45)
                       && (varBind[1].value.asnValue.address.stream[2] ==
                           0x53)
                       && (varBind[1].value.asnValue.address.stream[3] ==
                           0x54)
                       && (varBind[1].value.asnValue.address.stream[4] ==
                           0x00)) {

                       /* Ignore all dial-up networking adapters */
                       //printf("Interface #%i is a DUN adapter\n", j);
                       continue;
                   }
                   if (
                       (varBind[1].value.asnValue.address.stream[0] ==
                        0x00)
                       && (varBind[1].value.asnValue.address.stream[1] ==
                           0x00)
                       && (varBind[1].value.asnValue.address.stream[2] ==
                           0x00)
                       && (varBind[1].value.asnValue.address.stream[3] ==
                           0x00)
                       && (varBind[1].value.asnValue.address.stream[4] ==
                           0x00)
                       && (varBind[1].value.asnValue.address.stream[5] ==
                           0x00)) {

                       /* Ignore NULL addresses returned by other network
                          interfaces */
                       //printf("Interface #%i is a NULL address\n", j);
                       continue;
                   }
                   sprintf(addr, "%02x%02x%02x%02x%02x%02x",
                           varBind[1].value.asnValue.address.stream[0],
                           varBind[1].value.asnValue.address.stream[1],
                           varBind[1].value.asnValue.address.stream[2],
                           varBind[1].value.asnValue.address.stream[3],
                           varBind[1].value.asnValue.address.stream[4],
                           varBind[1].value.asnValue.address.stream[5]);
                   //printf("MAC Address of interface #%i: %s\n", j, TempEthernet);
              }
           }
       }
   } while (!ret);         /* Stop only on an error.  An error will occur
                              when we go exhaust the list of interfaces to
                              be examined */
   //getch();

   /* Free the bindings */
   SNMP_FreeVarBind(&varBind[0]);
   SNMP_FreeVarBind(&varBind[1]);
   return 0;
#endif //_WIN32

/* implementation for Linux */
#ifdef __linux__
    struct ifreq ifr;
    struct ifreq *IFR;
    struct ifconf ifc;
    char buf[1024];
    int s, i;
    int ok = 0;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s==-1) {
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    ioctl(s, SIOCGIFCONF, &ifc);
 
    IFR = ifc.ifc_req;
    for (i = ifc.ifc_len / sizeof(struct ifreq); --i >= 0; IFR++) {

        strcpy(ifr.ifr_name, IFR->ifr_name);
        if (ioctl(s, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) {
                if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
                    ok = 1;
                    break;
                }
            }
        }
    }

    close(s);
    if (ok) {
        bcopy( ifr.ifr_hwaddr.sa_data, addr, 6);
    }
    else {
        return -1;
    }
    return 0;
#endif

/* implementation for HP-UX */
#ifdef __HP_aCC

#define LAN_DEV0 "/dev/lan0"

    int fd;
    struct fis iocnt_block;
    int i;
    char net_buf[sizeof(LAN_DEV0)+1];
    char *p;

    (void)sprintf(net_buf, "%s", LAN_DEV0);
    p = net_buf + strlen(net_buf) - 1;

    /* 
     * Get 802.3 address from card by opening the driver and interrogating it.
     */
    for (i = 0; i < 10; i++, (*p)++) {
        if ((fd = open (net_buf, O_RDONLY)) != -1) {
      iocnt_block.reqtype = LOCAL_ADDRESS;
      ioctl (fd, NETSTAT, &iocnt_block);
      close (fd);

            if (iocnt_block.vtype == 6)
                break;
        }
    }

    if (fd == -1 || iocnt_block.vtype != 6) {
        return -1;
    }

  bcopy( &iocnt_block.value.s[0], addr, 6);
  return 0;

#endif /* HPUX */

/* implementation for AIX */
#ifdef _AIX

    int size;
    struct kinfo_ndd *nddp;

    size = getkerninfo(KINFO_NDD, 0, 0, 0);
    if (size <= 0) {
        return -1;
    }
    nddp = (struct kinfo_ndd *)malloc(size);
          
    if (!nddp) {
        return -1;
    }
    if (getkerninfo(KINFO_NDD, nddp, &size, 0) < 0) {
        free(nddp);
        return -1;
    }
    bcopy(nddp->ndd_addr, addr, 6);
    free(nddp);
    return 0;
#endif //_AIX

/* Not implemented platforms */
  return -1;
}

std::string Util::GetMACAddress()
{
   // This is a rip from: http://cplus.kompf.de/macaddr.html for Linux, HPUX and AIX 
   // and http://tangentsoft.net/wskfaq/examples/src/snmpmac.cpp for windows version
   long stat;
   u_char addr[6];
   std::string macaddr;
 
   stat = GetMacAddrSys( addr);
   if (0 == stat)
   {
      //printf( "MAC address = ");
        for (int i=0; i<6; ++i) 
        {
            //printf("%2.2x", addr[i]);
            macaddr += Format("%2.2x", addr[i]);
        }
       // printf( "\n");
      return macaddr;
   }
   else
   {
      //printf( "No MAC address !\n" );
      return "";
   }
}

/**
 * \ingroup Util
 * \brief   Return the IP adress of the machine writting the DICOM image
 */
std::string Util::GetIPAddress()
{
  // This is a rip from http://www.codeguru.com/Cpp/I-N/internet/network/article.php/c3445/
#ifndef HOST_NAME_MAX
  // SUSv2 guarantees that `Host names are limited to 255 bytes'.
  // POSIX 1003.1-2001 guarantees that `Host names (not including the
  // terminating NUL) are limited to HOST_NAME_MAX bytes'.
#  define HOST_NAME_MAX 255
  // In this case we should maybe check the string was not truncated.
  // But I don't known how to check that...
#if defined(_MSC_VER) || defined(__BORLANDC__)
  // with WinSock DLL we need to initialise the WinSock before using gethostname
  WORD wVersionRequested = MAKEWORD(1,0);
  WSADATA WSAData;
  int err = WSAStartup(wVersionRequested,&WSAData);
  if (err != 0) {
      /* Tell the user that we could not find a usable */
      /* WinSock DLL.                                  */
      WSACleanup();
      return "127.0.0.1";
  }
#endif
  
#endif //HOST_NAME_MAX

  std::string str;
  char szHostName[HOST_NAME_MAX+1];
  int r = gethostname(szHostName, HOST_NAME_MAX);

  if( r == 0 )
  {
    // Get host adresses
    struct hostent * pHost = gethostbyname(szHostName);

    for( int i = 0; pHost!= NULL && pHost->h_addr_list[i]!= NULL; i++ )
    {
      for( int j = 0; j<pHost->h_length; j++ )
      {
        if( j > 0 ) str += ".";

        str += Util::Format("%u", 
            (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j]);
      }
      // str now contains one local IP address 

#if defined(_MSC_VER) || defined(__BORLANDC__)
  WSACleanup();
#endif
  
    }
  }
  // If an error occur r == -1
  // Most of the time it will return 127.0.0.1...
  return str;
}

/**
 * \ingroup Util
 * \brief Creates a new UID. As stipulate in the DICOM ref
 *        each time a DICOM image is create it should have 
 *        a unique identifier (URI)
 */
std::string Util::CreateUniqueUID(const std::string& root)
{
  // The code works as follow:
  // echo "gdcm" | od -b
  // 0000000 147 144 143 155 012
  // Therefore we return
  // radical + 147.144.143.155 + IP + time()
  std::string radical = root;
  if( !root.size() ) //anything better ?
  {
    radical = "0.0."; // Is this really usefull ?
  }
  // else
  // A root was specified use it to forge our new UID:
  radical += "147.144.143.155"; // gdcm
  radical += ".";
  radical += Util::GetIPAddress();
  radical += ".";
  radical += Util::GetCurrentDate();
  radical += ".";
  radical += Util::GetCurrentTime();

  return radical;
}

template <class T>
std::ostream& binary_write(std::ostream& os, const T& val)
{
    return os.write(reinterpret_cast<const char*>(&val), sizeof val);
}

std::ostream& binary_write(std::ostream& os, const uint16_t& val)
{
#ifdef GDCM_WORDS_BIGENDIAN
    uint16_t swap;
    swap = ((( val << 8 ) & 0x0ff00 ) | (( val >> 8 ) & 0x00ff ) );
    return os.write(reinterpret_cast<const char*>(&swap), 2);
#else
    return os.write(reinterpret_cast<const char*>(&val), 2);
#endif //GDCM_WORDS_BIGENDIAN
}

std::ostream& binary_write(std::ostream& os, const uint32_t& val)
{
#ifdef GDCM_WORDS_BIGENDIAN
    uint32_t swap;
    swap = ( ((val<<24) & 0xff000000) | ((val<<8)  & 0x00ff0000) | 
             ((val>>8)  & 0x0000ff00) | ((val>>24) & 0x000000ff) );
    return os.write(reinterpret_cast<const char*>(&swap), 4);
#else
    return os.write(reinterpret_cast<const char*>(&val), 4);
#endif //GDCM_WORDS_BIGENDIAN
}

std::ostream& binary_write(std::ostream& os, const char* val)
{
    return os.write(val, strlen(val));
}

std::ostream& binary_write(std::ostream& os, std::string const & val)
{
    return os.write(val.c_str(), val.size());
}

} // end namespace gdcm

