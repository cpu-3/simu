int cnt = 0;
class IO
{
    uint8_t led;

  public:
    void show_status()
    {
        printf("LED: %02x\n", led);
        std::cout << std::endl;
    }
    void write_led(uint8_t val)
    {
        led = val;
    }

    void transmit_uart(uint8_t val)
    {
        //std::cout << val;
        if (val == 0xa)
        {
            std::cerr << ++cnt << std::endl;
        }
    }

    uint8_t receive_uart()
    {
        return hook_getchar();
    }
};
