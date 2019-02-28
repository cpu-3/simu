class Memory
{
    /* Current Memory Map
    0x0     --------
              Inst
    0x1ffff  --------
              IO
    0x20fff --------
              Data
    0xf4240 --------
    */
    static const uint32_t memory_size = 0xf4240;
    static const uint32_t inst_mem_lim = 0x1ffff;
    static const uint32_t IO_mem_lim = 0x20fff;
    static const uint32_t memory_base = 0;
    static const uint32_t memory_lim = memory_base + memory_size;

    static const uint32_t uart_rx_addr = 0x20000;
    static const uint32_t uart_tx_addr = 0x20004;
    static const uint32_t led_addr = 0x20008;

    uint8_t memory[memory_size];
    IO *io;

    void addr_alignment_check(uint32_t addr)
    {
        if (addr % 4 != 0)
        {
            error_dump("メモリアドレスのアラインメントがおかしいです: %x\n", addr);
        }
    }

    void data_mem_check(uint32_t addr, uint8_t size)
    {
        if (addr + 4 > memory_size)
        {
            error_dump("多分不正なアドレスにアクセスしようとしました: %x\n", addr);
        }
    }

    void read_mem_check(uint32_t addr, uint8_t size)
    {
    }

    void inst_mem_check(uint32_t addr)
    {
        addr_alignment_check(addr);
        if (addr + 4 > inst_mem_lim)
        {
            error_dump("多分不正な命令アドレスにアクセスしようとしました: %x\n", addr);
        }
    }

    void map_mem_check(uint32_t addr, uint32_t size)
    {
        if ((addr + size) >= memory_size)
        {
            error_dump("多分不正なアドレスにアクセスしようとしました: %x\n", addr);
        }
    }

    bool hook_io_write(uint32_t addr, uint8_t val)
    {
        if (addr == uart_rx_addr)
        {
            error_dump("uartの読み込みポートに書き込みを試みました");
            return true;
        }
        else if (addr == uart_tx_addr)
        {
            io->transmit_uart(val);
        }
        else if (addr == led_addr)
        {
            io->write_led(val);
        }
        else
        {
            return false;
        }
        return true;
    }

    bool hook_io_read(uint32_t addr, uint8_t *v)
    {
        if (addr == uart_rx_addr)
        {
            *v = io->receive_uart();
        }
        else if (addr == uart_tx_addr)
        {
            error_dump("uartの書き込みポートを読み込もうとしました");
        }
        else if (addr == led_addr)
        {
            error_dump("ledの値を読み取ろうとしました");
        }
        else
        {
            return false;
        }
        return true;
    }

  public:
    Memory(IO *io)
    {
        this->io = io;
    }

    void write_mem(uint32_t addr, uint8_t val)
    {
        addr *= 4;
        if (!hook_io_write(addr, val))
        {
            data_mem_check(addr, 1);
            memory[addr] = val;
        }
    }

    void write_mem(uint32_t addr, uint16_t val)
    {
        addr *= 4;
        if (!hook_io_write(addr, val))
        {
            data_mem_check(addr, 2);
            uint16_t *m = (uint16_t *)memory;
            m[addr / 2] = val;
        }
    }

    void write_mem(uint32_t addr, uint32_t val)
    {
        addr *= 4;
        if (!hook_io_write(addr, val))
        {
            data_mem_check(addr, 4);
            uint32_t *m = (uint32_t *)memory;
            m[addr / 4] = val;
        }
    }

    uint8_t read_mem_1(uint32_t addr)
    {
        addr *= 4;
        uint8_t v;
        if (hook_io_read(addr, &v))
        {
            return v;
        }
        read_mem_check(addr, 1);
        return memory[addr];
    }

    uint16_t read_mem_2(uint32_t addr)
    {
        addr *= 4;
        uint8_t v;
        if (hook_io_read(addr, &v))
        {
            return v;
        }
        read_mem_check(addr, 2);
        uint16_t *m = (uint16_t *)memory;
        return m[addr / 2];
    }

    uint32_t read_mem_4(uint32_t addr)
    {
        addr *= 4;
        uint8_t v;
        if (hook_io_read(addr, &v))
        {
            return v;
        }
        read_mem_check(addr, 4);
        uint32_t *m = (uint32_t *)memory;
        return m[addr / 4];
    }

    uint32_t get_inst(uint32_t addr)
    {
        addr *= 4;
        inst_mem_check(addr);
        uint32_t *m = (uint32_t *)memory;
        return m[addr / 4];
    }

    // set instructions to memory
    void mmap(uint32_t addr, uint8_t *data, uint32_t length)
    {
        addr_alignment_check(addr);
        addr_alignment_check(length);
        for (int i = 0; i < length; i++)
        {
            memory[addr + i] = data[i];
        }
    }

    void show_data(uint32_t addr, uint32_t length)
    {
        int cnt = 0;
        for (uint32_t ad = addr; ad < addr + length; ad += 4)
        {
            if (ad + 4 >= memory_lim)
            {
                break;
            }
            uint32_t v = read_mem_4(ad);
            printf("%08x: %08x\n", ad, v);
        }
        std::cout << std::endl;
    }

    void dump_heap()
    {
        show_data(0x21000, 0x1000);
    }
};

typedef union {
    uint32_t i;
    float f;
} float_int;

int f2i(float x)
{
    float_int data;
    data.f = x;
    return data.i;
}

float i2f(int x)
{
    float_int data;
    data.i = x;
    return data.f;
}

class Register
{
    static const int ireg_size = 32;
    static const int freg_size = 32;
    uint32_t i_registers[ireg_size] = {0};
    uint32_t f_registers[freg_size] = {0};

    static void check_ireg_name(int name, int write)
    {
        if (name < 0 || name > ireg_size)
        {
            error_dump("レジスタの番号が不正です: %d\n", name);
        }
        /*
        if (write && name == 0)
        {
            warn_dump("レジスタ0に書き込もうとしていますが\n");
        }
        */
    }
    static void check_freg_name(int name)
    {
        if (name < 0 || name > freg_size)
        {
            error_dump("レジスタの番号が不正です: %d\n", name);
        }
    }

  public:
    uint32_t ip;
    Register() : ip(0) {}
    void set_ireg(int name, uint32_t val)
    {
        check_ireg_name(name, 1);

        if (name == 0)
        {
            return;
        }
        i_registers[name] = val;
    }
    void set_freg(int name, float val)
    {
        check_freg_name(name);

        f_registers[name] = f2i(val);
    }
    void set_freg_raw(int name, uint32_t val)
    {
        check_freg_name(name);

        f_registers[name] = val;
    }
    uint32_t get_ireg(int name)
    {
        check_ireg_name(name, 0);
        if (name == 0)
        {
            return 0;
        }
        return i_registers[name];
    }
    float get_freg(int name)
    {
        check_freg_name(name);
        return i2f(f_registers[name]);
    }
    uint32_t get_freg_raw(int name)
    {
        check_freg_name(name);
        return f_registers[name];
    }

    void info()
    {
        std::cout << "iRegister: " << std::endl;
        std::cout << std::hex;
        for (int i = 0; i < ireg_size; i++)
        {
            std::cout << std::dec << "x" << i << std::hex << ": " << i_registers[i] << " ";
            if (i % 6 == 5)
            {
                std::cout << std::endl;
            }
        }
        std::cout << std::endl
                  << std::endl;
        std::cout << "fRegister: " << std::endl;
        for (int i = 0; i < freg_size; i++)
        {
            std::cout << std::dec << "f" << i << std::hex << ": " << i2f(f_registers[i]) << " ";
            if (i % 6 == 5)
            {
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
        std::cout << std::dec;
    }
};
