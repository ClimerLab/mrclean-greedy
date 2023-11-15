#ifndef BIN_CONTAINER_H
#define BIN_CONTAINER_H

#include <string>
#include <vector>
class BinContainer
{
private:
  const std::string file_name;
  const std::string na_symbol;
  const std::size_t num_header_rows;
  const std::size_t num_header_cols;

  std::vector<std::vector<bool>> data;
  
  void read();
  std::string trim(std::string &str) const;

public:  
  BinContainer(const std::string &_file_name,
               const std::string &_na_symbol,
               const std::size_t _num_header_rows = 1,
               const std::size_t _num_header_cols = 1);
  ~BinContainer();

  std::size_t get_num_header_rows() const;
  std::size_t get_num_header_cols() const;
  std::size_t get_num_data_rows() const;
  std::size_t get_num_data_cols() const;
  std::size_t get_num_data() const;
  std::size_t get_num_valid_data() const;
  std::size_t get_num_valid_data_kept(const std::vector<bool> &keep_row,
                                      const std::vector<bool> &keep_col) const;
  std::size_t get_num_valid_data_kept(const std::vector<int> &keep_row,
                                      const std::vector<int> &keep_col) const;

  bool is_data_na(const std::size_t i, const std::size_t j) const;

  void write_orig(const std::string &out_file,
                  const std::vector<bool> &rows_to_keep,
                  const std::vector<bool> &cols_to_keep) const;
  void write_orig(const std::string &out_file,
                  const std::vector<int> &rows_to_keep,
                  const std::vector<int> &cols_to_keep) const;
  
  void print_stats() const;
};

#endif