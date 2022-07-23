#pragma once

#include <string>
#include <vector>

struct FileView {
  std::string m_fileName;

  unsigned int m_cursorX = 0;
  unsigned int m_cursorY = 0;

  std::vector<std::string> m_rows;

  // Constructor
  FileView() = default;

  int openFile(std::string_view fileName);

  // Move Cursor
  void cursorUp();
  void cursorLeft();
  void cursorRight();
  void cursorDown();

  // Debug
  void dbgPrint();

  // Destructor
  ~FileView() = default;
};
