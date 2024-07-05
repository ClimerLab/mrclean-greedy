# mrclean-greedy
Cleans a text file of data by removing rows and columns until the percentage of missing data in each remaining row and columns is below a certain threshold.

A greedy algorithm is used to determine which rows and columns to remove. The percent of missing data in each retained row and column is calculated. The row/column with the largest percentage of missing data is selected. If a row is selected, the algorithm selects the number of columns, with missing data in the selected row, that need to be removed so that the amount of missing data in the row is below the threshold. The columns are selected so that the smallest amount of valid elements would be removed. If the number of valid elements in the row is less than the number of valid elements in the selected columns, the row is removed. Otherwise, the columns are removed. If a column has the largest percentage of missing data, the above process is repeated, but the rows and columns are swapped. After removing the rows(s) or column(s), the number of valid elements in each remaining row and column are recalculated and the process repeats until each remaining row and column have an acceptable amount of missing data.

## To Use
Compile with the Makefile by navigating to the root directory and entering: make

Run the program by entering: ./mrclean-greedy <data_file> <max_missing> <row_lb> <col_lb> <na_symbol> <output_path> (opt)<num_hr> (opt)<num_hc>

## Inputs
<data_file> - Path to data file

<max_missing> - Decimal value indicating maximum percentage of missing data in each row and column of the cleaned matrix

<row_lb> - Minimum number of rows allowed in a solution

<col_lb> - Minimum number of columns allowed in a solution

<na_symbol> - Symbol used to indicate missing values in the data_file

<output_path> - Directory to record results

<num_hr> - (Optional) Number of header rows in the data file. Defaults to 1 if no value is provided

<num_hc> - (Optional) Number of header columns in the data file. Defaults to 1 if no value is provided

## Outputs
### Greedy Summary
Greedy_summary.csv - File containing details of cleaning result. The following columns are recorded each time the program runs.

data_file - Input data file

max_perc_missing - Maximum percentage of missing data that the file was cleaned to

time - Run time of mrclean-greedy

num_val_elements - Number of valid elements in cleaned file

num_rows_kept - Number of data rows in cleaned matrix

num_cols_kept - Number of data columns in cleaned matrix


### Cleaned Data File
<output_path><data_file>\_gamma_<max_missing>_cleaned.tsv - File containing the cleaned data, along with the retained header rows and header columns.

### Retained Rows and Columns File
<output_path><data_file>\_gamma_<max_missing>_cleaned.sol - File containing two binary vectors indicating which rows and columns were retained. First vector corresponds to rows and the second to columns.

## Notes
The <data_file> should be tab seperated.

The original data file is unaltered.
