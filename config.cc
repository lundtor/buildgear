#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <list>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "buildgear/config.h"

extern void stripChar(string &str, char c);

CConfig::CConfig()
{
   CConfig::download = false;
   CConfig::build = false;
   CConfig::info = false;   
   CConfig::default_name_prefix = "target/";
   CConfig::source_dir = SOURCE_DIR;
}

void CConfig::CorrectName()
{
   // Prepend default name prefix if none is provided
   if ((CConfig::name.find("target/") != 0) &&
      (CConfig::name.find("host/") != 0))
      CConfig::name = CConfig::default_name_prefix + CConfig::name;
}

void CConfig::CorrectSourceDir(void)
{
   // Replace "~" with $HOME value
   if (CConfig::source_dir.find("~/") == 0)
   {
      FILE *fp;
      char line_buffer[PATH_MAX];
      string command = "echo $HOME";
      string home;
      
      fp = popen(command.c_str(), "r");
      if (fp == NULL)
         throw std::runtime_error(strerror(errno));
      
      while (fgets(line_buffer, PATH_MAX, fp) != NULL)
      {
         home = line_buffer;
         stripChar(home, '\n');
      }
      
      pclose(fp);

      if (home != "")
      {
         CConfig::source_dir.erase(0,1);
         CConfig::source_dir = home + CConfig::source_dir;
      }
   }
}

void CConfig::GuessSystem(void)
{
      FILE *fp;
      char line_buffer[PATH_MAX];
      
      fp = popen(CONFIG_GUESS_SCRIPT, "r");
      if (fp == NULL)
         throw std::runtime_error(strerror(errno));
      
      while (fgets(line_buffer, PATH_MAX, fp) != NULL)
      {
         CConfig::host_system = line_buffer;
         stripChar(host_system, '\n');
         CConfig::build_system = CConfig::host_system;
      }
      
      pclose(fp);
}

void CConfig::ShowSystem(void)
{
   cout << "Building '" << CConfig::name << "' for:" << endl;
   cout << "   build  = " << CConfig::build_system << endl;
   cout << "   host   = " << CConfig::host_system << endl;
   cout << "   target = " << CConfig::target_system << endl << endl;
}