#include <libsystem/BuildInfo.h>
#include <libsystem/utils/NumberFormatter.h>
#include <libwidget/Application.h>
#include <libwidget/Markup.h>
#include <libwidget/Widgets.h>
#include <string.h>

Window *window;

#define CALCULATOR_BUTTON(stringcodename, is_number, code)         \
    target->with_widget<Button>(stringcodename, [&](auto button) { \
        button->on(Event::ACTION, [&](auto) {                      \
            calculator_update(is_number, code);                    \
        });                                                        \
    });

enum CALCULATOR_OPERATOR
{
    OPERATOR_ADD = 0,
    OPERATOR_SUBSTRACT = 1,
    OPERATOR_MULT = 2,
    OPERATOR_DIV = 3,
    OPERATOR_EQUAL = 4,
    OPERATOR_CLEAR = 5,
    OPERATOR_CLEAR_ALL = 6
};

#define CALC_ID_BUTTON_0 "0"
#define CALC_ID_BUTTON_1 "1"
#define CALC_ID_BUTTON_2 "2"
#define CALC_ID_BUTTON_3 "3"
#define CALC_ID_BUTTON_4 "4"
#define CALC_ID_BUTTON_5 "5"
#define CALC_ID_BUTTON_6 "6"
#define CALC_ID_BUTTON_7 "7"
#define CALC_ID_BUTTON_8 "8"
#define CALC_ID_BUTTON_9 "9"
#define CALC_ID_BUTTON_PLUS "plus"
#define CALC_ID_BUTTON_MINUS "minus"
#define CALC_ID_BUTTON_EQUAL "equal"
#define CALC_ID_BUTTON_CLEAR "clear"
#define CALC_TEXT_BOX_NAME "calcscreen"

class calculator_system
{

    Window *target;
    double main_number = 0;
    double second_number = 0;
    int selectionned_number = 0;
    int current_number_count = 0;
    double current_operator = 0;
    bool do_reset_after = true;
    int calculator_text_length = 0;
    char calculator_text[32];

public:
    void init_button()
    {

        CALCULATOR_BUTTON(CALC_ID_BUTTON_0, true, 0)
        CALCULATOR_BUTTON(CALC_ID_BUTTON_1, true, 1)
        CALCULATOR_BUTTON(CALC_ID_BUTTON_2, true, 2)
        CALCULATOR_BUTTON(CALC_ID_BUTTON_3, true, 3)
        CALCULATOR_BUTTON(CALC_ID_BUTTON_4, true, 4)
        CALCULATOR_BUTTON(CALC_ID_BUTTON_5, true, 5)
        CALCULATOR_BUTTON(CALC_ID_BUTTON_6, true, 6)
        CALCULATOR_BUTTON(CALC_ID_BUTTON_7, true, 7)
        CALCULATOR_BUTTON(CALC_ID_BUTTON_8, true, 8)
        CALCULATOR_BUTTON(CALC_ID_BUTTON_9, true, 9)
        CALCULATOR_BUTTON(CALC_ID_BUTTON_PLUS, false, OPERATOR_ADD);
        CALCULATOR_BUTTON(CALC_ID_BUTTON_MINUS, false, OPERATOR_SUBSTRACT);
        CALCULATOR_BUTTON(CALC_ID_BUTTON_EQUAL, false, OPERATOR_EQUAL);
        CALCULATOR_BUTTON(CALC_ID_BUTTON_CLEAR, false, OPERATOR_CLEAR);
    }

    calculator_system()
    {
    }
    calculator_system(Window *target_window)
    {
        target = target_window;
        calculator_text_length = 32;
        for (int i = 0; i < calculator_text_length; i++)
        {
            calculator_text[i] = ' ';
        }
        calculator_text[calculator_text_length - 1] = 0;

        window->with_widget<Label>(CALC_TEXT_BOX_NAME, [&](auto label) {
            label->text("0");
        });
        init_button();
    };

    void update_calculator_text()
    {
        if (selectionned_number == 1)
        {
            format_double(FORMAT_DECIMAL, main_number, (char *)calculator_text, 32);
        }
        else
        {
            format_double(FORMAT_DECIMAL, second_number, (char *)calculator_text, 32);
        }
        window->with_widget<Label>(CALC_TEXT_BOX_NAME, [&](auto label) {
            label->text((calculator_text));
        });
    }

    void enter_switch_operator()
    {
        if (selectionned_number == 1)
        {
            current_number_count = 0;
            selectionned_number = 2;
        }
        else
        {
            selectionned_number = 2;
        }
    }

    bool handle_operator(double operator_code)
    {
        if (operator_code == OPERATOR_EQUAL)
        {
            do_reset_after = true;

            if (current_operator == OPERATOR_ADD)
            {
                main_number = main_number + second_number;
            }
            else if (current_operator == OPERATOR_SUBSTRACT)
            {
                main_number = main_number - second_number;
            }
            selectionned_number = 1;

            return false;
        }
        else if (operator_code == OPERATOR_CLEAR)
        {
            if (selectionned_number == 1)
            {
                main_number = 0;
            }
            else
            {
                second_number = 0;
            }
            return false;
        }
        else if (operator_code == OPERATOR_CLEAR_ALL)
        {
            main_number = 0;

            second_number = 0;
            selectionned_number = 1;
            return false;
        }

        return true;
    }
    void calculator_update(bool is_number, double code)
    {
        if (is_number)
        {
            if (do_reset_after)
            {
                main_number = 0;
                second_number = 0;
                do_reset_after = false;
                selectionned_number = 1;
            }
            if (selectionned_number == 1)
            {
                current_number_count++;
                if (current_number_count < 10)
                {
                    main_number = main_number * 10 + code;
                }
            }
            else
            {
                current_number_count++;
                if (current_number_count < 10)
                {
                    second_number = second_number * 10 + code;
                }
            }
        }
        else
        {

            if (handle_operator(code))
            {

                enter_switch_operator();
                current_operator = code;
            }
        }
        update_calculator_text();
    }
};
calculator_system main_calculator_system;

int main(int argc, char **argv)
{
    application_initialize(argc, argv);
    window = window_create_from_file("/Applications/calculator/calculator.markup");
    main_calculator_system = calculator_system(window);
    window->show();

    return application_run();
}
