#ifndef DATA_CONTAINER_H
#define DATA_CONTAINER_H

#include <vector>
#include <string>

class DataContainer
{
private:
  const std::string na_symbol;
  std::vector<std::vector<std::string>> header_rows;
  std::vector<std::vector<std::string>> header_cols;
  std::vector<std::vector<std::string>> data;
  std::vector<std::size_t> num_valid_rows;
  std::vector<std::size_t> num_valid_cols;

  void read(const std::string &file_name, const std::size_t num_header_rows, const std::size_t num_header_cols );
  void calc_num_valid();

public:
  DataContainer(const std::string &file_name, const std::string &_na_symbol, const std::size_t num_header_rows = 1, const std::size_t num_header_cols = 1);
  ~DataContainer();
  
  void print_binary(const std::string &filename) const;
  void write(const std::string &file_name,
             const std::vector<bool> &rows_to_keep,
             const std::vector<bool> &cols_to_keep) const;
  void write(const std::string &file_name,
             const std::vector<int> &rows_to_keep,
             const std::vector<int> &cols_to_keep) const;
  void write_transpose(const std::string &file_name) const;
  
  std::size_t get_num_header_rows() const;
  std::size_t get_num_header_cols() const;
  std::size_t get_num_data_rows() const;
  std::size_t get_num_data_cols() const;
  std::size_t get_num_data() const;
  
  bool is_data_na(const std::size_t i, const std::size_t j) const;

  std::size_t get_num_valid_data() const;
  std::size_t get_num_valid_data_kept(const std::vector<bool> &keep_row,
                                      const std::vector<bool> &keep_col) const;
  std::size_t get_num_valid_data_kept(const std::vector<int> &keep_row,
                                      const std::vector<int> &keep_col) const;
 
  double get_max_perc_miss_row() const;
  double get_max_perc_miss_col() const;
  double get_min_perc_miss_row() const;
  double get_min_perc_miss_col() const;

  std::size_t get_num_invalid_in_row(const std::size_t row) const;
  std::size_t get_num_invalid_in_col(const std::size_t col) const;
  std::size_t get_num_valid_in_row(const std::size_t row) const;
  std::size_t get_num_valid_in_col(const std::size_t col) const;
};

#endif