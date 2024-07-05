#ifndef ADD_ROW_GREEDY_H
#define ADD_ROW_GREEDY_H

#include <vector>
#include "BinContainer.h"

class AddRowGreedy {
private:
  const BinContainer *data;
  const std::size_t num_rows;
  const std::size_t num_cols;
  const std::size_t min_rows;
  const std::size_t min_cols;

  std::size_t best_obj_value;
  std::size_t best_num_rows;

  std::vector<bool> columns;
  std::size_t num_included_cols;
  std::vector<std::size_t> alphas;
  std::vector<std::size_t> excluded_rows;
  std::vector<std::size_t> included_rows;

  std::size_t calc_obj() const;

  std::size_t get_next_row();
  void include_row(const std::size_t row);
  void update_alphas(const std::size_t row);

public:
  AddRowGreedy(const BinContainer &_data,
               const std::size_t _min_rows,
               const std::size_t _min_cols);
  ~AddRowGreedy();

  void solve();

  std::vector<bool> get_rows_to_keep() const;
  std::vector<bool> get_cols_to_keep() const;
  std::size_t get_num_rows_to_keep() const;
  std::size_t get_num_cols_to_keep() const;
};

#endif
