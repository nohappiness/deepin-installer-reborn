// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#ifndef INSTALLER_SETTINGS_MANAGER_H
#define INSTALLER_SETTINGS_MANAGER_H

#include <QString>
#include <QStringList>
#include <QVariant>

namespace installer {

// Read settings value from ini file.

// Get boolean option value from settings file.
// If |key| does not exist, returns false.
bool GetSettingsBool(const QString& key);

// Get integer option value from settings file.
// If |key| does not exist, return 0.
int GetSettingsInt(const QString& key);

// Get string option value from settings file.
// If |key| does not exist, returns an empty string.
QString GetSettingsString(const QString& key);

// Get string list option value form settings file.
// Items in value are separated by ;
// If |key| does not exist, returns an empty string list.
QStringList GetSettingsStringList(const QString& key);

// Get variant option value from settings file.
QVariant GetSettingsValue(const QString& key);

// Returns absolute path to oem/auto_part.sh
QString GetAutoPartFile();

// Get all available avatars in avatar folder.
QStringList GetAvatars();

// Get default user avatar, is specified. Or else returns a random avatar.
QString GetDefaultAvatar();

// Get absolute path to oem hooks folder.
QString GetOemHooksDir();

// Get vendor logo.
QString GetVendorLogo();

// Get image path of window background.
// First try to find in root of iso.
// Then read from /usr/share/backgrounds/.
// If not appropriate image is found, use the fallback image.
QString GetWindowBackground();

// Operations of /etc/deepin-installer.conf
bool DeleteConfigFile();
// Setup uefi mode or not.
void WriteUEFI(bool is_efi);
//void WriteInstallerMode(bool is_simple_mode);
void WriteLocale(const QString& locale);
void WriteUsername(const QString& username);
void WriteHostname(const QString& hostname);
void WritePassword(const QString& password);
void WriteAvatar(const QString& avatar);
void WriteTimezone(const QString& timezone);
void WriteKeyboard(const QString& layout, const QString& variant);
// Write disk info.
//  * |root_disk|, device path to install system into, like /dev/sda;
//  * |root_partition|, partition path to install system into;
//  * |boot_partition|, partition path to install grub into;
//  * |mount_point|, a list of partition path and mount-point info,
//    items are separated by ';'
void WritePartitionInfo(const QString& root_disk,
                        const QString& root_partition,
                        const QString& boot_partition,
                        const QString& mount_points);

// Save current oem_tool settings to /etc/deepin-installer-oem_tool.conf
// This file is accessed in hook scripts.
void SaveOemConfig();

}  // namespace installer

#endif  // INSTALLER_SETTINGS_MANAGER_H
