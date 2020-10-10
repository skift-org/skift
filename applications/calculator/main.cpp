#include <libsystem/BuildInfo.h>
#include <libsystem/utils/NumberFormatter.h>
#include <libwidget/Application.h>
#include <libwidget/Markup.h>
#include <libwidget/Widgets.h>
#include <string.h>
double main_number = 0;
double second_number = 0;
int selectionned_number = 0;
int current_number_count = 0;
double current_operator = 0;
bool do_reset_after = true;

Window *window;
char calculator_text[32];

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
    window->with_widget<Label>("calcscreen", [&](auto label) {
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
#define CALCULATOR_BUTTON(stringcodename, is_number, code)         \
    window->with_widget<Button>(stringcodename, [&](auto button) { \
        button->on(Event::ACTION, [&](auto) {                      \
            calculator_update(is_number, code);                    \
        });                                                        \
    });

int main(int argc, char **argv)
{
    selectionned_number = 1;
    application_initialize(argc, argv);
    window = window_create_from_file("/Applications/calculator/calculator.markup");
    for (int i = 0; i < 32; i++)
    {
        calculator_text[i] = ' ';
    }
    calculator_text[31] = '\0';
    window->with_widget<Label>("calcscreen", [&](auto label) {
        label->text("0");
    });
    CALCULATOR_BUTTON("0", true, 0)
    CALCULATOR_BUTTON("1", true, 1)
    CALCULATOR_BUTTON("2", true, 2)
    CALCULATOR_BUTTON("3", true, 3)
    CALCULATOR_BUTTON("4", true, 4)
    CALCULATOR_BUTTON("5", true, 5)
    CALCULATOR_BUTTON("6", true, 6)
    CALCULATOR_BUTTON("7", true, 7)
    CALCULATOR_BUTTON("8", true, 8)
    CALCULATOR_BUTTON("9", true, 9)
    CALCULATOR_BUTTON("plus", false, OPERATOR_ADD);
    CALCULATOR_BUTTON("minus", false, OPERATOR_SUBSTRACT);
    CALCULATOR_BUTTON("equal", false, OPERATOR_EQUAL);
    CALCULATOR_BUTTON("clear", false, OPERATOR_CLEAR);
    window->show();

    return application_run();
}
