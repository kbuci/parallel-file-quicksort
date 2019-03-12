# parallel-file-quicksort
My in-progress code to implement quicksort in C++ for sorting a  CSV file, utilizing multiple threads

# Instructions:
-run the "make" command
-format for running the program:
quicksorter src-file dest-file "columns to sort by" #threads 
ex) quicksorter x.csv y.csv "1,0" 2
would sort x.csv with 2 threads and write results to y.csv, sorting by column 1 then column 0
