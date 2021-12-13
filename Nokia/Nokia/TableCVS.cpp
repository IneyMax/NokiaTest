#include "TableCVS.h"

#include <regex>
#include <fstream>
#include <iostream>


void TableCVS::cell_init(std::string cell_key, std::string cell_data)
{
    if (!cell_data.empty())
    {
        map_cells_[cell_key].cell_data = cell_data;
        std::regex number ("([\\d]*)"); // Для извлечения готовых чисел
        if (std::regex_match(cell_data, number))
        {
            map_cells_[cell_key].number = stol(cell_data);
        }
        else 
        {
            map_cells_[cell_key].update = true;
        }
        map_cells_[cell_key].empty = false;
    }
    else
    {
        map_cells_[cell_key].empty = true;
        map_cells_[cell_key].cell_data = "EMPTY!";
    }
}


void TableCVS::table_init(std::string file_name)
{
    std::ifstream input_stream (file_name);
    if (input_stream.is_open())
    {
        std::cout << "File open!\n";
    }
    column_sep_ = input_stream.peek(); // Для извлечения первого символа строки, чтобы в дальнейшем разделять колонки
    std::string buffer_string; // Временное хранение извлекаемых строк

    // Регулярное выражение:
    std::cmatch rezult;
    std::regex r_line_end ("([\\w]*)([\n]{1})([\\w]*)"); // Для разделения строк при извлечении
    
    // Анализ первой строки для создания колонок
    int index_column {0};
    int max_index_column {};
    while (std::getline(input_stream, buffer_string, column_sep_)) // Извлекаем первую строку
        {
        if (!buffer_string.empty())
        {
            if (std::regex_match(buffer_string.c_str(), rezult, r_line_end))
            {
                map_column_[index_column] = rezult[1];
                max_index_column = index_column;
                index_column = 0;
                break;
            }
            map_column_[index_column] = buffer_string;
            index_column ++;
        }
        }

    // Работа со строками
    std::string name_line {rezult[3]};
    int index_line {};
    map_line_[index_line] = name_line;
    while (std::getline(input_stream, buffer_string, column_sep_))
    {
        if (!buffer_string.empty())
        {
            if (std::regex_match(buffer_string.c_str(), rezult, r_line_end))
            {
                index_line ++;
                if (index_column <= max_index_column) // отрезаем лишние клетки в рамках линии
                {
                    cell_init(map_column_[index_column] + name_line, rezult[1]);
                }
                name_line = rezult[3];
                map_line_[index_line] = name_line;
                index_column = 0;
            }
            else if (index_column <= max_index_column)
            {
                cell_init(map_column_[index_column] + name_line, buffer_string);
                index_column ++;
            }
        }
        else
        {
            cell_init(map_column_[index_column] + name_line, buffer_string);
            index_column ++;
        }
    }

    input_stream.close();
    if (!input_stream.is_open())
    {
        std::cout << "File close!\n";
    }
}


void TableCVS::table_update()
{
    // Вызываем метод обновления для каждой ячейки без простых чисел
    for (auto element : map_cells_)
    {
        if (element.second.update)
        {
            try
            {
                cell_update(element.first);
            }
            catch (ERRORE_CODE)
            {
                std::cout << "DIVISION_BY_ZERO";
            }
        }
    }
}


// Обновление ячейки
void TableCVS::cell_update(std::string cur_cell)
{
    std::string cell_data {map_cells_[cur_cell].cell_data};
    std::regex expression ("([=]{1})([\\w]*)([\\W]{1})([\\w]*)");
    std::cmatch rezult;
    if (std::regex_match(cell_data.c_str(), rezult, expression))
    {
        int first_number {};
        int second_number {};
        std::set<std::string> inf_loop_detected;
        if (check_cell_name(rezult[2], first_number, inf_loop_detected) && check_cell_name(rezult[4], second_number, inf_loop_detected))
        {     
            map_cells_[cur_cell].number = calculate(first_number, second_number, rezult[3].first[0]);
            map_cells_[cur_cell].cell_data = std::to_string(map_cells_[cur_cell].number);
            map_cells_[cur_cell].update = false;
        }
    }
}


// Проверка имени ячейки на соответствие стандарту
bool TableCVS::check_cell_name(std::string cell_name, int & number, std::set<std::string> & loop_detect)
{
    if (loop_detect.count(cell_name))
    {
        std::regex line_column ("([\\D]*)([\\d]*)");
        if (std::regex_match(cell_name, line_column))
        {
            if (map_cells_.count(cell_name))
            {
                if (!map_cells_[cell_name].update && !map_cells_[cell_name].empty)
                {
                    number = map_cells_[cell_name].number;
                }
                else
                {
                    loop_detect.emplace(cell_name);
                    cell_update(cell_name);
                }
                return true;
            }
        }
    }
    return false;
}


// Выполненеи арифметических операций
int TableCVS::calculate(int f_number, int s_number, char op_sign)
{
    switch (op_sign)
    {
    case '+':
        return f_number + s_number;
    case '-':
        return f_number - s_number;
    case '*':
        return f_number * s_number;
    case '/':
        if (s_number == 0)
        {
            throw DIVISION_BY_ZERO;
        }
        return f_number / s_number;
    case '%':
        return f_number % s_number;
    default:
        std::cout << "calculate(): Unhandled case\n";
        return 0;
    }
}


void TableCVS::print_table()
{
    for (auto element_line : map_line_)
    {
        for (auto element_column : map_column_)
        {
            std::cout << " (" << element_column.second + element_line.second << " : " << map_cells_[element_column.second + element_line.second].cell_data << ") ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


