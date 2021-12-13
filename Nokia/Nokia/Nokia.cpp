#include "TableCVS.h"


int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        TableCVS main_table (argv[1]);
        main_table.print_table();
    }
    else
    {
        TableCVS main_table ("CsvTest.csv");
        main_table.print_table();
    }
    
    return 0;
    
}


