//
// Created by Nikita Gorojanin on 2019-04-25.
//
#include <string.h>
#include <string>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <unistd.h>
#include <signal.h>

#ifndef SHENN_SHELL_H
#define SHENN_SHELL_H

using namespace std;

using Duration = std::chrono::duration<double>;
using High_Res_Clock = std::chrono::high_resolution_clock;
using Time_Point = std::chrono::time_point<High_Res_Clock>;

/*!
 *  \brief Структура, хранящая результат выполнения команды
 *
 *  Данную структуру можно использовать если необходимо использовать результат выполнения команды,
 *  а не выводить его в консоль
 */
struct CmdExecResult {
    string success_result; ///< Хранит текст результата, в случае успешного выполнения команды
    string error_result; ///< Хранит текст ошибки, в случае неуспешного выполнения команды
    bool success=false; ///< Принимает значение true, в случае успешного выполнения команды, иначе false

    CmdExecResult()
            : success_result(""), error_result(""), success(false){};

    CmdExecResult(string success_result, string error_result, bool success=false)
            : success_result(move(success_result)), error_result(move(error_result)), success(success){};

};

/*!
 *  \brief Основной класс для работы с оболочкой SHELL
 *
 *  Данный класс предоставляет набор методов для выполнения команд языка shell и
 *  работы с результатами их выполнения
 */
class Shell {

public:
    using DelimetersArray = array<string, 11>;
private:
    const DelimetersArray delimeters = {"||", "|", "&&", ";", "1&>", "2&>", ">>", ">", "(", ")", "&"};

    Duration m_child_time_total;
    vector<string> parse_command_line(const string& command_line);
    void parse_command_line_for_pipe(const string &command_line, vector<string>& commands);
    void redirect(int oldfd, int newfd);
    bool is_delimeter(const string& str);
    bool validate_pipe_command(const string& cmd);

    Duration timeChild() const;

    CmdExecResult execute_cmd(char** parsed, const string& output_type="", const string& path="");
    CmdExecResult run_piped_cmd(vector<std::string>& commands);
public:
    Shell() : m_child_time_total(0){};

    /*!
        Выполняет надор команд разделенных операторами ||, && и ;,
        а также операторами вывода в файл >, >>, 1&>, 2&>
        \param[in] command_line Команда пользователя
    */
    void execute(const string& command_line);
    /*!
        Выполняет надор команд разделенных операторами ||, && и ;,
        а также операторами вывода в файл >, >>, 1&>, 2&>, возвращет результат в переменной result
        \param[in] command_line Команда пользователя
        \param[out] result Ссылка на экземпляр структуры CmdExecResult, хранящий результат выполнения команды
    */
    void execute(const string& command_line, CmdExecResult& result);

    /*!
        Выполняет надор команд разделенных операторами ||, && и ;,
        а также операторами вывода в файл >, >>, 1&>, 2&> в бекграунд режиме
        \param[in] command_line Команда пользователя
        \param[out] f коллбек-функция, принимающая ссылку на экземпляр стуктуры CmdExecResult,
        хранящий результат выполнения команды
    */
    void execute_background(string& command_line, function<void(CmdExecResult&)> f);

    /*!
        Выполняет надор команд разделенных операторами | (pipe)
        \param[in] command_line Команда пользователя
    */
    void execute_piped_command(string& command_line);
    /*!
        Выполняет надор команд разделенных операторами | (pipe), возвращет результат в переменной result
        \param[in] command_line Команда пользователя
        \param[out] result Ссылка на экземпляр структуры CmdExecResult, хранящий результат выполнения команды
    */
    void execute_piped_command(string& command_line, CmdExecResult& result);

    /*!
        Выполняет надор команд внутри опеаторов while, for, if
        \param[in] command_line Команда пользователя
        \param[out] result ссылка на вектор резльтатов от каждой, выполненной команды
    */
    void execute_conditional(string& command_line, vector<CmdExecResult>& result);

    /*!
        Выполняет надор команд внутри опеаторов while, for, if
        \param[in] command_line Команда пользователя
    */
    void execute_conditional(string& command_line);
};

#endif //SHENN_SHELL_H
