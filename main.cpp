#include <vector>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <utility>
#include <string>
#include <exception>
using namespace std::filesystem;
using namespace std::string_literals;

// Compare two files byte-by-byte, 
// return early if bytes or IO state flags don't match
// @param a - full path to first file 
// @param b - full path to second file
// @return  - true if files match, false otherwise
auto isEqualFile(const path& a, const path& b) {
  if ((file_size(a) == 0 && file_size(b)) || (file_size(a) != file_size(b)))
    return false;

  std::ifstream fileA( a, std::ios::binary );
  std::ifstream fileB( b, std::ios::binary );

  char c{}, d{};
  while (fileA.read(reinterpret_cast<char*>(&c), sizeof c) 
      && fileB.read(reinterpret_cast<char*>(&d), sizeof d)) {
    if (c != d) return false; 
    if (!fileA.good() && fileB.good()) return false;
    if (fileA.good() && !fileB.good()) return false;
  } 

  return true;
}

auto removeUnique(std::vector<path>& v) {
  std::sort(v.begin(), v.end(), [](auto a, auto b) { return a < b; });
  auto last = std::unique(v.begin(), v.end(), [](auto a, auto b) {
    return a == b;
  });
  v.erase(last, v.end());
}

// returns a vector<pair<path, vector<path>>> where the
// paths are grouped to a filename
// where pair.first  = filename
// and   pair.second = vector of paths
auto groupPathToFile(std::vector<path>& duplicates) {
  auto prev_size = 0;
  auto temp_pair = std::pair<path, std::vector<path>>{};
  auto fin       = std::vector<std::pair<path, std::vector<path>>>{};

  std::sort(duplicates.begin(), duplicates.end(), [](auto a, auto b) { 
    return file_size(a) < file_size(b);
  });

  for (int i{0}; i != duplicates.size(); ++i) {
    if (i == 0) {
      prev_size       = file_size(duplicates[i]);
      temp_pair.first = duplicates[i].filename();
    } else if (prev_size != file_size(duplicates[i])) {
      fin.push_back(temp_pair);
      prev_size       = file_size(duplicates[i]);
      temp_pair       = std::pair<path, std::vector<path>>{};
      temp_pair.first = duplicates[i].filename();
    }
    temp_pair.second.emplace_back(duplicates[i]);
  }
  if (prev_size == file_size(duplicates[duplicates.size()-1]))
    fin.push_back(temp_pair);

  return fin;
}

auto printToFile(std::vector<std::pair<path, std::vector<path>>>& v) {
  std::sort(v.begin(), v.end(), [](auto a, auto b) {
    return a.first < b.first; 
  });

  auto output = std::ofstream{ "output.txt" };

  for (const auto& i : v) {
    output << i.first << '\n';
    for (const auto& j : i.second)
      output << '\t' << j.parent_path() << '\n';
  }
}

auto findDuplicates(std::vector<path>& v) {
  auto duplicates = std::vector<path>{};
  auto it         = v.begin();

  while ((it = std::adjacent_find(it, v.end(), isEqualFile)) != v.end()) {
    duplicates.emplace_back(*it);
    duplicates.emplace_back(*(it + 1));
    it++;
  }

  return duplicates;
}

auto displayUsage() {
  std::cerr << "Usage: <program> [directory path]\n";
  return -1;
}

auto displayNoPath() {
  std::cout << "path does not exist!\n";
  return -1;
}

auto displayEmpty() {
  std::cout << "directory is empty\n";
  return -1;
}

int main(int argc, char* argv[]) {
  auto p = path{};              // input path
  auto v = std::vector<path>{}; // holds all paths

  if (argc >  2) { return displayUsage();    }
  if (argc == 1) { p.assign(current_path()); }
  if (argc == 2) { p.assign(argv[1]);        }

  if (!exists       (p)) return displayNoPath();
  if (!is_directory (p)) return displayUsage();
  if (is_empty      (p)) return displayEmpty();

  for (auto& i : recursive_directory_iterator(p)) 
    if (is_regular_file(i)) 
      v.emplace_back(i.path());

  // sort array by size before searching for duplicates
  std::sort(v.begin(), v.end(), [](auto f1, auto f2) { 
    return file_size(f1) < file_size(f2);
  });

  auto duplicates = findDuplicates(v);
  removeUnique(duplicates);

  auto result = groupPathToFile(duplicates);
  printToFile(result);
}

