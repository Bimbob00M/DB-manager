#ifndef GLOBAL_H
#define GLOBAL_H

namespace PatientsDBManager::Global
{
    constexpr auto DATE_TIME_FORMAT = "dd.MM.yyyy hh:mm";
    constexpr auto DATE_FORMAT = "dd.MM.yyyy";
    constexpr auto NOT_EMPTY_REGEX_PATTERN = "^(?!\\s*$).+";
    constexpr auto EMPTY_CELL_DEFAULT_VALUE = "Not set";
}

#endif // GLOBAL_H
