#!/usr/bin/env python3
# coding=utf-8

import os
import platform
import subprocess
import logging
import tarfile
import time
import threading
import argparse
import shlex
import inspect
import warnings
import configparser
from dataclasses import dataclass
from typing import IO

class Color:
    PURPLE = '\033[95m'
    CYAN = '\033[96m'
    DARKCYAN = '\033[36m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'
    RESET = '\033[0m'

logging.basicConfig(
    format="[ {}LOGOUT{} ] %(asctime)s %(levelname)s {}@{} %(filename)s:%(lineno)d:%(funcName)s  %(message)s"
    .format(Color.GREEN + Color.BOLD, Color.RESET, Color.BLUE + Color.BOLD, Color.RESET),
    level=logging.INFO
)

def get_full_function_name(hidden_format_log: bool=True):
    """获取当前函数的完整名称（包含类名）"""
    stack = inspect.stack()
    frame = stack[1 + hidden_format_log].frame
    func_name = stack[1 + hidden_format_log].function
    
    # 检查是否是类方法
    if 'self' in frame.f_locals:
        # 实例方法
        class_name = frame.f_locals['self'].__class__.__name__
        return f"{class_name}.{func_name}"
    elif 'cls' in frame.f_locals:
        # 类方法
        class_name = frame.f_locals['cls'].__name__
        return f"{class_name}.{func_name}"
    else:
        # 普通函数
        return func_name

def exec(cmd: list[str]) -> tuple[int, str, str]:
    print(f"[  {Color.CYAN + Color.BOLD}EXEC{Color.RESET}  ] Executing: {cmd}")
    process = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            bufsize=1,
            universal_newlines=True
            )

    stdout_lines: list[str] = []
    stderr_lines: list[str] = []

    def read_stream(stream: IO, stream_name: str, lines: list[str]) -> None:
        for line in iter(stream.readline, ""):
            print(f"{stream_name}  {line}", end='')
            lines.append(line)

    stdout_thread = threading.Thread(
            target=read_stream,
            args=(process.stdout, f"[ {Color.YELLOW + Color.BOLD}STDOUT{Color.RESET} ]", stdout_lines)
            )
    stderr_thread = threading.Thread(
            target=read_stream,
            args=(process.stderr, f"[ {Color.RED + Color.BOLD}STDERR{Color.RESET} ]", stderr_lines)
            )

    stdout_thread.start()
    stderr_thread.start()

    return_code = process.wait()

    stdout_thread.join()
    stderr_thread.join()

    return return_code, "".join(stdout_lines), "".join(stderr_lines)


@dataclass
class CMakeTargetBuilder:
    cmake_path: str
    make_path: str
    target: str
    cmake_flags: list[str]
    make_flags: list[str]
    cmake_toolchain: str
    nproc: int
    host_compile: bool

    build_dir: str
    install_dir: str
    dist_dir: str
    src_dir: str

    project: str
    version: str
    other_info: str
    log_prefix: str

    can_continue: bool
    fatal_location: str

    def __init__(self,
                 src_dir: str,
                 project: str="",
                 version: str="",
                 other_info: str="",
                 log_prefix: str="",
                 make_path: str="/usr/bin/make",
                 dist_dir: str="dist",
                 cmake_path: str="/usr/bin/cmake",
                 target: str = "",
                 cmake_flags: list[str]=[],
                 make_flags: list[str] = [],
                 host_compile: bool=False,
                 cmake_toolchain: str=""):
        self.src_dir = src_dir
        self.dist_dir = dist_dir
        self.cmake_flags = cmake_flags
        self.cmake_path = cmake_path
        self.cmake_toolchain = cmake_toolchain
        self.target = target
        self.make_path = make_path
        self.project = project
        self.version = version
        self.other_info = other_info
        self.make_flags = make_flags
        self.log_prefix = log_prefix
        self.host_compile = host_compile

        self.can_continue = False
        self.fatal_location = ""
    
    def add_logprefix(self, prefix: str) -> None:
        self.log_prefix = prefix

    def formatlog(self, msg: str) -> str:
        return f"{self.log_prefix} Target:{self.target} [{get_full_function_name()}] {msg}"

    def prepare(self) -> None:
        logging.info(self.formatlog("--- Start Preparing."))

        self.nproc = 2 if os.process_cpu_count() is None else os.process_cpu_count() * 2

        self.build_dir = f"{self.target}.build"
        self.install_dir = f"{self.target}.install"
        os.makedirs(self.build_dir, exist_ok=True)
        os.makedirs(self.install_dir, exist_ok=True)
        if not os.path.exists(self.dist_dir):
            os.makedirs(self.dist_dir, exist_ok=True)

        if not self.host_compile:
            if not os.path.exists(self.cmake_toolchain):
                logging.fatal(self.formatlog(f"Toolchain file does not exist: {self.cmake_toolchain}"))
                self.fatal_location = get_full_function_name(False)
                return
            if not os.access(self.cmake_toolchain, os.R_OK):
                logging.fatal(self.formatlog(f"Toolchain file not readable: {self.cmake_toolchain}"))
                self.fatal_location = get_full_function_name(False)
                return
            logging.info(self.formatlog(f"Using toolchain: {self.cmake_toolchain}"))
        else:
            self.cmake_toolchain = ""
            logging.info(self.formatlog("Native compilation, no toolchain needed"))

        if (not os.path.exists(self.build_dir)) or (not os.path.exists(self.install_dir)) or (not os.path.exists(self.src_dir)):
            logging.fatal(self.formatlog(f"Cannot open build_dir ({self.build_dir}) or install_dir ({self.install_dir}) or src_dir ({self.src_dir})."))
            self.fatal_location = get_full_function_name(False)
            return

        if subprocess.run([self.cmake_path, "--version"]).returncode != 0:
            logging.fatal(self.formatlog(f"CMake ({self.cmake_path}) cannot work"))
            self.fatal_location = get_full_function_name(False)
            return
        
        if subprocess.run([self.make_path, "--version"]).returncode != 0:
            logging.fatal(self.formatlog(f"Make ({self.make_path}) cannot work"))
            self.fatal_location = get_full_function_name(False)
            return

        self.can_continue = True
        logging.info(self.formatlog("--- End Preparing."))

    def configure(self) -> None:
        logging.info(self.formatlog("--- Start Configuring."))

        if not self.can_continue:
            logging.fatal(self.formatlog("Cannot configure project, can_continue is False."))
            return

        self.can_continue = False

        cmake_args = [self.cmake_path, "-B", self.build_dir, "-S", self.src_dir] + self.cmake_flags
        if not self.host_compile:
            cmake_args.append(f"-DCMAKE_TOOLCHAIN_FILE={self.cmake_toolchain}")
        cmake_args.append(f"-DCMAKE_INSTALL_PREFIX={self.install_dir}")
        
        result: int = -1
        stdout: str = ""
        stderr: str = ""
        try:
            result, stdout, stderr = exec(cmake_args)
        except Exception as e:
            logging.fatal(self.formatlog(f"Configure failed: {cmake_args} returns {result}. Exception: {e}. Stderr output:"))
            print(stderr)
            self.can_continue = False
            self.fatal_location = get_full_function_name(False)
            return

        if result == 0:
            self.can_continue = True
        else:
            self.can_continue = False
            self.fatal_location = get_full_function_name(False)

        logging.info(self.formatlog("--- End Configuring."))

    def compile(self) -> None:
        logging.info(self.formatlog("--- Start Compiling."))

        if not self.can_continue:
            logging.fatal(self.formatlog("Cannot compile project, can_continue is False."))
            return

        self.can_continue = False

        make_args = [f"{self.make_path}", "-j", f"{self.nproc}", "-C", self.build_dir] + self.make_flags

        result: int = -1
        stdout: str = ""
        stderr: str = ""
        try:
            result, stdout, stderr = exec(make_args)
        except Exception as e:
            logging.fatal(self.formatlog(f"Compile failed: {make_args} returns {result}. Exception: {e}. Stderr output:"))
            print(stderr)
            self.can_continue = False
            self.fatal_location = get_full_function_name(False)
            return

        if result == 0:
            self.can_continue = True
        else:
            self.can_continue = False
            self.fatal_location = get_full_function_name(False)

        logging.info(self.formatlog("--- End Compiling."))

    def install(self) -> None:
        logging.info(self.formatlog("--- Start Installing."))

        if not self.can_continue:
            logging.fatal(self.formatlog("Cannot install, can_continue is False."))
            return

        self.can_continue = False

        result_install: int = -1
        result_copy: int = -1
        stdout: str = ""
        stderr: str = ""
        try:
            result_install, stdout, stderr = exec([f"{self.make_path}", "install", "-C", self.build_dir])
            result_copy, stdout, stderr = exec(["cp", "-rv", f"{self.src_dir}/LICENSE", f"{self.src_dir}/README.md", f"{self.src_dir}/doc", self.build_dir])
        except Exception as e:
            logging.fatal(self.formatlog(f"Install failed: Make install eturns {result_install}, cp returns {result_copy}. Exception: {e}. Stderr output:"))
            print(stderr)
            self.can_continue = False
            self.fatal_location = get_full_function_name(False)
            return
       
        if result_install == 0 and result_copy == 0:
            self.can_continue = True
        else:
            self.can_continue = False
            self.fatal_location = get_full_function_name(False)

        logging.info(self.formatlog("--- End Installing."))

    def package(self) -> None:
        logging.info(self.formatlog("--- Start Packaging."))

        if not self.can_continue:
            logging.fatal(self.formatlog("Cannot install, can_continue is False."))
            return

        self.can_continue = False

        file_name: str = ""
        if (self.project == "") or (self.version == ""):
            file_name = f"project-{self.target}.tar.gz"
        else:
            file_name = f"{self.project}-{self.version}-{self.other_info}-{self.target}.tar.gz"

        with tarfile.open(f"{self.dist_dir}/{file_name}", "w:gz") as tar:
            tar.add(self.install_dir)

        logging.info(self.formatlog("--- End Packaging."))


@warnings.deprecated("You should use RunBuild1.")
class RunBuild:

    def __init__(self):
        self.parser = argparse.ArgumentParser()

        self.parser.add_argument("src_dir")
        self.parser.add_argument("dist_dir")
        self.parser.add_argument("project")
        self.parser.add_argument("version")
        self.parser.add_argument("other_info")
        self.parser.add_argument("--make_flags")
        self.parser.add_argument("--cmake_flags")
        self.parser.add_argument("--cmake_path")
        self.parser.add_argument("--make_path")

        self.args = self.parser.parse_args()

        self.make_flags: list[str] = []
        self.cmake_flags: list[str] = []
        self.make_path: str = "/usr/bin/make"
        self.cmake_path: str = "/usr/bin/cmake"

        if self.args.make_flags:
            self.make_flags = self.args.make_flags.split()
        if self.args.cmake_flags:
            self.cmake_flags = self.args.cmake_flags.split()
        if self.args.make_path:
            self.make_path = self.args.make_path
        if self.args.cmake_path:
            self.cmake_path = self.args.cmake_path

        self.target_list: list[CMakeTargetBuilder] = []

    def add_targets(self) -> None:
        host_arch = platform.machine().lower()
        host_system = platform.system().lower()

        logging.info(f"Host architecture: {host_arch}")
        logging.info(f"Host system: {host_system}")

        targets: list[dict[str, str]] = [
            {"target": "aarch64-linux", "toolchain": "aarch64-linux-toolchain.cmake"},
            {"target": "x86_64-linux", "toolchain": "x86_64-linux-toolchain.cmake"},
            {"target": "x86_64-mingw", "toolchain": "x86_64-mingw-toolchain.cmake"}
        ]

        for target in targets:
            host_compile = False
            if f"{host_arch}-{host_system}" == target["target"]:
                host_compile = True

            self.target_list.append(CMakeTargetBuilder(
                src_dir=self.args.src_dir,
                project=self.args.project,
                version=self.args.version,
                other_info=self.args.other_info,
                dist_dir=self.args.dist_dir,
                cmake_path=self.cmake_path,
                make_path=self.make_path,
                cmake_flags=self.cmake_flags,
                make_flags=self.make_flags,
                target=f"{target['target']}",
                cmake_toolchain=f"{self.args.src_dir}/cmake/{target['toolchain']}",
                host_compile=host_compile
            ))
        
    def prepare_all(self) -> None:
        for target in self.target_list:
            target.prepare()

    def configure_all(self) -> None:
        for target in self.target_list:
            target.configure()

    def compile_all(self) -> None:
        for target in self.target_list:
            target.compile()

    def install_all(self) -> None:
        for target in self.target_list:
            target.install()

    def package_all(self) -> None:
        for target in self.target_list:
            target.package()

class RunBuild1:

    def __init__(self):
        self.cparser = configparser.ConfigParser()
        self.aparser = argparse.ArgumentParser()

        self.aparser.add_argument("-C", "--directory", default=".", help="Chdir before build.")
        self.aparser.add_argument("--configfile", default="build.ini", help="The build configure file (Ini format).", required=True)
        self.args = self.aparser.parse_args()

        self.cparser.read(self.args.configfile)

        self.target_list: list[CMakeTargetBuilder] = []
        self.log_prefix = ""
        self.start_time_ns: int = time.perf_counter_ns()
        self.end_time_ns:int = 0

    def formatlog(self, msg: str) -> str:
        return f"{self.log_prefix} [{get_full_function_name()}] {msg}"

    def add_targets(self) -> None:
        self.src_dir = self.cparser.get("project", "dirs.src")
        self.dist_dir = self.cparser.get("project", "dirs.dist")
        self.project = self.cparser.get("project", "name")
        self.version = self.cparser.get("project", "version")
        self.other_info = self.cparser.get("project", "other_info", fallback="")
        self.make_path = self.cparser.get("project", "path.make", fallback="/usr/bin/make")
        self.cmake_path = self.cparser.get("project", "path.cmake", fallback="/usr/bin/cmake")
        self.cmake_flags = shlex.split(self.cparser.get("project", "flags.cmake", fallback=""))
        self.make_flags = shlex.split(self.cparser.get("project", "flags.make", fallback=""))
        self.toolchain_force = self.cparser.getboolean("project", "toolchain.force", fallback=False)

        host_target = f"{platform.machine().lower()}-{platform.system().lower()}"
        logging.info(self.formatlog(f"Host target: {host_target}."))
        logging.info(self.formatlog(f"Use build configure: {self.args.configfile}"))
        
        for section in self.cparser:
            if section.startswith("target."):
                target = self.cparser.get(section, "name")
                make_path = self.cparser.get(section, "path.make", fallback=self.make_path)
                cmake_path = self.cparser.get(section, "path.cmake", fallback=self.cmake_path)
                cmake_flags = shlex.split(self.cparser.get(section, "flags.cmake", fallback="")) + self.cmake_flags
                make_flags = shlex.split(self.cparser.get(section, "flags.make", fallback="")) + self.make_flags
                toolchain_file = self.cparser.get(section, "toolchain.file")
                toolchain_force = self.cparser.getboolean(section, "toolchain.force", fallback=self.toolchain_force)
                if self.cparser.getboolean(section, "toolchain.internal", fallback=True):
                    toolchain_path = f"{self.src_dir}/cmake/{toolchain_file}"
                else:
                    toolchain_path = toolchain_file
        
                host_compile = (target == host_target)
                if toolchain_force:
                    host_compile = False

                builder = CMakeTargetBuilder(
                        src_dir=self.src_dir,
                        project=self.project,
                        version=self.version,
                        other_info=self.other_info,
                        dist_dir=self.dist_dir,
                        cmake_path=cmake_path,
                        make_path=make_path,
                        cmake_flags=cmake_flags,
                        make_flags=make_flags,
                        target=target,
                        cmake_toolchain=toolchain_path,
                        host_compile=host_compile
                        )
                self.target_list.append(builder)

                logging.info(self.formatlog(f"Add Target:{target} Toolchain:{toolchain_path}{(lambda x: f" {Color.GREEN}HostCompile:True{Color.RESET}" if x else "")(host_compile)}."))

    def chdir(self) -> None:
        self.current_dir = os.getcwd()
        build_base_dir = os.path.abspath(self.args.directory)

        if self.current_dir == build_base_dir:
            return
        else:
            try:
                os.chdir(build_base_dir)
            except Exception as e:
                logging.fatal(self.formatlog(f"Cannot change directory to {build_base_dir}, error: {e}"))
                os.chdir(self.current_dir)
        
    def prepare_all(self) -> None:
        i: int = 1
        total = len(self.target_list)
        logging.info(self.formatlog(f"{Color.BLUE + Color.BOLD}*** START PREPARING ***{Color.RESET}"))
        for target in self.target_list:
            target.add_logprefix(f"Preparing: {Color.BLUE}{i}/{total}{Color.RESET}")
            target.prepare()
            target.add_logprefix("")
            i += 1

    def configure_all(self) -> None:
        i: int = 1
        total = len(self.target_list)
        logging.info(self.formatlog(f"{Color.BLUE + Color.BOLD}*** START CONFIGURING ***{Color.RESET}"))
        for target in self.target_list:
            target.add_logprefix(f"Configuring: {Color.BLUE}{i}/{total}{Color.RESET}")
            target.configure()
            target.add_logprefix("")
            i += 1

    def compile_all(self) -> None:
        i: int = 1
        total = len(self.target_list)
        logging.info(self.formatlog(f"{Color.BLUE + Color.BOLD}*** START COMPILING ***{Color.RESET}"))
        for target in self.target_list:
            target.add_logprefix(f"Compiling: {Color.BLUE}{i}/{total}{Color.RESET}")
            target.compile()
            target.add_logprefix("")
            i += 1

    def install_all(self) -> None:
        i: int = 1
        total = len(self.target_list)
        logging.info(self.formatlog(f"{Color.BLUE + Color.BOLD}*** START INSTALLING ***{Color.RESET}"))
        for target in self.target_list:
            target.add_logprefix(f"Installing: {Color.BLUE}{i}/{total}{Color.RESET}")
            target.install()
            target.add_logprefix("")
            i += 1
            
    def package_all(self) -> None:
        i: int = 1
        total = len(self.target_list)
        logging.info(self.formatlog(f"{Color.BLUE + Color.BOLD}*** START PACKAGING ***{Color.RESET}"))
        for target in self.target_list:
            target.add_logprefix(f"Packaging: {Color.BLUE}{i}/{total}{Color.RESET}")
            target.package()
            target.add_logprefix("")
            i += 1

    def finish(self) -> None:
        try:
            os.chdir(self.current_dir)
        except Exception as e:
            logging.fatal(self.formatlog(f"Cannot change directory to {self.current_dir}, error: {e}"))

        self.end_time_ns = time.perf_counter_ns()
        elapsed_ns = self.end_time_ns - self.start_time_ns

        time.sleep(3)

        logging.info(self.formatlog(f"{Color.GREEN + Color.BOLD + Color.UNDERLINE}*** *** Build finish *** ***{Color.RESET}"))
        failed: int = 0
        passed: int = 0
        for target in self.target_list:
            if target.fatal_location != "":
                logging.info(self.formatlog(f"Target {target.target} {Color.RED + Color.BOLD}FAILED{Color.RESET} at location {target.fatal_location}."))
                failed += 1
            else:
                 logging.info(self.formatlog(f"Target {target.target} {Color.GREEN + Color.BOLD}PASSED{Color.RESET}."))
                 passed += 1
        logging.info(self.formatlog(f"Count: {Color.BLUE}{failed + passed}{Color.RESET} total, {Color.GREEN}{passed}{Color.RESET} passed, {Color.RED}{failed}{Color.RESET} failed."))
        logging.info(self.formatlog(f"Start at {self.start_time_ns}, end at {self.end_time_ns}, total {elapsed_ns} ns."))

if __name__ == "__main__":
    rb = RunBuild1()
    rb.chdir()
    rb.add_targets()
    rb.prepare_all()
    rb.configure_all()
    rb.compile_all()
    rb.install_all()
    rb.package_all()
    rb.finish()

