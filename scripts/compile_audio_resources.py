'''
 * @author Alexandru ALEXANDRESCU
 * All rights reserved.
'''

# compile_sound_resources.py
import os, sys

def generate_resource_mapping(folder_path, output_hpp):
    files = [f for f in os.listdir(folder_path) if f.endswith(".wav")]
    
    with open(output_hpp, 'w') as out:
        out.write("#pragma once\n")
        out.write("#include <string>\n")
        out.write("#include <unordered_map>\n\n")
        
        # 1. Generate extern declarations for the linker symbols
        out.write("extern \"C\" {\n")

        for filename in files:
            symbol_base = f"_binary_{filename.replace('.', '_')}"
            out.write(f"    extern const unsigned char {symbol_base}_start[];\n")
            out.write(f"    extern const unsigned char {symbol_base}_end[];\n")
        out.write("}\n\n")
        
        # 2. Structure for the registry
        out.write("struct AudioResource\n")
        out.write("{\n")
        out.write("    const unsigned char* data;\n")
        out.write("    size_t size;\n")
        out.write("};\n\n")

        # 3. Create the lookup function
        out.write("inline const AudioResource* get_audio_resource(const std::string& name)\n")
        out.write("{\n")
        out.write("    static const std::unordered_map<std::string, AudioResource> registry = {\n")
        for filename in files:
            symbol_base = f"_binary_{filename.replace('.', '_')}"
            out.write(f"        {{\"{filename}\", {{{symbol_base}_start, (size_t)({symbol_base}_end - (size_t){symbol_base}_start)}}}},\n")
        out.write("    };\n\n")
        out.write("    auto it = registry.find(name);\n")
        out.write("    return (it != registry.end()) ? &it->second : nullptr;\n")
        out.write("}\n")

if __name__ == "__main__":
    generate_resource_mapping(sys.argv[1], sys.argv[2])