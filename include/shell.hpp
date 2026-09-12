#pragma once

#include "base_types.hpp"
#include "file.hpp"
#include "logger.hpp"

#include <cerrno>
#include <cstring>
#include <format>
#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace cbu {

    /*
     * Executes a shell command in directory.
     *
     * Both stdout and stderr are written to output.
     *
     * Returns true when the command exits with status 0.
     * Returns false when the command fails or exits with a non-zero status.
     *
     * Note:
     * command is executed through /bin/sh -c. Do not pass untrusted,
     * unvalidated user input directly to command.
     */
    inline uint8_t run_shell_command(
        fpath directory,
        string command,
        string* output
    ) {
        log_verbose(std::format("Shell: running command {}",command));

        if (output == nullptr) {
            log_info("Output is NULL! streaming output to stdout...");
        } else output->clear();

        int pipe_fds[2];

        if (pipe(pipe_fds) == -1) {
            log_error(
                false,
                "Could not create command output pipe: " +
                string(std::strerror(errno))
            );

            return -1;
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

            return -1;
        }

        if (child_pid == 0) {
            close(pipe_fds[0]);

            /*
             * Redirect stdout and stderr to the same pipe.
             */
            if (dup2(pipe_fds[1], STDOUT_FILENO) == -1 ||
                dup2(pipe_fds[1], STDERR_FILENO) == -1) {
                _exit(127);
                }

                close(pipe_fds[1]);

            log_debug(std::format("Shell: entering directory {}",path_to_utf8(directory)));
            if (chdir(path_to_utf8(directory).c_str()) == -1) {
                dprintf(STDERR_FILENO, "chdir failed: %s\n", strerror(errno));
                _exit(127);
            }

            execl(
                "/bin/sh",
                "sh",
                "-c",
                command.c_str(),
                static_cast<char*>(nullptr)
            );

            dprintf(STDERR_FILENO, "exec failed: %s\n", strerror(errno));
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
                if (!output) {
                    string bfr = "";
                    bfr.append(buffer,static_cast<string::size_type>(bytes_read));

                    std::cout << bfr;
                } else output->append(
                    buffer,
                    static_cast<string::size_type>(bytes_read)
                );
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

            return -1;
        }

        if (WIFEXITED(wait_status)) {
            int exit_code = WEXITSTATUS(wait_status);

            if (exit_code == 0) {
                log_success("Shell command completed successfully");
                return 0;
            }

            if (output) {
                log_error(
                    false,
                    "Shell: command '" + command + "' exited with status " +
                    std::to_string(exit_code) + " with output " + *output
                );
            } else log_error(
                    false,
                    "Shell: command '" + command + "' exited with status " +
                    std::to_string(exit_code)
                );

            return exit_code;
        }

        if (WIFSIGNALED(wait_status)) {
            int signal_number = WTERMSIG(wait_status);

            log_error(
                false,
                "Shell command was terminated by signal " +
                std::to_string(signal_number)
            );

            return -1;
        }

        log_error(false, "Shell command ended in an unknown state");
        return -1;
    }

} // namespace cbu

