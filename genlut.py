import math

def generateSinLut():
    lut = []

    for i in range(0, 91):
        lut.append(round(math.sin(math.radians(i)) * 0xFFFF))

    return lut

def buildLutArray(lut):
    code = "{\n   "
    col = 0

    for i in range(0, len(lut)):
        if col >= 8:
            code += "\n   "
            col = 0

        code += " 0x{0:0{1}X}".format(lut[i], 4)

        if i < len(lut) - 1:
            code += ","

        col += 1

    code += "\n}"

    return code

file = open("src/lut.c", "w")
sinLut = generateSinLut()

file.write("\n".join([
    "#include \"lut.h\"",
    "",
    "const unsigned int LUT_SIN[" + str(len(sinLut)) + "] = " + buildLutArray(sinLut) + ";"
]))

file.close()