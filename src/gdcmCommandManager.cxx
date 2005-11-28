/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmCommandManager.cxx,v $
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
// ---------------------------------------------------------------
#include "gdcmCommandManager.h"
#include "gdcmCommand.h"

namespace gdcm 
{
//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief Constructor used when we want to generate dicom files from scratch
 */
CommandManager::CommandManager()
{
}


/**
 * \brief   Canonical destructor.
 */
CommandManager::~CommandManager ()
{
}

//-----------------------------------------------------------------------------
// Public
void CommandManager::SetCommand(unsigned int type,Command *command)
{
   Command *cmd=CommandList[type];
   if(cmd!=command)
   {
      if(cmd)
         cmd->Unregister();
      if(command)
      {
         CommandList[type]=command;
         command->Register();
      }
      else
         CommandList.erase(type);
   }
}

Command *CommandManager::GetCommand(unsigned int type) const
{
   try
   {
      return CommandList[type];
   }
   catch(...)
   {
      return NULL;
   }
}

bool CommandManager::ExecuteCommand(unsigned int type,std::string text)
{
   Command *cmd = GetCommand(type);
   if(cmd)
   {
      cmd->SetText(text);
      cmd->SetObject(this);
      cmd->SetType(type);
      cmd->Execute();
      return true;
   }
   return false;
}

bool CommandManager::ConstExecuteCommand(unsigned int type,std::string text) const
{
   Command *cmd = GetCommand(type);
   if(cmd)
   {
      cmd->SetText(text);
      cmd->SetConstObject(this);
      cmd->SetType(type);
      cmd->Execute();
      return true;
   }
   return false;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
// Print
void CommandManager::Print(std::ostream &os, std::string const &indent)
{
   os<<indent<<"Command list : \n";
   CommandHT::iterator it;
   for(it=CommandList.begin();it!=CommandList.end();++it)
   {
      os<<indent<<"   "<<Command::GetCommandAsString(it->first)
        <<" : "<<typeid(it->second).name()
        <<" ("<<it->second<<")"<<std::endl;
   }
}

//-----------------------------------------------------------------------------
} // end namespace gdcm
