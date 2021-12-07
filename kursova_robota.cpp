/* BC-01 Andrii Aleksandrovych
 *
 * Kursova robota (course work)
 *
 * Program calculates 6 * x / (2 - 3 * x) via series.
 *
 * You can input calculation parametrs:
 * - via console 
 * - via text file
 *
 * After doing the calculations you can save them to the text file.
 */

#include <iostream>
#include <cmath>


namespace console
{
    void pause()
    {
        system("PAUSE");
    }

    void clear()
    {
        system("CLS");
    }

    void clear_stream()
    {
        std::cin.clear();
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');
    }

    char read_char()
    {
        char input_text;
        scanf_s("%c", &input_text, 2);
        return input_text;
    }

    const char* table_line()
    {
        return "-------------------------------------------------"
           "-----------------------------------------------------"
           "-----------------------------------------------------";
    }
}


namespace settings
{
    /*-----------------------------------------------------------------
     * struct contains everything needed to calculate
     */
    struct calculation_settings
    {
        bool   is_empty = true;
        double x_start  = 0;
        double x_end    = 0;
        double step     = 0;
        double accuracy = 0;
    };

    /*-----------------------------------------------------------------
     * function implements the cyclic input
     * of the value with the specified bounds
     */
    double input_with_restricts(const double min, const double max,
        const char* title, const char* less, const char* more)
    {
        while (true)
        {
            double input_value = 0;
            printf("%s", title);
            scanf_s("%lf", &input_value);

            const bool x_is_normal =
                input_value > min && input_value < max;

            if (x_is_normal)
            {
                return input_value;
            }
            if (input_value <= min)
            {
                printf("%s", less);
            }
            else
            {
                printf("%s", more);
            }
        } // end of "while (true)"
    }

    /*-----------------------------------------------------------------
     * the function returns an object of type calculation_settings
     * with fields cyclically entered from the keyboard
     */
    calculation_settings input_calculation_parameters()
    {
        calculation_settings settings;
        settings.is_empty = false;

        //input x_start
        settings.x_start = input_with_restricts(
            -2 / 3.0, 2 / 3.0,
            "Enter start value -2/3 < x < 2/3:\n>>>",
            "Value should be more than -2/3\n",
            "Value should be less than 2/3\n");

        //input x_end
        settings.x_end = input_with_restricts(
            settings.x_start, 2 / 3.0,
            "\nEnter end value start < x < 2/3:\n>>>",
            "Value should be more than start value\n",
            "Value should be less than 2/3\n");

        //input step
        settings.step = input_with_restricts(
            0, settings.x_end - settings.x_start,
            "\nEnter step value:\n>>>",
            "Step should be more than 0\n",
            "Step is too big\n");

        //input accuracy
        settings.accuracy = input_with_restricts(
            0, 1,
            "\nEnter 0 < accuracy < 1:\n>>>",
            "Accuracy should be more than 0\n",
            "Accuracy should be less than 1\n");


        printf("\nCalculation parameters set successfully.\n");
        console::pause();
        return settings;
    }

    /*-----------------------------------------------------------------
     * the function checks if the calculation_settings
     * object's fields match the task's conditions
     */
    bool validate_settings(const calculation_settings settings)
    {
        if (
            -2 / 3.0 < settings.x_start &&
            settings.x_start < 2 / 3.0 &&
            settings.x_start < settings.x_end &&
            settings.x_end < 2 / 3.0 &&
            0 < settings.step &&
            settings.step < settings.x_end - settings.x_start &&
            0 < settings.accuracy &&
            settings.accuracy < 1
            )
        {
            return true;
        }
        return false;
    }

    /*-----------------------------------------------------------------
     * function returns an object of type calculation_settings
     * with fields read from the file specified by the user
     */
    calculation_settings read_calculation_parameters()
    {
        console::clear_stream();

        calculation_settings settings;

        printf("Reading calculation parameters from text file\n"
            "\nx start: -2/3 < x < 2/3\nx end: x start < x end < 2/3\n"
            "step:  0 < step < end - start\naccuracy: 0 < accuracy < 1\n"
            "\nText format should be like this: \n"
            "\"x_start;x_end;step;accuracy;\"\n"
            "\nEnter full text file path:\n");

        char* file_name = new char[200];
        scanf_s("%[^\n]", file_name, 200);

        FILE* file = nullptr;
        fopen_s(&file, file_name, "r");
        delete[] file_name;
        if (file != nullptr) //file was opened successfully
        {
            fscanf_s(file, "%lf;%lf;%lf;%lf;",
                &settings.x_start,
                &settings.x_end,
                &settings.step,
                &settings.accuracy);
            fclose(file);

            if (validate_settings(settings)) //file was read successfully
            {
                settings.is_empty = false;
                printf(
                    "\nCalculation parameters set successfully.\n\n"
                    "Start point = %lf;\n"
                    "End point = %lf;\n"
                    "Step = %lf;\n"
                    "Accuracy = %lf;\n\n",
                    settings.x_start,
                    settings.x_end,
                    settings.step,
                    settings.accuracy);

            } // end of if (validate_settings(settings))
            else //file text has wrong format
            {
                printf("Wrong text format.\n");
            }
        } // end of if (file != nullptr)
        else //file was not opened
        {
            printf("Could read file\n");
        }

        console::pause();
        return settings;
    }
}


namespace calculation
{
    /*-----------------------------------------------------------------
     * struct contains an single calculation
     */
    struct calculation_point
    {
        unsigned int iterations_amount = 0;
        double       x                 = 0;
        double       y                 = 0;
        double       y_series          = 0;
        double       error             = 0;
    };

    /*-----------------------------------------------------------------
     * struct contains calculation_settings object
     * and calculation results
     */
    struct calculation_results
    {
        settings::calculation_settings settings;

        bool               is_empty            = true;
        unsigned int       values_amount       = 1;
        unsigned int       symbols_after_comma = 0;
        calculation_point* points              = nullptr; //results array
    };

    /*-----------------------------------------------------------------
     * the exact calculating function
     * calculates 6 * x / (2 - 3 * x)
     */
    double calculate_function(const double x)
    {
        return 6 * x / (2 - 3 * x);
    }

    /*-----------------------------------------------------------------
     * resursive calculating function
     * resursively calculates series
     * used by the main calculation function
     * n - number of series
     */
    double calculate_function_series(const double x,
        const double accuracy, unsigned int* n, double a_n)
    {
        if (abs(a_n) <= accuracy) 
        {
            return 0; //base case
        }
        
        const double q = 3 * x / 2;
        a_n = a_n * q;
        (*n)++;

        return a_n + calculate_function_series(
            x, accuracy, n, a_n);
    }

    /*-----------------------------------------------------------------
     * the main calculating function
     * calculates 6 * x / (2 - 3 * x) via series
     * usinng recursive overload
     * returns the sum of series
     * n - number of series
     */
    double calculate_function_series(const double x,
        const double accuracy, unsigned int* n)
    {
        double a_n = 3 * x; //default start value
        (*n) = 1;        
        return a_n + calculate_function_series(x, accuracy, n, a_n);
    }

    /*-----------------------------------------------------------------
     * function fills all the result calculation points
     */
    void calculate_for_array(const calculation_results results)
    {
        for (unsigned int i = 0; i < results.values_amount; i++)
        {
            //x from start to end with step
            results.points[i].x = i == 0
                ? results.settings.x_start
                : results.points[i - 1].x + results.settings.step;

            //y is f(x)
            results.points[i].y = calculate_function(results.points[i].x);

            //y series is f(x) via series function
            results.points[i].y_series = calculate_function_series(
                results.points[i].x,
                results.settings.accuracy,
                &results.points[i].iterations_amount);

            //error is series minus original true value
            results.points[i].error =
                abs(results.points[i].y_series - results.points[i].y);

        } // end of for (unsigned int i = 0; i < results.values_amount; i++)
    }

    /*-----------------------------------------------------------------
     * function prints calculation settings
     * and results via table
     */
    void print_result(const calculation_results results)
    {
        printf(
            "f(x) = 6x/(2-3x);\n"
            "Start point = %lf;\n"
            "End point = %lf;\n"
            "Step = %lf;\n"
            "Accuracy = %.*lf;\n\n",
            results.settings.x_start,
            results.settings.x_end,
            results.settings.step,
            results.symbols_after_comma,
            results.settings.accuracy);

        printf("%s\n| %-34s| %-34s| %-34s| %-34s| Members |\n%s\n",
            console::table_line(),
            "x", "series(x)",
            "f(x)", "Error",
            console::table_line());

        for (unsigned int i = 0; i < results.values_amount; ++i)
        {
            printf("| %-34.*lf| %-34.*lf| %-34.*lf| %-34.31lf| %-8d|\n",
                results.symbols_after_comma,
                results.points[i].x,
                results.symbols_after_comma,
                results.points[i].y_series,
                results.symbols_after_comma,
                results.points[i].y,
                results.points[i].error,
                results.points[i].iterations_amount);

        } // end of for (unsigned int i = 0; i < results.values_amount; ++i)

        printf("%s\n", console::table_line());
    }

    /*-----------------------------------------------------------------
     * function obtains calculatin settings
     * creates calculation_results object
     * calculates and saves results to calculation_results.points array
     * prints results as table
     * and returns the calculation_results obj
     */
    calculation_results calculate(settings::calculation_settings settings)
    {
        calculation_results results;
        if (settings.is_empty)
        {
            printf("Nothing to calculate!\n"
                "Specify settings first.\n");
            console::pause();
            return results;
        }

        //results contains calculation settings
        results.settings = settings;


        //caclulatings how many symbols after comma we need
        //depending on the accuracy
        //for instanse 0.0001 means 4 symbols after comma
        for (; results.settings.accuracy < pow(0.11,
            results.symbols_after_comma + 1);)
        {
            results.symbols_after_comma++;
        }

        //caclulatings how many values (points) we need
        //to calculate depending on the step, start and end
        results.values_amount = static_cast<int>(
            (results.settings.x_end - results.settings.x_start)
            / results.settings.step + 1.1);

        //intializing the dynamic array before the main calculatings
        results.points = new calculation_point[results.values_amount];
        results.is_empty = false;

        //the main calculations
        calculate_for_array(results);

        print_result(results);

        console::pause();
        return results;
    }
}


namespace saving
{
    /*-----------------------------------------------------------------
     * function prints calculation settings
     * and results via table to the given opened file
     */
    void print_result_to_file(FILE* file,
        const calculation::calculation_results results)
    {
        fprintf(file,
            "f(x) = 6x/(2-3x);\n"
            "Start point = %lf;\n"
            "End point = %lf;\n"
            "Step = %lf;\n"
            "Accuracy = %.*lf;\n\n",
            results.settings.x_start,
            results.settings.x_end,
            results.settings.step,
            results.symbols_after_comma,
            results.settings.accuracy);

        fprintf(file, "%s\n| %-34s| %-34s| %-34s| %-34s| Members |\n%s\n",
            console::table_line(),
            "x", "series(x)",
            "f(x)", "Error",
            console::table_line());

        for (unsigned int i = 0; i < results.values_amount; ++i)
        {
            fprintf(file,
                "| %-34.*lf| %-34.*lf| %-34.*lf| %-34.31lf| %-8d|\n",
                results.symbols_after_comma,
                results.points[i].x,
                results.symbols_after_comma,
                results.points[i].y_series,
                results.symbols_after_comma,
                results.points[i].y,
                results.points[i].error,
                results.points[i].iterations_amount);

        } // end of for (unsigned int i = 0; i < results.values_amount; ++i)

        fprintf(file, "%s\n", console::table_line());
    }

    /*-----------------------------------------------------------------
     * function opens file with with the given name
     * writes to file results
     * closes file
     * returns true if success
     */
    bool save_to_file(const char* file_name,
        const calculation::calculation_results results)
    {
        FILE* file = nullptr;
        fopen_s(&file, file_name, "w+");
        if (file != nullptr)
        {
            print_result_to_file(file, results);
            fclose(file);
            return true;
        }
        return false;
    }

    /*-----------------------------------------------------------------
     * function saves the given results to text file
     */
    void save_calculations(calculation::calculation_results results)
    {
        if (results.is_empty || results.settings.is_empty)
        {
            printf("Nothing to save!\n"
                "Make calculation first.\n");
            console::pause();
            return;
        }

        printf("Saving...\n");
        const char* file_name = "data.txt";
        if (save_to_file(file_name, results))
        {
            printf("Saved successfully to %s\n", file_name);
        }
        else
        {
            printf("Could not save file\n");
        }

        console::pause();
    }
}


int main()
{
    settings::calculation_settings    settings;
    calculation::calculation_results  results;
    bool exit_trigger = false;

    do
    {
        printf("Enter \"0\" to exit\n"
            "Enter \"1\" to input calculation parameters\n"
            "Enter \"2\" to read calculation parameters from file\n"
            "Enter \"3\" to calculate\n"
            "Enter \"4\" to save calculations\n>>>");

        const char input_key = console::read_char();
        console::clear();
        switch (input_key)
        {
        case '0':
            exit_trigger = true;
            break;
        case '1':
            settings = settings::input_calculation_parameters();
            console::clear();
            break;
        case '2':
            settings = settings::read_calculation_parameters();
            console::clear();
            break;
        case '3':
            delete[] results.points; //deleting points if exists
            results = calculation::calculate(settings);
            console::clear();
            break;
        case '4':
            saving::save_calculations(results);
            console::clear();
            break;
        default:
            break;
        }

    } while (!exit_trigger);

    printf("Exiting...\n");
    delete[] results.points; //deleting points if exists

    return 0;
}
