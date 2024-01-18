#ifndef Parameters_hpp
#define Parameters_hpp 1

#include <map>
#include <string>
#include <vector>

// Singleton class to read parameter file and provide access to parameters
class Parameters
{
 private:
  // Singleton instance
  static Parameters *m_instance;

  // Parameter file name
  std::string m_filename;

  // Parameter map
  std::map<std::string, std::string> m_parameters;

  // Private constructor
  Parameters(const std::string &filename) { PurseParameters(filename); }
  void PurseParameters(const std::string &filename);
  std::vector<std::string> Split(const std::string &s, char delim);

 public:
  // Destructor
  ~Parameters()
  {
    delete m_instance;
    m_instance = nullptr;
  }

  // Get singleton instance
  static Parameters *GetInstance() { return m_instance; }
  static Parameters *GetInstance(const std::string &filename);

  // Get parameter value
  std::string Get(const std::string &key);

  Parameters(const Parameters &) = delete;
  Parameters &operator=(const Parameters &) = delete;
};

#endif  // Parameters_hpp