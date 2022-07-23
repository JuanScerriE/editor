#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>

struct FileView {
  std::string m_fileName;

  unsigned int m_cursorX = 0;
  unsigned int m_cursorY = 0;

  std::vector<std::string> m_rows;

  // Constructor
  FileView() = default;

  int openFile(const std::string& fileName);

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

int FileView::openFile(const std::string& fileName) {
  m_fileName = fileName;

  std::ifstream fileStream(m_fileName);

  if (!fileStream.is_open()) {
    std::cerr << "ERROR: Couldn't open " << m_fileName
              << std::endl;
    return -1;
  }

  std::string line;

  while (std::getline(fileStream, line)) {
    m_rows.push_back(line);
  }

  fileStream.close();

  return 0;
}

void FileView::cursorUp() {
  if (m_cursorY > 0) {
    m_cursorY--;

    if (m_cursorX > m_rows[m_cursorY].length()) {
      m_cursorX = m_rows[m_cursorY].length();
    }
  }
}

void FileView::cursorLeft() {
  if (m_cursorX > 0) {
    m_cursorX--;
  }
}

void FileView::cursorRight() {
  if (m_cursorX < m_rows[m_cursorY].size()) {
    m_cursorX++;
  }
}

void FileView::cursorDown() {
  if (m_cursorY < m_rows.size()) {
    m_cursorY++;

    if (m_cursorX > m_rows[m_cursorY].length()) {
      m_cursorX = m_rows[m_cursorY].length();
    }
  }
}

void FileView::dbgPrint() {
  std::cout << "DEBUG: File name: " << m_fileName
            << std::endl;
  std::cout << "DEBUG: Number of rows: " << m_rows.size()
            << std::endl;
  std::cout << "DEBUG: File contents: " << std::endl;

  for (std::string row : m_rows) {
    std::cout << row << std::endl;
  }

  std::cout << std::endl;
}
