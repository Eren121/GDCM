/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmCommandManager.h,v $
  Language:  C++
  Date:      $Date: 2005/11/28 15:20:35 $
  Version:   $Revision: 1.1 $
                                                                                
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
typedef std::map<unsigned int,Command *> CommandHT;

//-----------------------------------------------------------------------------
/**
 * \brief CommandManager base class to react on a gdcm event
 *
 * \remarks The execution parameter depends on the
 */
class GDCM_EXPORT CommandManager : public RefCounter
{
   gdcmTypeMacro(CommandManager);

public:
   /// \brief Contructs an empty Dict with a RefCounter
   static CommandManager *New() {return new CommandManager();}
   void Print(std::ostream &os = std::cout, std::string const &indent = "" );

   void SetCommand(unsigned int type,Command *command);
   Command *GetCommand(unsigned int type) const;

   bool ExecuteCommand(unsigned int type,std::string text = "");
   bool ConstExecuteCommand(unsigned int type,std::string text = "") const;

protected:
   CommandManager();
   ~CommandManager();

private:
   mutable CommandHT CommandList;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
