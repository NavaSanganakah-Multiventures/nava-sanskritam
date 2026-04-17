import sys

def process_file(filepath):
    with open(filepath, 'r') as f:
        lines = f.readlines()

    with open(filepath, 'w') as f:
        skip = False
        for line in lines:
            if "name: Push to External Preview Repository" in line:
                skip = True

            if skip:
                # If we encounter an empty line, or we reach the end, but actually this is the last step
                # so we can just skip the rest of the file
                pass
            else:
                f.write(line)

if __name__ == "__main__":
    process_file('.github/workflows/wasm-build.yml')
