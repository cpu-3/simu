class Settings
{
    public:
    bool step_execution;
    bool show_stack;
    bool show_registers;
    bool show_inst_value;
    bool show_io;

    Settings(const char *cmd_arg) {
        step_execution = false;
        show_stack = false;
        show_registers = false;
        show_inst_value = false;
        show_io = false;

        for (const char *c = &cmd_arg[0]; *c; c++) {
            switch (*c) {
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
            case 'a':
                show_stack = true;
                show_registers = true;
                show_inst_value = true;
                show_io = true;
            }
        }
    }
};

