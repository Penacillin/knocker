#include <unistd.h>
#include <getopt.h>

#include <array>
#include <string>
#include <iostream>
#include <optional>
#include <filesystem>

#include <libgourou.h>
#include "drmprocessorclientimpl.h"

template<class T, size_t N>
constexpr size_t size(T (&)[N]) { return N; }

static const char *deviceFile = "device.xml";
static const char *activationFile = "activation.xml";
static const char *devicekeyFile = "devicesalt";
static const char *acsmFile = 0;
static bool exportPrivateKey = false;
static const char *outputFile = 0;
static const char *outputDir = 0;
static const char *defaultDirs[] = {
    ".adept/",
    "./adobe-digital-editions/",
    "./.adobe-digital-editions/"};

namespace fs = std::filesystem;

class ACSMDownloader
{
public:
    void run()
    {
        int ret = 0;
        try
        {
            DRMProcessorClientImpl client;
            gourou::DRMProcessor processor(&client, deviceFile, activationFile, devicekeyFile);
            gourou::User *user = processor.getUser();

            if (exportPrivateKey)
            {
                std::string filename;
                if (!outputFile)
                    filename = std::string("Adobe_PrivateLicenseKey--") + user->getUsername() + ".der";

                if (outputDir)
                {
                    auto dir = fs::directory_entry(outputDir);
                    if (!dir.exists())
                        fs::create_directories(dir);

                    filename = fs::path(outputDir) / fs::path(filename);
                }

                processor.exportPrivateLicenseKey(filename);

                std::cout << "Private license key exported to " << filename << std::endl;
            }
            else
            {
                gourou::FulfillmentItem *item = processor.fulfill(acsmFile);

                std::string filename;
                if (!outputFile)
                {
                    filename = item->getMetadata("title");
                    if (filename == "")
                        filename = "output";
                }
                else
                    filename = outputFile;

                if (outputDir)
                {
                    auto dir = fs::directory_entry(outputDir);
                    if (!dir.exists())
                        fs::create_directories(dir);

                    filename = std::string(outputDir) + "/" + filename;
                }

                gourou::DRMProcessor::ITEM_TYPE type = processor.download(item, filename);

                if (!outputFile)
                {
                    std::string finalName = filename;
                    if (type == gourou::DRMProcessor::ITEM_TYPE::PDF)
                        finalName += ".pdf";
                    else
                        finalName += ".epub";
                    fs::rename(filename, finalName);
                    filename = finalName;
                }
                std::cout << "Created " << filename << std::endl;
            }
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
            ret = 1;
        }
    }

};


static std::optional<std::string> findFile(std::optional<std::string> filename, bool inDefaultDirs = true)
{
    if (filename.has_value() && fs::exists(filename.value()))
        return filename.value();

    if (!inDefaultDirs)
        return std::nullopt;

    if (!filename.has_value()) return std::nullopt;

    for (int i = 0; i < size(defaultDirs); i++)
    {
        const auto path = std::string(defaultDirs[i]) + std::string(filename.value());
        if (std::filesystem::exists(path))
            return path;
    }

    return std::nullopt;
}

static void version(void)
{
    std::cout << "Current libgourou version : " << gourou::DRMProcessor::VERSION << std::endl;
}

static void usage(const char *cmd)
{
    std::cout << "Download EPUB file from ACSM request file" << std::endl;

    std::cout << "Usage: " << cmd << " [(-d|--device-file) device.xml] [(-a|--activation-file) activation.xml] [(-k|--device-key-file) devicesalt] [(-O|--output-dir) dir] [(-o|--output-file) output(.epub|.pdf|.der)] [(-v|--verbose)] [(-h|--help)] (-f|--acsm-file) file.acsm|(-e|--export-private-key)" << std::endl
              << std::endl;

    std::cout << "  "
              << "-d|--device-file"
              << "\t"
              << "device.xml file from eReader" << std::endl;
    std::cout << "  "
              << "-a|--activation-file"
              << "\t"
              << "activation.xml file from eReader" << std::endl;
    std::cout << "  "
              << "-k|--device-key-file"
              << "\t"
              << "private device key file (eg devicesalt/devkey.bin) from eReader" << std::endl;
    std::cout << "  "
              << "-O|--output-dir"
              << "\t"
              << "Optional output directory were to put result (default ./)" << std::endl;
    std::cout << "  "
              << "-o|--output-file"
              << "\t"
              << "Optional output filename (default <title.(epub|pdf|der)>)" << std::endl;
    std::cout << "  "
              << "-f|--acsm-file"
              << "\t"
              << "ACSM request file for epub download" << std::endl;
    std::cout << "  "
              << "-e|--export-private-key"
              << "\t"
              << "Export private key in DER format" << std::endl;
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
    std::cout << "Device file, activation file and device key file are optionals. If not set, they are looked into :" << std::endl;
    std::cout << "  * Current directory" << std::endl;
    std::cout << "  * .adept" << std::endl;
    std::cout << "  * adobe-digital-editions directory" << std::endl;
    std::cout << "  * .adobe-digital-editions directory" << std::endl;
}

int main(int argc, char **argv)
{
    int c, ret = -1;

    // const char **files[] = {&devicekeyFile, &deviceFile, &activationFile};
    std::array<std::optional<std::string>, 3> files{devicekeyFile, deviceFile, activationFile};
    int verbose = gourou::DRMProcessor::getLogLevel();

    while (1)
    {
        int option_index = 0;
        static struct option long_options[] = {
            {"device-file", required_argument, 0, 'd'},
            {"activation-file", required_argument, 0, 'a'},
            {"device-key-file", required_argument, 0, 'k'},
            {"output-dir", required_argument, 0, 'O'},
            {"output-file", required_argument, 0, 'o'},
            {"acsm-file", required_argument, 0, 'f'},
            {"export-private-key", no_argument, 0, 'e'},
            {"verbose", no_argument, 0, 'v'},
            {"version", no_argument, 0, 'V'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}};

        c = getopt_long(argc, argv, "d:a:k:O:o:f:evVh",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case 'd':
            // deviceFile = optarg;
            files[1] = optarg;
            break;
        case 'a':
            // activationFile = optarg;
            files[2] = optarg;
            break;
        case 'k':
            // devicekeyFile = optarg;
            files[0] = optarg;
            break;
        case 'f':
            acsmFile = optarg;
            break;
        case 'O':
            outputDir = optarg;
            break;
        case 'o':
            outputFile = optarg;
            break;
        case 'e':
            exportPrivateKey = true;
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
        default:
            usage(argv[0]);
            return -1;
        }
    }

    gourou::DRMProcessor::setLogLevel(verbose);

    if ((!acsmFile && !exportPrivateKey) || (outputDir && !outputDir[0]) ||
        (outputFile && !outputFile[0]))
    {
        usage(argv[0]);
        return -1;
    }

    ACSMDownloader downloader;

    int i;
    bool hasErrors = false;
    std::string orig;
    for (i = 0; i < files.size(); i++)
    {
        orig = files[i].value();
        files[i] = findFile(files[i]);
        if (!files[i].has_value())
        {
            std::cout << "Error : " << orig << " doesn't exists, did you activate your device ?" << std::endl;
            ret = -1;
            hasErrors = true;
        }
    }

    if (hasErrors)
        return ret;

    if (exportPrivateKey)
    {
        if (acsmFile)
        {
            usage(argv[0]);
            return -1;
        }
    }
    else
    {
        if (!fs::exists(acsmFile))
        {
            std::cout << "Error : " << acsmFile << " doesn't exists" << std::endl;
            return -1;
        }
    }

    downloader.run();

    return ret;
}
