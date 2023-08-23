#ifndef CLEAN_SOLUTION_H
#define CLEAN_SOLUTION_H

#include <vector>
#include <string>

class CleanSolution
{
private:
  std::vector<bool> rows_to_keep;
  std::vector<bool> cols_to_keep;

public:
  CleanSolution(const std::size_t num_rows,
                const std::size_t num_cols);
  CleanSolution(const std::vector<bool> &_rows_to_keep,
                const std::vector<bool> &_cols_to_keep);
  ~CleanSolution();

  void update(const std::vector<bool> &_rows_to_keep,
              const std::vector<bool> &_cols_to_keep);
  void write_to_file(const std::string &file_name);
  void read_from_file(const std::string &file_name);

  std::vector<bool> get_rows_to_keep() const;
  std::vector<bool> get_cols_to_keep() const;

  std::size_t get_num_rows_kept() const;
  std::size_t get_num_cols_kept() const;
};

#endif