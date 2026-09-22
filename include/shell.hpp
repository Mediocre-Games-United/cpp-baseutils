#pragma once

#include "base_types.hpp"
#include "file.hpp"
#include "logger.hpp"

#include <cerrno>
#include <cstring>
#include <format>
#include <iostream>
#include <string>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#else

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#endif

namespace cbu {

    #ifdef _WIN32

    namespace detail {

        inline std::wstring utf8_to_wide(const std::string& value) {
            if (value.empty()) {
                return {};
            }

            int size = MultiByteToWideChar(
                CP_UTF8,
                MB_ERR_INVALID_CHARS,
                value.data(),
                                           static_cast<int>(value.size()),
                                           nullptr,
                                           0
            );

            if (size <= 0) {
                return {};
            }

            std::wstring result(static_cast<std::size_t>(size), L'\0');

            MultiByteToWideChar(
                CP_UTF8,
                MB_ERR_INVALID_CHARS,
                value.data(),
                                static_cast<int>(value.size()),
                                result.data(),
                                size
            );

            return result;
        }

        inline std::string windows_error_message(DWORD error_code) {
            if (error_code == ERROR_SUCCESS) {
                return {};
            }

            LPSTR message_buffer = nullptr;

            DWORD size = FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                error_code,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                        reinterpret_cast<LPSTR>(&message_buffer),
                                        0,
                                        nullptr
            );

            std::string message(
                message_buffer != nullptr ? message_buffer : "Unknown Windows error",
                size
            );

            if (message_buffer != nullptr) {
                LocalFree(message_buffer);
            }

            while (!message.empty() &&
                (message.back() == '\r' || message.back() == '\n')) {
                message.pop_back();
                }

                return message;
        }

    } // namespace detail

    /*
     * Executes a shell command in directory.
     *
     * Both stdout and stderr are written to output.
     *
     * On Windows, the command is executed through:
     *
     *     cmd.exe /C command
     *
     * Do not pass untrusted, unvalidated user input directly to command.
     */
    inline uint8_t run_shell_command(
        fpath directory,
        string command,
        string* output
    ) {
        log_verbose(std::format("Shell: running command {}", command));

        if (output == nullptr) {
            log_info("Output is NULL! streaming output to stdout...");
        } else {
            output->clear();
        }

        SECURITY_ATTRIBUTES security_attributes{};
        security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
        security_attributes.bInheritHandle = TRUE;

        HANDLE read_handle = nullptr;
        HANDLE write_handle = nullptr;

        if (!CreatePipe(
            &read_handle,
            &write_handle,
            &security_attributes,
            0)) {
            log_error(
                false,
                "Could not create command output pipe: " +
                detail::windows_error_message(GetLastError())
            );

        return static_cast<uint8_t>(-1);
            }

            // The parent must not accidentally pass the read side to the child.
            if (!SetHandleInformation(read_handle, HANDLE_FLAG_INHERIT, 0)) {
                CloseHandle(read_handle);
                CloseHandle(write_handle);

                log_error(
                    false,
                    "Could not configure command output pipe: " +
                    detail::windows_error_message(GetLastError())
                );

                return static_cast<uint8_t>(-1);
            }

            std::wstring working_directory =
            detail::utf8_to_wide(path_to_utf8(directory));

            std::wstring command_line =
            L"cmd.exe /C " + detail::utf8_to_wide(command);

            if (working_directory.empty() && !path_to_utf8(directory).empty()) {
                CloseHandle(read_handle);
                CloseHandle(write_handle);

                log_error(false, "Could not convert working directory to UTF-16");
                return static_cast<uint8_t>(-1);
            }

            if (command_line.empty()) {
                CloseHandle(read_handle);
                CloseHandle(write_handle);

                log_error(false, "Could not convert command to UTF-16");
                return static_cast<uint8_t>(-1);
            }

            // CreateProcess may modify the command-line buffer.
            std::vector<wchar_t> mutable_command_line(
                command_line.begin(),
                                                      command_line.end()
            );
            mutable_command_line.push_back(L'\0');

            STARTUPINFOW startup_info{};
            startup_info.cb = sizeof(STARTUPINFOW);
            startup_info.dwFlags |= STARTF_USESTDHANDLES;
            startup_info.hStdOutput = write_handle;
            startup_info.hStdError = write_handle;
            startup_info.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

            PROCESS_INFORMATION process_info{};

            log_debug(std::format(
                "Shell: entering directory {}",
                path_to_utf8(directory)
            ));

            BOOL created = CreateProcessW(
                nullptr,
                mutable_command_line.data(),
                                          nullptr,
                                          nullptr,
                                          TRUE,
                                          CREATE_UNICODE_ENVIRONMENT,
                                          nullptr,
                                          working_directory.empty()
                                          ? nullptr
                                          : working_directory.c_str(),
                                          &startup_info,
                                          &process_info
            );

            // The parent no longer needs the write side.
            CloseHandle(write_handle);

            if (!created) {
                CloseHandle(read_handle);

                log_error(
                    false,
                    "Could not create command process: " +
                    detail::windows_error_message(GetLastError())
                );

                return static_cast<uint8_t>(-1);
            }

            char buffer[4096];

            for (;;) {
                DWORD bytes_read = 0;

                BOOL success = ReadFile(
                    read_handle,
                    buffer,
                    static_cast<DWORD>(sizeof(buffer)),
                                        &bytes_read,
                                        nullptr
                );

                if (success && bytes_read > 0) {
                    if (output == nullptr) {
                        std::cout.write(buffer, bytes_read);
                        std::cout.flush();
                    } else {
                        output->append(buffer, bytes_read);
                    }

                    continue;
                }

                if (!success) {
                    DWORD error = GetLastError();

                    if (error == ERROR_BROKEN_PIPE) {
                        break;
                    }

                    log_warn(
                        "Could not read command output: " +
                        detail::windows_error_message(error)
                    );
                }

                break;
            }

            CloseHandle(read_handle);

            WaitForSingleObject(process_info.hProcess, INFINITE);

            DWORD exit_code = 0;

            if (!GetExitCodeProcess(process_info.hProcess, &exit_code)) {
                log_error(
                    false,
                    "Could not get command exit status: " +
                    detail::windows_error_message(GetLastError())
                );

                CloseHandle(process_info.hThread);
                CloseHandle(process_info.hProcess);

                return static_cast<uint8_t>(-1);
            }

            CloseHandle(process_info.hThread);
            CloseHandle(process_info.hProcess);

            if (exit_code == 0) {
                log_success("Shell command completed successfully");
                return 0;
            }

            if (output != nullptr) {
                log_error(
                    false,
                    "Shell: command '" + command +
                    "' exited with status " +
                    std::to_string(exit_code) +
                    " with output " + *output
                );
            } else {
                log_error(
                    false,
                    "Shell: command '" + command +
                    "' exited with status " +
                    std::to_string(exit_code)
                );
            }

            return static_cast<uint8_t>(exit_code);
    }

    #else

    inline uint8_t run_shell_command(
        fpath directory,
        string command,
        string* output
    ) {
        log_verbose(std::format("Shell: running command {}", command));

        if (output == nullptr) {
            log_info("Output is NULL! streaming output to stdout...");
        } else {
            output->clear();
        }

        int pipe_fds[2];

        if (pipe(pipe_fds) == -1) {
            log_error(
                false,
                "Could not create command output pipe: " +
                string(std::strerror(errno))
            );

            return static_cast<uint8_t>(-1);
        }

        pid_t child_pid = fork();

        if (child_pid == -1) {
            close(pipe_fds[0]);
            close(pipe_fds[1]);

            log_error(
                false,
                "Could not fork command process: " +
                string(std::strerror(errno))
            );

            return static_cast<uint8_t>(-1);
        }

        if (child_pid == 0) {
            close(pipe_fds[0]);

            if (dup2(pipe_fds[1], STDOUT_FILENO) == -1 ||
                dup2(pipe_fds[1], STDERR_FILENO) == -1) {
                _exit(127);
                }

                close(pipe_fds[1]);

            log_debug(std::format(
                "Shell: entering directory {}",
                path_to_utf8(directory)
            ));

            if (chdir(path_to_utf8(directory).c_str()) == -1) {
                dprintf(
                    STDERR_FILENO,
                    "chdir failed: %s\n",
                    std::strerror(errno)
                );
                _exit(127);
            }

            execl(
                "/bin/sh",
                "sh",
                "-c",
                command.c_str(),
                  static_cast<char*>(nullptr)
            );

            dprintf(
                STDERR_FILENO,
                "exec failed: %s\n",
                std::strerror(errno)
            );

            _exit(127);
        }

        close(pipe_fds[1]);

        char buffer[4096];

        for (;;) {
            ssize_t bytes_read = read(
                pipe_fds[0],
                buffer,
                sizeof(buffer)
            );

            if (bytes_read > 0) {
                if (output == nullptr) {
                    std::cout.write(buffer, bytes_read);
                    std::cout.flush();
                } else {
                    output->append(
                        buffer,
                        static_cast<string::size_type>(bytes_read)
                    );
                }

                continue;
            }

            if (bytes_read == 0) {
                break;
            }

            if (errno == EINTR) {
                continue;
            }

            log_warn(
                "Could not read command output: " +
                string(std::strerror(errno))
            );

            break;
        }

        close(pipe_fds[0]);

        int wait_status = 0;

        while (waitpid(child_pid, &wait_status, 0) == -1) {
            if (errno == EINTR) {
                continue;
            }

            log_error(
                false,
                "Could not wait for command process: " +
                string(std::strerror(errno))
            );

            return static_cast<uint8_t>(-1);
        }

        if (WIFEXITED(wait_status)) {
            int exit_code = WEXITSTATUS(wait_status);

            if (exit_code == 0) {
                log_success("Shell command completed successfully");
                return 0;
            }

            if (output != nullptr) {
                log_error(
                    false,
                    "Shell: command '" + command +
                    "' exited with status " +
                    std::to_string(exit_code) +
                    " with output " + *output
                );
            } else {
                log_error(
                    false,
                    "Shell: command '" + command +
                    "' exited with status " +
                    std::to_string(exit_code)
                );
            }

            return static_cast<uint8_t>(exit_code);
        }

        if (WIFSIGNALED(wait_status)) {
            log_error(
                false,
                "Shell command was terminated by signal " +
                std::to_string(WTERMSIG(wait_status))
            );

            return static_cast<uint8_t>(-1);
        }

        log_error(false, "Shell command ended in an unknown state");
        return static_cast<uint8_t>(-1);
    }

    #endif

} // namespace cbu
