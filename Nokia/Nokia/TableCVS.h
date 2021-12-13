#pragma once
#include <map>
#include <set>
#include <string>

/*
 * Обработка иссключений:
 *      + Проверка на соответсвие длины строк
 *      + Неправильные названия строк/столбцов - проверка на соответствие формату при добавлении
 *      + Пустые ячейки - логика взаимодействия при ссылке на неё +
 *      + Иссключение деление на 0
 *      + Иссключение Бессконечный цикл
 *      + Иссключение ссылка на пустую ячейку
 *      + Иссключение Ссылка на несуществующую ячейку
 * Изменение - обработка:
 * Вывод в консоль +
 */

enum ERRORE_CODE
{
    DIVISION_BY_ZERO,
    INVALID_EXPRESSION_FORMAT,
    UNKNOWN_LINK,
    INVALID_LINK_FORMAT,
    INFINITE_LOOP
};

struct cell
{
    std::string cell_data;
    int number;
    bool update {false};
    bool empty;
};

class TableCVS
{
    std::string file_input_;
    std::map<int, std::string> map_column_;
    std::map<int, std::string> map_line_;
    std::map<std::string, cell> map_cells_;
    char column_sep_;

    void cell_init(std::string cell_key, std::string cell_data);
    void table_init(std::string file_name);
    void table_update();
    void cell_update(std::string cur_cell);
    bool check_cell_name (std::string cell_name, int & number, std::set<std::string> &loop_detect);
    int calculate(int f_number, int s_number, char op_sign);
    
public:
    TableCVS (std::string file_name):
    file_input_(file_name)
    {
        table_init(file_input_);
        table_update();
    };

    void print_table();
};
