#include "Permissions.h"

DiscordCPP::Permissions DiscordCPP::Permissions::All() {
    Permissions permissions;
    permissions.permissions = UINT64_MAX;
    return permissions;
}

DiscordCPP::Permissions::Permissions(const std::string& permissions) {
    std::istringstream iss(permissions);
    iss.exceptions(std::ios::failbit | std::ios::badbit);
    iss >> this->permissions;
}

DiscordCPP::Permissions::operator std::string() {
    std::ostringstream oss;
    oss.exceptions(std::ios::failbit | std::ios::badbit);
    oss << permissions;
    return oss.str();
}

json DiscordCPP::Permissions::to_json() {
    if (permissions == 0) {
        return nullptr;
    }
    return std::string(*this);
}

void DiscordCPP::Permissions::add(const Permission& permission) {
    permissions |= (1 << permission);
}

void DiscordCPP::Permissions::add(const Permissions& permissions) {
    this->permissions |= permissions.permissions;
}

void DiscordCPP::Permissions::remove(const Permission& permission) {
    permissions &= ~(1 << permission);
}

void DiscordCPP::Permissions::remove(const Permissions& permissions) {
    this->permissions &= ~permissions.permissions;
}

bool DiscordCPP::Permissions::has_permission(Permission permission) const {
    return permissions & (1 << permission);
}

bool DiscordCPP::Permissions::has_all_permissions(std::vector<Permission> permissions) const {
    for (auto permission : permissions) {
        if (!has_permission(permission)) {
            return false;
        }
    }
    return true;
}

bool DiscordCPP::Permissions::has_any_permission(std::vector<Permission> permissions) const {
    for (auto permission : permissions) {
        if (has_permission(permission)) {
            return true;
        }
    }
    return false;
}

DiscordCPP::PermissionOverwrites::PermissionOverwrites(const json& data) {
    id = data.at("id").get<std::string>();
    type = static_cast<Type>(data["type"].get<int>());
    if (get_or_else<std::string>(data, "allow", "").length() > 0) {
        allow = Permissions(data.at("allow").get<std::string>());
    }
    if (get_or_else<std::string>(data, "deny", "").length() > 0) {
        deny = Permissions(data.at("deny").get<std::string>());
    }
}
