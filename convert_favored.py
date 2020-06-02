with open('camera_favored.txt', 'r') as f:
    with open('camera_favored.h', 'w') as w:
        w.write("#pragma once\n")
        w.write("uint16_t FAVORED_ANGLES[] = {\n\t")
        lines = f.readlines()
        camera_angles = []
        count = 0
        for line in lines:
            v = line.strip()
            w.write("0x" + v + ",")
            count += 1
            if count % 16 == 0:
                w.write("\n\t")
        w.write("\n};\n")
        w.write(f"uint16_t FAVORED_COUNT = {count};\n")