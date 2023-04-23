#include <utility/settings/SettingsIORegistry.h>
#include <utility/Exceptions.h>

std::unordered_map<std::string, std::shared_ptr<settings::io::detail::ISettingRef>> settings::io::detail::settings_storage;
std::vector<settings::io::SettingSection> settings::io::detail::settings_sections;

settings::io::SettingSection::SettingSection(std::string_view name, std::initializer_list<std::shared_ptr<detail::ISettingRef>> settings) : name(name), settings(settings) {
    for (auto& setting : settings) {
        for (auto& name : setting->names) {
            if (settings::io::detail::settings_storage.count(name) != 0) {
                throw std::runtime_error("Settings::add: Duplicate setting name: \"" + name + "\".");
            }
            settings::io::detail::settings_storage[name] = std::move(setting);
        }
    }
}

template<> std::string settings::io::detail::SettingRef<std::string>::get() const {return settingref;}
template<> std::string settings::io::detail::SettingRef<double>::get() const {return std::to_string(settingref);}
template<> std::string settings::io::detail::SettingRef<int>::get() const {return std::to_string(settingref);}
template<> std::string settings::io::detail::SettingRef<unsigned int>::get() const {return std::to_string(settingref);}
template<> std::string settings::io::detail::SettingRef<bool>::get() const {return std::to_string(settingref);}
template<> std::string settings::io::detail::SettingRef<std::vector<std::string>>::get() const {
    std::string str;
    std::for_each(settingref.begin(), settingref.end(), [&str] (const std::string& s) {str += s + " ";});
    return str;
}
template<> std::string settings::io::detail::SettingRef<std::vector<double>>::get() const {
    std::string str;
    std::for_each(settingref.begin(), settingref.end(), [&str] (double s) {str += std::to_string(s) + " ";});
    return str;
}
template<> std::string settings::io::detail::SettingRef<std::vector<int>>::get() const {
    std::string str;
    std::for_each(settingref.begin(), settingref.end(), [&str] (int s) {str += std::to_string(s) + " ";});
    return str;
}


template<> void settings::io::detail::SettingRef<std::string>::set(const std::vector<std::string>& str) {
    if (str.size() != 1) {throw except::parse_error("Settings::SmartOption::parse: Option \"" + get() + "\" received too many settings.");}
    settingref = str[0];
}
template<> void settings::io::detail::SettingRef<bool>::set(const std::vector<std::string>& str) {
    if (str.size() != 1) {throw except::parse_error("Settings::SmartOption::parse: Option \"" + get() + "\" received too many settings.");}

    if (str[0] == "true" || str[0] == "TRUE" || str[0] == "1") {settingref = true; return;}
    else if (str[0] == "false" || str[0] == "FALSE" || str[0] == "0") {settingref = false; return;}
    throw except::parse_error("Settings::parse_bool: Option \"" + get() + "\" expected boolean string, but got \"" + str[0] + "\".");
}
template<> void settings::io::detail::SettingRef<double>::set(const std::vector<std::string>& str) {
    if (str.size() != 1) {throw except::parse_error("Settings::SmartOption::parse: Option \"" + get() + "\" received too many settings.");}
    settingref = std::stod(str[0]);
}
template<> void settings::io::detail::SettingRef<int>::set(const std::vector<std::string>& str) {
    if (str.size() != 1) {throw except::parse_error("Settings::SmartOption::parse: Option \"" + get() + "\" received too many settings.");}
    settingref = std::stoi(str[0]); 
}
template<> void settings::io::detail::SettingRef<unsigned int>::set(const std::vector<std::string>& str) {
    if (str.size() != 1) {throw except::parse_error("Settings::SmartOption::parse: Option \"" + get() + "\" received too many settings.");}
    settingref = std::stoi(str[0]);
}
template<> void settings::io::detail::SettingRef<std::vector<std::string>>::set(const std::vector<std::string>& str) {
    settingref = str;
}
template<> void settings::io::detail::SettingRef<std::vector<double>>::set(const std::vector<std::string>& str) {
    std::vector<double> new_val;
    for (auto& s : str) {
        if (s.empty() || s == " ") {continue;}
        new_val.push_back(std::stod(s));
    }
    if (new_val.empty()) {throw except::parse_error("Settings::SmartOption::parse: Option \"" + get() + "\" received no settings.");}
    settingref = new_val;
}
template<> void settings::io::detail::SettingRef<std::vector<int>>::set(const std::vector<std::string>& str) {
    std::vector<int> new_val;
    for (auto& s : str) {
        if (s.empty() || s == " ") {continue;}
        new_val.push_back(std::stoi(s));
    }
    if (new_val.empty()) {throw except::parse_error("Settings::SmartOption::parse: Option \"" + get() + "\" received no settings.");}
    settingref = new_val;
}