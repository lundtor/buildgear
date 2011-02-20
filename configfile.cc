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
#include "buildgear/configfile.h"

extern void stripChar(string &str, char c);

CConfigFile::CConfigFile()
{
   CConfigFile::default_name_prefix = "target/";
   CConfigFile::source_dir = SOURCE_DIR;
}

void CConfigFile::Parse(string filename)
{
      FILE *fp;
      char line_buffer[PATH_MAX];
      string command =  "bash -c 'source " + filename + " 2> /dev/null" +
                        "; echo source_dir=$source_dir \
                         ; echo source_mirror=$source_mirror \
                         ; echo required_version=$required_version \
                         ; echo default_name_prefix=$default_name_prefix \
                         ; echo ignore_footprint=$ignore_footprint \
                         ; echo ignore_checksum=$ignore_checksum \
                         ; echo download_parallel_level=$download_parallel_level \
                         ; echo build_parallel_level=$build_parallel_level \
                         ; echo package_compression_level=$package_compression_level'";
      
      fp = popen(command.c_str(), "r");
      if (fp == NULL)
         throw std::runtime_error(strerror(errno));

      while (fgets(line_buffer, PATH_MAX, fp) != NULL)
      {
         // Parse key=value pairs
         string line(line_buffer);
         string key, value;
         size_t pos = line.find_first_of('=');
	
         key=line.substr(0, pos);
         value=line.substr(pos+1);

         stripChar(value, '\n');

         if (value != "")
         {
            if (key == CONFIG_KEY_DEFAULT_NAME_PREFIX)
               CConfigFile::default_name_prefix = value;
            if (key == CONFIG_KEY_SOURCE_DIR)
               CConfigFile::source_dir = value;
         }
      }
      pclose(fp);
}

void CConfigFile::CorrectSourceDir(void)
{
   // Replace "~" with $HOME value
   if (CConfigFile::source_dir.find("~/") == 0)
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
         CConfigFile::source_dir.erase(0,1);
         CConfigFile::source_dir = home + CConfigFile::source_dir;
      }
   }
}
