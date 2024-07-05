#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "GreedySolver.h"
#include "Timer.h"
#include "CleanSolution.h"
#include "BinContainer.h"
#include "AddRowGreedy.h"

void write_stats_to_file(const std::string &file_name,
                         const std::string &data_file,
                         const double max_perc_missing,
                         const double time,
                         const std::size_t num_valid_element,
                         const std::size_t num_rows_kept,
                         const std::size_t num_cols_kept);

int main(int argc, char *argv[]) {
  if (!((argc == 7) || (argc == 9))) {
    fprintf(stderr, "Usage: %s <data_file> <max_missing> <min_rows> <min_cols> <na_symbol> <output_path> (opt)<num_hr> (opt)<num_hc>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  std::string data_file(argv[1])  ;
  double max_perc_missing = std::stod(argv[2]);
  std::size_t min_rows = std::stoul(argv[3]);
  std::size_t min_cols = std::stoul(argv[4]);
  std::string na_symbol(argv[5]);
  std::string out_path(argv[6]);

  std::size_t num_header_rows = 1;
  std::size_t num_header_cols = 1;
  if (argc == 9) {
    num_header_rows = std::stoul(argv[7]);
    num_header_cols = std::stoul(argv[8]);
  }

  if ((max_perc_missing < 0) || (max_perc_missing > 1)) {
    throw std::runtime_error("max_perc_missing must be between [0,1].");
  }

  Timer timer;
  timer.start();

  BinContainer data(data_file, na_symbol, num_header_rows, num_header_cols);
  fprintf(stderr, "Num rows: %lu\n", data.get_num_data_rows());
  fprintf(stderr, "Num cols: %lu\n", data.get_num_data_cols());
  fprintf(stderr, "Num valid data: %lu\n", data.get_num_valid_data());
  fprintf(stderr, "Max percent missing: %lf\n\n", max_perc_missing);

  // Check that min_rows and min_cols are <= num_data_rows and num_data_cols
  if (min_rows > data.get_num_data_rows()) {
    fprintf(stderr, "ERROR - min_rows must be <= num_data_rows (%lu vs. %lu).\n", min_rows, data.get_num_data_rows());
    exit(EXIT_FAILURE);
  }
  if (min_cols > data.get_num_data_cols()) {
    fprintf(stderr, "ERROR - min_cols must be <= num_data_cols (%lu vs. %lu).\n", min_cols, data.get_num_data_cols());
    exit(EXIT_FAILURE);
  }
  
  CleanSolution sol(data.get_num_data_rows(), data.get_num_data_cols());

  GreedySolver greedy_solver(data, max_perc_missing, min_rows, min_cols);
  fprintf(stderr, "running greedy\n");
  greedy_solver.solve();
  sol.update(greedy_solver.get_rows_kept_as_bool(), greedy_solver.get_cols_kept_as_bool());

  if (max_perc_missing == 0.0) {
    AddRowGreedy ar_greedy(data, min_rows, min_cols);
    fprintf(stderr, "running add-row greedy\n");
    ar_greedy.solve();

    auto ar_rows_to_keep = ar_greedy.get_rows_to_keep();
    auto ar_cols_to_keep = ar_greedy.get_cols_to_keep();

    std::size_t ar_num_elements_kept = data.get_num_valid_data_kept(ar_rows_to_keep, ar_cols_to_keep);
    std::size_t greedy_num_elements_kept = data.get_num_valid_data_kept(sol.get_rows_to_keep(), sol.get_cols_to_keep());

    if (ar_num_elements_kept > greedy_num_elements_kept) {
      sol.update(ar_rows_to_keep, ar_cols_to_keep);
    }
  }

  timer.stop();

  auto rows_to_keep = sol.get_rows_to_keep();
  auto cols_to_keep = sol.get_cols_to_keep();

  double time = timer.elapsed_cpu_time();
  std::size_t num_val_elements = data.get_num_valid_data_kept(rows_to_keep, cols_to_keep);
  std::size_t num_rows_kept = sol.get_num_rows_kept();
  std::size_t num_cols_kept = sol.get_num_cols_kept(); 
  
  std::size_t file_start = data_file.find_last_of("/");
  std::string file_name = data_file.substr(file_start+1);

  std::size_t last_index = file_name.find_last_of(".");
  file_name = file_name.substr(0, last_index);
  std::stringstream gamma;
  gamma << std::fixed << std::setprecision(2) << max_perc_missing;
  std::string partial_file = out_path + file_name + "_gamma_" + gamma.str().c_str();

  std::string cleaned_file =  partial_file + "_cleaned.tsv";
  data.write_orig(cleaned_file, rows_to_keep, cols_to_keep);

  write_stats_to_file("Greedy_summary.csv", data_file, max_perc_missing, time, num_val_elements, num_rows_kept, num_cols_kept);

  // Write rows and cols kept
  std::string sol_file = partial_file + "_cleaned.sol";
  sol.write_to_file(sol_file);

  return 0;
}

//------------------------------------------------------------------------------
// Write the statistics to a file
//------------------------------------------------------------------------------
void write_stats_to_file(const std::string &file_name,
                         const std::string &data_file,
                         const double max_perc_missing,
                         const double time,
                         const std::size_t num_valid_element,
                         const std::size_t num_rows_kept,
                         const std::size_t num_cols_kept) {
  FILE *summary;
  
  if((summary = fopen(file_name.c_str(), "a+")) == nullptr) {
    fprintf(stderr, "Could not open file (%s)", file_name.c_str());
    exit(EXIT_FAILURE);
  }

  fprintf(summary, "%s,%lf,%lf,%lu,%lu,%lu\n", data_file.c_str(), max_perc_missing, time, num_valid_element, num_rows_kept, num_cols_kept);

  fclose(summary);
}
