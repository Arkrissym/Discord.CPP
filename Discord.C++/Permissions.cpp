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

void DiscordCPP::Permissions::add(const Permission& permission) {
    permissions |= (1 << permission);
}

void DiscordCPP::Permissions::add(const Permissions& permissions) {
    this->permissions |= permissions.permissions;
}

bool DiscordCPP::Permissions::has_permission(Permission permission) {
    return permissions & (1 << permission);
}

bool DiscordCPP::Permissions::has_all_permissions(std::vector<Permission> permissions) {
    for (auto permission : permissions) {
        if (!has_permission(permission)) {
            return false;
        }
    }
    return true;
}

bool DiscordCPP::Permissions::has_any_permission(std::vector<Permission> permissions) {
    for (auto permission : permissions) {
        if (has_permission(permission)) {
            return true;
        }
    }
    return false;
}
