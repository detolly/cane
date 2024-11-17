#include <Utility/config.h>
#include <Utility/FileReader.h>
#include <Utility/FileWriter.h>
#include <Utility/dbgprint.h>

config config::c;

void config::serialize(const char *filename) {
    FileWriter writer(filename);
    writer.write(&config::the(), sizeof(config));
}

void config::deserialize(const char *filename) {
    FileReader reader(filename);
    const auto buffer = reader.read();
    if (reader.was_opened()) {
        const auto len = reader.length();

        if (len != sizeof(config)){
            dbgprint("Config file is newer/older or corrupted. Ignoring.");
            return;
        }

        config::the() = *(config*)buffer.data();
    } else {
        // File does not exist, that's fine!
    };

}
