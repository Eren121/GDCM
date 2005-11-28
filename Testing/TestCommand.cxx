/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestCommand.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/28 15:20:29 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmCommand.h"
#include "gdcmCallbackCommand.h"
#include "gdcmCommandManager.h"

#include <iostream>
#include <typeinfo.h>

class CommandTest : public gdcm::Command
{
   gdcmTypeMacro(CommandTest);
   gdcmNewMacro(CommandTest);

public:
   virtual void Execute()
   {
      std::cout << "Test class command... for "
                << typeid(GetObject()).name()
                << " (" << GetObject() << ")" << std::endl
                << GetText() << std::endl;
      Executed = true;
   }

   bool IsExecuted() {return Executed;}

private:
   CommandTest() {Executed = false;}

   bool Executed;
};

static bool fctExecuted = false;
void CallbackTest(gdcm::CallbackCommand *cmd)
{
   std::cout << "Test class command... for "
             << typeid(cmd->GetObject()).name()
             << " (" << cmd->GetObject() << ")" << std::endl
             << cmd->GetText() << std::endl;

   fctExecuted = true;
}

int TestCommand(int argc, char *argv[]) 
{
   int error=0;

   gdcm::CommandManager *mgr = gdcm::CommandManager::New();
   CommandTest *cmd = CommandTest::New();
   gdcm::CallbackCommand *cbk = gdcm::CallbackCommand::New();
   cbk->SetCallback(CallbackTest);

   mgr->SetCommand(2,cmd);
   mgr->SetCommand(1,cbk);

   cbk->Delete();
   cmd->Delete();

   std::cout << "Test on callback function execution\n";
   mgr->ExecuteCommand(1,"Test on callback function");
   if(!fctExecuted)
      std::cout<<"... Failed\n";
   error+=!fctExecuted;
   std::cout << std::endl;

   std::cout << "Test on command class execution\n";
   mgr->ExecuteCommand(2,"Test on command class");
   if(!cmd->IsExecuted())
      std::cout<<"... Failed\n";
   error+=!cmd->IsExecuted();
   std::cout << std::endl;

   std::cout << "Test on unset command execution\n";
   mgr->ExecuteCommand(3,"Test on callback function");
   std::cout << std::endl;

   return error;
}
