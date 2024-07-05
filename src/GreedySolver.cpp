#include "GreedySolver.h"
#include <assert.h>
#include <algorithm>
#include "MrCleanUtils.h"

//------------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------------
GreedySolver::GreedySolver(const BinContainer &_data,
                           const double _max_perc_miss,
                           const std::size_t _min_rows,
                           const std::size_t _min_cols) : data(&_data),
                                                          num_rows(data->get_num_data_rows()),
                                                          num_cols(data->get_num_data_cols()),
                                                          max_perc_miss(_max_perc_miss),
                                                          min_rows(_min_rows),
                                                          min_cols(_min_cols),
                                                          alphas(num_rows),
                                                          betas(num_cols),
                                                          keep_row(num_rows, true),
                                                          keep_col(num_cols, true),
                                                          num_rows_kept(num_rows),
                                                          num_cols_kept(num_cols) {
  calc_alphas();
  calc_betas();
}

//------------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------------
GreedySolver::~GreedySolver() {}

//------------------------------------------------------------------------------
// Run greedy solver.
//------------------------------------------------------------------------------
void GreedySolver::solve() {
  // Loop until matrix is cleaned or dimension limit is reached
  while (!matrix_cleaned()) {
    bool idx_is_row = true;
    std::vector<std::size_t> idx_to_remove;

    // Check if both dimension limits are reached
    if (get_num_rows_kept() == min_rows && get_num_cols_kept() == min_cols) {
      fprintf(stderr, "ERROR - Matrix is at dimension limit (%lu x %lu), but fails percent missing requirement\n", min_rows, min_cols);
      exit(EXIT_FAILURE);
    } else if (get_num_rows_kept() == min_rows) { // Row limit reached
      // Find row with most missing data
      std::size_t idx = num_rows;
      double worst_perc_miss = 0.0;
      for (std::size_t i = 0; i < num_rows; ++i) {
        if (keep_row[i] &&
            get_perc_miss_row(i) > max_perc_miss &&
            get_perc_miss_row(i) > worst_perc_miss) {
          worst_perc_miss = get_perc_miss_row(i);
          idx = i;
        }
      }

      if (idx == num_rows) {
        fprintf(stderr, "ERROR - Could not find row with missing data over threshold.\n");
        exit(EXIT_FAILURE);
      }

      // Calculate the number of columns that need to be removed so that the percent of missing data
      // in the row is <= the maximum amount allowed
      std::size_t k = calc_num_cols_to_remove(idx);

      // Get all columns with missing data for the desired row
      auto colsWithMissingData = get_missing_cols(idx);

      // Verify that there are enough columns to remove
      if (k > colsWithMissingData.size()) {
        fprintf(stderr, "ERROR - need to remove more missing data than is present (%lu vs. %lu).\n", k, colsWithMissingData.size());
        exit(EXIT_FAILURE);
      }

      // Sort the columns with missing data based on the number of valid elements in each column
      std::vector<std::pair<std::size_t, std::size_t>> sortedCols;
      for (auto j : colsWithMissingData) {
        sortedCols.push_back(std::make_pair(j, betas[j]));
      }
      std::sort(sortedCols.begin(), sortedCols.end(), mr_clean_utils::SortPairBySecondItemDecreasing());

      // Add columns to 'idx_to_remove' and set flag indicating columns
      for (std::size_t j = 0; j < k; j++) {
        idx_to_remove.push_back(sortedCols[j].first);
      }
      idx_is_row = false;

    } else if (get_num_cols_kept() == min_cols) { // Column limit reached
      // Find columns with most missing data
      std::size_t idx = num_cols;
      double worst_perc_miss = 0.0;
      for (std::size_t j = 0; j < num_cols; ++j) {
        if (keep_col[j] &&
            get_perc_miss_col(j) > max_perc_miss &&
            get_perc_miss_col(j) > worst_perc_miss) {
          worst_perc_miss = get_perc_miss_col(j);
          idx = j;
        }
      }

      // Check that valid column was found
      if (idx == num_cols) {
        fprintf(stderr, "ERROR - Could not find column with missing data over threshold.\n");
        exit(EXIT_FAILURE);
      }

      // Calculate the number of rows that need to be removed so that the percent of missing data
      // in the column is <= the maximum amount allowed
      std::size_t k = calc_num_rows_to_remove(idx);

      // Get all rows with missing data for the desired column
      auto rowsWithMissingData = get_missing_rows(idx);

      // Verify that there are enough rows to remove
      if (k > rowsWithMissingData.size()) {
        fprintf(stderr, "ERROR - Need to remove more missing data than is present (%lu vs. %lu).\n", k, rowsWithMissingData.size());
        exit(EXIT_FAILURE);
      }

      // Sort the rows with missing data based on the number of valid elements in each row
      std::vector<std::pair<std::size_t, std::size_t>> sortedRows;
      for (auto i : rowsWithMissingData) {
        sortedRows.push_back(std::make_pair(i, alphas[i]));
      }
      std::sort(sortedRows.begin(), sortedRows.end(), mr_clean_utils::SortPairBySecondItemDecreasing());

      // Add rows to 'idx_to_remove' and set flag indicating rows
      for (std::size_t i = 0; i < k; i++) {
        idx_to_remove.push_back(sortedRows[i].first);
      }
      idx_is_row = true;

    } else { // No limit reached
      // Reset values
      bool found_row_col_to_remove = false;
      bool row = false;
      std::size_t idx = 0;
      double worse_perc_miss = 0.0;

      // Loop through all rows checking for a row that is 1) valid, 2) whose percentange of missing data is >
      // the maximum allowed, 3) has the highest percent of missing data. If a row is found save information
      // for future use.
      for (std::size_t i = 0; i < num_rows; ++i) {
        if (keep_row[i] &&
            get_perc_miss_row(i) > max_perc_miss &&
            get_perc_miss_row(i) > worse_perc_miss) {
          worse_perc_miss = get_perc_miss_row(i);
          row = true;
          idx = i;
          found_row_col_to_remove = true;
        }
      }
   
      // Loop through all columns checking for a columns that is 1) valid, 2) whose percentange of missing data
      // is > the maximum allowed, 3) has the highest percent of missing data. If a column is found save
      // information for future use.
      for (std::size_t j = 0; j < num_cols; ++j) {
        if (keep_col[j] &&
            get_perc_miss_col(j) > max_perc_miss &&
            get_perc_miss_col(j) > worse_perc_miss) {
          worse_perc_miss = get_perc_miss_col(j);
          row = false;
          idx = j;
          found_row_col_to_remove = true;
        }
      }

      // If no row or column was found above that matches the 3 criteria report error
      if (!found_row_col_to_remove) {
        fprintf(stderr, "ERROR - Could not find row or column over max_perc_miss limit.\n");
        exit(EXIT_FAILURE);
      }

      // A row was found that matched all 3 criteria
      if (row) {
        // Save the number of valid elements that would be removed if the row was removed
        std::size_t validRemoved = alphas[idx];

        // Calculate the number of columns that need to be removed so that the percent of missing data
        // in the row is <= the maximum amount allowed
        std::size_t k = calc_num_cols_to_remove(idx);

        // Get all columns with missing data for the desired row
        auto colsWithMissingData = get_missing_cols(idx);

        // Verify that there are enough columns to remove
        if (k > colsWithMissingData.size()) {
          fprintf(stderr, "ERROR - need to remove more missing data than is present (%lu vs. %lu).\n", k, colsWithMissingData.size());
          exit(EXIT_FAILURE);
        }

        // Sort the columns with missing data based on the number of valid elements in each column
        std::vector<std::pair<std::size_t, std::size_t>> sortedCols;
        for (auto j : colsWithMissingData) {
          sortedCols.push_back(std::make_pair(j, betas[j]));
        }
        std::sort(sortedCols.begin(), sortedCols.end(), mr_clean_utils::SortPairBySecondItemDecreasing());
      
        // Calculate the number of valid elements that would be removed if the 'k' columns with the least amount of valid elements
        // are removed.
        std::size_t sumRemoved = 0;
        for (std::size_t j_col = 0; j_col < k; ++j_col) {
          sumRemoved += sortedCols[j_col].second;
        }

        // Check if removing the row or k columns results in the loss of more valid elements
        if (sumRemoved < validRemoved) {
          // Remove the k columns
          for (std::size_t j_col = 0; j_col < k; ++j_col) {
            idx_to_remove.push_back(sortedCols[j_col].first);
          }
          idx_is_row = false;
        } else { // Remove the row
          idx_to_remove.push_back(idx);
          idx_is_row = true;
        }      
      } else {
        // Save the number of valid elements that would be removed if the column was removed
        std::size_t validRemoved = betas[idx];

        // Calculate the number of rows that need to be removed so that the percent of missing data
        // in the column is <= the maximum amount allowed
        std::size_t k = calc_num_rows_to_remove(idx);

        // Get all rows with missing data for the desired row
        auto rowsWithMissingData = get_missing_rows(idx);

        // Verify that there are enough rows to remove
        if (k > rowsWithMissingData.size()) {
          fprintf(stderr, "ERROR - need to remove more missing data than is present (%lu vs. %lu).\n", k, rowsWithMissingData.size());
          exit(EXIT_FAILURE);
        }

        // Sort the rows with missing data based on the number of valid elements in each row
        std::vector<std::pair<std::size_t, std::size_t>> sortedRows;
        for (auto i : rowsWithMissingData) {
          sortedRows.push_back(std::make_pair(i, alphas[i]));
        }
        std::sort(sortedRows.begin(), sortedRows.end(), mr_clean_utils::SortPairBySecondItemDecreasing());
      
        // Calculate the number of valid elements that would be removed if the 'k' rows with the least amount of valid elements
        // are removed.
        std::size_t sumRemoved = 0;
        for (std::size_t i_row = 0; i_row < k; ++i_row) {
          sumRemoved += sortedRows[i_row].second;
        }

        // Check if removing the column or k rows results in the loss of more valid elements
        if (sumRemoved < validRemoved) {
          // Remove the k rows
          for (std::size_t i_row = 0; i_row < k; ++i_row) {
            idx_to_remove.push_back(sortedRows[i_row].first);
            idx_is_row = true;
          }
        } else { // Remove column
          idx_to_remove.push_back(idx);
          idx_is_row = false;
        }
      }
    }

    // Check that some rows or columns were selected for removal
    if (idx_to_remove.empty()) {
      fprintf(stderr, "No rows or columns were selected for removal\n");
      exit(EXIT_FAILURE);
    }

    // Determine if rows or columns are selected for removal
    if (idx_is_row) { // Remove rows
      // Loop through all rows to remove
      for (auto idx : idx_to_remove) {
        // Check that row limit has not been reached
        if (get_num_rows_kept() > min_rows) {
          remove_row(idx);
          update_cols(idx);
        }
      }
    } else { // Remove columns
      // Loop through all columns to remove
      for (auto idx : idx_to_remove) {
        // Check that columns limit has not been reached
        if (get_num_cols_kept() > min_cols) {
          remove_col(idx);
          update_rows(idx);
        }
      }    
    }
  }
}

//------------------------------------------------------------------------------
// Calculates the number of valid elements in each row.
//------------------------------------------------------------------------------
void GreedySolver::calc_alphas() {
  for (std::size_t i = 0; i < num_rows; ++i) {
    alphas[i] = 0;

    for (std::size_t j = 0; j < num_cols; ++j) {
      if (keep_col[j] && !data->is_data_na(i,j)) {
        ++alphas[i];
      }
    }
  }
}

//------------------------------------------------------------------------------
// Calculates the number of valid elements in each column.
//------------------------------------------------------------------------------
void GreedySolver::calc_betas() {
  for (std::size_t j = 0; j < num_cols; ++j) {
    betas[j] = 0;

    for (std::size_t i = 0; i < num_rows; ++i) {
      if (keep_row[i] && !data->is_data_na(i,j)) {
        ++betas[j];
      }
    }
  }
}

//------------------------------------------------------------------------------
// Remove a row from the solution by updating the boolean vector and decreasing
// the number of rows kept counter.
//------------------------------------------------------------------------------
void GreedySolver::remove_row(const std::size_t idx) {
  assert(idx < num_rows);

  if (keep_row[idx]) {
    --num_rows_kept;
  }
  keep_row[idx] = false;
}

//------------------------------------------------------------------------------
// Remove a column from the solution by updating the boolean vector and
// decreasing the number of rows kept counter.
//------------------------------------------------------------------------------
void GreedySolver::remove_col(const std::size_t idx) {
  assert(idx < num_cols);

  if (keep_col[idx]) {
    --num_cols_kept;
  }
  keep_col[idx] = false;
}

//------------------------------------------------------------------------------
// Update the number of valid elements in each row based on the column that was
// removed.
//------------------------------------------------------------------------------
void GreedySolver::update_rows(const std::size_t removed_col) {
  for (std::size_t i = 0; i < num_rows; ++i) {
    if (keep_row[i]) {
      if (!data->is_data_na(i, removed_col)) {
        --alphas[i];
      }
    }
  }
}

//------------------------------------------------------------------------------
// Update the number of valid elements in each column based on the row that was
// removed.
//------------------------------------------------------------------------------
void GreedySolver::update_cols(const std::size_t removed_row) {
  for (std::size_t j = 0; j < num_cols; ++j) {
    if (keep_col[j]) {
      if (!data->is_data_na(removed_row,j)) {
        --betas[j];
      }
    }
  }
}

//------------------------------------------------------------------------------
// Get the number of invalid elements in the desired row.
//------------------------------------------------------------------------------
std::size_t GreedySolver::get_num_missing_row(const std::size_t idx) const {
  assert(idx < num_rows);
  return num_cols_kept - alphas[idx];
}

//------------------------------------------------------------------------------
// Get the number of invalid elements in the desired column.
//------------------------------------------------------------------------------
std::size_t GreedySolver::get_num_missing_col(const std::size_t colIdx) const {
 assert(colIdx < num_cols);
  return num_rows_kept - betas[colIdx];
}

//------------------------------------------------------------------------------
// Return the percentage of kept elements thare are missing for the desired row
//------------------------------------------------------------------------------
double GreedySolver::get_perc_miss_row(const std::size_t idx) const {
  assert(idx < num_rows);
  return static_cast<double>(get_num_missing_row(idx)) / num_cols_kept;
}

//------------------------------------------------------------------------------
// Return the percentage of kept elements thare are missing for the desired
// column
//------------------------------------------------------------------------------
double GreedySolver::get_perc_miss_col(const std::size_t colIdx) const {
  assert(colIdx < num_cols);
  return static_cast<double>(get_num_missing_col(colIdx)) / num_rows_kept;
}

//------------------------------------------------------------------------------
// Returns a vector that contains the indices of columns that are both valid
// and contain a missing element in the provided row.
//------------------------------------------------------------------------------
std::vector<std::size_t> GreedySolver::get_missing_cols(const std::size_t rowIdx) const {
  assert(rowIdx < num_rows);
  std::vector<std::size_t> missing;  

  for (std::size_t j = 0; j < num_cols; ++j) {
    if (keep_col[j] && data->is_data_na(rowIdx, j)) {
      missing.push_back(j);
    }
  }

  return missing;
}

//------------------------------------------------------------------------------
// Returns a vector that contains the indices of rows that are both valid and
// contain a missing element in the provided column.
//------------------------------------------------------------------------------
std::vector<std::size_t> GreedySolver::get_missing_rows(const std::size_t colIdx) const {
  assert(colIdx < num_cols);
  std::vector<std::size_t> missing;  

  for (std::size_t i = 0; i < num_rows; ++i) {
    if (keep_row[i] && data->is_data_na(i, colIdx)) {
      missing.push_back(i);
    }
  }

  return missing;
}

//------------------------------------------------------------------------------
// Returns the number of missing elements that need to be removed from the 
// desired column so that the percent of missing elements is <= the maximum
// percent missing.
//------------------------------------------------------------------------------
std::size_t GreedySolver::calc_num_rows_to_remove(const std::size_t colIdx) const {
  double tmpNumMissing = static_cast<double>(get_num_missing_col(colIdx));  
  std::size_t tmpNumRows = num_rows_kept;

  while (tmpNumMissing / tmpNumRows > max_perc_miss) {
    --tmpNumMissing;
    --tmpNumRows;
  }

  return num_rows_kept - tmpNumRows;
}

//------------------------------------------------------------------------------
// Returns the number of missing elements that need to be removed from the 
// desired row so that the percent of missing elements is <= the maximum
// percent missing.
//------------------------------------------------------------------------------
std::size_t GreedySolver::calc_num_cols_to_remove(const std::size_t idx) const {
  double tmpNumMissing = static_cast<double>(get_num_missing_row(idx));
  std::size_t tmpNumCols = num_cols_kept;

  while (tmpNumMissing / tmpNumCols > max_perc_miss) {
    --tmpNumMissing;
    --tmpNumCols;
  }

  return num_cols_kept - tmpNumCols;
}

//------------------------------------------------------------------------------
// Returns the number of rows kept in the current solution.
//------------------------------------------------------------------------------
std::size_t GreedySolver::get_num_rows_kept() const {
  std::size_t count = 0;
  for (auto r : keep_row) {
    if (r) {
      ++count;
    }
  }
  return count;
}

//------------------------------------------------------------------------------
// Returns the number of columns kept in the current solution.
//------------------------------------------------------------------------------
std::size_t GreedySolver::get_num_cols_kept() const {
  std::size_t count = 0;
  for (auto c : keep_col) {
    if (c) {
      ++count;
    }
  }
  return count;
}

//------------------------------------------------------------------------------
// Returns a boolean vector where elements are 'true' if the corresponding row
// is kept and 'false' if the row is removed.
//------------------------------------------------------------------------------
std::vector<bool> GreedySolver::get_rows_kept_as_bool() const {
  std::vector<bool> tmp(num_rows);
  for (std::size_t i = 0; i < keep_row.size(); ++i) {
    tmp[i] = keep_row[i];
  }
  return tmp;
}

//------------------------------------------------------------------------------
// Returns a boolean vector where elements are 'true' if the corresponding
// column is kept and 'false' if the column is removed.
//------------------------------------------------------------------------------
std::vector<bool> GreedySolver::get_cols_kept_as_bool() const {
  std::vector<bool> tmp(num_cols);
  for (std::size_t j = 0; j < keep_col.size(); ++j) {
    tmp[j] = keep_col[j];
  }
  return tmp;
}

bool GreedySolver::matrix_cleaned() const {
  // Check that all remaining rows meet max_perc_miss requirement
  for (std::size_t i = 0; i < num_rows; ++i) {
    if (keep_row[i] && (get_perc_miss_row(i) > max_perc_miss)) {
      return false;
    }
  }

  // Check that all remaining columns meet max_perc_miss requirement
  for (std::size_t j = 0; j < num_cols; ++j) {
    if (keep_col[j] && (get_perc_miss_col(j) > max_perc_miss)) {
      return false;
    }
  }

  // All remaining rows and columns meet requirements
  return true;
}