import sys
import os
import re
# write your code here
errors = {"S001": "Too long",
          "S002": "Indentation is not a multiple of four",
          "S003": "Unnecessary semicolon after a statement",
          "S004": "At least two spaces required before inline comments",
          "S005": "TODO found",
          "S006": "More than two blank lines preceding a code line",
          "S007": "Too many spaces after '{construct}'",
          "S008": "Class name '{name}' should be written in CamelCase",
          "S009": "Function name '{name}' should be written in snake_case"}


def check_line_length(code_line):
    return "S001" if len(code_line) > 79 else None


def check_indentation(code_line):
    return "S002" if (len(code_line) - len(code_line.lstrip())) % 4 else None


def check_semicolon(code_line):
    in_single = False
    in_double = False
    comment = code_line.find("#") if "#" in code_line else len(code_line)
    semicolon = None
    for i, char in enumerate(code_line[:comment]):
        in_single = not in_single if char == "\'" and not in_double else in_single
        in_double = not in_double if char == "\"" and not in_single else in_double
        if char == ";" and not in_single and not in_double:
            semicolon = i
    return "S003" if semicolon is not None else None  # Returns error for multiple statements, too PEP8:E702


def check_inline_comments(code_line):
    return "S004" if not code_line.startswith("#") and "#" in code_line[1:] and code_line.find("  #") == -1 else None


def check_todo(code_line):
    return "S005" if -1 < code_line.find("#") < code_line.upper().find("TODO") else None


def check_name_spacing(code_line):
    check = re.match(r"(class|def)\s(\s*)", code_line)
    return ("S007", check[1]) if check and check[2] else None


def check_names(code_line):
    check = re.match(r"^(class|def) +(\w*)", code_line)
    if check:
        if check[1] == "class" and not re.match(r"^class +_?_?[A-Z][A-Za-z\d()]*:$", code_line):
            return "S008", check[2]
        if check[1] == "def" and not re. match(r"^def +_?_?[a-z][a-z\d_()]*:$", code_line):
            return "S009", check[2]
    return None


def check_file(path, code):
    # check style errors line by line
    count_blank = 0
    for c, line in enumerate(code, 1):
        err_code = []
        line = line.replace("\n", "")
        err_code.append(check_line_length(line))
        err_code.append(check_indentation(line))
        err_code.append(check_semicolon(line))
        err_code.append(check_inline_comments(line))
        err_code.append(check_todo(line))
        err_code.append("S006" if count_blank > 2 else None)
        count_blank = count_blank + 1 if line.strip() == "" else 0
        for err in err_code:
            print(f"{path}: Line {c}: {err} {errors[err]}") if err else ""
        err_code = check_name_spacing(line.lstrip())
        print(f"{path}: Line {c}: {err_code[0]} {errors[err_code[0]].format(construct=err_code[1])}")\
            if err_code else ""
        err_code = check_names(line.lstrip())
        print(f"{path}: Line {c}: {err_code[0]} {errors[err_code[0]].format(name=err_code[1])}") if err_code else ""


def list_files(path):
    if os.path.isfile(path) and path[-2:] == "py":
        code = open(path, "r")
        check_file(path, code)
        code.close()
    elif os.path.isdir(path):
        file_list = os.listdir(path)
        for file in file_list:
            list_files(path + "\\" + file)
    return


# file_path = input()

args = sys.argv
if len(args) < 2:
    print("Please enter project file or directory as an argument.")
else:
    list_files(args[1])
