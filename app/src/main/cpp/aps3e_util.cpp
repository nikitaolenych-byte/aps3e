// SPDX-License-Identifier: GPL-2.0-only

namespace aps3e_util{

    bool install_firmware(int fd){
        fs::file pup_f=fs::file::from_fd(fd);
        if (!pup_f)
        {
            //LOGE("Error opening PUP file %s (%s)", path);
            LOGE("Firmware installation failed: The selected firmware file couldn't be opened.");
            return false;
        }

        pup_object pup(std::move(pup_f));

        switch (pup.operator pup_error())
        {
            case pup_error::header_read:
            {
                LOGE("%s", pup.get_formatted_error().data());
                LOGE("Firmware installation failed: The provided file is empty.");
                return false;
            }
            case pup_error::header_magic:
            {
                LOGE("Error while installing firmware: provided file is not a PUP file.");
                LOGE("Firmware installation failed: The provided file is not a PUP file.");
                return false;
            }
            case pup_error::expected_size:
            {
                LOGE("%s", pup.get_formatted_error().data());
                LOGE("Firmware installation failed: The provided file is incomplete. Try redownloading it.");
                return false;
            }
            case pup_error::header_file_count:
            case pup_error::file_entries:
            case pup_error::stream:
            {
                std::string error = "Error while installing firmware: PUP file is invalid.";

                if (!pup.get_formatted_error().empty())
                {
                    fmt::append(error, "\n%s", pup.get_formatted_error().data());
                }

                LOGE("%s", error.data());
                LOGE("Firmware installation failed: The provided file is corrupted.");
                return false;
            }
            case pup_error::hash_mismatch:
            {
                LOGE("Error while installing firmware: Hash check failed.");
                LOGE("Firmware installation failed: The provided file's contents are corrupted.");
                return false;
            }
            case pup_error::ok: break;
        }

        fs::file update_files_f = pup.get_file(0x300);

        const usz update_files_size = update_files_f ? update_files_f.size() : 0;

        if (!update_files_size)
        {
            LOGE("Error while installing firmware: Couldn't find installation packages database.");
            LOGE("Firmware installation failed: The provided file's contents are corrupted.");
            return false;
        }

        fs::device_stat dev_stat{};
        if (!fs::statfs(g_cfg_vfs.get_dev_flash(), dev_stat))
        {
            LOGE("Error while installing firmware: Couldn't retrieve available disk space. ('%s')", g_cfg_vfs.get_dev_flash().data());
            LOGE("Firmware installation failed: Couldn't retrieve available disk space.");
            return false;
        }

        if (dev_stat.avail_free < update_files_size)
        {
            LOGE("Error while installing firmware: Out of disk space. ('%s', needed: %d bytes)", g_cfg_vfs.get_dev_flash().data(), update_files_size - dev_stat.avail_free);
            LOGE("Firmware installation failed: Out of disk space.");
            return false;
        }

        tar_object update_files(update_files_f);

        auto update_filenames = update_files.get_filenames();

        update_filenames.erase(std::remove_if(
                                       update_filenames.begin(), update_filenames.end(), [](const std::string& s) { return s.find("dev_flash_") == umax; }),
                               update_filenames.end());

        if (update_filenames.empty())
        {
            LOGE("Error while installing firmware: No dev_flash_* packages were found.");
            LOGE("Firmware installation failed: The provided file's contents are corrupted.");
            return false;
        }

        static constexpr std::string_view cur_version = "4.91";

        std::string version_string;

        if (fs::file version = pup.get_file(0x100))
        {
            version_string = version.to_string();
        }

        if (const usz version_pos = version_string.find('\n'); version_pos != umax)
        {
            version_string.erase(version_pos);
        }

        if (version_string.empty())
        {
            LOGE("Error while installing firmware: No version data was found.");
            LOGE("Firmware installation failed: The provided file's contents are corrupted.");
            return false;
        }

        LOGI("FIRMWARD VER %s",version_string.data());

        if (std::string installed = utils::get_firmware_version(); !installed.empty())
        {
            LOGW("Reinstalling firmware: old=%s, new=%s", installed.data(), version_string.data());
        }

        // Used by tar_object::extract() as destination directory
        g_fxo->reset();
        //g_cfg_vfs.from_default();

        vfs::mount("/dev_flash", g_cfg_vfs.get_dev_flash());

        for (const auto& update_filename : update_filenames)
        {
            auto update_file_stream = update_files.get_file(update_filename);

            if (update_file_stream->m_file_handler)
            {
                // Forcefully read all the data
                update_file_stream->m_file_handler->handle_file_op(*update_file_stream, 0, update_file_stream->get_size(umax), nullptr);
            }

            fs::file update_file = fs::make_stream(std::move(update_file_stream->data));

            SCEDecrypter self_dec(update_file);
            self_dec.LoadHeaders();
            self_dec.LoadMetadata(SCEPKG_ERK, SCEPKG_RIV);
            self_dec.DecryptData();

            auto dev_flash_tar_f = self_dec.MakeFile();
            if (dev_flash_tar_f.size() < 3)
            {
                LOGE("Error while installing firmware: PUP contents are invalid.");
                LOGE("Firmware installation failed: Firmware could not be decompressed");
                //progress = -1;
                return false;
            }

            tar_object dev_flash_tar(dev_flash_tar_f[2]);
            if (!dev_flash_tar.extract())
            {
                LOGE("Error while installing firmware: TAR contents are invalid. (package=%s)", update_filename.data());
                LOGE("The firmware contents could not be extracted."
                     "\nThis is very likely caused by external interference from a faulty anti-virus software."
                     "\nPlease add RPCS3 to your anti-virus\' whitelist or use better anti-virus software.");

                //progress = -1;
                return false;
            }
        }
        update_files_f.close();
        LOGW("install_firmware ok");
        return true;
    }

    bool install_pkg(iso_fs& iso_fs, const std::string& path){

        std::deque<package_reader> readers;
        readers.emplace_back(iso_fs, path);

        std::deque<std::string> bootable_paths;

        package_install_result result = package_reader::extract_data(readers, bootable_paths);
        LOGW("install_pkg %d %s %s",result.error,result.version.expected.c_str(),result.version.found.c_str());
        return result.error == package_install_result::error_type::no_error;
    }

    bool install_pkg(const char* path){
        std::deque<package_reader> readers;
        readers.emplace_back(std::string(path));

        std::deque<std::string> bootable_paths;

        package_install_result result = package_reader::extract_data(readers, bootable_paths);
        LOGW("install_pkg %d %s %s",result.error,result.version.expected.c_str(),result.version.found.c_str());
        return result.error == package_install_result::error_type::no_error;

    }
    bool install_pkg(int pkg_fd){
        std::deque<package_reader> readers;
        readers.emplace_back(fs::file::from_fd(pkg_fd));

        std::deque<std::string> bootable_paths;

        package_install_result result = package_reader::extract_data(readers, bootable_paths);
        LOGW("install_pkg %d %s %s",result.error,result.version.expected.c_str(),result.version.found.c_str());
        return result.error == package_install_result::error_type::no_error;

    }

    bool allow_eboot_decrypt(const fs::file& eboot_file){
        fs::file dec_eboot = decrypt_self(eboot_file);
        return dec_eboot?true:false;
    }
}