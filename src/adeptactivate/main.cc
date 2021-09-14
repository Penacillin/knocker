#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <termios.h>

#include <iostream>
#include <ostream>
#include <filesystem>
#include <string.h>

#include <libgourou.h>
#include "drmprocessorclientimpl.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

template <class T, size_t N>
constexpr size_t size(T (&)[N]) { return N; }

static const char *username = 0;
static const char *password = 0;
static const char *outputDir = 0;
static const char *hobbesVersion = HOBBES_DEFAULT_VERSION;
static bool randomSerial = false;

namespace fs = std::filesystem;

// From http://www.cplusplus.com/articles/E6vU7k9E/
static int getch()
{
    int ch;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
    return ch;
}

static std::string getpass(const char *prompt, bool show_asterisk = false)
{
    const char BACKSPACE = 127;
    const char RETURN = 10;

    std::string password;
    unsigned char ch = 0;

    std::cout << prompt;

    while ((ch = getch()) != RETURN)
    {
        if (ch == BACKSPACE)
        {
            if (password.length() != 0)
            {
                if (show_asterisk)
                    std::cout << "\b \b";
                password.resize(password.length() - 1);
            }
        }
        else
        {
            password += ch;
            if (show_asterisk)
                std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}

class ADEPTActivate
{
public:
    void run()
    {
        int ret = 0;
        try
        {
            DRMProcessorClientImpl client;
            gourou::DRMProcessor *processor = gourou::DRMProcessor::createDRMProcessor(
                &client, randomSerial, outputDir, hobbesVersion);

            processor->signIn(username, password);
            processor->activateDevice();

            std::cout << username << " fully signed and device activated in " << outputDir << std::endl;
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
            ret = 1;
        }
    }
};

static void version(void)
{
    std::cout << "Current libgourou version : " << gourou::DRMProcessor::VERSION << std::endl;
}

static void usage(const char *cmd)
{
    std::cout << "Create new device files used by ADEPT DRM" << std::endl;

    std::cout << "Usage: " << cmd << " (-u|--username) username [(-p|--password) password] [(-O|--output-dir) dir] [(-r|--random-serial)] [(-v|--verbose)] [(-h|--help)]" << std::endl
              << std::endl;

    std::cout << "  "
              << "-u|--username"
              << "\t\t"
              << "AdobeID username (ie adobe.com email account)" << std::endl;
    std::cout << "  "
              << "-p|--password"
              << "\t\t"
              << "AdobeID password (asked if not set via command line) " << std::endl;
    std::cout << "  "
              << "-O|--output-dir"
              << "\t"
              << "Optional output directory were to put result (default ./.adept). This directory must not already exists" << std::endl;
    std::cout << "  "
              << "-H|--hobbes-version"
              << "\t"
              << "Force RMSDK version to a specific value (default: version of current librmsdk)" << std::endl;
    std::cout << "  "
              << "-r|--random-serial"
              << "\t"
              << "Generate a random device serial (if not set, it will be dependent of your current configuration)" << std::endl;
    std::cout << "  "
              << "-v|--verbose"
              << "\t\t"
              << "Increase verbosity, can be set multiple times" << std::endl;
    std::cout << "  "
              << "-V|--version"
              << "\t\t"
              << "Display libgourou version" << std::endl;
    std::cout << "  "
              << "-h|--help"
              << "\t\t"
              << "This help" << std::endl;

    std::cout << std::endl;
}

static std::string abspath(std::string filename)
{
    std::string root = std::string(getcwd(0, 0));
    std::string fullPath = fs::path(root) / fs::path(filename);
    return fullPath;
}

int main(int argc, char **argv)
{
    int c, ret = -1;
    const char* _outputDir = outputDir;
    int verbose = gourou::DRMProcessor::getLogLevel();

    while (1)
    {
        int option_index = 0;
        static struct option long_options[] = {
            {"username", required_argument, 0, 'u'},
            {"password", required_argument, 0, 'p'},
            {"output-dir", required_argument, 0, 'O'},
            {"hobbes-version", required_argument, 0, 'H'},
            {"random-serial", no_argument, 0, 'r'},
            {"verbose", no_argument, 0, 'v'},
            {"version", no_argument, 0, 'V'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}};

        c = getopt_long(argc, argv, "u:p:O:H:rvVh",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 'u':
            username = optarg;
            break;
        case 'p':
            password = optarg;
            break;
        case 'O':
            _outputDir = optarg;
            break;
        case 'H':
            hobbesVersion = optarg;
            break;
        case 'v':
            verbose++;
            break;
        case 'V':
            version();
            return 0;
        case 'h':
            usage(argv[0]);
            return 0;
        case 'r':
            randomSerial = true;
            break;
        default:
            usage(argv[0]);
            return -1;
        }
    }

    gourou::DRMProcessor::setLogLevel(verbose);

    if (!username)
    {
        usage(argv[0]);
        return -1;
    }

    if (!_outputDir || _outputDir[0] == 0)
    {
        outputDir = abspath(DEFAULT_ADEPT_DIR).c_str();
    }
    else
    {
        // Relative path
        if (_outputDir[0] == '.' || _outputDir[0] != '/')
        {
            // realpath doesn't works if file/dir doesn't exists
            if (fs::exists(_outputDir))
                outputDir = realpath(_outputDir, 0);
            else
                outputDir = abspath(_outputDir).c_str();
        }
        else
            outputDir = strdup(_outputDir);
    }

    if (!password)
    {
        char prompt[128];
        std::snprintf(prompt, sizeof(prompt), "Enter password for <%s> : ", username);
        std::string pass = getpass((const char *)prompt, false);
        password = pass.c_str();
    }

    ADEPTActivate activate;

    activate.run();

    free((void *)outputDir);
    return ret;
}
