#include "Parameters.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

std::vector<std::string> Parameters::Split(const std::string &s, char delim)
{
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

void Parameters::PurseParameters(const std::string &filename)
{
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open parameter file: " + filename);
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    auto tokens = Split(line, ' ');
    if (tokens.size() != 2) {
      throw std::runtime_error("Invalid parameter line: " + line);
    }

    m_parameters[tokens[0]] = tokens[1];
  }
  file.close();

  //Print all parameters
  for (auto it = m_parameters.begin(); it != m_parameters.end(); ++it) {
    std::cout << it->first << " = " << it->second << std::endl;
  }
}

Parameters *Parameters::GetInstance(const std::string &filename)
{
  if (m_instance == nullptr) {
    m_instance = new Parameters(filename);
  }
  return m_instance;
}

std::string Parameters::Get(const std::string &key)
{
  auto it = m_parameters.find(key);
  if (it == m_parameters.end()) {
    throw std::runtime_error("Parameter not found: " + key);
  }
  return it->second;
}

Parameters *Parameters::m_instance = nullptr;