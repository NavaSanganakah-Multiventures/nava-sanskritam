import sys

def process_file(filepath):
    with open(filepath, 'r') as f:
        lines = f.readlines()

    with open(filepath, 'w') as f:
        for line in lines:
            f.write(line)
            if "uses: actions/upload-artifact@v4" in line:
                f.write("      continue-on-error: true\n")

if __name__ == "__main__":
    process_file('.github/workflows/desktop-build.yml')
