#include <sys/stat.h>
#include <cstdlib>
#include <iostream>
#include <string>

int main() {

  struct stat statbuf;
  std::string unFile = "Dvm.cc";
  std::string chelAltriFile = "VPlot.cc";
    

  if( stat( unFile.c_str(), &statbuf ) == -1 ) {
    std::cout << "Sacrabolt, no hai cjatat\xE2t el fail " << unFile << std::endl;
    exit(1);
  }

  time_t lastModUn = statbuf.st_mtime;

  if( stat( chelAltriFile.c_str(), &statbuf ) == -1 ) {
    std::cout << "Sacrabolt, no hai cjatat\xE2t el fail " << chelAltriFile << std::endl;
    exit(1);
  }

  time_t lastModChelAltri = statbuf.st_mtime;

  int timeDiff = lastModUn - lastModChelAltri;

  std::cout << "Diferenze di 'ultime modifiche': " << timeDiff << " secons." << std::endl;


  return 0;
}
