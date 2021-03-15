import yaml
import re

PACKET_HEADER_SIZE = 0

def split_camel_case(camelStr):
    return re.split("(?<=[a-z])(?=[A-Z])|(?<=[A-Z])(?=[A-Z][a-z])", camelStr)

def convert_camel_case_to_caps_underscore(camelStr):
    splitStr = split_camel_case(camelStr)
    output = ""
    for s in splitStr:
        output += s.upper() + "_"
    return output[0:-1]

def convert_camel_case_to_lower_underscore(camelStr):
    splitStr = split_camel_case(camelStr)
    output = ""
    for s in splitStr:
        output += s.lower() + "_"
    return output[0:-1]

python_struct_pack_str_dict = {
"uint8_t": "<B",
"int8_t": "<b",
"uint16_t": "<H",
"int16_t": "<h",
"uint32_t": "<L",
"int32_t": "<l",
"float": "<f",
"uint64_t": "<Q"
}

class EnumField:
    def __init__(self, name, value):
        self.name = name
        self.value = value

class EnumClass:
    def __init__(self, fields):
        self.fields = fields

enumDict = {}
definesDict = {}

def python_get_packet_size_lookup():
    output = "packet_size_lookup_dict = {\n"

    for key in packetsDict:
        packet = packetsDict[key]
        output += "    fp_type_t.{}: {},\n".format(packetTypesDict[packet.name]["typeName"], packet.size)

    output = output[0:-1]
    output += "\n};"
    return output

def python_get_packet_size_str():
    output = ""
    output += python_get_packet_size_lookup()
    output += "\n"
    output += "\n"

    output += "def get_packet_size(packetType):"
    output += "\n   return packet_size_lookup_dict[packetType]"
    output += "\n"
    output += "\n"

    return output

def python_defines_str():
    output = ""
    for define in definesDict:
        output += "{} = {}\n".format(define, definesDict[define])
    output += "\n"
    return output


def python_encode_header_str():
    output = ""
    output += "def encode_header(dest, packetType, offset):"
    output += "\n    struct.pack_into('<B', dest, offset, PACKET_DELIMITER1)"
    output += "\n    struct.pack_into('<B', dest, offset + 1, PACKET_DELIMITER2)"
    output += "\n    struct.pack_into('<B', dest, offset + 2, packetType)\n\n"
    return output

def python_encode_field_str(field, offset):
    output = ""
    if (field.varType.isPrimitive):
        output += "struct.pack_into('{}', dest, offset + {}, self.{})".format(python_struct_pack_str_dict[field.varType.nameStr], offset, field.name)
    else:
        output += "self.{}.encode(dest, offset + {})".format(field.name, offset)
    return output

def python_class_str(className, fields, isPacket=False):
    output = ""
    output += "class {}:".format(className)
    output += "\n    def __init__(self, encoded=None, offset=0, **kwargs):"
    output += "\n        if encoded:"
    output += "\n            self.decode(encoded, offset)"
    output += "\n        else:"
    for field in fields:
        output += "\n            self.{} = kwargs.get(\"{}\", 0)".format(field.name, field.name)
    output += "\n\n    def encode(self, dest, offset=0):"
    offset = 0

    if (isPacket):
        output += "\n        encode_header(dest, fp_type_t.{}, 0)".format(packetTypesDict[className]["typeName"])
        offset = PACKET_HEADER_SIZE

    for field in fields:
        output += "\n        {}".format(python_encode_field_str(field, offset))
        offset += field.varType.size
    output += "\n"
    output += "\n"
    return output

def python_enum_str(className):
    output = ""
    output += "class {}(IntEnum):".format(className)
    enumClass = enumDict[className]
    fields = enumClass.fields
    for field in fields:
        output += "\n    {} = {}".format(field.name, field.value)
    output += "\n"
    output += "\n"
    return output

def build_python_packet_type_enum():
    count = 0
    fields = []
    for key in packetsDict:
        packet = packetsDict[key]
        packetTypeName = convert_camel_case_to_caps_underscore(key)
        packetTypeEnumName = "FPT_{}".format(packetTypeName)
        fields += [EnumField(packetTypeEnumName, count)]
        count += 1

    fields += [EnumField("FPT_CNT", count)]

    output = ""
    output += "class fp_type_t(IntEnum):"
    for field in fields:
        output += "\n    {} = {}".format(field.name, field.value)
    output += "\n"
    output += "\n"
    return output


class Field:
    def __init__(self, varType, name):
        self.varType = varType
        self.name = name

class VarType:
    def __init__(self, nameStr, size, isPrimitive = False, isEnum = False, fields = None):
        self.nameStr = nameStr
        self.size = size
        self.isPrimitive = isPrimitive
        self.isEnum = isEnum
        self.fields = fields

    def build_struct_str(self):
        output = "typedef struct {"
        for field in self.fields:
            output += "\n  {} {};".format(field.varType.nameStr, field.name)
        output += "\n} " + "{};".format(self.nameStr)
        return output

    def build_python_struct_str(self):
        if self.isEnum:
            python_enum_str(self.nameStr)
        else:
            return python_class_str(self.nameStr, self.fields)

    def __str__(self):
        output = "VarType:"
        output += "\n  nameStr: {}".format(self.nameStr)
        output += "\n  size: {}".format(self.size)
        output += "\n  fields:"
        for field in self.fields:
            output += "\n    {} {}".format(field.varType.nameStr, field.name)
        output += "\n"
        return output

class Packet:
    def __init__(self, name, size, packetType, fields = None):
        self.name = name
        self.size = size
        self.packetType = packetType
        self.fields = fields

    def build_struct_str(self):
        output = "typedef struct {"
        for field in self.fields:
            output += "\n  {} {};".format(field.varType.nameStr, field.name)
        output += "\n} " + "{};".format(self.name)
        return output

    def build_python_struct_str(self):
        return python_class_str(self.name, self.fields, True)

    def __str__(self):
        output = "Packet:"
        output += "\n  name: {}".format(self.name)
        output += "\n  size: {}".format(self.size)
        output += "\n  type: {}".format(self.packetType)
        output += "\n  fields:"
        for field in self.fields:
            output += "\n    {} {}".format(field.varType.nameStr, field.name)
        output += "\n"
        return output

packetTypesDict = {}
packetsDict = {}
typesDict = {
    "u8": VarType("uint8_t", 1, True),
    "i8": VarType("int8_t", 1, True),
    "u16": VarType("uint16_t", 2, True),
    "i16": VarType("int16_t", 2, True),
    "u32": VarType("uint32_t", 4, True),
    "i32": VarType("int32_t", 4, True),
    "f32": VarType("float", 4, True),
    "u64": VarType("uint64_t", 8, True)
}

def build_types(types):
    output = ""

    for t in types:
        try:
            if t["primitive"]:
                pass
        except KeyError:
            fields = []
            size = 0

            for field in t["fields"]:
                varType = typesDict[field["type"]]
                fields += [Field(varType, field["name"])]
                size += varType.size

            newTypeName = "ft_{}_t".format(convert_camel_case_to_lower_underscore(t["name"]))
            newType = VarType(newTypeName, size, False, False, fields)
            typesDict[t["name"]] = newType
            output += newType.build_struct_str() + "\n\n"

    return output


def build_enums(enums):
    output = ""

    for e in enums:
        output += "typedef enum {"
        values = []
        size = e["size"]

        enumFields = []
        for value in e["values"]:
            output += "\n  FE_{} = {},".format(value["name"], value["value"])
            enumFields += [EnumField("FE_{}".format(value["name"]), value["value"])]

        # remove last comma in emum
        output = output[0:-1]
        newEnum = EnumClass(enumFields)


        newTypeName = "fe_{}_t".format(convert_camel_case_to_lower_underscore(e["name"]))
        newType = VarType(newTypeName, size, False, True)

        enumDict[newTypeName] = newEnum

        output += "\n} " + "{};\n\n".format(newTypeName);

        try:
            print(typesDict[e["name"]])
            assert()
        except KeyError:
            typesDict[e["name"]] = newType


    return output

def get_packet_key(packet, packetType):
    return packet["name"] + packetType[0:1].upper() + packetType[1:]

def build_packet(packet, packetPrefix, packetType):
    fields = []
    size = 0

    for field in packet["fields"]:
        varType = typesDict[field["type"]]
        fields += [Field(varType, field["name"])]
        size += varType.size

    newPacketName = "{}_{}_t".format(packetPrefix, convert_camel_case_to_lower_underscore(packet["name"]))
    newPacket = Packet(newPacketName, size, packetType, fields)
    packetsDict[get_packet_key(packet, packetType)] = newPacket

    return newPacket.build_struct_str()

def build_packets(packets):
    output = ""

    for p in packets:
        if p["command"]:
            packetPrefix = "fpc"
            packetType = "command"
            output += build_packet(p, packetPrefix, packetType) + "\n\n"

        if p["query"]:
            packetPrefix = "fpq"
            packetType = "query"
            output += build_packet(p, packetPrefix, packetType) + "\n\n"

        if p["response"]:
            packetPrefix = "fpr"
            packetType = "response"
            output += build_packet(p, packetPrefix, packetType) + "\n\n"

    print("built {} packets".format(len(packetsDict)))
    #256th packet type is needed for cnt do not make below <=
    assert len(packetsDict) < 0xFF, "too many packets"

    return output

def build_packet_type_enum():
    output = "typedef enum {"
    count = 0

    for key in packetsDict:
        packet = packetsDict[key]
        packetTypeName = convert_camel_case_to_caps_underscore(key)
        packetTypeEnumName = "FPT_{}".format(packetTypeName)
        packetTypesDict[packet.name] = {"id": count, "typeName": packetTypeEnumName}
        output += "\n  {} = {},".format(packetTypeEnumName, count)
        count += 1

    output += "\n  FPT_CNT = {}".format(count)
    output += "\n} fp_type_t;\n\n"
    return output

def build_defines(defines):
    output = ""
    for d in defines:
        definesDict[d] = defines[d];
        output += "#define {} {}\n".format(d, defines[d])
    output += "\n"
    return output

def get_header_wrapper_start(name):
    return "#ifndef {}\n#define {}\n\n".format(name, name)

def get_header_wrapper_end(name):
    return "#endif /* {} */".format(name)

def get_packet_function_headers():
    output = ""
    output += "fp_type_t fp_get_packet_type(uint8_t packetID);\n"
    output += "uint8_t fp_get_packet_length(fp_type_t packetType);\n"
    output += "\n"
    return output

def write_packet_header_file(yaml_file):
    global PACKET_HEADER_SIZE
    PACKET_HEADER_SIZE = yaml_file["defines"]["PACKET_HEADER_SIZE"]
    enums = build_enums(yaml_file["enums"])
    types = build_types(yaml_file["types"])
    packets = build_packets(yaml_file["packets"])
    defines = build_defines(yaml_file["defines"])
    packetTypeEnum = build_packet_type_enum()

    with open("falcon_packet.h", "w") as outputFile:
        outputFile.write(get_header_wrapper_start("FALCON_PACKET_H"))
        outputFile.write("\n#include <stdint.h>\n\n")
        outputFile.write(defines)
        outputFile.write("/* Enums */\n")
        outputFile.write(packetTypeEnum)
        outputFile.write(enums)
        outputFile.write("/* Types */\n")
        outputFile.write(types)
        outputFile.write("/* Packets */\n")
        outputFile.write(packets)
        outputFile.write("\n/* Functions */\n")
        outputFile.write(get_packet_function_headers())
        outputFile.write(get_header_wrapper_end("FALCON_PACKET_H"))

def get_packet_functions():
    output = ""
    output += "fp_type_t fp_get_packet_type(uint8_t packetID)\n"
    output += "{\n"
    output += "  return (fp_type_t) packetID;\n"
    output += "}\n\n"
    output += "uint8_t fp_get_packet_length(fp_type_t packetType)\n"
    output += "{\n"
    output += "  return packet_size_lookup_table[packetType];\n"
    output += "}\n\n"
    return output

def get_packet_size_lookup():
    output = "static uint8_t packet_size_lookup_table[FPT_CNT] = {\n"

    for key in packetsDict:
        packet = packetsDict[key]
        output += "  [{}] = {},\n".format(packetTypesDict[packet.name]["typeName"], packet.size)

    output = output[0:-1]
    output += "\n};"
    return output

def write_packet_source_file():
    with open("falcon_packet.c", "w") as outputFile:
        outputFile.write("\n#include \"falcon_packet.h\"\n\n")
        outputFile.write(get_packet_size_lookup())
        outputFile.write("\n\n")
        outputFile.write(get_packet_functions())


def get_packet_encode_function_name(packet):
    return "{}_encode".format(packet.name[0:-2])

def get_packet_encode_function_header(packet):
    return "uint8_t {}(uint8_t *buffer, {} *packet)".format(get_packet_encode_function_name(packet), packet.name)

def get_packet_decode_function_name(packet):
    return "void {}_decode(uint8_t *buffer, {} *packet)".format(packet.name[0:-2], packet.name)

def get_type_encode_function_name(varType):
    return "encode_{}".format(varType.nameStr[:-2])

def get_type_encode_function_declaration(varType):
    function_prefix = "static inline uint8_t "
    function_params = "(uint8_t *buffer, {} *value)".format(varType.nameStr)
    return function_prefix + get_type_encode_function_name(varType) + function_params

def get_type_decode_function_name(varType):
    return "decode_{}".format(varType.nameStr[:-2])

def get_type_decode_function_declaration(varType):
    function_prefix = "static inline void "
    function_params = "(uint8_t *buffer, {} *data)".format(varType.nameStr)
    return function_prefix + get_type_decode_function_name(varType) + function_params

def get_enum_encode_function_name(varType):
    return get_type_encode_function_name(varType)

def get_enum_encode_function_declaration(varType):
    return get_type_encode_function_declaration(varType)

def get_enum_decode_function_name(varType):
    return get_type_decode_function_name(varType)

def get_enum_decode_function_declaration(varType):
    return get_type_decode_function_declaration(varType)

def get_encode_function_headers():
    output = ""
    output += "uint8_t fp_encode_packet(uint8_t *buffer, void *packet, fp_type_t packetType);\n"
    for key in packetsDict:
        output += "{};\n".format(get_packet_encode_function_header(packetsDict[key]))
    return output

def get_enum_encode_function(varType):
    output = ""
    output += get_enum_encode_function_declaration(varType) + "\n"
    output += "{\n"
    output += "  uint32_t tempValue = (uint32_t) *value;\n"
    output += "  for (uint32_t i = 0; i < {}; i++)".format(varType.size) + " {\n"
    output += "     buffer[i] = ((tempValue) >> (i * 8)) & 0xFF;\n"
    output += "  }\n"
    output += "  return {};\n".format(varType.size)
    output += "}\n"
    return output

def get_type_encode_function(varType):
    output = ""
    output += get_type_encode_function_declaration(varType) + "\n"
    output += "{\n"

    count = 0
    for field in varType.fields:
        output += "  {}(&buffer[{}], &value->{});\n".format(get_type_encode_function_name(field.varType), count, field.name)
        count += field.varType.size

    output += "  return {};\n".format(varType.size)
    output += "}\n"
    return output

def get_primitive_type_encode_functions():
    with open("primitive_encode_funcs.txt", 'r') as inputFile:
        return inputFile.read()

def get_packet_header_encode_function():
    output = ""
    output += "static uint8_t encode_header(uint8_t *buffer, fp_type_t packetType)\n"
    output += "{\n"
    output += "  buffer[0] = PACKET_DELIMITER1;\n"
    output += "  buffer[1] = PACKET_DELIMITER2;\n"
    output += "  buffer[2] = (uint8_t)packetType;\n"
    output += "  return PACKET_HEADER_SIZE;\n"
    output += "}\n"
    return output

def get_packet_encode_function(packet):
    output = ""
    output += "{}\n".format(get_packet_encode_function_header(packet))
    output += "{\n"
    output += "  uint8_t size = sizeof(*packet) + PACKET_HEADER_SIZE;\n"
    output += "  encode_header(buffer, {});\n".format(packetTypesDict[packet.name]["typeName"])
    count = PACKET_HEADER_SIZE
    for field in packet.fields:
        output += "  {}(&buffer[{}], &packet->{});\n".format(get_type_encode_function_name(field.varType), count, field.name)
        count += field.varType.size

    output += "  return size;\n"
    output += "}\n"
    return output

def get_generic_encode_function():
    output = "uint8_t fp_encode_packet(uint8_t *buffer, void *packet, fp_type_t packetType)\n{\n"
    output += "  switch(packetType) {\n"

    for key in packetsDict:
        packet = packetsDict[key]
        packetTypeName = packetTypesDict[packet.name]["typeName"]
        output += "    case {}:\n".format(packetTypeName)
        output += "      return {}(buffer, ({} *) packet);\n\n".format(get_packet_encode_function_name(packet), packet.name)


    output += "    default:\n"
    output += "       return 0;\n"
    output += "  }\n"
    output += "}"
    return output

def get_encode_functions():
    output = ""
    output += get_packet_header_encode_function()
    output += "\n"

    output += get_primitive_type_encode_functions()
    output += "\n"
    for key in typesDict:
        varType = typesDict[key]
        if varType.isPrimitive is False:
            if varType.isEnum:
                output += get_enum_encode_function(varType)
                output += "\n"
            else:
                output += get_type_encode_function(varType)
                output += "\n"

    for key in packetsDict:
        output += get_packet_encode_function(packetsDict[key])
        output += "\n"
    return output

def get_type_decode_function(varType):
    output = ""
    output += get_type_decode_function_declaration(varType) + "\n"
    output += "{\n"

    count = 0
    for field in varType.fields:
        output += "  {}(&buffer[{}], &data->{});\n".format(get_type_decode_function_name(field.varType), count, field.name)
        count += field.varType.size

    output += "}\n"
    return output

def get_enum_decode_function(varType):
    output = ""
    output += get_enum_decode_function_declaration(varType) + "\n"
    output += "{\n"
    output += "  uint32_t tempdata = 0;\n"
    output += "  for (uint32_t i = 0; i < {}; i++)".format(varType.size) + " {\n"
    output += "    tempdata |= buffer[i] << (i * 8);\n"
    output += "  }\n"
    output += "  *data = ({}) tempdata;\n".format(format(varType.nameStr))
    output += "}\n"
    return output

def get_primitive_type_decode_functions():
    with open("primitive_decode_funcs.txt", 'r') as inputFile:
        return inputFile.read()

def get_decode_function_headers():
    output = ""
    for key in packetsDict:
        output += "{};\n".format(get_packet_decode_function_name(packetsDict[key]))
    return output

def get_packet_decode_function(packet):
    output = ""
    output += "{}\n".format(get_packet_decode_function_name(packet))
    output += "{\n"
    count = 0
    for field in packet.fields:
        output += "  {}(&buffer[{}], &packet->{});\n".format(get_type_decode_function_name(field.varType), count, field.name)
        count += field.varType.size

    output += "}\n"
    return output

def get_decode_functions():
    output = ""
    output += get_primitive_type_decode_functions()
    output += "\n"

    for key in typesDict:
        varType = typesDict[key]
        if varType.isPrimitive is False:
            if varType.isEnum:
                output += get_enum_decode_function(varType)
                output += "\n"
            else:
                output += get_type_decode_function(varType)
                output += "\n"

    for key in packetsDict:
        output += get_packet_decode_function(packetsDict[key])
        output += "\n"
    return output

def write_decode_header_file():
    with open("fp_decode.h", "w") as outputFile:
        outputFile.write(get_header_wrapper_start("FP_DECODE_H"))
        outputFile.write("\n#include \"falcon_packet.h\"\n\n")
        outputFile.write(get_decode_function_headers())
        outputFile.write(get_header_wrapper_end("FP_DECODE_H"))

def write_decode_source_file():
    with open("fp_decode.c", "w") as outputFile:
        outputFile.write('#include "fp_decode.h"\n\n')
        outputFile.write("\n\n")
        outputFile.write(get_decode_functions())

def write_encode_header_file():
    with open("fp_encode.h", "w") as outputFile:
        outputFile.write(get_header_wrapper_start("FP_ENCODE_H"))
        outputFile.write("\n#include \"falcon_packet.h\"\n\n")
        outputFile.write(get_encode_function_headers())
        outputFile.write(get_header_wrapper_end("FP_ENCODE_H"))

def write_encode_source_file():
    with open("fp_encode.c", "w") as outputFile:
        outputFile.write('#include "fp_encode.h"\n\n')
        outputFile.write(get_encode_functions())
        outputFile.write(get_generic_encode_function())

def write_python_file():
    output = ""
    output += "import struct"
    output += "\nfrom enum import IntEnum\n\n"

    output += python_defines_str()
    output += build_python_packet_type_enum()
    output += python_get_packet_size_str()
    output += python_encode_header_str()


    for key in typesDict:
        varType = typesDict[key]
        if varType.isPrimitive is False:
                if varType.isEnum:
                    output += python_enum_str(varType.nameStr)
                else:
                    output += varType.build_python_struct_str()
                    output += "\n"

    for key in packetsDict:
        packet = packetsDict[key]
        output += packet.build_python_struct_str()
        output += "\n"

    with open("falcon_packet.py", "w") as outputFile:
        outputFile.write(output)

with open("fp.yaml", 'r') as stream:
    try:
        yaml_file = yaml.safe_load(stream)
        write_packet_header_file(yaml_file)
        write_packet_source_file()
        write_encode_header_file()
        write_encode_source_file()
        write_decode_header_file()
        write_decode_source_file()
        write_python_file()

    except yaml.YAMLError as exc:
        print(exc)



