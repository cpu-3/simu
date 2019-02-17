std::string input;
bool input_flag;
int input_index = 0;

char hook_getchar()
{
    if (input_flag)
    {
        return input.at(input_index++);
    }
    else
    {
        return getchar();
    }
}

class Settings
{
  public:
    bool break_point;
    bool step_execution;
    bool show_stack;
    bool show_registers;
    bool show_inst_value;
    bool show_io;
    bool hide_error_dump;
    int ip;
    unsigned long long wait;

    std::string input_data;
    bool input_data_flag;

    Settings(const char *cmd_arg, const int x, unsigned long long y, std::string filename)
    {
        break_point = false;
        step_execution = false;
        show_stack = false;
        show_registers = false;
        show_inst_value = false;
        show_io = false;
        hide_error_dump = false;
        ip = x;
        wait = y;

        if (filename != std::string(""))
        {
            std::ifstream ifs(filename);

            std::stringstream strstream;
            strstream << ifs.rdbuf();
            ifs.close();
            std::string data(strstream.str());
            input = data;
            input_flag = true;
        }
        else
        {
            input_flag = false;
        }

        for (const char *c = &cmd_arg[0]; *c; c++)
        {
            switch (*c)
            {
            case 'b':
                break_point = true;
                break;
            case 's':
                step_execution = true;
                break;
            case 't':
                show_stack = true;
                break;
            case 'r':
                show_registers = true;
                break;
            case 'i':
                show_inst_value = true;
                break;
            case 'o':
                show_io = true;
                break;
            case 'h':
                hide_error_dump = true;
                break;
            case 'a':
                show_stack = true;
                show_registers = true;
                show_inst_value = true;
                show_io = true;
            }
        }
    }
};
