#include "../include/file_utils_cpp.h"
#include "../include/file_utils_c.h"

std::vector<std::pair<std::string, std::string>> parse_hosts_file(const char *filename) {
  std::ifstream infile("hosts.txt");
  std::string line;
  std::vector<std::pair<std::string, std::string>> translations = std::vector<std::pair<std::string, std::string>>();
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    std::string domain, ip;
    if (!(iss >> domain >> ip)) {
      fprintf(stderr, "Invalid host file");
      abort();
    }
    translations.push_back({domain, ip});
  }
  return translations;
}

uint8_t get_domains(const char *filename, char ***domains) {
  auto translations = parse_hosts_file(filename);
  *domains = (char**) malloc(translations.size() * sizeof(char*));
  for (int i = 0; i < translations.size(); ++i) {
    (*domains)[i] = strdup(translations[i].first.c_str());
  }
  return (uint8_t) translations.size();
}
