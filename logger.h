#include <fstream>
#include<stdlib.h>

void write_text_to_log_file( const std::string &text)
{
    std::ofstream log_file(
        getenv("LOG_FILE"), std::ios_base::out | std::ios_base::app );
    log_file << text << std::endl;;
}
