/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmCommandManager.h,v $
  Language:  C++
  Date:      $Date: 2005/11/28 16:31:22 $
  Version:   $Revision: 1.2 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMCOMMANDMANAGER_H
#define GDCMCOMMANDMANAGER_H

#include "gdcmRefCounter.h"

#include <map>
#include <iostream>

namespace gdcm 
{
//-----------------------------------------------------------------------------
class Command;
typedef std::pair<const Base *,unsigned int> CommandKey;
typedef std::map<CommandKey,Command *> CommandHT;

//-----------------------------------------------------------------------------
/**
 * \brief CommandManager base class to react on a gdcm event
 *
 * \remarks The execution parameter depends on the
 */
class GDCM_EXPORT CommandManager : public Base
{
   gdcmTypeMacro(CommandManager);

public:
   void Print(std::ostream &os = std::cout, std::string const &indent = "" );

   static void SetCommand(const Base *object,unsigned int type,Command *command);
   static Command *GetCommand(const Base *object,unsigned int type);

   static bool ExecuteCommand(Base *object,unsigned int type,std::string text = "");
   static bool ExecuteCommandConst(const Base *object,unsigned int type,std::string text = "");

   static const CommandManager *GetInstance();

protected:
   CommandManager();
   ~CommandManager();

   void InClearCommand(void);
   void InSetCommand(const Base *object,unsigned int type,Command *command);
   Command *InGetCommand(const Base *object,unsigned int type);

   bool InExecuteCommand(Base *object,unsigned int type,std::string text = "");
   bool InExecuteCommandConst(const Base *object,unsigned int type,std::string text = "");

private:
   static CommandManager Instance;
   CommandHT CommandList;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif