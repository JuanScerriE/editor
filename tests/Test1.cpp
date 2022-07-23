#include <FileView.hpp>

int stringTest(FileView &view) {
  std::string fileName = "ThisIsFile.txt";
  view.openFile(fileName);

  printf("Pointer to file name: %p\n", &fileName[0]);
  view.dbgPrint();

  return 0;
}

int main() {
  FileView view;
  stringTest(view);

  printf("Pointer to file name: %p\n", &view.m_fileName[0]);
  view.dbgPrint();

  return 0;
}
